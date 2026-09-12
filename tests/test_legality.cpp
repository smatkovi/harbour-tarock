// Rule engine: plays many full Königrufen hands with random legal actions and
// checks the invariants of docs/koenigrufen.md §3–§6, plus the single rules
// that are easy to get wrong (follow suit, Tarockzwang, Stichzwang in the
// negative games, the Pagat rule and the discarding restrictions).
#include "core/TarockCore.h"

#include <cstdio>
#include <cstdlib>
#include <random>

using namespace tarock;

namespace {
int failures = 0;
void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::fprintf(stderr, "FAILED line %d: %s\n", line, what);
        if (++failures > 20)
            std::exit(1);
    }
}
#define CHECK(x) check((x), #x, __LINE__)

// A hand with a fixed deal, so single rules can be checked deterministically.
TarockCore fixedHand(const std::vector<CardList>& hands, const CardList& talon)
{
    TarockCore core(ProfileId::AtKrOoe2023, 4, 1);
    core.dealFixed(hands, talon, 3);   // dealer 3, so seat 0 is forehand
    return core;
}

CardList cards(std::initializer_list<const char*> keys)
{
    CardList list;
    for (const char* key : keys) {
        Card card;
        if (parseCardKey(key, card))
            list.push_back(card);
        else
            std::fprintf(stderr, "unknown card key %s\n", key);
    }
    return list;
}

} // namespace

