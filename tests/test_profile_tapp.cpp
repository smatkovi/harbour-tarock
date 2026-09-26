// Tapp-Tarock zu dritt (docs/tapptarock.md), Regelprofil AT-TAPP-KLASSIK.
//
// Geprüft wird, was das Dreierspiel vom Königrufen unterscheidet und was der
// Kern dafür können musste:
//
//   * drei Sitze, je 16 Karten, sechs im Talon in zwei Hälften zu drei (§2.4),
//   * die Leiter Dreier < Unterer < Oberer < Solo (§3.1),
//   * Unterer und Oberer nehmen die festgelegte Hälfte, der Dreier wählt,
//     das Solo lässt den Talon liegen (§4.1, §4.2),
//   * die Vorhand darf "Weiter" sagen; passen alle, wird trischakt (§3.5),
//   * der Alleinspieler zahlt an jeden Gegner bzw. kassiert von jedem (§7.1),
//   * und eine ganze Hand läuft mit zufälligen erlaubten Zügen durch.
#include "core/TarockCore.h"
#include "core/RuleProfile.h"
#include "core/Scoring.h"
#include "core/Ai.h"

#include <cstdio>
#include <cstdlib>
#include <random>

using namespace tarock;

namespace {
int failures = 0;
void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::fprintf(stderr, "FEHLT Zeile %d: %s\n", line, what);
        if (++failures > 20)
            std::exit(1);
    }
}
#define CHECK(x) check((x), #x, __LINE__)

bool canBid(const TarockCore& core, int seat, ContractId id)
{
    for (const Action& action : core.legalActions(seat)) {
        if (action.type == ActionType::Bid && action.a == static_cast<int>(id))
            return true;
    }
    return false;
}

bool canTakeHalf(const TarockCore& core, int seat, int half)
{
    for (const Action& action : core.legalActions(seat)) {
        if (action.type == ActionType::TakeTalon && action.a == half)
            return true;
    }
    return false;
}

