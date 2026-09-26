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
// Ein Tisch mit anderen Geräten (docs/design.md §8), das Gegenstück zu
// sailfish/NetPage.qml. Über WLAN oder über Bluetooth: wer eröffnet, ist
// über beides zugleich erreichbar.
SubPage {
    id: page
    objectName: "netPage"
    title: qsTr("Mit anderen Geräten")

    property variant engine: tarockEngine
    property variant table: tarockEngine.table

    Component.onCompleted: {
        page.table.browser.search()
        page.table.bluetooth.refresh()
    }

    TextBlock {
        width: parent.width
        text: qsTr("Auf jedem Gerät muss dieses Tarock laufen. Über WLAN müssen alle im selben "
                   + "Netz sein; ohne Netz geht es über Bluetooth, dafür die Geräte einmal in "
                   + "den Systemeinstellungen koppeln. Plätze, die frei bleiben, spielt der "
                   + "Computer.")
    }

    TextBlock {
        width: parent.width
        visible: text !== ""
        color: AppTheme.highlightColor
        text: page.table.status
    }

    Repeater {
        model: page.table.role !== 0 ? page.table.lobby : 0
        TextBlock {
            width: page.width
            elide: Text.ElideRight
            color: modelData.human ? AppTheme.primaryColor : AppTheme.secondaryColor
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
            page.pageStack.replace(Qt.resolvedUrl("TablePage.qml"))
        }
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        visible: page.table.role !== 0
        text: qsTr("Tisch verlassen")
        onClicked: page.engine.leaveTable()
    }

    SectionLabel { text: qsTr("Tisch eröffnen") }

    SectionLabel { text: qsTr("Regeln") }

    ButtonRow {
        id: profileRow
        width: parent.width
        property variant keys: tarockEngine.profileKeys()
        property int currentIndex: Math.max(0, keys.indexOf(Prefs.profileKey))
        property string key: keys[Math.max(0, Math.min(currentIndex, keys.length - 1))]
        Repeater {
            model: profileRow.keys
            Button {
                text: tarockEngine.profileNameFor(modelData)
                onClicked: profileRow.currentIndex = index
            }
        }
    }

    ButtonRow {
        id: playersRow
        width: parent.width
        // Die Sitzzahlen des gewählten Profils, nicht mehr fest vier und fünf.
        property variant seats: tarockEngine.seatOptionsFor(profileRow.key)
        property int currentIndex: Math.max(0, seats.indexOf(Prefs.players))
        property int count: seats.length === 0 ? 4
                : seats[Math.max(0, Math.min(currentIndex, seats.length - 1))]
        Repeater {
            model: playersRow.seats
            Button {
                text: String(modelData)
                onClicked: playersRow.currentIndex = index
            }
        }
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        enabled: page.table.role === 0
        text: qsTr("Eröffnen")
        onClicked: page.engine.hostTable(profileRow.key, playersRow.count)
    }

    TextBlock {
        width: parent.width
        text: (page.table.browser.localAddresses !== ""
               ? qsTr("Dieses Gerät im Netz: %1").arg(page.table.browser.localAddresses)
               : qsTr("Dieses Gerät hängt in keinem Netz"))
              + "\n"
              + (page.table.bluetooth.available
                 ? qsTr("Bluetooth: %1").arg(page.table.bluetooth.localName !== ""
                        ? page.table.bluetooth.localName : page.table.bluetooth.localAddress)
                 : qsTr("Bluetooth ist ausgeschaltet"))
    }

    SectionLabel { text: qsTr("Im WLAN beitreten") }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: page.table.browser.searching ? qsTr("Suche läuft ...") : qsTr("Suchen")
        enabled: !page.table.browser.searching
        onClicked: {
            page.table.browser.search()
            page.table.bluetooth.refresh()
        }
    }

    Repeater {
        model: page.table.browser.hosts
        Item {
            width: page.width
            height: AppTheme.itemSizeSmall

            Rectangle {
                anchors.fill: parent
                color: "#33ffffff"
                visible: hostMouse.pressed
            }
            MouseArea {
                id: hostMouse
                anchors.fill: parent
                enabled: page.table.role === 0
                onClicked: page.engine.joinTable(modelData.address)
            }
            Column {
                x: AppTheme.horizontalPageMargin
                width: parent.width - 2 * AppTheme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                Label {
                    width: parent.width
                    elide: Text.ElideRight
                    font.pixelSize: AppTheme.fontSizeSmall
                    color: hostMouse.pressed ? AppTheme.highlightColor : AppTheme.primaryColor
                    text: modelData.name
                }
                Label {
                    width: parent.width
                    elide: Text.ElideRight
                    font.pixelSize: AppTheme.fontSizeExtraSmall
                    color: AppTheme.secondaryColor
                    text: qsTr("%1 Plätze, %2 frei").arg(modelData.players).arg(modelData.openSeats)
                          + " · " + modelData.address
                }
            }
        }
    }

    SectionLabel { text: qsTr("Über Bluetooth beitreten") }

    TextBlock {
        width: parent.width
        visible: page.table.bluetooth.devices.length === 0
        text: page.table.bluetooth.available
              ? qsTr("Keine gekoppelten Geräte. Die Geräte einmal in den Systemeinstellungen "
                     + "koppeln, dann hier erneut suchen.")
              : qsTr("Bluetooth ist ausgeschaltet.")
    }

    Repeater {
        model: page.table.bluetooth.devices
        Item {
            width: page.width
            height: AppTheme.itemSizeSmall

            Rectangle {
                anchors.fill: parent
                color: "#33ffffff"
                visible: btMouse.pressed
            }
            MouseArea {
                id: btMouse
                anchors.fill: parent
                enabled: page.table.role === 0
                onClicked: page.engine.joinTableBluetooth(modelData.address)
            }
            Column {
                x: AppTheme.horizontalPageMargin
                width: parent.width - 2 * AppTheme.horizontalPageMargin
                anchors.verticalCenter: parent.verticalCenter
                Label {
                    width: parent.width
                    elide: Text.ElideRight
                    font.pixelSize: AppTheme.fontSizeSmall
                    color: btMouse.pressed ? AppTheme.highlightColor : AppTheme.primaryColor
                    text: modelData.name
                }
                Label {
                    width: parent.width
                    font.pixelSize: AppTheme.fontSizeExtraSmall
                    color: AppTheme.secondaryColor
                    text: modelData.address
                }
            }
        }
    }

    SectionLabel { text: qsTr("Adresse eingeben") }

    TextField {
        id: manual
        width: parent.width
        placeholderText: qsTr("z. B. 192.168.1.23")
        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Dieser Adresse beitreten")
        enabled: manual.text.length > 0 && page.table.role === 0
        onClicked: page.engine.joinTable(manual.text)
    }
}
