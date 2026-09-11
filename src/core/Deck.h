// Deck description and dealing. The Hungarian 42-card deck drops the three
// lowest cards of every suit, so both profiles share one card numbering.
#pragma once

#include "Card.h"

#include <cstdint>
#include <random>

namespace tarock {

struct DeckSpec {
    std::uint8_t tarocks = 22;
    std::uint8_t suitSlots = 8;   // 8 = Königrufen (54 cards), 5 = Hungarian (42 cards)

    CardSet cards() const;
    int size() const { return tarocks + 4 * suitSlots; }
    bool contains(Card card) const;
};

// Deals `packets` in the given order, e.g. {6, 6} for Königrufen's two rounds
// of six, and hands the talon out separately.
struct DealPlan {
    std::vector<int> packets;      // cards per round and player
    int talonSize = 0;
    int talonAfterPacket = 0;      // talon is laid aside after this packet (0 = first)
};

struct Deal {
    std::vector<CardList> hands;   // one per seat, in dealing order
    CardList talon;
};

CardList shuffled(const DeckSpec& deck, std::mt19937& rng);
Deal deal(const DeckSpec& deck, const DealPlan& plan, int seats, std::mt19937& rng);
Deal dealFrom(const CardList& shuffledDeck, const DealPlan& plan, int seats);

} // namespace tarock
