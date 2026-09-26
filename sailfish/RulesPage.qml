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

// The rule reference of docs/design.md §6.1 and §7.7. Every reason and every
// "more on this" carries an anchor of the form "rules:at-kr-ooe#6.2"; the page
// takes it in `anchor`, opens that chapter and scrolls to it. The chapters
// themselves come from the engine's rule index (rules/*.de.md); as long as it
// is empty the page names the chapter and the specification it stands in.
Page {
    id: page
    objectName: "rulesPage"
    allowedOrientations: Orientation.All

    property var engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property var learn: engine.learn === undefined ? null : engine.learn

    // "rules:<document>#<section>", as it stands in Reason.anchor.
    property string anchor: ""
    property string openAnchor: page.anchor
    property string filter: ""

    readonly property var chapters: page.ruleChapters()

    // LearnEngine::chapters() is the rule index of the active profile:
    // { anchor, title, text }.
    function ruleChapters() {
        if (page.learn === null || typeof page.learn.chapters !== "function")
            return []
        var list = page.learn.chapters()
        return list ? list : []
    }

    // "rules:at-kr-ooe#6.2" → "6.2"; a bare "#6.2" works as well.
    function sectionOf(value) {
        if (!value)
            return ""
        var hash = String(value).indexOf("#")
        return hash < 0 ? String(value) : String(value).substring(hash + 1)
    }

    function documentOf(value) {
        if (!value)
            return ""
        var text = String(value)
        var colon = text.indexOf(":")
        var hash = text.indexOf("#")
        var start = colon < 0 ? 0 : colon + 1
        return hash < 0 ? text.substring(start) : text.substring(start, hash)
    }

    function sameAnchor(a, b) {
        if (!a || !b)
            return false
        return page.sectionOf(a) === page.sectionOf(b)
               && (page.documentOf(a) === "" || page.documentOf(b) === ""
                   || page.documentOf(a) === page.documentOf(b))
    }

    function matches(chapter) {
        if (page.filter === "")
            return true
        var needle = page.filter.toLowerCase()
        var fields = [chapter.title, chapter.text, chapter.anchor]
        for (var i = 0; i < fields.length; ++i) {
            if (fields[i] && String(fields[i]).toLowerCase().indexOf(needle) >= 0)
                return true
        }
        return false
    }

    function chapterTitle(chapter) {
        var section = page.sectionOf(chapter.anchor)
        if (chapter.title && section !== "")
            return section + "  " + chapter.title
        return chapter.title ? chapter.title : section
    }

    SilicaFlickable {
        id: flickable
        anchors.fill: parent
        contentHeight: content.height + Theme.paddingLarge
        VerticalScrollDecorator { }

        PullDownMenu {
            MenuItem {
                text: qsTr("Glossary")
                onClicked: pageStack.push(Qt.resolvedUrl("GlossaryPage.qml"))
            }
            MenuItem {
                text: qsTr("Lessons")
                onClicked: pageStack.push(Qt.resolvedUrl("LearnPage.qml"))
            }
        }

        Column {
            id: content
            width: page.width
            spacing: Theme.paddingMedium

            PageHeader {
                title: qsTr("Rules")
                description: page.engine.profileNameFor(Prefs.profileKey)
            }

            // The chapter a "more on this" pointed at, when the index cannot
            // resolve it yet.
            Item {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                height: missingLabel.height
                visible: page.anchor !== "" && page.chapters.length === 0

                Label {
                    id: missingLabel
                    width: parent.width
                    wrapMode: Text.WordWrap
                    color: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeExtraSmall
                    text: qsTr("Chapter %1 of %2").arg(page.sectionOf(page.anchor))
                          .arg(page.documentOf(page.anchor))
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                width: parent.width - 2 * Theme.horizontalPageMargin
                wrapMode: Text.WordWrap
                color: Theme.secondaryColor
                text: page.chapters.length > 0
                      ? qsTr("Every rule the app enforces is written down in one of the specifications below, section by section.")
                      : qsTr("For these rules the app does not carry the chapters yet -- they are in the specification in the source tree. The glossary below applies all the same.")
            }

            SearchField {
                width: parent.width
                visible: page.chapters.length > 0
                placeholderText: qsTr("Search the rules")
                onTextChanged: page.filter = text.trim()
            }

            SectionHeader {
                text: qsTr("Chapters")
                visible: page.chapters.length > 0
            }

            Repeater {
                model: page.chapters

                ListItem {
                    id: chapterItem
                    width: parent.width
                    visible: page.matches(modelData)
                    height: visible ? contentHeight : 0
                    contentHeight: chapterColumn.height + 2 * Theme.paddingSmall
                    // Not Silica's `highlighted` — see GlossaryPage.qml: a
                    // binding on it makes the item click itself, and the
                    // chapter would fold up again at once.
                    readonly property bool open: page.sameAnchor(modelData.anchor, page.openAnchor)
                    onClicked: page.openAnchor = chapterItem.open ? "" : modelData.anchor

                    // The anchor a reason pointed at is scrolled into view.
                    Component.onCompleted: {
                        if (page.anchor !== "" && page.sameAnchor(modelData.anchor, page.anchor))
                            flickable.contentY = Math.max(0, chapterItem.mapToItem(content, 0, 0).y
                                                          - Theme.paddingLarge)
                    }

                    Column {
                        id: chapterColumn
                        x: Theme.horizontalPageMargin
                        width: parent.width - 2 * Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter

                        Label {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            text: page.chapterTitle(modelData)
                            color: chapterItem.open ? Theme.highlightColor
                                                     : Theme.primaryColor
                        }
                        Label {
                            width: parent.width
                            visible: chapterItem.open && text !== ""
                            wrapMode: Text.WordWrap
                            color: Theme.secondaryColor
                            font.pixelSize: Theme.fontSizeExtraSmall
                            text: modelData.text ? modelData.text : ""
                        }
                    }
                }
            }

            SectionHeader { text: qsTr("Profiles") }

            Repeater {
                model: page.engine.profileKeys()

                ListItem {
                    width: parent.width
                    contentHeight: entry.height + 2 * Theme.paddingSmall
                    onClicked: Prefs.profileKey = modelData

                    Column {
                        id: entry
                        x: Theme.horizontalPageMargin
                        width: parent.width - 2 * Theme.horizontalPageMargin
                        anchors.verticalCenter: parent.verticalCenter

                        Label {
                            width: parent.width
                            text: page.engine.profileNameFor(modelData)
                            wrapMode: Text.WordWrap
                            color: modelData === Prefs.profileKey ? Theme.highlightColor
                                                                  : Theme.primaryColor
                        }
                        Label {
                            width: parent.width
                            text: modelData
                            color: Theme.secondaryColor
                            font.pixelSize: Theme.fontSizeExtraSmall
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }

            SectionHeader { text: qsTr("Specifications") }

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
                    }
                    Label {
                        width: parent.width
                        text: modelData.source
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeExtraSmall
                        wrapMode: Text.WordWrap
                    }
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Glossary")
                onClicked: pageStack.push(Qt.resolvedUrl("GlossaryPage.qml"))
            }
        }
    }
}
