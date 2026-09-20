// The contract between the engines and the QML (docs/design.md §5, §7.2): every
// property and method the pages read has to arrive in QML as something other
// than `undefined`.
//
// This test exists because it did not: `learn` was declared with a
// forward-declared LearnEngine, so moc had no metatype for it and
// tarockEngine.learn reached QML as `undefined`. Nothing crashed — the panels
// of the learning mode simply switched themselves off, the tutorial listed no
// lessons, and the C++ side looked perfectly healthy in every other test.
//
// A real QQmlEngine evaluates the expressions here; no window is needed.
#include "LearnEngine.h"
#include "TarockEngine.h"

#include <QCoreApplication>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSettings>
#include <QString>
#include <QTemporaryDir>
#include <QUrl>
#include <QVariant>

#include <cstdio>
#include <cstdlib>

namespace {

int failures = 0;

void report(const char* what, const QString& detail, int line)
{
    std::fprintf(stderr, "FAILED line %d: %s (%s)\n", line, what, detail.toUtf8().constData());
    if (++failures > 20)
        std::exit(1);
}
#define CHECK_MSG(x, detail) do { if (!(x)) report(#x, (detail), __LINE__); } while (false)

// Evaluates one expression against the engine and returns it as QML sees it.
QVariant evaluate(QQmlEngine& qml, const QString& expression, QString* error)
{
    QQmlComponent component(&qml);
    const QString source = QStringLiteral("import QtQml 2.0\nQtObject { property var probe: %1 }\n")
                               .arg(expression);
    component.setData(source.toUtf8(), QUrl());
    QObject* object = component.create();
    if (!object) {
        *error = component.errorString();
        return QVariant();
    }
    const QVariant value = object->property("probe");
    delete object;
    return value;
}

} // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("harbour-tarock"));
    QCoreApplication::setApplicationName(QStringLiteral("harbour-tarock"));
    QTemporaryDir settingsDir;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settingsDir.path());

    TarockEngine engine;
    QQmlEngine qml;
    qml.rootContext()->setContextProperty(QStringLiteral("tarockEngine"), &engine);

    // Everything the pages of docs/design.md §6.1 touch before a match runs.
    const char* const expressions[] = {
        "tarockEngine.learn",
        "tarockEngine.learn.course",
        "tarockEngine.learn.level",
        "tarockEngine.learn.tourSeen",
        "tarockEngine.learn.lessonCount",
        "tarockEngine.learn.lessonsDone",
        "tarockEngine.learn.nextLessonId",
        "tarockEngine.learn.lessonActive",
        "tarockEngine.learn.lessonStep",
        "tarockEngine.learn.explanation",
        "tarockEngine.learn.hint",
        "tarockEngine.learn.panelOpen",
        "tarockEngine.learn.autoHint",
        "tarockEngine.learn.lessonIds()",
        "tarockEngine.learn.glossary()",
        "tarockEngine.learn.chapters()",
        "tarockEngine.learn.commonMistakes()",
        "tarockEngine.active",
        "tarockEngine.canResume",
        "tarockEngine.profileKey",
        "tarockEngine.profileName",
        "tarockEngine.players",
        "tarockEngine.seats",
        "tarockEngine.hand",
        "tarockEngine.trick",
        "tarockEngine.options",
        "tarockEngine.status",
        "tarockEngine.deck",
        "tarockEngine.ledger",
        "tarockEngine.liveCount",
        "tarockEngine.profileNameFor(tarockEngine.profileKey)",
    };
    for (const char* expression : expressions) {
        QString error;
        const QVariant value = evaluate(qml, QString::fromLatin1(expression), &error);
        const QString what = QString::fromLatin1(expression);
        CHECK_MSG(error.isEmpty(), what + QLatin1String(": ") + error);
        // `undefined` arrives as an invalid QVariant; an empty string or an
        // empty list is a legitimate answer before a match runs.
        CHECK_MSG(value.isValid(), what + QLatin1String(" is undefined in QML"));
    }

    // The tutorial of §7.9 lists the course the engine reads from disk; an
    // empty one means the pages would show the guided tour and nothing else.
    QString error;
    const QVariant length = evaluate(qml, QStringLiteral("tarockEngine.learn.course.length"), &error);
    CHECK_MSG(error.isEmpty(), error);
    CHECK_MSG(length.toInt() == engine.learn()->course().size(),
              QStringLiteral("QML sees %1 lessons, C++ has %2")
                  .arg(length.toInt()).arg(engine.learn()->course().size()));
    CHECK_MSG(length.toInt() > 0, QStringLiteral("the course reached QML empty"));

    const QVariant title = evaluate(qml, QStringLiteral("tarockEngine.learn.course[0].title"), &error);
    CHECK_MSG(error.isEmpty(), error);
    CHECK_MSG(!title.toString().isEmpty(), QStringLiteral("the first lesson has no title in QML"));

    if (failures == 0)
        std::printf("test_qmlbridge: the engine is complete in QML\n");
    else
        std::fprintf(stderr, "test_qmlbridge: %d failures\n", failures);
    return failures == 0 ? 0 : 1;
}
