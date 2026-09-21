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

    The talon (docs/design.md §6.4). In Königrufen it lies open in two halves:
    a tap takes one, and what the declarer leaves goes to the defenders. The
    Sechserdreier shows one closed packet instead, and the cards of a half the
    engine keeps hidden are drawn face down — the panel does not decide what
    may be seen, it draws what talonHalves carries.

    Whatever else the phase allows (schleifen, taking the whole talon) comes
    out of tarockEngine.options and stands as a chip below the halves.
*/
import QtQuick 1.1
import "."

Item {
    id: panel

    property variant engine: null
    // tarockEngine.talonHalves: a list of card lists, or of maps that carry
    // the cards plus a line about them.
    property variant halves: []
    property variant options: []
    property string deck: "modern"
    property real cardRatio: 1.78
    property color panelColor: "#f0202020"
    // function(option) — set by the table, shows why an option is barred.
    property variant explain: null

    implicitHeight: column.height

    function cardsOf(half) {
        if (!half)
            return []
        return half.cards === undefined ? half : half.cards
    }

    function lineOf(half, index) {
        if (half && half.label)
            return half.label
        var cards = panel.cardsOf(half)
        var tarocks = 0
        var points = 0
        var known = false
        for (var i = 0; i < cards.length; ++i) {
            var card = cards[i]
            if (card && card.suit === 4)
                tarocks++
            if (card && card.points !== undefined) {
                points += card.points
                known = true
            }
        }
        if (known)
            return qsTr("%1 tarock · %2 points").arg(tarocks).arg(points)
        return qsTr("Half %1").arg(index + 1)
    }

    function idOf(card) {
        if (card === undefined || card === null)
            return -1
        if (typeof card === "number")
            return card
        return card.id === undefined ? -1 : card.id
    }

    function take(index) {
        if (engine === null)
            return
        engine.act("taketalon", panel.halves.length > 1 ? index : -1)
    }

    function extras() {
        var result = []
        for (var i = 0; i < options.length; ++i) {
            if (options[i].type !== "taketalon")
                result.push(options[i])
        }
        return result
    }

    function choose(option) {
        if (option.enabled === false) {
            if (explain)
                explain(option)
            return
        }
        if (engine === null)
            return
        engine.act(option.type, option.a === undefined ? -1 : option.a,
                   option.b === undefined ? -1 : option.b)
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
            color: Style.secondaryColor
            text: panel.halves.length > 1 ? qsTr("Tap a half to take it")
                                          : qsTr("Tap the talon to take it")
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: Style.paddingLarge

            Repeater {
                model: panel.halves

                Item {
                    id: half
                    property variant list: panel.cardsOf(modelData)
                    property real cardWidth:
                        Math.min(Style.itemSizeSmall * 0.8,
                                 (column.width / Math.max(1, panel.halves.length)
                                  - 3 * Style.paddingLarge) / Math.max(1, list.length))
                    width: halfColumn.width
                    height: halfColumn.height

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: -Style.paddingSmall / 2
                        radius: Style.paddingSmall
                        color: halfMouse.pressed ? "#33ffffff" : "transparent"
                        border.width: 1
                        border.color: Style.secondaryColor
                    }

                    Column {
                        id: halfColumn
                        spacing: Style.paddingSmall / 2

                        Row {
                            spacing: Style.paddingSmall / 2

                            Repeater {
                                model: half.list

                                Card {
                                    width: half.cardWidth
                                    height: width * panel.cardRatio
                                    cardId: Math.max(0, panel.idOf(modelData))
                                    deck: panel.deck
                                    faceUp: panel.idOf(modelData) >= 0
                                }
                            }
                        }

                        Text {
                            width: parent.width
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                            font.pixelSize: Style.fontSizeTiny
                            color: Style.secondaryColor
                            text: panel.lineOf(modelData, index)
                        }
                    }

                    MouseArea {
                        id: halfMouse
                        anchors.fill: parent
                        onClicked: panel.take(index)
                    }
                }
            }
        }

        Flow {
            width: parent.width
            spacing: Style.paddingSmall

            Repeater {
                model: panel.extras()

                TableButton {
                    text: modelData.speech ? modelData.speech : modelData.label
                    opacity: modelData.enabled === false ? 0.4 : 1.0
                    onClicked: panel.choose(modelData)
                }
            }
        }
    }
}
