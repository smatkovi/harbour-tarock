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

// One playing card of the 54-card Industrie und Glück pack, drawn from its
// card id; the counterpart of sailfish/Card.qml and deliberately the same
// property interface, so the shared QML in qml-common/ never has to know which
// platform it runs on.
//
// Until the real artwork arrives in M8 (docs/design.md §10) the card is drawn
// from text: the tarock number in Roman numerals, or the suit symbol with the
// rank letter. The id encoding is the one of src/core/Card.h:
//   0..21  tarock, 0 = I … 20 = XXI, 21 = Sküs
//   22..53 suit cards, 22 + 8 * suit + slot, slot 0 = king … 3 = jack,
//          slot 4..7 = the low cards, strongest first.
Item {
    id: root

    // --- the interface both platforms implement -----------------------------
    property int cardId: -1
    // Long name from TarockEngine.cardLabel(); drawn as the caption. Optional:
    // the rank and the suit are derived from the id when it is empty.
    property string label: ""
    // 0 hearts, 1 diamonds, 2 spades, 3 clubs, 4 tarock; -1 = derive from the id.
    property int suit: -1
    // An illegal card in the current trick: drawn flat, still tappable so the
    // table can answer the tap with the reason why it cannot be played.
    property bool dimmed: false
    // Picked up in the hand fan, or the winning card of a trick.
    property bool selected: false
    property bool faceDown: false
    // The shared table says faceUp instead; either side may be set, the card
    // shows its back as soon as one of them asks for it.
    property bool faceUp: !faceDown
    // Deck id from TarockEngine.deck ("clean54", "iug1904"). The text card
    // below ignores it; M8 picks the artwork with it (docs/design.md §10).
    property string deck: ""
    signal clicked()

    readonly property bool showsBack: faceDown || !faceUp

    // M8: das Bild aus dem eingestellten Deck (docs/design.md §10). Findet die
    // Engine keines -- Deck "modern", oder die Datei fehlt --, bleibt die
    // gezeichnete Karte darunter stehen.
    readonly property string artwork: (typeof tarockEngine === "undefined")
                                       ? "" : tarockEngine.deckPath(root.showsBack ? -1 : root.cardId)

    width: Theme.itemSizeSmall
    height: width * 1.6

    readonly property int suitIndex: suit >= 0 ? suit : root.suitOf(cardId)
    readonly property bool isTarock: suitIndex === 4
    readonly property bool isRed: suitIndex === 0 || suitIndex === 1
    readonly property color inkColor: isTarock ? "#2f4f7f" : (isRed ? "#a01b1b" : "#1c1c1c")

    // The Industrie und Glück numerals; "IIII" for the four, as on the card.
    function tarockName(id) {
        var names = ["I", "II", "III", "IIII", "V", "VI", "VII", "VIII", "IX", "X", "XI",
                     "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX",
                     "XXI", "Sküs"]
        return (id >= 0 && id < names.length) ? names[id] : "?"
    }

    function suitOf(id) {
        if (id < 0 || id > 53)
            return -1
        return id < 22 ? 4 : Math.floor((id - 22) / 8)
    }

    function suitSymbol(index) {
        var symbols = ["♥", "♦", "♠", "♣", "✹"]
        return (index >= 0 && index < symbols.length) ? symbols[index] : ""
    }

    // Short rank as it is printed in the corner: the numeral for a tarock, the
    // court letter or the number of pips for a suit card.
    function rankText(id) {
        if (id < 0 || id > 53)
            return ""
        if (id < 22)
            return tarockName(id)
        var slot = (id - 22) % 8
        if (slot < 4)
            return ["K", "D", "R", "B"][slot]
        return root.isRed ? ["A", "2", "3", "4"][slot - 4]
                          : ["10", "9", "8", "7"][slot - 4]
    }

    opacity: dimmed ? 0.35 : 1.0
    Behavior on opacity { NumberAnimation { duration: 120 } }

    scale: selected ? 1.06 : 1.0
    Behavior on scale { NumberAnimation { duration: 120; easing.type: Easing.OutCubic } }

    // Face side.
    Rectangle {
        id: face
        anchors.fill: parent
        radius: width * 0.1
        visible: !root.showsBack
        color: "#f4efe2"

        // Das Kartenbild liegt über der Zeichnung; erst wenn es wirklich da
        // ist, verschwindet sie darunter.
        Image {
            id: artworkImage
            anchors.fill: parent
            anchors.margins: face.border.width
            source: root.artwork
            visible: status === Image.Ready
            asynchronous: true
            fillMode: Image.PreserveAspectFit
            smooth: true
        }
        border.width: Math.max(1, root.width * 0.02)
        border.color: root.selected ? Theme.highlightColor : "#b8ad93"

        // Corner index, repeated upside down in the opposite corner the way a
        // real card carries it.
        Repeater {
            visible: !artworkImage.visible
            model: 2
            Column {
                x: index === 0 ? face.border.width + root.width * 0.06
                               : face.width - width - face.border.width - root.width * 0.06
                y: index === 0 ? face.border.width + root.height * 0.04
                               : face.height - height - face.border.width - root.height * 0.04
                rotation: index === 0 ? 0 : 180
                spacing: -root.height * 0.01

                Text {
                    text: root.rankText(root.cardId)
                    color: root.inkColor
                    font.pixelSize: Math.max(Theme.fontSizeTiny, root.width * 0.26)
                    font.bold: true
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: root.suitSymbol(root.suitIndex)
                    color: root.inkColor
                    font.pixelSize: Math.max(Theme.fontSizeTiny, root.width * 0.22)
                    visible: !root.isTarock
                }
            }
        }

        // The middle of the card: the big suit symbol, or the numeral again on
        // a tarock, where the number is what the player reads.
        Text {
            visible: !artworkImage.visible
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -root.height * 0.04
            width: parent.width * 0.8
            horizontalAlignment: Text.AlignHCenter
            text: root.isTarock ? root.tarockName(root.cardId)
                                : root.suitSymbol(root.suitIndex)
            color: root.inkColor
            font.pixelSize: root.isTarock ? root.width * 0.42 : root.width * 0.58
            font.bold: true
            fontSizeMode: Text.HorizontalFit
            minimumPixelSize: 8
        }

        // The full name from the engine, so the placeholder deck still reads
        // like a card until the artwork replaces it.
        Text {
            visible: !artworkImage.visible
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: root.height * 0.06
            width: parent.width * 0.86
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            text: root.label
            visible: root.label !== "" && root.height > Theme.itemSizeSmall
            color: "#6b6455"
            font.pixelSize: Math.max(8, root.width * 0.14)
        }
    }

    // Back side: the same shape, so a face-down card takes exactly the space a
    // face-up one does.
    Rectangle {
        id: back
        anchors.fill: parent
        radius: face.radius
        visible: root.showsBack

        Image {
            id: backArtwork
            anchors.fill: parent
            anchors.margins: parent.border.width
            source: root.showsBack ? root.artwork : ""
            visible: status === Image.Ready
            asynchronous: true
            fillMode: Image.PreserveAspectFit
            smooth: true
        }
        color: "#7b2230"
        border.width: face.border.width
        border.color: "#f4efe2"

        Rectangle {
            visible: !backArtwork.visible
            anchors.fill: parent
            anchors.margins: root.width * 0.1
            radius: parent.radius * 0.6
            color: "transparent"
            border.width: Math.max(1, root.width * 0.015)
            border.color: "#f0d7a8"
        }

        Text {
            visible: !backArtwork.visible
            anchors.centerIn: parent
            text: "✹"
            color: "#f0d7a8"
            font.pixelSize: root.width * 0.34
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
