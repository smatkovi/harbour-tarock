import QtQuick 1.1
import com.nokia.meego 1.0
// Section heading inside a SubPage; the counterpart of Silica's SectionHeader.
Label {
    x: AppTheme.horizontalPageMargin
    width: parent ? parent.width - 2 * AppTheme.horizontalPageMargin : implicitWidth
    // Controls' Label has topPadding; a QtQuick 1.1 Text does not, so the
    // gap is made by height instead and the text sits at its bottom.
    height: implicitHeight + AppTheme.paddingMedium
    verticalAlignment: Text.AlignBottom
    horizontalAlignment: Text.AlignRight
    font.pixelSize: AppTheme.fontSizeSmall
    color: AppTheme.highlightColor
}
