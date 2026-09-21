import QtQuick 1.1
import com.nokia.meego 1.0
// Section heading inside a SubPage; the counterpart of Silica's SectionHeader.
Label {
    x: AppTheme.horizontalPageMargin
    width: parent ? parent.width - 2 * AppTheme.horizontalPageMargin : implicitWidth
    y: AppTheme.paddingMedium   // QtQuick 1.1 has no padding here
    horizontalAlignment: Text.AlignRight
    font.pixelSize: AppTheme.fontSizeSmall
    color: AppTheme.highlightColor
}
