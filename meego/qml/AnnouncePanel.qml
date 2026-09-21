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

    The announcements (docs/design.md §6.5). Königrufen has eight bonuses, the
    Hungarian profile up to seventeen figures, so the chips are laid out as a
    Flow grouped by the engine's `group` — possession, course of play, target —
    and the panel scrolls once the groups outgrow the space the table gives it.

    A figure the rules bar stays visible and dimmed (that is the lesson) and
    says on a tap why it is barred. Below the figures stands one row per open
    Posten with its Kontra, and at the bottom "Ich liege", which closes the
    announcement round for this seat.
*/
import QtQuick 1.1
import "."

Item {
    id: panel

    property variant engine: null
    // All three lists carry the option map of docs/design.md §5:
    // { type, a, b, label, speech, group, enabled, reasonKey, hint, hintText }
    property variant bonusOptions: []
    property variant kontraTargets: []
    property variant options: []
    // What this seat has announced so far (seats[0].announcements).
    property variant announcements: []
    property real maxHeight: Style.itemSizeLarge * 3
    property color panelColor: "#f0202020"
    // function(option) — set by the table, shows why an option is barred.
    signal explain(variant a1)
    property variant groups: []

    implicitHeight: Math.min(column.height, maxHeight)

    function regroup() {
        var result = []
        var seen = ({})
        for (var i = 0; i < bonusOptions.length; ++i) {
            var option = bonusOptions[i]
            var name = option.group === undefined || option.group === null ? "" : String(option.group)
            var key = "g:" + name
            if (seen[key] === undefined) {
                seen[key] = result.length
                result.push({ "name": name, "items": [] })
            }
            result[seen[key]].items.push(option)
        }
        groups = result
    }

    // Everything the phase allows besides the bonuses, the kontras and the
    // "ready" that has its own button.
    function extras() {
        var result = []
        for (var i = 0; i < options.length; ++i) {
            var type = options[i].type
            if (type !== "bonus" && type !== "kontra" && type !== "ready")
                result.push(options[i])
        }
        return result
    }

    function readyOption() {
        for (var i = 0; i < options.length; ++i) {
            if (options[i].type === "ready")
                return options[i]
        }
        return null
    }

    function readyLabel() {
        var option = panel.readyOption()
        if (option && option.speech)
            return option.speech
        if (option && option.label)
            return option.label
        return qsTr("I am ready")
    }

    function labelOf(option) {
        if (option.speech)
            return option.speech
        return option.label ? option.label : ""
    }

    function myAnnouncementText() {
        if (!announcements || !announcements.length)
            return ""
        var parts = []
        for (var i = 0; i < announcements.length; ++i) {
            var item = announcements[i]
            if (typeof item === "string")
                parts.push(item)
            else if (item.label)
                parts.push(item.label)
            else if (item.speech)
                parts.push(item.speech)
        }
        return qsTr("You announced: %1").arg(parts.join(" · "))
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

    onBonusOptionsChanged: panel.regroup()
    Component.onCompleted: panel.regroup()

    Flickable {
        anchors.fill: parent
        contentWidth: width
        contentHeight: column.height
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        Column {
            id: column
            width: parent.width
            spacing: Style.paddingSmall / 2

            Repeater {
                model: panel.groups

                Column {
                    width: column.width
                    spacing: 0

                    Text {
                        visible: modelData.name !== ""
                        x: Style.paddingMedium
                        font.pixelSize: Style.fontSizeTiny
                        color: Style.secondaryColor
                        text: modelData.name
                    }

                    Flow {
                        width: parent.width - 2 * Style.paddingMedium
                        x: Style.paddingMedium
                        spacing: Style.paddingSmall

                        Repeater {
                            model: modelData.items

                            TableButton {
                                text: panel.labelOf(modelData)
                                opacity: modelData.enabled === false ? 0.4 : 1.0
                                onClicked: panel.choose(modelData)
                            }
                        }
                    }
                }
            }

            // One row per Posten that may still be doubled (§6.5, KontraRow).
            Repeater {
                model: panel.kontraTargets

                Row {
                    x: Style.paddingMedium
                    spacing: Style.paddingMedium

                    TableButton {
                        text: panel.labelOf(modelData)
                        opacity: modelData.enabled === false ? 0.4 : 1.0
                        onClicked: panel.choose(modelData)
                    }
                }
            }

            Flow {
                width: parent.width - 2 * Style.paddingMedium
                x: Style.paddingMedium
                spacing: Style.paddingSmall

                Repeater {
                    model: panel.extras()

                    TableButton {
                        text: panel.labelOf(modelData)
                        opacity: modelData.enabled === false ? 0.4 : 1.0
                        onClicked: panel.choose(modelData)
                    }
                }
            }

            Text {
                width: parent.width
                visible: text !== ""
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                font.pixelSize: Style.fontSizeTiny
                color: Style.secondaryHighlightColor
                text: panel.myAnnouncementText()
            }

            TableButton {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: panel.readyOption() !== null
                text: panel.readyLabel()
                onClicked: {
                    if (panel.engine !== null)
                        panel.engine.act("ready")
                }
            }
        }
    }
}
