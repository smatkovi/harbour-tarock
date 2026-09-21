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
// The score page of docs/design.md §6.1; the counterpart of
// sailfish/ScorePage.qml. The sheet itself is shared with the Sailfish OS
// build (qml-common/ScoreSheet.qml).
SubPage {
    id: page
    objectName: "scorePage"

    property variant engine: tarockEngine

    title: engine.active ? qsTr("Hand %1 of %2").arg(engine.handNumber).arg(engine.handsPerMatch)
                         : qsTr("Scores")

    ScoreSheet {
        width: page.width
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
            page.pageStack.pop()
        }
    }
}
