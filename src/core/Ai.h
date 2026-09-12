// The computer players. Everything here is a pure function of the state a seat
// may legally see, so the same code drives the opponents, the hint engine of
// the learning mode and the self-play tests (docs/design.md §4.9).
#pragma once

#include "TarockCore.h"

#include <cstdint>
#include <vector>

namespace tarock {

enum class Difficulty : std::uint8_t { Beginner = 0, Club = 1, Tournament = 2 };

// Why the computer chose an action. The learning mode shows the matching
// sentence, so every decision path must set one.
enum class HintReason : std::uint16_t {
    None = 0,
    // bidding
    HandTooWeak, HandStrongEnough, BirdCountSufficient, SoloWorthIt,
    ContractEconomy, SeatPosition, ForehandMustChoose,
    // calling, talon, discarding
    CallFromGuardedQueen, CallShortSuit, PreferTarockOverPoints, VoidASuit,
    KeepCalledSuit, ProtectKing, TalonHalfRicher,
    // announcements
    BonusSafe, BonusRisky, KontraWorthwhile, SignalToPartner, NothingToAnnounce,
    // card play
    DrawTrumps, SmearToPartner, DuckCheaply, KeepStopper, LeadShortKing,
    SaveBirdForTarget, LastTrickPlan, CountSaysEnough, CountSaysMustAttack,
    OnlyOneCard, FollowSuitForced
};

const char* hintKey(HintReason reason);

struct Hint {
    Action action;
    int score = 0;                       // expected settlement points x100
    int rank = 0;                        // 0 = the recommendation
    HintReason reason = HintReason::None;
    Reason detail;                       // cards and parameters for the text
};

// What one seat may deduce from the public course of the hand: who is void in
// which suit, which cards have fallen, what was announced. Filled from the
// public record only, never from another seat's hand (§10.8).
class Inference {
public:
    void observe(const TarockCore& core, int seat);

    bool isVoid(int seat, int suit) const;      // suit 0..3, 4 = tarock
    bool hasFallen(Card card) const { return contains(m_seen, card); }
    const CardSet& seen() const { return m_seen; }
    int tricksSeen() const { return m_tricksSeen; }
    // Cards that may still be in another seat's hand from `seat`'s point of view.
    CardSet unknownCards(const TarockCore& core, int seat) const;

private:
    void markVoid(int seat, int suit);
    // One card of the running trick, with the suit that was led.
    void noteCard(int seat, int leadSuit, Card card);

    CardSet m_seen{};
    // bit per suit (0..3) plus bit 4 for tarock, one entry per seat
    std::array<std::uint8_t, TarockCore::MaxSeats> m_void{};
    int m_tricksSeen = 0;
    // Bookkeeping so that repeated looks follow the trick card by card: the
    // core only publishes the running trick in playing order, so a seat that
    // looks after every action sees who did not follow suit.
    int m_trickNumber = 0;
    int m_inTrick = 0;              // cards of that trick already folded in
    std::int8_t m_leadSeat = -1;
    std::int8_t m_leadSuit = -1;
};

class AiPlayer {
public:
    explicit AiPlayer(Difficulty level = Difficulty::Club);

    Difficulty level() const { return m_level; }
    void setLevel(Difficulty level) { m_level = level; }

    // The action this seat plays. Never returns an illegal action; when the
    // core offers nothing it returns an Action with type None.
    Action bestAction(const TarockCore& core, int seat, int budgetMs = 80) const;
    // All legal actions, best first, for the hint engine of the learning mode.
    std::vector<Hint> rank(const TarockCore& core, int seat, int budgetMs = 80) const;
    // Why a given action is good or bad, without choosing one.
    HintReason explain(const TarockCore& core, int seat, const Action& action) const;

    // The same three decisions with an inference the caller keeps up to date.
    // Whoever watches every action — the engine, the tests — deduces more than
    // a fresh look at the table can, because only the running trick is
    // published in playing order.
    Action bestAction(const TarockCore& core, int seat, const Inference& inference,
                      int budgetMs = 80) const;
    std::vector<Hint> rank(const TarockCore& core, int seat, const Inference& inference,
                           int budgetMs = 80) const;
    HintReason explain(const TarockCore& core, int seat, const Inference& inference,
                       const Action& action) const;

private:
    Difficulty m_level;
};

} // namespace tarock
