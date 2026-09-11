#include "Deck.h"

#include <algorithm>
#include <stdexcept>

namespace tarock {

CardSet DeckSpec::cards() const
{
    CardSet set;
    for (int i = 0; i < tarocks; ++i)
        set.set(static_cast<std::size_t>(i));
    for (int suit = 0; suit < 4; ++suit) {
        for (int slot = 0; slot < suitSlots; ++slot)
            set.set(static_cast<std::size_t>(22 + 8 * suit + slot));
    }
    return set;
}

bool DeckSpec::contains(Card card) const
{
    if (!card.valid())
        return false;
    if (card.tarock())
        return card.id < tarocks;
    return card.slot() < suitSlots;
}

CardList shuffled(const DeckSpec& deck, std::mt19937& rng)
{
    CardList cards = toList(deck.cards());
    std::shuffle(cards.begin(), cards.end(), rng);
    return cards;
}

Deal dealFrom(const CardList& shuffledDeck, const DealPlan& plan, int seats)
{
    Deal result;
    result.hands.assign(static_cast<std::size_t>(seats), CardList());
    std::size_t next = 0;
    auto take = [&]() {
        if (next >= shuffledDeck.size())
            throw std::runtime_error("deck exhausted while dealing");
        return shuffledDeck[next++];
    };
    for (std::size_t round = 0; round < plan.packets.size(); ++round) {
        if (plan.talonSize > 0 && static_cast<int>(round) == plan.talonAfterPacket) {
            for (int i = 0; i < plan.talonSize; ++i)
                result.talon.push_back(take());
        }
        for (int seat = 0; seat < seats; ++seat) {
            for (int i = 0; i < plan.packets[round]; ++i)
                result.hands[static_cast<std::size_t>(seat)].push_back(take());
        }
    }
    if (plan.talonSize > 0 && plan.talonAfterPacket >= static_cast<int>(plan.packets.size())) {
        for (int i = 0; i < plan.talonSize; ++i)
            result.talon.push_back(take());
    }
    return result;
}

Deal deal(const DeckSpec& deck, const DealPlan& plan, int seats, std::mt19937& rng)
{
    return dealFrom(shuffled(deck, rng), plan, seats);
}

} // namespace tarock
