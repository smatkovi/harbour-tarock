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

// The glossary of docs/design.md §7.7; the counterpart of
// sailfish/GlossaryPage.qml. A term tapped in an explanation arrives in `term`
// and is opened at once.
SubPage {
    id: page
    objectName: "glossaryPage"
    title: qsTr("Glossary")

    property var engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn

    property string term: ""
    property string filter: ""
    property string openTerm: page.term

    readonly property var entries: page.glossary()

    // LearnEngine::glossary() is rules/glossary.de.tsv of the active profile:
    // { term, definition, anchor, synonyms }.
    function glossary() {
        if (page.learn === null)
            return []
        if (typeof page.learn.glossary === "function") {
            var list = page.learn.glossary()
            if (list && list.length > 0)
                return list
        }
        // Before the index is built the page still shows the terms the current
        // explanation carries (explanation.terms of §7.2).
        var explanation = page.learn.explanation
        if (explanation && explanation.terms)
            return explanation.terms
        return []
    }

    function matches(entry) {
        if (page.filter === "")
            return true
        var needle = page.filter.toLowerCase()
        var fields = [entry.term, entry.definition, entry.synonyms]
        for (var i = 0; i < fields.length; ++i) {
            if (fields[i] && String(fields[i]).toLowerCase().indexOf(needle) >= 0)
                return true
        }
        return false
    }

    function choose(entry) {
        page.openTerm = page.openTerm === entry.term ? "" : entry.term
        // The engine may want to put the term into the explanation panel too.
        if (page.learn !== null && page.openTerm !== "")
            page.learn.explainTerm(entry.term)
    }

    Component.onCompleted: {
        if (page.term !== "" && page.learn !== null)
            page.learn.explainTerm(page.term)
    }

    TextField {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        placeholderText: qsTr("Search term")
        font.pixelSize: Theme.fontSizeSmall
        onTextChanged: page.filter = text.trim()
    }

    TextBlock {
        visible: page.entries.length === 0
        text: qsTr("The glossary is not loaded yet.")
        color: Theme.secondaryColor
        font.pixelSize: Theme.fontSizeExtraSmall
    }

    Repeater {
        model: page.entries

        ItemDelegate {
            id: delegate
            readonly property bool open: modelData.term === page.openTerm
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            visible: page.matches(modelData)
            height: visible ? implicitHeight : 0
            highlighted: delegate.open
            onClicked: page.choose(modelData)

            contentItem: Column {
                Label {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeSmall
                    color: delegate.open ? Theme.highlightColor : Theme.primaryColor
                    text: modelData.term ? modelData.term : String(modelData)
                }
                Label {
                    width: parent.width
                    visible: delegate.open && text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                    text: modelData.definition ? modelData.definition : ""
                }
                Label {
                    width: parent.width
                    visible: delegate.open && modelData.anchor !== undefined
                             && modelData.anchor !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.highlightColor
                    text: qsTr("More on this")

                    MouseArea {
                        anchors.fill: parent
                        onClicked: page.StackView.view.push(Qt.resolvedUrl("RulesPage.qml"),
                                                            { "anchor": modelData.anchor })
                    }
                }
            }
        }
    }
}
