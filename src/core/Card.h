// Card model shared by every rule profile. The Hungarian 42-card deck is a
// subset of the 54-card Industrie und Glück deck, so one numbering serves all
// profiles, the save format and the network protocol.
#pragma once

#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

namespace tarock {

enum class Suit : std::uint8_t { Heart = 0, Diamond = 1, Spade = 2, Club = 3, Tarock = 4 };

// Card id 0..53:
//   0..21  tarock:      0 = I, 1 = II, … 19 = XX, 20 = XXI, 21 = Sküs
//   22..53 suit cards:  22 + 8 * suit + slot
//          slot 0 = king, 1 = queen, 2 = knight, 3 = jack,
//          slot 4..7 = the four low cards, strongest first
//          red:   slot 4 = ace, 5 = two,  6 = three, 7 = four
//          black: slot 4 = ten, 5 = nine, 6 = eight, 7 = seven
struct Card {
    std::uint8_t id = 0xFF;

    constexpr Card() = default;
    constexpr explicit Card(std::uint8_t value) : id(value) {}

    constexpr bool valid() const { return id < 54; }
    constexpr bool tarock() const { return id < 22; }
    constexpr Suit suit() const { return tarock() ? Suit::Tarock : Suit((id - 22) / 8); }
    constexpr int slot() const { return tarock() ? -1 : (id - 22) % 8; }
    constexpr int tarockNo() const { return tarock() ? id + 1 : 0; }   // Sküs counts as 22
    constexpr bool king() const { return !tarock() && slot() == 0; }
    constexpr bool honour() const { return id == 0 || id == 20 || id == 21; } // I, XXI, Sküs
    constexpr bool bird() const { return id <= 3; }                    // I..IIII
    constexpr bool red() const { return !tarock() && (suit() == Suit::Heart || suit() == Suit::Diamond); }

    constexpr bool operator==(const Card& other) const { return id == other.id; }
    constexpr bool operator!=(const Card& other) const { return id != other.id; }
};

constexpr Card kSkues{21};
constexpr Card kMond{20};
constexpr Card kPagat{0};

constexpr Card tarockCard(int number) { return Card(static_cast<std::uint8_t>(number - 1)); } // 1..22
constexpr Card suitCard(Suit suit, int slot)
{
    return Card(static_cast<std::uint8_t>(22 + 8 * static_cast<int>(suit) + slot));
}

// Trick power inside one suit: tarock 1..22, suit cards 8 (king) down to 1.
constexpr int power(Card card) { return card.tarock() ? card.tarockNo() : 8 - card.slot(); }

using CardSet = std::bitset<54>;
using CardList = std::vector<Card>;

inline void add(CardSet& set, Card card) { set.set(card.id); }
inline void remove(CardSet& set, Card card) { set.reset(card.id); }
inline bool contains(const CardSet& set, Card card) { return card.valid() && set.test(card.id); }
CardList toList(const CardSet& set);
CardSet toSet(const CardList& cards);

// Stable text keys used by lessons, tests and logs: "I", "XXI", "Sk",
// "H:K", "H:D", "H:R", "H:B", "H:1".."H:4", "S:10".."S:7".
std::string cardKey(Card card);
bool parseCardKey(const std::string& key, Card& card);
const char* suitKey(Suit suit);

} // namespace tarock
