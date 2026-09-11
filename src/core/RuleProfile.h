// A rule profile is the data part of a variant: which deck, how cards are
// counted, which contracts can be bid, which bonuses exist and what they pay.
// Everything the profiles share in procedure lives in TarockRules.
#pragma once

#include "Card.h"
#include "Deck.h"

#include <cstdint>
#include <string>
#include <vector>

namespace tarock {

enum class ProfileId : std::uint8_t {
    AtKrOoe2023 = 0,   // Königrufen, Oberösterreich 4/2023
    HuIlluItvb2019 = 1 // ungarisches Illusztrált tarokk, ITVB 2019
};

enum class ContractId : std::uint8_t {
    None = 0,
    // Königrufen
    Trischaken, Rufer, Piccolo, Bettel, Solorufer, Besserrufer, Farbendreier,
    Sechserdreier, Dreier, Farbensolo, PiccoloOuvert, BettelOuvert, Solodreier,
    // Hungarian
    Harom, Ketto, Egy, Szolo
};

// Shared ids where the two games mean the same thing: Trull = tulétroá,
// AllKings = négykirály, Pagat = pagát ultimó, KingUltimo = király ultimó,
// Valat = volát.
enum class BonusId : std::uint8_t {
    None = 0,
    Trull, AllKings, KingUltimo, Pagat, Uhu, Kakadu, Quapil, Valat,
    Duplajatek, SasUltimo, XXIFogas, Centrum, Kismadar, Nagymadar,
    PagatUhu, SasUhu, KiralyUhu, Tarokk8, Tarokk9, Pagatfogas
};

enum class BonusKind : std::uint8_t {
    Holding,        // the cards are in the party's tricks at the end
    TrickWithCard,  // a named card wins a named trick
    TrickRun,       // the first n tricks, the n-th won with a named card
    PointTarget,    // a card-point target (duplajáték)
    AllTricks,      // Valat / volát
    Capture,        // the Skíz catches the opponents' XXI
    TarockCount     // exactly n tarocks after discarding
};

enum class TalonMode : std::uint8_t {
    None,          // talon is not used
    OpenHalves,    // both halves face up, the declarer takes one
    AllHidden,     // all six cards, face down (Sechserdreier)
    ToOpponents,   // talon stays and counts for the opponents (solo games)
    ToLastTrick,   // goes to the winner of the last trick (Trischaken)
    Distributed    // shared out among all players (Hungarian)
};

enum class PartnerMode : std::uint8_t { Alone, CallKing, CallTarock, EveryoneAlone };
enum class PlayMode : std::uint8_t { Positive, SuitGame, Negative, Trischaken };
enum class LeadRule : std::uint8_t { Forehand, Declarer };
enum class KontraMode : std::uint8_t { None, Party, Individual };
enum class CountMode : std::uint8_t { ThirdsAustrian, PlainHungarian };

// Who may bid a contract and when.
enum BidFlag : std::uint16_t {
    BidAnyone = 0,
    ForehandOnly = 1 << 0,        // only the forehand may bid it
    ForehandLastOnly = 1 << 1,    // "hintennach": only after everyone else passed
    ForehandFirstOnly = 1 << 2,   // "vorneweg": only as the very first bid
    RequiresBird = 1 << 3,        // Besserrufer needs a bird in hand
    ForbidsFourKings = 1 << 4,    // a caller may not hold all four kings
    AnnounceWithBid = 1 << 5      // bonuses are announced together with the bid
};

struct ContractDef {
    ContractId id = ContractId::None;
    const char* key = "";
    int rank = 0;                 // bidding order, low to high
    int baseValue = 0;            // game value ("Schrift")
    int lostValue = 0;            // value when lost, if it differs
    TalonMode talon = TalonMode::None;
    std::int8_t talonToDeclarer = 0;
    std::int8_t discardCount = 0;
    PartnerMode partner = PartnerMode::Alone;
    PlayMode play = PlayMode::Positive;
    LeadRule firstLead = LeadRule::Forehand;
    KontraMode kontra = KontraMode::Party;
    std::uint32_t bonusMask = 0;  // 1 << BonusId
    bool soloDoublesBonuses = false;
    std::uint16_t bidFlags = BidAnyone;
};

struct BonusDef {
    BonusId id = BonusId::None;
    const char* key = "";
    BonusKind kind = BonusKind::Holding;
    int silent = 0;               // value when it happens without an announcement
    int announced = 0;
    std::int8_t targetTrick = -1; // 1-based trick the bound card must win, -1 = none
    Card boundCard;               // the card that has to win it (invalid = none)
    std::int8_t count = -1;       // tarock count, or the point target
    bool multiplier = false;      // multiplies the game value instead of adding to it
};

constexpr std::uint32_t bonusBit(BonusId id) { return 1u << static_cast<int>(id); }

// Optional rule switches the player can change in the settings; the defaults
// follow the tournament rules of the profile.
struct FlagSet {
    bool silentBonusesCountNegative = false;  // a failed silent bonus costs points
    bool valatTakesTalon = false;
    bool hardBirdReservation = false;         // playing a reserved bird is refused, not just warned
    int handsPerRound = 20;                   // five-player table: games per round
};

struct CountResult {
    int units = 0;                 // thirds (Austrian) or points (Hungarian)
    int cards = 0;
    int points() const;            // rounded points as players say them
};

class RuleProfile {
public:
    ProfileId id() const { return m_id; }
    const char* key() const { return m_key; }
    const DeckSpec& deck() const { return m_deck; }
    CountMode countMode() const { return m_countMode; }
    int winThreshold() const { return m_winThreshold; }     // in units
    int totalUnits() const { return m_totalUnits; }
    int tricks() const { return m_tricks; }
    int handCards() const { return m_handCards; }
    int talonSize() const { return m_talonSize; }
    const DealPlan& dealPlan() const { return m_dealPlan; }

    int cardValue(Card card) const;
    CountResult count(const CardSet& cards) const;

    const std::vector<ContractDef>& contracts() const { return m_contracts; }
    const ContractDef& contract(ContractId id) const;
    const std::vector<BonusDef>& bonuses() const { return m_bonuses; }
    const BonusDef& bonus(BonusId id) const;

    static const RuleProfile& get(ProfileId id);

private:
    friend struct RuleProfileBuilder;

    ProfileId m_id = ProfileId::AtKrOoe2023;
    const char* m_key = "";
    DeckSpec m_deck;
    CountMode m_countMode = CountMode::ThirdsAustrian;
    int m_winThreshold = 107;
    int m_totalUnits = 210;
    int m_tricks = 12;
    int m_handCards = 12;
    int m_talonSize = 6;
    DealPlan m_dealPlan;
    std::vector<ContractDef> m_contracts;
    std::vector<BonusDef> m_bonuses;
};

// Plain data holder so a profile file reads as a table.
struct RuleProfileBuilder {
    ProfileId id = ProfileId::AtKrOoe2023;
    const char* key = "";
    DeckSpec deck;
    CountMode countMode = CountMode::ThirdsAustrian;
    int winThreshold = 0;
    int totalUnits = 0;
    int tricks = 0;
    int handCards = 0;
    int talonSize = 0;
    DealPlan dealPlan;
    std::vector<ContractDef> contracts;
    std::vector<BonusDef> bonuses;

    RuleProfile build() const;
};

} // namespace tarock
