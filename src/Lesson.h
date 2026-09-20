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
#ifndef LESSON_H
#define LESSON_H

#include "core/TarockCore.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

#include <vector>

// One practice hand: the loader and the state machine for the JSON format of
// docs/design.md §7.6.
//
// A lesson is a fixed deal plus a list of steps. Every step either expects one
// particular action of the learner (`expect`), plays actions of the computer
// players by itself (`auto`), or walks a whole trick (`moves`) in which the
// learner plays only the cards of his own seat. `traps` are the beginner
// mistakes the step expects: hitting one produces an explanation instead of a
// bare refusal (docs/koenigrufen.md §11.5).
//
// The class is free of QObject on purpose: LearnEngine owns it and turns its
// results into signals, and tests/test_learn.cpp drives it without an event
// loop. Nothing here touches the file system unless loadFile() is called.
class Lesson
{
    Q_DECLARE_TR_FUNCTIONS(Lesson)

public:
    // What offering an action to the running lesson did.
    enum Outcome {
        NotRunning,   // no lesson loaded, or it is already finished
        Advanced,     // the expected action, applied to the core
        Trap,         // a listed beginner mistake — explain, do not apply
        Rejected,     // legal, but not what this step is about
        Finished      // the expected action ended the lesson
    };

    // A trap without action and without card is the third kind of
    // assets/lessons/README.md: a plain misconception that cannot be tapped
    // ("35 points are half of it, so I won"). It is shown with the step
    // instead of waiting for a move.
    struct TrapDef {
        tarock::Action action;        // type None = matched by card only
        tarock::Card card;            // invalid = matched by action only
        QString reasonKey;            // "E_ABLAGE_KOENIG", may be empty
        QString text;                 // own wording, may be empty
        // Kontra names its posten by name ("GAME", "PAGAT"); which index that
        // is only follows from the declarations of the running hand, so it is
        // resolved when the action is offered, not when the file is read.
        QString kontraTarget;
    };

    struct Move {
        int seat = -1;
        tarock::Card card;
    };

    // One entry of `auto`. The short form ("pass") leaves the seat open: it
    // runs for whoever is to act as long as that is not the learner. The long
    // form names a seat and may name the learner's own, which is how the
    // modules that start in the middle of a hand wind the previous decisions
    // forward (assets/lessons/README.md). `discardSet` is not an action of the
    // core but the whole discard at once.
    struct AutoAction {
        int seat = -1;
        tarock::Action action;
        tarock::CardList cards;
        bool discardSet = false;
        QString kontraTarget;         // as in TrapDef
    };

    struct Step {
        QString id;
        QString phase;                // "bidding", "call", … — checked, not driving
        int seat = 0;                 // the seat the step belongs to
        bool hasExpect = false;
        QString expectType;           // the action name, "discardSet" included
        tarock::Action expect;
        tarock::CardList expectCards; // discardSet
        QVector<AutoAction> autoActions;
        QString text;
        tarock::CardList highlight;
        QVector<TrapDef> traps;
        QVector<Move> moves;
        int trick = -1;
        int lead = -1;
        int expectWinner = -1;
        int trickUnits = -1;
        QVariantList compare;         // handed to the panel unchanged
    };

    Lesson() = default;

    // --- loading -----------------------------------------------------------
    // Both report a readable message in `error` instead of throwing or
    // crashing on a malformed file; the lesson stays unloaded then.
    bool load(const QByteArray& json, QString* error);
    bool loadFile(const QString& path, QString* error);
    bool loaded() const { return m_loaded; }
    // The deal against the deck of the profile: 54 cards, no duplicates.
    bool validateDeal(QString* error) const;

    // --- what the lesson is ------------------------------------------------
    QString id() const { return m_id; }
    QString title() const { return m_title; }
    QString module() const { return m_module; }
    QString moral() const { return m_moral; }
    QStringList goals() const { return m_goals; }
    QStringList seatNames() const { return m_seatNames; }
    tarock::ProfileId profile() const { return m_profile; }
    int players() const { return m_players; }
    int dealer() const { return m_dealer; }
    int localSeat() const { return m_localSeat; }
    const std::vector<tarock::CardList>& hands() const { return m_hands; }
    const tarock::CardList& talon() const { return m_talon; }
    QVariantMap result() const { return m_result; }
    QVariantMap toMap() const;

    // --- running -----------------------------------------------------------
    // Deals the fixed hand into `core` and runs up to the first decision.
    bool begin(tarock::TarockCore& core, QString* error);
    void stop();
    bool running() const { return m_running; }
    bool finished() const { return m_loaded && m_index >= m_steps.size(); }
    int stepIndex() const { return m_index; }
    int stepCount() const { return m_steps.size(); }
    const Step* currentStep() const;
    QVariantMap stepMap() const;

    // Plays everything the current step does by itself: the `auto` actions and
    // the moves of the other seats. Call it after begin() and after every
    // accepted action.
    void runAutomatic(tarock::TarockCore& core);
    // Offers one action of the local seat. `info` receives the explanation of
    // a trap or of a refusal.
    Outcome offer(tarock::TarockCore& core, const tarock::Action& action, QVariantMap* info);
    // "Weiter" when a step only shows text.
    bool advance(tarock::TarockCore& core);
    // Replays the hand from the deal up to the start of the current step.
    bool restartStep(tarock::TarockCore& core);

private:
    struct Applied {
        int seat = -1;
        tarock::Action action;
    };

    bool parseSteps(const QVariantList& raw, QString* error);
    bool parseStep(const QVariantMap& raw, int position, Step* step, QString* error);
    bool parseExpect(const QVariantMap& raw, Step* step, QString* error);
    bool parseTraps(const QVariantList& raw, Step* step, QString* error);
    // Applies one action and writes it into the journal restartStep() replays.
    bool apply(tarock::TarockCore& core, int seat, const tarock::Action& action);
    // One `auto` entry, which may be a whole discard.
    bool applyAuto(tarock::TarockCore& core, int seat, const AutoAction& entry);
    const TrapDef* trapFor(const tarock::TarockCore& core, const Step& step,
                           const tarock::Action& action) const;
    bool matchesExpect(const Step& step, const tarock::Action& action) const;
    // The next move of the trick the local seat still owes.
    const Move* pendingLocalMove() const;
    bool stepComplete() const;
    void enterStep(int index, tarock::TarockCore& core);

    bool m_loaded = false;
    bool m_running = false;
    QString m_id;
    QString m_title;
    QString m_module;
    QString m_moral;
    QStringList m_goals;
    QStringList m_seatNames;
    tarock::ProfileId m_profile = tarock::ProfileId::AtKrOoe2023;
    int m_players = 4;
    int m_dealer = 0;
    int m_localSeat = 0;
    std::vector<tarock::CardList> m_hands;
    tarock::CardList m_talon;
    QVector<Step> m_steps;
    QVariantMap m_result;

    int m_index = 0;          // the step being worked on
    int m_moveCursor = 0;     // moves of that step already played
    int m_autoCursor = 0;     // `auto` actions of that step already played
    bool m_expectDone = false;
    QVector<Applied> m_journal;
    QVector<int> m_stepStart; // journal length when each step began
};

#endif // LESSON_H
