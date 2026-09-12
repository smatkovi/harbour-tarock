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
import QtQuick 2.6
import Sailfish.Silica 1.0

ApplicationWindow {
    id: app

    // main.cpp installs the one C++ engine instance as a root context
    // property; the alias keeps pages pushed at runtime on the same instance.
    property var engine: tarockEngine

    // CoverPage.qml (docs/design.md §6.1) follows with the running match in
    // M4; until then Sailfish shows the application icon as the cover.
    initialPage: Component { MainPage { } }
}
