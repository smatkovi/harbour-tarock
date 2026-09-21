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

    "Why am I not allowed to do that?" — docs/design.md §7.3. The sheet shows
    the long text of a reason map, draws the cards the reason names and links
    into the rule chapter its anchor points at.

    It serves the warning of §7.3 as well: actionWarned(reason, type, a, b)
    means the move is legal but costs a bonus. Then the sheet offers "anyway",
    which repeats the action through actConfirmed() — the engine performs it
    without asking a second time.

    The last ten refused actions (learn.history) hang below the text as the
    "why was that forbidden?" list of hungarian.md §11.1.
*/
import QtQuick 1.1
import "."

Item {
    id: dialog

    // Set by the table; the single place that changes if LearnEngine becomes
    // its own context property.
    property variant learn: null
    property variant engine: null

    property color panelColor: "#f0202020"
    // The three hues Style does not carry, as in TarockTable: the recommended
    // move, the move that costs something, and the shade behind a dialog.
    property color warnColor: "#ffc266"
    property color shadeColor: "#99000000"
    property real cardRatio: 1.78
    property string deck: "modern"

    property variant reason: null
    // A warning keeps the action it was raised for, so "anyway" can repeat it.
    property bool warning: false
    property string pendingType: ""
    property int pendingA: -1
    property int pendingB: -1
    property bool historyOpen: false

    // function(anchor) — set by the table; opens the rule chapter.
    signal openAnchor(variant a1)
    visible: false
    z: 950

    function textOf(name, fallback) {
        if (!dialog.reason || dialog.reason[name] === undefined || dialog.reason[name] === null)
            return fallback
        return dialog.reason[name]
    }

    // `cards` names the cards the rule is about, `card` a single one; both are
    // drawn so the sentence has something to point at.
    function namedCards() {
        var list = []
        var cards = dialog.textOf("cards", null)
        var i
        if (cards) {
            for (i = 0; i < cards.length; ++i)
                list.push(cards[i])
        }
        var single = dialog.textOf("card", -1)
        if (single >= 0 && list.indexOf(single) < 0)
            list.push(single)
        return list
    }

    function cardText(id) {
        if (dialog.engine === null || !dialog.engine.cardLabel)
            return ""
        return dialog.engine.cardLabel(id)
    }

    function severityColor() {
        return dialog.warning ? dialog.warnColor : Style.highlightColor
    }

    function show(reason) {
        dialog.reason = reason
        dialog.warning = false
        dialog.pendingType = ""
        dialog.pendingA = -1
        dialog.pendingB = -1
        dialog.historyOpen = false
        dialog.visible = true
    }

    function warn(reason, type, a, b) {
        dialog.reason = reason
        dialog.warning = true
        dialog.pendingType = type === undefined ? "" : type
        dialog.pendingA = a === undefined ? -1 : a
        dialog.pendingB = b === undefined ? -1 : b
        dialog.historyOpen = false
        dialog.visible = true
    }

    function close() {
        dialog.visible = false
        dialog.reason = null
        dialog.warning = false
    }

    // "Trotzdem": the same action once more, this time without the warning.
    function confirm() {
        var type = dialog.pendingType
        var a = dialog.pendingA
        var b = dialog.pendingB
        dialog.close()
        if (dialog.engine !== null && type !== "")
            dialog.engine.actConfirmed(type, a, b)
    }

    function history() {
        if (dialog.learn === null || !dialog.learn.history)
            return []
        return dialog.learn.history
    }

    // A tap outside cancels; a warning must be answered by one of the buttons.
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (!dialog.warning)
                dialog.close()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: dialog.shadeColor
    }

    Rectangle {
        id: sheet
        anchors.centerIn: parent
        width: parent.width - 2 * Style.paddingMedium
        height: Math.min(flow.height + 2 * Style.paddingMedium, parent.height - 2 * Style.paddingMedium)
        radius: Style.paddingSmall
        color: dialog.panelColor
        border.color: dialog.severityColor()

        // Swallows the taps meant for the sheet itself.
        MouseArea { anchors.fill: parent }

        Flickable {
            id: flow
            anchors.fill: parent
            anchors.margins: Style.paddingMedium
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
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeSmall
                    color: dialog.severityColor()
                    text: {
                        var head = dialog.textOf("short", "")
                        if (head !== "")
                            return head
                        return dialog.warning ? qsTr("That costs something")
                                              : qsTr("Not allowed")
                    }
                }

                Text {
                    width: parent.width
                    visible: text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeExtraSmall
                    color: Style.primaryColor
                    text: dialog.textOf("text", "")
                }

                // The cards the sentence talks about, framed.
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Style.paddingSmall
                    visible: dialog.namedCards().length > 0

                    Repeater {
                        model: dialog.visible ? dialog.namedCards() : []

                        Column {
                            spacing: Style.paddingSmall / 2

                            Item {
                                width: Math.min(Style.itemSizeSmall * 0.8,
                                                (content.width - 4 * Style.paddingSmall) / 5)
                                height: width * dialog.cardRatio

                                Card {
                                    anchors.fill: parent
                                    cardId: modelData
                                    deck: dialog.deck
                                    faceUp: true
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    radius: Style.paddingSmall / 2
                                    color: "transparent"
                                    border.width: 2
                                    border.color: dialog.severityColor()
                                }
                            }

                            Text {
                                width: parent.width
                                horizontalAlignment: Text.AlignHCenter
                                elide: Text.ElideRight
                                font.pixelSize: Style.fontSizeTiny
                                color: Style.secondaryColor
                                text: dialog.cardText(modelData)
                            }
                        }
                    }
                }

                Text {
                    width: parent.width
                    visible: dialog.textOf("anchor", "") !== ""
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Style.fontSizeTiny
                    font.underline: true
                    color: Style.highlightColor
                    text: qsTr("More on this")

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            var target = dialog.textOf("anchor", "")
                            dialog.close()
                            if (dialog.openAnchor)
                                dialog.openAnchor(target)
                        }
                    }
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Style.paddingMedium

                    TableButton {
                        visible: dialog.warning
                        text: qsTr("Anyway")
                        onClicked: dialog.confirm()
                    }

                    TableButton {
                        text: dialog.warning ? qsTr("Cancel") : qsTr("Understood")
                        onClicked: dialog.close()
                    }
                }

                // The history of §7.3; folded away until it is asked for.
                Text {
                    width: parent.width
                    visible: dialog.history().length > 0
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.secondaryColor
                    text: dialog.historyOpen ? qsTr("Hide the earlier ones")
                                             : qsTr("Why was that forbidden?")

                    MouseArea {
                        anchors.fill: parent
                        onClicked: dialog.historyOpen = !dialog.historyOpen
                    }
                }

                Column {
                    width: parent.width
                    visible: dialog.historyOpen
                    spacing: Style.paddingSmall / 2

                    Repeater {
                        model: dialog.historyOpen ? dialog.history() : []

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            font.pixelSize: Style.fontSizeTiny
                            color: Style.secondaryColor
                            text: {
                                if (typeof modelData === "string")
                                    return modelData
                                if (modelData.short)
                                    return modelData.short
                                return modelData.text ? modelData.text : ""
                            }
                        }
                    }
                }
            }
        }
    }
}
