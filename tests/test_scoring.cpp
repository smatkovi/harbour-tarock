// Settlement: every worked example of docs/koenigrufen.md §7 must come out
// exactly — the sixteen hands of §7.10 and the nine Trischaken rows of §7.6.
// The examples are reproduced from card sets and trick winners, not from
// pre-computed sums, so the counting of §1.5 is part of the test.
#include "core/Card.h"
#include "core/RuleProfile.h"
#include "core/Scoring.h"

#include <array>
#include <cstdio>

using namespace tarock;

namespace {

int failures = 0;

void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::fprintf(stderr, "FAILED line %d: %s\n", line, what);
        ++failures;
    }
}
#define CHECK(x) check((x), #x, __LINE__)

const RuleProfile& kr()
{
    return RuleProfile::get(ProfileId::AtKrOoe2023);
}

// Hands out real cards: named cards go where the example wants them, the rest
// pads a party up to the exact number of thirds the example needs.
class Dealer {
public:
    Dealer() { m_free.set(); }

    void give(CardSet& target, Card card)
    {
        m_free.reset(card.id);
        target.set(card.id);
    }

    // Every card is worth 3 * value - 2 thirds (§1.5), so any target can be
    // reached by taking the largest card that still fits.
    bool fill(CardSet& target, int thirds)
    {
        int need = thirds - kr().count(target).units;
        while (need > 0) {
            Card best;
            int bestThirds = 0;
            for (int id = 0; id < 54; ++id) {
                const Card card(static_cast<std::uint8_t>(id));
                if (!m_free.test(static_cast<std::size_t>(id)))
                    continue;
                const int value = 3 * kr().cardValue(card) - 2;
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

Card king(Suit suit)
{
    return suitCard(suit, 0);
}

HandResult hand(ContractId contract, int partner)
{
    HandResult result;
    result.profile = ProfileId::AtKrOoe2023;
    result.contract = contract;
    result.players = 4;
    result.declarer = 0;
    result.partner = static_cast<std::int8_t>(partner);
    result.forehand = 0;
    return result;
}

// Twelve tricks; only who took them matters until a bonus card is named.
void setTricks(HandResult& result, const std::array<int, 12>& winners, int count = 12)
{
    result.tricks.clear();
    for (int i = 0; i < count; ++i) {
        TrickResult trick;
        trick.winner = static_cast<std::int8_t>(winners[static_cast<std::size_t>(i)]);
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

void expect(const char* section, int example, const Ledger& ledger,
            const std::array<int, 5>& schrift, const std::array<int, 5>& geld)
{
    for (std::size_t seat = 0; seat < 5; ++seat) {
        if (ledger.schrift[seat] != schrift[seat]) {
            std::fprintf(stderr, "FAILED %s example %d: Schrift seat %d = %d, expected %d\n",
                         section, example, static_cast<int>(seat), ledger.schrift[seat],
                         schrift[seat]);
            ++failures;
        }
        if (ledger.geld[seat] != geld[seat]) {
            std::fprintf(stderr, "FAILED %s example %d: Geld seat %d = %d, expected %d\n", section,
                         example, static_cast<int>(seat), ledger.geld[seat], geld[seat]);
            ++failures;
        }
    }
    if (!ledger.zeroSum()) {
        std::fprintf(stderr, "FAILED %s example %d: ledger is not zero-sum\n", section, example);
        ++failures;
    }
}

void expectItem(const char* section, int example, const Ledger& ledger, PostenType type,
                BonusId bonus, Party winner, int value)
{
    for (const Posten& posten : ledger.items) {
        if (posten.type == type && posten.bonus == bonus && posten.winner == winner
            && posten.value == value)
            return;
    }
    std::fprintf(stderr, "FAILED %s example %d: missing Posten (type %d, bonus %d, value %d)\n",
                 section, example, static_cast<int>(type), static_cast<int>(bonus), value);
    ++failures;
}

const std::array<int, 12> kDeclarerTakesAll = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const std::array<int, 12> kMixed = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3};

// --- §7.10, the sixteen worked hands ---------------------------------------

// 1: Rufer won (41/2), silent Trull for the defenders, silent four kings for
//    the declarer's party.
void example1()
{
    HandResult h = hand(ContractId::Rufer, 1);
    h.calledKing = king(Suit::Club);
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    for (Suit suit : {Suit::Heart, Suit::Diamond, Suit::Spade, Suit::Club})
        dealer.give(declarer, king(suit));
    dealer.give(defenders, kSkues);
    dealer.give(defenders, kMond);
    dealer.give(defenders, kPagat);
    CHECK(dealer.fill(declarer, 125));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);

    const Ledger ledger = settle(h);
    CHECK(ledger.declarerCards.units == 125 && ledger.declarerWon);
    expectItem("§7.10", 1, ledger, PostenType::Game, BonusId::None, Party::Declarer, 1);
    expectItem("§7.10", 1, ledger, PostenType::Bonus, BonusId::Trull, Party::Defenders, 1);
    expectItem("§7.10", 1, ledger, PostenType::Bonus, BonusId::AllKings, Party::Declarer, 1);
    expect("§7.10", 1, ledger, {1, 1, -1, -1, 0}, {1, 1, -1, -1, 0});
}

// 2: Besserrufer, Pagat announced and made, game won, one opponent contra'd
//    the Pagat.
void example2()
{
    HandResult h = hand(ContractId::Besserrufer, 1);
    h.calledKing = king(Suit::Club);
    h.announcements.push_back({BonusId::Pagat, Party::Declarer, 2});
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kPagat);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(defenders, kSkues);
    dealer.give(defenders, kMond);
    dealer.give(defenders, king(Suit::Club));
    CHECK(dealer.fill(declarer, 120));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);
    wonWith(h, 12, kPagat, 0);

    const Ledger ledger = settle(h);
    expectItem("§7.10", 2, ledger, PostenType::Bonus, BonusId::Pagat, Party::Declarer, 2);
    expect("§7.10", 2, ledger, {3, 3, -3, -3, 0}, {5, 5, -5, -5, 0});
}

// 3: Piccolo won, one opponent contra'd. Every opponent settles on his own.
void example3()
{
    HandResult h = hand(ContractId::Piccolo, -1);
    h.seatKontra = {{1, 2, 1, 1, 1}};
    setTricks(h, {1, 2, 0, 3, 1, 2, 3, 1, 2, 3, 1, 2});   // the declarer takes one

    const Ledger ledger = settle(h);
    expect("§7.10", 3, ledger, {6, -2, -2, -2, 0}, {8, -4, -2, -2, 0});
}

// 4: Dreier won, silent Trull for the declarer, silent Pagat for the
//    defenders. An accounting illustration: the two bonuses cancel, so the
//    Posten themselves are checked as well.
void example4()
{
    HandResult h = hand(ContractId::Dreier, -1);
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, kMond);
    dealer.give(declarer, kPagat);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(defenders, king(Suit::Club));
    CHECK(dealer.fill(declarer, 130));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);
    wonWith(h, 12, kPagat, 2);

