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
pragma Singleton
import QtQuick

// The choices that belong to the next match rather than to the running one,
// so the settings page and the main page share them without passing objects
// through the stack view. sailfish/Prefs.qml is the counterpart.
//
// Deck, difficulty and the animation pace are not kept here: those are
// properties of the engine and it saves them itself. Profile and player count
// are the only two the engine cannot store before the match exists, so they
// wait here until startMatch() hands them over.
QtObject {
    // Rule profile of the next match, e.g. "AT-KR-OOE-2023-04".
    property string profileKey: ""
    // Four or five players at the table (docs/design.md §4.7).
    property int players: 4
    property bool adopted: false

    // Called by the first page that comes up, which passes the engine in: a
    // singleton must not reach for the root context itself. The engine has
    // restored the profile and the table size of the last match, and those are
    // the defaults offered for the next one.
    function adopt(engine) {
        if (adopted || !engine)
            return
        adopted = true
        if (engine.profileKey !== "")
            profileKey = engine.profileKey
        if (engine.players > 0)
            players = engine.players
    }
}
