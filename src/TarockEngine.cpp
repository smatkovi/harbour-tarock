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
#include "TarockEngine.h"

#include "LearnEngine.h"

#include <QByteArray>
#include <QDateTime>
#include <QSettings>

#include <algorithm>
#include <string>
#include <vector>

using tarock::Action;
using tarock::ActionType;
using tarock::BonusDef;
using tarock::BonusId;
using tarock::BonusKind;
using tarock::Card;
using tarock::CardSet;
using tarock::ContractDef;
using tarock::ContractId;
using tarock::CountMode;
using tarock::CountResult;
using tarock::Declaration;
using tarock::Difficulty;
using tarock::Ledger;
using tarock::PartnerMode;
using tarock::Party;
using tarock::Phase;
using tarock::Posten;
using tarock::PostenType;
using tarock::ProfileId;
using tarock::Reason;
using tarock::ReasonCode;
using tarock::RuleProfile;
using tarock::Severity;
using tarock::Suit;

namespace {

// The computer waits long enough for the table to be read, but a bare "gut"
// in the bidding or announcement round would make four seats feel like a
// four second pause.
const int kComputerDelayMs = 600;
const int kPassDelayMs = 260;
const int kTrickPauseMs = 700;
const int kCardWatchdogMs = 5000;
const int kTrickWatchdogMs = 7000;
const int kRevealWatchdogMs = 9000;

const char* const kStateKey = "match/state";
const char* const kSchriftKey = "match/schrift";
const char* const kGeldKey = "match/geld";
const char* const kMatchGroup = "match";
const char* const kProfileKey = "settings/profile";
const char* const kDeckKey = "settings/deck";
const char* const kDifficultyKey = "settings/difficulty";
const char* const kAnimationsKey = "settings/animations";
const char* const kAnimationSpeedKey = "settings/animation-speed";

struct ActionName {
    ActionType type;
    const char* name;
};

// The names QML speaks; they follow ActionType, not the rule texts.
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

// The counterpart, so that an option can be handed straight back to act().
QString actionNameOf(ActionType type)
{
    for (const ActionName& entry : kActionNames) {
        if (entry.type == type)
            return QString::fromLatin1(entry.name);
    }
    return QString();
}

std::uint32_t freshSeed()
{
    return static_cast<std::uint32_t>(QDateTime::currentMSecsSinceEpoch() & 0xffffffffu);
}

ProfileId profileIdFor(const QString& key)
{
    return key == QLatin1String("HU-ILLU-ITVB-2019") ? ProfileId::HuIlluItvb2019
                                                     : ProfileId::AtKrOoe2023;
}

Severity severityOf(const Reason& reason)
{
    return tarock::severityOf(reason.code);
}

QString joinInts(const QVector<int>& values)
{
    QStringList parts;
    for (int index = 0; index < values.size(); ++index)
        parts.append(QString::number(values.at(index)));
    return parts.join(QString::fromLatin1(" "));
}

QVector<int> splitInts(const QString& text, int size)
{
    QVector<int> values(size, 0);
    const QStringList parts = text.split(QLatin1Char(' '));
    int index = 0;
    for (int i = 0; i < parts.size() && index < size; ++i) {
        if (parts.at(i).isEmpty())
            continue;
        values[index++] = parts.at(i).toInt();
    }
    return values;
}

QString levelWord(int level)
{
    if (level >= 8)
        return TarockEngine::tr("Subkontra");
    if (level >= 4)
        return TarockEngine::tr("Rekontra");
    return TarockEngine::tr("Kontra");
}

} // namespace

TarockEngine::TarockEngine(QObject* parent)
    : QObject(parent)
{
    m_aiTimer.setSingleShot(true);
    m_trickPauseTimer.setSingleShot(true);
    m_watchdog.setSingleShot(true);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    connect(&m_aiTimer, SIGNAL(timeout()), this, SLOT(runComputer()));
    connect(&m_trickPauseTimer, SIGNAL(timeout()), this, SLOT(onTrickPauseTimeout()));
    connect(&m_watchdog, SIGNAL(timeout()), this, SLOT(onWatchdogTimeout()));
#else
    connect(&m_aiTimer, &QTimer::timeout, this, &TarockEngine::runComputer);
    connect(&m_trickPauseTimer, &QTimer::timeout, this, &TarockEngine::onTrickPauseTimeout);
    connect(&m_watchdog, &QTimer::timeout, this, &TarockEngine::onWatchdogTimeout);
#endif
    // Der Tisch über Netz oder Bluetooth. Nach Signatur verbunden, damit
    // dieselben Zeilen unter Qt 4.7 und Qt 5 gelten.
    connect(&m_table, SIGNAL(requestArrived(int,QString,int,int)),
            this, SLOT(onTableRequest(int,QString,int,int)));
    connect(&m_table, SIGNAL(viewArrived(QString,QVariantList,QVariantList)),
            this, SLOT(onTableView(QString,QVariantList,QVariantList)));
    connect(&m_table, SIGNAL(matchStarted(QString,int,int)),
            this, SLOT(onTableMatchStarted(QString,int,int)));
    connect(&m_table, SIGNAL(tableClosed(QString)), this, SLOT(onTableClosed(QString)));
    connect(&m_table, SIGNAL(refused(QString)), this, SLOT(onTableRefused(QString)));
    connect(&m_table, SIGNAL(seatsChanged()), this, SIGNAL(stateChanged()));

    loadSettings();

    // The learning mode is a layer over the engine, not a second engine; QML
    // reaches it as tarockEngine.learn (docs/design.md §7.2).
    m_learn = new LearnEngine(this, this);
}

void TarockEngine::onTrickPauseTimeout()
{
    if (m_visualPhase != TrickPause)
        return;
    setVisualPhase(TrickFlight);
    emit stateChanged();
    emit trickAnimationRequested(m_flyingWinner);
    m_watchdog.start(kTrickWatchdogMs);
}

// A table that never answers must not lock the engine for good.
void TarockEngine::onWatchdogTimeout()
{
    switch (m_visualPhase) {
    case CardFlight: completeCardAnimation(); break;
    case TrickFlight: completeTrickAnimation(); break;
    case Reveal: completeReveal(); break;
    default: finishIdle(); break;
    }
}

TarockEngine::~TarockEngine()
{
    persist();
}

// --- settings and persistence -------------------------------------------------------

void TarockEngine::loadSettings()
{
    QSettings settings;
    m_profileId = profileIdFor(settings.value(QLatin1String(kProfileKey)).toString());
    const QString deck = settings.value(QLatin1String(kDeckKey), m_deck).toString().trimmed();
    if (!deck.isEmpty())
        m_deck = deck;
    m_difficulty = static_cast<Difficulty>(
        qBound(0, settings.value(QLatin1String(kDifficultyKey), static_cast<int>(m_difficulty)).toInt(), 2));
    m_ai.setLevel(m_difficulty);
    m_animationsEnabled = settings.value(QLatin1String(kAnimationsKey), m_animationsEnabled).toBool();
    m_animationSpeed = qBound(25, settings.value(QLatin1String(kAnimationSpeedKey),
                                                 m_animationSpeed).toInt(), 300);
    m_hasSaved = settings.contains(QLatin1String(kStateKey));
}

