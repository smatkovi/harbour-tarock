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

// The tutorial of docs/design.md §7.9; the counterpart of
// sailfish/TutorialPage.qml: the guided tour of the table and the course of
// §7.6 as one chain, with the progress LearnEngine keeps.
SubPage {
    id: page
    objectName: "tutorialPage"
    title: qsTr("Learn to play")

    property var engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn

    readonly property var units: page.buildChain(page.learn === null ? [] : page.learn.course,
                                                 page.learn !== null && page.learn.tourSeen === true)
    readonly property int doneCount: page.countDone(page.units)
    readonly property var nextUnit: page.firstOpen(page.units)

    function buildChain(course, tourSeen) {
        var tour = {
            "id": "tour",
            "kind": "tour",
            "module": "",
            "title": qsTr("First steps at the table"),
            "description": qsTr("A guided tour of the table: what every part of it does and how a card is played."),
            "done": tourSeen === true,
            "current": false
        }
        var result = []
        for (var i = 0; i < course.length; ++i) {
            var entry = course[i]
            // The overview of the game opens the chain; the tour of the table
            // follows it, because the question "what is this game" comes
            // before "where do I tap".
            if (i === 1 || (i === 0 && entry.kind !== "overview"))
                result.push(tour)
            result.push({
                "id": entry.id,
                "kind": entry.kind === undefined ? "module" : entry.kind,
                "module": entry.module === undefined ? "" : entry.module,
                "title": entry.title === undefined || entry.title === "" ? entry.id : entry.title,
                "description": entry.description === undefined ? "" : entry.description,
                "done": entry.done === true,
                "current": false
            })
        }
        if (result.length === course.length)
            result.push(tour)
        for (i = 0; i < result.length; ++i) {
            if (!result[i].done) {
                result[i].current = true
                break
            }
        }
        return result
    }

    function countDone(units) {
        var count = 0
        for (var i = 0; i < units.length; ++i) {
            if (units[i].done)
                count++
        }
        return count
    }

    function firstOpen(units) {
        for (var i = 0; i < units.length; ++i) {
            if (units[i].current)
                return units[i]
        }
        return null
    }

    function badge(unit) {
        if (unit.done)
            return "✓"
        return unit.current ? "▸" : "·"
    }

    function subtitle(unit) {
        var parts = []
        if (unit.kind === "tour")
            parts.push(qsTr("Tour"))
        else if (unit.kind === "overview")
            parts.push(qsTr("Overview"))
        else if (unit.kind === "practice")
            parts.push(qsTr("Practice hand"))
        else if (unit.module !== "")
            parts.push(unit.module)
        if (unit.description !== "")
            parts.push(unit.description)
        return parts.join("  ·  ")
    }

    // The tour runs over an ordinary table, so there has to be one: the match
    // that is already going on, the saved one, or a fresh one.
    function startTour() {
        if (page.learn !== null) {
            page.learn.tourSeen = true
            if (page.learn.level === 0)
                page.learn.level = 1
        }
        if (!page.engine.active) {
            if (page.engine.canResume)
                page.engine.resume()
            else
                page.engine.startMatch(Prefs.profileKey, Prefs.players)
        }
        page.StackView.view.push(Qt.resolvedUrl("TablePage.qml"), { "runTour": true })
    }

    function startLesson(id) {
        if (page.learn === null)
            return
        page.learn.startLesson(id)
        page.StackView.view.push(Qt.resolvedUrl("LessonPage.qml"))
    }

    function start(unit) {
        if (!unit)
            return
        if (unit.kind === "tour")
            page.startTour()
        else
            page.startLesson(unit.id)
    }

    TextBlock {
        text: qsTr("The way into the game: the goal, the run of a hand and the rules in one read-through, then the table itself, then the rules module by module and three hands played through step by step. Every unit can be repeated, and none has to be taken in this order.")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    ProgressBar {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        from: 0
        to: Math.max(1, page.units.length)
        value: page.doneCount
    }

    TextBlock {
        text: qsTr("%1 of %2 done").arg(page.doneCount).arg(page.units.length)
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: page.nextUnit === null ? qsTr("Everything done")
                                     : (page.doneCount === 0 ? qsTr("Start") : qsTr("Go on"))
        enabled: page.nextUnit !== null
        onClicked: page.start(page.nextUnit)
    }

    TextBlock {
        visible: page.nextUnit !== null
        horizontalAlignment: Text.AlignHCenter
        text: page.nextUnit === null ? "" : page.nextUnit.title
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    SectionLabel { text: qsTr("The course") }

    Repeater {
        model: page.units

        ItemDelegate {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            onClicked: page.start(modelData)

            contentItem: Row {
                spacing: Theme.paddingMedium

                Label {
                    width: Theme.fontSizeLarge
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: Theme.fontSizeSmall
                    text: page.badge(modelData)
                    color: modelData.done ? Theme.highlightColor
                                          : (modelData.current ? Theme.primaryColor
                                                               : Theme.secondaryColor)
                }

                Column {
                    width: parent.width - Theme.fontSizeLarge - Theme.paddingMedium

                    Label {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        font.pixelSize: Theme.fontSizeSmall
                        text: modelData.title
                        color: modelData.current ? Theme.highlightColor : Theme.primaryColor
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

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Start over")
        onClicked: {
            if (page.learn !== null)
                page.learn.resetCourse()
        }
    }
}
