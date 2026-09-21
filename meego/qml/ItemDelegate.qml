import QtQuick 1.1

// QtQuick.Controls' ItemDelegate; com.nokia.meego has no counterpart. Only
// what the rules, glossary and tutorial pages use: a tappable row that takes
// its height from whatever is assigned to contentItem, with a highlight.
//
// contentItem is assigned an inline object, which QML parents to this item
// but does not lay out, so it is moved into the holder and its width bound
// there. QtQuick 1.1 has no Qt.binding(), hence the Binding element.
Item {
    id: root

    property Item contentItem
    property bool highlighted: false
    signal clicked()

    implicitHeight: contentItem ? contentItem.height + 2 * Theme.paddingSmall : 0

    Rectangle {
        anchors.fill: parent
        color: Theme.highlightColor
        opacity: root.highlighted ? 0.18 : (area.pressed ? 0.28 : 0)
        Behavior on opacity { NumberAnimation { duration: 100 } }
    }

    Item {
        id: holder
        anchors.fill: parent
        anchors.margins: Theme.paddingSmall
    }

    Binding { target: root.contentItem; property: "width"; value: holder.width }

    onContentItemChanged: if (contentItem) contentItem.parent = holder

    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
