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

    The place where the declarer lays his skart (docs/design.md §6.4): one slot
    per card he owes, filled by a tap in the hand fan and emptied by a tap in
    the tray — both are the same toggling "discard" action. A card that has to
    be laid open (a tarock, in the Hungarian profile the XX) carries an eye, so
    the player sees before he confirms what the table will learn about him.

    "Done" stays barred until the tray is full; the count in the line above
    says how many cards are still missing.
*/
import QtQuick 1.1
import "."

Item {
    id: tray

    property variant engine: null
    // tarockEngine.discardTray: the cards already chosen, as card maps.
    property variant cards: []
    // Ids out of tarockEngine.openDiscards; those go down face up.
    property variant openIds: ({})
    property int target: 0
    property string deck: "modern"
    property real cardRatio: 1.78
    property color panelColor: "#f0202020"
    // function(reason) — set by the table, shows why a card may not go down.
    signal explain(variant a1)
    property int missing: Math.max(0, target - (cards ? cards.length : 0))
    property real slotWidth:
        Math.min(Style.itemSizeSmall * 0.8,
                 (width - (Math.max(1, target) + 1) * Style.paddingSmall) / Math.max(1, target))

    implicitHeight: column.height

    function cardAt(index) {
        return cards && index < cards.length ? cards[index] : null
    }

    function takeBack(index) {
        var card = tray.cardAt(index)
        if (card === null || engine === null)
            return
        engine.act("discard", card.id)
    }

    Column {
        id: column
        width: parent.width
        spacing: Style.paddingSmall

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            font.pixelSize: Style.fontSizeTiny
            color: tray.missing > 0 ? Style.secondaryColor : Style.highlightColor
            text: tray.missing > 0 ? qsTr("%n more card(s) to put away", "", tray.missing)
                                   : qsTr("Ready to put away")
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Style.paddingSmall

            Repeater {
                model: tray.target

                Item {
                    property variant card: tray.cardAt(index)
                    width: tray.slotWidth
                    height: width * tray.cardRatio

                    Rectangle {
                        anchors.fill: parent
                        visible: parent.card === null
                        radius: Style.paddingSmall / 2
                        color: "transparent"
                        border.width: 1
                        border.color: Style.secondaryColor
                        opacity: 0.6
                    }

                    Card {
                        anchors.fill: parent
                        visible: parent.card !== null
                        cardId: parent.card === null ? 0 : parent.card.id
                        deck: tray.deck
                        faceUp: true
                    }

                    // Laid open: the other seats get to see this one.
                    Text {
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.margins: Style.paddingSmall / 2
                        visible: parent.card !== null && tray.openIds[parent.card.id] === true
                        font.pixelSize: Style.fontSizeTiny
                        color: Style.highlightColor
                        text: "◉"
                    }

                    MouseArea {
                        anchors.fill: parent
                        enabled: parent.card !== null
                        onClicked: tray.takeBack(index)
                    }
                }
            }
        }

        TableButton {
            anchors.horizontalCenter: parent.horizontalCenter
            enabled: tray.missing === 0 && tray.target > 0
            opacity: enabled ? 1.0 : 0.4
            text: qsTr("Done")
            onClicked: {
                if (tray.engine !== null)
                    tray.engine.act("confirmdiscard")
            }
        }
    }
}
