import QtQuick 1.1

// Neutral name for the metrics used by the QML shared with Sailfish OS
// (qml-common), which cannot use either platform's own Theme directly.
QtObject {
    property real paddingSmall: Theme.paddingSmall
    property real paddingMedium: Theme.paddingMedium
    property real paddingLarge: Theme.paddingLarge
    property real horizontalPageMargin: Theme.horizontalPageMargin
    property real itemSizeSmall: Theme.itemSizeSmall
    property real itemSizeMedium: Theme.itemSizeMedium
    property real itemSizeLarge: Theme.itemSizeLarge
    property real fontSizeTiny: Theme.fontSizeTiny
    property real fontSizeExtraSmall: Theme.fontSizeExtraSmall
    property real fontSizeSmall: Theme.fontSizeSmall
    property real fontSizeMedium: Theme.fontSizeMedium
    property real fontSizeLarge: Theme.fontSizeLarge
    property color primaryColor: Theme.primaryColor
    property color secondaryColor: Theme.secondaryColor
    property color highlightColor: Theme.highlightColor
    property color secondaryHighlightColor: Theme.secondaryHighlightColor
}
