#include "Scoring.h"

#include <algorithm>
#include <cstdlib>

namespace tarock {
namespace {

// The seats that take part in this hand and how they are grouped. At a
// five-player table the dealer sits out and appears in neither group, which is
// why nothing here counts "three opponents" by a constant.
struct Seating {
    std::vector<int> declarerSide;
    std::vector<int> defenderSide;

    bool alone() const { return declarerSide.size() == 1; }
};

Seating seatingOf(const HandResult& hand)
{
    Seating seating;
    // After a concession nobody knows who the partner would have been, and the
    // declarer pays all three others on his own (§7.7 a).
    const int partner = hand.conceded ? -1 : hand.partner;
    for (int seat = 0; seat < hand.players && seat < kMaxSeats; ++seat) {
        if (seat == hand.sittingOut)
            continue;
        if (seat == hand.declarer || seat == partner)
            seating.declarerSide.push_back(seat);
        else
            seating.defenderSide.push_back(seat);
    }
    return seating;
}

const Announcement* findAnnouncement(const HandResult& hand, BonusId bonus)
{
    for (const Announcement& announcement : hand.announcements) {
        if (announcement.bonus == bonus)
            return &announcement;
    }
    return nullptr;
}

const CardSet& cardsOf(const HandResult& hand, Party party)
{
    return party == Party::Declarer ? hand.declarerCards : hand.defenderCards;
}

bool holdsAll(const CardSet& cards, const CardList& wanted)
{
    for (Card card : wanted) {
        if (!contains(cards, card))
            return false;
    }
    return true;
}

// The party that won every single trick, or Neutral. A Valat only exists once
// all twelve tricks are on the table.
Party valatParty(const HandResult& hand, const RuleProfile& profile)
{
    if (static_cast<int>(hand.tricks.size()) != profile.tricks() || hand.tricks.empty())
        return Party::Neutral;
    const Party first = partyOfSeat(hand, hand.tricks.front().winner);
    if (first == Party::Neutral)
        return Party::Neutral;
    for (const TrickResult& trick : hand.tricks) {
        if (partyOfSeat(hand, trick.winner) != first)
            return Party::Neutral;
    }
    return first;
}

int tricksWonBy(const HandResult& hand, int seat)
{
    int count = 0;
    for (const TrickResult& trick : hand.tricks) {
        if (trick.winner == seat)
            ++count;
    }
    return count;
}

Posten makePosten(PostenType type, Party winner, int value, int kontra)
{
    Posten posten;
    posten.type = type;
    posten.winner = winner;
    posten.value = value;
    posten.kontra = kontra;
    posten.thirds = 3 * value;
    posten.achieved = true;
    return posten;
}

// --- Trischaken (§7.6) ----------------------------------------------------
//
// Everyone plays for himself, the loser is whoever holds the most points, and
// the pot can be split between two "Jungfrauen". Amounts stay in thirds here so
// that a split is exact; only the display rounds.
void buildTrischaken(const HandResult& hand, const RuleProfile& profile,
                     std::vector<Posten>& items)
{
    std::vector<int> active;
    for (int seat = 0; seat < hand.players && seat < kMaxSeats; ++seat) {
        if (seat != hand.sittingOut)
            active.push_back(seat);
    }
    if (active.empty())
        return;

    std::array<int, kMaxSeats> units{};
    std::array<int, kMaxSeats> amount{};
    std::vector<int> losers;
    std::vector<int> maidens;
    int best = -1;
    for (int seat : active) {
        units[seat] = profile.count(hand.seatCards[seat]).units;
        best = std::max(best, units[seat]);
        // "Jungfrau" = no trick. When no tricks were handed in (tests, lessons)
        // the empty card set says the same thing.
        const bool maiden = hand.tricks.empty() ? hand.seatCards[seat].none()
                                                : tricksWonBy(hand, seat) == 0;
        if (maiden)
            maidens.push_back(seat);
    }
    for (int seat : active) {
        if (units[seat] == best)
            losers.push_back(seat);
    }

    const int trischaker = hand.forehand >= 0 ? hand.forehand : hand.declarer;
    const bool trischakerLoses =
        std::find(losers.begin(), losers.end(), trischaker) != losers.end();

    // Hands the pot to the receivers, exactly, in thirds.
    const auto share = [&amount](int potThirds, const std::vector<int>& receivers) {
        if (receivers.empty())
            return;
        const int each = potThirds / static_cast<int>(receivers.size());
        int rest = potThirds - each * static_cast<int>(receivers.size());
        for (int seat : receivers) {
            amount[seat] += each;
            if (rest > 0) {   // the rules never leave one over; stay zero-sum if they do
                ++amount[seat];
                --rest;
            }
        }
    };

    if (trischakerLoses) {
        // The Trischaker loses alone and pays double, four times as "Bürgermeister".
        const int base = units[trischaker] >= profile.winThreshold() ? 4 : 2;
        const int pot = 9 * base;
        amount[trischaker] -= pot;
        std::vector<int> receivers = maidens;
        if (receivers.empty()) {
            for (int seat : active) {
                if (seat != trischaker)
                    receivers.push_back(seat);
            }
        }
        share(pot, receivers);
    } else if (losers.size() == 1) {
        const int loser = losers.front();
        const int base = units[loser] >= profile.winThreshold() ? 2 : 1;
        int pot = 9 * base;
        if (!maidens.empty()) {
            // Three points cannot be halved, so two Jungfrauen are paid two
            // each [OÖ23 Trischaken 3].
            if (maidens.size() == 2 && base == 1)
                pot = 12;
            amount[loser] -= pot;
            share(pot, maidens);
        } else {
            amount[loser] -= pot;
            for (int seat : active) {
                if (seat != loser)
                    amount[seat] += 3 * base;
            }
        }
    } else if (losers.size() == 2) {
        std::vector<int> receivers;
        for (int seat : active) {
            if (std::find(losers.begin(), losers.end(), seat) == losers.end())
                receivers.push_back(seat);
        }
        for (int seat : losers)
            amount[seat] -= 6;
        // A single Jungfrau among the two takes all four points (§9.6 T-2).
        std::vector<int> single;
        for (int seat : receivers) {
            if (std::find(maidens.begin(), maidens.end(), seat) != maidens.end())
                single.push_back(seat);
        }
        share(12, single.size() == 1 ? single : receivers);
    } else if (losers.size() == 3) {
        for (int seat : losers)
            amount[seat] -= 3;
        for (int seat : active) {
            if (std::find(losers.begin(), losers.end(), seat) == losers.end())
                amount[seat] += 9;
        }
    }

    for (int seat : active) {
        if (amount[seat] == 0)
            continue;
        Posten posten;
        posten.type = PostenType::Trischaken;
        posten.owner = Party::Neutral;
        posten.winner = Party::Neutral;
        posten.achieved = amount[seat] > 0;
        posten.value = pointsFromThirds(amount[seat]);
        posten.thirds = amount[seat];
        posten.againstSeat = static_cast<std::int8_t>(seat);
        items.push_back(posten);
    }
}

// --- negative contracts (§7.5) --------------------------------------------
void buildNegative(const HandResult& hand, const RuleProfile& profile, const ContractDef& contract,
                   std::vector<Posten>& items)
{
    const int target =
        (contract.id == ContractId::Piccolo || contract.id == ContractId::PiccoloOuvert) ? 1 : 0;
    // The play only stops early when the declarer has already failed (§6.8), so
    // a win needs the full twelve tricks.
    const bool won = tricksWonBy(hand, hand.declarer) == target
                     && static_cast<int>(hand.tricks.size()) == profile.tricks();
    const Seating seating = seatingOf(hand);
    for (int opponent : seating.defenderSide) {
        Posten posten = makePosten(PostenType::Game, won ? Party::Declarer : Party::Defenders,
                                   contract.baseValue,
                                   hand.kontraVoid ? 1 : hand.seatKontra[opponent]);
        posten.achieved = won;
        posten.againstSeat = static_cast<std::int8_t>(opponent);
        items.push_back(posten);
    }
}

} // namespace

Party otherParty(Party party)
{
    switch (party) {
    case Party::Declarer: return Party::Defenders;
    case Party::Defenders: return Party::Declarer;
    default: return Party::Neutral;
    }
}

Party partyOfSeat(const HandResult& hand, int seat)
{
    if (seat < 0 || seat >= hand.players || seat == hand.sittingOut)
        return Party::Neutral;
    if (seat == hand.declarer || (!hand.conceded && seat == hand.partner))
        return Party::Declarer;
    return Party::Defenders;
}

int pointsFromThirds(int thirds)
{
    // Nearest point; thirds never land exactly between two points.
    const int magnitude = (std::abs(thirds) + 1) / 3;
    return thirds < 0 ? -magnitude : magnitude;
}

int bonusValue(const RuleProfile& profile, const ContractDef& contract, BonusId bonus,
               bool announced)
{
    const BonusDef& def = profile.bonus(bonus);
    int value = announced ? def.announced : def.silent;
    // "Alle Prämien zählen still die Hälfte, bei den Solospielen doppelt"
    // [OÖ23 Ansagen 1]; the Valat is not doubled again (§9.4 V-6) and never
    // travels through here.
    if (contract.soloDoublesBonuses)
        value *= 2;
    return value;
}

bool bonusAchievedBy(const HandResult& hand, BonusId bonus, Party party)
{
    if (party == Party::Neutral)
        return false;
    const RuleProfile& profile = RuleProfile::get(hand.profile);
    const BonusDef& def = profile.bonus(bonus);

    switch (def.kind) {
    case BonusKind::Holding: {
        // Where the cards lie at the end decides; tricks, discards and the
        // talon share count the same (§5.2).
        const CardSet& cards = cardsOf(hand, party);
        if (bonus == BonusId::Trull)
            return holdsAll(cards, {kSkues, kMond, kPagat});
        if (bonus == BonusId::AllKings)
            return holdsAll(cards, {suitCard(Suit::Heart, 0), suitCard(Suit::Diamond, 0),
                                    suitCard(Suit::Spade, 0), suitCard(Suit::Club, 0)});
        return false;
    }
    case BonusKind::TrickWithCard: {
        const int index = def.targetTrick - 1;
        if (index < 0 || index >= static_cast<int>(hand.tricks.size()))
            return false;
        const TrickResult& trick = hand.tricks[static_cast<std::size_t>(index)];
        if (bonus == BonusId::KingUltimo) {
            // The called king lies in the last trick and the declarer's party
            // takes it; here the partner may do the striking (§5.2).
            return party == Party::Declarer && hand.calledKing.valid()
                   && contains(trick.cards, hand.calledKing)
                   && partyOfSeat(hand, trick.winner) == Party::Declarer;
        }
        // A bird has to take its trick itself, so the winning card is the bird.
        return def.boundCard.valid() && trick.winningCard == def.boundCard
               && partyOfSeat(hand, trick.winner) == party;
    }
    case BonusKind::AllTricks:
        return valatParty(hand, profile) == party;
    default:
        return false;
    }
}

std::vector<Posten> buildPosten(const HandResult& hand)
{
    const RuleProfile& profile = RuleProfile::get(hand.profile);
    const ContractDef& contract = profile.contract(hand.contract);
    std::vector<Posten> items;

    if (contract.play == PlayMode::Trischaken) {
        buildTrischaken(hand, profile, items);
        return items;
    }

    if (hand.conceded) {
        // Schleifen: the declarer pays game plus the bird he had announced, to
        // all three others, and no kontra applies (§7.7 a).
        Posten game = makePosten(PostenType::Concede, Party::Defenders, contract.baseValue, 1);
        game.achieved = false;
        items.push_back(game);
        if (hand.concededBird != BonusId::None) {
            Posten bird = makePosten(PostenType::Concede, Party::Defenders,
                                     profile.bonus(hand.concededBird).announced, 1);
            bird.bonus = hand.concededBird;
            bird.owner = Party::Declarer;
            bird.announced = true;
            bird.achieved = false;
            items.push_back(bird);
        }
        return items;
    }

    if (contract.play == PlayMode::Negative) {
        buildNegative(hand, profile, contract, items);
        return items;
    }

    const int gameKontra = hand.kontraVoid ? 1 : hand.gameKontra;
    // Only the Sechserdreier is worth more when lost; §7.4 applies the same
    // factor to a Valat that the declarer's party loses.
    const int lostFactor =
        contract.baseValue > 0 ? contract.lostValue / contract.baseValue : 1;

    const Announcement* valat = findAnnouncement(hand, BonusId::Valat);
    const Party allTricks = valatParty(hand, profile);
    bool valatCounts = false;

    if (valat != nullptr) {
        // An announced Valat replaces the game: made it is eight times the game
        // value, missed it the announcing party pays the same (§7.4, §9.4 V-2,
        // V-5). A kontra on the game carries over to it (§9.4 V-4).
        const bool made = allTricks == valat->party;
        Posten posten = makePosten(PostenType::Valat, made ? valat->party : otherParty(valat->party),
                                   contract.baseValue * 8,
                                   hand.kontraVoid ? 1 : std::max(valat->kontra, hand.gameKontra));
        if (posten.winner == Party::Defenders)
            posten.value *= lostFactor;
        posten.bonus = BonusId::Valat;
        posten.owner = valat->party;
        posten.announced = true;
        posten.achieved = made;
        posten.thirds = 3 * posten.value;
        items.push_back(posten);
        valatCounts = true;
    } else if (allTricks != Party::Neutral) {
        Posten posten = makePosten(PostenType::Valat, allTricks, contract.baseValue * 4, gameKontra);
        if (posten.winner == Party::Defenders)
            posten.value *= lostFactor;
        posten.bonus = BonusId::Valat;
        posten.thirds = 3 * posten.value;
        items.push_back(posten);
        valatCounts = true;
    } else {
        const bool won = profile.count(hand.declarerCards).units >= profile.winThreshold();
        Posten posten = makePosten(PostenType::Game, won ? Party::Declarer : Party::Defenders,
                                   won ? contract.baseValue : contract.lostValue, gameKontra);
        posten.achieved = won;
        items.push_back(posten);
    }

    for (const BonusDef& def : profile.bonuses()) {
        if (def.id == BonusId::Valat || (contract.bonusMask & bonusBit(def.id)) == 0)
            continue;
        const Announcement* announcement = findAnnouncement(hand, def.id);
        if (announcement != nullptr) {
            // An announced bonus that is missed is paid to the other party.
            const bool made = bonusAchievedBy(hand, def.id, announcement->party);
            Posten posten = makePosten(PostenType::Bonus,
                                       made ? announcement->party : otherParty(announcement->party),
                                       bonusValue(profile, contract, def.id, true),
                                       hand.kontraVoid ? 1 : announcement->kontra);
            posten.bonus = def.id;
            posten.owner = announcement->party;
            posten.announced = true;
            posten.achieved = made;
            posten.thirds = 3 * posten.value;
            items.push_back(posten);
            continue;
        }
        // Silent bonuses only count when they succeed, and next to a Valat they
        // do not count at all (§5.5, §7.4, §9.3 P-5).
        if (valatCounts)
            continue;
        Party party = Party::Neutral;
        if (bonusAchievedBy(hand, def.id, Party::Declarer))
            party = Party::Declarer;
        else if (bonusAchievedBy(hand, def.id, Party::Defenders))
            party = Party::Defenders;
        if (party == Party::Neutral)
            continue;
        Posten posten = makePosten(PostenType::Bonus, party,
                                   bonusValue(profile, contract, def.id, false), gameKontra);
        posten.bonus = def.id;
        items.push_back(posten);
    }
    return items;
}

Ledger book(const HandResult& hand, std::vector<Posten> items)
{
    const RuleProfile& profile = RuleProfile::get(hand.profile);
    const Seating seating = seatingOf(hand);
    Ledger ledger;

    for (const Posten& posten : items) {
        if (posten.type == PostenType::Trischaken) {
            // Already an amount for one seat; no kontra exists in Trischaken.
            const int seat = posten.againstSeat;
            if (seat < 0 || seat >= kMaxSeats)
                continue;
            ledger.schriftThirds[static_cast<std::size_t>(seat)] += posten.thirds;
            ledger.geldThirds[static_cast<std::size_t>(seat)] += posten.thirds;
            continue;
        }
        const int sign = posten.winner == Party::Declarer ? 1 : -1;
        const int value = sign * posten.thirds;
        if (posten.againstSeat >= 0) {
            // Negative contracts: one settlement per opponent, with his own
            // kontra level (§7.5).
            const std::size_t opponent = static_cast<std::size_t>(posten.againstSeat);
            const std::size_t declarer = static_cast<std::size_t>(hand.declarer);
            ledger.schriftThirds[declarer] += value;
            ledger.geldThirds[declarer] += value * posten.kontra;
            ledger.schriftThirds[opponent] -= value;
            ledger.geldThirds[opponent] -= value * posten.kontra;
            continue;
        }
        if (seating.alone()) {
            // One against three: the declarer moves the whole pot (§7.1).
            const std::size_t declarer = static_cast<std::size_t>(seating.declarerSide.front());
            const int total = value * static_cast<int>(seating.defenderSide.size());
            ledger.schriftThirds[declarer] += total;
            ledger.geldThirds[declarer] += total * posten.kontra;
            for (int opponent : seating.defenderSide) {
                ledger.schriftThirds[static_cast<std::size_t>(opponent)] -= value;
                ledger.geldThirds[static_cast<std::size_t>(opponent)] -= value * posten.kontra;
            }
            continue;
        }
        for (int seat : seating.declarerSide) {
            ledger.schriftThirds[static_cast<std::size_t>(seat)] += value;
            ledger.geldThirds[static_cast<std::size_t>(seat)] += value * posten.kontra;
        }
        for (int seat : seating.defenderSide) {
            ledger.schriftThirds[static_cast<std::size_t>(seat)] -= value;
            ledger.geldThirds[static_cast<std::size_t>(seat)] -= value * posten.kontra;
        }
    }

    for (std::size_t seat = 0; seat < kMaxSeats; ++seat) {
        ledger.schrift[seat] = pointsFromThirds(ledger.schriftThirds[seat]);
        ledger.geld[seat] = pointsFromThirds(ledger.geldThirds[seat]);
    }

    ledger.declarerCards = profile.count(hand.declarerCards);
    ledger.defenderCards = profile.count(hand.defenderCards);
    for (std::size_t seat = 0; seat < kMaxSeats; ++seat)
        ledger.seatCards[seat] = profile.count(hand.seatCards[seat]);
    ledger.declarerWon = ledger.declarerCards.units >= profile.winThreshold();
    ledger.items = std::move(items);
    return ledger;
}

Ledger settle(const HandResult& hand)
{
    return book(hand, buildPosten(hand));
}

bool Ledger::zeroSum() const
{
    int schriftSum = 0;
    int geldSum = 0;
    int schriftThirdsSum = 0;
    int geldThirdsSum = 0;
    for (std::size_t seat = 0; seat < kMaxSeats; ++seat) {
        schriftSum += schrift[seat];
        geldSum += geld[seat];
        schriftThirdsSum += schriftThirds[seat];
        geldThirdsSum += geldThirds[seat];
    }
    return schriftSum == 0 && geldSum == 0 && schriftThirdsSum == 0 && geldThirdsSum == 0;
}

} // namespace tarock
