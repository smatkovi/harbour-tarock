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

// Ein Tisch mit anderen Geräten (docs/design.md §8), das Gegenstück zu
// sailfish/NetPage.qml.
//
// Auf Android nur über das WLAN: eine App bekommt dort keine RFCOMM-Sockets
// (src/net/BtLink.cpp), der Bluetooth-Teil der anderen Fassungen fehlt hier
// deshalb. Ein Android-Gerät kann trotzdem an jedem Tisch mitspielen, den ein
// Gerät im selben Netz eröffnet hat.
SubPage {
    id: page
    objectName: "netPage"
    title: qsTr("Mit anderen Geräten")

    property var engine: tarockEngine
    readonly property var table: tarockEngine.table

    Component.onCompleted: page.table.browser.search()

    Label {
        width: parent.width
        wrapMode: Text.WordWrap
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.secondaryColor
        text: qsTr("Auf jedem Gerät muss dieses Tarock laufen, und alle müssen im selben WLAN "
                   + "sein. Plätze, die frei bleiben, spielt der Computer.")
    }

    Label {
        width: parent.width
        visible: text !== ""
        wrapMode: Text.WordWrap
        color: Theme.highlightColor
        text: page.table.status
    }

    Repeater {
        model: page.table.role !== 0 ? page.table.lobby : 0
        Label {
            width: page.width - 2 * Theme.paddingLarge
            elide: Text.ElideRight
            font.pixelSize: Theme.fontSizeSmall
            color: modelData.human ? Theme.primaryColor : Theme.secondaryColor
            text: qsTr("Platz %1: %2").arg(modelData.seat + 1).arg(modelData.name)
                  + (modelData.isMe ? qsTr(" (du)") : "")
        }
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        visible: page.table.role === 1 && !page.engine.active
        text: qsTr("Partie starten")
        onClicked: {
            page.engine.startTableMatch()
            page.StackView.view.replace(Qt.resolvedUrl("TablePage.qml"))
        }
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        visible: page.table.role !== 0
        text: qsTr("Tisch verlassen")
        onClicked: page.engine.leaveTable()
    }

    Label {
        width: parent.width
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.highlightColor
        text: qsTr("Tisch eröffnen")
    }

    Row {
        spacing: Theme.paddingMedium
        property int players: Prefs.players === 5 ? 5 : 4
        id: seatRow
        Button {
            text: "4"
            checked: seatRow.players === 4
            onClicked: seatRow.players = 4
        }
        Button {
            text: "5"
            checked: seatRow.players === 5
            onClicked: seatRow.players = 5
        }
        Button {
            text: qsTr("Eröffnen")
            enabled: page.table.role === 0
            onClicked: page.engine.hostTable(Prefs.profileKey, seatRow.players)
        }
    }

    Label {
        width: parent.width
        wrapMode: Text.WordWrap
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.secondaryColor
        text: page.table.browser.localAddresses !== ""
              ? qsTr("Dieses Gerät im Netz: %1").arg(page.table.browser.localAddresses)
              : qsTr("Dieses Gerät hängt in keinem Netz")
    }

    Label {
        width: parent.width
        font.pixelSize: Theme.fontSizeSmall
        color: Theme.highlightColor
        text: qsTr("Beitreten")
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: page.table.browser.searching ? qsTr("Suche läuft …") : qsTr("Suchen")
        enabled: !page.table.browser.searching
        onClicked: page.table.browser.search()
    }

    Repeater {
        model: page.table.browser.hosts
        ItemDelegate {
            width: page.width
            enabled: page.table.role === 0
            onClicked: page.engine.joinTable(modelData.address)
            contentItem: Column {
                Label {
                    text: modelData.name
                    color: Theme.primaryColor
                }
                Label {
                    text: qsTr("%1 Plätze, %2 frei").arg(modelData.players)
                          .arg(modelData.openSeats) + " · " + modelData.address
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                }
            }
        }
    }

    TextField {
        id: manual
        width: parent.width
        placeholderText: qsTr("Adresse, z. B. 192.168.1.23")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Dieser Adresse beitreten")
        enabled: manual.text.length > 0 && page.table.role === 0
        onClicked: page.engine.joinTable(manual.text)
    }
}
