import QtQuick 2.6
import Sailfish.Silica 1.0
import "." as App

// Ein Tisch mit anderen Geräten (docs/design.md §8): eines eröffnet, die
// anderen treten bei, dann startet der Gastgeber. Freie Plätze spielt der
// Computer. Über WLAN oder über Bluetooth -- wer eröffnet, ist über beides
// zugleich erreichbar.
Page {
    id: page
    objectName: "netPage"
    allowedOrientations: Orientation.All

    property var engine: tarockEngine
    readonly property var table: tarockEngine.table

    Component.onCompleted: {
        table.browser.search()
        table.bluetooth.refresh()
    }

    // Sobald die Partie läuft, gehört der Bildschirm dem Tisch.
    Connections {
        target: page.engine
        onStateChanged: {
            if (page.engine.active && page.table.role !== 0
                    && pageStack.currentPage === page && page.table.guest)
                pageStack.replace(Qt.resolvedUrl("TablePage.qml"))
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge
        VerticalScrollDecorator { }

        PullDownMenu {
            MenuItem {
                text: qsTr("Erneut suchen")
                onClicked: {
                    page.table.browser.search()
                    page.table.bluetooth.refresh()
                }
            }
        }

        Column {
            id: content
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Mit anderen Geräten")
                description: qsTr("Ein Tisch über WLAN oder Bluetooth")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryHighlightColor
                text: qsTr("Auf jedem Gerät muss dieses Tarock laufen. Über WLAN müssen alle im "
                           + "selben Netz sein; ohne Netz geht es über Bluetooth, dafür die Geräte "
                           + "einmal in den Systemeinstellungen koppeln. Plätze, die frei bleiben, "
                           + "spielt der Computer.")
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                visible: text !== ""
                wrapMode: Text.WordWrap
                color: Theme.highlightColor
                text: page.table.status
            }

            // ---- Die Plätze, sobald ein Tisch offen ist ------------------
            Repeater {
                model: page.table.role !== 0 ? page.table.lobby : 0
                Label {
                    x: Theme.horizontalPageMargin * 2
                    width: content.width - 3 * Theme.horizontalPageMargin
                    truncationMode: TruncationMode.Fade
                    text: qsTr("Platz %1: %2").arg(modelData.seat + 1).arg(modelData.name)
                          + (modelData.isMe ? qsTr(" (du)") : "")
                    color: modelData.human ? Theme.primaryColor : Theme.secondaryColor
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: page.table.role === 1 && !page.engine.active
                text: qsTr("Partie starten")
                onClicked: {
                    page.engine.startTableMatch()
                    pageStack.replace(Qt.resolvedUrl("TablePage.qml"))
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                visible: page.table.role !== 0
                text: qsTr("Tisch verlassen")
                onClicked: page.engine.leaveTable()
            }

            // ---- Eröffnen -----------------------------------------------
            SectionHeader { text: qsTr("Tisch eröffnen") }

            ComboBox {
                id: profileBox
                width: parent.width
                label: qsTr("Regeln")
                enabled: page.table.role === 0
                currentIndex: Math.max(0, page.engine.profileKeys().indexOf(Prefs.profileKey))
                menu: ContextMenu {
                    Repeater {
                        model: page.engine.profileKeys()
                        MenuItem { text: page.engine.profileNameFor(modelData) }
                    }
                }
                property string key: page.engine.profileKeys()[
                        Math.max(0, Math.min(currentIndex, page.engine.profileKeys().length - 1))]
            }

            ComboBox {
                id: playersBox
                width: parent.width
                label: qsTr("Spieler")
                enabled: page.table.role === 0 && seats.length > 1
                // Die Sitzzahlen des gewählten Profils: Königrufen vier oder
                // fünf (der fünfte setzt aus), das ungarische Blatt vier,
                // Tapp-Tarock drei.
                property var seats: page.engine.seatOptionsFor(profileBox.key)
                currentIndex: Math.max(0, seats.indexOf(Prefs.players))
                menu: ContextMenu {
                    Repeater {
                        model: playersBox.seats
                        MenuItem { text: String(modelData) }
                    }
                }
                property int count: seats.length === 0 ? 4
                        : seats[Math.max(0, Math.min(currentIndex, seats.length - 1))]
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: page.table.role === 0
                text: qsTr("Eröffnen")
                onClicked: page.engine.hostTable(profileBox.key, playersBox.count)
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: (page.table.browser.localAddresses !== ""
                       ? qsTr("Dieses Gerät im Netz: %1").arg(page.table.browser.localAddresses)
                       : qsTr("Dieses Gerät hängt in keinem Netz"))
                      + "\n"
                      + (page.table.bluetooth.available
                         ? qsTr("Bluetooth: %1").arg(page.table.bluetooth.localName !== ""
                                ? page.table.bluetooth.localName
                                : page.table.bluetooth.localAddress)
                         : qsTr("Bluetooth ist ausgeschaltet"))
            }

            // ---- Beitreten ----------------------------------------------
            SectionHeader { text: qsTr("Im WLAN beitreten") }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                visible: page.table.browser.hosts.length === 0
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: page.table.browser.searching
                      ? qsTr("Suche läuft …")
                      : qsTr("Noch kein offener Tisch gefunden. Erst am anderen Gerät eröffnen, "
                             + "dann hier von oben nachziehen.")
            }

            Repeater {
                model: page.table.browser.hosts
                BackgroundItem {
                    width: content.width
                    height: Theme.itemSizeSmall
                    enabled: page.table.role === 0
                    onClicked: page.engine.joinTable(modelData.address)

                    Column {
                        x: Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter
                        Label {
                            text: modelData.name
                            color: parent.parent.highlighted ? Theme.highlightColor : Theme.primaryColor
                        }
                        Label {
                            text: qsTr("%1 Plätze · %2 frei · %3").arg(modelData.players)
                                  .arg(modelData.openSeats).arg(modelData.address)
                            font.pixelSize: Theme.fontSizeExtraSmall
                            color: Theme.secondaryColor
                        }
                    }
                }
            }

            SectionHeader { text: qsTr("Über Bluetooth beitreten") }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                visible: page.table.bluetooth.devices.length === 0
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.secondaryColor
                text: page.table.bluetooth.available
                      ? qsTr("Keine gekoppelten Geräte. Die Geräte einmal in den "
                             + "Systemeinstellungen koppeln, dann hier von oben nachziehen.")
                      : qsTr("Bluetooth ist ausgeschaltet.")
            }

            Repeater {
                model: page.table.bluetooth.devices
                BackgroundItem {
                    width: content.width
                    height: Theme.itemSizeSmall
                    enabled: page.table.role === 0
                    onClicked: page.engine.joinTableBluetooth(modelData.address)

                    Column {
                        x: Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter
                        Label {
                            text: modelData.name
                            color: parent.parent.highlighted ? Theme.highlightColor : Theme.primaryColor
                        }
                        Label {
                            text: modelData.address
                            font.pixelSize: Theme.fontSizeExtraSmall
                            color: Theme.secondaryColor
                        }
                    }
                }
            }

            SectionHeader { text: qsTr("Adresse eingeben") }

            TextField {
                id: manual
                width: parent.width
                label: qsTr("Adresse des eröffnenden Geräts")
                placeholderText: qsTr("z. B. 192.168.1.23")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                EnterKey.enabled: text.trim().length > 0 && page.table.role === 0
                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: {
                    focus = false
                    page.engine.joinTable(text)
                }
            }
        }
    }
}
