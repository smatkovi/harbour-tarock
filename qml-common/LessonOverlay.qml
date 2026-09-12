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

    A lesson in progress (docs/design.md §7.6). The band floats over the table
    and carries the text of the current step, the cards the step points at and
    the three buttons the learner needs: on, repeat the step, leave the lesson.

    Two signals of the LearnEngine land here: lessonStepPassed(text) confirms
    the expected decision, lessonTrap(reason) fires when the learner walks into
    one of the beginner mistakes the lesson anticipates. Both are connected by
    hand rather than through Connections, because the shared QML has to parse
    on Qt 5.6, where the typed handler syntax does not exist yet.

    Highlighted cards arrive either as ids or as the card keys of the lesson
    JSON ("XX", "H:K"); ids are drawn, keys are named.
*/
import QtQuick 2.6
import "."

Item {
    id: overlay

    // Set by the table; the single place that changes if LearnEngine becomes
    // its own context property.
    property var learn: null
    property var engine: null

    property color panelColor: "#f0202020"
    // The three hues Style does not carry, as in TarockTable: the recommended
    // move, the move that costs something, and the shade behind a dialog.
    property color goodColor: "#7fd67f"
    property color warnColor: "#ffc266"
    property real cardRatio: 1.78
    property string deck: "modern"
    property real maxHeight: Style.itemSizeLarge * 2.6

    // The reason of the trap that was just sprung, until it is acknowledged.
    property var trap: null
    property string passedText: ""
    property var attached: null

    readonly property var step: learn === null ? null : learn.lessonStep
    readonly property bool active: learn !== null && learn.lessonActive === true

    visible: active
    height: visible ? box.height : 0
    z: 700

    function value(name, fallback) {
        if (!overlay.step || overlay.step[name] === undefined || overlay.step[name] === null)
            return fallback
        return overlay.step[name]
    }

    function headline() {
        var title = overlay.value("title", "")
        var index = overlay.value("index", -1)
        var count = overlay.value("count", 0)
        if (index >= 0 && count > 0) {
            var position = qsTr("Step %1 of %2").arg(index + 1).arg(count)
            return title === "" ? position : position + " · " + title
        }
        return title === "" ? qsTr("Lesson") : title
    }

    // highlight is either a list or the { hand: [...] } map of the lesson JSON.
    function highlights() {
        var value = overlay.value("highlight", null)
        if (!value)
            return []
        if (value.length !== undefined)
            return value
        if (value.hand !== undefined && value.hand !== null)
            return value.hand
        return []
    }

    function cardText(entry) {
        if (typeof entry !== "number")
            return String(entry)
        if (overlay.engine === null || !overlay.engine.cardLabel)
            return ""
        return overlay.engine.cardLabel(entry)
    }

    function showTrap(reason) {
        overlay.trap = reason
        overlay.passedText = ""
    }

    function showPassed(text) {
        overlay.trap = null
        overlay.passedText = text === undefined ? "" : text
        passedTimer.restart()
    }

    function attach() {
        if (overlay.attached === overlay.learn)
            return
        overlay.detach()
        var target = overlay.learn
        if (target === null || target === undefined)
            return
        if (target.lessonTrap)
            target.lessonTrap.connect(overlay.showTrap)
        if (target.lessonStepPassed)
            target.lessonStepPassed.connect(overlay.showPassed)
        overlay.attached = target
    }

    function detach() {
        var target = overlay.attached
        overlay.attached = null
        if (target === null || target === undefined)
            return
        if (target.lessonTrap)
            target.lessonTrap.disconnect(overlay.showTrap)
        if (target.lessonStepPassed)
            target.lessonStepPassed.disconnect(overlay.showPassed)
    }

    function next() {
        overlay.trap = null
        if (overlay.learn !== null)
            overlay.learn.lessonNext()
    }

    function repeatStep() {
        overlay.trap = null
        if (overlay.learn !== null)
            overlay.learn.lessonRestartStep()
    }

    function leave() {
        overlay.trap = null
        if (overlay.learn !== null)
            overlay.learn.stopLesson()
    }

    onLearnChanged: overlay.attach()
    onStepChanged: overlay.trap = null
    Component.onCompleted: overlay.attach()
    Component.onDestruction: overlay.detach()

    Timer {
        id: passedTimer
        interval: 2500
        onTriggered: overlay.passedText = ""
    }

    Rectangle {
        id: box
        anchors.left: parent.left
        anchors.right: parent.right
        height: Math.min(column.height + 2 * Style.paddingSmall, overlay.maxHeight)
        radius: Style.paddingSmall
        color: overlay.panelColor
        border.color: overlay.trap ? overlay.warnColor : Style.highlightColor

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

                Text {
                    width: parent.width
                    elide: Text.ElideRight
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.secondaryColor
                    text: overlay.headline()
                }

                Text {
                    width: parent.width
                    visible: text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeExtraSmall
                    color: Style.primaryColor
                    text: overlay.value("text", "")
                }

                // The cards the step points at.
                Flow {
                    width: parent.width
                    spacing: Style.paddingSmall
                    visible: overlay.highlights().length > 0

                    Repeater {
                        model: overlay.visible ? overlay.highlights() : []

                        Item {
                            width: typeof modelData === "number"
                                   ? Style.itemSizeSmall * 0.5
                                   : keyLabel.implicitWidth + Style.paddingSmall
                            height: typeof modelData === "number"
                                    ? Style.itemSizeSmall * 0.5 * overlay.cardRatio
                                    : keyLabel.implicitHeight + Style.paddingSmall / 2

                            Card {
                                anchors.fill: parent
                                visible: typeof modelData === "number"
                                cardId: typeof modelData === "number" ? modelData : 0
                                deck: overlay.deck
                                faceUp: true
                            }

                            Rectangle {
                                anchors.fill: parent
                                radius: Style.paddingSmall / 2
                                color: "transparent"
                                border.width: 2
                                border.color: overlay.goodColor
                            }

                            Text {
                                id: keyLabel
                                anchors.centerIn: parent
                                visible: typeof modelData !== "number"
                                font.pixelSize: Style.fontSizeTiny
                                color: Style.highlightColor
                                text: typeof modelData === "number" ? "" : String(modelData)
                            }
                        }
                    }
                }

                // The expected beginner mistake, sprung.
                Column {
                    width: parent.width
                    visible: overlay.trap !== null
                    spacing: 0

                    Text {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        font.pixelSize: Style.fontSizeExtraSmall
                        color: overlay.warnColor
                        text: {
                            if (!overlay.trap)
                                return ""
                            if (overlay.trap.short)
                                return overlay.trap.short
                            return qsTr("Careful")
                        }
                    }

                    Text {
                        width: parent.width
                        visible: text !== ""
                        wrapMode: Text.WordWrap
                        font.pixelSize: Style.fontSizeTiny
                        color: Style.primaryColor
                        text: overlay.trap && overlay.trap.text ? overlay.trap.text : ""
                    }
                }

                Text {
                    width: parent.width
                    visible: overlay.passedText !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeTiny
                    color: overlay.goodColor
                    text: "✓ " + overlay.passedText
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Style.paddingSmall

                    TableButton {
                        text: qsTr("Next")
                        onClicked: overlay.next()
                    }

                    TableButton {
                        text: qsTr("Repeat step")
                        onClicked: overlay.repeatStep()
                    }

                    TableButton {
                        text: qsTr("Leave lesson")
                        onClicked: overlay.leave()
                    }
                }
            }
        }
    }
}
