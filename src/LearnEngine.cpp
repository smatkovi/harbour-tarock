/*
    Copyright (C) 2026 smatkovi

    This file is part of harbour-tarock.

    harbour-tarock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    harbour-tarock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with harbour-tarock. If not, see <https://www.gnu.org/licenses/>.

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "LearnEngine.h"

#include "ReasonText.h"
#include "RulesIndex.h"
#include "TarockEngine.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

#include <algorithm>
#include <vector>

using tarock::Action;
using tarock::ActionType;
using tarock::BonusId;
using tarock::Card;
using tarock::CardSet;
using tarock::ContractId;
using tarock::CountResult;
using tarock::Declaration;
using tarock::Hint;
using tarock::HintReason;
using tarock::Phase;
using tarock::PlayMode;
using tarock::ProfileId;
using tarock::Reason;
using tarock::ReasonCode;
using tarock::RuleProfile;
using tarock::Severity;
using tarock::Suit;
using tarock::TarockCore;

namespace {

const char* const kLevelKey = "learn/level";
const char* const kAutoHintKey = "learn/autoHint";
const char* const kWarnBonusKey = "learn/warnBonusLoss";
const char* const kDimIllegalKey = "learn/dimIllegal";
const char* const kCountTutorKey = "learn/countTutor";
const char* const kPlayToEndKey = "learn/playToEnd";
// The guided tour has run once, and the lessons finished per rule profile
// (docs/design.md Â§7.9). The profile stem is appended to the progress key.
const char* const kTourSeenKey = "learn/tourSeen";
const char* const kProgressKey = "learn/progress/";

const int kHistoryLength = 10;
const int kHintBudgetMs = 80;

struct ActionName {
    ActionType type;
    const char* name;
};

// The vocabulary of TarockEngine::act(), so QML asks "why" with the same word
// it would act with.
const ActionName kActionNames[] = {
    {ActionType::Bid, "bid"},
    {ActionType::Pass, "pass"},
    {ActionType::Hold, "hold"},
    {ActionType::OpenForehand, "vorhand"},
    {ActionType::CallKing, "callking"},
    {ActionType::CallTarock, "calltarock"},
    {ActionType::TakeTalon, "taketalon"},
    {ActionType::Concede, "concede"},
    {ActionType::Discard, "discard"},
    {ActionType::ConfirmDiscard, "confirmdiscard"},
    {ActionType::AnnounceBonus, "bonus"},
    {ActionType::Kontra, "kontra"},
    {ActionType::Ready, "ready"},
    {ActionType::PlayCard, "play"},
};

ActionType actionTypeOf(const QString& name)
{
    for (const ActionName& entry : kActionNames) {
        if (name == QLatin1String(entry.name))
            return entry.type;
    }
    return ActionType::None;
}

QString actionNameOf(ActionType type)
{
    for (const ActionName& entry : kActionNames) {
        if (entry.type == type)
            return QString::fromLatin1(entry.name);
    }
    return QString();
}

// Which beginner mistake of koenigrufen.md §11.5 a refusal is an instance of.
struct MistakeLink {
    ReasonCode code;
    int mistake;
};

const MistakeLink kMistakeLinks[] = {
    {ReasonCode::CallOwnKing, 4},
    {ReasonCode::CallerHasFourKings, 4},
    {ReasonCode::DiscardKing, 5},
    {ReasonCode::DiscardTrull, 5},
    {ReasonCode::DiscardTarockWhileSuit, 5},
    {ReasonCode::MustTarock, 6},
    {ReasonCode::PagatNotLastTarock, 7},
    {ReasonCode::SuitGameNoTarockLead, 8},
    {ReasonCode::W_BirdReserved, 9},
    {ReasonCode::BoundWinsTooEarly, 9},
    {ReasonCode::W_KingUltimoReserved, 9},
    {ReasonCode::BoundOrder, 9},
    {ReasonCode::FollowSuit, 6},
    {ReasonCode::FollowTarock, 6},
};

int mistakeFor(ReasonCode code)
{
    for (const MistakeLink& link : kMistakeLinks) {
        if (link.code == code)
            return link.mistake;
    }
    return 0;
}

const char* const kProfileStemAt = "at-kr-ooe";
const char* const kProfileStemHu = "hu-illu";
// Für das Tapp-Tarock gibt es noch keine Lektionen; der eigene Stamm sorgt
// dafür, dass dort auch keine fremden auftauchen (docs/design.md §12, M11).
const char* const kProfileStemTapp = "at-tapp";
const char* const kProfileStemStroh = "at-stroh";

const char* profileStemOf(ProfileId profile)
{
    switch (profile) {
    case ProfileId::HuIlluItvb2019: return kProfileStemHu;
    case ProfileId::AtTappKlassik:  return kProfileStemTapp;
    case ProfileId::AtStrohMsErw:   return kProfileStemStroh;
    default:                        return kProfileStemAt;
    }
}

} // namespace

// --- construction -------------------------------------------------------------------

LearnEngine::LearnEngine(TarockEngine* engine, QObject* parent)
    : QObject(parent), m_engine(engine)
{
    loadSettings();
    m_panelOpen = m_level == Novice;
}

LearnEngine::~LearnEngine() = default;

const TarockCore& LearnEngine::core() const
{
    return m_engine->core();
}

ProfileId LearnEngine::profileId() const
{
    return core().profile().id();
}

bool LearnEngine::ready() const
{
    return m_engine != nullptr && m_engine->active();
}

// --- settings, docs/design.md §7.8 --------------------------------------------------

void LearnEngine::loadSettings()
{
    QSettings settings;
    m_level = static_cast<LearnLevel>(
        qBound(0, settings.value(QLatin1String(kLevelKey), static_cast<int>(m_level)).toInt(), 2));
    m_autoHint = settings.value(QLatin1String(kAutoHintKey), m_autoHint).toBool();
    m_warnBonusLoss = settings.value(QLatin1String(kWarnBonusKey), m_warnBonusLoss).toBool();
    m_dimIllegal = settings.value(QLatin1String(kDimIllegalKey), m_dimIllegal).toBool();
    m_countTutor = settings.value(QLatin1String(kCountTutorKey), m_countTutor).toBool();
    m_playToEnd = settings.value(QLatin1String(kPlayToEndKey), m_playToEnd).toBool();
    m_tourSeen = settings.value(QLatin1String(kTourSeenKey), m_tourSeen).toBool();
}

void LearnEngine::saveSettings()
{
    QSettings settings;
    settings.setValue(QLatin1String(kLevelKey), static_cast<int>(m_level));
    settings.setValue(QLatin1String(kAutoHintKey), m_autoHint);
    settings.setValue(QLatin1String(kWarnBonusKey), m_warnBonusLoss);
    settings.setValue(QLatin1String(kDimIllegalKey), m_dimIllegal);
    settings.setValue(QLatin1String(kCountTutorKey), m_countTutor);
    settings.setValue(QLatin1String(kPlayToEndKey), m_playToEnd);
    settings.setValue(QLatin1String(kTourSeenKey), m_tourSeen);
    settings.sync();
}

void LearnEngine::setLevel(int value)
{
    const LearnLevel level = static_cast<LearnLevel>(qBound(0, value, 2));
    if (level == m_level)
        return;
    m_level = level;
    saveSettings();
    // The panel is permanently open for a novice (§7.1).
    if (m_level == Novice && !m_panelOpen) {
        m_panelOpen = true;
        emit panelChanged();
    }
    emit settingsChanged();
    refresh();
}

void LearnEngine::setPanelOpen(bool value)
{
    if (value == m_panelOpen)
        return;
    m_panelOpen = value;
    emit panelChanged();
}

void LearnEngine::setAutoHint(bool value)
{
    if (value == m_autoHint)
        return;
    m_autoHint = value;
    saveSettings();
    emit settingsChanged();
}

void LearnEngine::setWarnBonusLoss(bool value)
{
    if (value == m_warnBonusLoss)
        return;
    m_warnBonusLoss = value;
    saveSettings();
    emit settingsChanged();
}

void LearnEngine::setDimIllegal(bool value)
{
    if (value == m_dimIllegal)
        return;
    m_dimIllegal = value;
    saveSettings();
    emit settingsChanged();
}

void LearnEngine::setCountTutor(bool value)
{
    if (value == m_countTutor)
        return;
    m_countTutor = value;
    saveSettings();
    emit settingsChanged();
}

void LearnEngine::setPlayToEnd(bool value)
{
    if (value == m_playToEnd)
        return;
    m_playToEnd = value;
    saveSettings();
    emit settingsChanged();
}

// --- "Warum nicht?", docs/design.md §7.3 --------------------------------------------

Reason LearnEngine::enrich(const Reason& in) const
{
    Reason out = in;
    if (!ready())
        return out;
    const TarockCore& c = core();
    const CardSet& own = c.hand(kLocalSeat);

    switch (out.code) {
    case ReasonCode::FollowSuit:
    case ReasonCode::FollowTarock:
    case ReasonCode::MustTarock:
    case ReasonCode::MustOvertrumpSuit:
    case ReasonCode::MustOvertrumpTarock:
    case ReasonCode::MustTrumpNegative:
    case ReasonCode::BoundMustPlay:
    case ReasonCode::BoundOrder:
    case ReasonCode::BoundWinsTooEarly:
    case ReasonCode::W_BirdReserved:
        // "Erlaubt sind: …" is exactly what the core would accept now.
        if (out.cards.none())
            out.cards = c.playableCards(kLocalSeat);
        if (out.card.valid() && out.cards.test(out.card.id))
            out.cards.reset(out.card.id);
        if (out.suit < 0 && !c.trick().empty())
            out.suit = static_cast<std::int16_t>(c.trick().front().suit());
        break;
    case ReasonCode::PagatNotLastTarock:
        // The text names the other tarocks, not the Pagat itself.
        if (out.cards.none()) {
            for (Card card : tarock::toList(own)) {
                if (card.tarock() && card != tarock::kPagat)
                    out.cards.set(card.id);
            }
        }
        break;
    case ReasonCode::SuitGameNoTarockLead:
        if (out.cards.none()) {
            for (Card card : tarock::toList(own)) {
                if (!card.tarock())
                    out.cards.set(card.id);
            }
        }
        break;
    case ReasonCode::DiscardTarockWhileSuit:
        if (out.cards.none()) {
            const CardSet& tray = c.discardTray(kLocalSeat);
            for (Card card : tarock::toList(own)) {
                if (!card.tarock() && !card.king() && !tarock::contains(tray, card))
                    out.cards.set(card.id);
            }
        }
        break;
    case ReasonCode::DiscardSuitWhileTarock:
        if (out.cards.none()) {
            const CardSet& tray = c.discardTray(kLocalSeat);
            for (Card card : tarock::toList(own)) {
                if (card.tarock() && !card.honour() && !tarock::contains(tray, card))
                    out.cards.set(card.id);
            }
        }
        break;
    case ReasonCode::DiscardCount:
        // The sentence of §11.4 names the wanted number and the current one;
        // Reason has no field of its own for the latter, so it travels in
        // `trick`, which no discard reason uses.
        out.trick = static_cast<std::int16_t>(c.discardTray(kLocalSeat).count());
        break;
    case ReasonCode::NotYourTurn:
        if (out.seat < 0)
            out.seat = static_cast<std::int16_t>(c.actor());
        break;
    default:
        break;
    }
    if (out.contract < 0 && c.contract() != ContractId::None)
        out.contract = static_cast<std::int16_t>(c.contract());
    return out;
}

QVariantMap LearnEngine::why(const QString& type, int a, int b) const
{
    Reason reason;
    if (!ready()) {
        reason.code = ReasonCode::WrongPhase;
        return ReasonText::toMap(reason, profileId());
    }
    const ActionType actionType = actionTypeOf(type);
    if (actionType == ActionType::None) {
        reason.code = ReasonCode::UnknownAction;
        return ReasonText::toMap(reason, profileId());
    }
    // check(), never apply(): asking must not change the hand (§7.2).
    const Action action(actionType, static_cast<std::int16_t>(a), static_cast<std::int16_t>(b));
    QVariantMap map = ReasonText::toMap(enrich(core().check(kLocalSeat, action)), profileId());
    map.insert(QStringLiteral("action"), type);
    map.insert(QStringLiteral("a"), a);
    map.insert(QStringLiteral("b"), b);
    return map;
}

QVariantMap LearnEngine::whyCard(int cardId) const
{
    Reason reason;
    if (!ready()) {
        reason.code = ReasonCode::WrongPhase;
        return ReasonText::toMap(reason, profileId());
    }
    const Card card(static_cast<std::uint8_t>(cardId));
    if (cardId < 0 || !card.valid()) {
        reason.code = ReasonCode::NotYourCard;
        return ReasonText::toMap(reason, profileId());
    }
    QVariantMap map = ReasonText::toMap(enrich(core().cardReason(kLocalSeat, card)), profileId());
    map.insert(QStringLiteral("action"), QStringLiteral("play"));
    map.insert(QStringLiteral("a"), cardId);
    // The fan greys out what is refused and highlights what is allowed (§7.8).
    QVariantList playable;
    for (Card allowed : tarock::toList(core().playableCards(kLocalSeat)))
        playable.append(static_cast<int>(allowed.id));
    map.insert(QStringLiteral("playable"), playable);
    return map;
}

void LearnEngine::noteRefusal(const Reason& reason, const QString& type, int a, int b)
{
    QVariantMap entry = ReasonText::toMap(enrich(reason), profileId());
    entry.insert(QStringLiteral("action"), type);
    entry.insert(QStringLiteral("a"), a);
    entry.insert(QStringLiteral("b"), b);
    entry.insert(QStringLiteral("trickNumber"), ready() ? core().trickNumber() : 0);
    m_history.prepend(entry);
    while (m_history.size() > kHistoryLength)
        m_history.removeLast();
    const int mistake = mistakeFor(reason.code);
    if (mistake > 0 && !m_mistakesSeen.contains(mistake))
        m_mistakesSeen.append(mistake);
    emit historyChanged();
}

// --- the hint engine, docs/design.md §7.4 -------------------------------------------

QString LearnEngine::hintShort(HintReason reason)
{
    switch (reason) {
    case HintReason::None: return tr("Möglicher Zug");
    case HintReason::HandTooWeak: return tr("Blatt zu schwach");
    case HintReason::HandStrongEnough: return tr("Blatt reicht");
    case HintReason::BirdCountSufficient: return tr("Vogel gedeckt");
    case HintReason::SoloWorthIt: return tr("Solo lohnt sich");
    case HintReason::ContractEconomy: return tr("Preis und Risiko");
    case HintReason::SeatPosition: return tr("Sitzposition");
    case HintReason::ForehandMustChoose: return tr("Die Vorhand wählt");
    case HintReason::CallFromGuardedQueen: return tr("König bei eigener Dame");
    case HintReason::CallShortSuit: return tr("Kurze Farbe rufen");
    case HintReason::PreferTarockOverPoints: return tr("Tarock vor Punkten");
    case HintReason::VoidASuit: return tr("Farbe blank machen");
    case HintReason::KeepCalledSuit: return tr("Rufffarbe halten");
    case HintReason::ProtectKing: return tr("König schützen");
    case HintReason::TalonHalfRicher: return tr("Die reichere Hälfte");
    case HintReason::BonusSafe: return tr("Prämie ist sicher");
    case HintReason::BonusRisky: return tr("Prämie ist riskant");
    case HintReason::KontraWorthwhile: return tr("Kontra lohnt sich");
    case HintReason::SignalToPartner: return tr("Signal an den Partner");
    case HintReason::NothingToAnnounce: return tr("Nichts anzusagen");
    case HintReason::DrawTrumps: return tr("Tarock ziehen");
    case HintReason::SmearToPartner: return tr("Schmieren");
    case HintReason::DuckCheaply: return tr("Billig abwerfen");
    case HintReason::KeepStopper: return tr("Stecher behalten");
    case HintReason::LeadShortKing: return tr("Kurzen König anspielen");
    case HintReason::SaveBirdForTarget: return tr("Vogel aufheben");
    case HintReason::LastTrickPlan: return tr("Plan für den letzten Stich");
    case HintReason::CountSaysEnough: return tr("Die Punkte reichen");
    case HintReason::CountSaysMustAttack: return tr("Es fehlen Punkte");
    case HintReason::OnlyOneCard: return tr("Die einzige Karte");
    case HintReason::FollowSuitForced: return tr("Farbzwang");
    }
    return tr("Möglicher Zug");
}

QString LearnEngine::hintText(HintReason reason, const Reason& detail)
{
    const QString card = detail.card.valid() ? ReasonText::cardName(detail.card) : QString();
    switch (reason) {
    case HintReason::None:
        return tr("Dieser Zug ist erlaubt; er bringt aber keinen besonderen Vorteil.");
    case HintReason::HandTooWeak:
        return tr("Für ein Gebot ist dein Blatt zu schwach: zu wenige Tarock, zu wenige "
                  "Stecher. Sag „weiter“ – wer nichts wagt, verliert hier auch nichts.");
    case HintReason::HandStrongEnough:
        return tr("Dein Blatt trägt dieses Spiel: genug Tarock mit Stechern, dazu Figuren "
                  "zum Schmieren.");
    case HintReason::BirdCountSufficient:
        return tr("Du hast genug hohe Tarock, um den Vogel bis zu seinem Stich "
                  "durchzubringen – das verlangt der Besserrufer.");
    case HintReason::SoloWorthIt:
        return tr("Mit so vielen hohen Tarock brauchst du den Talon nicht. Im Solospiel "
                  "zählt außerdem jede Prämie doppelt.");
    case HintReason::ContractEconomy:
        return tr("Dieses Spiel passt zum Blatt: Ein höheres Gebot kostet im Verlustfall "
                  "mehr, als es im Gewinnfall einbringt.");
    case HintReason::SeatPosition:
        return tr("Nach dir kommen noch Spieler zu Wort. Leg dich nicht zu früh fest.");
    case HintReason::ForehandMustChoose:
        return tr("Als Vorhand musst du dich erklären. Mit „Vorhand“ hältst du dir alles "
                  "offen; den Sechserdreier musst du sofort ansagen, sonst ist er weg.");
    case HintReason::CallFromGuardedQueen:
        return tr("Ruf aus einer Farbe, in der du die Dame hast: Dein Partner kann den König "
                  "dann sicher heimbringen, und du deckst ihn von hinten.");
    case HintReason::CallShortSuit:
        return tr("Ruf aus einer kurzen Farbe. So wirst du sie schnell los und kannst sie "
                  "danach stechen.");
    case HintReason::PreferTarockOverPoints:
        return tr("Nimm die Hälfte mit den Tarock: Stiche gewinnst du mit Tarock, nicht mit "
                  "Punkten. Punkte kannst du später noch einsammeln.");
    case HintReason::VoidASuit:
        return tr("Verleg diese Karte, dann bist du in der Farbe blank und kannst sie "
                  "künftig abstechen.");
    case HintReason::KeepCalledSuit:
        return tr("Behalte die gerufene Farbe: Über sie findet ihr euch als Partner.");
    case HintReason::ProtectKing:
        return tr("Der König braucht Deckung. Gib die kleinen Karten dieser Farbe nicht her, "
                  "sonst steht er nackt.");
    case HintReason::TalonHalfRicher:
        return tr("Diese Talonhälfte bringt mehr. Denk daran: Die andere Hälfte zählt am Ende "
                  "zu den Gegnern.");
    case HintReason::BonusSafe:
        return tr("Diese Prämie ist mit deinem Blatt sicher genug für eine Ansage – angesagt "
                  "zählt sie doppelt.");
    case HintReason::BonusRisky:
        return tr("Das ist zu unsicher. Versuch es still: Ohne Ansage zählt die Prämie die "
                  "Hälfte, kann dafür aber nicht verloren gehen.");
    case HintReason::KontraWorthwhile:
        return tr("Die Ansage der Gegenseite ist angreifbar. Ein Kontra verdoppelt den "
                  "Posten – und zählt bei uns nur fürs Geld.");
    case HintReason::SignalToPartner:
        return tr("Damit zeigst du deinem Partner, wo du stehst. Reden ist verboten; das "
                  "Spiel selbst ist das Signal.");
    case HintReason::NothingToAnnounce:
        return tr("Du hast weder Trullstück noch Vogel. Sag „Ich liege“ und spiel.");
    case HintReason::DrawTrumps:
        return card.isEmpty()
            ? tr("Zieh mit deinem hohen Tarock die restlichen Stecher heraus, bevor die "
                 "Gegner deine Könige abstechen.")
            : tr("Mit %1 hast du eines der höchsten lebenden Tarock. Zieh damit die "
                 "restlichen hohen Tarock heraus, bevor die Gegner deine Könige abstechen.")
                  .arg(card);
    case HintReason::SmearToPartner:
        return tr("Der Stich gehört deiner Partei. Leg die hohe Figur dazu, statt sie im "
                  "eigenen Blatt zu horten – das ist das Schmieren.");
    case HintReason::DuckCheaply:
        return tr("Diesen Stich kannst du nicht gewinnen. Wirf die billigste Karte ab und "
                  "spar dir die Zähler für später.");
    case HintReason::KeepStopper:
        return tr("Behalte den Stecher für den Stich, der wirklich zählt. Wer nie sticht, "
                  "bleibt am Ende mit wertlosen hohen Tarock sitzen.");
    case HintReason::LeadShortKing:
        return card.isEmpty()
            ? tr("Spiel den kurzen König aus, solange ihn noch niemand abstechen kann.")
            : tr("Spiel %1 aus: In dieser Farbe hast du nur wenige Karten, und je länger du "
                 "wartest, desto eher sticht ihn jemand ab.").arg(card);
    case HintReason::SaveBirdForTarget:
        return tr("Heb den angesagten Vogel für seinen Stich auf. Gibst du ihn früher her, "
                  "ist die Prämie weg.");
    case HintReason::LastTrickPlan:
        return tr("Der letzte Stich ist verplant. Spiel jetzt so, dass die Karte dafür übrig "
                  "bleibt.");
    case HintReason::CountSaysEnough:
        return tr("Nach dem bisherigen Stand reichen deine Punkte. Spiel auf Sicherheit und "
                  "gib nichts mehr her.");
    case HintReason::CountSaysMustAttack:
        return tr("Für 35 Punkte und 2 Blatt fehlt dir noch etwas. Du musst angreifen, "
                  "abwarten reicht nicht mehr.");
    case HintReason::OnlyOneCard:
        return tr("Du hast nur diese eine erlaubte Karte – hier gibt es nichts zu überlegen.");
    case HintReason::FollowSuitForced:
        return tr("Die ausgespielte Farbe musst du bedienen. Die Wahl beschränkt sich auf "
                  "die Karten dieser Farbe.");
    }
    return tr("Dieser Zug ist erlaubt.");
}

QString LearnEngine::qualityOf(int bestScore, int score)
{
    // The scores are settlement points times 100 (docs/design.md §7.4).
    const int gap = bestScore - score;
    if (gap < 25)
        return QStringLiteral("best");
    if (gap < 100)
        return QStringLiteral("good");
    if (gap < 300)
        return QStringLiteral("playable");
    return QStringLiteral("bad");
}

QString LearnEngine::actionLabel(const Action& action) const
{
    switch (action.type) {
    case ActionType::Bid:
        return tr("%1 ansagen").arg(ReasonText::contractName(action.a));
    case ActionType::Pass:
        return tr("„weiter“ sagen");
    case ActionType::Hold:
        return tr("halten");
    case ActionType::OpenForehand:
        return tr("„Vorhand“ sagen");
    case ActionType::CallKing:
        return action.b == 1 ? tr("den vierten König rufen")
                             : tr("den %1-König rufen").arg(ReasonText::suitName(action.a));
    case ActionType::CallTarock:
        return tr("%1 rufen").arg(ReasonText::cardName(Card(static_cast<std::uint8_t>(action.a))));
    case ActionType::TakeTalon:
        return action.a < 0 ? tr("den ganzen Talon nehmen")
                            : tr("Talonhälfte %1 nehmen").arg(action.a + 1);
    case ActionType::Concede:
        return tr("schleifen");
    case ActionType::Discard:
        return tr("%1 verlegen")
            .arg(ReasonText::cardName(Card(static_cast<std::uint8_t>(action.a))));
    case ActionType::ConfirmDiscard:
        return tr("Ablage bestätigen");
    case ActionType::AnnounceBonus:
        return tr("%1 ansagen").arg(ReasonText::bonusName(action.a));
    case ActionType::Kontra:
        return tr("kontrieren");
    case ActionType::Ready:
        return tr("„Ich liege“ sagen");
    case ActionType::PlayCard:
        return tr("%1 spielen")
            .arg(ReasonText::cardName(Card(static_cast<std::uint8_t>(action.a))));
    case ActionType::ClaimEnd:
    case ActionType::Throwin:
    case ActionType::None:
        break;
    }
    return QString();
}

std::vector<Hint> LearnEngine::rankLocal() const
{
    if (!ready() || core().handOver() || core().actor() != kLocalSeat)
        return std::vector<Hint>();
    // KNOWLEDGE LIMIT (docs/koenigrufen.md §10.8, design.md §7.4):
    // the only seat named here is kLocalSeat. AiPlayer::rank(core, seat, …)
    // builds its View from core.hand(seat) and from the public record only —
    // no other seat's hand is reachable from this call — and the Inference it
    // uses is created inside rank() from what everybody at the table saw. So
    // the hint is a pure function of (public state, own hand); swapping the
    // cards of any other seat cannot change it, and tests/test_learn.cpp
    // asserts exactly that.
    return m_engine->ai().rank(core(), kLocalSeat, kHintBudgetMs);
}

QVariantMap LearnEngine::hintMap(const std::vector<Hint>& hints, int index) const
{
    QVariantMap map;
    if (hints.empty() || index < 0 || index >= static_cast<int>(hints.size()))
        return map;
    const Hint& hint = hints[static_cast<std::size_t>(index)];
    const int bestScore = hints.front().score;
    QVariantMap action;
    action.insert(QStringLiteral("type"), actionNameOf(hint.action.type));
    action.insert(QStringLiteral("a"), static_cast<int>(hint.action.a));
    action.insert(QStringLiteral("b"), static_cast<int>(hint.action.b));
    map.insert(QStringLiteral("action"), action);
    map.insert(QStringLiteral("label"), actionLabel(hint.action));
    map.insert(QStringLiteral("quality"), qualityOf(bestScore, hint.score));
    map.insert(QStringLiteral("rank"), hint.rank);
    map.insert(QStringLiteral("rankCount"), static_cast<int>(hints.size()));
    map.insert(QStringLiteral("score"), hint.score);
    map.insert(QStringLiteral("reasonKey"),
               QString::fromLatin1(tarock::hintKey(hint.reason)));
    map.insert(QStringLiteral("short"), hintShort(hint.reason));
    map.insert(QStringLiteral("text"), hintText(hint.reason, hint.detail));
    QVariantList alternatives;
    for (std::size_t i = 0; i < hints.size() && alternatives.size() < 3; ++i) {
        if (static_cast<int>(i) == index)
            continue;
        const Hint& other = hints[i];
        QVariantMap entry;
        entry.insert(QStringLiteral("label"), actionLabel(other.action));
        entry.insert(QStringLiteral("quality"), qualityOf(bestScore, other.score));
        entry.insert(QStringLiteral("text"), hintText(other.reason, other.detail));
        entry.insert(QStringLiteral("a"), static_cast<int>(other.action.a));
        entry.insert(QStringLiteral("type"), actionNameOf(other.action.type));
        alternatives.append(entry);
    }
    map.insert(QStringLiteral("alternatives"), alternatives);
    return map;
}

void LearnEngine::requestHint()
{
    const std::vector<Hint> hints = rankLocal();
    m_hint = hintMap(hints, 0);
    emit hintChanged();
}

QVariantMap LearnEngine::whyGood(const QString& type, int a, int b) const
{
    QVariantMap map;
    const ActionType actionType = actionTypeOf(type);
    if (actionType == ActionType::None)
        return map;
    const Action wanted(actionType, static_cast<std::int16_t>(a), static_cast<std::int16_t>(b));
    const std::vector<Hint> hints = rankLocal();
    int index = -1;
    for (std::size_t i = 0; i < hints.size(); ++i) {
        if (hints[i].action == wanted) {
            index = static_cast<int>(i);
            break;
        }
    }
    if (index < 0) {
        // Not among the legal actions: then the refusal is the answer.
        return why(type, a, b);
    }
    map = hintMap(hints, index);
    const QString comparison = index == 0
        ? tr("Das ist der beste Zug in dieser Stellung.")
        : tr("Das ist spielbar (Rang %1 von %2). Besser wäre: %3 – %4")
              .arg(index + 1).arg(static_cast<int>(hints.size()))
              .arg(actionLabel(hints.front().action),
                   hintText(hints.front().reason, hints.front().detail));
    map.insert(QStringLiteral("comparison"), comparison);
    return map;
}

// --- the running helpers, docs/design.md §7.5 ---------------------------------------

QVariantMap LearnEngine::standing() const
{
    QVariantMap map;
    if (!ready())
        return map;
    const TarockCore& c = core();
    const RuleProfile& rules = c.profile();

    // Everything the whole table has seen: the finished tricks, the running
    // trick, the open talon, plus what the local seat holds itself.
    CardSet seen;
    for (int trick = 1; trick <= rules.tricks(); ++trick) {
        for (Card card : c.trickCards(trick))
            tarock::add(seen, card);
    }
    if (c.talonOpen()) {
        for (int half = 0; half < 2; ++half)
            seen |= c.talonHalf(half);
    }
    const CardSet publicSeen = seen;
    seen |= c.hand(kLocalSeat);
    seen |= c.discards(kLocalSeat);

    int tarocksOut = 0;
    for (int id = 0; id < 22; ++id) {
        if (!tarock::contains(seen, Card(static_cast<std::uint8_t>(id))))
            ++tarocksOut;
    }
    map.insert(QStringLiteral("tarocksOut"), tarocksOut);
    map.insert(QStringLiteral("tarocksSeen"), 22 - tarocksOut);

    // Card points as far as they are public: the finished tricks belong to the
    // seat that took them, and everybody watched that happen.
    QVariantList seats;
    int declarerUnits = 0;
    int defenderUnits = 0;
    const int declarer = c.declarer();
    const int partner = c.partnerKnown() ? c.partner() : -1;
    for (int seat = 0; seat < c.players(); ++seat) {
        const CountResult won = rules.count(c.won(seat));
        QVariantMap entry;
        entry.insert(QStringLiteral("seat"), seat);
        entry.insert(QStringLiteral("units"), won.units);
        entry.insert(QStringLiteral("points"), won.points());
        entry.insert(QStringLiteral("cards"), won.cards);
        seats.append(entry);
        if (seat == declarer || (partner >= 0 && seat == partner))
            declarerUnits += won.units;
        else if (c.active(seat))
            defenderUnits += won.units;
    }
    // The declarer's own discards count for him; the half he left lying counts
    // for the defenders — koenigrufen.md §11.5 Nr. 17, the classic trap.
    if (declarer >= 0)
        declarerUnits += rules.count(c.discards(declarer)).units;
    const CountResult talonToDefenders = rules.count(c.talonToDefenders());
    defenderUnits += talonToDefenders.units;

    map.insert(QStringLiteral("seatPoints"), seats);
    map.insert(QStringLiteral("declarerUnits"), declarerUnits);
    map.insert(QStringLiteral("defenderUnits"), defenderUnits);
    map.insert(QStringLiteral("declarerPoints"), declarerUnits / 3);
    map.insert(QStringLiteral("defenderPoints"), defenderUnits / 3);
    map.insert(QStringLiteral("thresholdUnits"), rules.winThreshold());
    map.insert(QStringLiteral("thresholdPoints"), rules.winThreshold() / 3);
    map.insert(QStringLiteral("partnerKnown"), c.partnerKnown());
    map.insert(QStringLiteral("talonToDefendersUnits"), talonToDefenders.units);
    map.insert(QStringLiteral("talonNote"),
               tr("Vergiss nicht: Die liegen gelassene Talonhälfte zählt am Ende zu den "
                  "Stichen der Gegner."));

    // The birds and the honours: fallen or still out, and who took them.
    QVariantList birds;
    const int watched[6] = {0, 1, 2, 3, 20, 21};
    for (int i = 0; i < 6; ++i) {
        const Card card(static_cast<std::uint8_t>(watched[i]));
        QVariantMap entry;
        entry.insert(QStringLiteral("card"), static_cast<int>(card.id));
        entry.insert(QStringLiteral("name"), ReasonText::cardName(card));
        entry.insert(QStringLiteral("bird"), card.bird());
        entry.insert(QStringLiteral("honour"), card.honour());
        int winner = -1;
        for (int trick = 1; trick <= rules.tricks() && winner < 0; ++trick) {
            for (Card played : c.trickCards(trick)) {
                if (played == card && c.trickWinner(trick) >= 0) {
                    winner = c.trickWinner(trick);
                    break;
                }
            }
        }
        entry.insert(QStringLiteral("fallen"), tarock::contains(publicSeen, card));
        entry.insert(QStringLiteral("wonBy"), winner);
        entry.insert(QStringLiteral("inMyHand"), tarock::contains(c.hand(kLocalSeat), card));
        birds.append(entry);
    }
    map.insert(QStringLiteral("birds"), birds);

    // Every announcement that is still open.
    QVariantList announcements;
    for (const Declaration& declaration : c.declarations()) {
        QVariantMap entry;
        entry.insert(QStringLiteral("bonus"), static_cast<int>(declaration.bonus));
        entry.insert(QStringLiteral("name"), ReasonText::bonusName(static_cast<int>(declaration.bonus)));
        entry.insert(QStringLiteral("announced"), declaration.announced);
        entry.insert(QStringLiteral("level"), declaration.level);
        entry.insert(QStringLiteral("declarerSide"), declaration.declarerSide);
        announcements.append(entry);
    }
    map.insert(QStringLiteral("announcements"), announcements);
    map.insert(QStringLiteral("trick"), c.trickNumber());
    map.insert(QStringLiteral("tricks"), rules.tricks());
    return map;
}

QVariantList LearnEngine::commonMistakes() const
{
    return RulesIndex::commonMistakes(profileId());
}

void LearnEngine::noteLocalAction(const Action& action)
{
    if (!ready() || m_level == Off)
        return;
    const std::vector<Hint> hints = rankLocal();
    if (hints.empty())
        return;
    Decision decision;
    decision.trick = core().trickNumber();
    decision.phase = static_cast<int>(core().phase());
    decision.action = action;
    decision.bestScore = hints.front().score;
    decision.best = hints.front().action;
    decision.playedScore = hints.front().score;
    for (const Hint& hint : hints) {
        if (hint.action == action) {
            decision.playedScore = hint.score;
            break;
        }
    }
    m_decisions.append(decision);
}

void LearnEngine::handStarted()
{
    m_decisions.clear();
    m_mistakesSeen.clear();
    m_hint.clear();
    emit hintChanged();
    refresh();
}

void LearnEngine::handFinished()
{
    if (!ready())
        return;
    // Two mistakes of §11.5 can only be seen from the settlement.
    const tarock::Ledger& ledger = core().ledger();
    if (!ledger.declarerWon && ledger.declarerCards.points() == 35
        && !m_mistakesSeen.contains(3)) {
        m_mistakesSeen.append(3);   // "35 Punkte für genug halten"
    }
    const CountResult talon = core().profile().count(core().talonToDefenders());
    if (talon.units > 0 && !ledger.declarerWon
        && ledger.declarerCards.units + talon.units >= core().profile().winThreshold()
        && !m_mistakesSeen.contains(17)) {
        m_mistakesSeen.append(17);  // the talon half that decided the hand
    }
    refresh();
}

QVariantMap LearnEngine::debrief() const
{
    QVariantMap map;
    if (!ready())
        return map;
    const TarockCore& c = core();
    map.insert(QStringLiteral("handOver"), c.handOver());
    map.insert(QStringLiteral("contract"), static_cast<int>(c.contract()));
    map.insert(QStringLiteral("contractName"),
               ReasonText::contractName(static_cast<int>(c.contract())));
    map.insert(QStringLiteral("declarer"), c.declarer());
    map.insert(QStringLiteral("partner"), c.partner());
    map.insert(QStringLiteral("calledCard"), static_cast<int>(c.calledKing().id));

    if (c.handOver()) {
        const tarock::Ledger& ledger = c.ledger();
        map.insert(QStringLiteral("declarerWon"), ledger.declarerWon);
        map.insert(QStringLiteral("gameWon"), ledger.gameWon);
        map.insert(QStringLiteral("declarerPoints"), ledger.declarerCards.points());
        map.insert(QStringLiteral("defenderPoints"), ledger.defenderCards.points());
        map.insert(QStringLiteral("declarerCards"), ledger.declarerCards.cards);
        map.insert(QStringLiteral("defenderCards"), ledger.defenderCards.cards);
        QVariantList items;
        for (const tarock::Posten& posten : ledger.items) {
            QVariantMap entry;
            entry.insert(QStringLiteral("bonus"), static_cast<int>(posten.bonus));
            entry.insert(QStringLiteral("name"),
                         posten.type == tarock::PostenType::Game
                             ? tr("Spiel")
                             : ReasonText::bonusName(static_cast<int>(posten.bonus)));
            entry.insert(QStringLiteral("announced"), posten.announced);
            entry.insert(QStringLiteral("achieved"), posten.achieved);
            entry.insert(QStringLiteral("value"), posten.value);
            entry.insert(QStringLiteral("declarerSide"),
                         posten.winner == tarock::Party::Declarer);
            items.append(entry);
        }
        map.insert(QStringLiteral("items"), items);
    }

    // The move with the largest gap between what was played and what was best.
    int worst = -1;
    int worstGap = 0;
    for (int i = 0; i < m_decisions.size(); ++i) {
        const int gap = m_decisions.at(i).bestScore - m_decisions.at(i).playedScore;
        if (gap > worstGap) {
            worstGap = gap;
            worst = i;
        }
    }
    if (worst >= 0) {
        const Decision& decision = m_decisions.at(worst);
        QVariantMap decisive;
        decisive.insert(QStringLiteral("trick"), decision.trick);
        decisive.insert(QStringLiteral("played"), actionLabel(decision.action));
        decisive.insert(QStringLiteral("best"), actionLabel(decision.best));
        decisive.insert(QStringLiteral("gap"), worstGap);
        decisive.insert(QStringLiteral("text"),
                        decision.trick > 0
                            ? tr("Entscheidend war Stich %1: Du hast %2 gespielt, besser wäre "
                                 "%3 gewesen.").arg(decision.trick)
                                  .arg(actionLabel(decision.action), actionLabel(decision.best))
                            : tr("Entscheidend war eine Entscheidung vor dem Spiel: Du hast "
                                 "%1 gewählt, besser wäre %2 gewesen.")
                                  .arg(actionLabel(decision.action), actionLabel(decision.best)));
        map.insert(QStringLiteral("decisive"), decisive);
    }
    map.insert(QStringLiteral("decisions"), m_decisions.size());

    // Which of the beginner mistakes of §11.5 actually happened in this hand.
    QVariantList mistakes;
    for (int number : m_mistakesSeen) {
        const QVariantMap entry = RulesIndex::mistake(profileId(), number);
        if (!entry.isEmpty())
            mistakes.append(entry);
    }
    map.insert(QStringLiteral("mistakes"), mistakes);
    return map;
}

// --- the explanation panel, docs/design.md §7.2 -------------------------------------

void LearnEngine::refresh()
{
    if (!ready()) {
        if (!m_explanation.isEmpty()) {
            m_explanation.clear();
            emit explanationChanged();
        }
        return;
    }
    const TarockCore& c = core();
    QVariantMap map = RulesIndex::explanation(profileId(), c.phase(), c.contract());
    const QString happens = map.value(QStringLiteral("whatHappens")).toString();
    const QString todo = map.value(QStringLiteral("whatToDo")).toString();
    map.insert(QStringLiteral("whatHappensRich"), RulesIndex::markup(profileId(), happens));
    map.insert(QStringLiteral("whatToDoRich"), RulesIndex::markup(profileId(), todo));
    map.insert(QStringLiteral("level"), level());
    map.insert(QStringLiteral("myTurn"), c.actor() == kLocalSeat && !c.handOver());
    if (c.contract() != ContractId::None)
        map.insert(QStringLiteral("purpose"), RulesIndex::contractPurpose(profileId(), c.contract()));
    if (map != m_explanation) {
        m_explanation = map;
        emit explanationChanged();
    }
    // A novice gets the hint without asking (§7.1); on the other levels it
    // stays on the button, and a stale hint is cleared as soon as the turn
    // moves on.
    const bool mine = c.actor() == kLocalSeat && !c.handOver();
    if (m_level == Novice && m_autoHint && mine) {
        requestHint();
    } else if (!mine && !m_hint.isEmpty()) {
        m_hint.clear();
        emit hintChanged();
    }
}

void LearnEngine::explainTerm(const QString& name)
{
    emit termExplained(RulesIndex::term(profileId(), name));
}

QVariantList LearnEngine::glossary() const
{
    return RulesIndex::glossary(profileId());
}

QVariantMap LearnEngine::term(const QString& name) const
{
    return RulesIndex::term(profileId(), name);
}

QString LearnEngine::markup(const QString& text) const
{
    return RulesIndex::markup(profileId(), text);
}

QVariantList LearnEngine::chapters() const
{
    return RulesIndex::chapters(profileId());
}

QVariantList LearnEngine::tariffTable() const
{
    return RulesIndex::tariffTable(profileId());
}

QVariantMap LearnEngine::contractPurpose(int contractId) const
{
    return RulesIndex::contractPurpose(profileId(), static_cast<ContractId>(contractId));
}

QVariantMap LearnEngine::bonusPurpose(int bonusId) const
{
    return RulesIndex::bonusPurpose(profileId(), static_cast<BonusId>(bonusId));
}

// --- lessons, docs/design.md §7.6 ---------------------------------------------------

QStringList LearnEngine::lessonDirectories() const
{
    const QString stem = QLatin1String(profileStemOf(profileId()));
    QStringList directories;
#ifdef TAROCK_DATA_DIR
    // The tests of docs/design.md §11 run from the build directory and are
    // pointed at the lessons in the source tree by CMake.
    directories << QLatin1String(TAROCK_DATA_DIR "/assets/lessons/") + stem;
#endif
    const QString appDir = QCoreApplication::applicationDirPath();
    if (!appDir.isEmpty()) {
        directories << appDir + QLatin1String("/lessons/") + stem;
        directories << appDir + QLatin1String("/assets/lessons/") + stem;
        directories << appDir + QLatin1String("/../assets/lessons/") + stem;
    }
    directories << QLatin1String("/usr/share/harbour-tarock/lessons/") + stem;
    directories << QLatin1String("assets/lessons/") + stem;
    // Android ships the lessons inside the binary; QFile reads ":/" like a path.
    directories << QLatin1String(":/lessons/") + stem;
    return directories;
}

QStringList LearnEngine::lessonIds() const
{
    QStringList ids;
    const QStringList directories = lessonDirectories();
    for (const QString& path : directories) {
        QDir dir(path);
        if (!dir.exists())
            continue;
        const QStringList files = dir.entryList(QStringList() << QStringLiteral("*.json"),
                                                QDir::Files, QDir::Name);
        for (const QString& file : files) {
            const QString id = QFileInfo(file).completeBaseName();
            // Overlay files like kr-practice-1.hu.json are not lessons, and
            // index.json is the course order rather than a lesson.
            if (id.contains(QLatin1Char('.')) || id == QLatin1String("index"))
                continue;
            if (!ids.contains(id))
                ids.append(id);
        }
    }
    return ids;
}

void LearnEngine::startLesson(const QString& lessonId)
{
    m_error.clear();
    if (!m_engine) {
        m_error = tr("Der Lernmodus hat keine Verbindung zum Spiel.");
        emit lessonFailed(m_error);
        return;
    }
    QString path;
    const QStringList directories = lessonDirectories();
    for (const QString& directory : directories) {
        const QString candidate = directory + QLatin1Char('/') + lessonId + QLatin1String(".json");
        if (QFileInfo(candidate).exists()) {
            path = candidate;
            break;
        }
    }
    if (path.isEmpty()) {
        m_error = tr("Die Lektion „%1“ wurde nicht gefunden.").arg(lessonId);
        emit lessonFailed(m_error);
        return;
    }
    if (!m_lesson.loadFile(path, &m_error)) {
        emit lessonFailed(m_error);
        return;
    }
    if (!m_lesson.begin(m_engine->coreForLearning(), &m_error)) {
        emit lessonFailed(m_error);
        return;
    }
    m_decisions.clear();
    m_mistakesSeen.clear();
    m_engine->lessonStateChanged();
    emit lessonChanged();
    refresh();
    const Lesson::Step* step = m_lesson.currentStep();
    if (step)
        emit lessonStepPassed(step->text);
}

bool LearnEngine::lessonAct(const Action& action, bool* accepted)
{
    if (accepted)
        *accepted = false;
    if (!m_lesson.running() || !m_engine)
        return false;
    QVariantMap info;
    const Lesson::Outcome outcome = m_lesson.offer(m_engine->coreForLearning(), action, &info);
    switch (outcome) {
    case Lesson::NotRunning:
        return false;
    case Lesson::Trap: {
        QVariantMap map = info;
        map.insert(QStringLiteral("kind"), QStringLiteral("trap"));
        const QString key = info.value(QStringLiteral("reason")).toString();
        if (!key.isEmpty()) {
            Reason reason;
            reason.code = ReasonText::codeForSpecKey(key);
            if (action.type == ActionType::PlayCard || action.type == ActionType::Discard)
                reason.card = Card(static_cast<std::uint8_t>(action.a));
            const QVariantMap explained = ReasonText::toMap(enrich(reason), profileId());
            map.insert(QStringLiteral("short"), explained.value(QStringLiteral("short")));
            map.insert(QStringLiteral("anchor"), explained.value(QStringLiteral("anchor")));
            if (map.value(QStringLiteral("text")).toString().isEmpty())
                map.insert(QStringLiteral("text"), explained.value(QStringLiteral("text")));
            const int mistake = mistakeFor(reason.code);
            if (mistake > 0 && !m_mistakesSeen.contains(mistake))
                m_mistakesSeen.append(mistake);
        }
        emit lessonTrap(map);
        emit lessonChanged();
        return true;
    }
    case Lesson::Rejected: {
        QVariantMap map = info;
        map.insert(QStringLiteral("kind"), QStringLiteral("reject"));
        emit lessonTrap(map);
        return true;
    }
    case Lesson::Advanced:
    case Lesson::Finished:
        if (accepted)
            *accepted = true;
        m_engine->lessonStateChanged();
        emit lessonChanged();
        refresh();
        if (outcome == Lesson::Finished) {
            markLessonDone(m_lesson.id());
            m_lesson.stop();
            emit lessonStepPassed(m_lesson.moral());
            emit lessonChanged();
        } else if (const Lesson::Step* step = m_lesson.currentStep()) {
            emit lessonStepPassed(step->text);
        }
        return true;
    }
    return false;
}

void LearnEngine::lessonNext()
{
    if (!m_lesson.running() || !m_engine)
        return;
    if (!m_lesson.advance(m_engine->coreForLearning()))
        return;
    m_engine->lessonStateChanged();
    emit lessonChanged();
    refresh();
    if (m_lesson.finished()) {
        markLessonDone(m_lesson.id());
        m_lesson.stop();
        emit lessonStepPassed(m_lesson.moral());
        emit lessonChanged();
        return;
    }
    if (const Lesson::Step* step = m_lesson.currentStep())
        emit lessonStepPassed(step->text);
}

void LearnEngine::lessonRestartStep()
{
    if (!m_lesson.running() || !m_engine)
        return;
    if (!m_lesson.restartStep(m_engine->coreForLearning())) {
        m_error = tr("Der Schritt lässt sich nicht wiederholen.");
        emit lessonFailed(m_error);
        return;
    }
    m_engine->lessonStateChanged();
    emit lessonChanged();
    refresh();
}

void LearnEngine::stopLesson()
{
    if (!m_lesson.running())
        return;
    m_lesson.stop();
    emit lessonChanged();
    refresh();
}

// --- the course and its progress (docs/design.md §7.9) ----------------------
//
// The order is the one index.json writes down, because it is the order the
// course is meant to be walked in; a profile without an index falls back to
// the file names, which sort L0..L7 before the practice hands. Progress is one
// string list per profile in the same QSettings the level lives in, so a
// finished lesson survives the next start.

const QVariantList& LearnEngine::courseOrder() const
{
    if (m_courseLoaded && m_courseProfile == profileId())
        return m_course;
    m_course.clear();
    m_courseLoaded = true;
    m_courseProfile = profileId();

    const QStringList directories = lessonDirectories();
    QByteArray raw;
    for (const QString& directory : directories) {
        QFile file(directory + QLatin1String("/index.json"));
        if (file.exists() && file.open(QIODevice::ReadOnly)) {
            raw = file.readAll();
            break;
        }
    }

    QStringList listed;
    if (!raw.isEmpty()) {
        const QJsonObject root = QJsonDocument::fromJson(raw).object();
        const QJsonArray lessons = root.value(QLatin1String("lessons")).toArray();
        for (const QJsonValue& value : lessons) {
            const QJsonObject entry = value.toObject();
            const QString id = entry.value(QLatin1String("id")).toString();
            if (id.isEmpty())
                continue;
            QVariantMap map;
            map.insert(QStringLiteral("id"), id);
            map.insert(QStringLiteral("title"), entry.value(QLatin1String("title")).toString());
            map.insert(QStringLiteral("description"),
                       entry.value(QLatin1String("description")).toString());
            map.insert(QStringLiteral("module"), entry.value(QLatin1String("module")).toString());
            // index.json may say what kind of unit this is; where it does not,
            // the practice hands are the ones that carry it in their id.
            QString kind = entry.value(QLatin1String("kind")).toString();
            if (kind.isEmpty())
                kind = id.contains(QLatin1String("practice")) ? QStringLiteral("practice")
                                                              : QStringLiteral("module");
            map.insert(QStringLiteral("kind"), kind);
            m_course.append(map);
            listed.append(id);
        }
    }

    // Whatever lies next to the index without being listed there is still a
    // lesson and is appended rather than hidden.
    const QStringList ids = lessonIds();
    for (const QString& id : ids) {
        if (listed.contains(id))
            continue;
        QVariantMap map;
        map.insert(QStringLiteral("id"), id);
        map.insert(QStringLiteral("title"), id);
        map.insert(QStringLiteral("description"), QString());
        map.insert(QStringLiteral("module"), QString());
        map.insert(QStringLiteral("kind"), id.contains(QLatin1String("practice"))
                                               ? QStringLiteral("practice")
                                               : QStringLiteral("module"));
        m_course.append(map);
    }
    return m_course;
}

QString LearnEngine::progressKey() const
{
    return QLatin1String(kProgressKey) + QLatin1String(profileStemOf(profileId()));
}

QStringList LearnEngine::doneLessons() const
{
    QSettings settings;
    return settings.value(progressKey()).toStringList();
}

QVariantList LearnEngine::course() const
{
    const QVariantList order = courseOrder();
    const QStringList done = doneLessons();
    QVariantList result;
    bool nextMarked = false;
    for (const QVariant& entry : order) {
        QVariantMap map = entry.toMap();
        const bool finished = done.contains(map.value(QStringLiteral("id")).toString());
        map.insert(QStringLiteral("done"), finished);
        // Exactly one entry is the one to go on with, so the page can point at
        // it without repeating the search.
        const bool current = !finished && !nextMarked;
        if (current)
            nextMarked = true;
        map.insert(QStringLiteral("current"), current);
        result.append(map);
    }
    return result;
}

int LearnEngine::lessonsDone() const
{
    const QVariantList order = courseOrder();
    const QStringList done = doneLessons();
    int count = 0;
    for (const QVariant& entry : order) {
        if (done.contains(entry.toMap().value(QStringLiteral("id")).toString()))
            ++count;
    }
    return count;
}

QString LearnEngine::nextLessonId() const
{
    const QVariantList order = courseOrder();
    const QStringList done = doneLessons();
    for (const QVariant& entry : order) {
        const QString id = entry.toMap().value(QStringLiteral("id")).toString();
        if (!done.contains(id))
            return id;
    }
    return QString();
}

QVariantMap LearnEngine::lessonInfo(const QString& lessonId) const
{
    const QVariantList order = courseOrder();
    for (const QVariant& entry : order) {
        QVariantMap map = entry.toMap();
        if (map.value(QStringLiteral("id")).toString() != lessonId)
            continue;
        map.insert(QStringLiteral("done"), lessonDone(lessonId));
        if (map.value(QStringLiteral("title")).toString() == lessonId
                || map.value(QStringLiteral("title")).toString().isEmpty()) {
            // Not listed in the index: the file itself knows its title.
            Lesson lesson;
            QString error;
            const QStringList directories = lessonDirectories();
            for (const QString& directory : directories) {
                const QString path = directory + QLatin1Char('/') + lessonId
                                     + QLatin1String(".json");
                if (!QFileInfo(path).exists())
                    continue;
                if (lesson.loadFile(path, &error)) {
                    map.insert(QStringLiteral("title"), lesson.title());
                    map.insert(QStringLiteral("module"), lesson.module());
                    map.insert(QStringLiteral("goals"), lesson.goals());
                }
                break;
            }
        }
        return map;
    }
    return QVariantMap();
}

bool LearnEngine::lessonDone(const QString& lessonId) const
{
    return doneLessons().contains(lessonId);
}

void LearnEngine::markLessonDone(const QString& lessonId)
{
    if (lessonId.isEmpty())
        return;
    QStringList done = doneLessons();
    if (done.contains(lessonId))
        return;
    done.append(lessonId);
    QSettings settings;
    settings.setValue(progressKey(), done);
    settings.sync();
    emit courseChanged();
}

void LearnEngine::resetCourse()
{
    QSettings settings;
    settings.remove(progressKey());
    settings.sync();
    if (m_tourSeen) {
        m_tourSeen = false;
        saveSettings();
    }
    emit courseChanged();
}

void LearnEngine::setTourSeen(bool value)
{
    if (value == m_tourSeen)
        return;
    m_tourSeen = value;
    saveSettings();
    emit courseChanged();
}
