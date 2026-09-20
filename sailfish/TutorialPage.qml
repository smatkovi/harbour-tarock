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

// The tutorial of docs/design.md §7.9: the guided tour of the table and the
// course of §7.6 as one chain, in the order they are meant to be taken, with
// the progress LearnEngine keeps. Nothing is locked — an entry further down
// can be started at any time; the chain only says where to go on.
Page {
    id: page
    objectName: "tutorialPage"
    allowedOrientations: Orientation.All

    property var engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn

    // The tour is the first unit and lives in the QML, the lessons come from
    // the engine. Both notifying properties are read here, so the chain is
    // rebuilt as soon as one of them changes.
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
        // Exactly one entry is the one to go on with.
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
            // Half of what the tour points at belongs to the learning mode; a
            // learner who switched it off keeps that choice, everyone else
            // sees the panels the tour talks about.
            if (page.learn.level === 0)
                page.learn.level = 1
        }
        if (!page.engine.active) {
            if (page.engine.canResume)
                page.engine.resume()
            else
                page.engine.startMatch(Prefs.profileKey, Prefs.players)
        }
        pageStack.push(Qt.resolvedUrl("TablePage.qml"), { "runTour": true })
    }

    function startLesson(id) {
        if (page.learn === null)
            return
        page.learn.startLesson(id)
        pageStack.push(Qt.resolvedUrl("LessonPage.qml"))
    }

    function start(unit) {
        if (!unit)
            return
        if (unit.kind === "tour")
            page.startTour()
        else
            page.startLesson(unit.id)
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge
        VerticalScrollDecorator { }

        PullDownMenu {
            MenuItem {
                text: qsTr("Start over")
                onClicked: {
                    if (page.learn !== null)
                        page.learn.resetCourse()
                }
            }
        }

        Column {
            id: content
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Learn to play")
                description: page.engine.profileNameFor(Prefs.profileKey)
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("The way into the game: the goal, the run of a hand and the rules in one read-through, then the table itself, then the rules module by module and three hands played through step by step. Every unit can be repeated, and none has to be taken in this order.")
            }

            ProgressBar {
                width: parent.width
                minimumValue: 0
                maximumValue: Math.max(1, page.units.length)
                value: page.doneCount
                label: qsTr("%1 of %2 done").arg(page.doneCount).arg(page.units.length)
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: page.nextUnit === null ? qsTr("Everything done")
                                             : (page.doneCount === 0 ? qsTr("Start")
                                                                     : qsTr("Go on"))
                enabled: page.nextUnit !== null
                onClicked: page.start(page.nextUnit)
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                visible: page.nextUnit !== null
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
                text: page.nextUnit === null ? "" : page.nextUnit.title
            }

            SectionHeader { text: qsTr("The course") }

            Repeater {
                model: page.units

                ListItem {
                    width: parent.width
                    contentHeight: entry.height + 2 * Theme.paddingSmall
                    onClicked: page.start(modelData)

                    Row {
                        id: entry
                        x: Theme.horizontalPageMargin
                        width: parent.width - 2 * Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: Theme.paddingMedium

                        Label {
                            width: Theme.itemSizeExtraSmall / 2
                            anchors.verticalCenter: parent.verticalCenter
                            horizontalAlignment: Text.AlignHCenter
                            text: page.badge(modelData)
                            color: modelData.done ? Theme.highlightColor
                                                  : (modelData.current ? Theme.primaryColor
                                                                       : Theme.secondaryColor)
                        }

                        Column {
                            width: parent.width - Theme.itemSizeExtraSmall / 2 - Theme.paddingMedium

                            Label {
                                width: parent.width
                                wrapMode: Text.WordWrap
                                text: modelData.title
                                color: modelData.current ? Theme.highlightColor : Theme.primaryColor
                            }

                            Label {
                                width: parent.width
                                visible: text !== ""
                                wrapMode: Text.WordWrap
                                color: Theme.secondaryColor
                                font.pixelSize: Theme.fontSizeExtraSmall
                                text: page.subtitle(modelData)
                            }
                        }
                    }
                }
            }

            SectionHeader { text: qsTr("Reference") }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Rules")
                onClicked: pageStack.push(Qt.resolvedUrl("RulesPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Glossary")
                onClicked: pageStack.push(Qt.resolvedUrl("GlossaryPage.qml"))
            }
        }
    }
}
