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

// The course list of docs/design.md §7.6; the counterpart of
// sailfish/LearnPage.qml. The index comes from the engine, which reads
// lessons/<profile>/*.json; the page only lays it out.
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

    readonly property var lessons: page.lessonIndex()

    // LearnEngine::lessonIds() lists the lesson files of the active profile;
    // lessonInfo(id) would add title, module and goals, so until it exists an
    // entry carries what the file name says.
    function lessonIndex() {
        if (page.learn === null || typeof page.learn.lessonIds !== "function")
            return []
        var result = []
        var ids = page.learn.lessonIds()
        for (var i = 0; i < ids.length; ++i) {
            if (typeof page.learn.lessonInfo === "function") {
                var info = page.learn.lessonInfo(ids[i])
                if (info && info.id) {
                    result.push(info)
                    continue
                }
            }
            result.push({ "id": ids[i], "title": ids[i] })
        }
        return result
    }

    function subtitle(entry) {
        var parts = []
        if (entry.module)
            parts.push(entry.module)
        if (entry.goals && entry.goals.length > 0)
            parts.push(entry.goals.join(" · "))
        return parts.join("  ·  ")
    }

    function start(entry) {
        if (page.learn === null || !entry.id)
            return
        page.learn.startLesson(entry.id)
        page.StackView.view.push(Qt.resolvedUrl("LessonPage.qml"))
    }

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
        text: qsTr("The learning mode is a layer over the normal game: every match, a LAN match too, can have it switched on. The lessons below deal a fixed hand and take you through it step by step.")
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
        visible: page.lessons.length === 0
        text: qsTr("No lessons are installed for this rule profile.")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    Repeater {
        model: page.lessons

        ItemDelegate {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            onClicked: page.start(modelData)

            contentItem: Column {
                Label {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeSmall
                    text: modelData.title ? modelData.title : modelData.id
                }
                Label {
                    width: parent.width
                    visible: text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                    text: page.subtitle(modelData)
                }
            }
        }
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
