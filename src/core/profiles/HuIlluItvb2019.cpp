// Hungarian "Illusztrált tarokk", rule profile "HU-ILLU-ITVB-2019" (tournament
// rules of the ITVB). Values follow docs/hungarian.md §3 and §5.
#include "../RuleProfile.h"

namespace tarock {
namespace {

constexpr std::uint32_t kFigures =
    bonusBit(BonusId::Trull) | bonusBit(BonusId::AllKings) | bonusBit(BonusId::Duplajatek)
    | bonusBit(BonusId::Valat) | bonusBit(BonusId::Pagat) | bonusBit(BonusId::SasUltimo)
    | bonusBit(BonusId::XXIFogas) | bonusBit(BonusId::Centrum) | bonusBit(BonusId::Kismadar)
    | bonusBit(BonusId::Nagymadar) | bonusBit(BonusId::PagatUhu) | bonusBit(BonusId::SasUhu)
    | bonusBit(BonusId::KingUltimo) | bonusBit(BonusId::KiralyUhu)
    | bonusBit(BonusId::Tarokk8) | bonusBit(BonusId::Tarokk9);

ContractDef contract(ContractId id, const char* key, int rank, int value, std::int8_t talonCards)
{
    ContractDef def;
    def.id = id;
    def.key = key;
    def.rank = rank;
    def.baseValue = value;
    def.lostValue = value;
    def.talon = TalonMode::Distributed;
    def.talonToDeclarer = talonCards;
    def.discardCount = talonCards;   // as many cards as taken are discarded again
    def.partner = PartnerMode::CallTarock;
    def.play = PlayMode::Positive;
    def.firstLead = LeadRule::Forehand;
    def.kontra = KontraMode::Party;
    def.bonusMask = kFigures;
    return def;
}

BonusDef figure(BonusId id, const char* key, BonusKind kind, int announced, int silent,
                int targetTrick = -1, Card bound = Card(), int count = -1, bool multiplier = false)
{
    BonusDef def;
    def.id = id;
    def.key = key;
    def.kind = kind;
    def.announced = announced;
    def.silent = silent;
    def.targetTrick = static_cast<std::int8_t>(targetTrick);
    def.boundCard = bound;
    def.count = static_cast<std::int8_t>(count);
    def.multiplier = multiplier;
    return def;
}

} // namespace

const RuleProfile& makeHungarian()
{
    static const RuleProfile profile = [] {
        RuleProfileBuilder builder;
        builder.id = ProfileId::HuIlluItvb2019;
        builder.key = "HU-ILLU-ITVB-2019";
        builder.deck = DeckSpec{22, 5};        // 42 cards: the three lowest of each suit are out
        builder.countMode = CountMode::PlainHungarian;
        builder.winThreshold = 48;             // of 94 points
        builder.totalUnits = 94;
        builder.tricks = 9;
        builder.handCards = 9;
        builder.talonSize = 6;
        builder.dealPlan = DealPlan{{5, 4}, 6, 0};   // talon first, then five and four

        builder.contracts = {
            contract(ContractId::Harom, "HAROM", 1, 1, 3),
            contract(ContractId::Ketto, "KETTO", 2, 2, 2),
            contract(ContractId::Egy, "EGY", 3, 3, 1),
            contract(ContractId::Szolo, "SZOLO", 4, 4, 0),
        };

        builder.bonuses = {
            figure(BonusId::Trull, "TULETROA", BonusKind::Holding, 2, 1),
            figure(BonusId::AllKings, "NEGYKIRALY", BonusKind::Holding, 2, 1),
            figure(BonusId::Duplajatek, "DUPLAJATEK", BonusKind::PointTarget, 4, 2, -1, Card(), 71, true),
            figure(BonusId::Valat, "VOLAT", BonusKind::AllTricks, 6, 3, -1, Card(), -1, true),
            figure(BonusId::Pagat, "PAGAT_ULTIMO", BonusKind::TrickWithCard, 10, 5, 9, kPagat),
            figure(BonusId::SasUltimo, "SAS_ULTIMO", BonusKind::TrickWithCard, 10, 5, 9, tarockCard(2)),
            figure(BonusId::XXIFogas, "XXI_FOGAS", BonusKind::Capture, 42, 21, -1, kMond),
            figure(BonusId::Centrum, "CENTRUM", BonusKind::TrickRun, 10, 0, 5, tarockCard(20)),
            figure(BonusId::Kismadar, "KISMADAR", BonusKind::TrickRun, 10, 0, 6, kMond),
            figure(BonusId::Nagymadar, "NAGYMADAR", BonusKind::TrickRun, 10, 0, 7, kSkues),
            figure(BonusId::PagatUhu, "PAGAT_UHU", BonusKind::TrickWithCard, 15, 0, 8, kPagat),
            figure(BonusId::SasUhu, "SAS_UHU", BonusKind::TrickWithCard, 15, 0, 8, tarockCard(2)),
            figure(BonusId::KingUltimo, "KIRALY_ULTIMO", BonusKind::TrickWithCard, 15, 0, 9),
            figure(BonusId::KiralyUhu, "KIRALY_UHU", BonusKind::TrickWithCard, 20, 0, 8),
            figure(BonusId::Tarokk8, "TAROKK_8", BonusKind::TarockCount, 1, 1, -1, Card(), 8),
            figure(BonusId::Tarokk9, "TAROKK_9", BonusKind::TarockCount, 2, 2, -1, Card(), 9),
        };
        return builder.build();
    }();
    return profile;
}

} // namespace tarock
