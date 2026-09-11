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
import QtQuick.Controls.Material

ApplicationWindow {
    id: app
    visible: true
    width: 411
    height: 900
    title: "Tarock"
    color: Theme.tableColor

    Material.theme: Material.Dark
    Material.accent: Theme.highlightColor
    Material.primary: Theme.barColor

    // Android draws the app behind the status and navigation bars.
    Item {
        id: safeFrame
        anchors.fill: parent

        StackView {
            id: stack
            anchors.fill: parent
            anchors.topMargin: safeFrame.SafeArea.margins.top
            anchors.bottomMargin: safeFrame.SafeArea.margins.bottom
            anchors.leftMargin: safeFrame.SafeArea.margins.left
            anchors.rightMargin: safeFrame.SafeArea.margins.right
            initialItem: TablePage { }
        }
    }

    // The Android back button closes pages before it closes the app.
    onClosing: (close) => {
        if (stack.depth > 1) {
            close.accepted = false
            stack.pop()
        }
    }
}