    const Ledger ledger = settle(h);
    expectItem("§7.10", 4, ledger, PostenType::Game, BonusId::None, Party::Declarer, 4);
    expectItem("§7.10", 4, ledger, PostenType::Bonus, BonusId::Trull, Party::Declarer, 1);
    expectItem("§7.10", 4, ledger, PostenType::Bonus, BonusId::Pagat, Party::Defenders, 1);
    expect("§7.10", 4, ledger, {12, -4, -4, -4, 0}, {12, -4, -4, -4, 0});
}

// 5: Sechserdreier lost — the game counts double, the announced Uhu does not.
void example5()
{
    HandResult h = hand(ContractId::Sechserdreier, -1);
    h.announcements.push_back({BonusId::Uhu, Party::Declarer, 1});
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(defenders, kMond);
    dealer.give(defenders, kPagat);
    dealer.give(defenders, king(Suit::Club));
    CHECK(dealer.fill(declarer, 85));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);
    wonWith(h, 11, tarockCard(2), 0);

    const Ledger ledger = settle(h);
    CHECK(!ledger.declarerWon);
    expectItem("§7.10", 5, ledger, PostenType::Game, BonusId::None, Party::Defenders, 8);
    expectItem("§7.10", 5, ledger, PostenType::Bonus, BonusId::Uhu, Party::Declarer, 4);
    expect("§7.10", 5, ledger, {-12, 4, 4, 4, 0}, {-12, 4, 4, 4, 0});
}

// 6: Sechserdreier lost with a kontra on the game — money only.
void example6()
{
    HandResult h = hand(ContractId::Sechserdreier, -1);
    h.gameKontra = 2;
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(defenders, kMond);
    dealer.give(defenders, kPagat);
    dealer.give(defenders, king(Suit::Club));
    CHECK(dealer.fill(declarer, 85));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);

    const Ledger ledger = settle(h);
    expect("§7.10", 6, ledger, {-24, 8, 8, 8, 0}, {-48, 16, 16, 16, 0});
}

