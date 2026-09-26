// Settlement of one finished hand, docs/koenigrufen.md §7 and design.md §4.8.
//
// The hand is broken into Posten: the game itself and every bonus, announced or
// silent. Each Posten has a tariff, a kontra level and the party it is paid to;
// the Ledger turns them into an amount per seat — for four players (the declarer
// alone against three, or the declarer and his partner against two) and for the
// five-player table, where the dealer sits out and is neither paid nor charged.
//
// Everything here is a pure function of its input: no state, no I/O, no clock.
#pragma once

#include "Card.h"
#include "RuleProfile.h"

#include <array>
#include <cstdint>
#include <vector>

namespace tarock {

constexpr int kMaxSeats = 5;

enum class Party : std::uint8_t { Declarer, Defenders, Neutral };

enum class PostenType : std::uint8_t {
    Game,        // the contract itself
    Bonus,       // Trull, all kings, king ultimo, the birds
    Valat,       // replaces the game posten (§7.4)
    Concede,     // "schleifen": game plus the named bird (§7.7 a)
    Trischaken,  // one seat's share of a Trischaken (§7.6)
    Tarokkszam   // Hungarian tarokkszám; listed so one score sheet serves both
};

struct Posten {
    PostenType type = PostenType::Game;
    BonusId bonus = BonusId::None;
    Party owner = Party::Neutral;    // who announced it; Neutral = silent
    Party winner = Party::Neutral;   // who is paid
    bool announced = false;
    bool achieved = false;
    int value = 0;                   // tariff in points, per paying player
    int kontra = 1;                  // 1|2|4|8, money only (§7.8)
    // Exact amount this posten moves per paying player, in thirds of a point.
    // Three thirds are one point; it is 3 * value everywhere except in
    // Trischaken, where a pot is split between two players (§7.6).
    int thirds = 0;
    // Negative contracts settle with each opponent separately (§7.5); in a
    // Trischaken this is the seat the amount belongs to. -1 otherwise.
    std::int8_t againstSeat = -1;
};

// One finished trick as the whole table saw it.
struct TrickResult {
    CardSet cards;                   // the four cards that fell
    Card winningCard;                // the card that took the trick
    std::int8_t winner = -1;         // the seat that took it
};

// An announced Posten. The game itself is "announced" by the bid, so its kontra
// level travels in HandResult::gameKontra.
struct Announcement {
    BonusId bonus = BonusId::None;
    Party party = Party::Declarer;   // the announcing party
    int kontra = 1;                  // 1|2|4|8
};

// Everything the settlement needs once the last trick lies on the table.
struct HandResult {
    ProfileId profile = ProfileId::AtKrOoe2023;
    ContractId contract = ContractId::None;

    int players = 4;                 // seats at the table, 4 or 5
    std::int8_t sittingOut = -1;     // five-player table: the dealer (§2.7)
    std::int8_t declarer = 0;
    std::int8_t partner = -1;        // -1 = the declarer plays alone
    std::int8_t forehand = -1;       // the Trischaker; -1 falls back to declarer

    // Who owns which cards at the end, talon and discards included (§1.8). The
    // caller decides the ownership, TarockRules::ownedBy() knows the table.
    CardSet declarerCards;
    CardSet defenderCards;
    std::array<CardSet, kMaxSeats> seatCards{};   // Trischaken counts per seat

    std::vector<TrickResult> tricks;             // in playing order, 1..12
    std::vector<Announcement> announcements;

    Card calledKing;                 // Ruferspiele: the king that was called
    int gameKontra = 1;              // positive games: one level for all four
    std::array<int, kMaxSeats> seatKontra{{1, 1, 1, 1, 1}};  // negative games (§7.5)
    bool kontraVoid = false;         // Solorufer, called king in the talon (§7.7 c)
    bool conceded = false;           // "schleifen" (§7.7 a)
    BonusId concededBird = BonusId::None;   // the bird named when conceding
};

struct Ledger {
    // Exact amounts; the two point arrays are these rounded for display.
    std::array<int, kMaxSeats> schriftThirds{};
    std::array<int, kMaxSeats> geldThirds{};
    std::array<int, kMaxSeats> schrift{};   // tournament score, without kontra
    std::array<int, kMaxSeats> geld{};      // payout, with kontra
    std::vector<Posten> items;
    CountResult declarerCards;
    CountResult defenderCards;
    std::array<CountResult, kMaxSeats> seatCards{};   // Trischaken
    bool declarerWon = false;   // the declarer's party reached the card threshold
    bool gameWon = false;       // the declarer made his contract (§7.4, §7.5)
    // Unentschieden: niemand hat die Schwelle erreicht und das Spiel selbst
    // erzeugt keinen Posten. Nur das einfache Spiel im Strohmandeln kann das
    // (strohmandeln.md §1.7.1); überall sonst bleibt es falsch.
    bool gameDrawn = false;

    bool zeroSum() const;            // test invariant, both accounts
};

Party otherParty(Party party);
Party partyOfSeat(const HandResult& hand, int seat);
int pointsFromThirds(int thirds);

// Tariff of one bonus: half when silent, doubled in the solo games (§7.3).
int bonusValue(const RuleProfile& profile, const ContractDef& contract, BonusId bonus,
               bool announced);
// Did that party actually make the bonus? Read off the captured cards and the
// trick winners (§5.2).
bool bonusAchievedBy(const HandResult& hand, BonusId bonus, Party party);

std::vector<Posten> buildPosten(const HandResult& hand);
Ledger book(const HandResult& hand, std::vector<Posten> items);
Ledger settle(const HandResult& hand);

} // namespace tarock
