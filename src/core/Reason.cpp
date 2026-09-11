#include "Reason.h"

namespace tarock {

const char* reasonKey(ReasonCode code)
{
    switch (code) {
    case ReasonCode::Ok: return "OK";
    case ReasonCode::NotYourTurn: return "E_NOT_YOUR_TURN";
    case ReasonCode::NotYourCard: return "E_NOT_YOUR_CARD";
    case ReasonCode::WrongPhase: return "E_WRONG_PHASE";
    case ReasonCode::SeatSittingOut: return "E_SEAT_SITTING_OUT";
    case ReasonCode::UnknownAction: return "E_UNKNOWN_ACTION";
    case ReasonCode::BidTooLow: return "E_BID_TOO_LOW";
    case ReasonCode::ForehandGameOnlyLast: return "E_FOREHAND_GAME_ONLY_LAST";
    case ReasonCode::SechserOnlyFirst: return "E_SECHSER_ONLY_FIRST";
    case ReasonCode::AlreadyPassed: return "E_ALREADY_PASSED";
    case ReasonCode::ForehandMustSpeak: return "E_FOREHAND_MUST_SPEAK";
    case ReasonCode::HoldOnlyForehand: return "E_HOLD_ONLY_FOREHAND";
    case ReasonCode::HoldNotAllowed: return "E_HOLD_NOT_ALLOWED";
    case ReasonCode::CallerHasFourKings: return "E_CALLER_HAS_FOUR_KINGS";
    case ReasonCode::BesserruferWithoutBird: return "E_BESSERRUFER_WITHOUT_BIRD";
    case ReasonCode::NoHonour: return "E_NO_HONOUR";
    case ReasonCode::InviterLocked: return "E_INVITER_LOCKED";
    case ReasonCode::JumpWithoutCard: return "E_JUMP_WITHOUT_CARD";
    case ReasonCode::PassIsEngedes: return "E_PASS_IS_ENGEDES";
    case ReasonCode::CallOwnKing: return "E_CALL_OWN_KING";
    case ReasonCode::FourthKingNeedsThree: return "E_FOURTH_KING_NEEDS_THREE";
    case ReasonCode::CallMustBeXX: return "E_CALL_MUST_BE_X_X";
    case ReasonCode::CallOwnSkart: return "E_CALL_OWN_SKART";
    case ReasonCode::CallHonourForbidden: return "E_CALL_HONOUR_FORBIDDEN";
    case ReasonCode::TalonHalfTaken: return "E_TALON_HALF_TAKEN";
    case ReasonCode::DiscardKing: return "E_DISCARD_KING";
    case ReasonCode::DiscardTrull: return "E_DISCARD_TRULL";
    case ReasonCode::DiscardTarockWhileSuit: return "E_DISCARD_TAROCK_WHILE_SUIT";
    case ReasonCode::DiscardSuitWhileTarock: return "E_DISCARD_SUIT_WHILE_TAROCK";
    case ReasonCode::DiscardCount: return "E_DISCARD_COUNT";
    case ReasonCode::DiscardXX: return "E_DISCARD_X_X";
    case ReasonCode::DiscardInvitCard: return "E_DISCARD_INVIT_CARD";
    case ReasonCode::DiscardNotHeld: return "E_DISCARD_NOT_HELD";
    case ReasonCode::BonusCardNotHeld: return "E_BONUS_CARD_NOT_HELD";
    case ReasonCode::KingUltimoWithoutKing: return "E_KING_ULTIMO_WITHOUT_KING";
    case ReasonCode::AnnounceOnlyOnce: return "E_ANNOUNCE_ONLY_ONCE";
    case ReasonCode::BesserruferBirdMandatory: return "E_BESSERRUFER_BIRD_MANDATORY";
    case ReasonCode::NoBonusInNegative: return "E_NO_BONUS_IN_NEGATIVE";
    case ReasonCode::SuitGameBonusLimited: return "E_SUIT_GAME_BONUS_LIMITED";
    case ReasonCode::SolodreierAnnounceTooLate: return "E_SOLODREIER_ANNOUNCE_TOO_LATE";
    case ReasonCode::KontraBeforeReady: return "E_KONTRA_BEFORE_READY";
    case ReasonCode::KontraOwnAnnouncement: return "E_KONTRA_OWN_ANNOUNCEMENT";
    case ReasonCode::KontraLevel: return "E_KONTRA_LEVEL";
    case ReasonCode::KontraTooLate: return "E_KONTRA_TOO_LATE";
    case ReasonCode::TrischakenNoKontra: return "E_TRISCHAKEN_NO_KONTRA";
    case ReasonCode::SuitGameKontraOrValat: return "E_SUIT_GAME_KONTRA_OR_VALAT";
    case ReasonCode::TrullNeedsHonour: return "E_TRULL_NEEDS_HONOUR";
    case ReasonCode::AnnounceAfterVolat: return "E_ANNOUNCE_AFTER_VOLAT";
    case ReasonCode::AnnounceRepeat: return "E_ANNOUNCE_REPEAT";
    case ReasonCode::SameTrickTwice: return "E_SAME_TRICK_TWICE";
    case ReasonCode::FollowSuit: return "E_FOLLOW_SUIT";
    case ReasonCode::FollowTarock: return "E_FOLLOW_TAROCK";
    case ReasonCode::MustTarock: return "E_MUST_TAROCK";
    case ReasonCode::MustOvertrumpSuit: return "E_MUST_OVERTRUMP_SUIT";
    case ReasonCode::MustOvertrumpTarock: return "E_MUST_OVERTRUMP_TAROCK";
    case ReasonCode::MustTrumpNegative: return "E_MUST_TRUMP_NEGATIVE";
    case ReasonCode::PagatNotLastTarock: return "E_PAGAT_NOT_LAST_TAROCK";
    case ReasonCode::SuitGameNoTarockLead: return "E_SUIT_GAME_NO_TAROCK_LEAD";
    case ReasonCode::BoundMustPlay: return "E_BOUND_MUST_PLAY";
    case ReasonCode::BoundOrder: return "E_BOUND_ORDER";
    case ReasonCode::BoundWinsTooEarly: return "E_BOUND_WINS_TOO_EARLY";
    case ReasonCode::W_BirdReserved: return "W_BIRD_RESERVED";
    case ReasonCode::W_KingUltimoReserved: return "W_KING_ULTIMO_RESERVED";
    case ReasonCode::W_GivesUpSilentBonus: return "W_GIVES_UP_SILENT_BONUS";
    case ReasonCode::I_DiscardShownOpen: return "I_DISCARD_SHOWN_OPEN";
    case ReasonCode::I_TalonHalfGoesToOpponents: return "I_TALON_HALF_GOES_TO_OPPONENTS";
    case ReasonCode::I_SilentFigure: return "I_SILENT_FIGURE";
    case ReasonCode::I_VolatSilentRule: return "I_VOLAT_SILENT_RULE";
    }
    return "E_UNKNOWN";
}

Severity severityOf(ReasonCode code)
{
    switch (code) {
    case ReasonCode::Ok:
        return Severity::Ok;
    case ReasonCode::W_BirdReserved:
    case ReasonCode::W_KingUltimoReserved:
    case ReasonCode::W_GivesUpSilentBonus:
        return Severity::Warning;
    case ReasonCode::I_DiscardShownOpen:
    case ReasonCode::I_TalonHalfGoesToOpponents:
    case ReasonCode::I_SilentFigure:
    case ReasonCode::I_VolatSilentRule:
        return Severity::Info;
    default:
        return Severity::Error;
    }
}

} // namespace tarock
