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
import QtQuick

// A card in flight: from a hand to the trick, from the trick to the winner's
// pile, or from the talon into a hand. The table creates one per moving card
// and drops it again on finished(); the engine leaves its animation phase when
// the last one is done (docs/design.md §5).
//
// Same property interface as sailfish/FlyingCard.qml.
Item {
    id: root

    // --- the card it shows (same names as Card.qml) -------------------------
    property int cardId: -1
    property string label: ""
    property int suit: -1
    property bool dimmed: false
    property bool selected: false
    property bool faceDown: false
    property bool faceUp: !faceDown
    property string deck: ""
    signal clicked()

    // --- the flight ---------------------------------------------------------
    // Centre points, not corners: the table computes them from item centres.
    property real fromX: 0
    property real fromY: 0
    property real toX: 0
    property real toY: 0
    property real fromRotation: 0
    property real toRotation: 0
    property int flightDuration: 320
    property int startDelay: 0
    // Lets the table tell a flying card from a resting one.
    property bool isFlyingCard: true
    signal finished()

    // The pace is an engine setting and the engine saves it: animationSpeed is
    // the per cent of the normal duration (100 normal, 50 twice as fast), and
    // with the animations switched off every flight is over at once.
    property var engine: tarockEngine
    readonly property int paceP: engine && engine.animationSpeed > 0 ? engine.animationSpeed : 100
    readonly property bool animate: !engine || engine.animationsEnabled
    readonly property int scaledDuration: animate ? Math.max(1, Math.round(flightDuration * paceP / 100)) : 1
    readonly property int scaledDelay: animate ? Math.max(0, Math.round(startDelay * paceP / 100)) : 0

    width: Theme.itemSizeSmall
    height: width * 1.6
    x: fromX - width / 2
    y: fromY - height / 2
    rotation: fromRotation
    z: 1000

    Card {
        anchors.fill: parent
        cardId: root.cardId
        label: root.label
        suit: root.suit
        dimmed: root.dimmed
        selected: root.selected
        faceDown: root.faceDown
        faceUp: root.faceUp
        deck: root.deck
        onClicked: root.clicked()
    }

    SequentialAnimation {
        running: true
        PauseAnimation { duration: root.scaledDelay }
        ParallelAnimation {
            NumberAnimation {
                target: root; property: "x"
                from: root.fromX - root.width / 2
                to: root.toX - root.width / 2
                duration: root.scaledDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: root; property: "y"
                from: root.fromY - root.height / 2
                to: root.toY - root.height / 2
                duration: root.scaledDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: root; property: "rotation"
                from: root.fromRotation; to: root.toRotation
                duration: root.scaledDuration
                easing.type: Easing.OutCubic
            }
            NumberAnimation {
                target: root; property: "scale"
                from: 0.88; to: 1.0
                duration: root.scaledDuration
                easing.type: Easing.OutCubic
            }
        }
        ScriptAction { script: root.finished() }
    }
}
