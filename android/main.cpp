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
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QTranslator>

#include "ScreenHelper.h"
#include "TarockEngine.h"

// Android entry point. The Sailfish OS build has its own src/main.cpp; the
// engine and every rule of the game are shared between the two.
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    // Same settings location as the Sailfish build (AppConfigLocation).
    QCoreApplication::setOrganizationName(QStringLiteral("harbour-tarock"));
    QCoreApplication::setApplicationName(QStringLiteral("harbour-tarock"));
    QQuickStyle::setStyle(QStringLiteral("Material"));

    QTranslator translator;
    if (translator.load(QLocale(), QStringLiteral("harbour-tarock"), QStringLiteral("-"),
                        QStringLiteral(":/i18n")))
        app.installTranslator(&translator);

    // Declared before the QML engine so it outlives every binding to it.
    TarockEngine engine;
    // Keeps the screen on and the LAN foreground service alive from M7 on;
    // nothing switches it on yet.
    ScreenHelper screen;
    Q_UNUSED(screen)

    QQmlApplicationEngine qml;
    qml.rootContext()->setContextProperty(QStringLiteral("tarockEngine"), &engine);
    QObject::connect(&qml, &QQmlApplicationEngine::objectCreationFailed, &app,
                     []() { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    qml.loadFromModule("Tarock", "Main");
    return app.exec();
}
