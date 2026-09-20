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

SubPage {
    title: qsTr("About Tarock")

    Image {
        width: Theme.itemSizeHuge
        height: width
        anchors.horizontalCenter: parent.horizontalCenter
        source: "qrc:/sailfish/icons/icon-256.png"
        fillMode: Image.PreserveAspectFit
        smooth: true
    }

    Label {
        width: parent.width
        horizontalAlignment: Text.AlignHCenter
        text: "Tarock"
        font.pixelSize: Theme.fontSizeHuge
        font.bold: true
        color: Theme.highlightColor
    }

    Label {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        text: qsTr("Königrufen and Hungarian Tarokk with a learning mode")
        color: Theme.secondaryHighlightColor
        font.pixelSize: Theme.fontSizeSmall
    }

    Label {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        text: qsTr("Version %1 · GPL-3.0-or-later · Copyright 2026 smatkovi").arg("0.2.3")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    Label {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        text: qsTr("Rule sources")
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeSmall
    }

    Label {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        wrapMode: Text.WordWrap
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeSmall
        text: qsTr("The rules this game follows are written down in four specifications that ship with the source code. Every rule the app enforces can be traced back to a numbered section in them.")
    }

    Repeater {
        model: [
            { profile: "AT-KR-OOE-2023-04", source: "docs/koenigrufen.md",
              title: qsTr("Königrufen, Upper Austria") },
            { profile: "HU-ILLU-ITVB-2019", source: "docs/hungarian.md",
              title: qsTr("Hungarian Tarokk, Illusztrált") },
            { profile: "AT-TAPP", source: "docs/tapptarock.md",
              title: qsTr("Tapp- and Dreiertarock") },
            { profile: "AT-STROH", source: "docs/strohmandeln.md",
              title: qsTr("Strohmandeln") }
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
                text: modelData.source + "  ·  " + modelData.profile
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.WordWrap
            }
        }
    }

    Label {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        text: qsTr("Card artwork")
        color: Theme.highlightColor
        font.pixelSize: Theme.fontSizeSmall
    }

    Label {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        wrapMode: Text.WordWrap
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeSmall
        text: qsTr("Credits for the card decks, fonts and other assets are listed in CREDITS/ASSETS.md. The decks themselves are added in a later release.")
    }
}
