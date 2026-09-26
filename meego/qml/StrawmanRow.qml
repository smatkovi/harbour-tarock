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

    Die Strohmänner eines Sitzes (docs/strohmandeln.md §4): drei Päckchen, auf
    jedem liegt die oberste Karte offen als "Deckblatt". Sie gehört zum Blatt --
    der Spieler kann sie ausspielen, mit ihr stechen und zugeben (§4.4) -- und
    darum ist sie hier anklickbar wie eine Karte im Fächer. Was darunter liegt,
    weiß niemand; die Zahl daneben sagt nur, wie viele Karten es noch sind.
*/
import QtQuick 1.1
import "."

Item {
    id: row

    property variant engine: null
    // Ein Eintrag je Päckchen: { count, hasCard, card }.
    property variant packets: []
    // Die Karten, die dieser Sitz zuletzt offen aus einem Päckchen ins Blatt
    // genommen hat. Sie stehen hier, weil der Gegner sie sehen muss (§4.2) --
    // klein und nicht anklickbar, sie liegen ja längst im Blatt.
    property variant taken: []
    // Nur der eigene Sitz darf sein Deckblatt anklicken.
    property bool own: false
    property bool interactive: true
    property string deck: "modern"
    property real cardRatio: 1.78
    property real maxCardHeight: Style.itemSizeMedium
    property color panelColor: "#f0202020"
    // function(reason) -- die Tafel zeigt den Grund, warum es gerade nicht geht.
    signal explain(variant a1)
    property int count: packets ? packets.length : 0
    property real cardHeight: Math.min(maxCardHeight,
                                                (width - 2 * Style.paddingSmall)
                                                / Math.max(1, count) * cardRatio * 0.82)
    property real cardWidth: cardHeight / cardRatio

    property int takenCount: taken ? taken.length : 0
    property real takenHeight: takenCount > 0 ? cardHeight * 0.55 : 0

    visible: count > 0
    implicitHeight: count > 0 ? cardHeight + takenHeight + Style.paddingSmall : 0
    height: implicitHeight

    function tap(packet) {
        if (!own || !interactive || engine === null || packet.hasCard !== true)
            return
        var card = packet.card
        if (card.legal === false) {
            if (explain)
                explain(card.dimReason)
            return
        }
        engine.act("play", card.id)
    }

    // Aufgenommen, für den Gegner sichtbar.
    Row {
        id: takenRow
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        height: row.takenHeight
        spacing: Style.paddingSmall / 2
        visible: row.takenCount > 0

        Repeater {
            model: row.taken

            Card {
                height: row.takenHeight
                width: height / row.cardRatio
                cardId: modelData.id
                deck: row.deck
                faceUp: true
            }
        }
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: takenRow.bottom
        spacing: Style.paddingMedium

        Repeater {
            model: row.packets

            Item {
                width: row.cardWidth
                height: row.cardHeight
                opacity: modelData.count > 0 || modelData.hasCard === true ? 1.0 : 0.0

                // Das verdeckte Päckchen darunter, leicht versetzt, damit man
                // sieht, dass noch etwas liegt.
                Card {
                    anchors.fill: parent
                    anchors.leftMargin: -Style.paddingSmall / 2
                    anchors.topMargin: -Style.paddingSmall / 2
                    visible: modelData.count > 0
                    deck: row.deck
                    faceUp: false
                }

                Card {
                    id: faceCard
                    anchors.fill: parent
                    visible: modelData.hasCard === true
                    cardId: modelData.hasCard === true ? modelData.card.id : -1
                    deck: row.deck
                    faceUp: true
                    opacity: row.own && modelData.hasCard === true
                             && modelData.card.legal === false ? 0.35 : 1.0
                }

                // Wie viele Karten noch verdeckt darunter liegen.
                Rectangle {
                    visible: modelData.count > 0
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.margins: Style.paddingSmall / 2
                    width: countText.implicitWidth + Style.paddingSmall
                    height: countText.implicitHeight + Style.paddingSmall / 2
                    radius: height / 2
                    color: row.panelColor

                    Text {
                        id: countText
                        anchors.centerIn: parent
                        font.pixelSize: Style.fontSizeTiny
                        color: Style.primaryColor
                        text: String(modelData.count)
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: row.own && modelData.hasCard === true
                    onClicked: row.tap(modelData)
                }
            }
        }
    }
}
