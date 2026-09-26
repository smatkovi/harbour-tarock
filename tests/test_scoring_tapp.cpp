// Die Abrechnung des Tapp-Tarocks: die sieben durchgerechneten Beispiele aus
// docs/tapptarock.md §7.4, Zahl für Zahl.
//
// Die Nullsumme allein beweist nichts -- sie stimmt auch bei falschen Tarifen.
// Hier steht darum jeder Betrag: Dreier 3, Unterer 4, Oberer 5, Solo 8, je
// Gegenspieler; Pagat ultimo still 4 und angesagt 8, im Solo das Doppelte;
// Valat still viermal, angesagt achtmal den Spielwert; Kontra verdoppelt.
//
// Beispiel 2, 6 und 7 enthalten zusätzlich Handprämien (Köpfe, Trull,
// Königstrull). Die gibt es in der App noch nicht -- sie gehen nicht an die
// Partei, sondern von jedem an einen einzelnen Spieler (§5.3) und brauchen
// einen eigenen Posten. Die Prüfung rechnet diese drei Beispiele deshalb
// ohne den Handprämienteil und hält in einem Kommentar fest, was fehlt.
#include "core/Card.h"
#include "core/RuleProfile.h"
#include "core/Scoring.h"

#include <array>
#include <cstdio>

using namespace tarock;

namespace {

int failures = 0;

const RuleProfile& tapp()
{
    return RuleProfile::get(ProfileId::AtTappKlassik);
}

// Verteilt echte Karten, bis eine Partei genau so viele Drittel hat, wie das
// Beispiel verlangt (§1.5: jede Karte zählt 3 * Wert - 2 Drittel).
class Dealer {
public:
    Dealer() { m_free.set(); }

    void give(CardSet& target, Card card)
    {
        m_free.reset(card.id);
        target.set(card.id);
    }

    bool fill(CardSet& target, int thirds)
    {
        int need = thirds - tapp().count(target).units;
        while (need > 0) {
            Card best;
            int bestThirds = 0;
            for (int id = 0; id < 54; ++id) {
                const Card card(static_cast<std::uint8_t>(id));
                if (!m_free.test(static_cast<std::size_t>(id)))
                    continue;
                const int value = 3 * tapp().cardValue(card) - 2;
                if (value <= need && value > bestThirds) {
                    best = card;
                    bestThirds = value;
                }
            }
            if (!best.valid())
                return false;
            give(target, best);
            need -= bestThirds;
        }
        return need == 0;
    }

    CardSet rest() const { return m_free; }

private:
    CardSet m_free;
};

// Ein Dreiertisch: A ist Platz 0, B und C sind 1 und 2.
HandResult hand(ContractId contract)
{
    HandResult result;
    result.profile = ProfileId::AtTappKlassik;
    result.contract = contract;
    result.players = 3;
    result.declarer = 0;
    result.partner = -1;      // der Alleinspieler spielt allein
    result.forehand = 0;
    return result;
}

void setTricks(HandResult& result, int declarerTricks)
{
    result.tricks.clear();
    for (int i = 0; i < 16; ++i) {
        TrickResult trick;
        trick.winner = static_cast<std::int8_t>(i < declarerTricks ? 0 : 1 + (i % 2));
        result.tricks.push_back(trick);
    }
}

void wonWith(HandResult& result, int trickNumber, Card card, int winner)
{
    TrickResult& trick = result.tricks[static_cast<std::size_t>(trickNumber - 1)];
    trick.winningCard = card;
    trick.winner = static_cast<std::int8_t>(winner);
    trick.cards.set(card.id);
}

// Punkte des Beispiels in Drittel: 3 Drittel je Punkt.
void give(HandResult& result, Dealer& dealer, int declarerPoints)
{
    const int thirds = 3 * declarerPoints;
    if (!dealer.fill(result.declarerCards, thirds))
        std::fprintf(stderr, "FEHLT: %d Punkte lassen sich nicht auslegen\n", declarerPoints);
    result.defenderCards = dealer.rest();
}

void expect(int example, const Ledger& ledger, int a, int b, int c)
{
    const int got[3] = { ledger.geld[0], ledger.geld[1], ledger.geld[2] };
    const int want[3] = { a, b, c };
    for (int seat = 0; seat < 3; ++seat) {
        if (got[seat] != want[seat]) {
            std::fprintf(stderr, "FEHLT Beispiel %d: Platz %d bekommt %d, erwartet %d\n",
                         example, seat, got[seat], want[seat]);
            ++failures;
        }
    }
    if (!ledger.zeroSum()) {
        std::fprintf(stderr, "FEHLT Beispiel %d: die Abrechnung ist keine Nullsumme\n", example);
        ++failures;
    }
}

} // namespace

