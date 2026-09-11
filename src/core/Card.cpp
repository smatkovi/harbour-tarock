#include "Card.h"

#include <cstdio>
#include <cstring>

namespace tarock {
namespace {

const char* const kTarockNames[22] = {
    "I", "II", "III", "IIII", "V", "VI", "VII", "VIII", "IX", "X", "XI",
    "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX", "XXI", "Sk"
};
const char kSuitLetters[4] = {'H', 'D', 'S', 'C'};
const char* const kCourtNames[4] = {"K", "D", "R", "B"};
const char* const kRedLow[4] = {"1", "2", "3", "4"};
const char* const kBlackLow[4] = {"10", "9", "8", "7"};

} // namespace

CardList toList(const CardSet& set)
{
    CardList cards;
    for (int id = 0; id < 54; ++id) {
        if (set.test(static_cast<std::size_t>(id)))
            cards.push_back(Card(static_cast<std::uint8_t>(id)));
    }
    return cards;
}

CardSet toSet(const CardList& cards)
{
    CardSet set;
    for (Card card : cards) {
        if (card.valid())
            set.set(card.id);
    }
    return set;
}

const char* suitKey(Suit suit)
{
    switch (suit) {
    case Suit::Heart: return "H";
    case Suit::Diamond: return "D";
    case Suit::Spade: return "S";
    case Suit::Club: return "C";
    case Suit::Tarock: return "T";
    }
    return "?";
}

std::string cardKey(Card card)
{
    if (!card.valid())
        return "??";
    if (card.tarock())
        return kTarockNames[card.id];
    std::string key(1, kSuitLetters[static_cast<int>(card.suit())]);
    key += ':';
    const int slot = card.slot();
    if (slot < 4)
        key += kCourtNames[slot];
    else
        key += card.red() ? kRedLow[slot - 4] : kBlackLow[slot - 4];
    return key;
}

bool parseCardKey(const std::string& key, Card& card)
{
    for (int i = 0; i < 22; ++i) {
        if (key == kTarockNames[i]) {
            card = Card(static_cast<std::uint8_t>(i));
            return true;
        }
    }
    if (key.size() < 3 || key[1] != ':')
        return false;
    int suit = -1;
    for (int i = 0; i < 4; ++i) {
        if (key[0] == kSuitLetters[i])
            suit = i;
    }
    if (suit < 0)
        return false;
    const std::string rank = key.substr(2);
    const bool red = suit <= 1;
    for (int slot = 0; slot < 4; ++slot) {
        if (rank == kCourtNames[slot]) {
            card = suitCard(Suit(suit), slot);
            return true;
        }
    }
    for (int i = 0; i < 4; ++i) {
        if (rank == (red ? kRedLow[i] : kBlackLow[i])) {
            card = suitCard(Suit(suit), 4 + i);
            return true;
        }
    }
    return false;
}

} // namespace tarock
