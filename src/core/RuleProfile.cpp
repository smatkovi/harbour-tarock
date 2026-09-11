#include "RuleProfile.h"

#include <stdexcept>

namespace tarock {
const RuleProfile& makeKoenigrufen();
const RuleProfile& makeHungarian();
}

namespace tarock {

int CountResult::points() const
{
    // Austrian thirds are shown as whole points: three thirds make one point.
    return units / 3;
}

int RuleProfile::cardValue(Card card) const
{
    if (!m_deck.contains(card))
        return 0;
    if (card.king() || card.honour())
        return 5;
    if (card.tarock())
        return 1;
    switch (card.slot()) {
    case 1: return 4;  // queen
    case 2: return 3;  // knight
    case 3: return 2;  // jack
    default: return 1;
    }
}

CountResult RuleProfile::count(const CardSet& cards) const
{
    CountResult result;
    int sum = 0;
    for (int id = 0; id < 54; ++id) {
        const Card card(static_cast<std::uint8_t>(id));
        if (!cards.test(static_cast<std::size_t>(id)) || !m_deck.contains(card))
            continue;
        sum += cardValue(card);
        ++result.cards;
    }
    // Austrian counting in thirds: every card carries a third less than the
    // rounded value players call out, so three cards "lose" one point.
    result.units = m_countMode == CountMode::ThirdsAustrian ? 3 * sum - 2 * result.cards : sum;
    return result;
}

const ContractDef& RuleProfile::contract(ContractId id) const
{
    for (const ContractDef& def : m_contracts) {
        if (def.id == id)
            return def;
    }
    throw std::runtime_error("unknown contract for this profile");
}

const BonusDef& RuleProfile::bonus(BonusId id) const
{
    for (const BonusDef& def : m_bonuses) {
        if (def.id == id)
            return def;
    }
    throw std::runtime_error("unknown bonus for this profile");
}

RuleProfile RuleProfileBuilder::build() const
{
    RuleProfile profile;
    profile.m_id = id;
    profile.m_key = key;
    profile.m_deck = deck;
    profile.m_countMode = countMode;
    profile.m_winThreshold = winThreshold;
    profile.m_totalUnits = totalUnits;
    profile.m_tricks = tricks;
    profile.m_handCards = handCards;
    profile.m_talonSize = talonSize;
    profile.m_dealPlan = dealPlan;
    profile.m_contracts = contracts;
    profile.m_bonuses = bonuses;
    return profile;
}

const RuleProfile& RuleProfile::get(ProfileId id)
{
    switch (id) {
    case ProfileId::AtKrOoe2023: return makeKoenigrufen();
    case ProfileId::HuIlluItvb2019: return makeHungarian();
    }
    return makeKoenigrufen();
}

} // namespace tarock
