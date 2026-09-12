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

    "How do I stand?" — the running helpers of docs/design.md §7.5, read from
    learn.standing(): the card points of both parties so far, the tarock still
    out, the birds and honours that have fallen, the state of every open
    announcement, and the reminder which party the talon half that was left
    lying counts for (koenigrufen.md §11.5 no. 17, the most common mistake).

    The map is read through named keys where the engine offers them and through
    a generic `rows` list otherwise, so a profile may add its own lines without
    a change here.
*/
import QtQuick 2.6
import "."

Item {
    id: standing

    // Set by the table; the single place that changes if LearnEngine becomes
    // its own context property.
    property var learn: null
    property var engine: null

    property color panelColor: "#f0202020"
    property color shadeColor: "#99000000"
    property var report: ({})

    visible: false
    z: 800

    function refresh() {
        if (standing.learn === null || !standing.learn.standing) {
            standing.report = ({})
            return
        }
        var map = standing.learn.standing()
        standing.report = map ? map : ({})
    }

    function open() {
        standing.refresh()
        standing.visible = true
    }

    function close() {
        standing.visible = false
    }

    function has(name) {
        return standing.report && standing.report[name] !== undefined && standing.report[name] !== null
    }

    function joined(value) {
        if (value === undefined || value === null)
            return ""
        if (typeof value !== "object")
            return String(value)
        if (value.length !== undefined) {
            var parts = []
            for (var i = 0; i < value.length; ++i) {
                var item = value[i]
                if (item === null || item === undefined)
                    continue
                if (typeof item === "object")
                    parts.push(item.label !== undefined ? item.label : standing.joined(item))
                else
                    parts.push(String(item))
            }
            return parts.join(" · ")
        }
        return ""
    }

    // The lines of §7.5, in the order a beginner needs them.
    function lines() {
        var result = []
        var map = standing.report
        if (!map)
            return result
        if (standing.has("points")) {
            var points = map.points
            var declarer = points.declarer === undefined ? 0 : points.declarer
            var defenders = points.defenders === undefined ? 0 : points.defenders
            result.push({ "label": qsTr("Card points"),
                          "value": qsTr("%1 declarer · %2 defenders").arg(declarer).arg(defenders) })
        }
        if (standing.has("tarocksOut"))
            result.push({ "label": qsTr("Tarock still out"), "value": String(map.tarocksOut) })
        if (standing.has("tarocksBySeat"))
            result.push({ "label": qsTr("Estimated per seat"), "value": standing.joined(map.tarocksBySeat) })
        if (standing.has("fallenBirds"))
            result.push({ "label": qsTr("Birds fallen"), "value": standing.joined(map.fallenBirds) })
        if (standing.has("fallenHonours"))
            result.push({ "label": qsTr("Honours fallen"), "value": standing.joined(map.fallenHonours) })
        if (standing.has("kings"))
            result.push({ "label": qsTr("Kings fallen"), "value": standing.joined(map.kings) })
        if (standing.has("announcements"))
            result.push({ "label": qsTr("Announcements"), "value": standing.joined(map.announcements) })
        if (standing.has("talon"))
            result.push({ "label": qsTr("The talon half left lying"), "value": String(map.talon) })
        var rows = standing.has("rows") ? map.rows : []
        for (var i = 0; i < rows.length; ++i) {
            var row = rows[i]
            result.push({ "label": row.label === undefined ? "" : row.label,
                          "value": row.value === undefined ? "" : String(row.value) })
        }
        return result
    }

    MouseArea {
        anchors.fill: parent
        onClicked: standing.close()
    }

    Rectangle {
        anchors.fill: parent
        color: standing.shadeColor
    }

    Rectangle {
        anchors.centerIn: parent
        width: parent.width - 2 * Style.paddingMedium
        height: Math.min(column.height + 2 * Style.paddingMedium,
                         parent.height - 2 * Style.paddingMedium)
        radius: Style.paddingSmall
        color: standing.panelColor
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
                    font.pixelSize: Style.fontSizeSmall
                    color: Style.highlightColor
                    text: qsTr("How do I stand?")
                }

                Repeater {
                    model: standing.visible ? standing.lines() : []

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
                    visible: standing.lines().length === 0
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.secondaryColor
                    text: qsTr("Nothing to count yet.")
                }

                TableButton {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Close")
                    onClicked: standing.close()
                }
            }
        }
    }
}
