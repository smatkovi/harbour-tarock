// Ein Gastgeber ohne Oberfläche, für die Probe mit einem echten zweiten
// Gerät (keine Prüfung, läuft nicht unter ctest): eröffnet einen Tisch,
// wartet, bis ein Gast sitzt, beginnt die Partie und spielt seinen eigenen
// Platz so stumpf wie lan_tarock4 -- erster Vorschlag, erste spielbare Karte.
//
//   lan_host [<profil>] [<plätze>] [<sekunden bis zum Anfang nach dem ersten Gast>]
//
// Gedacht für die Jolla als Gastgeber ohne Fenster, während das N950 mit
// seiner Oberfläche beitritt.
#include "TarockEngine.h"
#include "net/LanTable.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QSettings>
#include <QTemporaryDir>
#include <QThread>
#include <QVariantList>
#include <QVariantMap>

#include <cstdio>

namespace {

bool playSomething(TarockEngine& engine)
{
    if (!engine.myTurn())
        return false;
    const QVariantList options = engine.options();
    for (int i = 0; i < options.size(); ++i) {
        const QVariantMap option = options.at(i).toMap();
        if (engine.actConfirmed(option.value(QStringLiteral("type")).toString(),
                                option.value(QStringLiteral("a")).toInt(),
                                option.value(QStringLiteral("b")).toInt()))
            return true;
    }
    const QVariantList hand = engine.hand();
    const bool discarding = engine.phase() == int(tarock::Phase::Discard);
    for (int i = 0; i < hand.size(); ++i) {
        const QVariantMap card = hand.at(i).toMap();
        if (!card.value(QStringLiteral("legal")).toBool())
            continue;
        if (engine.actConfirmed(discarding ? QStringLiteral("discard") : QStringLiteral("play"),
                                card.value(QStringLiteral("id")).toInt()))
            return true;
    }
    return false;
}

} // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    QTemporaryDir home;
    QCoreApplication::setOrganizationName(QStringLiteral("tarock-test"));
    QCoreApplication::setApplicationName(QStringLiteral("lan_host"));
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, home.path());

    const QString profileKey = argc > 1 ? QString::fromLocal8Bit(argv[1])
                                        : QStringLiteral("AT-KR-OOE-2023-04");
    const int seatCount = argc > 2 ? QString::fromLocal8Bit(argv[2]).toInt() : 4;
    const int graceMs = (argc > 3 ? QString::fromLocal8Bit(argv[3]).toInt() : 8) * 1000;

    TarockEngine host;
    host.setAnimationsEnabled(false);
    host.setWaitAfterTrick(false);
    host.setWaitAfterReveal(false);
    QObject::connect(&host, &TarockEngine::cardAnimationRequested,
                     &host, [&host](int, int) { host.completeCardAnimation(); },
                     Qt::QueuedConnection);
    QObject::connect(&host, &TarockEngine::revealRequested,
                     &host, [&host](const QString&) { host.completeReveal(); },
                     Qt::QueuedConnection);
    LanTable* table = qobject_cast<LanTable*>(host.tableObject());
    QObject::connect(table, &LanTable::changed, [table] {
        std::printf("Tisch: %s\n", table->status().toLocal8Bit().constData());
    });

    // Wie auf der Netzseite: erst eine eigene Partie, dann der Tisch -- der
    // Fall, der bis 0.7.0 den Anfang verhinderte.
    host.startMatch(profileKey, seatCount);
    if (!host.hostTable(profileKey, seatCount)) {
        std::printf("der Tisch geht nicht auf\n");
        return 1;
    }
    std::printf("Tisch offen: %s, %d Plätze, gestartet=%d, eigene Partie aktiv=%d\n",
                profileKey.toLocal8Bit().constData(), seatCount, int(table->started()),
                int(host.active()));

    QElapsedTimer sinceGuest;
    bool haveGuest = false;
    QElapsedTimer clock;
    clock.start();
    while (clock.elapsed() < 20 * 60 * 1000) {
        app.processEvents(QEventLoop::AllEvents, 10);
        QThread::msleep(5);
        if (!table->started()) {
            int guests = 0;
            for (int seat = 1; seat < seatCount; ++seat)
                guests += table->seatIsGuest(seat) ? 1 : 0;
            if (guests > 0 && !haveGuest) {
                haveGuest = true;
                sinceGuest.start();
                std::printf("ein Gast sitzt, Anfang in %d s\n", graceMs / 1000);
            }
            if (haveGuest && sinceGuest.elapsed() >= graceMs) {
                host.startTableMatch();
                std::printf("Partie begonnen, gestartet=%d\n", int(table->started()));
            }
            continue;
        }
        if (playSomething(host)) {
            std::printf("Gastgeber: Phase %d, Stich %d\n", host.phase(), host.trickNumber());
            for (int i = 0; i < 20; ++i)
                app.processEvents(QEventLoop::AllEvents, 5);
        }
        if (host.handOver()) {
            std::printf("Hand zu Ende\n");
            for (int i = 0; i < 200; ++i)
                app.processEvents(QEventLoop::AllEvents, 5);
            break;
        }
    }
    host.leaveTable();
    return 0;
}