void TarockEngine::saveSettings()
{
    QSettings settings;
    settings.setValue(QLatin1String(kProfileKey), profileKey());
    settings.setValue(QLatin1String(kDeckKey), m_deck);
    settings.setValue(QLatin1String(kDifficultyKey), static_cast<int>(m_difficulty));
    settings.setValue(QLatin1String(kAnimationsKey), m_animationsEnabled);
    settings.setValue(QLatin1String(kAnimationSpeedKey), m_animationSpeed);
    settings.sync();
}

void TarockEngine::persist()
{
    if (!m_active)
        return;
    // Nach jeder Änderung bekommt jeder Gast seinen eigenen Sichtzustand.
    publishViews();
    // Der Gast speichert nicht: sein Kern ist ein gefilterter Fremdzustand
    // und hätte die eigene angefangene Partie überschrieben.
    if (m_table.isGuest())
        return;
    if (matchOver()) {
        clearSaved();
        return;
    }
    QSettings settings;
    const std::string serialized = m_core.serialize();
    settings.setValue(QLatin1String(kStateKey),
                      QString::fromLatin1(QByteArray(serialized.data(),
                                                     int(serialized.size())).toBase64()));
    settings.setValue(QLatin1String(kSchriftKey), joinInts(m_matchSchrift));
    settings.setValue(QLatin1String(kGeldKey), joinInts(m_matchGeld));
    settings.setValue(QLatin1String(kProfileKey), profileKey());
    settings.sync();
    m_hasSaved = true;
}

void TarockEngine::clearSaved()
{
    QSettings settings;
    settings.remove(QLatin1String(kMatchGroup));
    settings.sync();
    m_hasSaved = false;
}

QString TarockEngine::profileKey() const
{
    return QString::fromLatin1(RuleProfile::get(m_profileId).key());
}

QString TarockEngine::profileName() const
{
    return profileNameFor(profileKey());
}

QStringList TarockEngine::profileKeys() const
{
    QStringList keys;
    keys.append(QString::fromLatin1(RuleProfile::get(ProfileId::AtKrOoe2023).key()));
    keys.append(QString::fromLatin1(RuleProfile::get(ProfileId::HuIlluItvb2019).key()));
    return keys;
}

QString TarockEngine::profileNameFor(const QString& key) const
{
    return profileIdFor(key) == ProfileId::HuIlluItvb2019 ? tr("Illusztrált tarokk")
                                                          : tr("Königrufen");
}

void TarockEngine::setDeck(const QString& value)
{
    const QString deck = value.trimmed();
    if (deck.isEmpty() || deck == m_deck)
        return;
    m_deck = deck;
    saveSettings();
    emit settingsChanged();
}

void TarockEngine::setDifficulty(int value)
{
    const Difficulty level = static_cast<Difficulty>(qBound(0, value, 2));
    if (level == m_difficulty)
        return;
    m_difficulty = level;
    m_ai.setLevel(level);
    saveSettings();
    emit settingsChanged();
}

// --- starting and running a match ---------------------------------------------------

void TarockEngine::publishViews()
{
    if (!m_table.hosting() || !m_active)
        return;
    QVariantList schrift;
    QVariantList geld;
    for (int seat = 0; seat < m_core.players(); ++seat) {
        schrift.append(seat < m_matchSchrift.size() ? m_matchSchrift[seat] : 0);
        geld.append(seat < m_matchGeld.size() ? m_matchGeld[seat] : 0);
    }
    for (int seat = 1; seat < m_core.players(); ++seat) {
        if (!m_table.seatIsGuest(seat))
            continue;
        // Gedreht, damit der Gast wie jede Fassung der App auf Platz 0 sitzt,
        // und gefiltert, damit er nur sieht, was ihm zusteht.
        tarock::TarockCore view = m_core;
        view.rotateSeats(seat);
        const std::string state = view.serializeFor(0);
        QVariantList rotatedSchrift;
        QVariantList rotatedGeld;
        for (int s = 0; s < m_core.players(); ++s) {
            const int from = (s + seat) % m_core.players();
            rotatedSchrift.append(schrift.at(from));
            rotatedGeld.append(geld.at(from));
        }
        m_table.sendView(seat, QString::fromLatin1(state.data(), int(state.size())),
                         rotatedSchrift, rotatedGeld);
    }
}

bool TarockEngine::hostTable(const QString& profileKeyName, int players)
{
    QString error;
    players = players == 5 ? 5 : 4;
    if (!m_table.startHosting(seatName(0), profileKeyName, players, &error)) {
        emit actionRejected(QVariantMap());
        return false;
    }
    emit stateChanged();
    return true;
}

void TarockEngine::startTableMatch()
{
    if (!m_table.hosting())
        return;
    m_table.closeTable();
    startMatch(profileKey(), m_table.seatCount());
    publishViews();
}

void TarockEngine::joinTable(const QString& address)
{
    m_table.join(LanSession::normalizeAddress(address), seatName(0));
    emit stateChanged();
}

void TarockEngine::joinTableBluetooth(const QString& address)
{
    m_table.joinBluetooth(address, seatName(0));
    emit stateChanged();
}

void TarockEngine::leaveTable()
{
    const bool wasGuest = m_table.isGuest();
    m_table.leave(QString());
    if (wasGuest) {
        // Die eigene angefangene Partie ist noch da, der Gast landet wieder
        // bei ihr statt vor einem leeren Tisch.
        m_active = false;
        if (canResume())
            resume();
    }
    emit stateChanged();
}

void TarockEngine::onTableRequest(int seat, const QString& type, int a, int b)
{
    if (!m_table.hosting() || !m_active)
        return;
    if (m_visualPhase != Idle) {
        // Die Tischanimation läuft noch. Der Gast bekommt sein Tippen zurück
        // und den Zustand gleich danach mit publishViews().
        m_table.sendNack(seat, tr("Noch einen Augenblick"));
        return;
    }
    const tarock::Action action(actionTypeOf(type), static_cast<std::int16_t>(a),
                                static_cast<std::int16_t>(b));
    if (action.type == tarock::ActionType::None)
        return;
    const tarock::Reason reason = m_core.check(seat, action);
    if (severityOf(reason) == tarock::Severity::Error) {
        m_table.sendNack(seat, reasonMap(reason).value(QStringLiteral("key")).toString());
        publishViews();   // damit der Gast wieder auf dem Stand des Tisches ist
        return;
    }
    perform(seat, action);
}

