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

// The rule reference of docs/design.md §6.1; the counterpart of
// sailfish/RulesPage.qml. The searchable browser with the tariff table and the
// glossary arrives with the learning mode in M5/M6; until then this page names
// the profile that is in force and the specification its rules come from.
SubPage {
    id: page
    objectName: "rulesPage"
    title: qsTr("Rules")

    property var engine: tarockEngine

    TextBlock {
        text: qsTr("Every rule the app enforces is written down in one of the specifications below, section by section. The searchable rule browser and the tariff tables follow with the learning mode.")
        color: Theme.secondaryColor
    }

    SectionLabel { text: qsTr("Profiles") }

    Repeater {
        model: page.engine.profileKeys()

        ItemDelegate {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            highlighted: modelData === Prefs.profileKey
            onClicked: Prefs.profileKey = modelData

            contentItem: Column {
                Label {
                    width: parent.width
                    text: page.engine.profileNameFor(modelData)
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeSmall
                    color: modelData === Prefs.profileKey ? Theme.highlightColor
                                                          : Theme.primaryColor
                }
                Label {
                    width: parent.width
                    text: modelData
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                }
            }
        }
    }

    SectionLabel { text: qsTr("Specifications") }

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
                font.pixelSize: Theme.fontSizeSmall
            }
            Label {
                width: parent.width
                text: modelData.source
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
            }
        }
    }
}
