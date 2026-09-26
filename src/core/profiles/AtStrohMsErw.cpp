// Strohmandeln zu zweit, Regelprofil "AT-STROH-MS-ERW". Die Werte folgen
// docs/strohmandeln.md: §1.5 (Zählung), §1.7 (Gewinnschwelle und
// Unentschieden), §2.4 (Geben), §3.2 (Erklärung statt Lizit), §3.3 (Tarif der
// erweiterten Mayer-&-Schiffner-Variante), §5.11 (Prämien), §7.3 (Abrechnung).
//
// Gespielt wird mit demselben 54er-Blatt wie das Königrufen und mit derselben
// Schwelle (107 von 210 Dritteln, "35 und zwei Blatt"). Anders ist alles
// andere: zwei Sitze, kein Talon und kein Drücken, dafür je drei Strohmänner
// zu vier Karten, deren oberste offen liegt und mitgespielt wird (§4). Es gibt
// keine Bietleiter -- die Vorhand erklärt, ob sie aufnimmt, sonst der Geber;
// sagen beide "Weiter!", wird das einfache Spiel gespielt.
#include "../RuleProfile.h"

namespace tarock {
namespace {

// Angesagt wird hier nichts: alle Prämien sind still und werden am Ende der
// Partie abgelesen (§5.1). Der Rostopschin fehlt, weil er an Rufworte gebunden
// ist, der Quapil, weil die erweiterte Variante ihn nicht kennt (§5.11).
constexpr std::uint32_t kStrohBonuses =
        bonusBit(BonusId::Trull) | bonusBit(BonusId::AllKings) | bonusBit(BonusId::Pagat)
        | bonusBit(BonusId::Uhu) | bonusBit(BonusId::Kakadu) | bonusBit(BonusId::Grammel)
        | bonusBit(BonusId::Valat);

ContractDef contract(ContractId id, const char* key, int rank, int value, int lost,
                     bool drawIfNobodyWins, std::uint16_t flags)
{
    ContractDef def;
    def.id = id;
    def.key = key;
    def.rank = rank;
    def.baseValue = value;
    def.lostValue = lost;
    // Der Talon ist durch die Strohmänner ersetzt; es bleibt keine Karte liegen.
    def.talon = TalonMode::None;
    def.talonToDeclarer = 0;
    def.discardCount = 0;
    def.partner = PartnerMode::Alone;
    def.play = PlayMode::Positive;
    def.firstLead = LeadRule::Forehand;
    def.kontra = KontraMode::None;       // §5.11: kein Kontra
    def.bonusMask = kStrohBonuses;
    def.soloDoublesBonuses = false;
    def.bidFlags = flags;
    def.talonHalf = -1;
    def.drawIfNobodyWins = drawIfNobodyWins;
    return def;
}

BonusDef bonus(BonusId id, const char* key, BonusKind kind, int value, int targetTrick = -1,
               Card bound = Card(), int count = -1)
{
    BonusDef def;
    def.id = id;
    def.key = key;
    def.kind = kind;
    def.silent = value;
    def.announced = value;               // angesagt wird nichts, der Wert ist derselbe
    def.targetTrick = static_cast<std::int8_t>(targetTrick);
    def.boundCard = bound;
    def.count = static_cast<std::int8_t>(count);
    def.multiplier = false;              // der Valat kommt zum Spiel dazu (§7.4, Beispiel 4)
    return def;
}

} // namespace

const RuleProfile& makeStrohmandeln()
{
    static const RuleProfile profile = [] {
        RuleProfileBuilder builder;
        builder.id = ProfileId::AtStrohMsErw;
        builder.key = "AT-STROH-MS-ERW";
        builder.deck = DeckSpec{22, 8};              // "Industrie und Glück", 54 Karten
        builder.countMode = CountMode::ThirdsAustrian;
        builder.winThreshold = 107;                  // von 210 Dritteln (§1.7)
        builder.totalUnits = 210;
        builder.tricks = 27;
        builder.handCards = 15;                      // dazu kommen 12 aus den Strohmännern
        builder.talonSize = 24;                      // die sechs Päckchen, nicht ein Talon
        // Dreimal fünf Karten auf die Hand, dann die vierundzwanzig für die
        // Strohmänner (§2.4).
        builder.dealPlan = DealPlan{{5, 5, 5}, 24, 3};
        builder.seatCounts = {2};
        // Beide dürfen "Weiter!" sagen; dann wird einfach gespielt (§3.2).
        builder.forehandMustBid = false;
        builder.strawmen = 3;
        builder.strawmanSize = 4;
        builder.announcements = false;   // alles still, kein Kontra (§5.1, §5.11)

        builder.contracts = {
            // Einfaches Spiel: niemand hat aufgenommen. Gewonnen 1; erreicht
            // keiner der beiden die Schwelle, ist es unentschieden (§1.7.1).
            contract(ContractId::StrohEinfach, "EINFACH", 1, 1, 1, true, WhenAllPass),
            // Aufgenommenes Spiel: gewonnen 3 für den Aufnehmer, verloren 4
            // für den Gegner -- die Asymmetrie ist gewollt (§3.3, §7.4).
            contract(ContractId::StrohAufgenommen, "AUFGENOMMEN", 2, 3, 4, false, BidAnyone),
        };

        builder.bonuses = {
            bonus(BonusId::Trull, "TRULL", BonusKind::Holding, 1),
            bonus(BonusId::AllKings, "KOENIGE", BonusKind::Holding, 1),
            // Die drei Vögel zahlen in beide Richtungen: gelingt der Pagat im
            // 27. Stich, bekommt ihn sein Spieler, wird er dort gefangen, der
            // Fänger (§7.3 Zeilen 8-12).
            bonus(BonusId::Pagat, "PAGAT", BonusKind::TrickCapture, 1, 27, kPagat),
            bonus(BonusId::Uhu, "UHU", BonusKind::TrickCapture, 1, 26, tarockCard(2)),
            bonus(BonusId::Kakadu, "KAKADU", BonusKind::TrickCapture, 1, 25, tarockCard(3)),
            // Grammel-Punkt: wer in den eigenen Stichen 45 Punkte hat, bekommt
            // ihn, gleich ob er aufgenommen hat oder nicht (§7.2 Schritt 4,
            // Vorgabe M&S).
            bonus(BonusId::Grammel, "GRAMMEL", BonusKind::PointTarget, 1, -1, Card(), 45),
            bonus(BonusId::Valat, "VALAT", BonusKind::AllTricks, 12),
        };
        return builder.build();
    }();
    return profile;
}

} // namespace tarock
