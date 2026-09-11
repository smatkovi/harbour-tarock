// Königrufen, rule profile "AT-KR-OOE-2023-04" (Oberösterreichische
// Tarockregeln 4/2023). Values follow docs/koenigrufen.md §3.1 and §5.1.
#include "../RuleProfile.h"

namespace tarock {
namespace {

constexpr std::uint32_t kPositiveBonuses =
    bonusBit(BonusId::Trull) | bonusBit(BonusId::AllKings) | bonusBit(BonusId::Pagat)
    | bonusBit(BonusId::Uhu) | bonusBit(BonusId::Kakadu) | bonusBit(BonusId::Quapil)
    | bonusBit(BonusId::Valat);
// Ruferspiele add the called king; games where the declarer plays alone with
// the talon cannot have it.
constexpr std::uint32_t kCallerBonuses = kPositiveBonuses | bonusBit(BonusId::KingUltimo);
// Farbenspiele: only all kings and Valat (docs/koenigrufen.md §3.1).
constexpr std::uint32_t kSuitGameBonuses = bonusBit(BonusId::AllKings) | bonusBit(BonusId::Valat);

ContractDef contract(ContractId id, const char* key, int rank, int value, TalonMode talon,
                     std::int8_t talonToDeclarer, std::int8_t discard, PartnerMode partner,
                     PlayMode play, LeadRule lead, KontraMode kontra, std::uint32_t bonuses,
                     bool soloDoubles = false, std::uint16_t flags = BidAnyone, int lostValue = -1)
{
    ContractDef def;
    def.id = id;
    def.key = key;
    def.rank = rank;
    def.baseValue = value;
    def.lostValue = lostValue < 0 ? value : lostValue;
    def.talon = talon;
    def.talonToDeclarer = talonToDeclarer;
    def.discardCount = discard;
    def.partner = partner;
    def.play = play;
    def.firstLead = lead;
    def.kontra = kontra;
    def.bonusMask = bonuses;
    def.soloDoublesBonuses = soloDoubles;
    def.bidFlags = flags;
    return def;
}

BonusDef bonus(BonusId id, const char* key, BonusKind kind, int silent, int announced,
               int targetTrick = -1, Card bound = Card(), bool multiplier = false)
{
    BonusDef def;
    def.id = id;
    def.key = key;
    def.kind = kind;
    def.silent = silent;
    def.announced = announced;
    def.targetTrick = static_cast<std::int8_t>(targetTrick);
    def.boundCard = bound;
    def.multiplier = multiplier;
    return def;
}

} // namespace

const RuleProfile& makeKoenigrufen()
{
    static const RuleProfile profile = [] {
        RuleProfileBuilder builder;
        builder.id = ProfileId::AtKrOoe2023;
        builder.key = "AT-KR-OOE-2023-04";
        builder.deck = DeckSpec{22, 8};
        builder.countMode = CountMode::ThirdsAustrian;
        builder.winThreshold = 107;   // of 210 thirds, i.e. 35 2/3 points
        builder.totalUnits = 210;
        builder.tricks = 12;
        builder.handCards = 12;
        builder.talonSize = 6;
        builder.dealPlan = DealPlan{{6, 6}, 6, 1};   // six each, talon, six each

        builder.contracts = {
            contract(ContractId::Trischaken, "TRISCHAKEN", 1, 1, TalonMode::ToLastTrick, 0, 0,
                     PartnerMode::EveryoneAlone, PlayMode::Trischaken, LeadRule::Forehand,
                     KontraMode::None, 0, false, ForehandOnly | ForehandLastOnly),
            contract(ContractId::Rufer, "RUFER", 2, 1, TalonMode::OpenHalves, 3, 3,
                     PartnerMode::CallKing, PlayMode::Positive, LeadRule::Forehand,
                     KontraMode::Party, kCallerBonuses, false, ForehandOnly | ForehandLastOnly),
            contract(ContractId::Piccolo, "PICCOLO", 3, 2, TalonMode::None, 0, 0,
                     PartnerMode::Alone, PlayMode::Negative, LeadRule::Declarer,
                     KontraMode::Individual, 0),
            contract(ContractId::Bettel, "BETTEL", 4, 2, TalonMode::None, 0, 0,
                     PartnerMode::Alone, PlayMode::Negative, LeadRule::Declarer,
                     KontraMode::Individual, 0),
            contract(ContractId::Solorufer, "SOLORUFER", 5, 2, TalonMode::ToOpponents, 0, 0,
                     PartnerMode::CallKing, PlayMode::Positive, LeadRule::Forehand,
                     KontraMode::Party, kCallerBonuses, true),
            contract(ContractId::Besserrufer, "BESSERRUFER", 6, 1, TalonMode::OpenHalves, 3, 3,
                     PartnerMode::CallKing, PlayMode::Positive, LeadRule::Forehand,
                     KontraMode::Party, kCallerBonuses, false, RequiresBird),
            contract(ContractId::Farbendreier, "FARBENDREIER", 7, 3, TalonMode::OpenHalves, 3, 3,
                     PartnerMode::Alone, PlayMode::SuitGame, LeadRule::Forehand,
                     KontraMode::Party, kSuitGameBonuses),
            contract(ContractId::Sechserdreier, "SECHSERDREIER", 8, 4, TalonMode::AllHidden, 6, 6,
                     PartnerMode::Alone, PlayMode::Positive, LeadRule::Forehand,
                     KontraMode::Party, kPositiveBonuses, false, ForehandOnly | ForehandFirstOnly, 8),
            contract(ContractId::Dreier, "DREIER", 9, 4, TalonMode::OpenHalves, 3, 3,
                     PartnerMode::Alone, PlayMode::Positive, LeadRule::Forehand,
                     KontraMode::Party, kPositiveBonuses),
            contract(ContractId::Farbensolo, "FARBENSOLO", 10, 6, TalonMode::ToOpponents, 0, 0,
                     PartnerMode::Alone, PlayMode::SuitGame, LeadRule::Forehand,
                     KontraMode::Party, kSuitGameBonuses, true),
            contract(ContractId::PiccoloOuvert, "PICCOLO_OUVERT", 11, 6, TalonMode::None, 0, 0,
                     PartnerMode::Alone, PlayMode::Negative, LeadRule::Declarer,
                     KontraMode::Individual, 0),
            contract(ContractId::BettelOuvert, "BETTEL_OUVERT", 12, 7, TalonMode::None, 0, 0,
                     PartnerMode::Alone, PlayMode::Negative, LeadRule::Declarer,
                     KontraMode::Individual, 0),
            contract(ContractId::Solodreier, "SOLODREIER", 13, 8, TalonMode::ToOpponents, 0, 0,
                     PartnerMode::Alone, PlayMode::Positive, LeadRule::Forehand,
                     KontraMode::Party, kPositiveBonuses, true, AnnounceWithBid),
        };

        builder.bonuses = {
            bonus(BonusId::Trull, "TRULL", BonusKind::Holding, 1, 2),
            bonus(BonusId::AllKings, "ALLE_KOENIGE", BonusKind::Holding, 1, 2),
            bonus(BonusId::KingUltimo, "KOENIG_ULTIMO", BonusKind::TrickWithCard, 1, 2, 12),
            bonus(BonusId::Pagat, "PAGAT", BonusKind::TrickWithCard, 1, 2, 12, kPagat),
            bonus(BonusId::Uhu, "UHU", BonusKind::TrickWithCard, 2, 4, 11, tarockCard(2)),
            bonus(BonusId::Kakadu, "KAKADU", BonusKind::TrickWithCard, 3, 6, 10, tarockCard(3)),
            bonus(BonusId::Quapil, "QUAPIL", BonusKind::TrickWithCard, 4, 8, 9, tarockCard(4)),
            bonus(BonusId::Valat, "VALAT", BonusKind::AllTricks, 4, 8, -1, Card(), true),
        };
        return builder.build();
    }();
    return profile;
}

} // namespace tarock
