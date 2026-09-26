// Strohmandeln zu zweit (docs/strohmandeln.md), Regelprofil AT-STROH-MS-ERW.
//
// Geprüft wird, was dieses Spiel von allen anderen unterscheidet:
//
//   * zwei Sitze, je 15 Handkarten, kein Talon, dafür je drei Päckchen zu
//     vier Karten (§2.4),
//   * statt eines Lizits eine einzige Erklärung: nimmt die Vorhand auf, ist
//     das Bieten sofort aus; sagen beide "Weiter", wird einfach gespielt (§3.2),
//   * die Erstaufdeckung: Tarock und Könige wandern ins Blatt, die erste
//     kleinere Farbkarte bleibt als Deckblatt liegen, die letzte Karte eines
//     Päckchens geht verdeckt auf die Hand (§4.2, §4.5),
//   * das Deckblatt wird wie eine Handkarte gespielt (§4.4) und erst
//     nachgedeckt, wenn der Stich umgelegt ist (§4.3),
//   * 27 Stiche, und die Abrechnung geht auf null auf (§7.1).
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

bool canPass(const TarockCore& core, int seat)
{
    for (const Action& action : core.legalActions(seat)) {
        if (action.type == ActionType::Pass)
            return true;
    }
    return false;
}

// Die Bedingungen aus §4.1, auf jedem Päckchen nachgerechnet.
void checkStrawmen(const TarockCore& core, int line)
{
    const RuleProfile& profile = core.profile();
    for (int seat = 0; seat < core.players(); ++seat) {
        for (int packet = 0; packet < profile.strawmen(); ++packet) {
            const Card top = core.strawmanTop(seat, packet);
            const int rest = core.strawmanSize(seat, packet);
            if (top.valid()) {
                // Ein Deckblatt ist nie ein Tarock und nie ein König, und
                // unter ihm liegt immer noch mindestens eine Karte.
                check(!top.tarock(), "das Deckblatt ist ein Tarock", line);
                check(!top.king(), "das Deckblatt ist ein König", line);
                check(rest >= 1, "unter dem Deckblatt liegt nichts", line);
                // Und es ist zugleich eine Handkarte: gespielt wird es wie eine.
                check(contains(core.hand(seat), top), "das Deckblatt fehlt im Blatt", line);
            } else {
                // Ohne Deckblatt ist das Päckchen leer -- eine einzelne Karte
                // wäre längst verdeckt auf die Hand gegangen (§4.5).
                check(rest == 0, "ein Päckchen liegt ohne Deckblatt da", line);
            }
        }
    }
}
#define CHECK_STRAWMEN(core) checkStrawmen((core), __LINE__)

