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

// A lesson in progress (docs/design.md §7.6); the counterpart of
// sailfish/LessonPage.qml. The same table as TablePage, only with the lesson
// band switched on.
Item {
    id: page
    objectName: "lessonPage"

    property var engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn
    property bool freePlay: false

    function leave() {
        if (page.learn !== null)
            page.learn.stopLesson()
        page.StackView.view.pop()
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.tableColor
    }

    TarockTable {
        anchors.fill: parent
        engine: page.engine
        learn: page.learn
        lessonMode: true
        onRuleRequested: (anchor) => page.StackView.view.push(Qt.resolvedUrl("RulesPage.qml"),
                                                              { "anchor": anchor })
        onGlossaryRequested: (term) => page.StackView.view.push(Qt.resolvedUrl("GlossaryPage.qml"),
                                                                { "term": term })
    }

    ToolButton {
        id: menuButton
        anchors.top: parent.top
        anchors.right: parent.right
        text: "⋮"
        font.pixelSize: Theme.fontSizeLarge
        z: 2000
        onClicked: menu.popup(menuButton, 0, menuButton.height)
    }

    Menu {
        id: menu
        MenuItem {
            text: qsTr("Play on freely")
            enabled: page.learn !== null && page.learn.lessonActive
            onTriggered: {
                page.freePlay = true
                page.learn.stopLesson()
            }
        }
        MenuItem {
            text: qsTr("Leave lesson")
            onTriggered: page.leave()
        }
        MenuItem {
            text: qsTr("Rules")
            onTriggered: page.StackView.view.push(Qt.resolvedUrl("RulesPage.qml"))
        }
    }

    // The lesson was stopped from inside the band; unless the learner asked to
    // play on, the page goes back to the course list.
    Connections {
        target: page.learn
        function onLessonChanged() {
            if (page.learn !== null && !page.learn.lessonActive && !page.freePlay
                    && page.StackView.view && page.StackView.view.currentItem === page)
                page.StackView.view.pop()
        }
    }
}
