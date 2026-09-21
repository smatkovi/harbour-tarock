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

// Root of the MeeGo Harmattan edition. The Android build uses an
// ApplicationWindow with a StackView; the Harmattan components bring their
// own PageStackWindow, which is the same idea and also supplies the status
// bar and the tool bar the platform expects.
//
// Style, Prefs and Theme are not imported here: meego/main.cpp instantiates
// them and puts them in the root context under those names, because QtQuick
// 1.1 has no singletons. That is what lets the QML shared with Sailfish and
// Android keep writing Style.paddingMedium unchanged.
PageStackWindow {
    id: app

    initialPage: MainPage { }
    showStatusBar: true
    showToolBar: true

    // The N9 has no back button of its own; the tool bar carries one and the
    // hardware key is the camera shutter, so nothing to intercept here.
    Component.onCompleted: theme.inverted = true
}
