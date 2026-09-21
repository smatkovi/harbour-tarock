import QtQuick 1.1
import com.nokia.meego 1.0
// Body text inside a SubPage, indented like the rest of the page content.
Label {
    x: AppTheme.horizontalPageMargin
    width: parent ? parent.width - 2 * AppTheme.horizontalPageMargin : implicitWidth
    wrapMode: Text.WordWrap
    font.pixelSize: AppTheme.fontSizeSmall
}
