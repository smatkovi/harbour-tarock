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

    The middle of the table: every card of the running trick lies towards the
    seat that played it, so four (at a five-seat table: four of five) cards
    never cover each other. slot() and slotRotation() are public because the
    table flies its cards to exactly these points.

    While a card is in the air the table hides it here (hiddenIds) and the
    flying copy draws it; once the trick is decided the winning card is framed
    for a moment before the whole trick flies to its winner.
*/
import QtQuick 2.6
import "."

Item {
    id: area

    property var engine: null
    // tarockEngine.trick: one card map per seat that has played, with `seat`.
    property var cards: []
    property int players: 4
    // The seat that took the trick, set by the table between the last card and
    // the trick flight; -1 while the trick is still open.
    property int winnerSeat: -1
    property var hiddenIds: ({})
    property string deck: "modern"
    property real cardRatio: 1.78
    property color feltColor: "#08492a"

    readonly property real cardHeight: Math.min(Style.itemSizeLarge * 1.1, height * 0.42,
                                                width * 0.3 * cardRatio)
    readonly property real cardWidth: cardHeight / cardRatio

    // Seat 0 lies at the bottom, the rest clockwise. The fifth seat squeezes
    // the two upper places apart; the seat that sits out plays no card.
    function slot(seat) {
        var cx = width / 2
        var cy = height / 2
        var dx = width * 0.28
        var dy = height * 0.26
        if (players === 3) {
            // Zu dritt: unten, links, rechts (tapptarock.md §2.1).
            if (seat === 0)
                return Qt.point(cx, cy + dy)
            if (seat === 1)
                return Qt.point(cx - dx, cy - dy * 0.3)
            return Qt.point(cx + dx, cy - dy * 0.3)
        }
        if (players === 5) {
            if (seat === 0)
                return Qt.point(cx, cy + dy)
            if (seat === 1)
                return Qt.point(cx - dx, cy + dy * 0.2)
            if (seat === 2)
                return Qt.point(cx - dx * 0.55, cy - dy)
            if (seat === 3)
                return Qt.point(cx + dx * 0.55, cy - dy)
            return Qt.point(cx + dx, cy + dy * 0.2)
        }
        if (seat === 0)
            return Qt.point(cx, cy + dy)
        if (seat === 1)
            return Qt.point(cx - dx, cy)
        if (seat === 2)
            return Qt.point(cx, cy - dy)
        return Qt.point(cx + dx, cy)
    }

    function slotRotation(seat) {
        return [-3, 8, -6, 5, 2][seat % 5]
    }

    // The oval the trick is played on.
    Rectangle {
        anchors.centerIn: parent
        width: Math.min(parent.width, parent.height)
        height: width
        radius: width / 2
        color: "transparent"
        border.color: area.feltColor
        border.width: Math.max(1, Style.paddingSmall / 2)
    }

    Repeater {
        model: area.cards

        Item {
            readonly property point place: area.slot(modelData.seat === undefined ? 0 : modelData.seat)
            width: area.cardWidth
            height: area.cardHeight
            x: place.x - width / 2
            y: place.y - height / 2
            rotation: area.slotRotation(modelData.seat === undefined ? 0 : modelData.seat)
            z: index
            opacity: area.hiddenIds[modelData.id] ? 0 : 1

            Rectangle {
                anchors.fill: parent
                anchors.margins: -Style.paddingSmall / 2
                visible: area.winnerSeat >= 0 && area.winnerSeat === modelData.seat
                radius: Style.paddingSmall
                color: "#33ffffff"
                border.width: 2
                border.color: Style.highlightColor
            }

            Card {
                anchors.fill: parent
                cardId: modelData.id
                deck: area.deck
                faceUp: true
            }
        }
    }
}
