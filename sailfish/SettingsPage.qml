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
*/
import QtQuick 2.6
import Sailfish.Silica 1.0
import "."

// Settings of docs/design.md §6.1: rule profile, table size, difficulty, card
// deck and animation pace. Deck and difficulty are engine properties and the
// engine saves them itself; profile and table size only take effect with the
// next new match, so they live in Prefs until startMatch() hands them over.
//
// The engine is read from the root context rather than passed through the page
// stack, the same way harbour-snapszer does it.
Page {
    id: page
    objectName: "settingsPage"
    allowedOrientations: Orientation.All

    property var engine: tarockEngine
    readonly property var profileKeys: engine.profileKeys()

    Component.onCompleted: Prefs.adopt(page.engine)

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge
        VerticalScrollDecorator { }

        Column {
            id: content
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Settings")
                description: qsTr("Rules, table and cards")
            }

            SectionHeader { text: qsTr("Rules") }

            ComboBox {
                id: profileCombo
                width: parent.width
                label: qsTr("Rule profile")
                description: qsTr("Applies to the next new match")
                currentIndex: Math.max(0, page.profileKeys.indexOf(Prefs.profileKey))
                menu: ContextMenu {
                    Repeater {
                        model: page.profileKeys
                        MenuItem { text: page.engine.profileNameFor(modelData) }
                    }
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0 && currentIndex < page.profileKeys.length)
                        Prefs.profileKey = page.profileKeys[currentIndex]
                }
            }

            ComboBox {
                width: parent.width
                label: qsTr("Players")
                description: qsTr("At five the dealer sits out the hand")
                currentIndex: Prefs.players === 5 ? 1 : 0
                menu: ContextMenu {
                    MenuItem { text: qsTr("Four") }
                    MenuItem { text: qsTr("Five") }
                }
                onCurrentIndexChanged: Prefs.players = currentIndex === 1 ? 5 : 4
            }

            SectionHeader { text: qsTr("Opponents") }

            ComboBox {
                width: parent.width
                label: qsTr("Difficulty")
                currentIndex: page.engine.difficulty
                menu: ContextMenu {
                    MenuItem { text: qsTr("Beginner") }
                    MenuItem { text: qsTr("Club") }
                    MenuItem { text: qsTr("Tournament") }
                }
                onCurrentIndexChanged: page.engine.difficulty = currentIndex
            }

            SectionHeader { text: qsTr("Cards") }

            ComboBox {
                width: parent.width
                label: qsTr("Card deck")
                // Deck ids of assets/decks (docs/design.md §10). Everything
                // that is not the historical pack is the clear one, which is
                // also what the engine falls back to.
                currentIndex: page.engine.deck === "iug1904" ? 1 : 0
                menu: ContextMenu {
                    MenuItem { text: qsTr("Clear") }
                    MenuItem { text: qsTr("Classic Vienna 1904") }
                }
                onCurrentIndexChanged: page.engine.deck = currentIndex === 1 ? "iug1904" : "clean54"
            }

            SectionHeader { text: qsTr("Animation") }

            TextSwitch {
                width: parent.width
                text: qsTr("Enable animations")
                description: qsTr("Turn off for immediate card movement")
                checked: page.engine.animationsEnabled
                onCheckedChanged: page.engine.animationsEnabled = checked
            }

            // animationSpeed is the per cent of the normal flight duration, so
            // the small numbers are the quick cards.
            Slider {
                width: parent.width
                minimumValue: 50
                maximumValue: 200
                stepSize: 25
                value: page.engine.animationSpeed
                label: qsTr("Card speed")
                valueText: value <= 75 ? qsTr("Fast")
                           : value <= 125 ? qsTr("Normal") : qsTr("Slow")
                enabled: page.engine.animationsEnabled
                onValueChanged: page.engine.animationSpeed = Math.round(value)
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Reset to defaults")
                onClicked: {
                    if (page.profileKeys.length > 0)
                        Prefs.profileKey = page.profileKeys[0]
                    Prefs.players = 4
                    page.engine.difficulty = 1
                    page.engine.deck = "clean54"
                    page.engine.animationsEnabled = true
                    page.engine.animationSpeed = 100
                }
            }
        }
    }
}
