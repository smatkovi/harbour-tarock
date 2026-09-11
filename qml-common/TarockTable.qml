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

    The tarock table, shared by the Sailfish OS and the Android build. Kept to
    QtQuick 2.6 and ES5 so it runs on Qt 5.6 and Qt 6 alike: no Sailfish.Silica
    and no QtQuick.Controls imports here. The platforms provide the Style
    singleton and, from M3 on, Card, FlyingCard, TableButton and SubPage.

    M0 draws the empty table only: header line and the seat and trick areas the
    layout of docs/design.md §6.2 reserves. Hand fan, trick area content, bid
    bar, talon, announcements and the learning bar arrive in M3.
*/
import QtQuick 2.6
import "."

Item {
    id: table

    // main.cpp of both platforms installs this as a root context property.
    property var engine: tarockEngine

    // The felt. Seat panels, hand fan and trick area are drawn on top of it
    // from M3 on; the five-seat variant of the layout follows in M7.
    readonly property color tableColor: "#0b5d36"
    readonly property color feltEdgeColor: "#08492a"

    Rectangle {
        anchors.fill: parent
        color: table.tableColor
    }

    Item {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: headerLabel.height + 2 * Style.paddingMedium

        Rectangle {
            anchors.fill: parent
            color: table.feltEdgeColor
        }

        Text {
            id: headerLabel
            anchors.centerIn: parent
            width: parent.width - 2 * Style.horizontalPageMargin
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            text: table.engine.profileName
            font.pixelSize: Style.fontSizeMedium
            color: Style.highlightColor
        }
    }

    // The oval the trick is played on; the cards themselves follow in M3.
    Rectangle {
        id: felt
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: header.bottom
        anchors.topMargin: Style.paddingLarge
        anchors.bottom: statusLabel.top
        anchors.bottomMargin: Style.paddingLarge
        width: Math.min(parent.width - 2 * Style.horizontalPageMargin, height)
        radius: width / 2
        color: "transparent"
        border.color: table.feltEdgeColor
        border.width: Math.max(1, Style.paddingSmall / 2)
    }

    Text {
        id: statusLabel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Style.paddingLarge
        width: parent.width - 2 * Style.horizontalPageMargin
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        text: table.engine.status
        font.pixelSize: Style.fontSizeSmall
        color: Style.secondaryColor
    }
}
