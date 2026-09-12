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

// The score page of docs/design.md §6.1: Schrift and Geld per player, the list
// of items of the last hand and the Radl counter. The sheet itself is shared
// with the Android build (qml-common/ScoreSheet.qml); this page is only the
// Silica frame around it and the button that deals the next hand.
Page {
    id: page
    objectName: "scorePage"
    allowedOrientations: Orientation.All

    property var engine: tarockEngine

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge
        VerticalScrollDecorator { }

        Column {
            id: content
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Scores")
                description: page.engine.active
                             ? qsTr("Hand %1 of %2").arg(page.engine.handNumber)
                                                    .arg(page.engine.handsPerMatch)
                             : qsTr("No match running")
            }

            // Shared with the Android build; the same sheet the table shows
            // over the felt when a hand is counted up.
            ScoreSheet {
                width: parent.width
                maxHeight: page.height
                engine: page.engine
                seats: page.engine.seats
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: page.engine.matchOver ? qsTr("New match") : qsTr("Next hand")
                visible: page.engine.handOver
                onClicked: {
                    if (page.engine.matchOver)
                        page.engine.newMatch()
                    else
                        page.engine.nextHand()
                    pageStack.pop()
                }
            }
        }
    }
}
