// Tapp-Tarock zu dritt, Regelprofil "AT-TAPP-KLASSIK". Die Werte folgen
// docs/tapptarock.md: §1 (Blatt und Zählung), §2.4 (Geben), §3.1 (Spielstufen),
// §4 (Talon), §5.2/§5.5 (Pagat ultimo, Valat), §7.3 (Tarif).
//
// Das Spiel ist das des Königrufens mit demselben 54er-Blatt und derselben
// Gewinnschwelle (107 von 210 Dritteln, also "35 und zwei Blatt"); anders sind
// drei Sitze, der Alleinspieler ohne Partner und die Leiter
// Dreier < Unterer < Oberer < Solo. Unterer und Oberer bezeichnen die
// Talonhälfte, auf die sich der Spieler festlegt, bevor sie aufgedeckt wird
// (§4.2, Lesart A) -- nicht eine Kartenzahl.
#include "../RuleProfile.h"

namespace tarock {
namespace {

// Zu holen sind hier nur zwei Prämien: der Pagat ultimo und der Valat. Trull,
// Königstrull und Köpfe sind Handprämien, die jeder Spieler für sich kassiert
// (§5.3); sie gehören nicht in die Parteienabrechnung und kommen erst mit dem
// Postenmodell für Einzelzahlungen dazu.
constexpr std::uint32_t kTappBonuses = bonusBit(BonusId::Pagat) | bonusBit(BonusId::Valat);


ContractDef contract(ContractId id, const char* key, int rank, int value, TalonMode talon,
                     std::int8_t talonToDeclarer, std::int8_t discard, std::int8_t talonHalf,
                     bool soloDoubles = false, std::uint16_t flags = BidAnyone)
{
    ContractDef def;
    def.id = id;
    def.key = key;
    def.rank = rank;
    def.baseValue = value;
    def.lostValue = value;
    def.talon = talon;
    def.talonToDeclarer = talonToDeclarer;
    def.discardCount = discard;
    def.partner = id == ContractId::Trischaken ? PartnerMode::EveryoneAlone : PartnerMode::Alone;
    def.play = id == ContractId::Trischaken ? PlayMode::Trischaken : PlayMode::Positive;
    def.firstLead = LeadRule::Forehand;
    def.kontra = id == ContractId::Trischaken ? KontraMode::None : KontraMode::Party;
    def.bonusMask = id == ContractId::Trischaken ? 0u : kTappBonuses;
    def.soloDoublesBonuses = soloDoubles;
    def.bidFlags = flags;
    def.talonHalf = talonHalf;
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

const RuleProfile& makeTappTarock()
{
    static const RuleProfile profile = [] {
        RuleProfileBuilder builder;
        builder.id = ProfileId::AtTappKlassik;
        builder.key = "AT-TAPP-KLASSIK";
        builder.deck = DeckSpec{22, 8};              // "Industrie und Glück", 54 Karten
        builder.countMode = CountMode::ThirdsAustrian;
        builder.winThreshold = 107;                  // von 210 Dritteln (§1.7)
        builder.totalUnits = 210;
        builder.tricks = 16;
        builder.handCards = 16;
        builder.talonSize = 6;
        // Zuerst der Talon, dann viermal reihum je vier Karten (§2.4,
        // Vorgabe `gebefolge = talon_zuerst_4er`).
        builder.dealPlan = DealPlan{{4, 4, 4, 4}, 6, 0};
        builder.seatCounts = {3};
        // Die Vorhand darf "Weiter!" sagen; passen alle, wird trischakt (§3.5).
        builder.forehandMustBid = false;
        builder.trischakenStyle = TrischakenStyle::PerOpponent;
        builder.trischakenValue = 5;                 // §7.6: 5 an jeden anderen

        builder.contracts = {
            // Passen alle, wird jeder gegen jeden gespielt; der Talon bleibt
            // dabei unberührt und zählt für niemanden (§3.5.3).
            contract(ContractId::Trischaken, "TRISCHAKEN", 1, 0, TalonMode::None, 0, 0, -1,
                     false, WhenAllPass),
            contract(ContractId::TappDreier, "DREIER", 2, 3, TalonMode::OpenHalves, 3, 3, -1),
            contract(ContractId::TappUnterer, "UNTERER", 3, 4, TalonMode::OpenHalves, 3, 3, 0),
            contract(ContractId::TappOberer, "OBERER", 4, 5, TalonMode::OpenHalves, 3, 3, 1),
            // Solo: der ganze Talon bleibt liegen und zählt für die Gegner,
            // und er verdoppelt den Pagat ultimo (§3.1, §5.2).
            contract(ContractId::TappSolo, "SOLO", 5, 8, TalonMode::ToOpponents, 0, 0, -1, true),
        };

        builder.bonuses = {
            // Pagat ultimo: still 4, angesagt 8, im Solo das Doppelte (§5.2).
            // Der letzte Stich ist hier der sechzehnte.
            bonus(BonusId::Pagat, "PAGAT", BonusKind::TrickWithCard, 4, 8, 16, kPagat),
            // Valat vervielfacht den Spielwert (§5.5, §7.3).
            bonus(BonusId::Valat, "VALAT", BonusKind::AllTricks, 4, 8, -1, Card(), true),
        };
        return builder.build();
    }();
    return profile;
}

} // namespace tarock