void TarockEngine::onTableView(const QString& state, const QVariantList& schrift,
                               const QVariantList& geld)
{
    const QByteArray raw = state.toLatin1();
    tarock::TarockCore view;
    if (!view.restore(std::string(raw.constData(), std::size_t(raw.size()))))
        return;
    m_core = view;
    m_awaitingView = false;
    m_matchSchrift.clear();
    m_matchGeld.clear();
    for (int i = 0; i < schrift.size(); ++i)
        m_matchSchrift.append(schrift.at(i).toInt());
    for (int i = 0; i < geld.size(); ++i)
        m_matchGeld.append(geld.at(i).toInt());
    m_active = true;
    // Der Gast bekommt ganze Zustände, keine einzelnen Aktionen: es gibt
    // nichts zu animieren, der Tisch steht einfach neu.
    m_flyingTrick.clear();
    m_flyingWinner = -1;
    setVisualPhase(Idle);
    emit stateChanged();
}

void TarockEngine::onTableMatchStarted(const QString& profileKeyName, int players, int seat)
{
    Q_UNUSED(seat);
    Q_UNUSED(players);
    m_profileId = profileIdFor(profileKeyName);
    m_aiTimer.stop();
    m_trickPauseTimer.stop();
    m_watchdog.stop();
    m_active = true;
    emit resetVisuals();
    emit matchStarted();
    emit settingsChanged();
    emit stateChanged();
}

void TarockEngine::onTableRefused(const QString& reason)
{
    Q_UNUSED(reason);
    // Der Wunsch ist abgelehnt; der Gast darf wieder tippen.
    m_awaitingView = false;
    emit stateChanged();
}

void TarockEngine::onTableClosed(const QString& reason)
{
    Q_UNUSED(reason);
    m_active = false;
    if (canResume())
        resume();
    emit stateChanged();
}

void TarockEngine::startMatch(const QString& profileKeyName, int players)
{
    m_profileId = profileIdFor(profileKeyName);
    players = players == 5 ? 5 : 4;
    m_aiTimer.stop();
    m_trickPauseTimer.stop();
    m_watchdog.stop();
    m_core.newMatch(m_profileId, players, freshSeed());
    m_matchSchrift = QVector<int>(players, 0);
    m_matchGeld = QVector<int>(players, 0);
    m_flyingTrick.clear();
    m_flyingWinner = -1;
    m_active = true;
    saveSettings();
    persist();
    emit resetVisuals();
    emit matchStarted();
    emit settingsChanged();
    if (m_learn)
        m_learn->handStarted();
    finishIdle();
}

void TarockEngine::newMatch()
{
    startMatch(profileKey(), m_active ? m_core.players() : 4);
}

void TarockEngine::resume()
{
    if (m_active) {
        emit matchStarted();   // back to the running table
        return;
    }
    QSettings settings;
    const QByteArray saved =
        QByteArray::fromBase64(settings.value(QLatin1String(kStateKey)).toString().toLatin1());
    tarock::TarockCore core;
    if (saved.isEmpty() || !core.restore(std::string(saved.constData(), saved.size()))) {
        m_hasSaved = false;
        emit stateChanged();
        return;
    }
    m_core = core;
    m_profileId = m_core.profile().id();
    m_matchSchrift = splitInts(settings.value(QLatin1String(kSchriftKey)).toString(), m_core.players());
    m_matchGeld = splitInts(settings.value(QLatin1String(kGeldKey)).toString(), m_core.players());
    m_aiTimer.stop();
    m_trickPauseTimer.stop();
    m_watchdog.stop();
    m_flyingTrick.clear();
    m_flyingWinner = -1;
    m_active = true;
    emit resetVisuals();
    emit matchStarted();
    emit settingsChanged();
    finishIdle();
}

void TarockEngine::nextHand()
{
    if (!m_active || m_visualPhase != Idle || !m_core.handOver() || matchOver())
        return;
    if (!m_core.nextHand())
        return;
    m_flyingTrick.clear();
    m_flyingWinner = -1;
    persist();
    emit resetVisuals();
    if (m_learn)
        m_learn->handStarted();
    finishIdle();
}

bool TarockEngine::act(const QString& type, int a, int b)
{
    return attempt(type, a, b, false);
}

bool TarockEngine::actConfirmed(const QString& type, int a, int b)
{
    return attempt(type, a, b, true);
}

bool TarockEngine::attempt(const QString& type, int a, int b, bool confirmed)
{
    if (!m_active || m_visualPhase != Idle)
        return false;
    // Ein Gast führt den Kern nicht: er schickt den Wunsch und bekommt den
    // neuen Zustand zurück. Der Gastgeber prüft ihn mit denselben Regeln.
    if (m_table.isGuest()) {
        if (m_awaitingView)
            return false;
        m_table.sendRequest(type, a, b);
        m_awaitingView = true;
        return true;
    }
    const Action action(actionTypeOf(type), static_cast<std::int16_t>(a), static_cast<std::int16_t>(b));
    if (action.type == ActionType::None) {
        Reason unknown;
        unknown.code = ReasonCode::UnknownAction;
        emit actionRejected(reasonMap(unknown));
        return false;
    }
    // Inside a practice hand the lesson script decides what happens; it drives
    // the core itself and tells the table through lessonStateChanged().
    if (m_learn && m_learn->lessonActive()) {
        bool accepted = false;
        if (m_learn->lessonAct(action, &accepted))
            return accepted;
    }
    const Reason reason = m_core.check(0, action);
    if (severityOf(reason) == Severity::Error) {
        if (m_learn)
            m_learn->noteRefusal(reason, type, a, b);
        emit actionRejected(reasonMap(reason));
        return false;
    }
    // A warning is allowed but costs something — a renonce, a bird given away.
    // The table says so and asks again; the repeat comes back confirmed.
    if (!confirmed && severityOf(reason) == Severity::Warning) {
        if (m_learn)
            m_learn->noteRefusal(reason, type, a, b);
        emit actionWarned(reasonMap(reason), type, a, b);
        return false;
    }
    if (!reason.ok() && severityOf(reason) == Severity::Info)
        emit actionRejected(reasonMap(reason));
    // Written down before the action is applied: the debriefing compares what
    // was played with what the same ranking would have recommended (§7.5).
    if (m_learn)
        m_learn->noteLocalAction(action);
    return perform(0, action);
}

