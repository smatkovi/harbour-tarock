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
SubPage {
    title: qsTr("About Tarock")

    Image {
        width: AppTheme.itemSizeHuge
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
        font.pixelSize: AppTheme.fontSizeHuge
        font.bold: true
        color: AppTheme.highlightColor
    }

    Label {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        text: qsTr("Königrufen and Hungarian Tarokk with a learning mode")
        color: AppTheme.secondaryHighlightColor
        font.pixelSize: AppTheme.fontSizeSmall
    }

    Label {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        text: qsTr("Version %1 · GPL-3.0-or-later · Copyright 2026 smatkovi").arg("0.2.3")
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeExtraSmall
    }

    Label {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        text: qsTr("Rule sources")
        color: AppTheme.highlightColor
        font.pixelSize: AppTheme.fontSizeSmall
    }

    Label {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        wrapMode: Text.WordWrap
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeSmall
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
            x: AppTheme.horizontalPageMargin
            width: parent.width - 2 * AppTheme.horizontalPageMargin
            Label {
                width: parent.width
                text: modelData.title
                wrapMode: Text.WordWrap
                font.pixelSize: AppTheme.fontSizeSmall
            }
            Label {
                width: parent.width
                text: modelData.source + "  ·  " + modelData.profile
                color: AppTheme.secondaryColor
                font.pixelSize: AppTheme.fontSizeExtraSmall
                wrapMode: Text.WordWrap
            }
        }
    }

    Label {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        text: qsTr("Card artwork")
        color: AppTheme.highlightColor
        font.pixelSize: AppTheme.fontSizeSmall
    }

    Label {
        x: AppTheme.horizontalPageMargin
        width: parent.width - 2 * AppTheme.horizontalPageMargin
        wrapMode: Text.WordWrap
        color: AppTheme.secondaryColor
        font.pixelSize: AppTheme.fontSizeSmall
        text: qsTr("Credits for the card decks, fonts and other assets are listed in CREDITS/ASSETS.md. The decks themselves are added in a later release.")
    }
}
