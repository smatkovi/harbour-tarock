// One action type for every decision a player makes, in every profile. The
// engine, the computer players and the LAN protocol all speak this language.
#pragma once

#include <cstdint>

namespace tarock {

enum class ActionType : std::uint8_t {
    None = 0,
    Bid = 1,            // a = ContractId
    Pass = 2,           // "gut" / "weiter" / "passz"
    Hold = 3,           // "ich halte" / "tartom"
    OpenForehand = 4,   // Königrufen: "Vorhand", still without a game
    CallKing = 5,       // a = Suit, b = 1 means "the fourth king"
    CallTarock = 6,     // a = card id (Hungarian XX call)
    TakeTalon = 7,      // a = 0|1 half, -1 = the whole talon
    Concede = 8,        // "schleifen"
    Throwin = 9,        // "bedobás", a = reason id
    Discard = 10,       // a = card id, toggles the card in the discard tray
    ConfirmDiscard = 11,
    AnnounceBonus = 12, // a = BonusId, b = suit for king ultimo, else -1
    Kontra = 13,        // a = posten id, b = level 2|4|8
    Ready = 14,         // "Ich liege" — announcements finished
    PlayCard = 15,      // a = card id
    ClaimEnd = 16       // early end when the rest is publicly certain
};

struct Action {
    ActionType type = ActionType::None;
    std::int16_t a = -1;
    std::int16_t b = -1;

    constexpr Action() = default;
    constexpr Action(ActionType type_, std::int16_t a_ = -1, std::int16_t b_ = -1)
        : type(type_), a(a_), b(b_) {}

    constexpr bool operator==(const Action& other) const
    {
        return type == other.type && a == other.a && b == other.b;
    }
};

const char* actionKey(ActionType type);

} // namespace tarock