bool TarockEngine::perform(int seat, const Action& action)
{
    const Phase phaseBefore = m_core.phase();
    const int trickBefore = m_core.trickNumber();
    const int leaderBefore = m_core.trickLeader();
    const tarock::CardList cardsBefore = m_core.trick();
    const bool wasHandOver = m_core.handOver();

    Reason reason;
    if (!m_core.apply(seat, action, &reason))
        return false;

    const QString formula = speechFor(action);
    if (!formula.isEmpty())
        emit speech(seat, formula);

    if (!wasHandOver && m_core.handOver()) {
        const Ledger& ledger = m_core.ledger();
        for (int s = 0; s < m_core.players() && s < m_matchSchrift.size(); ++s) {
            m_matchSchrift[s] += ledger.schrift[static_cast<std::size_t>(s)];
            m_matchGeld[s] += ledger.geld[static_cast<std::size_t>(s)];
        }
        if (m_learn)
            m_learn->handFinished();
        emit handFinished();
    }
    persist();

    if (action.type == ActionType::PlayCard) {
        const Card played(static_cast<std::uint8_t>(action.a));
        if (m_core.trick().empty()) {
            // The core folds a full trick away at once; the table keeps it
            // lying until the flight to the winner is over.
            m_flyingTrick.clear();
            int owner = leaderBefore;
            for (Card card : cardsBefore) {
                QVariantMap entry = cardMap(card);
                entry.insert(QStringLiteral("seat"), owner);
                m_flyingTrick.append(entry);
                owner = m_core.nextSeat(owner);
            }
            QVariantMap last = cardMap(played);
            last.insert(QStringLiteral("seat"), seat);
            m_flyingTrick.append(last);
            m_flyingWinner = m_core.trickWinner(trickBefore);
        }
        setVisualPhase(CardFlight);
        emit stateChanged();
        emit cardAnimationRequested(static_cast<int>(played.id), seat);
        m_watchdog.start(kCardWatchdogMs);
        return true;
    }

    // The talon goes face up for everybody, and tarocks that were laid away
    // have to be shown; both need a moment of their own.
    QString reveal;
    if (phaseBefore != Phase::Talon && m_core.phase() == Phase::Talon && m_core.talonOpen())
        reveal = QStringLiteral("talon");
    else if (action.type == ActionType::ConfirmDiscard && !openDiscards().isEmpty())
        reveal = QStringLiteral("skart");
    if (!reveal.isEmpty()) {
        setVisualPhase(Reveal);
        emit stateChanged();
        emit revealRequested(reveal);
        m_watchdog.start(kRevealWatchdogMs);
        return true;
    }

    finishIdle();
    return true;
}

void TarockEngine::completeCardAnimation()
{
    if (m_visualPhase != CardFlight)
        return;
    m_watchdog.stop();
    if (m_flyingWinner >= 0 && m_core.trick().empty()) {
        setVisualPhase(TrickPause);
        emit stateChanged();
        m_trickPauseTimer.start(trickPauseMs());
        return;
    }
    finishIdle();
}

void TarockEngine::completeTrickAnimation()
{
    if (m_visualPhase != TrickFlight)
        return;
    m_watchdog.stop();
    finishIdle();   // m_flyingTrick stays available as lastTrick()
}

void TarockEngine::completeReveal()
{
    if (m_visualPhase != Reveal)
        return;
    m_watchdog.stop();
    finishIdle();
}

void TarockEngine::setVisualPhase(VisualPhase phase)
{
    if (phase == m_visualPhase)
        return;
    m_visualPhase = phase;
    emit visualPhaseChanged();
}

void TarockEngine::finishIdle()
{
    setVisualPhase(Idle);
    emit stateChanged();
    if (m_learn)
        m_learn->refresh();
    scheduleComputer();
}

void TarockEngine::setAnimationsEnabled(bool value)
{
    if (value == m_animationsEnabled)
        return;
    m_animationsEnabled = value;
    saveSettings();
    emit settingsChanged();
}

void TarockEngine::setAnimationSpeed(int value)
{
    value = qBound(25, value, 300);
    if (value == m_animationSpeed)
        return;
    m_animationSpeed = value;
    saveSettings();
    emit settingsChanged();
}

// The table reads animationsEnabled and animationSpeed for its own flights;
// the engine only scales the pause in which the finished trick is read.
int TarockEngine::trickPauseMs() const
{
    if (!m_animationsEnabled)
        return 150;
    return qMax(120, kTrickPauseMs * m_animationSpeed / 100);
}

void TarockEngine::setPaused(bool value)
{
    if (value == m_paused)
        return;
    m_paused = value;
    if (m_paused)
        m_aiTimer.stop();
    else
        scheduleComputer();
    emit pausedChanged();
}

void TarockEngine::scheduleComputer()
{
    m_aiTimer.stop();
    if (!m_active || m_paused || m_visualPhase != Idle || m_core.handOver())
        return;
    // A lesson plays the other seats from its script, not from the AI timer.
    if (m_learn && m_learn->lessonActive())
        return;
    const int seat = m_core.actor();
    if (seat <= 0 || !seatIsComputer(seat))
        return;
    const std::vector<Action> legal = m_core.legalActions(seat);
    // A seat that can only say "gut" should not hold up the table.
    const bool merePass = legal.size() == 1
        && (legal.front().type == ActionType::Pass || legal.front().type == ActionType::Ready);
    m_aiTimer.start(merePass ? kPassDelayMs : kComputerDelayMs);
}

void TarockEngine::runComputer()
{
    if (!m_active || m_paused || m_visualPhase != Idle || m_core.handOver())
        return;
    if (m_table.isGuest())
        return;   // dort führt der Gastgeber, auch die Computerplätze
    const int seat = m_core.actor();
    if (seat <= 0 || !seatIsComputer(seat))
        return;
    const std::vector<Action> legal = m_core.legalActions(seat);
    if (legal.empty())
        return;
    Action action = m_ai.bestAction(m_core, seat);
    // The computer players may still be a stub: anything the core does not
    // offer falls back to the first legal action, so the hand keeps running.
    if (action.type == ActionType::None
        || std::find(legal.begin(), legal.end(), action) == legal.end())
        action = legal.front();
    if (perform(seat, action))
        return;
    // The core refused what it had offered. Rather than asking again for the
    // same action every 600 ms, walk the remaining offers once; if none of
    // them is accepted the hand is stuck and the table says so.
    for (const Action& option : legal) {
        if (!(option == action) && perform(seat, option))
            return;
    }
    setPaused(true);
}

tarock::TarockCore& TarockEngine::coreForLearning()
{
    // A lesson needs to deal a fixed hand and to replay scripted moves, which
    // no other caller may do; everything else reads the core through core().
    m_active = true;
    return m_core;
}

void TarockEngine::lessonStateChanged()
{
    m_aiTimer.stop();
    m_trickPauseTimer.stop();
    m_watchdog.stop();
    setVisualPhase(Idle);
    m_flyingTrick.clear();
    m_flyingWinner = -1;
    emit stateChanged();
}

