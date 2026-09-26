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

    The tarock table, shared by the Sailfish OS and the Android build. Kept to
    QtQuick 2.6 and ES5 so it runs on Qt 5.6 and Qt 6 alike: no Sailfish.Silica
    and no QtQuick.Controls imports here. The platforms provide the Style
    singleton, Card, FlyingCard, TableButton and SubPage.

    M3 draws the playable four-seat table: seat panels around the edge, the
    trick in the middle, the hand fan at the bottom and the phase-dependent
    action band above it. The five-seat variant is prepared (seats 1..4 sit
    around the edge, the seat that sits out is muted) and finished in M7.

    M6 hangs the learning mode of docs/design.md §7 into the same frame: the
    explanation bar under the header, the lesson band over the table, the hint
    bubble above the action band, the learning bar at the very bottom and the
    dialogs for "why not?", the standing and the review after the hand. None of
    them changes the table when the learning mode is off — every one of them is
    then invisible and zero high.

    Animations are the same protocol as harbour-snapszer/qml-common/MultiTable:
    the engine asks, the table flies, the table acknowledges. Every request is
    acknowledged exactly once, also when no card could be created — otherwise
    the engine stays in its animation phase and the hand stalls.
*/
import QtQuick 1.1
import "."

Item {
    id: table

    // main.cpp of both platforms installs this as a root context property.
    property variant engine: tarockEngine

    // The few colours Style does not carry: the felt and the two party hues.
    property color tableColor: "#0b5d36"
    property color feltEdgeColor: "#08492a"
    property color panelColor: "#f0202020"
    property color partnerColor: "#7fd67f"
    property color opponentColor: "#ff8a80"

    // Card artwork is 9:16 (assets/decks/*/deck.json, "aspect": 0.5625).
    property real cardRatio: 1.78

    property int players: engine.players
    // Der Platz rechts: zu dritt der zweite, zu viert der dritte, zu fünft
    // der vierte.
    property int lastSeat: table.players === 5 ? 4
            : (table.players === 4 ? 3 : (table.players === 3 ? 2 : 1))
    // Die Strohmänner des Strohmandelns, je Sitz eine Liste von Päckchen
    // (strohmandeln.md §4). In jedem anderen Profil ist die Liste leer und
    // die beiden Reihen sind null hoch.
    property variant strawmen: engine.strawmen
    property variant strawmenTaken: engine.strawmenTaken
    property bool hasStrawmen: strawmen && strawmen.length > 0
    function strawmenOf(seat) {
        return table.hasStrawmen && seat < table.strawmen.length ? table.strawmen[seat] : []
    }
    function strawmenTakenOf(seat) {
        return table.strawmenTaken && seat < table.strawmenTaken.length
                ? table.strawmenTaken[seat] : []
    }
    property variant seatList: engine.seats
    property variant me: seatList.length > 0 ? seatList[0] : ({})
    property bool meSittingOut: me.isSittingOut === true

    // The learning mode of docs/design.md §7. It reaches QML as a property of
    // the engine; if a platform installs LearnEngine as its own context
    // property instead, this one line is the only one that changes.
    property variant learn: engine && engine.learn !== undefined ? engine.learn : null
    // LearnLevel: 0 off, 1 learning, 2 novice (§7.1).
    property int learnLevel: learn === null || learn.level === undefined ? 0 : learn.level
    // Set by LessonPage; the table then keeps the lesson band even between steps.
    property bool lessonMode: false
    // The guided tour of docs/design.md §7.9 runs over the ordinary table; the
    // page that hosts it starts it and is told when it is through.
    property bool tourActive: tourOverlay.running
    // The reason of the last refused action, for the "why?" button.
    property variant lastReason: null

    // The page that hosts the table follows these into the rule reference.
    signal ruleRequested(string anchor)
    signal glossaryRequested(string term)
    // The tour has reached its last step or was skipped.
    signal tourFinished()

    function startTour() {
        tourOverlay.start()
    }

    // The names the steps of TourScript use. An unknown name, and a part that
    // this table does not show at all, simply leave the step without a frame:
    // the bubble then sits in the middle and the text still fits.
    function tourTarget(name) {
        if (name === "header")
            return header
        if (name === "seats")
            return table.players === 5 ? topLeftSeat : topSeat
        if (name === "trickArea")
            return trickArea
        if (name === "statusLabel")
            return statusLabel
        if (name === "handFan")
            return handFan
        if (name === "actionBand")
            return actionBand
        if (name === "learnPanel")
            return learnPanel
        if (name === "learnBar")
            return learnBar
        if (name === "mySeat")
            return mySeat
        return null
    }

    function openRule(anchor) {
        table.ruleRequested(anchor)
    }

    function openGlossary(term) {
        table.glossaryRequested(term)
    }

    // Cards that are currently in the air; they are drawn by the flying copy,
    // not by the trick area.
    property variant hiddenIds: ({})
    property int flightsRemaining: 0
    property int pendingWinner: -1
    property bool cardFlightActive: false
    property bool trickFlightActive: false
    property bool revealActive: false
    // The last formula every seat spoke, from the engine's speech() signal.
    property variant saidBySeat: ["", "", "", "", ""]
    // The ids that lie in the discard tray, so the fan can mark them.
    property variant trayIds: table.idSet(engine.discardTray)

    property int flightMs: 320
    property int stepMs: 60

    Rectangle {
        anchors.fill: parent
        color: table.tableColor
    }

    // --- helpers ----------------------------------------------------------------

    function seatInfo(seat) {
        return seat < seatList.length ? seatList[seat] : ({})
    }

    function mapSet(source, key, enabled) {
        var result = ({})
        for (var name in source) {
            if (source[name] && name !== String(key))
                result[name] = true
        }
        if (enabled)
            result[key] = true
        return result
    }

    // Card lists arrive either as maps or as bare ids; both are turned into a
    // lookup table here so the panels can stay simple.
    function idSet(list) {
        var result = ({})
        if (!list)
            return result
        for (var i = 0; i < list.length; ++i) {
            var id = table.cardIdOf(list[i])
            if (id >= 0)
                result[id] = true
        }
        return result
    }

    function cardIdOf(entry) {
        if (entry === undefined || entry === null)
            return -1
        if (typeof entry === "number")
            return entry
        return entry.id === undefined ? -1 : entry.id
    }

    // Seat 0 sits at the bottom; the others go clockwise around the table.
    function seatPanelOf(seat) {
        // Zu zweit (Strohmandeln) sitzt der Gegner gegenüber.
        if (table.players === 2)
            return topSeat
        // Zu dritt (Tapp-Tarock) sitzt niemand gegenüber: links und rechts.
        if (table.players === 3)
            return seat === 1 ? leftSeat : rightSeat
        if (table.players === 5) {
            if (seat === 1)
                return leftSeat
            if (seat === 2)
                return topLeftSeat
            if (seat === 3)
                return topRightSeat
            return rightSeat
        }
        if (seat === 1)
            return leftSeat
        if (seat === 2)
            return topSeat
        return rightSeat
    }

    function seatCenter(seat) {
        if (seat === 0)
            return handFan.mapToItem(animationLayer, handFan.width / 2, handFan.height / 2)
        var panel = seatPanelOf(seat)
        return panel.mapToItem(animationLayer, panel.width / 2, panel.height / 2)
    }

    function trickPoint(seat) {
        var point = trickArea.slot(seat)
        return trickArea.mapToItem(animationLayer, point.x, point.y)
    }

    // --- animation protocol --------------------------------------------------------

    function spawnFlight(id, fromPoint, toPoint, delay, rotation, callback) {
        var object = flyingComponent.createObject(animationLayer, {
            "cardId": id,
            "deck": table.engine.deck,
            "faceUp": true,
            "fromX": fromPoint.x,
            "fromY": fromPoint.y,
            "toX": toPoint.x,
            "toY": toPoint.y,
            "startDelay": delay,
            "flightDuration": table.flightMs,
            "toRotation": rotation
        })
        if (!object)
            return null
        object.finished.connect(function() { callback(object) })
        return object
    }

    function onCardFlight(cardId, seat) {
        cardFlightActive = true
        hiddenIds = table.mapSet(hiddenIds, cardId, true)
        var flight = table.spawnFlight(cardId, table.seatCenter(seat), table.trickPoint(seat), 0,
                                       trickArea.slotRotation(seat), function(object) {
            table.hiddenIds = table.mapSet(table.hiddenIds, object.cardId, false)
            object.destroy()
            table.finishCardFlight()
        })
        if (!flight)
            table.finishCardFlight()
    }

    function finishCardFlight() {
        if (!cardFlightActive)
            return
        cardFlightActive = false
        engine.completeCardAnimation()
    }

    // The winner is known only now, so the trick is held for a moment with the
    // winning card framed before it flies away.
    function onTrickFlight(winnerSeat) {
        pendingWinner = winnerSeat
        trickArea.winnerSeat = winnerSeat
        trickFlightActive = true
        trickLeadIn.restart()
    }

    function startTrickFlight() {
        var cards = engine.trick
        var end = table.seatCenter(pendingWinner)
        flightsRemaining = cards.length
        for (var i = 0; i < cards.length; ++i) {
            var id = table.cardIdOf(cards[i])
            hiddenIds = table.mapSet(hiddenIds, id, true)
            var flight = table.spawnFlight(id, table.trickPoint(cards[i].seat), end,
                                           i * table.stepMs, 0, function(object) {
                object.destroy()
                table.flightsRemaining--
                if (table.flightsRemaining <= 0)
                    table.finishTrickFlight()
            })
            if (!flight)
                flightsRemaining--
        }
        if (flightsRemaining <= 0)
            table.finishTrickFlight()
    }

    function finishTrickFlight() {
        if (!trickFlightActive)
            return
        trickFlightActive = false
        hiddenIds = ({})
        trickArea.winnerSeat = -1
        engine.completeTrickAnimation()
    }

    // Talon and open discards are shown for a moment, then acknowledged; a tap
    // cuts the pause short.
    function onReveal(what) {
        revealBox.what = what
        revealActive = true
        revealTimer.restart()
    }

    function finishReveal() {
        if (!revealActive)
            return
        revealActive = false
        revealTimer.stop()
        engine.completeReveal()
    }

    function clearFlights() {
        trickLeadIn.stop()
        for (var i = animationLayer.children.length - 1; i >= 0; --i) {
            var child = animationLayer.children[i]
            if (child && child.isFlyingCard)
                child.destroy()
        }
        hiddenIds = ({})
        flightsRemaining = 0
        cardFlightActive = false
        trickFlightActive = false
        trickArea.winnerSeat = -1
    }

    function onReset() {
        table.clearFlights()
        saidBySeat = ["", "", "", "", ""]
        noticeBox.visible = false
    }

    function onSpeech(seat, formula) {
        var next = saidBySeat.slice()
        if (seat >= 0 && seat < next.length)
            next[seat] = formula
        saidBySeat = next
    }

    // --- "why not?" (docs/design.md §7.3) ---------------------------------------------
    //
    // The reason travels as the map of §7.3; until the learning mode lands in
    // M6 the table shows the short text and keeps the long one one tap away.

    function reasonShort(reason) {
        if (reason === undefined || reason === null)
            return ""
        if (typeof reason === "string")
            return reason
        if (reason.short)
            return reason.short
        if (reason.text)
            return reason.text
        return reason.key ? reason.key : ""
    }

    function reasonLong(reason) {
        if (reason === undefined || reason === null || typeof reason === "string")
            return ""
        return reason.text ? reason.text : ""
    }

    // Off shows the short toast, from Lernend on the long dialog opens at once
    // (§7.1). Either way the reason is kept for the "why?" button.
    function showReason(reason) {
        table.lastReason = reason
        if (table.learnLevel > 0 && typeof reason !== "string") {
            whyDialog.show(reason)
            return
        }
        var head = table.reasonShort(reason)
        if (head === "")
            head = qsTr("Not allowed")
        noticeBox.head = head
        noticeBox.detail = table.reasonLong(reason)
        noticeBox.expanded = false
        noticeBox.visible = true
        noticeTimer.restart()
    }

    // Allowed, but it costs a bonus (§7.3, Severity::Warning). The dialog asks
    // and repeats the action through actConfirmed() if the answer is "anyway".
    function onActionWarned(reason, type, a, b) {
        table.lastReason = reason
        whyDialog.warn(reason, type, a, b)
    }

    // The learning bar: the hint, the last refusal and the running count.
    function showWhy() {
        if (table.lastReason !== null && typeof table.lastReason !== "string")
            whyDialog.show(table.lastReason)
        else
            whyDialog.show({ "short": qsTr("Nothing was refused yet"),
                             "text": qsTr("Tap a card the rules bar and the reason appears here.") })
    }

    // After the hand the review opens by itself, over the score sheet (§7.5).
    function onHandFinished() {
        if (table.learnLevel > 0 && !table.lessonMode)
            debriefPanel.open()
    }

    // An option the engine marked as not available; its reason is the hint the
    // learning mode will expand on.
    function explainOption(option) {
        if (!option)
            return
        if (option.hintText || option.hint || option.reasonKey) {
            table.showReason({ "short": option.hint ? option.hint : option.reasonKey,
                               "text": option.hintText ? option.hintText : "" })
        } else {
            table.showReason(qsTr("Not available now"))
        }
    }

    function actOption(option) {
        if (!option)
            return
        if (option.enabled === false) {
            table.explainOption(option)
            return
        }
        var a = option.a === undefined ? -1 : option.a
        var b = option.b === undefined ? -1 : option.b
        engine.act(option.type, a, b)
    }

    function optionLabel(option) {
        if (!option)
            return ""
        if (option.speech)
            return option.speech
        return option.label ? option.label : ""
    }

    // The action band follows the phase (Deal 0, Bidding 1, Call 2, Talon 3,
    // Discard 4, Announce 5, Play 6, Scoring 7, HandOver 8).
    function bandComponent() {
        if (!engine.active || engine.handOver || table.meSittingOut)
            return null
        var phase = engine.phase
        if (phase === 3)
            return talonComponent
        if (phase === 4)
            return discardComponent
        if (phase === 5)
            return announceComponent
        if (phase === 6 && (engine.bonusOptions.length > 0 || engine.kontraTargets.length > 0))
            return announceComponent
        if (engine.options.length > 0)
            return bidComponent
        return null
    }

    Component.onCompleted: {
        engine.cardAnimationRequested.connect(onCardFlight)
        engine.trickAnimationRequested.connect(onTrickFlight)
        engine.revealRequested.connect(onReveal)
        engine.resetVisuals.connect(onReset)
        engine.speech.connect(onSpeech)
        engine.actionRejected.connect(showReason)
        engine.matchStarted.connect(onReset)
        // Older engine stubs do not have the warning yet.
        if (engine.actionWarned)
            engine.actionWarned.connect(onActionWarned)
        if (engine.handFinished)
            engine.handFinished.connect(onHandFinished)
    }
    Component.onDestruction: {
        engine.cardAnimationRequested.disconnect(onCardFlight)
        engine.trickAnimationRequested.disconnect(onTrickFlight)
        engine.revealRequested.disconnect(onReveal)
        engine.resetVisuals.disconnect(onReset)
        engine.speech.disconnect(onSpeech)
        engine.actionRejected.disconnect(showReason)
        engine.matchStarted.disconnect(onReset)
        if (engine.actionWarned)
            engine.actionWarned.disconnect(onActionWarned)
        if (engine.handFinished)
            engine.handFinished.disconnect(onHandFinished)
    }

    Timer {
        id: trickLeadIn
        interval: 260
        onTriggered: table.startTrickFlight()
    }

    Timer {
        id: revealTimer
        interval: 1400
        onTriggered: table.finishReveal()
    }

    Timer {
        id: noticeTimer
        interval: 5000
        onTriggered: noticeBox.visible = false
    }

    // --- header -------------------------------------------------------------------

    Item {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: headerColumn.height + 2 * Style.paddingSmall

        Rectangle {
            anchors.fill: parent
            color: table.feltEdgeColor
        }

        Column {
            id: headerColumn
            anchors.centerIn: parent
            width: parent.width - 2 * Style.horizontalPageMargin
            spacing: 2

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                font.pixelSize: Style.fontSizeSmall
                color: Style.highlightColor
                text: {
                    var parts = [table.engine.profileName]
                    if (table.engine.contractName !== "")
                        parts.push(table.engine.contractName)
                    if (table.engine.calledCard !== "")
                        parts.push(qsTr("called: %1").arg(table.engine.calledCard))
                    return parts.join("  ·  ")
                }
            }

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                font.pixelSize: Style.fontSizeTiny
                color: Style.secondaryColor
                text: {
                    var parts = []
                    if (table.engine.handsPerMatch > 0)
                        parts.push(qsTr("Hand %1/%2").arg(table.engine.handNumber)
                                   .arg(table.engine.handsPerMatch))
                    if (table.engine.trickNumber > 0)
                        parts.push(qsTr("Trick %1/%2").arg(table.engine.trickNumber)
                                   .arg(table.engine.tricksTotal))
                    var count = table.countText()
                    if (count !== "")
                        parts.push(count)
                    return parts.join("  ·  ")
                }
            }
        }
    }

    // The running card count of both parties, as far as the engine reveals it.
    function countText() {
        var live = engine.liveCount
        if (!live)
            return ""
        if (live.declarer === undefined && live.defenders === undefined)
            return ""
        var mine = live.declarer === undefined ? 0 : live.declarer
        var theirs = live.defenders === undefined ? 0 : live.defenders
        return qsTr("Count %1 : %2").arg(mine).arg(theirs)
    }

    // --- learning mode ---------------------------------------------------------------

    // The explanation bar of §7.2 sits between the header and the seats; with
    // the learning mode off it is zero high and the table is untouched.
    LearnPanel {
        id: learnPanel
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        maxHeight: table.height * 0.28
        learn: table.learn
        engine: table.engine
        panelColor: table.panelColor
        onOpenAnchor: table.openRule(a1)
        onOpenTerm: table.openGlossary(a1)
    }

    // The running lesson floats over the seats rather than displacing them.
    LessonOverlay {
        id: lessonOverlay
        anchors.top: learnPanel.bottom
        anchors.topMargin: Style.paddingSmall
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Style.horizontalPageMargin
        anchors.rightMargin: Style.horizontalPageMargin
        maxHeight: table.height * 0.34
        learn: table.learn
        engine: table.engine
        deck: table.engine.deck
        cardRatio: table.cardRatio
        panelColor: table.panelColor
        goodColor: table.partnerColor
    }

    // --- seats --------------------------------------------------------------------

    SeatPanel {
        id: topSeat
        // Zu viert sitzt der dritte gegenüber, zu zweit der einzige Gegner.
        property int acrossSeat: table.players === 2 ? 1 : 2
        visible: table.players === 4 || table.players === 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: learnPanel.bottom
        anchors.topMargin: Style.paddingSmall
        width: parent.width * 0.34
        engine: table.engine
        seat: topSeat.acrossSeat
        info: table.seatInfo(topSeat.acrossSeat)
        said: table.saidBySeat[topSeat.acrossSeat]
        deck: table.engine.deck
        cardRatio: table.cardRatio
        partnerColor: table.partnerColor
        opponentColor: table.opponentColor
    }

    SeatPanel {
        id: topLeftSeat
        visible: table.players === 5
        anchors.left: parent.left
        anchors.leftMargin: parent.width * 0.24
        anchors.top: learnPanel.bottom
        anchors.topMargin: Style.paddingSmall
        width: parent.width * 0.26
        engine: table.engine
        seat: 2
        info: table.seatInfo(2)
        said: table.saidBySeat[2]
        deck: table.engine.deck
        cardRatio: table.cardRatio
        partnerColor: table.partnerColor
        opponentColor: table.opponentColor
    }

    SeatPanel {
        id: topRightSeat
        visible: table.players === 5
        anchors.right: parent.right
        anchors.rightMargin: parent.width * 0.24
        anchors.top: learnPanel.bottom
        anchors.topMargin: Style.paddingSmall
        width: parent.width * 0.26
        engine: table.engine
        seat: 3
        info: table.seatInfo(3)
        said: table.saidBySeat[3]
        deck: table.engine.deck
        cardRatio: table.cardRatio
        partnerColor: table.partnerColor
        opponentColor: table.opponentColor
    }

    SeatPanel {
        id: leftSeat
        visible: table.players >= 3
        anchors.left: parent.left
        anchors.leftMargin: Style.paddingSmall
        anchors.top: learnPanel.bottom
        anchors.topMargin: Style.itemSizeSmall * 1.1
        width: parent.width * 0.3
        engine: table.engine
        seat: 1
        info: table.seatInfo(1)
        said: table.saidBySeat[1]
        deck: table.engine.deck
        cardRatio: table.cardRatio
        partnerColor: table.partnerColor
        opponentColor: table.opponentColor
    }

    SeatPanel {
        id: rightSeat
        visible: table.players >= 3
        anchors.right: parent.right
        anchors.rightMargin: Style.paddingSmall
        anchors.top: leftSeat.top
        width: parent.width * 0.3
        engine: table.engine
        // Bound, not assigned: the same table serves three, four and five
        // seats -- rechts sitzt der letzte Platz, und das ist je nachdem der
        // zweite, dritte oder vierte.
        seat: table.lastSeat
        info: table.seatInfo(table.lastSeat)
        said: table.saidBySeat[table.lastSeat]
        deck: table.engine.deck
        cardRatio: table.cardRatio
        partnerColor: table.partnerColor
        opponentColor: table.opponentColor
    }

    // --- Strohmänner (strohmandeln.md §4) ------------------------------------------

    StrawmanRow {
        id: opponentStrawmen
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: table.players === 2 ? topSeat.bottom : leftSeat.bottom
        anchors.topMargin: Style.paddingSmall
        width: parent.width * 0.86
        engine: table.engine
        packets: table.strawmenOf(1)
        taken: table.strawmenTakenOf(1)
        own: false
        deck: table.engine.deck
        cardRatio: table.cardRatio
        maxCardHeight: table.height * 0.11
        panelColor: table.panelColor
    }

    StrawmanRow {
        id: myStrawmen
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: statusLabel.top
        anchors.bottomMargin: Style.paddingSmall
        width: parent.width * 0.86
        engine: table.engine
        packets: table.strawmenOf(0)
        // Die eigenen aufgenommenen Karten stehen schon im Fächer; noch einmal
        // hier wären sie nur doppelt und nähmen dem Stich den Platz weg.
        own: true
        interactive: !table.meSittingOut
        deck: table.engine.deck
        cardRatio: table.cardRatio
        maxCardHeight: table.height * 0.11
        panelColor: table.panelColor
        onExplain: table.showReason(a1)
    }

    // --- trick --------------------------------------------------------------------

    TrickArea {
        id: trickArea
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: table.hasStrawmen ? opponentStrawmen.bottom
                                       : (table.players === 2 ? topSeat.bottom : leftSeat.bottom)
        anchors.topMargin: Style.paddingMedium
        anchors.bottom: table.hasStrawmen ? myStrawmen.top : statusLabel.top
        anchors.bottomMargin: Style.paddingSmall
        width: parent.width * 0.88
        engine: table.engine
        cards: table.engine.trick
        players: table.players
        hiddenIds: table.hiddenIds
        deck: table.engine.deck
        cardRatio: table.cardRatio
        feltColor: table.feltEdgeColor
    }

    Text {
        id: statusLabel
        anchors.bottom: actionBand.top
        anchors.bottomMargin: Style.paddingSmall
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Style.horizontalPageMargin
        anchors.rightMargin: Style.horizontalPageMargin
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        font.pixelSize: Style.fontSizeSmall
        color: table.engine.myTurn ? Style.highlightColor : Style.primaryColor
        text: table.meSittingOut ? qsTr("You sit this hand out.") : table.engine.status
    }

    // --- action band -------------------------------------------------------------------

    // The Loader takes its width from the anchors and its height from the
    // implicit height of the panel it holds; the panels never set a height of
    // their own, or the two would chase each other.
    // The hint of §7.4, above the action band so it never covers the hand.
    HintBubble {
        id: hintBubble
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Style.horizontalPageMargin
        anchors.rightMargin: Style.horizontalPageMargin
        anchors.bottom: actionBand.top
        anchors.bottomMargin: Style.paddingSmall
        z: 500
        learn: table.learn
        engine: table.engine
        panelColor: table.panelColor
        maxHeight: table.height * 0.26
        // Only Neuling offers the hint unasked, and only there with the
        // alternatives (§7.1); the switch of §7.8 can turn that off.
        autoShow: table.learnLevel >= 2 && table.engine.myTurn
                  && (table.learn === null || table.learn.autoHint !== false)
        showAlternatives: table.learnLevel >= 2
        goodColor: table.partnerColor
        badColor: table.opponentColor
    }

    Loader {
        id: actionBand
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: mySeat.top
        anchors.bottomMargin: Style.paddingSmall
        sourceComponent: table.bandComponent()
    }

    Component {
        id: bidComponent
        BidBar {
            engine: table.engine
            options: table.engine.options
            headline: table.saidBySeat[table.engine.actor >= 0 ? table.engine.actor : 0]
            panelColor: table.panelColor
            onExplain: table.explainOption(a1)
        }
    }

    Component {
        id: talonComponent
        TalonPanel {
            engine: table.engine
            halves: table.engine.talonHalves
            options: table.engine.options
            deck: table.engine.deck
            cardRatio: table.cardRatio
            panelColor: table.panelColor
            onExplain: table.explainOption(a1)
        }
    }

    Component {
        id: discardComponent
        DiscardTray {
            engine: table.engine
            cards: table.engine.discardTray
            openIds: table.idSet(table.engine.openDiscards)
            target: table.engine.discardTarget
            deck: table.engine.deck
            cardRatio: table.cardRatio
            panelColor: table.panelColor
            onExplain: table.explainOption(a1)
        }
    }

    Component {
        id: announceComponent
        AnnouncePanel {
            engine: table.engine
            bonusOptions: table.engine.bonusOptions
            kontraTargets: table.engine.kontraTargets
            options: table.engine.options
            announcements: table.me.announcements
            maxHeight: table.height * 0.34
            panelColor: table.panelColor
            onExplain: table.explainOption(a1)
        }
    }

    // --- the local player ------------------------------------------------------------

    SeatPanel {
        id: mySeat
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: Style.horizontalPageMargin
        anchors.rightMargin: Style.horizontalPageMargin
        anchors.bottom: handFan.top
        anchors.bottomMargin: Style.paddingSmall
        compact: true
        engine: table.engine
        seat: 0
        info: table.me
        said: table.saidBySeat[0]
        deck: table.engine.deck
        cardRatio: table.cardRatio
        partnerColor: table.partnerColor
        opponentColor: table.opponentColor
    }

    HandFan {
        id: handFan
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: learnBar.top
        anchors.bottomMargin: Style.paddingSmall
        visible: !table.meSittingOut
        engine: table.engine
        cards: table.engine.hand
        chosenIds: table.trayIds
        discarding: table.engine.phase === 4
        deck: table.engine.deck
        cardRatio: table.cardRatio
        maxCardHeight: table.height * 0.24
        onExplain: table.showReason(a1)
    }

    // The learning bar of §6.2 closes the table off at the bottom; off it is
    // zero high, so the hand fan reaches the edge as before.
    Item {
        id: learnBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        visible: table.learnLevel > 0
        height: visible ? learnRow.height + Style.paddingSmall : 0

        Row {
            id: learnRow
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            spacing: Style.paddingSmall

            TableButton {
                text: qsTr("Hint")
                onClicked: hintBubble.ask()
            }

            TableButton {
                text: qsTr("Why?")
                onClicked: table.showWhy()
            }

            TableButton {
                text: qsTr("Standing")
                onClicked: standingPanel.open()
            }

            TableButton {
                visible: table.engine.handOver
                text: qsTr("Review")
                onClicked: debriefPanel.open()
            }
        }
    }

    // --- overlays ----------------------------------------------------------------------

    // "Why is that not allowed?" and the warning of §7.3.
    WhyDialog {
        id: whyDialog
        anchors.fill: parent
        learn: table.learn
        engine: table.engine
        deck: table.engine.deck
        cardRatio: table.cardRatio
        panelColor: table.panelColor
        onOpenAnchor: table.openRule(a1)
    }

    // The running helpers of §7.5.
    StandingPanel {
        id: standingPanel
        anchors.fill: parent
        learn: table.learn
        engine: table.engine
        panelColor: table.panelColor
    }

    // The review after the hand, over the score sheet (§7.5).
    DebriefPanel {
        id: debriefPanel
        anchors.fill: parent
        learn: table.learn
        engine: table.engine
        panelColor: table.panelColor
    }

    ScoreSheet {
        id: scoreSheet
        anchors.centerIn: parent
        width: parent.width - 2 * Style.paddingMedium
        maxHeight: table.height - 2 * Style.paddingLarge
        visible: table.engine.handOver
        z: 450
        engine: table.engine
        seats: table.seatList
        panelColor: table.panelColor
        onBeforeContinue: table.clearFlights()
    }

    // Talon or open discards, held for a moment and then acknowledged.
    Rectangle {
        id: revealBox
        property string what: ""
        visible: table.revealActive
        z: 600
        anchors.centerIn: parent
        width: parent.width - 2 * Style.horizontalPageMargin
        height: revealColumn.height + 2 * Style.paddingLarge
        radius: Style.paddingSmall
        color: table.panelColor
        border.color: Style.highlightColor

        function revealCards() {
            if (what === "skart")
                return table.engine.openDiscards
            var all = []
            var halves = table.engine.talonHalves
            for (var i = 0; i < halves.length; ++i) {
                var part = halves[i] && halves[i].cards !== undefined ? halves[i].cards : halves[i]
                for (var j = 0; j < part.length; ++j)
                    all.push(part[j])
            }
            return all
        }

        Column {
            id: revealColumn
            anchors.centerIn: parent
            width: parent.width - 2 * Style.paddingMedium
            spacing: Style.paddingMedium

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Style.fontSizeSmall
                color: Style.highlightColor
                text: revealBox.what === "skart" ? qsTr("Laid open") : qsTr("The talon")
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: Style.paddingSmall
                Repeater {
                    model: revealBox.visible ? revealBox.revealCards() : []
                    Card {
                        width: Math.min(Style.itemSizeSmall * 0.9,
                                        (revealColumn.width - 5 * Style.paddingSmall) / 6)
                        height: width * table.cardRatio
                        cardId: table.cardIdOf(modelData)
                        deck: table.engine.deck
                        faceUp: table.cardIdOf(modelData) >= 0
                    }
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: table.finishReveal()
        }
    }

    // Short reason of a refused action; a tap opens the long text, which the
    // learning mode of M6 replaces with its own dialog.
    Rectangle {
        id: noticeBox
        property string head: ""
        property string detail: ""
        property bool expanded: false

        visible: false
        z: 900
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: learnPanel.bottom
        anchors.topMargin: Style.paddingLarge
        width: parent.width - 2 * Style.horizontalPageMargin
        height: noticeColumn.height + 2 * Style.paddingMedium
        radius: Style.paddingSmall
        color: table.panelColor
        border.color: Style.highlightColor

        Column {
            id: noticeColumn
            anchors.centerIn: parent
            width: parent.width - 2 * Style.paddingMedium
            spacing: Style.paddingSmall

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                font.pixelSize: Style.fontSizeSmall
                color: Style.highlightColor
                text: noticeBox.head
            }

            Text {
                width: parent.width
                visible: noticeBox.expanded && noticeBox.detail !== ""
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                font.pixelSize: Style.fontSizeExtraSmall
                color: Style.primaryColor
                text: noticeBox.detail
            }

            Text {
                width: parent.width
                visible: !noticeBox.expanded && noticeBox.detail !== ""
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Style.fontSizeTiny
                color: Style.secondaryColor
                text: qsTr("Why not?")
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (table.learnLevel > 0 && table.lastReason !== null) {
                    noticeBox.visible = false
                    whyDialog.show(table.lastReason)
                } else if (noticeBox.detail !== "" && !noticeBox.expanded) {
                    noticeBox.expanded = true
                    noticeTimer.restart()
                } else {
                    noticeBox.visible = false
                }
            }
        }
    }

    // --- the guided tour (docs/design.md §7.9) ------------------------------------------

    TourScript {
        id: tourScript
    }

    TourOverlay {
        id: tourOverlay
        anchors.fill: parent
        steps: tourScript.steps
        onResolve: table.tourTarget(a1)
        panelColor: table.panelColor
        maxHeight: table.height * 0.42
        onFinished: table.tourFinished()
    }

    Item {
        id: animationLayer
        anchors.fill: parent
        z: 1000
    }

    Component {
        id: flyingComponent
        FlyingCard { }
    }
}