// Bietet das genannte Spiel an und lässt die anderen passen.
bool bidAndPass(TarockCore& core, ContractId id)
{
    const int bidder = core.actor();
    if (!core.apply(bidder, Action(ActionType::Bid, static_cast<std::int16_t>(id))))
        return false;
    for (int guard = 0; guard < 8 && core.phase() == Phase::Bidding; ++guard) {
        const int seat = core.actor();
        if (seat == bidder)
            break;
        if (!core.apply(seat, Action(ActionType::Pass)))
            return false;
    }
    return true;
}

} // namespace

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);
    const RuleProfile& profile = RuleProfile::get(ProfileId::AtTappKlassik);

    // --- das Blatt und der Tisch ------------------------------------------
    CHECK(profile.seatCounts().size() == 1 && profile.seatCounts().front() == 3);
    CHECK(profile.playableWith(3) && !profile.playableWith(4) && !profile.playableWith(5));
    CHECK(profile.deck().size() == 54);
    CHECK(profile.handCards() == 16);
    CHECK(profile.tricks() == 16);
    CHECK(profile.talonSize() == 6);
    CHECK(profile.winThreshold() == 107 && profile.totalUnits() == 210);
    CHECK(!profile.forehandMustBid());

    {
        TarockCore core(ProfileId::AtTappKlassik, 3, 12345);
        CHECK(core.players() == 3);
        CHECK(core.activePlayers() == 3);
        CHECK(core.sittingOut() == -1);
        int onHands = 0;
        for (int seat = 0; seat < 3; ++seat) {
            CHECK(static_cast<int>(core.hand(seat).count()) == 16);
            onHands += static_cast<int>(core.hand(seat).count());
        }
        CHECK(onHands == 48);
        CHECK(core.talonHalfSize(0) == 3 && core.talonHalfSize(1) == 3);
        CHECK(core.phase() == Phase::Bidding);
    }

    // Wer drei Spieler bestellt, aber ein Profil für vier nimmt, bekommt vier
    // -- der Kern richtet sich nach dem Profil.
    {
        TarockCore core(ProfileId::AtKrOoe2023, 3, 7);
        CHECK(core.players() == 4);
    }

    // --- die Leiter --------------------------------------------------------
    {
        TarockCore core(ProfileId::AtTappKlassik, 3, 99);
        const int forehand = core.actor();
        CHECK(canBid(core, forehand, ContractId::TappDreier));
        CHECK(canBid(core, forehand, ContractId::TappUnterer));
        CHECK(canBid(core, forehand, ContractId::TappOberer));
        CHECK(canBid(core, forehand, ContractId::TappSolo));
        // Das Trischaken sagt niemand an; es wird das Spiel, wenn alle passen.
        CHECK(!canBid(core, forehand, ContractId::Trischaken));
        // Die Vorhand darf passen -- im Königrufen darf sie das nicht.
        bool mayPass = false;
        for (const Action& action : core.legalActions(forehand))
            mayPass = mayPass || action.type == ActionType::Pass;
        CHECK(mayPass);

        CHECK(core.apply(forehand, Action(ActionType::Bid,
                                        static_cast<std::int16_t>(ContractId::TappDreier))));
        // Überboten wird nur nach oben.
        const int next = core.actor();
        CHECK(!canBid(core, next, ContractId::TappDreier));
        CHECK(canBid(core, next, ContractId::TappUnterer));
        CHECK(canBid(core, next, ContractId::TappSolo));
    }

    // --- alle passen: Trischaken ------------------------------------------
    {
        TarockCore core(ProfileId::AtTappKlassik, 3, 4242);
        for (int i = 0; i < 3; ++i)
            CHECK(core.apply(core.actor(), Action(ActionType::Pass)));
        CHECK(core.contract() == ContractId::Trischaken);
        CHECK(core.phase() != Phase::Bidding);
    }

    // --- der Talon ---------------------------------------------------------
    {
        // Dreier: freie Wahl zwischen beiden Hälften.
        TarockCore core(ProfileId::AtTappKlassik, 3, 5150);
        CHECK(bidAndPass(core, ContractId::TappDreier));
        CHECK(core.phase() == Phase::Talon);
        const int declarer = core.declarer();
        CHECK(declarer >= 0);
        if (declarer < 0)
            return 1;
        CHECK(canTakeHalf(core, declarer, 0) && canTakeHalf(core, declarer, 1));
        CHECK(core.apply(declarer, Action(ActionType::TakeTalon, 1)));
        CHECK(static_cast<int>(core.hand(declarer).count()) == 19);
        CHECK(core.phase() == Phase::Discard);
    }
    {
        // Unterer: nur die untere Hälfte.
        TarockCore core(ProfileId::AtTappKlassik, 3, 616);
        CHECK(bidAndPass(core, ContractId::TappUnterer));
        const int declarer = core.declarer();
        CHECK(declarer >= 0);
        if (declarer >= 0)
            CHECK(canTakeHalf(core, declarer, 0) && !canTakeHalf(core, declarer, 1));
    }
    {
        // Oberer: nur die obere.
        TarockCore core(ProfileId::AtTappKlassik, 3, 617);
        CHECK(bidAndPass(core, ContractId::TappOberer));
        const int declarer = core.declarer();
        CHECK(declarer >= 0);
        if (declarer >= 0)
            CHECK(!canTakeHalf(core, declarer, 0) && canTakeHalf(core, declarer, 1));
    }
    {
        // Solo: der Talon bleibt liegen, es wird nichts gelegt.
        TarockCore core(ProfileId::AtTappKlassik, 3, 618);
        CHECK(bidAndPass(core, ContractId::TappSolo));
        CHECK(core.phase() != Phase::Talon && core.phase() != Phase::Discard);
        CHECK(core.declarer() >= 0);
        if (core.declarer() >= 0)
            CHECK(static_cast<int>(core.hand(core.declarer()).count()) == 16);
        CHECK(core.talonHalfSize(0) + core.talonHalfSize(1) == 6);
    }

    // --- ganze Hände mit zufälligen erlaubten Zügen -------------------------
    {
        std::mt19937 rng(20260926);
        // Zufälliges Bieten steigert immer bis zum Solo; damit jedes Spiel
        // der Leiter wirklich gespielt wird, wird es hier reihum angesagt --
        // und jede vierte Hand lässt alle passen, also trischaken.
        const ContractId ladder[4] = { ContractId::TappDreier, ContractId::TappUnterer,
                                       ContractId::TappOberer, ContractId::TappSolo };
        int played = 0;
        int seen[5] = {0, 0, 0, 0, 0};
        for (int round = 0; round < 40; ++round) {
            TarockCore core(ProfileId::AtTappKlassik, 3, rng());
            if (round % 5 == 4) {
                for (int i = 0; i < 3; ++i)
                    core.apply(core.actor(), Action(ActionType::Pass));
            } else {
                CHECK(bidAndPass(core, ladder[round % 4]));
            }
            for (int step = 0; step < 4000 && core.phase() != Phase::HandOver
                               && core.phase() != Phase::Scoring; ++step) {
                const int seat = core.actor();
                if (seat < 0)
                    break;
                std::vector<Action> options = core.legalActions(seat);
                if (options.empty())
                    break;
                const Action action = options[rng() % options.size()];
                if (!core.apply(seat, action))
                    break;
            }
            CHECK(core.phase() == Phase::HandOver || core.phase() == Phase::Scoring);
            if (core.phase() != Phase::HandOver && core.phase() != Phase::Scoring)
                break;
            // Alle 54 Karten sind am Ende verteilt: Stiche, Legage, liegen
            // gebliebener Talon.
            const HandResult result = core.handResult();
            const Ledger ledger = settle(result);
            CHECK(ledger.zeroSum());
            // Alle sechzehn Stiche sind gespielt.
            CHECK(static_cast<int>(result.tricks.size()) == 16);
            switch (core.contract()) {
            case ContractId::Trischaken:  ++seen[0]; break;
            case ContractId::TappDreier:  ++seen[1]; break;
            case ContractId::TappUnterer: ++seen[2]; break;
            case ContractId::TappOberer:  ++seen[3]; break;
            case ContractId::TappSolo:    ++seen[4]; break;
            default: CHECK(false); break;
            }
            ++played;
        }
        std::printf("%d Hände zu dritt: %d Trischaken, %d Dreier, %d Unterer, %d Oberer, %d Solo\n",
                    played, seen[0], seen[1], seen[2], seen[3], seen[4]);
        CHECK(played == 40);
        for (int i = 0; i < 5; ++i)
            CHECK(seen[i] > 0);
    }

    // --- Abrechnung: der Alleinspieler gegen zwei --------------------------
    {
        std::mt19937 rng(7);
        int checked = 0;
        for (int round = 0; round < 60 && checked < 5; ++round) {
            TarockCore core(ProfileId::AtTappKlassik, 3, rng());
            if (!bidAndPass(core, ContractId::TappDreier))
                continue;
            for (int step = 0; step < 4000 && core.phase() != Phase::HandOver
                               && core.phase() != Phase::Scoring; ++step) {
                const int seat = core.actor();
                if (seat < 0)
                    break;
                std::vector<Action> options = core.legalActions(seat);
                if (options.empty())
                    break;
                if (!core.apply(seat, options[rng() % options.size()]))
                    break;
            }
            if (core.phase() != Phase::HandOver && core.phase() != Phase::Scoring)
                continue;
            const HandResult result = core.handResult();
            const Ledger ledger = settle(result);
            CHECK(ledger.zeroSum());
            // Was der Alleinspieler bekommt, zahlen die beiden anderen je zur
            // Hälfte -- und umgekehrt (§7.1).
            const int declarer = result.declarer;
            int others = 0;
            for (int seat = 0; seat < 3; ++seat) {
                if (seat != declarer)
                    others += ledger.geldThirds[static_cast<std::size_t>(seat)];
            }
            CHECK(ledger.geldThirds[static_cast<std::size_t>(declarer)] == -others);
            ++checked;
        }
        CHECK(checked == 5);
    }

    // --- die Computerspieler --------------------------------------------
    //
    // Der Kern allein macht noch kein Spiel: zu dritt muss auch der Computer
    // bieten, den Talon nehmen, legen und ausspielen, ohne hängenzubleiben.
    {
        AiPlayer ai(Difficulty::Club);
        int hands = 0;
        int byAi = 0;
        for (int round = 0; round < 12; ++round) {
            TarockCore core(ProfileId::AtTappKlassik, 3, 900 + round);
            Inference view[3];
            int steps = 0;
            for (; steps < 4000 && core.phase() != Phase::HandOver
                   && core.phase() != Phase::Scoring; ++steps) {
                const int seat = core.actor();
                if (seat < 0)
                    break;
                for (int s = 0; s < 3; ++s)
                    view[s].observe(core, s);
                const Action action = ai.bestAction(core, seat, view[seat], 20);
                CHECK(action.type != ActionType::None);
                if (action.type == ActionType::None)
                    break;
                Reason why;
                if (!core.apply(seat, action, &why)) {
                    std::printf("der Computer wollte etwas Unerlaubtes (Zug %d)\n", steps);
                    CHECK(false);
                    break;
                }
                ++byAi;
            }
            CHECK(core.phase() == Phase::HandOver || core.phase() == Phase::Scoring);
            if (core.phase() != Phase::HandOver && core.phase() != Phase::Scoring)
                break;
            const Ledger ledger = settle(core.handResult());
            CHECK(ledger.zeroSum());
            ++hands;
        }
        std::printf("%d Hände ganz von Computerspielern, %d Züge\n", hands, byAi);
        CHECK(hands == 12);
    }

    if (failures == 0)
        std::printf("test_profile_tapp: alles in Ordnung\n");
    else
        std::printf("test_profile_tapp: %d Fehler\n", failures);
    return failures == 0 ? 0 : 1;
}