bool TarockEngine::seatIsComputer(int seat) const
{
    // Am Netztisch spielt der Computer nur die Plätze, auf denen kein Gast
    // sitzt -- und beim Gast selbst niemanden: dort führt der Gastgeber.
    if (m_table.isGuest())
        return false;
    return seat > 0 && m_core.active(seat) && !m_table.seatIsGuest(seat);
}

bool TarockEngine::myTurn() const
{
    return m_active && m_visualPhase == Idle && !m_core.handOver() && m_core.actor() == 0;
}

bool TarockEngine::matchOver() const
{
    return m_active && m_core.handOver() && m_core.handNumber() + 1 >= m_handsPerMatch;
}

// --- the table ----------------------------------------------------------------------

QString TarockEngine::seatName(int seat) const
{
    if (seat < 0)
        return QString();
    return seat == 0 ? tr("You") : tr("Computer %1").arg(seat);
}

QString TarockEngine::phaseName() const
{
    switch (m_core.phase()) {
    case Phase::Deal: return tr("Deal");
    case Phase::Bidding: return tr("Bidding");
    case Phase::Call: return tr("Calling a king");
    case Phase::Talon: return tr("Talon");
    case Phase::Discard: return tr("Laying away");
    case Phase::Announce: return tr("Announcements");
    case Phase::Play: return tr("Play");
    case Phase::Scoring:
    case Phase::HandOver: return tr("Settlement");
    }
    return QString();
}

QString TarockEngine::status() const
{
    if (!m_active)
        return QString();
    if (matchOver())
        return tr("Match over");
    if (m_core.handOver())
        return tr("Hand over");
    if (m_visualPhase != Idle)
        return tr("Cards are moving…");
    const int actor = m_core.actor();
    const bool mine = actor == 0;
    const QString name = seatName(actor);
    switch (m_core.phase()) {
    case Phase::Deal:
        return tr("Dealing");
    case Phase::Bidding:
        return mine ? tr("Your bid") : tr("%1 is bidding").arg(name);
    case Phase::Call:
        return mine ? tr("Call a king") : tr("%1 calls a king").arg(name);
    case Phase::Talon:
        return mine ? tr("Choose a half of the talon") : tr("%1 chooses a half of the talon").arg(name);
    case Phase::Discard:
        return mine ? tr("Lay away %1 cards").arg(discardTarget())
                    : tr("%1 lays cards away").arg(name);
    case Phase::Announce:
        // Naming the seat would give away who sits with the declarer.
        return mine ? tr("Your announcements") : tr("Waiting for announcements");
    case Phase::Play:
        return mine ? tr("Your turn") : tr("%1's turn").arg(name);
    case Phase::Scoring:
    case Phase::HandOver:
        break;
    }
    return QString();
}

QVariantList TarockEngine::seats() const
{
    QVariantList result;
    if (!m_active)
        return result;
    const bool openParty = m_core.declarer() >= 0
        && m_core.contractDef().partner != PartnerMode::CallKing;
    for (int seat = 0; seat < m_core.players(); ++seat) {
        QVariantMap entry;
        entry.insert(QStringLiteral("name"), seatName(seat));
        entry.insert(QStringLiteral("seat"), seat);
        entry.insert(QStringLiteral("isMe"), seat == 0);
        entry.insert(QStringLiteral("isDealer"), seat == m_core.dealer());
        entry.insert(QStringLiteral("isForehand"), seat == m_core.forehand());
        entry.insert(QStringLiteral("isSittingOut"), seat == m_core.sittingOut());
        entry.insert(QStringLiteral("isDeclarer"), seat == m_core.declarer());

        QString party;
        if (seat == 0) {
            party = QStringLiteral("me");
        } else if (m_core.declarer() < 0) {
            party = QStringLiteral("unknown");
        } else if (openParty || m_core.partnerKnown()) {
            party = m_core.declarerSide(seat) == m_core.declarerSide(0) ? QStringLiteral("partner")
                                                                       : QStringLiteral("opponent");
        } else if (seat == m_core.declarer()) {
            // Holding the called king is my own knowledge, not the table's.
            party = m_core.declarerSide(0) ? QStringLiteral("partner") : QStringLiteral("opponent");
        } else if (m_core.declarer() != 0 && m_core.declarerSide(0)) {
            party = QStringLiteral("opponent");
        } else {
            party = QStringLiteral("unknown");
        }
        entry.insert(QStringLiteral("partnerState"), party);

        // handSize() statt hand().count(): am Netztisch sind fremde Hände
        // verdeckt, ihre Kartenzahl steht aber fest.
        entry.insert(QStringLiteral("cardCount"), m_core.handSize(seat));
        int tricks = 0;
        for (int number = 1; number <= m_core.trickNumber(); ++number)
            tricks += m_core.trickWinner(number) == seat ? 1 : 0;
        entry.insert(QStringLiteral("tricks"), tricks);
        entry.insert(QStringLiteral("schrift"), m_matchSchrift.value(seat));
        entry.insert(QStringLiteral("geld"), m_matchGeld.value(seat));

        QString lastBid;
        if (seat == m_core.declarer())
            lastBid = contractName();
        else if (m_core.hasPassed(seat))
            lastBid = tr("gut");
        else if (seat == m_core.bidHolder())
            lastBid = tr("holds the bid");
        entry.insert(QStringLiteral("lastBid"), lastBid);

        QVariantList announcements;
        for (const Declaration& declaration : m_core.declarations()) {
            if (declaration.seat != seat || declaration.bonus == BonusId::None || !declaration.announced)
                continue;
            QString label = bonusLabel(static_cast<int>(declaration.bonus));
            if (declaration.level > 1)
                label += QStringLiteral(" ×%1").arg(declaration.level);
            announcements.append(label);
        }
        entry.insert(QStringLiteral("announcements"), announcements);
        result.append(entry);
    }
    return result;
}

QVariantMap TarockEngine::cardMap(Card card, bool legal) const
{
    QVariantMap entry;
    entry.insert(QStringLiteral("id"), static_cast<int>(card.id));
    entry.insert(QStringLiteral("key"), cardKey(static_cast<int>(card.id)));
    entry.insert(QStringLiteral("label"), cardLabel(static_cast<int>(card.id)));
    entry.insert(QStringLiteral("suit"), static_cast<int>(card.suit()));
    entry.insert(QStringLiteral("power"), tarock::power(card));
    entry.insert(QStringLiteral("legal"), legal);
    entry.insert(QStringLiteral("warn"), false);
    entry.insert(QStringLiteral("recommended"), false);
    entry.insert(QStringLiteral("dimReason"), QString::fromLatin1(tarock::reasonKey(ReasonCode::Ok)));
    return entry;
}

