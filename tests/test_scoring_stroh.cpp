// Die Abrechnung des Strohmandelns: die vier durchgerechneten Beispiele aus
// docs/strohmandeln.md §7.4, Zahl für Zahl.
//
// Die Nullsumme allein beweist nichts -- sie stimmt auch bei falschen Tarifen.
// Hier steht darum jeder Betrag: einfaches Spiel 1, aufgenommenes gewonnen 3,
// verloren 4 an den Gegner, Grammel-Punkt 1 ab 45 eigenen Punkten, Trull 1,
// vier Könige 1, Pagat/Uhu/Kakadu je 1 an den Gewinner ihres Stiches, Valat 12
// zusätzlich zum Spiel. Kontra gibt es hier nicht (§5.11).
#include "core/Card.h"
#include "core/RuleProfile.h"
#include "core/Scoring.h"

#include <array>
#include <cstdio>

using namespace tarock;

namespace {

int failures = 0;

const RuleProfile& stroh()
{
    return RuleProfile::get(ProfileId::AtStrohMsErw);
}

void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::fprintf(stderr, "FEHLT Zeile %d: %s\n", line, what);
        ++failures;
    }
}
#define CHECK(x) check((x), #x, __LINE__)

// Verteilt echte Karten, bis eine Partei genau so viele Drittel hat, wie das
// Beispiel verlangt (§1.5.1: jede Karte zählt 3 * Wert - 2 Drittel).
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
        int need = thirds - stroh().count(target).units;
        while (need > 0) {
            Card best;
            int bestThirds = 0;
            for (int id = 0; id < 54; ++id) {
                const Card card(static_cast<std::uint8_t>(id));
                if (!m_free.test(static_cast<std::size_t>(id)))
                    continue;
                const int value = 3 * stroh().cardValue(card) - 2;
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

// Ein Zweiertisch: Vorhand ist Platz 0, der Geber Platz 1.
HandResult hand(ContractId contract, int declarer)
{
    HandResult result;
    result.profile = ProfileId::AtStrohMsErw;
    result.contract = contract;
    result.players = 2;
    result.sittingOut = -1;
    result.declarer = static_cast<std::int8_t>(declarer);
    result.partner = -1;
    result.forehand = 0;
    return result;
}

// Ein Stich, von dem nur zählt, wer ihn genommen hat und was darin lag.
TrickResult trick(int winner, CardList cards)
{
    TrickResult result;
    result.cards = toSet(cards);
    result.winner = static_cast<std::int8_t>(winner);
    result.winningCard = cards.empty() ? Card() : cards.front();
    return result;
}

// Füllt die Stichliste auf 27 auf, ohne eine Vogelprämie auszulösen: die
// Füllstiche bleiben leer und wechseln den Gewinner, damit kein Valat entsteht.
void padTricks(HandResult& result, int upTo)
{
    while (static_cast<int>(result.tricks.size()) < upTo)
        result.tricks.push_back(trick(static_cast<int>(result.tricks.size()) % 2, {}));
}

// Beispiel 1 (§7.4): Die Vorhand hat aufgenommen und erreicht 35 2/3, der
// Geber 34 1/3. Sie hat die Trull in den Stichen, aber nur drei Könige; der
// Pagat war nicht im letzten Stich.  Erwartet: +4 / -4.
void beispiel1()
{
    Dealer dealer;
    HandResult h = hand(ContractId::StrohAufgenommen, 0);
    for (Card card : {kSkues, kMond, kPagat})
        dealer.give(h.declarerCards, card);
    // Drei Könige für die Vorhand, der vierte für den Geber: keiner hat alle.
    for (Suit suit : {Suit::Heart, Suit::Diamond, Suit::Spade})
        dealer.give(h.declarerCards, suitCard(suit, 0));
    dealer.give(h.defenderCards, suitCard(Suit::Club, 0));
    CHECK(dealer.fill(h.declarerCards, 107));
    h.defenderCards |= dealer.rest();
    CHECK(stroh().count(h.declarerCards).units == 107);
    CHECK(stroh().count(h.defenderCards).units == 103);
    CHECK(stroh().count(h.declarerCards).units + stroh().count(h.defenderCards).units == 210);

    const Ledger ledger = settle(h);
    CHECK(ledger.zeroSum());
    CHECK(ledger.gameWon && !ledger.gameDrawn);
    CHECK(ledger.schrift[0] == 4);    // Spiel 3 + Trull 1
    CHECK(ledger.schrift[1] == -4);
}

// Beispiel 2 (§7.4): Der Geber hat aufgenommen und erreicht 35 1/3 -- das ist
// weniger als 35 und zwei Blatt, also verloren. Die Vorhand hat 34 2/3, die
// vier Könige in den Stichen, und sie fängt den Pagat im letzten Stich.
// Erwartet: Geber -6, Vorhand +6.
void beispiel2()
{
    Dealer dealer;
    HandResult h = hand(ContractId::StrohAufgenommen, 1);
    for (int suit = 0; suit < 4; ++suit)
        dealer.give(h.defenderCards, suitCard(Suit(suit), 0));
    // Der Pagat fällt im 27. Stich und die Vorhand nimmt ihn.
    dealer.give(h.defenderCards, kPagat);
    // Sküs und Mond liegen beim Aufnehmer, damit die Trull niemandem gehört.
    for (Card card : {kSkues, kMond})
        dealer.give(h.declarerCards, card);
    CHECK(dealer.fill(h.declarerCards, 106));
    h.defenderCards |= dealer.rest();
    CHECK(stroh().count(h.declarerCards).units == 106);
    CHECK(stroh().count(h.defenderCards).units == 104);

    padTricks(h, 26);
    h.tricks.push_back(trick(0, {kPagat}));
    CHECK(static_cast<int>(h.tricks.size()) == 27);

    const Ledger ledger = settle(h);
    CHECK(ledger.zeroSum());
    CHECK(!ledger.gameWon && !ledger.gameDrawn);
    CHECK(ledger.schrift[1] == -6);   // Spiel 4 + Könige 1 + Pagat gefangen 1
    CHECK(ledger.schrift[0] == 6);
}

// Beispiel 3 (§7.4): Beide haben gepasst, Endstand 35 : 35 -- niemand erreicht
// 35 und zwei Blatt, das Spiel ist unentschieden. Die Vorhand hat den Uhu
// gemacht. Erwartet: +1 / -1, das Spiel selbst 0:0.
void beispiel3()
{
    Dealer dealer;
    HandResult h = hand(ContractId::StrohEinfach, 0);
    for (Card card : {kSkues, kPagat})
        dealer.give(h.declarerCards, card);
    dealer.give(h.defenderCards, kMond);
    for (Suit suit : {Suit::Heart, Suit::Diamond})
        dealer.give(h.declarerCards, suitCard(suit, 0));
    for (Suit suit : {Suit::Spade, Suit::Club})
        dealer.give(h.defenderCards, suitCard(suit, 0));
    CHECK(dealer.fill(h.declarerCards, 105));
    h.defenderCards |= dealer.rest();
    CHECK(stroh().count(h.declarerCards).units == 105);
    CHECK(stroh().count(h.defenderCards).units == 105);

    padTricks(h, 25);
    h.tricks.push_back(trick(0, {tarockCard(2)}));   // der Uhu im 26. Stich
    h.tricks.push_back(trick(1, {}));
    CHECK(static_cast<int>(h.tricks.size()) == 27);

    const Ledger ledger = settle(h);
    CHECK(ledger.zeroSum());
    CHECK(ledger.schrift[0] == 1);    // nur der Uhu
    CHECK(ledger.schrift[1] == -1);
    // Unentschieden heißt nicht verloren: das muss am Ende auch dastehen.
    CHECK(ledger.gameDrawn);
    CHECK(!ledger.gameWon);
    // Das Spiel selbst hat keinen Posten erzeugt.
    for (const Posten& posten : ledger.items)
        CHECK(posten.type != PostenType::Game);
}

// Beispiel 4 (§7.4): Die Vorhand hat aufgenommen und macht alle 27 Stiche.
// Erwartet ohne die Vögel: Spiel 3 + Grammel 1 + Trull 1 + Könige 1 +
// Valat 12 = +18. Liegt der Pagat im 27. Stich, ist es einer mehr.
void beispiel4()
{
    for (bool pagatUltimo : {false, true}) {
        HandResult h = hand(ContractId::StrohAufgenommen, 0);
        h.declarerCards.set();          // alle 54 Karten
        h.declarerCards &= stroh().deck().cards();
        CHECK(stroh().count(h.declarerCards).units == 210);
        CHECK(stroh().count(h.defenderCards).units == 0);

        // 27 Stiche, alle von der Vorhand genommen. Uhu und Kakadu liegen
        // absichtlich nicht in ihren Zielstichen.
        for (int i = 0; i < 27; ++i)
            h.tricks.push_back(trick(0, i == 0 && !pagatUltimo ? CardList{kPagat} : CardList{}));
        if (pagatUltimo)
            h.tricks[26] = trick(0, {kPagat});

        const Ledger ledger = settle(h);
        CHECK(ledger.zeroSum());
        CHECK(ledger.gameWon);
        CHECK(ledger.schrift[0] == (pagatUltimo ? 19 : 18));
        CHECK(ledger.schrift[1] == (pagatUltimo ? -19 : -18));
    }
}

// Der Grammel-Punkt hängt an den gerundeten 45 Punkten, also an 134 Dritteln
// (§7.2, Schritt 4) -- eines weniger zahlt nicht.
void grammelSchwelle()
{
    for (int thirds : {133, 134}) {
        Dealer dealer;
        HandResult h = hand(ContractId::StrohAufgenommen, 0);
        CHECK(dealer.fill(h.declarerCards, thirds));
        h.defenderCards |= dealer.rest();
        const Ledger ledger = settle(h);
        CHECK(ledger.zeroSum());
        int grammel = 0;
        for (const Posten& posten : ledger.items) {
            if (posten.bonus == BonusId::Grammel)
                ++grammel;
        }
        CHECK(grammel == (thirds >= 134 ? 1 : 0));
    }
}

} // namespace

int main()
{
    beispiel1();
    beispiel2();
    beispiel3();
    beispiel4();
    grammelSchwelle();
    if (failures == 0)
        std::printf("test_scoring_stroh: alle Beispiele stimmen\n");
    return failures == 0 ? 0 : 1;
}
