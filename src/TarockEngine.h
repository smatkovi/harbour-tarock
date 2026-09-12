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
#ifndef TAROCKENGINE_H
#define TAROCKENGINE_H

#include "core/Ai.h"
#include "core/TarockCore.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

class LearnEngine;

// The QML facade of the rule core (docs/design.md §5). Seat 0 is always the
// local player; the others are computer players, and from M7 on people on
// other devices. The core itself never sleeps: animation phases decouple it
// from the table, a timer paces the computer moves, and a watchdog recovers a
// lost animation.
//
// Everything a seat must not see is filtered here, not in QML: hand() returns
// the local cards only, and seats() reports a partner as "unknown" until the
// called king falls.
class TarockEngine : public QObject
{
    Q_OBJECT

    // --- match and phase ----------------------------------------------------
    Q_PROPERTY(bool active READ active NOTIFY stateChanged)
    Q_PROPERTY(bool canResume READ canResume NOTIFY stateChanged)
    Q_PROPERTY(QString profileKey READ profileKey NOTIFY settingsChanged)
    Q_PROPERTY(QString profileName READ profileName NOTIFY settingsChanged)
    Q_PROPERTY(int players READ players NOTIFY stateChanged)
    Q_PROPERTY(int phase READ phase NOTIFY stateChanged)
    Q_PROPERTY(QString phaseName READ phaseName NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status NOTIFY stateChanged)
    Q_PROPERTY(int actor READ actor NOTIFY stateChanged)
    Q_PROPERTY(bool myTurn READ myTurn NOTIFY stateChanged)
    Q_PROPERTY(int handNumber READ handNumber NOTIFY stateChanged)
    Q_PROPERTY(int handsPerMatch READ handsPerMatch NOTIFY settingsChanged)

    // --- the table ----------------------------------------------------------
    Q_PROPERTY(QVariantList seats READ seats NOTIFY stateChanged)
    Q_PROPERTY(QVariantList hand READ hand NOTIFY stateChanged)
    Q_PROPERTY(QVariantList trick READ trick NOTIFY stateChanged)
    Q_PROPERTY(QVariantList talonHalves READ talonHalves NOTIFY stateChanged)
    Q_PROPERTY(QVariantList discardTray READ discardTray NOTIFY stateChanged)
    Q_PROPERTY(int discardTarget READ discardTarget NOTIFY stateChanged)
    Q_PROPERTY(QVariantList openDiscards READ openDiscards NOTIFY stateChanged)
    Q_PROPERTY(int trickNumber READ trickNumber NOTIFY stateChanged)
    Q_PROPERTY(int tricksTotal READ tricksTotal NOTIFY stateChanged)

    // --- decisions ----------------------------------------------------------
    Q_PROPERTY(QVariantList options READ options NOTIFY stateChanged)
    Q_PROPERTY(QVariantList bonusOptions READ bonusOptions NOTIFY stateChanged)
    Q_PROPERTY(QVariantList kontraTargets READ kontraTargets NOTIFY stateChanged)
    Q_PROPERTY(QString contractName READ contractName NOTIFY stateChanged)
    Q_PROPERTY(QString calledCard READ calledCard NOTIFY stateChanged)
    Q_PROPERTY(int calledCardId READ calledCardId NOTIFY stateChanged)
    // The seat that took the trick now lying on the table, -1 while one is
    // still being played. The table highlights the winning card with it during
    // the pause, before the trick flies away.
    Q_PROPERTY(int trickWinner READ trickWinner NOTIFY stateChanged)

    // --- result -------------------------------------------------------------
    Q_PROPERTY(bool handOver READ handOver NOTIFY stateChanged)
    Q_PROPERTY(bool matchOver READ matchOver NOTIFY stateChanged)
    Q_PROPERTY(QVariantMap ledger READ ledger NOTIFY stateChanged)
    Q_PROPERTY(QVariantMap liveCount READ liveCount NOTIFY stateChanged)

    // --- learning mode ------------------------------------------------------
    // QML reaches the whole learning mode through this one object:
    // tarockEngine.learn.why(…), tarockEngine.learn.level, … (design.md §7.2).
    Q_PROPERTY(LearnEngine* learn READ learn CONSTANT)

