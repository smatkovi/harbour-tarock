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

    The hand of the local player as an overlapping fan (docs/design.md §6.2).
    Twelve tall cards have to fit the width of a portrait phone, so the cards
    overlap by between 38 % and 72 % of their width; from eleven cards on, the
    suits are set apart by one small padding, tarock first.

    Legality is always visible: a card the rules refuse is drawn dimmed and a
    tap on it does not play but shows its dimReason — the hook the learning
    mode of M6 turns into the "why not?" dialog.
*/
import QtQuick 1.1
import "."

Item {
    id: fan

    property variant engine: null
    // One entry of tarockEngine.hand per card (docs/design.md §5):
    // { id, key, label, suit, power, legal, warn, bound, boundTrick,
    //   recommended, dimReason }
    property variant cards: []
    // Ids that already lie in the discard tray; they stay in the fan, raised.
    property variant chosenIds: ({})
    property bool discarding: false
    property bool interactive: true
    property string deck: "modern"
    property real cardRatio: 1.78
    property real maxCardHeight: Style.itemSizeLarge
    // function(reason) — set by the table, shows the "why not?" text.
    signal explain(variant a1)
    property int count: cards ? cards.length : 0
    property real cardWidth: (function() {
        var room = Math.max(Style.itemSizeSmall, width - 2 * Style.paddingSmall)
        var byHeight = maxCardHeight / cardRatio
        if (count <= 1)
            return Math.min(room, byHeight)
        // Even at the tightest overlap the whole fan must fit the width.
        return Math.min(room / (1 + 0.28 * (count - 1)), byHeight)
    })()
    property real cardHeight: cardWidth * cardRatio
    // The x of every card, worked out once per change instead of per delegate.
    property variant offsets: []

    implicitHeight: cardHeight + Style.paddingLarge

    function suitOf(index) {
        var card = cards[index]
        return card === undefined || card.suit === undefined ? -1 : card.suit
    }

    function relayout() {
        var result = []
        var total = count
        if (total <= 0) {
            offsets = result
            return
        }
        var groups = []
        var breaks = 0
        for (var i = 0; i < total; ++i) {
            if (i > 0 && fan.suitOf(i) !== fan.suitOf(i - 1))
                breaks++
            groups.push(breaks)
        }
        var card = cardWidth
        var room = Math.max(card, width - 2 * Style.paddingSmall)
        var gap = total > 10 ? Style.paddingSmall : 0
        var step = total > 1 ? (room - card - breaks * gap) / (total - 1) : 0
        if (step > card * 0.62)
            step = card * 0.62
        if (step < card * 0.28) {
            step = card * 0.28
            gap = 0
        }
        var used = card + (total - 1) * step + breaks * gap
        var start = (width - used) / 2
        for (i = 0; i < total; ++i)
            result.push(start + i * step + groups[i] * gap)
        offsets = result
    }

    function tap(card) {
        if (!interactive || engine === null)
            return
        if (card.legal === false) {
            if (explain)
                explain(card.dimReason)
            return
        }
        if (discarding)
            engine.act("discard", card.id)
        else
            engine.act("play", card.id)
    }

    onWidthChanged: fan.relayout()
    onCardsChanged: fan.relayout()
    onCardWidthChanged: fan.relayout()
    Component.onCompleted: fan.relayout()

    Repeater {
        model: fan.cards

        Item {
            id: slot
            property bool chosen: fan.chosenIds[modelData.id] === true
            width: fan.cardWidth
            height: fan.cardHeight
            x: index < fan.offsets.length ? fan.offsets[index] : 0
            y: fan.height - height - (cardMouse.pressed || chosen ? Style.paddingLarge : 0)
            z: cardMouse.pressed ? 100 : index
            opacity: modelData.legal === false ? 0.35 : 1.0

            Behavior on y {
                NumberAnimation { duration: 120; easing.type: Easing.OutCubic }
            }

            Card {
                anchors.fill: parent
                cardId: modelData.id
                deck: fan.deck
                faceUp: true
            }

            // Green for the move the learning mode recommends, amber for a
            // card that is allowed but gives a bonus away (§7.3, Severity
            // Warning); both stay dark until M6 fills them in.
            Rectangle {
                anchors.fill: parent
                visible: modelData.recommended === true || modelData.warn === true
                radius: Style.paddingSmall / 2
                color: "transparent"
                border.width: 2
                border.color: modelData.recommended === true ? "#7fd67f" : "#ffc266"
            }

            // A bird that is bound to a later trick carries the trick number.
            Rectangle {
                visible: modelData.bound === true
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: Style.paddingSmall / 2
                width: boundText.implicitWidth + Style.paddingSmall
                height: boundText.implicitHeight + Style.paddingSmall / 2
                radius: height / 2
                color: "#cc000000"

                Text {
                    id: boundText
                    anchors.centerIn: parent
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.highlightColor
                    text: "⊘" + (modelData.boundTrick > 0 ? String(modelData.boundTrick) : "")
                }
            }

            MouseArea {
                id: cardMouse
                anchors.fill: parent
                onClicked: fan.tap(modelData)
            }
        }
    }
}
