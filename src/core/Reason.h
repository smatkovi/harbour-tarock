// Machine-readable reasons for everything the rules refuse, warn about or
// point out. The learning mode turns them into sentences, the tests assert on
// the stable keys, and the engine uses the severity to decide between a hard
// refusal, a warning and a hint.
#pragma once

#include "Card.h"

#include <cstdint>

namespace tarock {

enum class Severity : std::uint8_t { Ok = 0, Info = 1, Warning = 2, Error = 3 };

enum class ReasonCode : std::uint16_t {
    Ok = 0,

    // general
    NotYourTurn, NotYourCard, WrongPhase, SeatSittingOut, UnknownAction,

    // bidding
    BidTooLow, ForehandGameOnlyLast, SechserOnlyFirst, AlreadyPassed,
    ForehandMustSpeak, HoldOnlyForehand, HoldNotAllowed, CallerHasFourKings,
    BesserruferWithoutBird, NoHonour, InviterLocked, JumpWithoutCard, PassIsEngedes,

    // calling a partner
    CallOwnKing, FourthKingNeedsThree, CallMustBeXX, CallOwnSkart, CallHonourForbidden,

    // talon and discarding
    TalonHalfTaken, DiscardKing, DiscardTrull, DiscardTarockWhileSuit,
    DiscardSuitWhileTarock, DiscardCount, DiscardXX, DiscardInvitCard, DiscardNotHeld,

    // announcements and kontra
    BonusCardNotHeld, KingUltimoWithoutKing, AnnounceOnlyOnce, BesserruferBirdMandatory,
    NoBonusInNegative, SuitGameBonusLimited, SolodreierAnnounceTooLate,
    KontraBeforeReady, KontraOwnAnnouncement, KontraLevel, KontraTooLate,
    TrischakenNoKontra, SuitGameKontraOrValat,
    TrullNeedsHonour, AnnounceAfterVolat, AnnounceRepeat, SameTrickTwice,

    // playing cards
    FollowSuit, FollowTarock, MustTarock, MustOvertrumpSuit, MustOvertrumpTarock,
    MustTrumpNegative, PagatNotLastTarock, SuitGameNoTarockLead,
    BoundMustPlay, BoundOrder, BoundWinsTooEarly,

    // warnings: allowed, but a bonus is given up
    W_BirdReserved, W_KingUltimoReserved, W_GivesUpSilentBonus,

    // hints
    I_DiscardShownOpen, I_TalonHalfGoesToOpponents, I_SilentFigure, I_VolatSilentRule
};

// Everything the texts of the learning mode may need to fill in.
struct Reason {
    ReasonCode code = ReasonCode::Ok;
    CardSet cards;                 // "allowed would be …" / "you still hold …"
    Card card;                     // the card in question
    std::int16_t seat = -1;
    std::int16_t suit = -1;
    std::int16_t bonus = -1;
    std::int16_t contract = -1;
    std::int16_t trick = -1;
    std::int16_t count = -1;

    constexpr bool ok() const { return code == ReasonCode::Ok; }
};

Severity severityOf(ReasonCode code);
const char* reasonKey(ReasonCode code);

} // namespace tarock
