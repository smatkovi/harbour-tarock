// Every lesson the app ships is played through from the first step to the last
// (docs/design.md §11.3): the fixed deal goes into the core, every scripted
// decision is offered to Lesson::offer() and has to be accepted, and the
// lesson has to reach its end. A typo in a lesson file, a step the core
// refuses or a script that walks into its own trap fails here instead of in
// front of a learner.
//
// What the settlement of the three practice hands must come to is checked by
// tests/test_hands.cpp against the fixtures; this test is about the scripts.
#include "Lesson.h"

#include "core/Action.h"
#include "core/Card.h"
#include "core/TarockCore.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QVariantMap>

#include <cstdio>
#include <cstdlib>

using namespace tarock;

namespace {

int failures = 0;

void report(const char* what, const QString& detail, int line)
{
    std::fprintf(stderr, "FAILED line %d: %s (%s)\n", line, what, detail.toUtf8().constData());
    if (++failures > 20)
        std::exit(1);
}
#define CHECK_MSG(x, detail) do { if (!(x)) report(#x, (detail), __LINE__); } while (false)

struct ActionName {
    ActionType type;
    const char* name;
};

// The names of assets/lessons/README.md; "discardSet" is not one of them
// because it stands for a whole discard rather than for one action.
const ActionName kActionNames[] = {
    {ActionType::Bid, "bid"},
    {ActionType::Pass, "pass"},
    {ActionType::Hold, "hold"},
    {ActionType::OpenForehand, "vorhand"},
    {ActionType::CallKing, "callking"},
    {ActionType::CallTarock, "calltarock"},
    {ActionType::TakeTalon, "taketalon"},
    {ActionType::Concede, "concede"},
    {ActionType::Discard, "discard"},
    {ActionType::ConfirmDiscard, "confirmdiscard"},
    {ActionType::AnnounceBonus, "bonus"},
    {ActionType::AnnounceBonus, "announce"},
    {ActionType::Kontra, "kontra"},
    {ActionType::Ready, "ready"},
    {ActionType::PlayCard, "play"},
};

ActionType actionTypeOf(const QString& name)
{
    for (const ActionName& entry : kActionNames) {
        if (name == QLatin1String(entry.name))
            return entry.type;
    }
    return ActionType::None;
}

const char* outcomeName(Lesson::Outcome outcome)
{
    switch (outcome) {
    case Lesson::NotRunning: return "not running";
    case Lesson::Advanced:   return "advanced";
    case Lesson::Trap:       return "trap";
    case Lesson::Rejected:   return "rejected";
    case Lesson::Finished:   return "finished";
    }
    return "?";
}

// Offers one action and insists that the lesson took it.
bool offerChecked(Lesson& lesson, TarockCore& core, const Action& action,
                  const QString& where, int line)
{
    QVariantMap info;
    const Lesson::Outcome outcome = lesson.offer(core, action, &info);
    if (outcome == Lesson::Advanced || outcome == Lesson::Finished)
        return true;
    report("lesson.offer() accepted the scripted action",
           where + QLatin1String(": ") + QLatin1String(outcomeName(outcome))
               + QLatin1String(" – ") + info.value(QStringLiteral("text")).toString(),
           line);
    return false;
}

// Plays one lesson from beginning to end. Returns the number of steps walked.
int replay(Lesson& lesson, const QString& file)
{
    TarockCore core;
    QString error;
    CHECK_MSG(lesson.begin(core, &error), file + QLatin1String(": ") + error);
    if (!lesson.running())
        return 0;

    int guard = 0;
    while (lesson.running() && !lesson.finished()) {
        if (++guard > 400) {
            report("the lesson ends", file + QLatin1String(": it keeps going in circles"),
                   __LINE__);
            return guard;
        }
        const QVariantMap map = lesson.stepMap();
        const QString where = file + QLatin1String(" step ")
                              + map.value(QStringLiteral("id")).toString();

        // A trick step: the learner owes the card the script names.
        if (map.contains(QStringLiteral("expectCard"))) {
            const int id = map.value(QStringLiteral("expectCard")).toInt();
            if (!offerChecked(lesson, core, Action(ActionType::PlayCard,
                                                   static_cast<std::int16_t>(id)),
                              where, __LINE__))
                return guard;
            continue;
        }

        if (map.contains(QStringLiteral("expect"))) {
            const QVariantMap expect = map.value(QStringLiteral("expect")).toMap();
            const QString type = expect.value(QStringLiteral("type")).toString();
            if (type == QLatin1String("discardSet")) {
                const QVariantList cards = expect.value(QStringLiteral("cards")).toList();
                CHECK_MSG(!cards.isEmpty(), where);
                bool ok = true;
                for (const QVariant& card : cards) {
                    ok = offerChecked(lesson, core,
                                      Action(ActionType::Discard,
                                             static_cast<std::int16_t>(card.toInt())),
                                      where, __LINE__);
                    if (!ok)
                        return guard;
                }
                if (!offerChecked(lesson, core, Action(ActionType::ConfirmDiscard), where,
                                  __LINE__))
                    return guard;
                continue;
            }
            const ActionType actionType = actionTypeOf(type);
            CHECK_MSG(actionType != ActionType::None, where + QLatin1String(": ") + type);
            if (actionType == ActionType::None)
                return guard;
            Action action(actionType);
            action.a = static_cast<std::int16_t>(expect.value(QStringLiteral("a"), -1).toInt());
            action.b = static_cast<std::int16_t>(expect.value(QStringLiteral("b"), -1).toInt());
            if (!offerChecked(lesson, core, action, where, __LINE__))
                return guard;
            continue;
        }

        // A step that only reads: "on" walks it.
        if (!lesson.advance(core)) {
            report("a reading step can be walked on", where, __LINE__);
            return guard;
        }
    }
    CHECK_MSG(lesson.finished(), file + QLatin1String(": the lesson did not reach its last step"));
    return guard;
}

} // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

#ifndef TAROCK_DATA_DIR
    std::fprintf(stderr, "test_lessons: TAROCK_DATA_DIR is not set\n");
    return 1;
#else
    const QString root = QLatin1String(TAROCK_DATA_DIR "/assets/lessons");
    QDir base(root);
    CHECK_MSG(base.exists(), root);

    int lessons = 0;
    const QStringList profiles = base.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    for (const QString& profile : profiles) {
        QDir dir(root + QLatin1Char('/') + profile);
        const QStringList files = dir.entryList(QStringList() << QStringLiteral("*.json"),
                                                QDir::Files, QDir::Name);
        for (const QString& file : files) {
            if (file == QLatin1String("index.json")
                || QFileInfo(file).completeBaseName().contains(QLatin1Char('.')))
                continue;   // the course order and the translation overlays
            const QString path = dir.filePath(file);
            const QString name = profile + QLatin1Char('/') + file;
            Lesson lesson;
            QString error;
            CHECK_MSG(lesson.loadFile(path, &error), name + QLatin1String(": ") + error);
            if (!lesson.loaded())
                continue;
            CHECK_MSG(!lesson.title().isEmpty(), name);
            CHECK_MSG(lesson.stepCount() > 0, name);
            CHECK_MSG(lesson.validateDeal(&error), name + QLatin1String(": ") + error);
            replay(lesson, name);
            ++lessons;
        }
    }
    CHECK_MSG(lessons > 0, QStringLiteral("no lesson was found at all"));

    if (failures == 0)
        std::printf("test_lessons: %d lessons played through\n", lessons);
    else
        std::fprintf(stderr, "test_lessons: %d failures\n", failures);
    return failures == 0 ? 0 : 1;
#endif
}
