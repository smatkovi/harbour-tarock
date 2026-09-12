import QtQuick
import QtQuick.Controls

// Section heading inside a SubPage; the counterpart of Silica's SectionHeader.
Label {
    x: Theme.horizontalPageMargin
    width: parent ? parent.width - 2 * Theme.horizontalPageMargin : implicitWidth
    topPadding: Theme.paddingMedium
    horizontalAlignment: Text.AlignRight
    font.pixelSize: Theme.fontSizeSmall
    color: Theme.highlightColor
}
