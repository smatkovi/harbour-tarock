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
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        model: [qsTr("Clear"), qsTr("Classic Vienna 1904")]
        currentIndex: page.engine.deck === "iug1904" ? 1 : 0
        onActivated: (index) => page.engine.deck = index === 1 ? "iug1904" : "clean54"
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
            page.engine.deck = "clean54"
            page.engine.animationsEnabled = true
            page.engine.animationSpeed = 100
        }
    }
}