    // --- presentation -------------------------------------------------------
    Q_PROPERTY(int visualPhase READ visualPhase NOTIFY visualPhaseChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(QString deck READ deck WRITE setDeck NOTIFY settingsChanged)
    Q_PROPERTY(int difficulty READ difficulty WRITE setDifficulty NOTIFY settingsChanged)
    Q_PROPERTY(bool animationsEnabled READ animationsEnabled WRITE setAnimationsEnabled NOTIFY settingsChanged)
    // Per cent of the normal duration: 100 is normal, 50 twice as fast.
    Q_PROPERTY(int animationSpeed READ animationSpeed WRITE setAnimationSpeed NOTIFY settingsChanged)

public:
    enum VisualPhase { Idle = 0, CardFlight = 1, TrickPause = 2, TrickFlight = 3, Reveal = 4 };
    Q_ENUM(VisualPhase)

    explicit TarockEngine(QObject* parent = nullptr);
    ~TarockEngine() override;

    // --- commands -----------------------------------------------------------
    Q_INVOKABLE void startMatch(const QString& profileKey, int players);
    Q_INVOKABLE void resume();
    Q_INVOKABLE void newMatch();
    Q_INVOKABLE void nextHand();
    // One action. `type` is the lower-case ActionType name ("bid", "pass",
    // "hold", "vorhand", "callking", "calltarock", "taketalon", "concede",
    // "discard", "confirmdiscard", "bonus", "kontra", "ready", "play").
    Q_INVOKABLE bool act(const QString& type, int a = -1, int b = -1);
    // The same, but a warning the table has already shown no longer stops it.
    Q_INVOKABLE bool actConfirmed(const QString& type, int a = -1, int b = -1);
    Q_INVOKABLE void completeCardAnimation();
    Q_INVOKABLE void completeTrickAnimation();
    Q_INVOKABLE void completeReveal();

    Q_INVOKABLE QString cardLabel(int cardId) const;
    Q_INVOKABLE QString cardKey(int cardId) const;
    Q_INVOKABLE QString contractLabel(int contractId) const;
    Q_INVOKABLE QString bonusLabel(int bonusId) const;
    Q_INVOKABLE QVariantList lastTrick() const;
    Q_INVOKABLE QStringList profileKeys() const;
    Q_INVOKABLE QString profileNameFor(const QString& key) const;

    // --- properties ---------------------------------------------------------
    bool active() const { return m_active; }
    bool canResume() const { return m_hasSaved && !m_active; }
    QString profileKey() const;
    QString profileName() const;
    int players() const { return m_core.players(); }
    int phase() const { return static_cast<int>(m_core.phase()); }
    QString phaseName() const;
    QString status() const;
    int actor() const { return m_active ? m_core.actor() : -1; }
    bool myTurn() const;
    int handNumber() const { return m_core.handNumber() + 1; }
    int handsPerMatch() const { return m_handsPerMatch; }

    QVariantList seats() const;
    QVariantList hand() const;
    QVariantList trick() const;
    QVariantList talonHalves() const;
    QVariantList discardTray() const;
    int discardTarget() const;
    QVariantList openDiscards() const;
    int trickNumber() const { return m_core.trickNumber(); }
    int tricksTotal() const;

    QVariantList options() const;
    QVariantList bonusOptions() const;
    QVariantList kontraTargets() const;
    QString contractName() const;
    QString calledCard() const;
    int calledCardId() const;
    int trickWinner() const;

    bool handOver() const { return m_active && m_core.handOver(); }
    bool matchOver() const;
    QVariantMap ledger() const;
    QVariantMap liveCount() const;

    int visualPhase() const { return static_cast<int>(m_visualPhase); }
    bool paused() const { return m_paused; }
    void setPaused(bool value);
    QString deck() const { return m_deck; }
    void setDeck(const QString& value);
    int difficulty() const { return static_cast<int>(m_difficulty); }
    void setDifficulty(int value);
    bool animationsEnabled() const { return m_animationsEnabled; }
    void setAnimationsEnabled(bool value);
    int animationSpeed() const { return m_animationSpeed; }
    void setAnimationSpeed(int value);

    LearnEngine* learn() const { return m_learn; }

    // Test hook.
    const tarock::TarockCore& core() const { return m_core; }

    // --- hooks for the learning mode ----------------------------------------
    // The hint engine ranks with the very same player the opponents use.
    const tarock::AiPlayer& ai() const { return m_ai; }
    // A lesson installs a fixed deal and replays scripted moves; it is the
    // only writer besides the engine itself, and it drives the core directly,
    // without the animation pipeline (design.md §7.6).
    tarock::TarockCore& coreForLearning();
    // A lesson changed the core behind the engine's back: tell the table.
    void lessonStateChanged();

signals:
    void stateChanged();
    void settingsChanged();
    void visualPhaseChanged();
    void pausedChanged();
    void resetVisuals();
    void matchStarted();
    void handFinished();
    void cardAnimationRequested(int cardId, int seat);
    void trickAnimationRequested(int winnerSeat);
    void revealRequested(const QString& what);   // "talon", "skart"
    void actionRejected(const QVariantMap& reason);
    // Allowed, but it costs something. The table shows the sentence and asks
    // again; a confirmed repeat goes through actConfirmed().
    void actionWarned(const QVariantMap& reason, const QString& type, int a, int b);
    void speech(int seat, const QString& formula);

private:
    QVariantMap cardMap(tarock::Card card, bool legal = true) const;
    QVariantMap reasonMap(const tarock::Reason& reason) const;
    QString seatName(int seat) const;
    QString speechFor(const tarock::Action& action) const;
    bool seatIsComputer(int seat) const;

    bool attempt(const QString& type, int a, int b, bool confirmed);
    bool perform(int seat, const tarock::Action& action);
    void setVisualPhase(VisualPhase phase);
    int trickPauseMs() const;
    void finishIdle();
    void scheduleComputer();
    void runComputer();

    void loadSettings();
    void saveSettings();
    void persist();
    void clearSaved();

    tarock::TarockCore m_core;
    tarock::AiPlayer m_ai;
    LearnEngine* m_learn = nullptr;
    tarock::ProfileId m_profileId = tarock::ProfileId::AtKrOoe2023;
    tarock::Difficulty m_difficulty = tarock::Difficulty::Club;

    bool m_active = false;
    bool m_hasSaved = false;
    bool m_paused = false;
    bool m_animationsEnabled = true;
    int m_animationSpeed = 100;
    int m_handsPerMatch = 4;         // one Radl at a table of four
    QString m_deck = QStringLiteral("modern");

    VisualPhase m_visualPhase = Idle;
    QTimer m_aiTimer;
    QTimer m_trickPauseTimer;
    QTimer m_watchdog;
    // The trick that is being flown away; kept so the table can still draw it.
    QVariantList m_flyingTrick;
    int m_flyingWinner = -1;
    // Running totals over the hands of the match, per seat.
    QVector<int> m_matchSchrift;
    QVector<int> m_matchGeld;
};

#endif // TAROCKENGINE_H
