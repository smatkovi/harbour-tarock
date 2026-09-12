import QtQuick
import QtQuick.Controls

// Body text inside a SubPage, indented like the rest of the page content.
Label {
    x: Theme.horizontalPageMargin
    width: parent ? parent.width - 2 * Theme.horizontalPageMargin : implicitWidth
    wrapMode: Text.WordWrap
    font.pixelSize: Theme.fontSizeSmall
}
