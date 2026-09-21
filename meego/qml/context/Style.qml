import QtQuick 1.1

// Neutral name for the metrics used by the QML shared with Sailfish OS
// (qml-common), which cannot use either platform's own Theme directly.
QtObject {
    property real paddingSmall: AppTheme.paddingSmall
    property real paddingMedium: AppTheme.paddingMedium
    property real paddingLarge: AppTheme.paddingLarge
    property real horizontalPageMargin: AppTheme.horizontalPageMargin
    property real itemSizeSmall: AppTheme.itemSizeSmall
    property real itemSizeMedium: AppTheme.itemSizeMedium
    property real itemSizeLarge: AppTheme.itemSizeLarge
    property real fontSizeTiny: AppTheme.fontSizeTiny
    property real fontSizeExtraSmall: AppTheme.fontSizeExtraSmall
    property real fontSizeSmall: AppTheme.fontSizeSmall
    property real fontSizeMedium: AppTheme.fontSizeMedium
    property real fontSizeLarge: AppTheme.fontSizeLarge
    property color primaryColor: AppTheme.primaryColor
    property color secondaryColor: AppTheme.secondaryColor
    property color highlightColor: AppTheme.highlightColor
    property color secondaryHighlightColor: AppTheme.secondaryHighlightColor
}
