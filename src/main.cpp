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
#include <sailfishapp.h>

#include <QGuiApplication>
#include <QLocale>
#include <QQmlContext>
#include <QQuickView>
#include <QTranslator>

#include "TarockEngine.h"

// Sailfish OS entry point. The Android build has its own main.cpp because it
// uses QQmlApplicationEngine and a QML module instead of SailfishApp.
int main(int argc, char *argv[])
{
    QGuiApplication *app = SailfishApp::application(argc, argv);
    QQuickView *view = SailfishApp::createView();

    QTranslator translator;
    const QString translationDirectory = SailfishApp::pathTo(
        QStringLiteral("translations")).toLocalFile();
    if (translator.load(QLocale(), QStringLiteral("harbour-tarock"),
                        QStringLiteral("-"), translationDirectory)) {
        app->installTranslator(&translator);
    }

    // learnEngine, lanBrowser and appSettings join this list in M5/M7
    // (docs/design.md §3.1).
    view->rootContext()->setContextProperty(QStringLiteral("tarockEngine"),
                                            new TarockEngine(app));

    view->setSource(SailfishApp::pathTo(QStringLiteral("qml/harbour-tarock.qml")));
    view->show();
    return app->exec();
}
