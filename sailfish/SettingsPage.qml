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
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn

    // The five switches of docs/design.md §7.8 are properties of LearnEngine
    // and it saves them itself, in the same QSettings group as the level.

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
                id: playersCombo
                width: parent.width
                label: qsTr("Players")
                // Wie viele mitspielen können, sagt das Regelprofil:
                // Königrufen vier oder fünf, das ungarische Blatt vier,
                // Tapp-Tarock drei, Strohmandeln zwei.
                property var seats: page.engine.seatOptionsFor(Prefs.profileKey)
                description: seats.length === 1
                             ? qsTr("These rules are played by %1").arg(seats[0])
                             : qsTr("At five the dealer sits out the hand")
                enabled: seats.length > 1
                currentIndex: Math.max(0, seats.indexOf(Prefs.players))
                menu: ContextMenu {
                    Repeater {
                        model: playersCombo.seats
                        MenuItem { text: String(modelData) }
                    }
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0 && currentIndex < seats.length)
                        Prefs.players = seats[currentIndex]
                }
                onSeatsChanged: {
                    // Ein Profilwechsel darf keine Spielerzahl stehen lassen,
                    // die es dort nicht gibt.
                    if (seats.indexOf(Prefs.players) < 0 && seats.length > 0)
                        Prefs.players = seats[0]
                }
            }

            SectionHeader { text: qsTr("Learning mode") }

            ComboBox {
                width: parent.width
                label: qsTr("Level")
                description: qsTr("Takes effect at once, in a LAN match too")
                enabled: page.learn !== null
                currentIndex: page.learn === null ? 0 : page.learn.level
                menu: ContextMenu {
                    MenuItem { text: qsTr("Off") }
                    MenuItem { text: qsTr("Learning") }
                    MenuItem { text: qsTr("Novice") }
                }
                onCurrentIndexChanged: {
                    if (page.learn !== null && page.learn.level !== currentIndex)
                        page.learn.level = currentIndex
                }
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Show hints automatically")
                checked: page.learn !== null && page.learn.autoHint
                enabled: page.learn !== null
                onClicked: {
                    if (page.learn !== null)
                        page.learn.autoHint = checked
                }
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Warn before a bonus is lost")
                checked: page.learn !== null && page.learn.warnBonusLoss
                enabled: page.learn !== null
                onClicked: {
                    if (page.learn !== null)
                        page.learn.warnBonusLoss = checked
                }
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Dim illegal cards instead of hiding them")
                checked: page.learn !== null && page.learn.dimIllegal
                enabled: page.learn !== null
                onClicked: {
                    if (page.learn !== null)
                        page.learn.dimIllegal = checked
                }
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Counting help after every trick")
                checked: page.learn !== null && page.learn.countTutor
                enabled: page.learn !== null
                onClicked: {
                    if (page.learn !== null)
                        page.learn.countTutor = checked
                }
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Always play positive games out to the twelfth trick")
                checked: page.learn !== null && page.learn.playToEnd
                enabled: page.learn !== null
                onClicked: {
                    if (page.learn !== null)
                        page.learn.playToEnd = checked
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Lessons")
                onClicked: pageStack.push(Qt.resolvedUrl("LearnPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Rules and glossary")
                onClicked: pageStack.push(Qt.resolvedUrl("RulesPage.qml"))
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
                id: deckBox
                width: parent.width
                label: qsTr("Card deck")
                // The decks the engine actually has (docs/design.md §10); a
                // pack that is not installed does not show up here.
                property var keys: page.engine.deckKeys()
                currentIndex: Math.max(0, keys.indexOf(page.engine.deck))
                menu: ContextMenu {
                    Repeater {
                        model: page.engine.deckNames()
                        MenuItem { text: modelData }
                    }
                }
                onCurrentIndexChanged: {
                    if (currentIndex >= 0 && currentIndex < keys.length)
                        page.engine.deck = keys[currentIndex]
                }
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
                    page.engine.deck = "iug1904"
                    page.engine.animationsEnabled = true
                    page.engine.animationSpeed = 100
                }
            }
        }
    }
}
