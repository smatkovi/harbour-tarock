// Der Fünfertisch über das Netz (docs/design.md §8.3): zwei Gäste, und einer
// sitzt aus. Geprüft wird vor allem, was der Aussetzer **nicht** sehen darf --
// der Talon gehört den vier aktiven Spielern, ihm nie.
//
// Anders als lan_tarock4 spielt diese Prüfung keine ganze Hand durch: sie
// schaut auf die Sicht, die jeder Platz bekommt, und das steht schon nach dem
// Geben fest.
#include "TarockEngine.h"
#include "net/LanTable.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QSettings>
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
bool waitFor(QCoreApplication& app, Predicate done, int ms = 6000)
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

// Wie viele Karten dieser Tisch insgesamt auf den Händen zeigt.
int shownCards(const TarockEngine& engine)
{
    int total = 0;
    const QVariantList seats = engine.seats();
    for (int i = 0; i < seats.size(); ++i)
        total += seats.at(i).toMap().value(QStringLiteral("cardCount")).toInt();
    return total;
}

} // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    QTemporaryDir home;
    QCoreApplication::setOrganizationName(QStringLiteral("tarock-test"));
    QCoreApplication::setApplicationName(QStringLiteral("lan_tarock5"));
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, home.path());

    TarockEngine host;
    TarockEngine first;
    TarockEngine second;
    host.setAnimationsEnabled(false);
    LanTable* hostTable = qobject_cast<LanTable*>(host.tableObject());
    LanTable* firstTable = qobject_cast<LanTable*>(first.tableObject());
    LanTable* secondTable = qobject_cast<LanTable*>(second.tableObject());
    if (!hostTable || !firstTable || !secondTable) {
        std::printf("FEHLT: die Engines haben keinen Tisch\n");
        return 1;
    }

    check(host.hostTable(QStringLiteral("at-kr-ooe-2023"), 5), "der Fünfertisch geht auf");
    first.joinTable(QStringLiteral("127.0.0.1"));
    check(waitFor(app, [&] { return firstTable->mySeat() >= 1; }), "der erste Gast sitzt");
    second.joinTable(QStringLiteral("127.0.0.1"));
    check(waitFor(app, [&] { return secondTable->mySeat() >= 1; }), "der zweite Gast sitzt");
    check(firstTable->mySeat() != secondTable->mySeat(), "jeder bekommt einen eigenen Platz");
    std::printf("Plätze: %d und %d von 5\n", firstTable->mySeat() + 1, secondTable->mySeat() + 1);

    host.startTableMatch();
    check(waitFor(app, [&] { return first.active() && second.active()
                                    && !first.seats().isEmpty() && !second.seats().isEmpty(); }),
          "beide Gäste sehen den Tisch");

    // Am Fünfertisch gibt der Geber und setzt dabei aus; er hat keine Karten,
    // die anderen vier je zwölf. Das muss jede Sicht genauso zeigen.
    check(shownCards(host) == 48, "der Gastgeber zählt 48 Karten auf den Händen");
    check(shownCards(first) == 48, "der erste Gast zählt dieselben 48");
    check(shownCards(second) == 48, "der zweite Gast auch");

    TarockEngine* sittingOut = 0;
    TarockEngine* playing = 0;
    for (TarockEngine* engine : { &host, &first, &second }) {
        const QVariantList seats = engine->seats();
        if (seats.isEmpty())
            continue;
        const bool out = seats.at(0).toMap().value(QStringLiteral("isSittingOut")).toBool();
        if (out)
            sittingOut = engine;
        else if (!playing)
            playing = engine;
    }
    std::printf("aussetzend: %s\n", sittingOut ? "ja" : "niemand von den dreien");

    // Der Talon liegt verdeckt, bis ihn jemand aufschlägt; danach sehen ihn
    // die vier aktiven Spieler, der Fünfte nie. Vor dem Aufschlagen darf
    // niemand eine Talonkarte kennen -- die Zahl schon.
    if (playing) {
        const QVariantList halves = playing->talonHalves();
        check(halves.size() == 2, "zwei Talonhälften");
        int cards = 0, known = 0;
        for (int i = 0; i < halves.size(); ++i) {
            cards += halves.at(i).toMap().value(QStringLiteral("count")).toInt();
            known += halves.at(i).toMap().value(QStringLiteral("cards")).toList().size();
        }
        check(cards == 6, "sechs Talonkarten sind gezählt");
        check(known == 0, "vor dem Aufschlagen ist keine davon zu sehen");
    }
    if (sittingOut) {
        const QVariantList halves = sittingOut->talonHalves();
        int known = 0;
        for (int i = 0; i < halves.size(); ++i)
            known += halves.at(i).toMap().value(QStringLiteral("cards")).toList().size();
        check(known == 0, "der Aussetzer sieht keine Talonkarte");
        check(sittingOut->hand().isEmpty(), "und hält selbst keine Karten");
    }

    first.leaveTable();
    second.leaveTable();
    check(waitFor(app, [&] { return firstTable->role() == LanTable::None
                                    && secondTable->role() == LanTable::None; }),
          "beide Gäste können gehen");

    std::printf(failures == 0 ? "lan_tarock5: alles in Ordnung\n" : "lan_tarock5: %d Fehler\n",
                failures);
    return failures == 0 ? 0 : 1;
}
