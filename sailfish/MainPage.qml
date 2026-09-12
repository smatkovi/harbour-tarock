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

// The start page of docs/design.md §6.1: new match, continue a saved one, the
// rule reference, the settings and the about page. The rule profile and the
// number of players it starts a match with are the ones the settings page
// last left in Prefs.
Page {
    id: page
    objectName: "mainPage"
    allowedOrientations: Orientation.All

    // main.cpp installs the engine as a root context property.
    property var engine: tarockEngine

    // The first page hands the engine to the singleton, which cannot read the
    // root context itself.
    Component.onCompleted: Prefs.adopt(page.engine)

    function startTable() {
        pageStack.push(Qt.resolvedUrl("TablePage.qml"))
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge
        VerticalScrollDecorator { }

        Column {
            id: content
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Tarock")
                description: page.engine.profileNameFor(Prefs.profileKey) + " · "
                             + qsTr("%1 players").arg(Prefs.players)
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("New game")
                onClicked: {
                    page.engine.startMatch(Prefs.profileKey, Prefs.players)
                    page.startTable()
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Continue")
                enabled: page.engine.canResume || page.engine.active
                onClicked: {
                    if (!page.engine.active)
                        page.engine.resume()
                    page.startTable()
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Rules")
                onClicked: pageStack.push(Qt.resolvedUrl("RulesPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("About")
                onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }

            Item { width: 1; height: Theme.paddingLarge }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
                text: page.engine.canResume ? qsTr("A saved match is waiting for you.")
                                            : qsTr("Nothing saved yet.")
            }
        }
    }
}
