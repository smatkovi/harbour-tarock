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

// The Sailfish frame around the shared table: orientation, pull-down menu and
// the page stack. Everything inside the table itself lives in
// qml-common/TarockTable.qml and is the same on Android.
Page {
    id: page
    objectName: "tablePage"
    allowedOrientations: Orientation.Portrait

    property var engine: tarockEngine
    // Set by the tutorial page (docs/design.md §7.9): the table comes up and
    // the tour starts on it instead of a hand being expected of the learner.
    property bool runTour: false

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: height

        PullDownMenu {
            MenuItem {
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }
            MenuItem {
                text: qsTr("Scores")
                onClicked: pageStack.push(Qt.resolvedUrl("ScorePage.qml"))
            }
        }

        TarockTable {
            id: tableView
            anchors.fill: parent
            engine: page.engine
            // "More on this" out of the learning panel, straight from the table.
            onRuleRequested: pageStack.push(Qt.resolvedUrl("RulesPage.qml"),
                                            { "anchor": anchor })
            onGlossaryRequested: pageStack.push(Qt.resolvedUrl("GlossaryPage.qml"),
                                                { "term": term })
        }
    }

    // The tour waits for the page to be on top: before that the table has no
    // size yet and the frames would sit next to the parts they point at.
    onStatusChanged: {
        if (page.status === PageStatus.Active && page.runTour) {
            page.runTour = false
            tableView.startTour()
        }
    }

    // The finished hand is counted up on the score page, from where the next
    // one is dealt (docs/design.md §6.6). During the tour the table stays as
    // it is; the hand is not being played then.
    Connections {
        target: page.engine
        onHandFinished: {
            if (page.status === PageStatus.Active && !tableView.tourActive)
                pageStack.push(Qt.resolvedUrl("ScorePage.qml"))
        }
    }
}
