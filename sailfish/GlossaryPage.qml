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

// The glossary of docs/design.md §7.7: term, short definition and the rule
// anchor behind it, from rules/glossary.de.tsv through the engine. A term
// tapped in an explanation arrives here in `term` and is opened at once.
Page {
    id: page
    objectName: "glossaryPage"
    allowedOrientations: Orientation.All

    property var engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn

    // The term the explanation panel asked about.
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

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge
        VerticalScrollDecorator { }

        Column {
            id: content
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader { title: qsTr("Glossary") }

            SearchField {
                width: parent.width
                placeholderText: qsTr("Search term")
                onTextChanged: page.filter = text.trim()
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                visible: page.entries.length === 0
                wrapMode: Text.WordWrap
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeExtraSmall
                text: qsTr("The glossary is not loaded yet.")
            }

            Repeater {
                model: page.entries

                ListItem {
                    width: parent.width
                    visible: page.matches(modelData)
                    height: visible ? contentHeight : 0
                    contentHeight: entry.height + 2 * Theme.paddingSmall
                    highlighted: modelData.term === page.openTerm
                    onClicked: page.choose(modelData)

                    Column {
                        id: entry
                        x: Theme.horizontalPageMargin
                        width: parent.width - 2 * Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter

                        Label {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            text: modelData.term ? modelData.term : String(modelData)
                            color: modelData.term === page.openTerm ? Theme.highlightColor
                                                                    : Theme.primaryColor
                        }
                        Label {
                            width: parent.width
                            visible: modelData.term === page.openTerm && text !== ""
                            wrapMode: Text.WordWrap
                            color: Theme.secondaryColor
                            font.pixelSize: Theme.fontSizeExtraSmall
                            text: modelData.definition ? modelData.definition : ""
                        }
                        Label {
                            width: parent.width
                            visible: modelData.term === page.openTerm
                                     && modelData.anchor !== undefined && modelData.anchor !== ""
                            wrapMode: Text.WordWrap
                            color: Theme.highlightColor
                            font.pixelSize: Theme.fontSizeExtraSmall
                            text: qsTr("More on this")

                            MouseArea {
                                anchors.fill: parent
                                onClicked: pageStack.push(Qt.resolvedUrl("RulesPage.qml"),
                                                          { "anchor": modelData.anchor })
                            }
                        }
                    }
                }
            }
        }
    }

    // A term that came from an explanation opens straight away.
    Component.onCompleted: {
        if (page.term !== "" && page.learn !== null)
            page.learn.explainTerm(page.term)
    }
}
