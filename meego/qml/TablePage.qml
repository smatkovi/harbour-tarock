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
// The Android frame around the shared table; the counterpart of
// sailfish/TablePage.qml. The table itself is qml-common/TarockTable.qml.
Item {
    id: page
    anchors.fill: parent   // a MeeGo PageStack sizes only a Page
    objectName: "tablePage"

    property variant engine: tarockEngine
    // Set by the tutorial page (docs/design.md §7.9): the table comes up and
    // the tour starts on it instead of a hand being expected of the learner.
    property bool runTour: false

    Rectangle {
        anchors.fill: parent
        color: AppTheme.tableColor
    }

    TarockTable {
        id: tableView
        anchors.fill: parent
        engine: page.engine
        // "More on this" out of the learning panel, straight from the table.
        onRuleRequested: page.pageStack.push(Qt.resolvedUrl("RulesPage.qml"),
                                                              { "anchor": anchor })
        onGlossaryRequested: page.pageStack.push(Qt.resolvedUrl("GlossaryPage.qml"),
                                                                { "term": term })
    }

    ToolButton {
        id: menuButton
        anchors.top: parent.top
        anchors.right: parent.right
        text: "⋮"
        font.pixelSize: AppTheme.fontSizeLarge
        z: 2000
        onClicked: menu.popup(menuButton, 0, menuButton.height)
    }

    Menu {
        id: menu
        MenuItem {
            text: qsTr("Scores")
            onClicked: page.pageStack.push(Qt.resolvedUrl("ScorePage.qml"))
        }
        MenuItem {
            text: qsTr("Settings")
            onClicked: page.pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
        }
        MenuItem {
            text: qsTr("About")
            onClicked: page.pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
        }
    }

    // The finished hand is counted up on the score page, from where the next
    // one is dealt (docs/design.md §6.6).
    Connections {
        target: page.engine
        function onHandFinished() {
            // During the tour the table stays as it is; the hand is not being
            // played then.
            if (page.pageStack && page.pageStack.currentItem === page
                    && !tableView.tourActive)
                page.pageStack.push(Qt.resolvedUrl("ScorePage.qml"))
        }
    }

    // The tour waits for the page to be on top: before that the table has no
    // size yet and the frames would sit next to the parts they point at.
    Component.onCompleted: activatedTimer.start()
    Timer {
        id: activatedTimer
        interval: 400
        onTriggered: {
        if (page.runTour) {
            page.runTour = false
            tableView.startTour()
        }
    }
    }
}
