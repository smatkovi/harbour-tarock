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

    One seat of the table: name, the cards it still holds, the tricks it took,
    the dealer and forehand marks, the declarer mark, what the partner search
    knows about it and the last formula it spoke. Everything comes out of one
    entry of tarockEngine.seats (docs/design.md §5); the engine hides what this
    seat must not see, this file only draws it.

    The parent gives the panel a width; the height follows the content, so the
    panel works both around the edge of the table and as the one-line strip of
    the local player (compact: true).
*/
import QtQuick 2.6
import "."

Item {
    id: panel

    property var engine: null
    property int seat: 0
    property var info: ({})
    // The last thing this seat said, from the engine's speech() signal.
    property string said: ""
    property string deck: "modern"
    property real cardRatio: 1.78
    property bool compact: false
    property color partnerColor: "#7fd67f"
    property color opponentColor: "#ff8a80"

    readonly property bool isActor: engine !== null && engine.actor === seat
    readonly property bool sittingOut: info.isSittingOut === true
    readonly property string party: info.partnerState === undefined ? "unknown" : info.partnerState

    implicitHeight: column.height

    // Announcements arrive either as plain formulas or as option maps.
    function listText(items) {
        if (!items || !items.length)
            return ""
        var parts = []
        for (var i = 0; i < items.length; ++i) {
            var item = items[i]
            if (typeof item === "string")
                parts.push(item)
            else if (item.label)
                parts.push(item.label)
            else if (item.speech)
                parts.push(item.speech)
        }
        return parts.join(" · ")
    }

    function marks() {
        var text = ""
        if (info.isDealer === true)
            text += "● "          // the dealer
        if (info.isForehand === true)
            text += "▶ "          // forehand leads
        if (info.isDeclarer === true)
            text += "★ "          // the declarer
        return text
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: -Style.paddingSmall / 2
        radius: Style.paddingSmall
        color: panel.isActor ? "#33ffffff" : "transparent"
        border.width: panel.party === "unknown" ? 0 : 2
        border.color: panel.party === "partner" ? panel.partnerColor : panel.opponentColor
        opacity: panel.sittingOut ? 0.5 : 1
    }

    Column {
        id: column
        width: parent.width
        spacing: 2
        opacity: panel.sittingOut ? 0.45 : 1

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            font.pixelSize: Style.fontSizeExtraSmall
            font.bold: info.isDeclarer === true
            color: panel.isActor ? Style.highlightColor : Style.primaryColor
            text: panel.marks() + (info.name === undefined ? "" : info.name)
        }

        // The cards this seat still holds, face down; the local player has the
        // real fan below the table, so his strip stays one line high.
        Item {
            width: parent.width
            height: Style.itemSizeSmall * 0.5
            visible: !panel.compact

            Repeater {
                model: Math.min(info.cardCount === undefined ? 0 : info.cardCount, 8)

                Card {
                    property int shown: Math.min(info.cardCount === undefined ? 0 : info.cardCount, 8)
                    height: parent.height
                    width: height / panel.cardRatio
                    x: (parent.width - width - (shown - 1) * width * 0.34) / 2 + index * width * 0.34
                    cardId: 0
                    deck: panel.deck
                    faceUp: false
                }
            }
        }

        Text {
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            font.pixelSize: Style.fontSizeTiny
            color: Style.secondaryColor
            text: {
                var parts = []
                if (panel.sittingOut)
                    parts.push(qsTr("sits out"))
                if (panel.party === "partner")
                    parts.push(qsTr("partner"))
                else if (panel.party === "opponent")
                    parts.push(qsTr("opponent"))
                if (info.isDeclarer === true)
                    parts.push(qsTr("declarer"))
                parts.push(qsTr("%n card(s)", "", info.cardCount === undefined ? 0 : info.cardCount))
                parts.push(qsTr("%n trick(s)", "", info.tricks === undefined ? 0 : info.tricks))
                return parts.join(" · ")
            }
        }

        Text {
            width: parent.width
            visible: text !== ""
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            font.pixelSize: Style.fontSizeTiny
            color: Style.secondaryHighlightColor
            text: panel.listText(info.announcements)
        }

        // What the seat said last: the bid formula, "ich halte", "Ich liege".
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(column.width, saidText.implicitWidth + 2 * Style.paddingSmall)
            height: saidText.height + Style.paddingSmall
            visible: panel.said !== "" || (info.lastBid !== undefined && info.lastBid !== "")
            radius: Style.paddingSmall / 2
            color: "#40000000"

            Text {
                id: saidText
                anchors.centerIn: parent
                width: parent.width - Style.paddingSmall
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                font.pixelSize: Style.fontSizeTiny
                color: Style.highlightColor
                text: panel.said !== "" ? panel.said
                                        : (info.lastBid === undefined ? "" : info.lastBid)
            }
        }
    }
}
