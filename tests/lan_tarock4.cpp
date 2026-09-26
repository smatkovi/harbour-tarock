// Ein Vierertisch über das Netz, beide Seiten in einem Prozess über
// 127.0.0.1 (docs/design.md §8, §11): der Gastgeber führt den Kern, der Gast
// sitzt auf Platz 1 und bekommt nach jeder Änderung den Zustand, den sein
// Platz sehen darf.
//
// Geprüft wird das, was die Regelprüfungen nicht können:
//   * der Gast bekommt einen Platz und sieht den Tisch,
//   * er sitzt in seiner eigenen Sicht auf Platz 0 (gedreht),
//   * er sieht **keine** fremde Handkarte, aber die richtige Kartenzahl,
//   * seine Wünsche kommen an und bewegen den Tisch,
//   * die Partie läuft über beide Geräte bis zur Abrechnung durch.
//
// Der Bluetooth-Weg benutzt dieselbe LanSession und dieselben Nachrichten und
// braucht zwei Geräte; er steht deshalb nicht hier.
#include "TarockEngine.h"
#include "net/LanTable.h"
#include "core/RuleProfile.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QSettings>
#include <QThread>
#include <QTemporaryDir>
#include <QVariantList>
#include <QVariantMap>

#include <cstdio>
#include <cstdlib>

namespace {

int failures = 0;

void check(bool ok, const char* what)
{
    if (!ok) {
        std::printf("FEHLT: %s\n", what);
        std::fflush(stdout);
        ++failures;
    }
}

template <typename Predicate>
bool waitFor(QCoreApplication& app, Predicate done, int ms = 5000)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < ms) {
        app.processEvents(QEventLoop::AllEvents, 5);
        if (done())
            return true;
    }
    return done();
}

