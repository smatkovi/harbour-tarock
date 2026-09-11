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

// The Android frame around the shared table; the counterpart of
// sailfish/TablePage.qml. The table itself is qml-common/TarockTable.qml.
Item {
    id: page
    objectName: "tablePage"

    property var engine: tarockEngine

    Rectangle {
        anchors.fill: parent
        color: Theme.tableColor
    }

    TarockTable {
        anchors.fill: parent
        engine: page.engine
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
            text: qsTr("About")
            onTriggered: page.StackView.view.push(Qt.resolvedUrl("AboutPage.qml"))
        }
    }
}
