import QtQuick 1.1
import com.nokia.meego 1.0
// Scrollable page with a back button, used for everything but the table.
// The Sailfish OS and Android builds have a counterpart of the same name.
//
// The Android version leans on QtQuick.Controls' Page: a "background"
// property, a "header" that takes a ToolBar, and a "title". A MeeGo Page has
// none of the three -- it offers "tools" with a ToolBarLayout instead -- so
// the bar is an ordinary Item anchored at the top and the content is anchored
// below it. That keeps the same look without the platform's own tool bar,
// which would sit at the bottom and cover the table.
Page {
    id: page
    default property alias content: column.data
    property string title: ""

    Rectangle {
        anchors.fill: parent
        color: AppTheme.tableColor
        z: -1
    }

    Item {
        id: bar
        anchors { left: parent.left; right: parent.right; top: parent.top }
        height: AppTheme.itemSizeSmall

        Rectangle { anchors.fill: parent; color: AppTheme.barColor }

        Row {
            anchors.fill: parent
            spacing: AppTheme.paddingSmall
            ToolButton {
                text: "←"
                font.pixelSize: AppTheme.fontSizeLarge
                onClicked: page.pageStack.pop()
            }
            Label {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - x - AppTheme.paddingLarge
                text: page.title
                font.pixelSize: AppTheme.fontSizeMedium
                elide: Text.ElideRight
            }
        }
    }

    Flickable {
        // Ohne diese Bremse nimmt die MouseArea die Berührung
        // sofort an, und die kleinste Fingerbewegung reißt sie an die
        // Flickable -- der Tipp geht dann verloren (MainPage.qml macht
        // es seit jeher so).
        pressDelay: 150
        anchors {
            left: parent.left; right: parent.right
            top: bar.bottom; bottom: parent.bottom
        }
        contentHeight: column.height + column.y + AppTheme.paddingLarge
        clip: true

        Column {
            id: column
            // QtQuick 1.1 positioners have no padding properties.
            y: AppTheme.paddingMedium
            width: page.width
            spacing: AppTheme.paddingMedium
        }
    }
}