QVariantList TarockEngine::hand() const
{
    QVariantList result;
    if (!m_active || !m_core.active(0))
        return result;
    const bool idle = m_visualPhase == Idle;
    const bool discarding = m_core.phase() == Phase::Discard && m_core.actor() == 0;
    const CardSet playable = m_core.playableCards(0);
    const CardSet& tray = m_core.discardTray(0);
    const bool trayFull = static_cast<int>(tray.count()) >= discardTarget();
    for (Card card : tarock::toList(m_core.hand(0))) {
        const Reason reason = m_core.cardReason(0, card);
        bool legal = false;
        if (discarding) {
            // A card already in the tray can always be taken back out.
            legal = tarock::contains(tray, card)
                || (!trayFull && severityOf(reason) != Severity::Error);
        } else {
            legal = tarock::contains(playable, card);
        }
        QVariantMap entry = cardMap(card, legal && idle);
        entry.insert(QStringLiteral("warn"), severityOf(reason) == Severity::Warning);
        entry.insert(QStringLiteral("dimReason"), QString::fromLatin1(tarock::reasonKey(reason.code)));
        result.append(entry);
    }
    return result;
}

QVariantList TarockEngine::trick() const
{
    QVariantList result;
    if (!m_active)
        return result;
    const tarock::CardList& cards = m_core.trick();
    if (cards.empty() && m_flyingWinner >= 0 && m_visualPhase != Idle)
        return m_flyingTrick;
    int seat = m_core.trickLeader();
    for (Card card : cards) {
        QVariantMap entry = cardMap(card);
        entry.insert(QStringLiteral("seat"), seat);
        result.append(entry);
        seat = m_core.nextSeat(seat);
    }
    return result;
}

QVariantList TarockEngine::lastTrick() const
{
    return m_flyingTrick;
}

QVariantList TarockEngine::talonHalves() const
{
    QVariantList result;
    if (!m_active)
        return result;
    // At a table of five the talon is taboo for the seat that sits out.
    const bool visible = m_core.talonOpen() && m_core.active(0);
    for (int half = 0; half < 2; ++half) {
        const CardSet& cards = m_core.talonHalf(half);
        const int size = m_core.talonHalfSize(half);
        QVariantMap entry;
        entry.insert(QStringLiteral("half"), half);
        entry.insert(QStringLiteral("count"), size);
        entry.insert(QStringLiteral("taken"), size == 0);
        QVariantList list;
        int tarocks = 0;
        for (Card card : tarock::toList(cards)) {
            if (visible)
                list.append(cardMap(card));
            tarocks += card.tarock() ? 1 : 0;
        }
        entry.insert(QStringLiteral("cards"), list);
        entry.insert(QStringLiteral("tarocks"), visible ? tarocks : -1);
        const CountResult count = m_core.profile().count(cards);
        entry.insert(QStringLiteral("points"),
                     visible ? (m_core.profile().countMode() == CountMode::ThirdsAustrian
                                    ? count.points() : count.units)
                             : -1);
        result.append(entry);
    }
    return result;
}

QVariantList TarockEngine::discardTray() const
{
    QVariantList result;
    if (!m_active)
        return result;
    for (Card card : tarock::toList(m_core.discardTray(0)))
        result.append(cardMap(card));
    return result;
}

int TarockEngine::discardTarget() const
{
    if (!m_active || m_core.contract() == ContractId::None)
        return 0;
    return m_core.contractDef().discardCount;
}

QVariantList TarockEngine::openDiscards() const
{
    QVariantList result;
    if (!m_active)
        return result;
    for (int seat = 0; seat < m_core.players(); ++seat) {
        for (Card card : tarock::toList(m_core.discards(seat))) {
            // Only laid away tarocks have to be shown (koenigrufen.md §4.6).
            if (!card.tarock())
                continue;
            QVariantMap entry = cardMap(card);
            entry.insert(QStringLiteral("seat"), seat);
            result.append(entry);
        }
    }
    return result;
}

int TarockEngine::tricksTotal() const
{
    return m_core.profile().tricks();
}

// --- decisions ----------------------------------------------------------------------

QVariantList TarockEngine::options() const
{
    QVariantList result;
    if (!myTurn())
        return result;
    for (const Action& action : m_core.legalActions(0)) {
        QVariantMap entry;
        QString label;
        QString formula = speechFor(action);
        QString group;
        switch (action.type) {
        case ActionType::Bid:
            group = QStringLiteral("bid");
            label = tr("%1 (%2)").arg(contractLabel(action.a))
                        .arg(m_core.profile().contract(static_cast<ContractId>(action.a)).baseValue);
            break;
        case ActionType::Pass:
            group = QStringLiteral("bid");
            label = tr("gut");
            break;
        case ActionType::Hold:
            group = QStringLiteral("bid");
            label = tr("Hold");
            break;
        case ActionType::OpenForehand:
            group = QStringLiteral("bid");
            label = tr("Vorhand");
            break;
        case ActionType::CallKing:
        case ActionType::CallTarock:
            group = QStringLiteral("call");
            label = formula;
            break;
        case ActionType::TakeTalon:
            group = QStringLiteral("talon");
            label = action.a < 0 ? tr("Take the talon") : tr("Take half %1").arg(action.a + 1);
            break;
        case ActionType::Concede:
            group = QStringLiteral("talon");
            label = tr("Schleifen");
            break;
        case ActionType::ConfirmDiscard:
            group = QStringLiteral("discard");
            label = tr("Done");
            break;
        case ActionType::Ready:
            group = QStringLiteral("announce");
            label = formula;
            break;
        default:
            // Cards, single discards and the announcement chips have panels of
            // their own; options() carries the buttons of the action bar.
            continue;
        }
        entry.insert(QStringLiteral("type"), actionNameOf(action.type));
        entry.insert(QStringLiteral("a"), static_cast<int>(action.a));
        entry.insert(QStringLiteral("b"), static_cast<int>(action.b));
        entry.insert(QStringLiteral("label"), label);
        entry.insert(QStringLiteral("speech"), formula);
        entry.insert(QStringLiteral("group"), group);
        result.append(entry);
    }
    return result;
}

QVariantList TarockEngine::bonusOptions() const
{
    QVariantList result;
    if (!myTurn() || m_core.phase() != Phase::Announce)
        return result;
    for (const Action& action : m_core.legalActions(0)) {
        if (action.type != ActionType::AnnounceBonus)
            continue;
        const BonusDef& bonus = m_core.profile().bonus(static_cast<BonusId>(action.a));
        QString group;
        switch (bonus.kind) {
        case BonusKind::Holding:
        case BonusKind::TarockCount:
            group = QStringLiteral("holding");
            break;
        case BonusKind::TrickWithCard:
        case BonusKind::TrickRun:
        case BonusKind::Capture:
            group = QStringLiteral("course");
            break;
        case BonusKind::PointTarget:
        case BonusKind::AllTricks:
            group = QStringLiteral("target");
            break;
        }
        QVariantMap entry;
        entry.insert(QStringLiteral("type"), QStringLiteral("bonus"));
        entry.insert(QStringLiteral("a"), static_cast<int>(action.a));
        entry.insert(QStringLiteral("b"), static_cast<int>(action.b));
        entry.insert(QStringLiteral("label"), bonusLabel(action.a));
        entry.insert(QStringLiteral("speech"), bonusLabel(action.a));
        entry.insert(QStringLiteral("group"), group);
        result.append(entry);
    }
    return result;
}