int main()
{
    std::setvbuf(stdout, nullptr, _IONBF, 0);

    // 1 -- einfacher Dreier, gewonnen mit 39 Punkten: Spiel 3 je Gegner.
    {
        HandResult result = hand(ContractId::TappDreier);
        setTricks(result, 9);
        Dealer dealer;
        give(result, dealer, 39);
        expect(1, settle(result), 6, -3, -3);
    }

    // 2 -- Dreier mit stillem Pagat ultimo: Spiel 3 + Pagat still 4 = 7.
    //      Das Beispiel rechnet zusätzlich 1 für die Köpfe (Handprämie, in
    //      der App noch nicht vorhanden); dort stünde je Gegner 8.
    {
        HandResult result = hand(ContractId::TappDreier);
        setTricks(result, 9);
        wonWith(result, 16, tarockCard(1), 0);
        Dealer dealer;
        dealer.give(result.declarerCards, tarockCard(1));
        give(result, dealer, 39);
        expect(2, settle(result), 14, -7, -7);
    }

    // 3 -- Solo mit angesagtem, verlorenem Pagat ultimo: Spiel +8, Ansage
    //      -16 (im Solo das Doppelte von 8), macht -8 je Gegner.
    {
        HandResult result = hand(ContractId::TappSolo);
        setTricks(result, 9);
        wonWith(result, 12, tarockCard(1), 1);   // der Pagat fällt im 12. Stich
        Announcement pagat;
        pagat.bonus = BonusId::Pagat;
        pagat.party = Party::Declarer;
        result.announcements.push_back(pagat);
        Dealer dealer;
        dealer.give(result.defenderCards, tarockCard(1));
        give(result, dealer, 41);
        expect(3, settle(result), -16, 8, 8);
    }

    // 4 -- Oberer mit Kontra, verloren mit 33: 5 x 2 = 10 je Gegner.
    {
        HandResult result = hand(ContractId::TappOberer);
        setTricks(result, 7);
        result.gameKontra = 2;
        Dealer dealer;
        give(result, dealer, 33);
        expect(4, settle(result), -20, 10, 10);
    }

    // 5 -- Remis: genau 35 von 70 ist verloren ("35 und zwei Blatt" sind 36).
    {
        HandResult result = hand(ContractId::TappDreier);
        setTricks(result, 8);
        Dealer dealer;
        give(result, dealer, 35);
        expect(5, settle(result), -6, 3, 3);
    }

    // 6 -- stiller Valat im Dreier: 4 x Spielwert 3 = 12 je Gegner.
    //      Das Beispiel zählt zusätzlich die Trull als Handprämie (3), also
    //      15 je Gegner; ohne Handprämien sind es 12.
    {
        HandResult result = hand(ContractId::TappDreier);
        setTricks(result, 16);
        Dealer dealer;
        give(result, dealer, 70);
        expect(6, settle(result), 24, -12, -12);
    }

    // 7 -- Unterer gewonnen mit 37: 4 je Gegner. Die Königstrull des dritten
    //      Spielers (3 von jedem anderen) fehlt der App noch; mit ihr stünde
    //      A +5, B -7, C +2.
    {
        HandResult result = hand(ContractId::TappUnterer);
        setTricks(result, 9);
        Dealer dealer;
        give(result, dealer, 37);
        expect(7, settle(result), 8, -4, -4);
    }

    if (failures == 0)
        std::printf("test_scoring_tapp: die sieben Beispiele stimmen\n");
    else
        std::printf("test_scoring_tapp: %d Fehler\n", failures);
    return failures == 0 ? 0 : 1;
}
