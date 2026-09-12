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

// A lesson in progress (docs/design.md §7.6). The same table as TablePage,
// only with the lesson band switched on; the page closes itself when the
// lesson is left, and "play on freely" simply keeps the table with
// lessonActive false.
Page {
    id: page
    objectName: "lessonPage"
    allowedOrientations: Orientation.Portrait

    property var engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn
    property bool freePlay: false

    function leave() {
        if (page.learn !== null)
            page.learn.stopLesson()
        pageStack.pop()
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: height

        PullDownMenu {
            MenuItem {
                text: qsTr("Play on freely")
                visible: page.learn !== null && page.learn.lessonActive
                onClicked: {
                    page.freePlay = true
                    page.learn.stopLesson()
                }
            }
            MenuItem {
                text: qsTr("Leave lesson")
                onClicked: page.leave()
            }
            MenuItem {
                text: qsTr("Rules")
                onClicked: pageStack.push(Qt.resolvedUrl("RulesPage.qml"))
            }
        }

        TarockTable {
            id: lessonTable
            anchors.fill: parent
            engine: page.engine
            learn: page.learn
            lessonMode: true
            onRuleRequested: pageStack.push(Qt.resolvedUrl("RulesPage.qml"),
                                            { "anchor": anchor })
            onGlossaryRequested: pageStack.push(Qt.resolvedUrl("GlossaryPage.qml"),
                                                { "term": term })
        }
    }

    // The lesson was stopped from inside the band; unless the learner asked to
    // play on, the page goes back to the course list.
    Connections {
        target: page.learn
        onLessonChanged: {
            if (page.learn !== null && !page.learn.lessonActive && !page.freePlay
                    && page.status === PageStatus.Active)
                pageStack.pop()
        }
    }
}
