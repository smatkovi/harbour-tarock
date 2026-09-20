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
#ifndef LEARNENGINE_H
#define LEARNENGINE_H

#include "Lesson.h"
#include "core/Ai.h"
#include "core/Reason.h"
#include "core/TarockCore.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QVector>

class TarockEngine;

// The learning mode of docs/design.md §7: the explanation panel (§7.2), the
// "why not?" texts (§7.3), the hint engine (§7.4), the running helpers (§7.5),
// the lessons (§7.6) and the reference (§7.7).
//
// It is a layer over the running hand, never a second rule engine: every
// answer comes from TarockCore::check(), TarockCore::cardReason() or
// AiPlayer::rank(). Nothing here applies an action of its own except inside a
// lesson, where the script drives the core.
//
// Knowledge limit: the seat this class speaks for is kLocalSeat, and every
// look at the table goes through that seat. See requestHint().
class LearnEngine : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY settingsChanged)   // LearnLevel
    Q_PROPERTY(bool panelOpen READ panelOpen WRITE setPanelOpen NOTIFY panelChanged)
    Q_PROPERTY(QVariantMap explanation READ explanation NOTIFY explanationChanged)
    // { phaseKey, phaseTitle, stepIndex, stepCount, whatHappens, whatToDo,
    //   terms: [{term, definition, anchor}], anchor }
    Q_PROPERTY(QVariantMap hint READ hint NOTIFY hintChanged)
    Q_PROPERTY(QVariantList history READ history NOTIFY historyChanged)  // last 10 refusals
    Q_PROPERTY(bool lessonActive READ lessonActive NOTIFY lessonChanged)
    Q_PROPERTY(QVariantMap lessonStep READ lessonStep NOTIFY lessonChanged)

    // The course of docs/design.md §7.9: the lessons of the active profile in
    // the order of index.json, which of them are done, and whether the guided
    // tour of the table has run. The tour itself is drawn by the QML, which is
    // why only its flag lives here.
    Q_PROPERTY(QVariantList course READ course NOTIFY courseChanged)
    Q_PROPERTY(int lessonCount READ lessonCount NOTIFY courseChanged)
    Q_PROPERTY(int lessonsDone READ lessonsDone NOTIFY courseChanged)
    Q_PROPERTY(QString nextLessonId READ nextLessonId NOTIFY courseChanged)
    Q_PROPERTY(bool tourSeen READ tourSeen WRITE setTourSeen NOTIFY courseChanged)

    // The remaining switches of docs/design.md §7.8; they live in the same
    // QSettings group as the level.
    Q_PROPERTY(bool autoHint READ autoHint WRITE setAutoHint NOTIFY settingsChanged)
    Q_PROPERTY(bool warnBonusLoss READ warnBonusLoss WRITE setWarnBonusLoss NOTIFY settingsChanged)
    Q_PROPERTY(bool dimIllegal READ dimIllegal WRITE setDimIllegal NOTIFY settingsChanged)
    Q_PROPERTY(bool countTutor READ countTutor WRITE setCountTutor NOTIFY settingsChanged)
    Q_PROPERTY(bool playToEnd READ playToEnd WRITE setPlayToEnd NOTIFY settingsChanged)

