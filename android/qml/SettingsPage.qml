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
import QtQuick
import QtQuick.Controls

// Settings of docs/design.md §6.1; the counterpart of
// sailfish/SettingsPage.qml. Deck and difficulty are engine properties and the
// engine saves them itself; profile and table size only take effect with the
// next new match, so they live in Prefs until startMatch() hands them over.
SubPage {
    id: page
    objectName: "settingsPage"
    title: qsTr("Settings")

    // Root-context engine instance, as on Sailfish OS.
    property var engine: tarockEngine
    readonly property var profileKeys: engine.profileKeys()
    readonly property var profileNames: page.namesFor(page.profileKeys)
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn

    // The five switches of docs/design.md §7.8 are properties of LearnEngine
    // and it saves them itself, in the same QSettings group as the level.

    Component.onCompleted: Prefs.adopt(page.engine)

    function namesFor(keys) {
        var names = []
        for (var i = 0; i < keys.length; ++i)
            names.push(page.engine.profileNameFor(keys[i]))
        return names
    }

    SectionLabel { text: qsTr("Rules") }

    TextBlock {
        text: qsTr("Rule profile")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }
    ComboBox {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        model: page.profileNames
        currentIndex: Math.max(0, page.profileKeys.indexOf(Prefs.profileKey))
        onActivated: (index) => Prefs.profileKey = page.profileKeys[index]
    }
    TextBlock {
        text: qsTr("Applies to the next new match")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    TextBlock {
        text: qsTr("Players")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }
    ComboBox {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        model: [qsTr("Four"), qsTr("Five")]
        currentIndex: Prefs.players === 5 ? 1 : 0
        onActivated: (index) => Prefs.players = index === 1 ? 5 : 4
    }
    TextBlock {
        text: qsTr("At five the dealer sits out the hand")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    SectionLabel { text: qsTr("Learning mode") }

    TextBlock {
        text: qsTr("Level")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }
    ComboBox {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        enabled: page.learn !== null
        model: [qsTr("Off"), qsTr("Learning"), qsTr("Novice")]
        currentIndex: page.learn === null ? 0 : page.learn.level
        onActivated: (index) => {
            if (page.learn !== null)
                page.learn.level = index
        }
    }
    TextBlock {
        text: qsTr("Takes effect at once, in a LAN match too")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    Switch {
        x: Theme.horizontalPageMargin
        text: qsTr("Show hints automatically")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.autoHint
        onToggled: {
            if (page.learn !== null)
                page.learn.autoHint = checked
        }
    }

    Switch {
        x: Theme.horizontalPageMargin
        text: qsTr("Warn before a bonus is lost")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.warnBonusLoss
        onToggled: {
            if (page.learn !== null)
                page.learn.warnBonusLoss = checked
        }
    }

    Switch {
        x: Theme.horizontalPageMargin
        text: qsTr("Dim illegal cards instead of hiding them")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.dimIllegal
        onToggled: {
            if (page.learn !== null)
                page.learn.dimIllegal = checked
        }
    }

    Switch {
        x: Theme.horizontalPageMargin
        text: qsTr("Counting help after every trick")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.countTutor
        onToggled: {
            if (page.learn !== null)
                page.learn.countTutor = checked
        }
    }

    Switch {
        x: Theme.horizontalPageMargin
        text: qsTr("Always play positive games out to the twelfth trick")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.playToEnd
        onToggled: {
            if (page.learn !== null)
                page.learn.playToEnd = checked
        }
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Lessons")
        onClicked: page.StackView.view.push(Qt.resolvedUrl("LearnPage.qml"))
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Rules and glossary")
        onClicked: page.StackView.view.push(Qt.resolvedUrl("RulesPage.qml"))
    }

    SectionLabel { text: qsTr("Opponents") }

    TextBlock {
        text: qsTr("Difficulty")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }
    ComboBox {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        model: [qsTr("Beginner"), qsTr("Club"), qsTr("Tournament")]
        currentIndex: page.engine.difficulty
        onActivated: (index) => page.engine.difficulty = index
    }

    SectionLabel { text: qsTr("Cards") }

    TextBlock {
        text: qsTr("Card deck")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }
    // Deck ids of assets/decks (docs/design.md §10). Everything that is not the
    // historical pack is the clear one, which is also the engine's fallback.
    ComboBox {
        id: deckBox
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        property var keys: page.engine.deckKeys()
        model: page.engine.deckNames()
        currentIndex: Math.max(0, keys.indexOf(page.engine.deck))
        onActivated: (index) => {
            if (index >= 0 && index < deckBox.keys.length)
                page.engine.deck = deckBox.keys[index]
        }
    }

    SectionLabel { text: qsTr("Animation") }

    Switch {
        x: Theme.horizontalPageMargin
        text: qsTr("Enable animations")
        checked: page.engine.animationsEnabled
        onToggled: page.engine.animationsEnabled = checked
    }
    TextBlock {
        text: qsTr("Turn off for immediate card movement")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    // animationSpeed is the per cent of the normal flight duration, so the
    // small numbers are the quick cards.
    TextBlock {
        text: qsTr("Card speed") + ": " + (speedSlider.value <= 75 ? qsTr("Fast")
                                           : speedSlider.value <= 125 ? qsTr("Normal") : qsTr("Slow"))
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }
    Slider {
        id: speedSlider
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        from: 50
        to: 200
        stepSize: 25
        value: page.engine.animationSpeed
        enabled: page.engine.animationsEnabled
        onMoved: page.engine.animationSpeed = Math.round(value)
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