// 7: Solorufer lost, announced king ultimo made — bonuses count double in the
//    solo games, and the partner may take the last trick.
void example7()
{
    HandResult h = hand(ContractId::Solorufer, 1);
    h.calledKing = king(Suit::Heart);
    h.announcements.push_back({BonusId::KingUltimo, Party::Declarer, 1});
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(declarer, king(Suit::Spade));
    dealer.give(defenders, kMond);
    dealer.give(defenders, kPagat);
    dealer.give(defenders, king(Suit::Club));
    dealer.give(defenders, king(Suit::Diamond));
    CHECK(dealer.fill(declarer, 85));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);
    wonWith(h, 12, king(Suit::Heart), 1);

    const Ledger ledger = settle(h);
    expectItem("§7.10", 7, ledger, PostenType::Bonus, BonusId::KingUltimo, Party::Declarer, 4);
    expect("§7.10", 7, ledger, {2, 2, -2, -2, 0}, {2, 2, -2, -2, 0});
}

// 8: Solorufer with the called king in the talon: one against three, and every
//    kontra is void (§7.7 c).
void example8()
{
    HandResult h = hand(ContractId::Solorufer, -1);
    h.calledKing = king(Suit::Heart);
    h.gameKontra = 2;
    h.kontraVoid = true;
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, kMond);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(declarer, king(Suit::Spade));
    dealer.give(defenders, kPagat);
    dealer.give(defenders, king(Suit::Club));
    dealer.give(defenders, king(Suit::Diamond));
    CHECK(dealer.fill(declarer, 125));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);

    const Ledger ledger = settle(h);
    expect("§7.10", 8, ledger, {6, -2, -2, -2, 0}, {6, -2, -2, -2, 0});
}

// 9: Besserrufer, game won, but the announced and contra'd Uhu is lost.
void example9()
{
    HandResult h = hand(ContractId::Besserrufer, 1);
    h.calledKing = king(Suit::Club);
    h.announcements.push_back({BonusId::Uhu, Party::Declarer, 2});
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(defenders, kMond);
    dealer.give(defenders, kPagat);
    dealer.give(defenders, king(Suit::Club));
    CHECK(dealer.fill(declarer, 120));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);

    const Ledger ledger = settle(h);
    expectItem("§7.10", 9, ledger, PostenType::Bonus, BonusId::Uhu, Party::Defenders, 4);
    expect("§7.10", 9, ledger, {-3, -3, 3, 3, 0}, {-7, -7, 7, 7, 0});
}

// 10: Dreier with a silent Valat and an announced Trull; the silent Pagat does
//     not count next to a Valat (§7.4).
void example10()
{
    HandResult h = hand(ContractId::Dreier, -1);
    h.announcements.push_back({BonusId::Trull, Party::Declarer, 1});
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, kMond);
    dealer.give(declarer, kPagat);
    CHECK(dealer.fill(declarer, 180));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kDeclarerTakesAll);
    wonWith(h, 12, kPagat, 0);

    const Ledger ledger = settle(h);
    expectItem("§7.10", 10, ledger, PostenType::Valat, BonusId::Valat, Party::Declarer, 16);
    expectItem("§7.10", 10, ledger, PostenType::Bonus, BonusId::Trull, Party::Declarer, 2);
    CHECK(ledger.items.size() == 2);   // no game posten, no silent Pagat
    expect("§7.10", 10, ledger, {54, -18, -18, -18, 0}, {54, -18, -18, -18, 0});
}

// 11: Dreier, Valat announced and lost although the game itself was won; the
//     announced Pagat still counts.
void example11()
{
    HandResult h = hand(ContractId::Dreier, -1);
    h.announcements.push_back({BonusId::Valat, Party::Declarer, 1});
    h.announcements.push_back({BonusId::Pagat, Party::Declarer, 1});
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kPagat);
    dealer.give(declarer, kSkues);
    dealer.give(defenders, kMond);
    CHECK(dealer.fill(declarer, 120));   // 40 points, the game would be won
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);
    wonWith(h, 12, kPagat, 0);

    const Ledger ledger = settle(h);
    CHECK(ledger.declarerWon);           // won on cards and still lost the hand
    expectItem("§7.10", 11, ledger, PostenType::Valat, BonusId::Valat, Party::Defenders, 32);
    expect("§7.10", 11, ledger, {-90, 30, 30, 30, 0}, {-90, 30, 30, 30, 0});
}