// Spielt eine ganze Hand mit zufälligen erlaubten Zügen zu Ende.
bool playOut(TarockCore& core, std::mt19937& rng)
{
    for (int step = 0; step < 4000 && core.phase() != Phase::HandOver
                       && core.phase() != Phase::Scoring; ++step) {
        const int seat = core.actor();
        if (seat < 0)
            return false;
        std::vector<Action> options = core.legalActions(seat);
        if (options.empty())
            return false;
        if (!core.apply(seat, options[rng() % options.size()]))
            return false;
    }
    return core.phase() == Phase::HandOver || core.phase() == Phase::Scoring;
}

} // namespace

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    std::setvbuf(stderr, nullptr, _IONBF, 0);
    const RuleProfile& profile = RuleProfile::get(ProfileId::AtStrohMsErw);

    // --- das Blatt und der Tisch ------------------------------------------
    CHECK(profile.seatCounts().size() == 1 && profile.seatCounts().front() == 2);
    CHECK(profile.playableWith(2) && !profile.playableWith(3) && !profile.playableWith(4));
    CHECK(profile.deck().size() == 54);
    CHECK(profile.handCards() == 15);
    CHECK(profile.tricks() == 27);
    CHECK(profile.strawmen() == 3 && profile.strawmanSize() == 4);
    CHECK(profile.winThreshold() == 107 && profile.totalUnits() == 210);
    CHECK(!profile.forehandMustBid());
    CHECK(!profile.announcements());

    // --- das Geben ---------------------------------------------------------
    {
        TarockCore core(ProfileId::AtStrohMsErw, 2, 12345);
        CHECK(core.players() == 2);
        CHECK(core.activePlayers() == 2);
        CHECK(core.sittingOut() == -1);
        int total = 0;
        for (int seat = 0; seat < 2; ++seat) {
            CHECK(static_cast<int>(core.hand(seat).count()) == 15);
            total += static_cast<int>(core.hand(seat).count());
            for (int packet = 0; packet < 3; ++packet) {
                CHECK(core.strawmanSize(seat, packet) == 4);
                // Vor der Erklärung liegt nichts offen (§2.4).
                CHECK(!core.strawmanTop(seat, packet).valid());
                total += core.strawmanSize(seat, packet);
            }
        }
        CHECK(total == 54);
        // Einen Talon gibt es nicht.
        CHECK(core.talonHalfSize(0) == 0 && core.talonHalfSize(1) == 0);
        CHECK(core.phase() == Phase::Bidding);
        CHECK(core.validate());
        // Wer das Profil für zwei nimmt, aber vier bestellt, bekommt zwei.
        TarockCore wrong(ProfileId::AtStrohMsErw, 4, 7);
        CHECK(wrong.players() == 2);
    }

    // --- die Erklärung statt eines Lizits ----------------------------------
    {
        TarockCore core(ProfileId::AtStrohMsErw, 2, 99);
        const int forehand = core.actor();
        CHECK(forehand == core.forehand());
        CHECK(canBid(core, forehand, ContractId::StrohAufgenommen));
        CHECK(canPass(core, forehand));
        // Das einfache Spiel sagt niemand an.
        CHECK(!canBid(core, forehand, ContractId::StrohEinfach));
        // "Ich nehme auf!" -- und das Bieten ist sofort aus (§3.2), der
        // Gegner wird nicht mehr gefragt.
        CHECK(core.apply(forehand, Action(ActionType::Bid,
                                          static_cast<std::int16_t>(ContractId::StrohAufgenommen))));
        CHECK(core.phase() != Phase::Bidding);
        CHECK(core.contract() == ContractId::StrohAufgenommen);
        CHECK(core.declarer() == forehand);
        // Es wird weder gedrückt noch angesagt: es geht gleich los.
        CHECK(core.phase() == Phase::Play);
        CHECK(core.trickNumber() == 1);
    }
    {
        // Passt die Vorhand, ist der Geber an der Reihe.
        TarockCore core(ProfileId::AtStrohMsErw, 2, 100);
        const int forehand = core.actor();
        CHECK(core.apply(forehand, Action(ActionType::Pass)));
        CHECK(core.phase() == Phase::Bidding);
        const int dealer = core.actor();
        CHECK(dealer != forehand);
        CHECK(canBid(core, dealer, ContractId::StrohAufgenommen));
        CHECK(core.apply(dealer, Action(ActionType::Bid,
                                        static_cast<std::int16_t>(ContractId::StrohAufgenommen))));
        CHECK(core.contract() == ContractId::StrohAufgenommen);
        CHECK(core.declarer() == dealer);
    }
    {
        // Sagen beide "Weiter", wird das einfache Spiel gespielt (§3.2).
        TarockCore core(ProfileId::AtStrohMsErw, 2, 4242);
        const int forehand = core.forehand();
        for (int i = 0; i < 2; ++i)
            CHECK(core.apply(core.actor(), Action(ActionType::Pass)));
        CHECK(core.contract() == ContractId::StrohEinfach);
        CHECK(core.declarer() == forehand);
        CHECK(core.phase() == Phase::Play);
    }

    // --- die Erstaufdeckung ------------------------------------------------
    {
        std::mt19937 rng(20260926);
        int deckblaetter = 0;
        int aufgenommen = 0;
        for (int round = 0; round < 200; ++round) {
            TarockCore core(ProfileId::AtStrohMsErw, 2, rng());
            CHECK(core.apply(core.actor(), Action(ActionType::Bid,
                                                  static_cast<std::int16_t>(ContractId::StrohAufgenommen))));
            CHECK_STRAWMEN(core);
            CHECK(core.validate());
            int cards = 0;
            for (int seat = 0; seat < 2; ++seat) {
                cards += static_cast<int>(core.hand(seat).count());
                for (int packet = 0; packet < 3; ++packet) {
                    if (core.strawmanTop(seat, packet).valid())
                        ++deckblaetter;
                    // Das Deckblatt steht schon im Blatt und zählt dort; hier
                    // kommen nur die verdeckten Karten darunter dazu.
                    cards += core.strawmanSize(seat, packet);
                }
                aufgenommen += static_cast<int>(core.hand(seat).count()) - 15;
            }
            CHECK(cards == 54);
            if (failures)
                break;
        }
        std::printf("200 Erstaufdeckungen: %d Deckblätter, %d Karten ins Blatt genommen\n",
                    deckblaetter, aufgenommen);
        // Ein Päckchen ohne jedes Deckblatt wäre ein Zeichen dafür, dass die
        // Schleife nie anhält; eines, das nie aufnimmt, dafür, dass sie nie läuft.
        CHECK(deckblaetter > 0 && aufgenommen > 0);
    }

    // --- das Deckblatt wird gespielt und erst danach nachgedeckt -----------
    {
        std::mt19937 rng(7);
        int gespielt = 0;
        for (int round = 0; round < 60 && gespielt < 5; ++round) {
            TarockCore core(ProfileId::AtStrohMsErw, 2, 3000 + round);
            if (!core.apply(core.actor(), Action(ActionType::Bid,
                                                 static_cast<std::int16_t>(ContractId::StrohAufgenommen))))
                continue;
            for (int step = 0; step < 200 && core.phase() == Phase::Play; ++step) {
                const int seat = core.actor();
                const CardSet playable = core.playableCards(seat);
                // Ein Deckblatt suchen, das gerade gelegt werden darf.
                int packet = -1;
                for (int p = 0; p < 3; ++p) {
                    const Card top = core.strawmanTop(seat, p);
                    if (top.valid() && contains(playable, top))
                        packet = p;
                }
                if (packet < 0 || core.trick().empty()) {
                    // Nur im ersten Zug eines Stiches prüfen, damit der Stich
                    // danach wirklich noch offen ist.
                    if (packet >= 0 && core.trick().empty()) {
                        // weiter unten
                    } else {
                        std::vector<Action> options = core.legalActions(seat);
                        if (options.empty())
                            break;
                        if (!core.apply(seat, options[rng() % options.size()]))
                            break;
                        continue;
                    }
                }
                if (packet < 0)
                    continue;
                const Card top = core.strawmanTop(seat, packet);
                const int before = core.strawmanSize(seat, packet);
                CHECK(core.apply(seat, Action(ActionType::PlayCard, top.id)));
                // Der Stich läuft noch: das Päckchen liegt offen da, es wurde
                // noch nichts nachgedeckt (§4.3, Regel 1).
                CHECK(!core.strawmanTop(seat, packet).valid());
                CHECK(core.strawmanSize(seat, packet) == before);
                CHECK(!core.trick().empty());
                // Der Gegner antwortet, der Stich wird umgelegt.
                const int other = core.actor();
                std::vector<Action> options = core.legalActions(other);
                CHECK(!options.empty());
                if (options.empty())
                    break;
                CHECK(core.apply(other, options[rng() % options.size()]));
                CHECK(core.trick().empty());
                // Jetzt ist nachgedeckt: entweder liegt ein neues Deckblatt,
                // oder das Päckchen ist verbraucht.
                CHECK(core.strawmanSize(seat, packet) < before
                      || core.strawmanTop(seat, packet).valid());
                CHECK_STRAWMEN(core);
                ++gespielt;
                break;
            }
        }
        CHECK(gespielt == 5);
    }

    // --- ganze Hände -------------------------------------------------------
    {
        std::mt19937 rng(515);
        int played = 0;
        int einfach = 0, aufgenommen = 0, unentschieden = 0;
        for (int round = 0; round < 40; ++round) {
            TarockCore core(ProfileId::AtStrohMsErw, 2, rng());
            if (round % 4 == 3) {
                for (int i = 0; i < 2; ++i)
                    CHECK(core.apply(core.actor(), Action(ActionType::Pass)));
            } else {
                CHECK(core.apply(core.actor(),
                                 Action(ActionType::Bid,
                                        static_cast<std::int16_t>(ContractId::StrohAufgenommen))));
            }
            CHECK(playOut(core, rng));
            if (core.phase() != Phase::HandOver && core.phase() != Phase::Scoring)
                break;
            const HandResult result = core.handResult();
            const Ledger ledger = settle(result);
            CHECK(ledger.zeroSum());
            // Alle 27 Stiche sind gespielt, und alle 54 Karten liegen darin.
            CHECK(static_cast<int>(result.tricks.size()) == 27);
            CardSet inTricks;
            for (const TrickResult& trick : result.tricks) {
                CHECK((inTricks & trick.cards).none());
                inTricks |= trick.cards;
            }
            CHECK(inTricks.count() == 54);
            // Und die Kartenpunkte gehen auf 210 Drittel auf (§7.2, Schritt 2).
            CHECK(profile.count(result.declarerCards).units
                  + profile.count(result.defenderCards).units == 210);
            // Kein Päckchen ist übrig geblieben.
            for (int seat = 0; seat < 2; ++seat) {
                for (int packet = 0; packet < 3; ++packet)
                    CHECK(core.strawmanSize(seat, packet) == 0);
            }
            if (core.contract() == ContractId::StrohEinfach) {
                ++einfach;
                if (ledger.schrift[0] == 0 && ledger.schrift[1] == 0)
                    ++unentschieden;
            } else {
                ++aufgenommen;
            }
            ++played;
        }
        std::printf("%d Hände zu zweit: %d einfach (davon %d ohne Schreiber), %d aufgenommen\n",
                    played, einfach, unentschieden, aufgenommen);
        CHECK(played == 40);
        CHECK(einfach > 0 && aufgenommen > 0);
    }

    // --- Speichern und Laden ----------------------------------------------
    {
        TarockCore core(ProfileId::AtStrohMsErw, 2, 8080);
        CHECK(core.apply(core.actor(), Action(ActionType::Bid,
                                              static_cast<std::int16_t>(ContractId::StrohAufgenommen))));
        std::mt19937 rng(3);
        for (int i = 0; i < 9; ++i) {
            const int seat = core.actor();
            std::vector<Action> options = core.legalActions(seat);
            if (options.empty())
                break;
            core.apply(seat, options[rng() % options.size()]);
        }
        TarockCore restored;
        CHECK(restored.restore(core.serialize()));
        CHECK(restored.players() == 2);
        CHECK(restored.trickNumber() == core.trickNumber());
        for (int seat = 0; seat < 2; ++seat) {
            CHECK(restored.hand(seat) == core.hand(seat));
            for (int packet = 0; packet < 3; ++packet) {
                CHECK(restored.strawmanTop(seat, packet) == core.strawmanTop(seat, packet));
                CHECK(restored.strawmanSize(seat, packet) == core.strawmanSize(seat, packet));
            }
        }
        // Und der Blick eines Gastes: das fremde Deckblatt bleibt sichtbar,
        // was darunter liegt, wird zur bloßen Zahl (§4.4, design.md §8.3).
        TarockCore guest;
        CHECK(guest.restore(core.serializeFor(0)));
        CHECK(guest.handSize(1) == core.handSize(1));
        CHECK(guest.hand(1).count() < core.hand(1).count()
              || core.strawmanTops(1).count() == core.hand(1).count());
        for (int packet = 0; packet < 3; ++packet) {
            CHECK(guest.strawmanTop(1, packet) == core.strawmanTop(1, packet));
            CHECK(guest.strawmanSize(1, packet) == core.strawmanSize(1, packet));
            CHECK(guest.strawman(1, packet).empty());
            // Die eigenen Päckchen sieht der Gast weiter ganz.
            CHECK(guest.strawman(0, packet).size() == core.strawman(0, packet).size());
        }
    }

    // --- die Computerspieler ----------------------------------------------
    //
    // Zu zweit muss der Computer erklären und spielen, ohne hängenzubleiben --
    // und er muss gelegentlich aufnehmen, sonst ist jede Hand ein einfaches
    // Spiel und die Aufnahme-Heuristik aus §10.2 fehlt einfach.
    {
        AiPlayer ai(Difficulty::Club);
        int hands = 0, byAi = 0, taken = 0;
        for (int round = 0; round < 12; ++round) {
            TarockCore core(ProfileId::AtStrohMsErw, 2, 900 + round);
            Inference view[2];
            for (int steps = 0; steps < 4000 && core.phase() != Phase::HandOver
                                && core.phase() != Phase::Scoring; ++steps) {
                const int seat = core.actor();
                if (seat < 0)
                    break;
                for (int s = 0; s < 2; ++s)
                    view[s].observe(core, s);
                const Action action = ai.bestAction(core, seat, view[seat], 20);
                CHECK(action.type != ActionType::None);
                if (action.type == ActionType::None)
                    break;
                if (!core.apply(seat, action)) {
                    std::printf("der Computer wollte etwas Unerlaubtes (Zug %d)\n", steps);
                    CHECK(false);
                    break;
                }
                ++byAi;
            }
            CHECK(core.phase() == Phase::HandOver || core.phase() == Phase::Scoring);
            if (core.phase() != Phase::HandOver && core.phase() != Phase::Scoring)
                break;
            CHECK(settle(core.handResult()).zeroSum());
            if (core.contract() == ContractId::StrohAufgenommen)
                ++taken;
            ++hands;
        }
        std::printf("%d Hände ganz von Computerspielern, %d Züge, %d mal aufgenommen\n",
                    hands, byAi, taken);
        CHECK(hands == 12);
        CHECK(taken > 0);
    }

    if (failures == 0)
        std::printf("test_profile_stroh: alles in Ordnung\n");
    else
        std::printf("test_profile_stroh: %d Fehler\n", failures);
    return failures == 0 ? 0 : 1;
}
