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

    The settlement of one hand (docs/design.md §6.6): the Posten of
    tarockEngine.ledger in the order the scoring built them — the game itself,
    then every bonus, announced or silent — and under them what each seat pays
    in Schrift and Geld. A silent figure is marked as such, a doubled one
    carries its factor, exactly as the example tables of the specifications
    print it.

    The list scrolls; the button below it starts the next hand, or a new match
    once the Radl is full.
*/
import QtQuick 2.6
import "."

Item {
    id: sheet

    property var engine: null
    property var seats: []
    property real maxHeight: Style.itemSizeLarge * 6
    property color panelColor: "#f0202020"
    // function() — the table clears any card still in the air before the
    // engine deals again.
    property var beforeContinue: null

    implicitHeight: Math.min(frame.implicitHeight, maxHeight)

    // struct Ledger (src/core/Scoring.h) as a map: items, schrift, geld.
    function postenList() {
        var ledger = engine === null ? null : engine.ledger
        if (!ledger)
            return []
        if (ledger.items)
            return ledger.items
        return ledger.posten ? ledger.posten : []
    }

    function postenLabel(item) {
        if (item.label)
            return item.label
        if (engine !== null && typeof item.bonus === "number" && item.bonus > 0)
            return engine.bonusLabel(item.bonus)
        return engine === null ? "" : engine.contractName
    }

    function postenNote(item) {
        var parts = []
        if (item.announced === false)
            parts.push(qsTr("silent"))
        if (item.achieved === false)
            parts.push(qsTr("lost"))
        if (item.kontra !== undefined && item.kontra > 1)
            parts.push("×" + item.kontra)
        return parts.join(" · ")
    }

    function amountOf(list, seat) {
        if (!list || seat >= list.length)
            return 0
        return list[seat]
    }

    function signed(value) {
        return (value > 0 ? "+" : "") + value
    }

    function ledgerList(name) {
        var ledger = engine === null ? null : engine.ledger
        return ledger && ledger[name] ? ledger[name] : []
    }

    function continueMatch() {
        if (beforeContinue)
            beforeContinue()
        if (engine === null)
            return
        if (engine.matchOver)
            engine.newMatch()
        else
            engine.nextHand()
    }

    Rectangle {
        id: frame
        anchors.fill: parent
        implicitHeight: content.height + 2 * Style.paddingLarge
        radius: Style.paddingSmall
        color: sheet.panelColor
        border.color: Style.highlightColor

        Flickable {
            anchors.fill: parent
            anchors.margins: Style.paddingLarge
            contentWidth: width
            contentHeight: content.height
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Column {
                id: content
                width: parent.width
                spacing: Style.paddingSmall

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Style.fontSizeMedium
                    color: Style.highlightColor
                    text: sheet.engine !== null && sheet.engine.matchOver
                          ? qsTr("Match over")
                          : qsTr("Hand %1 of %2").arg(sheet.engine === null ? 0 : sheet.engine.handNumber)
                                                 .arg(sheet.engine === null ? 0 : sheet.engine.handsPerMatch)
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeSmall
                    color: Style.primaryColor
                    text: {
                        var ledger = sheet.engine === null ? null : sheet.engine.ledger
                        var name = sheet.engine === null ? "" : sheet.engine.contractName
                        if (!ledger || ledger.declarerWon === undefined)
                            return name
                        // Im Strohmandeln kann das einfache Spiel unentschieden
                        // ausgehen -- dann hat niemand verloren (§1.7.1).
                        if (ledger.gameDrawn === true)
                            return qsTr("%1 drawn").arg(name)
                        return ledger.declarerWon ? qsTr("%1 won").arg(name)
                                                  : qsTr("%1 lost").arg(name)
                    }
                }

                // --- the Posten -------------------------------------------------
                Repeater {
                    model: sheet.postenList()

                    Item {
                        width: content.width
                        height: postenText.height

                        Text {
                            id: postenText
                            anchors.left: parent.left
                            width: parent.width * 0.62
                            elide: Text.ElideRight
                            font.pixelSize: Style.fontSizeExtraSmall
                            color: modelData.achieved === false ? Style.secondaryColor : Style.primaryColor
                            text: {
                                var note = sheet.postenNote(modelData)
                                return sheet.postenLabel(modelData) + (note === "" ? "" : "  (" + note + ")")
                            }
                        }

                        Text {
                            anchors.right: parent.right
                            font.pixelSize: Style.fontSizeExtraSmall
                            color: Style.highlightColor
                            text: modelData.value === undefined ? "" : sheet.signed(modelData.value)
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    color: Style.secondaryColor
                    opacity: 0.4
                }

                // --- what every seat pays ----------------------------------------
                Repeater {
                    model: sheet.seats

                    Item {
                        width: content.width
                        height: seatName.height

                        Text {
                            id: seatName
                            anchors.left: parent.left
                            width: parent.width * 0.5
                            elide: Text.ElideRight
                            font.pixelSize: Style.fontSizeExtraSmall
                            color: modelData.isDeclarer === true ? Style.highlightColor : Style.primaryColor
                            text: modelData.name === undefined ? "" : modelData.name
                        }

                        Text {
                            anchors.right: parent.right
                            font.pixelSize: Style.fontSizeExtraSmall
                            color: Style.primaryColor
                            text: qsTr("Schrift %1 · Geld %2")
                                  .arg(sheet.signed(sheet.amountOf(sheet.ledgerList("schrift"), index)))
                                  .arg(sheet.signed(sheet.amountOf(sheet.ledgerList("geld"), index)))
                        }
                    }
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.secondaryColor
                    text: qsTr("Running total of the match")
                }

                Repeater {
                    model: sheet.seats

                    Item {
                        width: content.width
                        height: totalName.height

                        Text {
                            id: totalName
                            anchors.left: parent.left
                            width: parent.width * 0.5
                            elide: Text.ElideRight
                            font.pixelSize: Style.fontSizeTiny
                            color: Style.secondaryColor
                            text: modelData.name === undefined ? "" : modelData.name
                        }

                        Text {
                            anchors.right: parent.right
                            font.pixelSize: Style.fontSizeTiny
                            color: Style.secondaryColor
                            text: qsTr("Schrift %1 · Geld %2")
                                  .arg(sheet.signed(modelData.schrift === undefined ? 0 : modelData.schrift))
                                  .arg(sheet.signed(modelData.geld === undefined ? 0 : modelData.geld))
                        }
                    }
                }

                TableButton {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: sheet.engine !== null && sheet.engine.matchOver ? qsTr("New match")
                                                                         : qsTr("Next hand")
                    onClicked: sheet.continueMatch()
                }
            }
        }
    }
}
