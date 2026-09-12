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
#ifndef REASONTEXT_H
#define REASONTEXT_H

#include "core/Reason.h"
#include "core/RuleProfile.h"

#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QVariantMap>

// The sentence for every reason code the rule core can produce
// (docs/koenigrufen.md §11.4, docs/design.md §7.3).
//
// Everything here is static and free of state: the learning mode, the toast of
// the table and the lesson script all format the same Reason the same way.
// The wording is German because the rule book is German and §11.4 prescribes
// it word for word; every string goes through tr() so that lupdate finds it
// and a later profile can be translated.
class ReasonText
{
    Q_DECLARE_TR_FUNCTIONS(ReasonText)

public:
    // The long sentence of §11.4, with {Karten}, {Farbe}, {Vogel} … filled in
    // from the Reason. Never empty: a code without a sentence yields a marked
    // placeholder so that tests/test_learn.cpp fails on it.
    static QString textFor(const tarock::Reason& reason);
    // The two or three words the toast shows ("Farbzwang").
    static QString shortLabel(tarock::ReasonCode code);
    // tarock::Severity of the code, as an int for QML.
    static int severity(tarock::ReasonCode code);
    // "rules:at-kr-ooe#6.2" — where "Mehr dazu" jumps to.
    static QString anchor(tarock::ReasonCode code,
                          tarock::ProfileId profile = tarock::ProfileId::AtKrOoe2023);
    // The profile specific key of §11.4 ("E_FARBZWANG"), for rule references
    // and lesson scripts. Empty when the profile does not know the code.
    static QString specKey(tarock::ReasonCode code,
                           tarock::ProfileId profile = tarock::ProfileId::AtKrOoe2023);
    // The other way round: "E_ABLAGE_KOENIG" or "E_DISCARD_KING" back to the
    // code, so a lesson script can name a reason (docs/design.md §7.6).
    static tarock::ReasonCode codeForSpecKey(const QString& key);

    // Everything above in one map, the shape the QML side binds to:
    // { key, specKey, severity, short, text, anchor, cards, card, seat, suit,
    //   bonus, contract, trick, count }
    static QVariantMap toMap(const tarock::Reason& reason,
                             tarock::ProfileId profile = tarock::ProfileId::AtKrOoe2023);

    // True when textFor() had nothing to say. The test asserts this is false
    // for every code of tarock::ReasonCode.
    static bool isPlaceholder(const QString& text);
    // Every code the core defines, in enum order — the test walks this list.
    static QVector<tarock::ReasonCode> allCodes();

    // --- naming, German, shared with RulesIndex and LearnEngine -------------
    static QString cardName(tarock::Card card);          // "Herz-König", "XVIII", "Sküs"
    static QString cardNames(const tarock::CardSet& set);  // "I, V, Herz-Dame"
    static QString cardNames(const tarock::CardList& list);
    static QString suitName(int suit);                   // "Herz" … "Tarock"
    static QString contractName(int contract);           // "Sechserdreier"
    static QString bonusName(int bonus);                 // "Pagat ultimo"
    static QString seatName(int seat);                   // "Du", "Spieler 2"
    static QString ordinalLast(int n);                   // "letzten", "vorletzten", …
    static QString ordinalLastNominative(int n);         // "letzte", "vorletzte", …

private:
    static QString fill(const tarock::Reason& reason);
};

#endif // REASONTEXT_H
