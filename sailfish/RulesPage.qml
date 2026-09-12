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

// The rule reference of docs/design.md §6.1. The searchable browser with the
// tariff table and the glossary (RulesBrowser.qml, TariffTable.qml,
// GlossaryList.qml) arrives with the learning mode in M5/M6; until then this
// page names the profile that is in force and the specification every rule of
// it comes from.
Page {
    id: page
    objectName: "rulesPage"
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
                title: qsTr("Rules")
                description: page.engine.profileNameFor(Prefs.profileKey)
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                color: Theme.secondaryColor
                text: qsTr("Every rule the app enforces is written down in one of the specifications below, section by section. The searchable rule browser and the tariff tables follow with the learning mode.")
            }

            SectionHeader { text: qsTr("Profiles") }

            Repeater {
                model: page.engine.profileKeys()

                ListItem {
                    width: parent.width
                    contentHeight: entry.height + 2 * Theme.paddingSmall
                    highlighted: modelData === Prefs.profileKey
                    onClicked: Prefs.profileKey = modelData

                    Column {
                        id: entry
                        x: Theme.horizontalPageMargin
                        width: parent.width - 2 * Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter

                        Label {
                            width: parent.width
                            text: page.engine.profileNameFor(modelData)
                            wrapMode: Text.WordWrap
                            color: modelData === Prefs.profileKey ? Theme.highlightColor
                                                                  : Theme.primaryColor
                        }
                        Label {
                            width: parent.width
                            text: modelData
                            color: Theme.secondaryColor
                            font.pixelSize: Theme.fontSizeExtraSmall
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }

            SectionHeader { text: qsTr("Specifications") }

            Repeater {
                model: [
                    { title: qsTr("Königrufen, Upper Austria"), source: "docs/koenigrufen.md" },
                    { title: qsTr("Hungarian Tarokk, Illusztrált"), source: "docs/hungarian.md" },
                    { title: qsTr("Tapp- and Dreiertarock"), source: "docs/tapptarock.md" },
                    { title: qsTr("Strohmandeln"), source: "docs/strohmandeln.md" }
                ]

                Column {
                    x: Theme.horizontalPageMargin
                    width: parent.width - 2 * Theme.horizontalPageMargin

                    Label {
                        width: parent.width
                        text: modelData.title
                        wrapMode: Text.WordWrap
                    }
                    Label {
                        width: parent.width
                        text: modelData.source
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                        wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}
