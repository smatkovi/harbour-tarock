#include "TarockCore.h"

#include "Deck.h"

#include <algorithm>
#include <iterator>
#include <sstream>

namespace tarock {
namespace {

std::uint64_t checksum(const std::string& value)
{
    std::uint64_t hash = UINT64_C(14695981039346656037);
    for (unsigned char byte : value) {
        hash ^= byte;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

Reason refuse(ReasonCode code)
{
    Reason reason;
    reason.code = code;
    return reason;
}

Reason refuse(ReasonCode code, Card card)
{
    Reason reason;
    reason.code = code;
    reason.card = card;
    return reason;
}

} // namespace

// --- construction and dealing ---------------------------------------------------

TarockCore::TarockCore(ProfileId profile, int players, std::uint32_t seed, const FlagSet& flags)
{
    newMatch(profile, players, seed, flags);
}

void TarockCore::newMatch(ProfileId profile, int players, std::uint32_t seed, const FlagSet& flags)
{
    m_profile = &RuleProfile::get(profile);
    m_players = players == 5 ? 5 : 4;
    m_flags = flags;
    m_rng.seed(seed);
    m_hand = 0;
    m_dealer = static_cast<int>(m_rng() % static_cast<unsigned>(m_players));
    startHand();
}

bool TarockCore::nextHand()
{
    if (m_phase != Phase::HandOver && m_phase != Phase::Scoring)
        return false;
    m_dealer = (m_dealer + 1) % m_players;
    ++m_hand;
    startHand();
    return true;
}

int TarockCore::forehand() const
{
    // At a table of five the dealer sits out, so the forehand is two seats on.
    return nextSeat(m_dealer);
}

int TarockCore::nextSeat(int seat) const
{
    int result = (seat + 1) % m_players;
    if (result == sittingOut())
        result = (result + 1) % m_players;
    return result;
}

// The next seat that still bids. When everybody has passed the turn stays on
// the seat that holds the bid, or comes back to the forehand, who must name a
// game (§3.3.3); either way the walk ends after one round of the table.
int TarockCore::nextBidder(int seat) const
{
    int next = nextSeat(seat);
    for (int step = 0; step < m_players && hasPassed(next) && next != m_bidHolder; ++step)
        next = nextSeat(next);
    return hasPassed(next) && next != m_bidHolder ? forehand() : next;
}

void TarockCore::startHand()
{
    for (int seat = 0; seat < MaxSeats; ++seat) {
        m_hands[static_cast<std::size_t>(seat)].reset();
        m_won[static_cast<std::size_t>(seat)].reset();
        m_tray[static_cast<std::size_t>(seat)].reset();
        m_discards[static_cast<std::size_t>(seat)].reset();
    }
    m_talonHalf[0].reset();
    m_talonHalf[1].reset();
    m_talonToDefenders.reset();
    m_talonOpen = false;
    m_talonTaken = false;
    m_trick.clear();
    m_trickNumber = 0;
    m_trickWinners.fill(-1);
    for (CardList& cards : m_trickCards)
        cards.clear();
    m_seatKontra.fill(1);
    m_ledger = Ledger();
    m_passed = 0;
    m_bid = ContractId::None;
    m_bidHolder = -1;
    m_forehandSpoke = false;
    m_contract = ContractId::None;
    m_declarer = -1;
    m_partner = -1;
    m_partnerKnown = false;
    m_calledSuit = -1;
    m_calledFourthKing = false;
    m_conceded = false;
    m_declarations.clear();
    m_announceReady = 0;

    const DealPlan& plan = profile().dealPlan();
    Deal dealt = deal(profile().deck(), plan, activePlayers(), m_rng);
    int index = 0;
    for (int i = 0; i < activePlayers(); ++i) {
        const int seat = (forehand() + i) % m_players == sittingOut()
                ? (forehand() + i + 1) % m_players : (forehand() + i) % m_players;
        m_hands[static_cast<std::size_t>(seat)] = toSet(dealt.hands[static_cast<std::size_t>(index++)]);
    }
    for (std::size_t i = 0; i < dealt.talon.size(); ++i)
        m_talonHalf[i < 3 ? 0 : 1].set(dealt.talon[i].id);

    beginBidding();
}

void TarockCore::dealFixed(const std::vector<CardList>& hands, const CardList& talon, int dealer)
{
    m_dealer = dealer % m_players;
    startHand();
    for (int seat = 0; seat < MaxSeats; ++seat)
        m_hands[static_cast<std::size_t>(seat)].reset();
    m_talonHalf[0].reset();
    m_talonHalf[1].reset();
    for (std::size_t seat = 0; seat < hands.size() && seat < MaxSeats; ++seat)
        m_hands[seat] = toSet(hands[seat]);
    for (std::size_t i = 0; i < talon.size(); ++i)
        m_talonHalf[i < 3 ? 0 : 1].set(talon[i].id);
    beginBidding();
}

void TarockCore::setPhase(Phase phase)
{
    m_phase = phase;
}

// --- bidding --------------------------------------------------------------------

void TarockCore::beginBidding()
{
    setPhase(Phase::Bidding);
    m_turn = forehand();
}

namespace {

// Which contracts a hand allows: a caller may not hold all four kings, and the
// Besserrufer needs one of the four birds (docs/koenigrufen.md §3.7).
bool handAllowsContract(const RuleProfile& profile, const CardSet& hand, ContractId id)
{
    const ContractDef& def = profile.contract(id);
    if ((def.bidFlags & ForbidsFourKings) || def.partner == PartnerMode::CallKing) {
        int kings = 0;
        for (int suit = 0; suit < 4; ++suit)
            kings += contains(hand, suitCard(Suit(suit), 0)) ? 1 : 0;
        if (kings == 4)
            return false;
    }
    if (def.bidFlags & RequiresBird) {
        bool bird = false;
        for (int i = 0; i < 4; ++i)
            bird = bird || contains(hand, tarockCard(i + 1));
        if (!bird)
            return false;
    }
    return true;
}

} // namespace

std::vector<Action> TarockCore::legalActions(int seat) const
{
    std::vector<Action> actions;
    if (!active(seat))
        return actions;

    switch (m_phase) {
    case Phase::Bidding: {
        if (seat != m_turn)
            break;
        const bool isForehand = seat == forehand();
        const int currentRank = m_bid == ContractId::None ? 0 : profile().contract(m_bid).rank;
        for (const ContractDef& def : profile().contracts()) {
            if (def.bidFlags & ForehandOnly) {
                if (!isForehand)
                    continue;
                // "vorneweg" only as the opening bid, "hintennach" only when
                // everybody else has passed after the forehand spoke.
                if ((def.bidFlags & ForehandFirstOnly) && m_forehandSpoke)
                    continue;
                if ((def.bidFlags & ForehandLastOnly)
                    && !(m_forehandSpoke && m_bid == ContractId::None))
                    continue;
            } else if ((def.bidFlags & ForehandFirstOnly) || (def.bidFlags & ForehandLastOnly)) {
                continue;
            }
            if (!(def.bidFlags & ForehandOnly) && def.rank <= currentRank)
                continue;
            if (!handAllowsContract(profile(), hand(seat), def.id))
                continue;
            actions.push_back(Action(ActionType::Bid, static_cast<std::int16_t>(def.id)));
        }
        if (isForehand && !m_forehandSpoke && m_bid == ContractId::None)
            actions.push_back(Action(ActionType::OpenForehand));
        if (isForehand && m_bid != ContractId::None && m_bidHolder != seat)
            actions.push_back(Action(ActionType::Hold));
        // Die Vorhand darf nie passen, solange kein Spiel geboten ist: weder
        // beim ersten Sprechen noch, wenn alle anderen „weiter“ gesagt haben
        // (§3.3.1 und §3.3.3, Endebedingung B).
        if (!(isForehand && m_bid == ContractId::None))
            actions.push_back(Action(ActionType::Pass));
        break;
    }
    case Phase::Call: {
        if (seat != m_turn)
            break;
        int kings = 0;
        for (int suit = 0; suit < 4; ++suit)
            kings += contains(hand(seat), suitCard(Suit(suit), 0)) ? 1 : 0;
        for (int suit = 0; suit < 4; ++suit) {
            if (contains(hand(seat), suitCard(Suit(suit), 0)))
                continue;   // never call a king you hold yourself
            actions.push_back(Action(ActionType::CallKing, static_cast<std::int16_t>(suit)));
        }
        if (kings == 3)
            actions.push_back(Action(ActionType::CallKing, -1, 1));  // "the fourth king"
        break;
    }
    case Phase::Talon: {
        if (seat != m_turn)
            break;
        const ContractDef& def = contractDef();
        if (def.talon == TalonMode::OpenHalves) {
            actions.push_back(Action(ActionType::TakeTalon, 0));
            actions.push_back(Action(ActionType::TakeTalon, 1));
            // The called king lying in the talon lets the declarer give up.
            if (def.partner == PartnerMode::CallKing && m_calledSuit >= 0) {
                const Card king = calledKing();
                if (contains(m_talonHalf[0], king) || contains(m_talonHalf[1], king))
                    actions.push_back(Action(ActionType::Concede));
            }
        } else if (def.talon == TalonMode::AllHidden) {
            actions.push_back(Action(ActionType::TakeTalon, -1));
        }
        break;
    }
    case Phase::Discard: {
        if (seat != m_turn)
            break;
        const CardSet& tray = m_tray[static_cast<std::size_t>(seat)];
        const bool trayFull = static_cast<int>(tray.count()) >= contractDef().discardCount;
        for (Card card : toList(hand(seat))) {
            // Taking a card back out is always possible; putting one in only
            // while there is room. A tarock that has to be shown openly is
            // allowed, so judge by severity, not by "no reason at all".
            if (contains(tray, card)
                || (!trayFull && severityOf(cardReason(seat, card).code) != Severity::Error))
                actions.push_back(Action(ActionType::Discard, card.id));
        }
        if (static_cast<int>(tray.count()) == contractDef().discardCount)
            actions.push_back(Action(ActionType::ConfirmDiscard));
        break;
    }
    case Phase::Announce: {
        if (seat != m_turn || ((m_announceReady >> seat) & 1))
            break;
        const ContractDef& def = contractDef();
        for (const BonusDef& bonus : profile().bonuses()) {
            if (!(def.bonusMask & bonusBit(bonus.id)))
                continue;
            if (!check(seat, Action(ActionType::AnnounceBonus, static_cast<std::int16_t>(bonus.id))).ok())
                continue;
            actions.push_back(Action(ActionType::AnnounceBonus, static_cast<std::int16_t>(bonus.id)));
        }
        for (std::size_t i = 0; i < m_declarations.size(); ++i) {
            const Action kontra(ActionType::Kontra, static_cast<std::int16_t>(i));
            if (check(seat, kontra).ok())
                actions.push_back(kontra);
        }
        actions.push_back(Action(ActionType::Ready));
        break;
    }
    case Phase::Play: {
        if (seat != m_turn)
            break;
        for (Card card : toList(playableCards(seat)))
            actions.push_back(Action(ActionType::PlayCard, card.id));
        break;
    }
    default:
        break;
    }
    return actions;
}

// --- trick rules ------------------------------------------------------------------

namespace {

// In the suit games tarocks do not beat suit cards; everywhere else they do.
bool beats(const ContractDef& def, Card candidate, Card best)
{
    const bool suitGame = def.play == PlayMode::SuitGame;
    if (candidate.tarock() && best.tarock())
        return candidate.tarockNo() > best.tarockNo();
    if (candidate.tarock())
        return !suitGame;
    if (best.tarock())
        return false;
    return candidate.suit() == best.suit() && power(candidate) > power(best);
}

int winnerIndex(const ContractDef& def, const CardList& trick)
{
    int best = 0;
    for (int i = 1; i < static_cast<int>(trick.size()); ++i) {
        if (beats(def, trick[static_cast<std::size_t>(i)], trick[static_cast<std::size_t>(best)]))
            best = i;
    }
    return best;
}

bool hasSuit(const CardSet& hand, Suit suit)
{
    for (Card card : toList(hand)) {
        if (card.suit() == suit)
            return true;
    }
    return false;
}

bool hasTarock(const CardSet& hand)
{
    return hasSuit(hand, Suit::Tarock);
}

} // namespace

CardSet TarockCore::playableCards(int seat) const
{
    CardSet result;
    if (m_phase != Phase::Play || seat != m_turn)
        return result;
    for (Card card : toList(hand(seat))) {
        if (cardReason(seat, card).ok())
            result.set(card.id);
    }
    return result;
}

Reason TarockCore::cardReason(int seat, Card card) const
{
    if (m_phase == Phase::Discard) {
        // Discarding: kings and the Trull never go down, and a tarock only
        // when nothing else is left (docs/koenigrufen.md §4.6).
        if (!contains(hand(seat), card))
            return refuse(ReasonCode::DiscardNotHeld, card);
        if (card.king())
            return refuse(ReasonCode::DiscardKing, card);
        if (card.honour())
            return refuse(ReasonCode::DiscardTrull, card);
        if (card.tarock()) {
            CardSet rest = hand(seat);
            for (Card inTray : toList(m_tray[static_cast<std::size_t>(seat)]))
                remove(rest, inTray);
            for (Card other : toList(rest)) {
                if (!other.tarock() && !other.king())
                    return refuse(ReasonCode::DiscardTarockWhileSuit, card);
            }
            Reason info = refuse(ReasonCode::I_DiscardShownOpen, card);
            return info;   // allowed, but the tarock has to be shown
        }
        return Reason();
    }

    if (m_phase != Phase::Play)
        return refuse(ReasonCode::WrongPhase, card);
    if (seat != m_turn)
        return refuse(ReasonCode::NotYourTurn, card);
    if (!contains(hand(seat), card))
        return refuse(ReasonCode::NotYourCard, card);

    const ContractDef& def = contractDef();
    const CardSet& own = hand(seat);
    const bool negative = def.play == PlayMode::Negative || def.play == PlayMode::Trischaken;

    // The Pagat may only be played when it is the last tarock in hand.
    if (negative && card == kPagat) {
        for (Card other : toList(own)) {
            if (other.tarock() && other != kPagat)
                return refuse(ReasonCode::PagatNotLastTarock, card);
        }
    }

    if (m_trick.empty()) {
        // In the suit games tarock may only be led once no suit card is left.
        if (def.play == PlayMode::SuitGame && card.tarock()) {
            for (Card other : toList(own)) {
                if (!other.tarock())
                    return refuse(ReasonCode::SuitGameNoTarockLead, card);
            }
        }
        return Reason();
    }

    const Suit lead = m_trick.front().suit();
    const bool holdsLead = hasSuit(own, lead);
    if (holdsLead && card.suit() != lead)
        return refuse(lead == Suit::Tarock ? ReasonCode::FollowTarock : ReasonCode::FollowSuit, card);
    if (!holdsLead && hasTarock(own) && !card.tarock())
        return refuse(ReasonCode::MustTarock, card);

    if (negative) {
        // Negative games: beat the highest card of the trick whenever the
        // suit rules leave you a card that can.
        const Card best = m_trick[static_cast<std::size_t>(winnerIndex(def, m_trick))];
        CardSet candidates;
        for (Card other : toList(own)) {
            const bool followsRule = holdsLead ? other.suit() == lead
                                               : (hasTarock(own) ? other.tarock() : true);
            if (!followsRule)
                continue;
            if (negative && other == kPagat) {
                bool lastTarock = true;
                for (Card rest : toList(own))
                    lastTarock = lastTarock && (!rest.tarock() || rest == kPagat);
                if (!lastTarock)
                    continue;   // the Pagat may not be used to overtake here
            }
            if (beats(def, other, best))
                candidates.set(other.id);
        }
        if (candidates.any() && !contains(candidates, card)) {
            Reason reason = refuse(holdsLead ? ReasonCode::MustOvertrumpSuit
                                             : ReasonCode::MustOvertrumpTarock, card);
            reason.cards = candidates;
            return reason;
        }
    }
    return Reason();
}

// --- checking and applying actions --------------------------------------------------

Reason TarockCore::check(int seat, const Action& action) const
{
    if (!active(seat))
        return refuse(ReasonCode::SeatSittingOut);

    switch (action.type) {
    case ActionType::Bid:
    case ActionType::Pass:
    case ActionType::Hold:
    case ActionType::OpenForehand: {
        if (m_phase != Phase::Bidding)
            return refuse(ReasonCode::WrongPhase);
        if (seat != m_turn)
            return refuse(ReasonCode::NotYourTurn);
        if (hasPassed(seat))
            return refuse(ReasonCode::AlreadyPassed);
        if (action.type == ActionType::Pass && seat == forehand() && m_bid == ContractId::None)
            return refuse(ReasonCode::ForehandMustSpeak);
        if (action.type == ActionType::Hold) {
            if (seat != forehand())
                return refuse(ReasonCode::HoldOnlyForehand);
            if (m_bid == ContractId::None || m_bidHolder == seat)
                return refuse(ReasonCode::HoldNotAllowed);
            return Reason();
        }
        if (action.type == ActionType::Bid) {
            const ContractId id = static_cast<ContractId>(action.a);
            const std::vector<Action> options = legalActions(seat);
            if (std::find(options.begin(), options.end(), action) == options.end()) {
                if (!handAllowsContract(profile(), hand(seat), id)) {
                    const ContractDef& def = profile().contract(id);
                    return refuse((def.bidFlags & RequiresBird) ? ReasonCode::BesserruferWithoutBird
                                                                : ReasonCode::CallerHasFourKings);
                }
                return refuse(ReasonCode::BidTooLow);
            }
        }
        return Reason();
    }
    case ActionType::CallKing: {
        if (m_phase != Phase::Call)
            return refuse(ReasonCode::WrongPhase);
        if (seat != m_turn)
            return refuse(ReasonCode::NotYourTurn);
        if (action.b == 1) {
            int kings = 0;
            for (int suit = 0; suit < 4; ++suit)
                kings += contains(hand(seat), suitCard(Suit(suit), 0)) ? 1 : 0;
            return kings == 3 ? Reason() : refuse(ReasonCode::FourthKingNeedsThree);
        }
        if (action.a < 0 || action.a > 3)
            return refuse(ReasonCode::UnknownAction);
        if (contains(hand(seat), suitCard(Suit(action.a), 0)))
            return refuse(ReasonCode::CallOwnKing, suitCard(Suit(action.a), 0));
        return Reason();
    }
    case ActionType::TakeTalon:
    case ActionType::Concede: {
        if (m_phase != Phase::Talon)
            return refuse(ReasonCode::WrongPhase);
        if (seat != m_turn)
            return refuse(ReasonCode::NotYourTurn);
        const std::vector<Action> options = legalActions(seat);
        return std::find(options.begin(), options.end(), action) == options.end()
                ? refuse(ReasonCode::TalonHalfTaken) : Reason();
    }
    case ActionType::Discard: {
        if (m_phase != Phase::Discard)
            return refuse(ReasonCode::WrongPhase);
        if (seat != m_turn)
            return refuse(ReasonCode::NotYourTurn);
        const Card card(static_cast<std::uint8_t>(action.a));
        if (contains(m_tray[static_cast<std::size_t>(seat)], card))
            return Reason();   // taking a card back out is always allowed
        if (static_cast<int>(m_tray[static_cast<std::size_t>(seat)].count()) >= contractDef().discardCount) {
            Reason reason = refuse(ReasonCode::DiscardCount, card);
            reason.count = contractDef().discardCount;
            return reason;
        }
        const Reason cardReasonResult = cardReason(seat, card);
        return severityOf(cardReasonResult.code) == Severity::Error ? cardReasonResult : Reason();
    }
    case ActionType::ConfirmDiscard: {
        if (m_phase != Phase::Discard || seat != m_turn)
            return refuse(ReasonCode::WrongPhase);
        if (static_cast<int>(m_tray[static_cast<std::size_t>(seat)].count()) != contractDef().discardCount) {
            Reason reason = refuse(ReasonCode::DiscardCount);
            reason.count = contractDef().discardCount;
            return reason;
        }
        return Reason();
    }
    case ActionType::AnnounceBonus: {
        if (m_phase != Phase::Announce)
            return refuse(ReasonCode::WrongPhase);
        if ((m_announceReady >> seat) & 1)
            return refuse(ReasonCode::AnnounceOnlyOnce);
        const BonusId id = static_cast<BonusId>(action.a);
        const ContractDef& def = contractDef();
        if (!(def.bonusMask & bonusBit(id)))
            return def.play == PlayMode::SuitGame ? refuse(ReasonCode::SuitGameBonusLimited)
                                                  : refuse(ReasonCode::NoBonusInNegative);
        for (const Declaration& announcement : m_declarations) {
            if (announcement.bonus == id)
                return refuse(ReasonCode::AnnounceRepeat);
        }
        const BonusDef& bonus = profile().bonus(id);
        // Birds and the king ultimo may only be announced by the player who
        // holds the card (docs/koenigrufen.md §5.3).
        if (bonus.kind == BonusKind::TrickWithCard) {
            if (id == BonusId::KingUltimo) {
                if (m_calledSuit < 0 || !contains(hand(seat), calledKing()))
                    return refuse(ReasonCode::KingUltimoWithoutKing);
            } else if (!contains(hand(seat), bonus.boundCard)) {
                return refuse(ReasonCode::BonusCardNotHeld, bonus.boundCard);
            }
        }
        return Reason();
    }
    case ActionType::Kontra: {
        if (m_phase != Phase::Announce)
            return refuse(ReasonCode::WrongPhase);
        if (action.a < 0 || action.a >= static_cast<std::int16_t>(m_declarations.size()))
            return refuse(ReasonCode::UnknownAction);
        const Declaration& announcement = m_declarations[static_cast<std::size_t>(action.a)];
        if (contractDef().kontra == KontraMode::None)
            return refuse(ReasonCode::TrischakenNoKontra);
        // Only the other party doubles, and only one step at a time.
        if (announcement.declarerSide == declarerSide(seat))
            return refuse(ReasonCode::KontraOwnAnnouncement);
        if (announcement.level >= 8)
            return refuse(ReasonCode::KontraLevel);
        return Reason();
    }
    case ActionType::Ready: {
        if (m_phase != Phase::Announce)
            return refuse(ReasonCode::WrongPhase);
        return ((m_announceReady >> seat) & 1) ? refuse(ReasonCode::AnnounceOnlyOnce) : Reason();
    }
    case ActionType::PlayCard: {
        if (m_phase != Phase::Play)
            return refuse(ReasonCode::WrongPhase);
        if (seat != m_turn)
            return refuse(ReasonCode::NotYourTurn);
        const Reason reason = cardReason(seat, Card(static_cast<std::uint8_t>(action.a)));
        return severityOf(reason.code) == Severity::Error ? reason : Reason();
    }
    default:
        return refuse(ReasonCode::UnknownAction);
    }
}

bool TarockCore::apply(int seat, const Action& action, Reason* out)
{
    const Reason reason = check(seat, action);
    if (out)
        *out = reason;
    if (!reason.ok() && severityOf(reason.code) == Severity::Error)
        return false;

    switch (action.type) {
    case ActionType::OpenForehand:
        m_forehandSpoke = true;
        m_turn = nextSeat(seat);
        break;
    case ActionType::Bid:
        m_bid = static_cast<ContractId>(action.a);
        m_bidHolder = seat;
        if (seat == forehand())
            m_forehandSpoke = true;
        m_turn = nextBidder(seat);
        break;
    case ActionType::Hold:
        m_bidHolder = seat;
        m_forehandSpoke = true;
        m_turn = nextBidder(seat);
        break;
    case ActionType::Pass:
        m_passed |= 1 << seat;
        m_turn = nextBidder(seat);
        break;
    case ActionType::CallKing:
        if (action.b == 1) {
            m_calledFourthKing = true;
            for (int suit = 0; suit < 4; ++suit) {
                if (!contains(hand(seat), suitCard(Suit(suit), 0)))
                    m_calledSuit = suit;
            }
        } else {
            m_calledSuit = action.a;
        }
        for (int other = 0; other < m_players; ++other) {
            if (active(other) && contains(hand(other), calledKing()))
                m_partner = other;
        }
        beginTalon();
        break;
    case ActionType::TakeTalon:
        takeTalonHalf(action.a);
        break;
    case ActionType::Concede:
        m_conceded = true;
        finishHand();
        break;
    case ActionType::Discard: {
        const Card card(static_cast<std::uint8_t>(action.a));
        CardSet& tray = m_tray[static_cast<std::size_t>(seat)];
        if (contains(tray, card))
            remove(tray, card);
        else
            add(tray, card);
        break;
    }
    case ActionType::ConfirmDiscard: {
        CardSet& tray = m_tray[static_cast<std::size_t>(seat)];
        for (Card card : toList(tray)) {
            remove(m_hands[static_cast<std::size_t>(seat)], card);
            add(m_discards[static_cast<std::size_t>(seat)], card);
        }
        tray.reset();
        beginAnnounce();
        break;
    }
    case ActionType::AnnounceBonus: {
        Declaration announcement;
        announcement.bonus = static_cast<BonusId>(action.a);
        announcement.seat = seat;
        announcement.declarerSide = declarerSide(seat);
        announcement.announced = true;
        announcement.suit = action.b;
        m_declarations.push_back(announcement);
        // Announcing the king ultimo tells everybody who the partner is.
        if (announcement.bonus == BonusId::KingUltimo)
            m_partnerKnown = true;
        advanceAnnounce(seat, AnnounceStep::Announced);
        break;
    }
    case ActionType::Kontra: {
        Declaration& announcement = m_declarations[static_cast<std::size_t>(action.a)];
        announcement.level = announcement.level < 2 ? 2 : announcement.level * 2;
        // In the negative games every opponent doubles for himself (§7.5).
        if (contractDef().kontra == KontraMode::Individual)
            m_seatKontra[static_cast<std::size_t>(seat)] = announcement.level;
        advanceAnnounce(seat, AnnounceStep::Doubled);
        break;
    }
    case ActionType::Ready:
        m_announceReady |= 1 << seat;
        advanceAnnounce(seat, AnnounceStep::Confirm);
        break;
    case ActionType::PlayCard: {
        const Card card(static_cast<std::uint8_t>(action.a));
        remove(m_hands[static_cast<std::size_t>(seat)], card);
        m_trick.push_back(card);
        if (m_calledSuit >= 0 && card == calledKing())
            m_partnerKnown = true;
        if (static_cast<int>(m_trick.size()) < activePlayers())
            m_turn = nextSeat(seat);
        else
            finishTrick();
        break;
    }
    default:
        return false;
    }

    // Bidding ends when only one bidder is left, or when everybody passed
    // after the forehand said "Vorhand" (docs/koenigrufen.md §3.5).
    if (m_phase == Phase::Bidding) {
        int activeBidders = 0;
        for (int s = 0; s < m_players; ++s) {
            if (active(s) && !hasPassed(s))
                ++activeBidders;
        }
        if (m_bid != ContractId::None && (activeBidders <= 1 || m_turn == m_bidHolder))
            finishBidding();
        else if (m_bid == ContractId::None && activeBidders <= 1 && m_forehandSpoke)
            m_turn = forehand();   // the forehand now names its game
    }
    if (m_phase == Phase::Announce) {
        int ready = 0;
        for (int s = 0; s < m_players; ++s) {
            if (active(s) && ((m_announceReady >> s) & 1))
                ++ready;
        }
        if (ready == activePlayers())
            beginPlay();
    }
    return true;
}

void TarockCore::finishBidding()
{
    m_contract = m_bid;
    m_declarer = m_bidHolder;
    const ContractDef& def = contractDef();
    // The game itself is the first entry; announcements are added to it.
    Declaration game;
    game.bonus = BonusId::None;
    game.seat = m_declarer;
    game.declarerSide = true;
    game.announced = true;
    m_declarations.push_back(game);

    if (def.partner == PartnerMode::CallKing) {
        setPhase(Phase::Call);
        m_turn = m_declarer;
        return;
    }
    beginTalon();
}

void TarockCore::beginTalon()
{
    const ContractDef& def = contractDef();
    if (def.talon == TalonMode::OpenHalves) {
        m_talonOpen = true;
        setPhase(Phase::Talon);
        m_turn = m_declarer;
        return;
    }
    if (def.talon == TalonMode::AllHidden) {
        setPhase(Phase::Talon);
        m_turn = m_declarer;
        return;
    }
    beginAnnounce();
}

bool TarockCore::takeTalonHalf(int half)
{
    const ContractDef& def = contractDef();
    CardSet taken;
    if (half < 0) {
        taken = m_talonHalf[0] | m_talonHalf[1];
        m_talonHalf[0].reset();
        m_talonHalf[1].reset();
    } else {
        taken = m_talonHalf[static_cast<std::size_t>(half)];
        m_talonHalf[static_cast<std::size_t>(half)].reset();
    }
    m_hands[static_cast<std::size_t>(m_declarer)] |= taken;
    m_talonTaken = true;
    if (def.discardCount > 0) {
        setPhase(Phase::Discard);
        m_turn = m_declarer;
    } else {
        beginAnnounce();
    }
    return true;
}

void TarockCore::beginAnnounce()
{
    setPhase(Phase::Announce);
    m_announceReady = 0;
    m_turn = forehand();
}

// Announcements go round the table; an announcement or a Kontra opens a new
// round, because everybody may answer it.
void TarockCore::advanceAnnounce(int seat, AnnounceStep step)
{
    // An announcement opens a fresh round in which everybody, the announcer
    // included, closes with "Ich liege". A Kontra is itself the answer of the
    // player who gave it, so only the others still have to speak.
    if (step != AnnounceStep::Confirm) {
        m_announceReady = step == AnnounceStep::Doubled ? (1 << seat) : 0;
        m_turn = step == AnnounceStep::Announced ? seat : nextSeat(seat);
        return;
    }
    int next = nextSeat(seat);
    for (int i = 0; i < m_players; ++i) {
        if (active(next) && !((m_announceReady >> next) & 1)) {
            m_turn = next;
            return;
        }
        next = nextSeat(next);
    }
    m_turn = seat;
}

void TarockCore::beginPlay()
{
    setPhase(Phase::Play);
    m_trickNumber = 1;
    m_leader = contractDef().firstLead == LeadRule::Declarer ? m_declarer : forehand();
    m_turn = m_leader;
    m_trick.clear();
}

void TarockCore::finishTrick()
{
    const ContractDef& def = contractDef();
    const int index = winnerIndex(def, m_trick);
    // Work out who played the winning card by walking the seats in play order.
    int seat = m_leader;
    int winnerSeat = m_leader;
    for (int i = 0; i < static_cast<int>(m_trick.size()); ++i) {
        if (i == index)
            winnerSeat = seat;
        seat = nextSeat(seat);
    }
    for (Card card : m_trick)
        add(m_won[static_cast<std::size_t>(winnerSeat)], card);
    m_trickWinners[static_cast<std::size_t>(m_trickNumber - 1)] = static_cast<std::int8_t>(winnerSeat);
    m_trickCards[static_cast<std::size_t>(m_trickNumber - 1)] = m_trick;
    m_trick.clear();
    m_leader = winnerSeat;
    m_turn = winnerSeat;
    if (m_trickNumber >= profile().tricks()) {
        dealTalonToWinner();
        finishHand();
        return;
    }
    ++m_trickNumber;
}

void TarockCore::dealTalonToWinner()
{
    const ContractDef& def = contractDef();
    const CardSet rest = m_talonHalf[0] | m_talonHalf[1];
    if (!rest.any())
        return;
    switch (def.talon) {
    case TalonMode::ToLastTrick:
        m_won[static_cast<std::size_t>(m_leader)] |= rest;
        break;
    case TalonMode::OpenHalves:
    case TalonMode::ToOpponents:
        // What the declarer did not take belongs to the defenders as a party.
        m_talonToDefenders |= rest;
        break;
    default:
        break;
    }
    m_talonHalf[0].reset();
    m_talonHalf[1].reset();
}

void TarockCore::finishHand()
{
    setPhase(Phase::Scoring);
    m_turn = -1;
    m_partnerKnown = true;
    m_ledger = settle(handResult());
}

// Everything the settlement needs, read off the finished hand.
HandResult TarockCore::handResult() const
{
    HandResult result;
    result.profile = profile().id();
    result.contract = m_contract;
    result.players = m_players;
    result.sittingOut = static_cast<std::int8_t>(sittingOut());
    result.declarer = static_cast<std::int8_t>(m_declarer);
    result.partner = static_cast<std::int8_t>(
        contractDef().partner == PartnerMode::CallKing ? m_partner : -1);
    result.forehand = static_cast<std::int8_t>(forehand());
    result.declarerCards = partyCards(true);
    result.defenderCards = partyCards(false);
    for (int seat = 0; seat < MaxSeats; ++seat) {
        result.seatCards[static_cast<std::size_t>(seat)] =
            m_won[static_cast<std::size_t>(seat)] | m_discards[static_cast<std::size_t>(seat)];
        result.seatKontra[static_cast<std::size_t>(seat)] = m_seatKontra[static_cast<std::size_t>(seat)];
    }
    for (int i = 0; i < profile().tricks(); ++i) {
        const CardList& cards = m_trickCards[static_cast<std::size_t>(i)];
        if (cards.empty())
            break;
        TrickResult trick;
        trick.cards = toSet(cards);
        trick.winner = m_trickWinners[static_cast<std::size_t>(i)];
        // Which card took it: the same comparison the play used.
        Card best = cards.front();
        for (Card card : cards) {
            if (beats(contractDef(), card, best))
                best = card;
        }
        trick.winningCard = best;
        result.tricks.push_back(trick);
    }
    for (const Declaration& declaration : m_declarations) {
        if (declaration.bonus == BonusId::None) {
            result.gameKontra = declaration.level;
            continue;
        }
        Announcement announcement;
        announcement.bonus = declaration.bonus;
        announcement.party = declaration.declarerSide ? Party::Declarer : Party::Defenders;
        announcement.kontra = declaration.level;
        result.announcements.push_back(announcement);
    }
    result.calledKing = calledKing();
    result.conceded = m_conceded;
    if (m_conceded) {
        for (const Declaration& declaration : m_declarations) {
            const BonusId bonus = declaration.bonus;
            if (bonus == BonusId::Pagat || bonus == BonusId::Uhu || bonus == BonusId::Kakadu
                || bonus == BonusId::Quapil)
                result.concededBird = bonus;
        }
    }
    return result;
}

// --- queries ------------------------------------------------------------------------

Card TarockCore::calledKing() const
{
    return m_calledSuit < 0 ? Card() : suitCard(Suit(m_calledSuit), 0);
}

bool TarockCore::declarerSide(int seat) const
{
    if (m_declarer < 0)
        return false;
    if (seat == m_declarer)
        return true;
    if (contractDef().partner == PartnerMode::CallKing)
        return seat == m_partner;
    return false;
}

int TarockCore::trickWinner(int trick) const
{
    if (trick < 1 || trick > profile().tricks())
        return -1;
    return m_trickWinners[static_cast<std::size_t>(trick - 1)];
}

CardSet TarockCore::partyCards(bool declarerParty) const
{
    CardSet cards;
    for (int seat = 0; seat < m_players; ++seat) {
        if (!active(seat) || declarerSide(seat) != declarerParty)
            continue;
        cards |= m_won[static_cast<std::size_t>(seat)];
        cards |= m_discards[static_cast<std::size_t>(seat)];
    }
    if (!declarerParty)
        cards |= m_talonToDefenders;
    return cards;
}

CountResult TarockCore::partyPoints(bool declarerParty) const
{
    return profile().count(partyCards(declarerParty));
}

// --- LAN perspective and persistence --------------------------------------------------

void TarockCore::rotateSeats(int offset)
{
    offset = ((offset % m_players) + m_players) % m_players;
    if (offset == 0)
        return;
    const int n = m_players;
    auto seatOf = [n, offset](int seat) { return seat < 0 ? seat : (seat - offset + 2 * n) % n; };
    auto rotate = [&](auto& values) {
        auto copy = values;
        for (int seat = 0; seat < n; ++seat)
            values[static_cast<std::size_t>(seatOf(seat))] = copy[static_cast<std::size_t>(seat)];
    };
    rotate(m_hands);
    rotate(m_won);
    rotate(m_tray);
    rotate(m_discards);

    int passed = 0, ready = 0;
    for (int seat = 0; seat < n; ++seat) {
        if ((m_passed >> seat) & 1)
            passed |= 1 << seatOf(seat);
        if ((m_announceReady >> seat) & 1)
            ready |= 1 << seatOf(seat);
    }
    m_passed = passed;
    m_announceReady = ready;

    for (Declaration& announcement : m_declarations)
        announcement.seat = seatOf(announcement.seat);
    for (std::int8_t& winner : m_trickWinners) {
        if (winner >= 0)
            winner = static_cast<std::int8_t>(seatOf(winner));
    }
    m_dealer = seatOf(m_dealer);
    m_turn = seatOf(m_turn);
    m_leader = seatOf(m_leader);
    m_bidHolder = seatOf(m_bidHolder);
    m_declarer = seatOf(m_declarer);
    m_partner = seatOf(m_partner);
}

namespace {

void writeSet(std::ostream& out, const CardSet& set)
{
    out << set.to_string() << '\n';
}

bool readSet(std::istream& in, CardSet& set)
{
    std::string text;
    if (!(in >> text) || text.size() != 54)
        return false;
    set = CardSet(text);
    return true;
}

} // namespace

std::string TarockCore::serialize() const
{
    std::ostringstream body;
    body << static_cast<int>(profile().id()) << ' ' << m_players << ' ' << m_dealer << ' ' << m_hand
         << ' ' << static_cast<int>(m_phase) << ' ' << m_turn << '\n';
    for (int seat = 0; seat < MaxSeats; ++seat) {
        writeSet(body, m_hands[static_cast<std::size_t>(seat)]);
        writeSet(body, m_won[static_cast<std::size_t>(seat)]);
        writeSet(body, m_tray[static_cast<std::size_t>(seat)]);
        writeSet(body, m_discards[static_cast<std::size_t>(seat)]);
    }
    writeSet(body, m_talonHalf[0]);
    writeSet(body, m_talonHalf[1]);
    writeSet(body, m_talonToDefenders);
    body << (m_talonOpen ? 1 : 0) << ' ' << (m_talonTaken ? 1 : 0) << '\n';
    body << m_trick.size();
    for (Card card : m_trick)
        body << ' ' << static_cast<int>(card.id);
    body << '\n' << m_leader << ' ' << m_trickNumber << '\n';
    for (std::int8_t winner : m_trickWinners)
        body << static_cast<int>(winner) << ' ';
    body << '\n';
    body << m_passed << ' ' << static_cast<int>(m_bid) << ' ' << m_bidHolder << ' '
         << (m_forehandSpoke ? 1 : 0) << '\n';
    body << static_cast<int>(m_contract) << ' ' << m_declarer << ' ' << m_partner << ' '
         << (m_partnerKnown ? 1 : 0) << ' ' << m_calledSuit << ' ' << (m_calledFourthKing ? 1 : 0)
         << ' ' << (m_conceded ? 1 : 0) << ' ' << m_announceReady << '\n';
    body << m_declarations.size() << '\n';
    for (const Declaration& announcement : m_declarations) {
        body << static_cast<int>(announcement.bonus) << ' ' << announcement.seat << ' '
             << (announcement.declarerSide ? 1 : 0) << ' ' << (announcement.announced ? 1 : 0) << ' '
             << announcement.level << ' ' << announcement.suit << '\n';
    }
    body << m_flags.silentBonusesCountNegative << ' ' << m_flags.valatTakesTalon << ' '
         << m_flags.hardBirdReservation << ' ' << m_flags.handsPerRound << '\n';
    body << m_rng << '\n';

    const std::string payload = body.str();
    std::ostringstream out;
    out << "TAROCK_STATE_V1\n" << std::hex << checksum(payload) << '\n' << payload;
    return out.str();
}

bool TarockCore::restore(const std::string& data)
{
    std::istringstream envelope(data);
    std::string magic, checksumText;
    if (!std::getline(envelope, magic) || magic != "TAROCK_STATE_V1"
        || !std::getline(envelope, checksumText))
        return false;
    std::uint64_t expected = 0;
    std::istringstream checkStream(checksumText);
    if (!(checkStream >> std::hex >> expected))
        return false;
    const std::string payload((std::istreambuf_iterator<char>(envelope)), std::istreambuf_iterator<char>());
    if (checksum(payload) != expected)
        return false;

    TarockCore r;
    std::istringstream in(payload);
    int profileId = 0, phase = 0;
    if (!(in >> profileId >> r.m_players >> r.m_dealer >> r.m_hand >> phase >> r.m_turn)
        || profileId < 0 || profileId > 1 || phase < 0 || phase > 8
        || (r.m_players != 4 && r.m_players != 5))
        return false;
    r.m_profile = &RuleProfile::get(static_cast<ProfileId>(profileId));
    r.m_phase = static_cast<Phase>(phase);
    for (int seat = 0; seat < MaxSeats; ++seat) {
        if (!readSet(in, r.m_hands[static_cast<std::size_t>(seat)])
            || !readSet(in, r.m_won[static_cast<std::size_t>(seat)])
            || !readSet(in, r.m_tray[static_cast<std::size_t>(seat)])
            || !readSet(in, r.m_discards[static_cast<std::size_t>(seat)]))
            return false;
    }
    if (!readSet(in, r.m_talonHalf[0]) || !readSet(in, r.m_talonHalf[1])
        || !readSet(in, r.m_talonToDefenders))
        return false;
    int talonOpen = 0, talonTaken = 0;
    if (!(in >> talonOpen >> talonTaken))
        return false;
    r.m_talonOpen = talonOpen != 0;
    r.m_talonTaken = talonTaken != 0;
    std::size_t trickSize = 0;
    if (!(in >> trickSize) || trickSize > 5)
        return false;
    for (std::size_t i = 0; i < trickSize; ++i) {
        int id = 0;
        if (!(in >> id) || id < 0 || id > 53)
            return false;
        r.m_trick.push_back(Card(static_cast<std::uint8_t>(id)));
    }
    if (!(in >> r.m_leader >> r.m_trickNumber))
        return false;
    for (std::int8_t& winner : r.m_trickWinners) {
        int value = 0;
        if (!(in >> value))
            return false;
        winner = static_cast<std::int8_t>(value);
    }
    int bid = 0, forehandSpoke = 0;
    if (!(in >> r.m_passed >> bid >> r.m_bidHolder >> forehandSpoke))
        return false;
    r.m_bid = static_cast<ContractId>(bid);
    r.m_forehandSpoke = forehandSpoke != 0;
    int contract = 0, partnerKnown = 0, fourthKing = 0, conceded = 0;
    if (!(in >> contract >> r.m_declarer >> r.m_partner >> partnerKnown >> r.m_calledSuit
          >> fourthKing >> conceded >> r.m_announceReady))
        return false;
    r.m_contract = static_cast<ContractId>(contract);
    r.m_partnerKnown = partnerKnown != 0;
    r.m_calledFourthKing = fourthKing != 0;
    r.m_conceded = conceded != 0;
    std::size_t postenCount = 0;
    if (!(in >> postenCount) || postenCount > 64)
        return false;
    for (std::size_t i = 0; i < postenCount; ++i) {
        Declaration announcement;
        int bonus = 0, side = 0, announced = 0;
        if (!(in >> bonus >> announcement.seat >> side >> announced >> announcement.level >> announcement.suit))
            return false;
        announcement.bonus = static_cast<BonusId>(bonus);
        announcement.declarerSide = side != 0;
        announcement.announced = announced != 0;
        r.m_declarations.push_back(announcement);
    }
    int silentNegative = 0, valatTalon = 0, hardBirds = 0;
    if (!(in >> silentNegative >> valatTalon >> hardBirds >> r.m_flags.handsPerRound))
        return false;
    r.m_flags.silentBonusesCountNegative = silentNegative != 0;
    r.m_flags.valatTakesTalon = valatTalon != 0;
    r.m_flags.hardBirdReservation = hardBirds != 0;
    if (!(in >> r.m_rng))
        return false;
    if (!r.validate())
        return false;
    *this = std::move(r);
    return true;
}

bool TarockCore::validate(std::string* error) const
{
    auto fail = [&](const char* message) {
        if (error)
            *error = message;
        return false;
    };
    if (m_players != 4 && m_players != 5)
        return fail("wrong number of players");
    auto seatOk = [this](int seat) { return seat >= -1 && seat < m_players; };
    if (!seatOk(m_turn) || !seatOk(m_leader) || !seatOk(m_declarer) || !seatOk(m_partner)
        || !seatOk(m_bidHolder) || m_dealer < 0 || m_dealer >= m_players)
        return fail("invalid seat reference");
    if (m_calledSuit < -1 || m_calledSuit > 3)
        return fail("invalid called suit");

    // Every card of the deck has to be somewhere, exactly once.
    CardSet seen;
    std::size_t total = 0;
    auto collect = [&](const CardSet& set) {
        if ((seen & set).any())
            return false;
        seen |= set;
        total += set.count();
        return true;
    };
    for (int seat = 0; seat < MaxSeats; ++seat) {
        if (!collect(m_hands[static_cast<std::size_t>(seat)])
            || !collect(m_won[static_cast<std::size_t>(seat)])
            || !collect(m_discards[static_cast<std::size_t>(seat)]))
            return fail("a card is in two places");
    }
    if (!collect(m_talonHalf[0]) || !collect(m_talonHalf[1]) || !collect(m_talonToDefenders))
        return fail("a talon card is in two places");
    CardSet inTrick;
    for (Card card : m_trick) {
        if (contains(seen, card) || contains(inTrick, card))
            return fail("a card of the trick is in two places");
        add(inTrick, card);
        ++total;
    }
    seen |= inTrick;
    if (static_cast<int>(total) != profile().deck().size())
        return fail("cards are missing");
    if ((seen & ~profile().deck().cards()).any())
        return fail("a card outside the deck");
    if (m_phase == Phase::Play && static_cast<int>(m_trick.size()) >= activePlayers())
        return fail("the trick was not resolved");
    return true;
}

} // namespace tarock