// 12: Sechserdreier, silent Valat of the defenders, game contra'd: the lost
//     Sechserdreier doubles the Valat too.
void example12()
{
    HandResult h = hand(ContractId::Sechserdreier, -1);
    h.gameKontra = 2;
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    CHECK(dealer.fill(declarer, 30));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, {1, 2, 3, 1, 2, 3, 1, 2, 3, 1, 2, 3});

    const Ledger ledger = settle(h);
    expectItem("§7.10", 12, ledger, PostenType::Valat, BonusId::Valat, Party::Defenders, 32);
    CHECK(ledger.items.size() == 1);     // silent bonuses of the defenders drop out
    expect("§7.10", 12, ledger, {-96, 32, 32, 32, 0}, {-192, 64, 64, 64, 0});
}

// 13: Rufer, Valat announced, made and contra'd — two against two.
void example13()
{
    HandResult h = hand(ContractId::Rufer, 1);
    h.calledKing = king(Suit::Club);
    h.announcements.push_back({BonusId::Valat, Party::Declarer, 2});
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    CHECK(dealer.fill(declarer, 200));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1});

    const Ledger ledger = settle(h);
    expect("§7.10", 13, ledger, {8, 8, -8, -8, 0}, {16, 16, -16, -16, 0});
}

// 14: Farbensolo won with silent four kings, which count double in a solo.
void example14()
{
    HandResult h = hand(ContractId::Farbensolo, -1);
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    for (Suit suit : {Suit::Heart, Suit::Diamond, Suit::Spade, Suit::Club})
        dealer.give(declarer, king(suit));
    dealer.give(defenders, kSkues);
    dealer.give(defenders, kMond);
    dealer.give(defenders, kPagat);
    CHECK(dealer.fill(declarer, 125));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);

    const Ledger ledger = settle(h);
    expectItem("§7.10", 14, ledger, PostenType::Bonus, BonusId::AllKings, Party::Declarer, 2);
    CHECK(ledger.items.size() == 2);     // no Trull in a suit game (§5.1)
    expect("§7.10", 14, ledger, {24, -8, -8, -8, 0}, {24, -8, -8, -8, 0});
}

// 15: Bettel ouvert lost, one kontra and one kontra plus rekontra — each
//     opponent settles with his own factor (§7.5).
void example15()
{
    HandResult h = hand(ContractId::BettelOuvert, -1);
    h.seatKontra = {{1, 2, 4, 1, 1}};
    setTricks(h, {0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 3);   // early end, §6.8

    const Ledger ledger = settle(h);
    expect("§7.10", 15, ledger, {-21, 7, 7, 7, 0}, {-49, 14, 28, 7, 0});
}

// 16: Schleifen of a Besserrufer with an Uhu: game plus the named bird, to all
//     three others (§7.7 a).
void example16()
{
    HandResult h = hand(ContractId::Besserrufer, -1);
    h.conceded = true;
    h.concededBird = BonusId::Uhu;
    h.calledKing = king(Suit::Club);

    const Ledger ledger = settle(h);
    CHECK(ledger.items.size() == 2);
    expect("§7.10", 16, ledger, {-15, 5, 5, 5, 0}, {-15, 5, 5, 5, 0});
}

// --- §7.8, a kontra belongs to exactly one Posten ---------------------------

// Rufer won with a kontra on the game and a silent Trull. Only the game and
// announced bonuses can be contra'd (§5.6, §7.8), so the silent Trull is paid at
// factor one: Schrift 1 + 1, money 1 × 2 + 1.
void silentBonusCarriesNoKontra()
{
    HandResult h = hand(ContractId::Rufer, 1);
    h.calledKing = king(Suit::Club);
    h.gameKontra = 2;
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, kMond);
    dealer.give(declarer, kPagat);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(defenders, king(Suit::Diamond));
    dealer.give(defenders, king(Suit::Spade));
    dealer.give(defenders, king(Suit::Club));
    CHECK(dealer.fill(declarer, 125));
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);

    const Ledger ledger = settle(h);
    CHECK(ledger.items.size() == 2);   // game and the silent Trull, no four kings
    expectItem("§7.8", 1, ledger, PostenType::Bonus, BonusId::Trull, Party::Declarer, 1);
    expect("§7.8", 1, ledger, {2, 2, -2, -2, 0}, {3, 3, -3, -3, 0});
}

// --- §7.6, the nine Trischaken rows ----------------------------------------

