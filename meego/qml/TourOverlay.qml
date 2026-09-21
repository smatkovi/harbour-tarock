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

    The guided tour of docs/design.md §7.9: it dims the table, cuts a hole
    around the part the current step talks about and puts a bubble next to it.
    The script is qml-common/TourScript.qml; the table resolves the target name
    of a step to an item through `resolve`.

    While the tour runs it swallows every tap on the table, so a step cannot be
    answered by accident; a tap on the dimmed area is the same as "Next". The
    hole follows the item it points at: the geometry is read inside the binding,
    so a rotation or a panel that opens moves the frame with it.

    Kept to QtQuick 2.6 and ES5 like the rest of qml-common, and it uses only
    Style and TableButton, which both platforms provide.
*/
import QtQuick 1.1
import "."

Item {
    id: tour

    // [{ target, title, text }] — normally TourScript.steps.
    property variant steps: []
    // function(name) -> Item, set by the table.
    property variant resolve: null

    property color panelColor: "#f0202020"
    property color dimColor: "#c0000000"
    property real maxHeight: Style.itemSizeLarge * 3

    property int index: 0
    property bool running: false
    // Set while the tour is being taken down, so the step binding cannot ask
    // for an item that is already gone.
    property Item targetItem: null

    signal finished()

    property int count: steps ? steps.length : 0
    property variant step: (running && index >= 0 && index < count) ? steps[index] : null
    property bool lastStep: index >= count - 1
    property real pad: Style.paddingSmall

    // The rectangle that stays bright. An empty one dims the whole table,
    // which is what the steps without a target want.
    property variant hole: (function() {
        var item = tour.targetItem
        if (item === null || !item.visible || item.width <= 0 || item.height <= 0)
            return Qt.rect(0, 0, 0, 0)
        var point = item.mapToItem(tour, 0, 0)
        var left = Math.max(0, point.x - tour.pad)
        var top = Math.max(0, point.y - tour.pad)
        var right = Math.min(tour.width, point.x + item.width + tour.pad)
        var bottom = Math.min(tour.height, point.y + item.height + tour.pad)
        if (right <= left || bottom <= top)
            return Qt.rect(0, 0, 0, 0)
        return Qt.rect(left, top, right - left, bottom - top)
    })()

    property bool hasHole: hole.width > 0 && hole.height > 0

    visible: running
    z: 950

    function value(name, fallback) {
        if (!tour.step || tour.step[name] === undefined || tour.step[name] === null)
            return fallback
        return tour.step[name]
    }

    function updateTarget() {
        var name = tour.value("target", "")
        if (!tour.running || name === "" || tour.resolve === null
                || tour.resolve === undefined) {
            tour.targetItem = null
            return
        }
        var item = tour.resolve(name)
        tour.targetItem = (item === undefined || item === null) ? null : item
    }

    function start() {
        if (tour.count === 0)
            return
        tour.index = 0
        tour.running = true
        tour.updateTarget()
    }

    function next() {
        if (tour.lastStep) {
            tour.stop()
            return
        }
        tour.index = tour.index + 1
        tour.updateTarget()
    }

    function back() {
        if (tour.index <= 0)
            return
        tour.index = tour.index - 1
        tour.updateTarget()
    }

    function stop() {
        if (!tour.running)
            return
        tour.running = false
        tour.targetItem = null
        tour.index = 0
        tour.finished()
    }

    onStepChanged: tour.updateTarget()

    // --- the four shades around the hole ---------------------------------------

    Rectangle {
        color: tour.dimColor
        x: 0
        y: 0
        width: tour.width
        height: tour.hole.y
    }

    Rectangle {
        color: tour.dimColor
        x: 0
        y: tour.hole.y + tour.hole.height
        width: tour.width
        height: Math.max(0, tour.height - y)
    }

    Rectangle {
        color: tour.dimColor
        x: 0
        y: tour.hole.y
        width: tour.hole.x
        height: tour.hole.height
    }

    Rectangle {
        color: tour.dimColor
        x: tour.hole.x + tour.hole.width
        y: tour.hole.y
        width: Math.max(0, tour.width - x)
        height: tour.hole.height
    }

    // The frame that names the part the step is about.
    Rectangle {
        visible: tour.hasHole
        x: tour.hole.x
        y: tour.hole.y
        width: tour.hole.width
        height: tour.hole.height
        color: "transparent"
        radius: Style.paddingSmall
        border.width: 2
        border.color: Style.highlightColor
    }

    // Nothing on the table may be touched while the tour runs; a tap anywhere
    // moves one step on, like the button does.
    MouseArea {
        anchors.fill: parent
        enabled: tour.running
        onClicked: tour.next()
    }

    // --- the bubble --------------------------------------------------------------

    Rectangle {
        id: bubble
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Style.horizontalPageMargin
        anchors.rightMargin: Style.horizontalPageMargin
        height: Math.min(column.implicitHeight + 2 * Style.paddingMedium, tour.maxHeight)
        radius: Style.paddingSmall
        color: tour.panelColor
        border.color: Style.highlightColor

        // Below the highlighted part where there is room for it, above it
        // otherwise, and in the middle of the table when the step has no
        // target at all.
        y: {
            if (!tour.hasHole)
                return Math.max(0, (tour.height - height) / 2)
            var below = tour.hole.y + tour.hole.height + Style.paddingMedium
            if (below + height + Style.paddingMedium <= tour.height)
                return below
            var above = tour.hole.y - Style.paddingMedium - height
            if (above >= 0)
                return above
            return Math.max(0, (tour.height - height) / 2)
        }

        Flickable {
            anchors.fill: parent
            anchors.margins: Style.paddingMedium
            contentWidth: width
            contentHeight: column.implicitHeight
            clip: true
            flickableDirection: Flickable.VerticalFlick

            Column {
                id: column
                width: parent.width
                spacing: Style.paddingSmall

                Text {
                    width: parent.width
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.secondaryColor
                    text: qsTr("Tour %1 of %2").arg(tour.index + 1).arg(tour.count)
                }

                Text {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeSmall
                    color: Style.highlightColor
                    text: tour.value("title", "")
                }

                Text {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeExtraSmall
                    color: Style.primaryColor
                    text: tour.value("text", "")
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Style.paddingSmall

                    TableButton {
                        text: qsTr("Back")
                        enabled: tour.index > 0
                        onClicked: tour.back()
                    }

                    TableButton {
                        text: tour.lastStep ? qsTr("Done") : qsTr("Next")
                        onClicked: tour.next()
                    }

                    TableButton {
                        visible: !tour.lastStep
                        text: qsTr("Skip")
                        onClicked: tour.stop()
                    }
                }
            }
        }
    }
}
