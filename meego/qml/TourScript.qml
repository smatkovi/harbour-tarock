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

    The script of the guided tour of docs/design.md §7.9: what every part of
    the table is for, in the order a beginner meets it. The tour explains the
    app, not the rules — the rules are what the course of §7.6 teaches, and the
    last step hands over to it.

    `target` is the name TarockTable::tourTarget() resolves to an item on the
    table; an empty target means the step talks about the table as a whole and
    the bubble sits in the middle. The texts go through Qt Linguist like every
    other string in the QML, which is why they live here and not in the lesson
    JSON.
*/
import QtQuick 1.1

QtObject {
    property variant steps: [
        {
            "target": "",
            "title": qsTr("Welcome to the table"),
            "text": qsTr("You sit at the bottom, the computer players sit around you. This short tour shows what each part of the table does. What the game itself is about - bids, the called king, the talon - is taught by the course.")
        },
        {
            "target": "header",
            "title": qsTr("The bar at the top"),
            "text": qsTr("The rule profile, the contract as soon as one is settled and the called king. Below it the hand and the trick you are in, and the card count of both parties as far as it is already known.")
        },
        {
            "target": "seats",
            "title": qsTr("The other players"),
            "text": qsTr("Each of the other players has a panel like the framed one: it shows what that player has just said - a bid, an announcement, Kontra. As soon as the called king falls, your partner is marked green and your opponents red.")
        },
        {
            "target": "trickArea",
            "title": qsTr("The trick in the middle"),
            "text": qsTr("The cards played to the current trick lie here, each in front of its seat. The winning card is framed for a moment, then the trick flies to the player who took it.")
        },
        {
            "target": "statusLabel",
            "title": qsTr("What is expected of you"),
            "text": qsTr("This line says whose turn it is and what the table is waiting for. It is the first place to look when nothing seems to happen.")
        },
        {
            "target": "handFan",
            "title": qsTr("Your hand"),
            "text": qsTr("Tap a card to play it. A card the rules forbid is drawn dimmed, and tapping it does not play it but says why it is not allowed. While you discard, a tapped card moves to the tray instead.")
        },
        {
            "target": "actionBand",
            "title": qsTr("The band above your hand"),
            "text": qsTr("Everything that is not a card: bidding, the two talon halves, the discard tray, the announcements and Kontra. The band changes with the phase, so it always offers exactly the decision that is due.")
        },
        {
            "target": "learnPanel",
            "title": qsTr("The explanation bar"),
            "text": qsTr("With the learning mode switched on it says where you are in the hand, what happens in this phase and what you have to decide now. "More on this" opens the matching rule chapter.")
        },
        {
            "target": "learnBar",
            "title": qsTr("Hint, why and standing"),
            "text": qsTr("Hint proposes a move and gives the reason for it, "Why?" explains the last refusal, and Standing counts what your party has so far. When the hand is over, Review goes through your decisions. This bar and the explanation bar belong to the learning mode, which the settings can turn down or off.")
        },
        {
            "target": "",
            "title": qsTr("That was the table"),
            "text": qsTr("The course under "Tutorial" teaches the game itself: eight modules and three practice hands, each with a fixed deal, one step at a time. A lesson can be left at any moment, and its hand played out freely.")
        }
    ]
}
