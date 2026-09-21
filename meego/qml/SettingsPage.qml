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
import QtQuick 1.1
import com.nokia.meego 1.0
// Settings of docs/design.md §6.1; the counterpart of
// sailfish/SettingsPage.qml. Deck and difficulty are engine properties and the
// engine saves them itself; profile and table size only take effect with the
// next new match, so they live in Prefs until startMatch() hands them over.
SubPage {
    id: page
    objectName: "settingsPage"
    title: qsTr("Settings")

    // Root-context engine instance, as on Sailfish OS.
    property variant engine: tarockEngine
    property variant profileKeys: engine.profileKeys()
    property variant profileNames: page.namesFor(page.profileKeys)
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property variant learn: engine.learn === undefined ? null : engine.learn

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
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }
    ComboBox {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        model: page.profileNames
        currentIndex: Math.max(0, page.profileKeys.indexOf(Prefs.profileKey))
        onActivated: Prefs.profileKey = page.profileKeys[index]
    }
    TextBlock {
        text: qsTr("Applies to the next new match")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }

    TextBlock {
        text: qsTr("Players")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }
    ComboBox {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        model: [qsTr("Four"), qsTr("Five")]
        currentIndex: Prefs.players === 5 ? 1 : 0
        onActivated: Prefs.players = index === 1 ? 5 : 4
    }
    TextBlock {
        text: qsTr("At five the dealer sits out the hand")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }

    SectionLabel { text: qsTr("Learning mode") }

    TextBlock {
        text: qsTr("Level")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }
    ComboBox {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        enabled: page.learn !== null
        model: [qsTr("Off"), qsTr("Learning"), qsTr("Novice")]
        currentIndex: page.learn === null ? 0 : page.learn.level
        onActivated: {
            if (page.learn !== null)
                page.learn.level = index
        }
    }
    TextBlock {
        text: qsTr("Takes effect at once, in a LAN match too")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }

    Switch {
        x: AppTheme.horizontalPageMargin
        text: qsTr("Show hints automatically")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.autoHint
        onToggled: {
            if (page.learn !== null)
                page.learn.autoHint = checked
        }
    }

    Switch {
        x: AppTheme.horizontalPageMargin
        text: qsTr("Warn before a bonus is lost")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.warnBonusLoss
        onToggled: {
            if (page.learn !== null)
                page.learn.warnBonusLoss = checked
        }
    }

    Switch {
        x: AppTheme.horizontalPageMargin
        text: qsTr("Dim illegal cards instead of hiding them")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.dimIllegal
        onToggled: {
            if (page.learn !== null)
                page.learn.dimIllegal = checked
        }
    }

    Switch {
        x: AppTheme.horizontalPageMargin
        text: qsTr("Counting help after every trick")
        enabled: page.learn !== null
        checked: page.learn !== null && page.learn.countTutor
        onToggled: {
            if (page.learn !== null)
                page.learn.countTutor = checked
        }
    }

    Switch {
        x: AppTheme.horizontalPageMargin
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
        onClicked: page.pageStack.push(Qt.resolvedUrl("LearnPage.qml"))
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Rules and glossary")
        onClicked: page.pageStack.push(Qt.resolvedUrl("RulesPage.qml"))
    }

    SectionLabel { text: qsTr("Opponents") }

    TextBlock {
        text: qsTr("Difficulty")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }
    ComboBox {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        model: [qsTr("Beginner"), qsTr("Club"), qsTr("Tournament")]
        currentIndex: page.engine.difficulty
        onActivated: page.engine.difficulty = index
    }

    SectionLabel { text: qsTr("Cards") }

    TextBlock {
        text: qsTr("Card deck")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }
    // Deck ids of assets/decks (docs/design.md §10). Everything that is not the
    // historical pack is the clear one, which is also the engine's fallback.
    ComboBox {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        model: [qsTr("Clear"), qsTr("Classic Vienna 1904")]
        currentIndex: page.engine.deck === "iug1904" ? 1 : 0
        onActivated: page.engine.deck = index === 1 ? "iug1904" : "clean54"
    }

    SectionLabel { text: qsTr("Animation") }

    Switch {
        x: AppTheme.horizontalPageMargin
        text: qsTr("Enable animations")
        checked: page.engine.animationsEnabled
        onToggled: page.engine.animationsEnabled = checked
    }
    TextBlock {
        text: qsTr("Turn off for immediate card movement")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }

    // animationSpeed is the per cent of the normal flight duration, so the
    // small numbers are the quick cards.
    TextBlock {
        text: qsTr("Card speed") + ": " + (speedSlider.value <= 75 ? qsTr("Fast")
                                           : speedSlider.value <= 125 ? qsTr("Normal") : qsTr("Slow"))
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }
    Slider {
        id: speedSlider
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
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