QVariantList TarockEngine::kontraTargets() const
{
    QVariantList result;
    if (!myTurn() || m_core.phase() != Phase::Announce)
        return result;
    const std::vector<Declaration>& declarations = m_core.declarations();
    for (const Action& action : m_core.legalActions(0)) {
        if (action.type != ActionType::Kontra || action.a < 0
            || static_cast<std::size_t>(action.a) >= declarations.size())
            continue;
        const Declaration& declaration = declarations[static_cast<std::size_t>(action.a)];
        const int level = declaration.level < 2 ? 2 : declaration.level * 2;
        const QString posten = declaration.bonus == BonusId::None
            ? tr("Game") : bonusLabel(static_cast<int>(declaration.bonus));
        QVariantMap entry;
        entry.insert(QStringLiteral("type"), QStringLiteral("kontra"));
        entry.insert(QStringLiteral("a"), static_cast<int>(action.a));
        entry.insert(QStringLiteral("b"), level);
        entry.insert(QStringLiteral("label"), tr("%1 · %2").arg(posten, levelWord(level)));
        entry.insert(QStringLiteral("speech"), levelWord(level));
        entry.insert(QStringLiteral("group"), QStringLiteral("kontra"));
        result.append(entry);
    }
    return result;
}

QString TarockEngine::contractName() const
{
    if (!m_active || m_core.contract() == ContractId::None)
        return QString();
    QString name = contractLabel(static_cast<int>(m_core.contract()));
    for (const Declaration& declaration : m_core.declarations()) {
        if (declaration.bonus == BonusId::None && declaration.level > 1)
            name += QStringLiteral(" ×%1").arg(declaration.level);
    }
    return name;
}

QString TarockEngine::calledCard() const
{
    const int id = calledCardId();
    return id < 0 ? QString() : cardLabel(id);
}

int TarockEngine::calledCardId() const
{
    if (!m_active || m_core.calledSuit() < 0)
        return -1;
    return static_cast<int>(m_core.calledKing().id);
}

int TarockEngine::trickWinner() const
{
    // Only while the finished trick is still lying on the table; during play
    // the winner of the running trick is not decided yet.
    return m_visualPhase == TrickPause || m_visualPhase == TrickFlight ? m_flyingWinner : -1;
}

// --- result -------------------------------------------------------------------------

QVariantMap TarockEngine::ledger() const
{
    QVariantMap result;
    if (!m_active || !m_core.handOver())
        return result;
    const Ledger& ledger = m_core.ledger();
    QVariantList schrift, geld, matchSchrift, matchGeld;
    for (int seat = 0; seat < m_core.players(); ++seat) {
        schrift.append(ledger.schrift[static_cast<std::size_t>(seat)]);
        geld.append(ledger.geld[static_cast<std::size_t>(seat)]);
        matchSchrift.append(m_matchSchrift.value(seat));
        matchGeld.append(m_matchGeld.value(seat));
    }
    result.insert(QStringLiteral("schrift"), schrift);
    result.insert(QStringLiteral("geld"), geld);
    result.insert(QStringLiteral("matchSchrift"), matchSchrift);
    result.insert(QStringLiteral("matchGeld"), matchGeld);
    result.insert(QStringLiteral("declarerWon"), ledger.declarerWon);
    result.insert(QStringLiteral("gameWon"), ledger.gameWon);
    result.insert(QStringLiteral("declarerCards"), ledger.declarerCards.cards);
    result.insert(QStringLiteral("defenderCards"), ledger.defenderCards.cards);
    result.insert(QStringLiteral("declarerUnits"), ledger.declarerCards.units);
    result.insert(QStringLiteral("defenderUnits"), ledger.defenderCards.units);

    QVariantList items;
    for (const Posten& posten : ledger.items) {
        QString label;
        switch (posten.type) {
        case PostenType::Game: label = contractName(); break;
        case PostenType::Valat:
        case PostenType::Bonus: label = bonusLabel(static_cast<int>(posten.bonus)); break;
        case PostenType::Concede: label = tr("Schleifen"); break;
        case PostenType::Trischaken: label = tr("Trischaken"); break;
        case PostenType::Tarokkszam: label = tr("Tarokkszám"); break;
        }
        QVariantMap entry;
        entry.insert(QStringLiteral("type"), static_cast<int>(posten.type));
        entry.insert(QStringLiteral("bonus"), static_cast<int>(posten.bonus));
        entry.insert(QStringLiteral("label"), label);
        entry.insert(QStringLiteral("announced"), posten.announced);
        entry.insert(QStringLiteral("achieved"), posten.achieved);
        entry.insert(QStringLiteral("value"), posten.value);
        entry.insert(QStringLiteral("kontra"), posten.kontra);
        entry.insert(QStringLiteral("thirds"), posten.thirds);
        entry.insert(QStringLiteral("declarerSide"), posten.winner == Party::Declarer);
        entry.insert(QStringLiteral("seat"), static_cast<int>(posten.againstSeat));
        items.append(entry);
    }
    result.insert(QStringLiteral("items"), items);
    return result;
}

QVariantMap TarockEngine::liveCount() const
{
    QVariantMap result;
    if (!m_active)
        return result;
    const bool thirds = m_core.profile().countMode() == CountMode::ThirdsAustrian;
    const CountResult declarer = m_core.partyPoints(true);
    const CountResult defenders = m_core.partyPoints(false);
    result.insert(QStringLiteral("declarer"), thirds ? declarer.points() : declarer.units);
    result.insert(QStringLiteral("defenders"), thirds ? defenders.points() : defenders.units);
    result.insert(QStringLiteral("declarerUnits"), declarer.units);
    result.insert(QStringLiteral("defenderUnits"), defenders.units);
    result.insert(QStringLiteral("declarerCards"), declarer.cards);
    result.insert(QStringLiteral("defenderCards"), defenders.cards);
    const int threshold = m_core.profile().winThreshold();
    result.insert(QStringLiteral("threshold"), thirds ? threshold / 3 : threshold);
    result.insert(QStringLiteral("thresholdUnits"), threshold);
    return result;
}

// --- texts --------------------------------------------------------------------------

