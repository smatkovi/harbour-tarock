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
// The rule reference of docs/design.md §6.1 and §7.7; the counterpart of
// sailfish/RulesPage.qml. Every reason and every "more on this" carries an
// anchor of the form "rules:at-kr-ooe#6.2"; the page takes it in `anchor` and
// opens that chapter at the top. The chapters come from the engine's rule
// index (rules/*.de.md); as long as it is empty the page names the chapter and
// the specification it stands in.
SubPage {
    id: page
    objectName: "rulesPage"
    title: qsTr("Rules")

    property variant engine: tarockEngine
    // LearnEngine reaches QML as a property of the engine; if a platform
    // installs it as its own context property, this one line changes.
    property variant learn: engine.learn === undefined ? null : engine.learn

    // "rules:<document>#<section>", as it stands in Reason.anchor.
    property string anchor: ""
    property string openAnchor: page.anchor
    property string filter: ""

    property variant chapters: page.ruleChapters()
    property variant opened: page.chapterFor(page.anchor)

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

    function chapterFor(value) {
        if (!value)
            return null
        var list = page.chapters
        for (var i = 0; i < list.length; ++i) {
            if (page.sameAnchor(list[i].anchor, value))
                return list[i]
        }
        return null
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

    // The chapter a "more on this" pointed at, opened at the top.
    Rectangle {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        height: openedColumn.height + 2 * Theme.paddingMedium
        visible: page.anchor !== ""
        radius: Theme.paddingSmall
        color: "transparent"
        border.color: Theme.highlightColor

        Column {
            id: openedColumn
            anchors.centerIn: parent
            width: parent.width - 2 * Theme.paddingMedium

            Label {
                width: parent.width
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeSmall
                color: Theme.highlightColor
                text: page.opened ? page.chapterTitle(page.opened)
                                  : qsTr("Chapter %1 of %2").arg(page.sectionOf(page.anchor))
                                    .arg(page.documentOf(page.anchor))
            }
            Label {
                width: parent.width
                visible: text !== ""
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
                text: page.opened && page.opened.text ? page.opened.text : ""
            }
        }
    }

    TextBlock {
        text: qsTr("Every rule the app enforces is written down in one of the specifications below, section by section.")
        color: Theme.secondaryColor
    }

    TextField {
        x: Theme.horizontalPageMargin
        width: parent.width - 2 * Theme.horizontalPageMargin
        visible: page.chapters.length > 0
        placeholderText: qsTr("Search the rules")
        font.pixelSize: Theme.fontSizeSmall
        onTextChanged: page.filter = text.trim()
    }

    SectionLabel {
        text: qsTr("Chapters")
        visible: page.chapters.length > 0
    }

    Repeater {
        model: page.chapters

        ItemDelegate {
            id: chapterItem
            property bool open: page.sameAnchor(modelData.anchor, page.openAnchor)
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            visible: page.matches(modelData)
            height: visible ? implicitHeight : 0
            highlighted: chapterItem.open
            onClicked: page.openAnchor = chapterItem.open ? "" : modelData.anchor

            contentItem: Column {
                Label {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeSmall
                    color: chapterItem.open ? Theme.highlightColor : Theme.primaryColor
                    text: page.chapterTitle(modelData)
                }
                Label {
                    width: parent.width
                    visible: chapterItem.open && text !== ""
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                    text: modelData.text ? modelData.text : ""
                }
            }
        }
    }

    SectionLabel { text: qsTr("Profiles") }

    Repeater {
        model: page.engine.profileKeys()

        ItemDelegate {
            x: Theme.horizontalPageMargin
            width: parent.width - 2 * Theme.horizontalPageMargin
            highlighted: modelData === Prefs.profileKey
            onClicked: Prefs.profileKey = modelData

            contentItem: Column {
                Label {
                    width: parent.width
                    text: page.engine.profileNameFor(modelData)
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeSmall
                    color: modelData === Prefs.profileKey ? Theme.highlightColor
                                                          : Theme.primaryColor
                }
                Label {
                    width: parent.width
                    text: modelData
                    wrapMode: Text.WordWrap
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                }
            }
        }
    }

    SectionLabel { text: qsTr("Specifications") }

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
                font.pixelSize: Theme.fontSizeSmall
            }
            Label {
                width: parent.width
                text: modelData.source
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.secondaryColor
            }
        }
    }

    Button {
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Glossary")
        onClicked: page.pageStack.push(Qt.resolvedUrl("GlossaryPage.qml"))
    }
}
