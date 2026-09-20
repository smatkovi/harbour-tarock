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

// The learning mode of docs/design.md §7; the counterpart of
// sailfish/LearnPage.qml: the level, the reference and the typical mistakes.
// The lessons are the course of TutorialPage.
SubPage {
    id: page
    objectName: "learnPage"
    title: qsTr("Learning mode")

    property var engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn

    property var mistakes: []
    property bool mistakesOpen: false

    // The lessons themselves are the course of TutorialPage (docs/design.md
    // §7.9), which lists them in the order of index.json together with the
    // tour and the progress; this page is about the layer over the normal
    // game: the level, the reference and the typical mistakes.
    function toggleMistakes() {
        if (!page.mistakesOpen && page.mistakes.length === 0 && page.learn !== null
                && page.learn.commonMistakes) {
            var list = page.learn.commonMistakes()
            page.mistakes = list ? list : []
        }
        page.mistakesOpen = !page.mistakesOpen
    }

    function mistakeText(entry) {
        if (typeof entry === "string")
            return entry
        if (entry.short && entry.text)
            return entry.short + " — " + entry.text
        return entry.short ? entry.short : (entry.text ? entry.text : "")
    }

    TextBlock {
        text: qsTr("The learning mode is a layer over the normal game: every match, a LAN match too, can have it switched on. The lessons themselves are in the tutorial, which keeps their order and remembers your progress.")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

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

    SectionLabel { text: qsTr("Lessons") }

    TextBlock {
        text: page.learn === null || page.learn.lessonCount === 0
              ? qsTr("No lessons are installed for this rule profile.")
              : qsTr("“Learn to play” takes the overview, the eight modules and the three practice hands in order and remembers where you are: %1 of %2 done.")
                .arg(page.learn.lessonsDone).arg(page.learn.lessonCount)
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Learn to play")
        onClicked: page.StackView.view.push(Qt.resolvedUrl("TutorialPage.qml"))
    }

    SectionLabel { text: qsTr("Reference") }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Rules")
        onClicked: page.StackView.view.push(Qt.resolvedUrl("RulesPage.qml"))
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Glossary")
        onClicked: page.StackView.view.push(Qt.resolvedUrl("GlossaryPage.qml"))
    }

    // The twenty resp. twelve beginner mistakes of §11.5 of both specs.
    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: page.mistakesOpen ? qsTr("Hide the typical mistakes")
                                : qsTr("Typical beginner mistakes")
        onClicked: page.toggleMistakes()
    }

    Repeater {
        model: page.mistakesOpen ? page.mistakes : []

        Label {
            x: Theme.horizontalPageMargin
            width: page.width - 2 * Theme.horizontalPageMargin
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontSizeExtraSmall
            color: Theme.secondaryColor
            text: "· " + page.mistakeText(modelData)
        }
    }
}
