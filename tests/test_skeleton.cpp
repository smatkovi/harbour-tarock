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

    M0 smoke test: the engine placeholder builds, links and answers the three
    properties the empty table binds to. The real test suite of
    docs/design.md §11 starts with test_cards in M1.
*/
#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QTemporaryDir>

#include <cstdio>

#include "TarockEngine.h"

static int failures = 0;

static void check(bool condition, const char *what)
{
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    // The same identity main.cpp installs; every QSettings in the app is the
    // default-constructed one and needs it.
    QCoreApplication::setOrganizationName(QStringLiteral("harbour-tarock"));
    QCoreApplication::setApplicationName(QStringLiteral("harbour-tarock"));
    // Never touch the settings of the person running the test: the engine
    // saves as soon as a match starts.
    QTemporaryDir settingsDir;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settingsDir.path());

    TarockEngine engine;
    check(!engine.active(), "a fresh engine has no running match");
    check(!engine.profileName().isEmpty(), "the profile name is displayable");
    // Since M3 the status line belongs to a running hand: without a match
    // there is nothing to say, and the table shows its own text instead.
    check(engine.status().isEmpty(), "a fresh engine says nothing about a hand");
    engine.startMatch(engine.profileKey(), 4);
    check(engine.active(), "startMatch() starts a match");
    check(!engine.status().isEmpty(), "the status line is displayable once a hand runs");

    if (failures == 0)
        std::printf("test_skeleton: ok\n");
    return failures == 0 ? 0 : 1;
}