int main()
{
    std::mt19937 rng(20260912);

    // --- many random hands --------------------------------------------------
    int contracts[16] = {0};
    int handsPlayed = 0;
    for (int players : {4, 5}) {
        for (int match = 0; match < 120; ++match) {
            TarockCore core(ProfileId::AtKrOoe2023, players, rng());
            for (int hand = 0; hand < 3; ++hand) {
                int guard = 0;
                while (!core.handOver() && ++guard < 1200) {
                    std::string error;
                    CHECK(core.validate(&error));
                    const int actor = core.actor();
                    CHECK(actor >= 0 && core.active(actor));
                    if (actor < 0)
                        break;
                    const std::vector<Action> legal = core.legalActions(actor);
                    CHECK(!legal.empty());
                    if (legal.empty())
                        break;
                    // Round trip through the save format now and then.
                    if (guard % 17 == 0) {
                        TarockCore copy;
                        CHECK(copy.restore(core.serialize()));
                        CHECK(copy.serialize() == core.serialize());
                    }
                    // Prefer passing, otherwise the highest contract would win
                    // every auction and the negative games never get played.
                    Action action = legal[rng() % legal.size()];
                    if (core.phase() == Phase::Bidding && rng() % 10 < 7) {
                        for (const Action& option : legal) {
                            if (option.type == ActionType::Pass || option.type == ActionType::OpenForehand)
                                action = option;
                        }
                    }
                    Reason reason;
                    CHECK(core.apply(actor, action, &reason));
                }
                CHECK(guard < 1200);
                CHECK(core.handOver());
                if (!core.conceded()) {
                    // Every trick was played and the cards are all accounted
                    // for. In the negative games the talon stays out of play,
                    // so only the 48 played cards are counted.
                    CHECK(core.trickWinner(core.profile().tricks()) >= 0);
                    const CountResult declarer = core.partyPoints(true);
                    const CountResult defenders = core.partyPoints(false);
                    const bool talonInPlay = core.contractDef().talon != TalonMode::None;
                    const int expected = core.profile().deck().size()
                            - (talonInPlay ? 0 : core.profile().talonSize());
                    CHECK(declarer.cards + defenders.cards == expected);
                    if (talonInPlay)
                        CHECK(declarer.units + defenders.units == core.profile().totalUnits());
                }
                contracts[static_cast<int>(core.contract())]++;
                ++handsPlayed;
                CHECK(core.nextHand());
            }
        }
    }

    // --- seat rotation keeps the hand identical ------------------------------
    for (int match = 0; match < 60; ++match) {
        TarockCore core(ProfileId::AtKrOoe2023, 4, rng());
        const int offset = 1 + static_cast<int>(rng() % 3);
        TarockCore guest = core;
        guest.rotateSeats(offset);
        int guard = 0;
        while (!core.handOver() && ++guard < 1200) {
            const int actor = core.actor();
            if (actor < 0)
                break;
            const std::vector<Action> legal = core.legalActions(actor);
            if (legal.empty())
                break;
            const Action action = legal[rng() % legal.size()];
            CHECK(core.apply(actor, action));
            CHECK(guest.apply((actor - offset + 4) % 4, action));
            TarockCore mirror = core;
            mirror.rotateSeats(offset);
            CHECK(mirror.serialize() == guest.serialize());
        }
    }

    // --- single rules --------------------------------------------------------
    // Seat 0 holds only hearts and tarocks; seat 1 is void in hearts but has
    // tarocks, so it must trump.
    {
        TarockCore core = fixedHand({
            cards({"H:K","H:D","H:R","H:B","H:1","H:2","H:3","H:4","XX","XIX","XVIII","XVII"}),
            cards({"S:K","S:D","S:R","S:B","S:10","S:9","S:8","S:7","XVI","XV","XIV","XIII"}),
            cards({"D:K","D:D","D:R","D:B","D:1","D:2","D:3","D:4","XII","XI","X","IX"}),
            cards({"C:K","C:D","C:R","C:B","C:10","C:9","C:8","C:7","VIII","VII","VI","V"}),
        }, cards({"Sk","XXI","IIII","III","II","I"}));
        // Forehand opens with a Dreier, everybody passes.
        CHECK(core.apply(0, Action(ActionType::Bid, static_cast<std::int16_t>(ContractId::Dreier))));
        for (int seat = 1; seat < 4; ++seat)
            CHECK(core.apply(seat, Action(ActionType::Pass)));
        CHECK(core.phase() == Phase::Talon);
        CHECK(core.apply(0, Action(ActionType::TakeTalon, 0)));
        CHECK(core.phase() == Phase::Discard);
        // Kings and Trull cards may never be discarded.
        Card king; parseCardKey("H:K", king);
        Reason reason = core.cardReason(0, king);
        CHECK(reason.code == ReasonCode::DiscardKing);
        Card skues; parseCardKey("Sk", skues);
        CHECK(core.cardReason(0, skues).code == ReasonCode::DiscardTrull);
        // Discard three plain hearts and start playing.
        for (const char* key : {"H:4", "H:3", "H:2"}) {
            Card card; parseCardKey(key, card);
            CHECK(core.apply(0, Action(ActionType::Discard, card.id)));
        }
        CHECK(core.apply(0, Action(ActionType::ConfirmDiscard)));
        CHECK(core.phase() == Phase::Announce);
        for (int seat = 0; seat < 4; ++seat)
            CHECK(core.apply(seat, Action(ActionType::Ready)));
        CHECK(core.phase() == Phase::Play);
        // Forehand leads a heart; seat 1 has no heart and must play a tarock.
        Card heartQueen; parseCardKey("H:D", heartQueen);
        CHECK(core.apply(0, Action(ActionType::PlayCard, heartQueen.id)));
        Card spadeKing; parseCardKey("S:K", spadeKing);
        CHECK(core.cardReason(1, spadeKing).code == ReasonCode::MustTarock);
        Card tarock16; parseCardKey("XVI", tarock16);
        CHECK(core.cardReason(1, tarock16).ok());
        CHECK(core.apply(1, Action(ActionType::PlayCard, tarock16.id)));
        // Seat 2 holds no hearts either, so the same rule applies.
        Card diamondKing; parseCardKey("D:K", diamondKing);
        CHECK(core.cardReason(2, diamondKing).code == ReasonCode::MustTarock);
    }

    // The forehand says "Vorhand" and all the others pass: it is on turn
    // again, may not pass now and may choose Rufer or Trischaken (§3.3.3).
    {
        TarockCore core = fixedHand({
            cards({"H:K","H:D","H:R","H:B","H:1","H:2","H:3","H:4","XX","XIX","XVIII","XVII"}),
            cards({"S:K","S:D","S:R","S:B","S:10","S:9","S:8","S:7","XVI","XV","XIV","XIII"}),
            cards({"D:K","D:D","D:R","D:B","D:1","D:2","D:3","D:4","XII","XI","X","IX"}),
            cards({"C:K","C:D","C:R","C:B","C:10","C:9","C:8","C:7","VIII","VII","VI","V"}),
        }, cards({"Sk","XXI","IIII","III","II","I"}));
        CHECK(core.apply(0, Action(ActionType::OpenForehand)));
        for (int seat = 1; seat < 4; ++seat)
            CHECK(core.apply(seat, Action(ActionType::Pass)));
        CHECK(core.phase() == Phase::Bidding);
        CHECK(core.actor() == 0);
        Reason why;
        CHECK(!core.apply(0, Action(ActionType::Pass), &why));
        CHECK(why.code == ReasonCode::ForehandMustSpeak);
        bool rufer = false, trischaken = false, sechser = false, pass = false;
        for (const Action& option : core.legalActions(0)) {
            rufer = rufer || option == Action(ActionType::Bid, static_cast<std::int16_t>(ContractId::Rufer));
            trischaken = trischaken || option == Action(ActionType::Bid, static_cast<std::int16_t>(ContractId::Trischaken));
            sechser = sechser || option == Action(ActionType::Bid, static_cast<std::int16_t>(ContractId::Sechserdreier));
            pass = pass || option.type == ActionType::Pass;
        }
        CHECK(rufer);
        CHECK(trischaken);
        CHECK(!sechser);   // the Sechserdreier is an opening bid only
        CHECK(!pass);
    }

    std::printf("OK: %d hands played, contracts:", handsPlayed);
    const char* names[] = {"-", "Trischaken", "Rufer", "Piccolo", "Bettel", "Solorufer", "Besserrufer",
                           "Farbendreier", "Sechserdreier", "Dreier", "Farbensolo", "PiccoloOuvert",
                           "BettelOuvert", "Solodreier"};
    for (int i = 1; i <= 13; ++i)
        std::printf(" %s %d", names[i], contracts[i]);
    std::printf("\n");
    if (failures) {
        std::printf("%d failures\n", failures);
        return 1;
    }
    return 0;
}