void trischakenRow(int row, const std::array<int, 4>& thirds, const std::array<int, 5>& schrift)
{
    HandResult h = hand(ContractId::Trischaken, -1);
    Dealer dealer;
    for (int seat = 0; seat < 4; ++seat) {
        CardSet cards;
        if (!dealer.fill(cards, thirds[static_cast<std::size_t>(seat)])) {
            std::fprintf(stderr, "FAILED §7.6 example %d: cannot build %d thirds for seat %d\n", row,
                         thirds[static_cast<std::size_t>(seat)], seat);
            ++failures;
            return;
        }
        h.seatCards[static_cast<std::size_t>(seat)] = cards;
    }
    const Ledger ledger = settle(h);
    for (int seat = 0; seat < 4; ++seat) {
        const std::size_t index = static_cast<std::size_t>(seat);
        if (ledger.seatCards[index].units != thirds[index]) {
            std::fprintf(stderr, "FAILED §7.6 example %d: seat %d holds %d thirds, wanted %d\n", row,
                         seat, ledger.seatCards[index].units, thirds[index]);
            ++failures;
        }
    }
    expect("§7.6", row, ledger, schrift, schrift);   // no kontra in Trischaken
}

void trischaken()
{
    // The table of §7.6, in thirds: Trischaker T is seat 0.
    trischakenRow(1, {60, 91, 59, 0}, {0, -3, 0, 3, 0});
    trischakenRow(2, {108, 40, 40, 22}, {-12, 4, 4, 4, 0});
    trischakenRow(3, {36, 75, 75, 24}, {2, -2, -2, 2, 0});
    trischakenRow(4, {0, 106, 104, 0}, {2, -4, 0, 2, 0});
    trischakenRow(5, {0, 107, 103, 0}, {3, -6, 0, 3, 0});
    trischakenRow(6, {30, 60, 60, 60}, {3, -1, -1, -1, 0});
    trischakenRow(7, {75, 75, 30, 30}, {-6, 2, 2, 2, 0});
    trischakenRow(8, {50, 110, 30, 20}, {2, -6, 2, 2, 0});
    trischakenRow(9, {0, 105, 105, 0}, {2, -2, -2, 2, 0});
}

// --- the five-player table (§7.1, point 5) ---------------------------------

void fivePlayerTable()
{
    HandResult h = hand(ContractId::Dreier, -1);
    h.players = 5;
    h.sittingOut = 4;
    Dealer dealer;
    CardSet declarer;
    CardSet defenders;
    dealer.give(declarer, kSkues);
    dealer.give(declarer, king(Suit::Heart));
    dealer.give(defenders, kMond);
    dealer.give(defenders, kPagat);
    dealer.give(defenders, king(Suit::Club));
    CHECK(dealer.fill(declarer, 125));   // won, and no silent bonus for either side
    h.declarerCards = declarer;
    h.defenderCards = defenders | dealer.rest();
    setTricks(h, kMixed);

    const Ledger ledger = settle(h);
    CHECK(ledger.items.size() == 1);
    CHECK(ledger.schrift[0] == 12 && ledger.schrift[1] == -4 && ledger.schrift[2] == -4
          && ledger.schrift[3] == -4);
    CHECK(ledger.schrift[4] == 0 && ledger.geld[4] == 0);   // the dealer sits out
    CHECK(ledger.zeroSum());

    // The same hand as a Trischaken: the fifth seat takes part in nothing.
    HandResult t = hand(ContractId::Trischaken, -1);
    t.players = 5;
    t.sittingOut = 4;
    Dealer other;
    for (int seat = 0; seat < 4; ++seat) {
        CardSet cards;
        CHECK(other.fill(cards, std::array<int, 4>{60, 91, 59, 0}[static_cast<std::size_t>(seat)]));
        t.seatCards[static_cast<std::size_t>(seat)] = cards;
    }
    const Ledger trischakenLedger = settle(t);
    CHECK(trischakenLedger.schrift[4] == 0 && trischakenLedger.zeroSum());
    CHECK(trischakenLedger.schrift[1] == -3 && trischakenLedger.schrift[3] == 3);
}

} // namespace

int main()
{
    CHECK(pointsFromThirds(0) == 0);
    CHECK(pointsFromThirds(12) == 4 && pointsFromThirds(-12) == -4);
    CHECK(otherParty(Party::Declarer) == Party::Defenders);

    example1();
    example2();
    example3();
    example4();
    example5();
    example6();
    example7();
    example8();
    example9();
    example10();
    example11();
    example12();
    example13();
    example14();
    example15();
    example16();
    silentBonusCarriesNoKontra();
    trischaken();
    fivePlayerTable();

    if (failures) {
        std::printf("%d failures\n", failures);
        return 1;
    }
    std::printf("OK: all 16 worked examples of koenigrufen.md §7.10, the kontra rules of §7.8, "
                "the nine Trischaken rows of §7.6 and the five-player table\n");
    return 0;
}
