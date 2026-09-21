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

    The review after the hand (docs/design.md §7.5): score, the bonuses that
    fell, and the one move where the played card differed most from the best
    one the ranking saw — "the hand turned on trick 5". The moral of a practice
    game (§7.6) closes it, and the twenty typical beginner mistakes of §11.5
    hang underneath, foldable.

    Everything is read defensively from learn.debrief(): a profile may leave
    single fields out, and the panel then simply drops those lines.
*/
import QtQuick 1.1
import "."

Item {
    id: debrief

    // Set by the table; the single place that changes if LearnEngine becomes
    // its own context property.
    property variant learn: null
    property variant engine: null

    property color panelColor: "#f0202020"
    property color shadeColor: "#99000000"
    property variant report: ({})
    property variant mistakes: []
    property bool mistakesOpen: false

    visible: false
    z: 820

    function value(name, fallback) {
        if (!debrief.report || debrief.report[name] === undefined || debrief.report[name] === null)
            return fallback
        return debrief.report[name]
    }

    function refresh() {
        if (debrief.learn === null || !debrief.learn.debrief) {
            debrief.report = ({})
            return
        }
        var map = debrief.learn.debrief()
        debrief.report = map ? map : ({})
    }

    function open() {
        debrief.refresh()
        debrief.mistakesOpen = false
        debrief.visible = true
    }

    function close() {
        debrief.visible = false
    }

    function loadMistakes() {
        if (debrief.learn === null || !debrief.learn.commonMistakes) {
            debrief.mistakes = []
            return
        }
        var list = debrief.learn.commonMistakes()
        debrief.mistakes = list ? list : []
    }

    function toggleMistakes() {
        if (!debrief.mistakesOpen && debrief.mistakes.length === 0)
            debrief.loadMistakes()
        debrief.mistakesOpen = !debrief.mistakesOpen
    }

    // Score and anything else the engine hands over as label/value rows.
    function lines() {
        var result = []
        if (debrief.value("score", "") !== "")
            result.push({ "label": qsTr("Result"), "value": String(debrief.value("score", "")) })
        var points = debrief.value("points", null)
        if (points) {
            var declarer = points.declarer === undefined ? 0 : points.declarer
            var defenders = points.defenders === undefined ? 0 : points.defenders
            result.push({ "label": qsTr("Card points"),
                          "value": qsTr("%1 declarer · %2 defenders").arg(declarer).arg(defenders) })
        }
        var rows = debrief.value("rows", [])
        for (var i = 0; i < rows.length; ++i) {
            result.push({ "label": rows[i].label === undefined ? "" : rows[i].label,
                          "value": rows[i].value === undefined ? "" : String(rows[i].value) })
        }
        return result
    }

    function bonuses() {
        var list = debrief.value("bonuses", [])
        return list ? list : []
    }

    function bonusText(entry) {
        if (typeof entry === "string")
            return entry
        var head = entry.label !== undefined ? entry.label
                                             : (entry.key !== undefined ? entry.key : "")
        var marks = []
        if (entry.announced === true)
            marks.push(qsTr("announced"))
        if (entry.achieved !== undefined)
            marks.push(entry.achieved ? qsTr("made") : qsTr("lost"))
        if (entry.party !== undefined && entry.party !== "")
            marks.push(String(entry.party))
        return marks.length > 0 ? head + " — " + marks.join(" · ") : head
    }

    // The move with the largest distance to the best one the ranking saw.
    function decisive() {
        var move = debrief.value("keyMove", null)
        if (!move)
            move = debrief.value("decisive", null)
        return move
    }

    function decisiveHead() {
        var move = debrief.decisive()
        if (!move)
            return ""
        if (move.trick !== undefined && move.trick > 0)
            return qsTr("The hand turned on trick %1").arg(move.trick)
        return qsTr("The decisive move")
    }

    function mistakeText(entry) {
        if (typeof entry === "string")
            return entry
        if (entry.short && entry.text)
            return entry.short + " — " + entry.text
        if (entry.short)
            return entry.short
        return entry.text ? entry.text : ""
    }

    MouseArea {
        anchors.fill: parent
        onClicked: debrief.close()
    }

    Rectangle {
        anchors.fill: parent
        color: debrief.shadeColor
    }

    Rectangle {
        anchors.centerIn: parent
        width: parent.width - 2 * Style.paddingMedium
        height: Math.min(column.height + 2 * Style.paddingMedium,
                         parent.height - 2 * Style.paddingMedium)
        radius: Style.paddingSmall
        color: debrief.panelColor
        border.color: Style.highlightColor

        MouseArea { anchors.fill: parent }

        Flickable {
            anchors.fill: parent
            anchors.margins: Style.paddingMedium
            contentWidth: width
            contentHeight: column.height
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Column {
                id: column
                width: parent.width
                spacing: Style.paddingSmall

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeSmall
                    color: Style.highlightColor
                    text: debrief.value("title", qsTr("How the hand went"))
                }

                Repeater {
                    model: debrief.visible ? debrief.lines() : []

                    Column {
                        width: column.width
                        spacing: 0

                        Text {
                            width: parent.width
                            font.pixelSize: Style.fontSizeTiny
                            color: Style.secondaryColor
                            text: modelData.label
                        }

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            font.pixelSize: Style.fontSizeExtraSmall
                            color: Style.primaryColor
                            text: modelData.value
                        }
                    }
                }

                Text {
                    width: parent.width
                    visible: debrief.bonuses().length > 0
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.secondaryColor
                    text: qsTr("Bonuses")
                }

                Repeater {
                    model: debrief.visible ? debrief.bonuses() : []

                    Text {
                        width: column.width
                        wrapMode: Text.WordWrap
                        font.pixelSize: Style.fontSizeExtraSmall
                        color: Style.primaryColor
                        text: debrief.bonusText(modelData)
                    }
                }

                Column {
                    width: parent.width
                    visible: debrief.decisive() !== null
                    spacing: 0

                    Text {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        font.pixelSize: Style.fontSizeTiny
                        color: Style.secondaryHighlightColor
                        text: debrief.decisiveHead()
                    }

                    Text {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        font.pixelSize: Style.fontSizeExtraSmall
                        color: Style.primaryColor
                        text: {
                            var move = debrief.decisive()
                            return move && move.text ? move.text : ""
                        }
                    }
                }

                Text {
                    width: parent.width
                    visible: text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeExtraSmall
                    color: Style.secondaryHighlightColor
                    text: debrief.value("moral", "")
                }

                Text {
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Style.fontSizeTiny
                    font.underline: true
                    color: Style.highlightColor
                    text: debrief.mistakesOpen ? qsTr("Hide the typical mistakes")
                                               : qsTr("Typical beginner mistakes")

                    MouseArea {
                        anchors.fill: parent
                        onClicked: debrief.toggleMistakes()
                    }
                }

                Repeater {
                    model: debrief.mistakesOpen ? debrief.mistakes : []

                    Text {
                        width: column.width
                        wrapMode: Text.WordWrap
                        font.pixelSize: Style.fontSizeTiny
                        color: Style.secondaryColor
                        text: "· " + debrief.mistakeText(modelData)
                    }
                }

                TableButton {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Close")
                    onClicked: debrief.close()
                }
            }
        }
    }
}