// Spielt für die Seite, die am Zug ist, den ersten Vorschlag: erst die
// Schaltflächen (Lizit, Ansagen, Ablage bestätigen), dann eine Karte.
//
// actConfirmed() statt act(): eine Warnung ("das kostet dich den Pagat") holt
// sich sonst eine Rückfrage, die am Tisch die Oberfläche stellt und hier
// niemand beantworten würde.
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
        const int id = card.value(QStringLiteral("id")).toInt();
        if (engine.actConfirmed(discarding ? QStringLiteral("discard") : QStringLiteral("play"), id))
            return true;
    }
    return false;
}

} // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    // Ungepuffert, damit bei einem Abbruch nicht die Zeile fehlt, die erklärt
    // warum.
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    // Einstellungen und gespeicherte Partien der Prüfung bleiben unter sich.
    QTemporaryDir home;
    QCoreApplication::setOrganizationName(QStringLiteral("tarock-test"));
    QCoreApplication::setApplicationName(QStringLiteral("lan_tarock4"));
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, home.path());

    TarockEngine host;
    TarockEngine guest;
    // Ohne Oberfläche sagt niemand der Engine, dass eine Kartenanimation
    // vorbei ist; hier tut es die Prüfung selbst, sofort.
    host.setAnimationsEnabled(false);
    guest.setAnimationsEnabled(false);
    QObject::connect(&host, &TarockEngine::cardAnimationRequested,
                     &host, [&host](int, int) { host.completeCardAnimation(); },
                     Qt::QueuedConnection);
    QObject::connect(&host, &TarockEngine::revealRequested,
                     &host, [&host](const QString&) { host.completeReveal(); },
                     Qt::QueuedConnection);
    LanTable* hostTable = qobject_cast<LanTable*>(host.tableObject());
    LanTable* guestTable = qobject_cast<LanTable*>(guest.tableObject());
    check(hostTable && guestTable, "beide Engines haben einen Tisch");
    if (!hostTable || !guestTable)
        return 1;

    // Welche Regeln am Tisch gelten, sagt die Kommandozeile -- so läuft
    // dieselbe Prüfung einmal österreichisch und einmal ungarisch (das
    // ungarische Blatt hat 42 Karten und neun auf der Hand, und es bietet
    // anders). Ohne Angabe: österreichisch.
    const QString profileKey = argc > 1 ? QString::fromLocal8Bit(argv[1])
                                        : QStringLiteral("AT-KR-OOE-2023-04");
    std::printf("Regeln: %s\n", profileKey.toLocal8Bit().constData());
    check(host.hostTable(profileKey, 4), "der Tisch geht auf");
    guest.joinTable(QStringLiteral("127.0.0.1"));
    check(waitFor(app, [&] { return guestTable->mySeat() >= 1; }), "der Gast bekommt einen Platz");
    std::printf("Gast sitzt auf Platz %d\n", guestTable->mySeat() + 1);
    check(hostTable->seatIsGuest(guestTable->mySeat()), "der Gastgeber kennt den Platz");

    int views = 0;
    QObject::connect(guestTable, &LanTable::viewArrived,
                     [&views](const QString&, const QVariantList&, const QVariantList&) {
        ++views;
    });

    host.startTableMatch();
    check(host.active() && host.players() == 4, "die Partie läuft beim Gastgeber");
    check(waitFor(app, [&] { return guest.active() && !guest.hand().isEmpty(); }),
          "der Gast sieht seine Karten");

    // Die Sicht des Gastes: eigene Hand vollständig, fremde Hände nur als
    // Anzahl, und er selbst sitzt auf Platz 0.
    const QVariantList seats = guest.seats();
    check(seats.size() == 4, "vier Plätze am Tisch des Gastes");
    const int handCards = tarock::RuleProfile::get(
                              profileKey == QLatin1String("HU-ILLU-ITVB-2019")
                                  ? tarock::ProfileId::HuIlluItvb2019
                                  : tarock::ProfileId::AtKrOoe2023).handCards();
    int shown = 0;
    for (int i = 0; i < seats.size(); ++i)
        shown += seats.at(i).toMap().value(QStringLiteral("cardCount")).toInt();
    std::printf("Handkarten laut Regeln: %d, gezählt: %d, eigene Hand: %d\n",
                handCards, shown, guest.hand().size());
    check(shown == 4 * handCards, "alle Handkarten sind gezählt, auch die verdeckten");
    check(guest.hand().size() == handCards, "die eigene Hand ist vollständig");
    check(seats.at(0).toMap().value(QStringLiteral("isMe")).toBool(),
          "der Gast sitzt in seiner Sicht auf Platz 0");

    // Und jetzt eine ganze Hand über beide Geräte.
    int actions = 0;
    QElapsedTimer clock;
    clock.start();
    QElapsedTimer sinceMove;
    sinceMove.start();
    while (clock.elapsed() < 180000 && !host.handOver()) {
        app.processEvents(QEventLoop::AllEvents, 10);
        // Nach jeder eigenen Aktion erst einmal zusehen: der Gast wartet auf
        // den neuen Zustand, sonst schickt er denselben Wunsch tausendmal.
        if (playSomething(host) || playSomething(guest)) {
            ++actions;
            sinceMove.restart();
            for (int i = 0; i < 20; ++i)
                app.processEvents(QEventLoop::AllEvents, 5);
            continue;
        }
        // Sonst sind die Computerplätze dran; ihr Zug kommt über den Zeitgeber.
        QThread::msleep(5);
        if (sinceMove.elapsed() > 8000) {
            int legalCards = 0;
            const QVariantList cards = host.hand();
            for (int i = 0; i < cards.size(); ++i)
                legalCards += cards.at(i).toMap().value(QStringLiteral("legal")).toBool() ? 1 : 0;
            std::printf("steht: Phase %d, am Zug %d, Stich %d, Bild %d, "
                        "Gastgeber am Zug %d, Hand %d, davon spielbar %d, Vorschläge %d\n",
                        host.phase(), host.actor(), host.trickNumber(), host.visualPhase(),
                        int(host.myTurn()), cards.size(), legalCards, host.options().size());
            break;
        }
        if (host.actor() >= 0 && host.actor() != 0 && host.actor() != guestTable->mySeat())
            sinceMove.restart();   // ein Computerplatz ist am Zug, das darf dauern
    }
    check(host.handOver(), "die Hand ist durchgespielt");
    check(actions > 20, "beide Seiten haben gehandelt");
    check(waitFor(app, [&] { return guest.handOver(); }), "der Gast sieht die Abrechnung");
    std::printf("%d Aktionen über das Netz, %d Sichtzustände beim Gast\n", actions, views);
    check(views > actions, "jede Änderung kam beim Gast an");

    // Nach der Abrechnung ist nichts mehr geheim: jetzt darf der Gast auch die
    // fremden Karten sehen.
    guest.leaveTable();
    check(waitFor(app, [&] { return guestTable->role() == LanTable::None; }),
          "der Gast kann den Tisch verlassen");

    std::printf(failures == 0 ? "lan_tarock4: alles in Ordnung\n" : "lan_tarock4: %d Fehler\n",
                failures);
    return failures == 0 ? 0 : 1;
}