QVariantMap TarockEngine::reasonMap(const Reason& reason) const
{
    QVariantMap result;
    result.insert(QStringLiteral("key"), QString::fromLatin1(tarock::reasonKey(reason.code)));
    result.insert(QStringLiteral("severity"), static_cast<int>(severityOf(reason)));
    QVariantList cards;
    for (Card card : tarock::toList(reason.cards))
        cards.append(static_cast<int>(card.id));
    result.insert(QStringLiteral("cards"), cards);
    result.insert(QStringLiteral("card"), reason.card.valid() ? static_cast<int>(reason.card.id) : -1);
    result.insert(QStringLiteral("seat"), static_cast<int>(reason.seat));
    result.insert(QStringLiteral("suit"), static_cast<int>(reason.suit));
    result.insert(QStringLiteral("bonus"), static_cast<int>(reason.bonus));
    result.insert(QStringLiteral("contract"), static_cast<int>(reason.contract));
    result.insert(QStringLiteral("trick"), static_cast<int>(reason.trick));
    result.insert(QStringLiteral("count"), static_cast<int>(reason.count));
    // The rule browser anchors on the profile the hand is played with.
    result.insert(QStringLiteral("anchor"),
                  QStringLiteral("rules:%1#%2").arg(profileKey(),
                      QString::fromLatin1(tarock::reasonKey(reason.code))));
    return result;
}

QString TarockEngine::speechFor(const Action& action) const
{
    static const char* const kSuitNames[4] = {
        QT_TR_NOOP("heart"), QT_TR_NOOP("diamond"), QT_TR_NOOP("spade"), QT_TR_NOOP("club")
    };
    switch (action.type) {
    case ActionType::OpenForehand:
        return tr("Vorhand");
    case ActionType::Pass:
        return tr("gut");
    case ActionType::Hold:
        return tr("ich halte");
    case ActionType::Bid:
        return contractLabel(action.a);
    case ActionType::CallKing:
        if (action.b == 1)
            return tr("the fourth king");
        return action.a >= 0 && action.a < 4 ? tr("the %1 king").arg(tr(kSuitNames[action.a]))
                                             : QString();
    case ActionType::CallTarock:
        return tr("I call %1").arg(cardLabel(action.a));
    case ActionType::Concede:
        return tr("Schleifen");
    case ActionType::AnnounceBonus:
        return bonusLabel(action.a);
    case ActionType::Kontra: {
        const std::vector<Declaration>& declarations = m_core.declarations();
        const int level = action.a >= 0 && static_cast<std::size_t>(action.a) < declarations.size()
            ? declarations[static_cast<std::size_t>(action.a)].level : 2;
        return levelWord(level);
    }
    case ActionType::Ready:
        return m_core.declarer() == 0 ? tr("Ich liege") : tr("gut");
    default:
        break;
    }
    return QString();
}

QString TarockEngine::cardKey(int cardId) const
{
    const Card card(static_cast<std::uint8_t>(cardId));
    if (cardId < 0 || !card.valid())
        return QString();
    return QString::fromStdString(tarock::cardKey(card));
}

QString TarockEngine::cardLabel(int cardId) const
{
    static const char* const kCourtNames[4] = {
        QT_TR_NOOP("King"), QT_TR_NOOP("Queen"), QT_TR_NOOP("Knight"), QT_TR_NOOP("Jack")
    };
    static const char* const kSuitSymbols[4] = {"♥", "♦", "♠", "♣"};
    const Card card(static_cast<std::uint8_t>(cardId));
    if (cardId < 0 || !card.valid())
        return QString();
    if (card.tarock())
        return card == tarock::kSkues ? tr("Sküs") : cardKey(cardId);
    const int slot = card.slot();
    const QString suit = QString::fromUtf8(kSuitSymbols[static_cast<int>(card.suit())]);
    if (slot < 4)
        return tr("%1 %2").arg(suit, tr(kCourtNames[slot]));
    // The low cards are named by their rank: ace to four, ten down to seven.
    const QString key = cardKey(cardId);
    return tr("%1 %2").arg(suit, key.mid(key.indexOf(QLatin1Char(':')) + 1));
}

QString TarockEngine::contractLabel(int contractId) const
{
    switch (static_cast<ContractId>(contractId)) {
    case ContractId::None: return QString();
    case ContractId::Trischaken: return tr("Trischaken");
    case ContractId::Rufer: return tr("Rufer");
    case ContractId::Piccolo: return tr("Piccolo");
    case ContractId::Bettel: return tr("Bettel");
    case ContractId::Solorufer: return tr("Solorufer");
    case ContractId::Besserrufer: return tr("Besserrufer");
    case ContractId::Farbendreier: return tr("Farbendreier");
    case ContractId::Sechserdreier: return tr("Sechserdreier");
    case ContractId::Dreier: return tr("Dreier");
    case ContractId::Farbensolo: return tr("Farbensolo");
    case ContractId::PiccoloOuvert: return tr("Piccolo ouvert");
    case ContractId::BettelOuvert: return tr("Bettel ouvert");
    case ContractId::Solodreier: return tr("Solodreier");
    case ContractId::Harom: return tr("Három");
    case ContractId::Ketto: return tr("Kettő");
    case ContractId::Egy: return tr("Egy");
    case ContractId::Szolo: return tr("Szóló");
    }
    return QString();
}

QString TarockEngine::bonusLabel(int bonusId) const
{
    const bool hungarian = m_core.profile().id() == ProfileId::HuIlluItvb2019;
    switch (static_cast<BonusId>(bonusId)) {
    case BonusId::None: return QString();
    case BonusId::Trull: return hungarian ? tr("tulétroá") : tr("Trull");
    case BonusId::AllKings: return hungarian ? tr("négykirály") : tr("All kings");
    case BonusId::KingUltimo: return hungarian ? tr("király ultimó") : tr("King ultimo");
    case BonusId::Pagat: return hungarian ? tr("pagát ultimó") : tr("Pagat");
    case BonusId::Uhu: return tr("Uhu");
    case BonusId::Kakadu: return tr("Kakadu");
    case BonusId::Quapil: return tr("Quapil");
    case BonusId::Valat: return hungarian ? tr("volát") : tr("Valat");
    case BonusId::Duplajatek: return tr("duplajáték");
    case BonusId::SasUltimo: return tr("sas ultimó");
    case BonusId::XXIFogas: return tr("XXI-fogás");
    case BonusId::Centrum: return tr("centrum");
    case BonusId::Kismadar: return tr("kismadár");
    case BonusId::Nagymadar: return tr("nagymadár");
    case BonusId::PagatUhu: return tr("pagát-uhu");
    case BonusId::SasUhu: return tr("sas-uhu");
    case BonusId::KiralyUhu: return tr("király-uhu");
    case BonusId::Tarokk8: return tr("tarokk 8");
    case BonusId::Tarokk9: return tr("tarokk 9");
    case BonusId::Pagatfogas: return tr("pagát-fogás");
    }
    return QString();
}
