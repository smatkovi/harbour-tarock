#include "Action.h"

namespace tarock {

const char* actionKey(ActionType type)
{
    switch (type) {
    case ActionType::None: return "NONE";
    case ActionType::Bid: return "BID";
    case ActionType::Pass: return "PASS";
    case ActionType::Hold: return "HOLD";
    case ActionType::OpenForehand: return "OPEN_FOREHAND";
    case ActionType::CallKing: return "CALL_KING";
    case ActionType::CallTarock: return "CALL_TAROCK";
    case ActionType::TakeTalon: return "TAKE_TALON";
    case ActionType::Concede: return "CONCEDE";
    case ActionType::Throwin: return "THROWIN";
    case ActionType::Discard: return "DISCARD";
    case ActionType::ConfirmDiscard: return "CONFIRM_DISCARD";
    case ActionType::AnnounceBonus: return "ANNOUNCE";
    case ActionType::Kontra: return "KONTRA";
    case ActionType::Ready: return "READY";
    case ActionType::PlayCard: return "PLAY";
    case ActionType::ClaimEnd: return "CLAIM_END";
    }
    return "?";
}

} // namespace tarock
