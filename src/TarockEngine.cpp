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
#include "TarockEngine.h"

TarockEngine::TarockEngine(QObject *parent)
    : QObject(parent)
{
}

QString TarockEngine::profileName() const
{
    // The profile registry (docs/design.md §4.3) picks this name from M1 on.
    return tr("Königrufen");
}

QString TarockEngine::status() const
{
    return tr("The table is set — dealing starts in a later milestone.");
}
