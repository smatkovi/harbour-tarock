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

    The bidding band (docs/design.md §6.3): one chip per entry of
    tarockEngine.options, in the order the engine offers them, grouped by the
    option's `group` — "weiter" and "halten" stand apart from the games, the
    Hungarian jump bids apart again. The label is the spoken formula, so the
    player learns the table talk while he bids.

    A chip the engine marked as not reachable is not disabled but dimmed: a tap
    on it explains why instead of doing nothing (§7.3). With many games the row
    scrolls sideways; the band never grows taller than one row of chips.
*/
import QtQuick 1.1
import "."

Item {
    id: bar

    property variant engine: null
    // { type, a, b, label, speech, group, enabled, reasonKey, hint, hintText }
    property variant options: []
    // The formula the seat at turn spoke last, above the row.
    property string headline: ""
    property color panelColor: "#f0202020"
    // function(option) — set by the table, shows why an option is barred.
    signal explain(variant a1)
    property variant groups: []

    implicitHeight: column.height

    function regroup() {
        var result = []
        var seen = ({})
        for (var i = 0; i < options.length; ++i) {
            var option = options[i]
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

    function labelOf(option) {
        if (option.speech)
            return option.speech
        return option.label ? option.label : ""
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

    onOptionsChanged: bar.regroup()
    Component.onCompleted: bar.regroup()

    Column {
        id: column
        width: parent.width
        spacing: Style.paddingSmall

        Text {
            width: parent.width
            visible: bar.headline !== ""
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            font.pixelSize: Style.fontSizeTiny
            color: Style.secondaryHighlightColor
            text: bar.headline
        }

        Flickable {
            width: parent.width
            height: chipRow.height
            contentWidth: chipRow.width
            contentHeight: chipRow.height
            flickableDirection: Flickable.HorizontalFlick
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Row {
                id: chipRow
                spacing: Style.paddingMedium

                // QtQuick 2.6 has no padding on a positioner, so the margins
                // of the scrolling row are two empty items.
                Item {
                    width: Style.paddingMedium
                    height: 1
                }

                Repeater {
                    model: bar.groups

                    Column {
                        spacing: 0

                        Text {
                            visible: modelData.name !== ""
                            font.pixelSize: Style.fontSizeTiny
                            color: Style.secondaryColor
                            text: modelData.name
                        }

                        Row {
                            spacing: Style.paddingSmall

                            Repeater {
                                model: modelData.items

                                TableButton {
                                    text: bar.labelOf(modelData)
                                    opacity: modelData.enabled === false ? 0.4 : 1.0
                                    onClicked: bar.choose(modelData)
                                }
                            }
                        }
                    }
                }

                Item {
                    width: Style.paddingMedium
                    height: 1
                }
            }
        }
    }
}
