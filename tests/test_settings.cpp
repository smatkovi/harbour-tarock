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

    Die Einstellungsdatei und das Kartenblatt (docs/design.md §10).

    Bis 0.3.x gab es nur gezeichnete Karten, und in jeder Einstellungsdatei
    stand darum "modern" -- gewählt hatte das niemand. Als 0.4.0 die Bilder
    brachte, hätte dieser Eintrag sie auf jedem Gerät verdeckt, auf dem die App
    schon einmal gelaufen war (auf der N950 tat er es auch). Geprüft wird
    darum:

      * ein Eintrag, den niemand gewählt hat, sticht die Voreinstellung nicht,
      * eine wirkliche Wahl bleibt erhalten -- auch die auf "modern",
      * und sie steht beim nächsten Start noch da.
*/
#include "TarockEngine.h"

#include <QCoreApplication>
#include <QSettings>
#include <QString>
#include <QTemporaryDir>

#include <cstdio>

namespace {

int failures = 0;

void check(bool ok, const char* what)
{
    if (!ok) {
        std::printf("FEHLT: %s\n", what);
        ++failures;
    }
}

} // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    QTemporaryDir home;
    QCoreApplication::setOrganizationName(QStringLiteral("tarock-test"));
    QCoreApplication::setApplicationName(QStringLiteral("test_settings"));
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, home.path());

    // Eine Einstellungsdatei, wie 0.3.x sie hinterlassen hat.
    {
        QSettings old;
        old.setValue(QStringLiteral("settings/deck"), QStringLiteral("modern"));
        old.sync();
    }

    {
        TarockEngine engine;
        check(engine.deck() == QLatin1String("iug1904"),
              "das alte \"modern\" verdeckt die Kartenbilder nicht mehr");
        check(!engine.deckPath(1).isEmpty() || engine.deckKeys().size() == 1,
              "und zu einem eingebauten Blatt gibt es auch ein Bild");

        // Jetzt wählt wirklich jemand -- und zwar das gezeichnete Blatt.
        engine.setDeck(QStringLiteral("modern"));
        check(engine.deck() == QLatin1String("modern"), "die Wahl gilt sofort");
        check(engine.deckPath(1).isEmpty(), "das gezeichnete Blatt hat keine Bilddatei");
    }

    {
        // Nächster Start: die Wahl steht noch.
        TarockEngine engine;
        check(engine.deck() == QLatin1String("modern"),
              "eine wirkliche Wahl überlebt den Neustart");
        engine.setDeck(QStringLiteral("iug1904"));
        check(engine.deck() == QLatin1String("iug1904"), "und lässt sich zurücknehmen");
    }

    {
        TarockEngine engine;
        check(engine.deck() == QLatin1String("iug1904"), "auch die zweite Wahl bleibt stehen");
    }

    // Die Liste der Blätter und ihre Namen gehören zusammen: die
    // Einstellungsseiten hängen beides aneinander.
    {
        TarockEngine engine;
        check(engine.deckKeys().size() == engine.deckNames().size(),
              "zu jedem Blatt gibt es genau einen Namen");
        check(engine.deckKeys().contains(QStringLiteral("modern")),
              "das gezeichnete Blatt steht immer zur Wahl");
        for (const QString& key : engine.deckKeys())
            check(!engine.deckName(key).isEmpty(), "jeder Name ist gesetzt");
    }

    if (failures == 0)
        std::printf("test_settings: alles in Ordnung\n");
    else
        std::printf("test_settings: %d Fehler\n", failures);
    return failures == 0 ? 0 : 1;
}