public:
    // docs/design.md §7.1
    enum LearnLevel { Off = 0, Learning = 1, Novice = 2 };
    Q_ENUM(LearnLevel)

    // Seat 0 is the person in front of the device, everywhere in this app.
    static const int kLocalSeat = 0;

    explicit LearnEngine(TarockEngine* engine, QObject* parent = nullptr);
    ~LearnEngine() override;

    // --- "Warum darf ich das nicht?" — without performing the action -------
    Q_INVOKABLE QVariantMap why(const QString& type, int a = -1, int b = -1) const;
    Q_INVOKABLE QVariantMap whyCard(int cardId) const;
    // "Warum ist das ein guter Zug?" for any allowed action.
    Q_INVOKABLE QVariantMap whyGood(const QString& type, int a = -1, int b = -1) const;
    // "Wie stehe ich gerade?" — card points, tarocks still out, fallen birds.
    Q_INVOKABLE QVariantMap standing() const;
    Q_INVOKABLE void requestHint();
    Q_INVOKABLE void explainTerm(const QString& term);
    Q_INVOKABLE QVariantList commonMistakes() const;     // §11.5
    Q_INVOKABLE QVariantMap debrief() const;             // after the hand

    Q_INVOKABLE void startLesson(const QString& lessonId);
    Q_INVOKABLE void lessonNext();
    Q_INVOKABLE void lessonRestartStep();
    Q_INVOKABLE void stopLesson();

    // --- course and progress, §7.9 ---------------------------------------
    // Title, module and description of one lesson, out of index.json where it
    // lists the lesson and out of the lesson file itself where it does not.
    Q_INVOKABLE QVariantMap lessonInfo(const QString& lessonId) const;
    Q_INVOKABLE bool lessonDone(const QString& lessonId) const;
    Q_INVOKABLE void markLessonDone(const QString& lessonId);
    // Forgets the tour and every finished lesson of the active profile.
    Q_INVOKABLE void resetCourse();

    // --- reference, §7.7 ---------------------------------------------------
    Q_INVOKABLE QVariantList glossary() const;
    Q_INVOKABLE QVariantMap term(const QString& name) const;
    Q_INVOKABLE QString markup(const QString& text) const;
    Q_INVOKABLE QVariantList chapters() const;
    Q_INVOKABLE QVariantList tariffTable() const;
    Q_INVOKABLE QVariantMap contractPurpose(int contractId) const;
    Q_INVOKABLE QVariantMap bonusPurpose(int bonusId) const;
    Q_INVOKABLE QStringList lessonIds() const;
    Q_INVOKABLE QString lastError() const { return m_error; }

    // --- properties --------------------------------------------------------
    int level() const { return static_cast<int>(m_level); }
    void setLevel(int value);
    bool panelOpen() const { return m_panelOpen; }
    void setPanelOpen(bool value);
    QVariantMap explanation() const { return m_explanation; }
    QVariantMap hint() const { return m_hint; }
    QVariantList history() const { return m_history; }
    bool lessonActive() const { return m_lesson.running(); }
    QVariantMap lessonStep() const { return m_lesson.stepMap(); }

    QVariantList course() const;
    int lessonCount() const { return course().size(); }
    int lessonsDone() const;
    QString nextLessonId() const;
    bool tourSeen() const { return m_tourSeen; }
    void setTourSeen(bool value);

    bool autoHint() const { return m_autoHint; }
    void setAutoHint(bool value);
    bool warnBonusLoss() const { return m_warnBonusLoss; }
    void setWarnBonusLoss(bool value);
    bool dimIllegal() const { return m_dimIllegal; }
    void setDimIllegal(bool value);
    bool countTutor() const { return m_countTutor; }
    void setCountTutor(bool value);
    bool playToEnd() const { return m_playToEnd; }
    void setPlayToEnd(bool value);

    // --- hooks TarockEngine calls -----------------------------------------
    // A rejected or warned action, for the "Warum war das verboten?" history.
    void noteRefusal(const tarock::Reason& reason, const QString& type, int a, int b);
    // The local seat is about to play `action`: writes down how it compares to
    // the best action, which is what debrief() reports afterwards (§7.5).
    void noteLocalAction(const tarock::Action& action);
    // Inside a lesson every local action goes through the script first.
    // Returns true when the lesson handled it; *accepted says whether it went
    // through to the core.
    bool lessonAct(const tarock::Action& action, bool* accepted);
    void handStarted();
    void handFinished();
    // The panel follows the phase.
    void refresh();

    // Test hook: the same Reason the texts are built from, enriched.
    tarock::Reason enrich(const tarock::Reason& reason) const;

signals:
    void settingsChanged();
    void courseChanged();
    void panelChanged();
    void explanationChanged();
    void hintChanged();
    void historyChanged();
    void lessonChanged();
    void lessonStepPassed(const QString& text);
    void lessonTrap(const QVariantMap& reason);   // an expected beginner mistake
    // explainTerm() answers through this; the panel opens a popup on it.
    void termExplained(const QVariantMap& term);
    void lessonFailed(const QString& message);

private:
    struct Decision {
        int trick = 0;
        int phase = 0;
        tarock::Action action;
        int playedScore = 0;
        int bestScore = 0;
        tarock::Action best;
    };

    const tarock::TarockCore& core() const;
    tarock::ProfileId profileId() const;
    bool ready() const;
    QVariantMap hintMap(const std::vector<tarock::Hint>& hints, int index) const;
    QString actionLabel(const tarock::Action& action) const;
    static QString qualityOf(int bestScore, int score);
    static QString hintShort(tarock::HintReason reason);
    static QString hintText(tarock::HintReason reason, const tarock::Reason& detail);
    std::vector<tarock::Hint> rankLocal() const;
    void loadSettings();
    void saveSettings();
    QStringList lessonDirectories() const;
    // The course order of the active profile, cached because every binding on
    // the course property asks for it: index.json where there is one, the
    // lesson files in file-name order where there is not.
    const QVariantList& courseOrder() const;
    QStringList doneLessons() const;
    QString progressKey() const;

    TarockEngine* m_engine = nullptr;
    LearnLevel m_level = Learning;
    bool m_panelOpen = false;
    bool m_autoHint = true;
    bool m_warnBonusLoss = true;
    bool m_dimIllegal = true;
    bool m_countTutor = true;
    bool m_playToEnd = false;

    QVariantMap m_explanation;
    QVariantMap m_hint;
    QVariantList m_history;
    QVector<Decision> m_decisions;
    QVector<int> m_mistakesSeen;     // §11.5 numbers hit in this hand
    Lesson m_lesson;
    QString m_error;
    bool m_tourSeen = false;
    // Filled on the first look and thrown away when the profile changes.
    mutable QVariantList m_course;
    mutable bool m_courseLoaded = false;
    mutable tarock::ProfileId m_courseProfile = tarock::ProfileId::AtKrOoe2023;
};

#endif // LEARNENGINE_H
