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

    The contextual explanation bar of docs/design.md §7.2. It sits under the
    header and answers three questions at every decision: where am I (phase and
    position in the chain), what happens here, what should I do now.

    Collapsed it is one line, so the table keeps its room; open it scrolls
    inside its own bounds and never pushes the seats off the screen. The open
    state belongs to the engine (learn.panelOpen), so it survives the page.

    Technical terms come with the explanation (terms: [{term, definition,
    anchor}]) and are offered as chips rather than as rich-text links: the
    shared QML is QtQuick 2.6, where a StyledText link needs markup the C++
    side would have to produce for both platforms.
*/
import QtQuick 1.1
import "."

Item {
    id: panel

    // The one line that changes if LearnEngine arrives as its own context
    // property rather than as tarockEngine.learn — the table passes it in.
    property variant learn: null
    property variant engine: null

    property real maxHeight: Style.itemSizeLarge * 2.2
    property color panelColor: "#f0202020"

    // function(anchor) / function(term) — set by the table, which forwards
    // both into the rule reference. Function properties rather than signals:
    // injected handler parameters are deprecated on Qt 6 and the typed handler
    // syntax does not exist on Qt 5.6.
    signal openAnchor(variant a1)
    signal openTerm(variant a1)
    property int level: learn === null || learn.level === undefined ? 0 : learn.level
    property variant explanation: learn === null ? null : learn.explanation
    property bool expanded: learn !== null && learn.panelOpen === true

    visible: level > 0
    height: visible ? column.height + Style.paddingSmall : 0

    function value(name, fallback) {
        var map = panel.explanation
        if (!map || map[name] === undefined || map[name] === null || map[name] === "")
            return fallback
        return map[name]
    }

    function terms() {
        var list = panel.value("terms", [])
        return list ? list : []
    }

    // "3 von 7 · König rufen" — the phase and where it sits in the chain.
    function whereText() {
        var title = panel.value("phaseTitle", panel.engine === null ? "" : panel.engine.phaseName)
        var count = panel.value("stepCount", 0)
        var index = panel.value("stepIndex", 0)
        if (count > 0)
            return qsTr("%1 of %2 · %3").arg(index + 1).arg(count).arg(title)
        return title
    }

    function toggle() {
        if (panel.learn !== null)
            panel.learn.panelOpen = !panel.expanded
    }

    Rectangle {
        anchors.fill: parent
        visible: panel.visible
        color: panel.panelColor
    }

    Column {
        id: column
        anchors.top: parent.top
        anchors.topMargin: Style.paddingSmall / 2
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Style.horizontalPageMargin
        anchors.rightMargin: Style.horizontalPageMargin
        spacing: Style.paddingSmall / 2

        // Level 1: where am I. Always visible, and the handle of the bar.
        Item {
            width: parent.width
            height: Math.max(whereLabel.implicitHeight, chevron.implicitHeight)

            Text {
                id: whereLabel
                anchors.left: parent.left
                anchors.right: chevron.left
                anchors.rightMargin: Style.paddingSmall
                anchors.verticalCenter: parent.verticalCenter
                elide: Text.ElideRight
                font.pixelSize: Style.fontSizeExtraSmall
                color: Style.highlightColor
                text: panel.whereText()
            }

            Text {
                id: chevron
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: Style.fontSizeExtraSmall
                color: Style.secondaryColor
                text: panel.expanded ? "▴" : "▾"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: panel.toggle()
            }
        }

        // Levels 2 and 3: what happens here, what to do now. The two texts run
        // long, so they scroll inside the bar instead of growing it.
        Flickable {
            width: parent.width
            height: Math.min(body.height, panel.maxHeight)
            visible: panel.expanded
            contentWidth: width
            contentHeight: body.height
            boundsBehavior: Flickable.StopAtBounds
            clip: true

            Column {
                id: body
                width: parent.width
                spacing: Style.paddingSmall / 2

                Text {
                    width: parent.width
                    visible: text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.primaryColor
                    text: panel.value("whatHappens", "")
                }

                Text {
                    width: parent.width
                    visible: text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Style.fontSizeTiny
                    color: Style.secondaryHighlightColor
                    text: panel.value("whatToDo", "")
                }

                // Every technical term in the text is one tap from the glossary.
                Flow {
                    width: parent.width
                    spacing: Style.paddingSmall / 2

                    Repeater {
                        model: panel.terms()

                        Rectangle {
                            width: termLabel.implicitWidth + Style.paddingSmall
                            height: termLabel.implicitHeight + Style.paddingSmall / 2
                            radius: height / 2
                            color: "transparent"
                            border.width: 1
                            border.color: Style.secondaryColor

                            Text {
                                id: termLabel
                                anchors.centerIn: parent
                                font.pixelSize: Style.fontSizeTiny
                                color: Style.secondaryColor
                                text: modelData.term === undefined ? String(modelData) : modelData.term
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (panel.openTerm)
                                        panel.openTerm(termLabel.text)
                                }
                            }
                        }
                    }
                }

                Text {
                    width: parent.width
                    visible: panel.value("anchor", "") !== ""
                    font.pixelSize: Style.fontSizeTiny
                    font.underline: true
                    color: Style.highlightColor
                    text: qsTr("More on this")

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (panel.openAnchor)
                                panel.openAnchor(panel.value("anchor", ""))
                        }
                    }
                }
            }
        }
    }
}
