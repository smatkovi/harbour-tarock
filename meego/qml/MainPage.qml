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
// The start page of docs/design.md §6.1; the counterpart of
// sailfish/MainPage.qml. The rule profile and the number of players it starts
// a match with are the ones the settings page last left in Prefs.
Page {
    id: page
    objectName: "mainPage"

    // main.cpp installs the engine as a root context property.
    property variant engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property variant learn: engine.learn === undefined ? null : engine.learn

    // Someone who has never taken the tour is offered it once, here, instead
    // of being dropped into a hand of tarock (docs/design.md §7.9).
    property bool offerHidden: false
    property bool offerTutorial: !page.offerHidden && page.learn !== null
                                          && page.learn.tourSeen !== true

    Rectangle { anchors.fill: parent; color: AppTheme.tableColor; z: -1 }
    // The first page hands the engine to the singleton, which cannot read the
    // root context itself.
    Component.onCompleted: Prefs.adopt(page.engine)

    function startTable() {
        page.pageStack.push(Qt.resolvedUrl("TablePage.qml"))
    }

    Flickable {
        anchors.fill: parent
        pressDelay: 150
        contentHeight: content.height + AppTheme.paddingLarge
        clip: true

        Column {
            id: content
            width: page.width
            y: AppTheme.paddingLarge   // QtQuick 1.1 has no padding here
            spacing: AppTheme.paddingMedium

            Label {
                x: AppTheme.horizontalPageMargin
                width: parent.width - 2 * AppTheme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("Tarock")
                font.pixelSize: AppTheme.fontSizeExtraLarge
                color: AppTheme.highlightColor
            }

            Label {
                x: AppTheme.horizontalPageMargin
                width: parent.width - 2 * AppTheme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                text: page.engine.profileNameFor(Prefs.profileKey) + " · "
                      + qsTr("%1 players").arg(page.engine.seatsFor(Prefs.profileKey, Prefs.players))
                font.pixelSize: AppTheme.fontSizeExtraSmall
                color: AppTheme.secondaryColor
            }

            Item { width: 1; height: AppTheme.paddingLarge }

            // The welcome of §7.9: shown until the tour has run once.
            Column {
                width: parent.width
                visible: page.offerTutorial
                spacing: AppTheme.paddingSmall

                Label {
                    x: AppTheme.horizontalPageMargin
                    width: page.width - 2 * AppTheme.horizontalPageMargin
                    wrapMode: Text.WordWrap
                    color: AppTheme.highlightColor
                    font.pixelSize: AppTheme.fontSizeSmall
                    text: qsTr("New to tarock?")
                }

                Label {
                    x: AppTheme.horizontalPageMargin
                    width: page.width - 2 * AppTheme.horizontalPageMargin
                    wrapMode: Text.WordWrap
                    color: AppTheme.secondaryColor
                    font.pixelSize: AppTheme.fontSizeExtraSmall
                    text: qsTr("It teaches the whole game in one go - the goal, how a hand runs and the rules - then shows you the table and takes the rules module by module.")
                }

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: AppTheme.paddingMedium

                    Button {
                        text: qsTr("Learn to play")
                        onClicked: page.pageStack.push(Qt.resolvedUrl("TutorialPage.qml"))
                    }

                    Button {
                        text: qsTr("Not now")
                        onClicked: page.offerHidden = true
                    }
                }
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

            // The way into the tutorial of docs/design.md §7.9 and, behind it,
            // the learning mode of §7.
            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: page.engine.table.role !== 0 ? qsTr("Zurück zum Tisch der anderen")
                                                   : qsTr("Mit anderen Geräten")
                onClicked: page.pageStack.push(Qt.resolvedUrl("NetPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: !page.offerTutorial
                text: qsTr("Learn to play")
                onClicked: page.pageStack.push(Qt.resolvedUrl("TutorialPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Learn")
                onClicked: page.pageStack.push(Qt.resolvedUrl("LearnPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Rules")
                onClicked: page.pageStack.push(Qt.resolvedUrl("RulesPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Settings")
                onClicked: page.pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("About")
                onClicked: page.pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
            }

            Item { width: 1; height: AppTheme.paddingLarge }

            Label {
                x: AppTheme.horizontalPageMargin
                width: parent.width - 2 * AppTheme.horizontalPageMargin
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                font.pixelSize: AppTheme.fontSizeExtraSmall
                color: AppTheme.secondaryColor
                text: page.engine.canResume ? qsTr("A saved match is waiting for you.")
                                            : qsTr("Nothing saved yet.")
            }
        }
    }
}
