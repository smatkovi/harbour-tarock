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

    The hint of docs/design.md §7.4: the recommended action with the one
    sentence that justifies it, from learn.hint — the same ranking that steers
    the computer players, so the advice never contradicts what they would do.

    The bubble never opens by itself. It appears when ask() is called, that is
    on the button of the learning bar; only when the table sets autoShow (level
    Neuling, §7.1) does the timer offer it after 1.5 s of thinking time.
*/
import QtQuick 1.1
import "."

Item {
    id: bubble

    // Set by the table; the single place that changes if LearnEngine becomes
    // its own context property.
    property variant learn: null
    property variant engine: null

    property color panelColor: "#f0202020"
    // The three hues Style does not carry, as in TarockTable: the recommended
    // move, the move that costs something, and the shade behind a dialog.
    property color goodColor: "#7fd67f"
    property color warnColor: "#ffc266"
    property color badColor: "#ff8a80"
    property real maxHeight: Style.itemSizeLarge * 2
    // Level Neuling only: offer the hint unasked after a moment (§7.4).
    property bool autoShow: false
    property bool showAlternatives: false

    property bool asked: false

    property variant hint: learn === null ? null : learn.hint
    property bool hasHint: hint !== null && hint !== undefined
                                    && (hint.label !== undefined || hint.text !== undefined)

    visible: asked && hasHint
    height: visible ? box.height : 0

    function value(name, fallback) {
        if (!bubble.hint || bubble.hint[name] === undefined || bubble.hint[name] === null)
            return fallback
        return bubble.hint[name]
    }

    function alternatives() {
        var list = bubble.value("alternatives", [])
        return list ? list : []
    }

    // best < 0.25 points behind, good < 1.0, playable < 3.0, else bad (§7.4).
    function qualityColor(quality) {
        if (quality === "best")
            return bubble.goodColor
        if (quality === "good")
            return Style.highlightColor
        if (quality === "bad")
            return bubble.badColor
        return Style.secondaryColor
    }

    function qualityText(quality) {
        if (quality === "best")
            return qsTr("best")
        if (quality === "good")
            return qsTr("good")
        if (quality === "bad")
            return qsTr("weak")
        return qsTr("playable")
    }

    function ask() {
        bubble.asked = true
        autoTimer.stop()
        if (bubble.learn !== null)
            bubble.learn.requestHint()
    }

    function dismiss() {
        bubble.asked = false
        autoTimer.stop()
    }

    // Play what the hint recommends, straight from the bubble.
    function follow() {
        var action = bubble.value("action", null)
        bubble.dismiss()
        if (!action || bubble.engine === null || !action.type)
            return
        bubble.engine.act(action.type, action.a === undefined ? -1 : action.a,
                          action.b === undefined ? -1 : action.b)
    }

    // The turn moved on, so the old hint is stale.
    onHintChanged: {
        if (bubble.autoShow && bubble.hasHint)
            autoTimer.restart()
    }

    Timer {
        id: autoTimer
        interval: 1500
        onTriggered: {
            if (bubble.autoShow)
                bubble.asked = true
        }
    }

    Rectangle {
        id: box
        anchors.left: parent.left
        anchors.right: parent.right
        height: column.height + 2 * Style.paddingSmall
        radius: Style.paddingSmall
        color: bubble.panelColor
        border.color: bubble.qualityColor(bubble.value("quality", ""))

        Flickable {
            anchors.fill: parent
            anchors.margins: Style.paddingSmall
            contentWidth: width
            contentHeight: column.height
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Column {
                id: column
                width: parent.width
                spacing: Style.paddingSmall / 2

                Row {
                    width: parent.width
                    spacing: Style.paddingSmall

                    Text {
                        width: parent.width - qualityLabel.width - Style.paddingSmall
                        elide: Text.ElideRight
                        font.pixelSize: Style.fontSizeExtraSmall
                        color: Style.highlightColor
                        text: "💡 " + bubble.value("label", bubble.value("short", ""))
                    }

                    Text {
                        id: qualityLabel
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: Style.fontSizeTiny
                        color: bubble.qualityColor(bubble.value("quality", ""))
                        text: bubble.qualityText(bubble.value("quality", ""))
                    }
                }

                Text {
                    width: parent.width
                    visible: text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.primaryColor
                    text: bubble.value("text", "")
                }

                // On Neuling the alternatives come with the hint (§7.1).
                Column {
                    width: parent.width
                    visible: bubble.showAlternatives && bubble.alternatives().length > 0
                    spacing: Style.paddingSmall / 2

                    Repeater {
                        model: bubble.showAlternatives ? bubble.alternatives() : []

                        Text {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            font.pixelSize: Style.fontSizeTiny
                            color: Style.secondaryColor
                            text: {
                                var head = modelData.label === undefined ? "" : modelData.label
                                var tail = modelData.text === undefined ? "" : modelData.text
                                return tail === "" ? head : head + " — " + tail
                            }
                        }
                    }
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Style.paddingMedium

                    TableButton {
                        visible: bubble.value("action", null) !== null
                        text: qsTr("Play it")
                        onClicked: bubble.follow()
                    }

                    TableButton {
                        text: qsTr("Close")
                        onClicked: bubble.dismiss()
                    }
                }
            }
        }
    }
}
