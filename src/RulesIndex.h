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
#ifndef RULESINDEX_H
#define RULESINDEX_H

#include "core/RuleProfile.h"
#include "core/TarockCore.h"

#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>

// The reference side of the learning mode (docs/design.md §7.2 and §7.7):
// the three levels of the explanation panel, the glossary of
// docs/koenigrufen.md §11.6, the chapter list of the rule browser and the
// tariff table.
//
// Everything is a table. The Hungarian profile is added by filling the same
// tables with a second ProfileId, never by writing code: every lookup falls
// back from {profile, phase, contract} to {profile, phase, any contract} and
// from there to the Königrufen row, so a half filled profile still shows
// something sensible.
class RulesIndex
{
    Q_DECLARE_TR_FUNCTIONS(RulesIndex)

public:
    // The whole explanation panel for one situation:
    // { phaseKey, phaseTitle, stepIndex, stepCount, whatHappens, whatToDo,
    //   terms: [{term, definition, anchor}], anchor }
    static QVariantMap explanation(tarock::ProfileId profile, tarock::Phase phase,
                                   tarock::ContractId contract);

    static QString phaseKey(tarock::Phase phase);
    static QString phaseTitle(tarock::ProfileId profile, tarock::Phase phase);
    // Position in the chain of §7.2, "3 von 7 · König rufen".
    static int stepIndex(tarock::Phase phase);
    static int stepCount();

    // --- glossary, koenigrufen.md §11.6 ------------------------------------
    static QVariantList glossary(tarock::ProfileId profile);
    // One entry by term or synonym; an empty map when the term is unknown.
    static QVariantMap term(tarock::ProfileId profile, const QString& name);
    // Every glossary term that occurs in `text`, in the order it occurs.
    static QVariantList termsIn(tarock::ProfileId profile, const QString& text);
    // The same text with <a href="glossary:Begriff">…</a> around every term,
    // so that LearnPanel can make them tappable (§7.7).
    static QString markup(tarock::ProfileId profile, const QString& text);

    // --- rule reference ----------------------------------------------------
    static QVariantList chapters(tarock::ProfileId profile);   // [{anchor, title, level}]
    static QString chapterTitle(tarock::ProfileId profile, const QString& anchor);
    // Contracts and bonuses with their tariffs, straight from RuleProfile —
    // there is no second, copied table (§7.7).
    static QVariantList tariffTable(tarock::ProfileId profile);

    // --- koenigrufen.md §11.3 and §11.5 ------------------------------------
    // "Wozu es das gibt / wann man es wählt" per contract and per bonus.
    static QVariantMap contractPurpose(tarock::ProfileId profile, tarock::ContractId contract);
    static QVariantMap bonusPurpose(tarock::ProfileId profile, tarock::BonusId bonus);
    // The 20 typical beginner mistakes, [{number, title, text, anchor}].
    static QVariantList commonMistakes(tarock::ProfileId profile);
    static QVariantMap mistake(tarock::ProfileId profile, int number);

    // "rules:at-kr-ooe#6.2"
    static QString anchorFor(tarock::ProfileId profile, const QString& chapter);
};

#endif // RULESINDEX_H
