// The learning mode (docs/design.md §7): the sentences of every reason code
// (koenigrufen.md §11.4), the "why not?" answers, the hint engine and its
// knowledge limit, and the lesson loader of §7.6.
//
// Self-contained like tests/test_legality.cpp, but Qt based: LearnEngine is a
// QObject and the texts go through tr().
#include "LearnEngine.h"
#include "Lesson.h"
#include "ReasonText.h"
#include "RulesIndex.h"
#include "TarockEngine.h"

#include <QCoreApplication>
#include <QSet>
#include <QSettings>
#include <QTemporaryDir>
#include <QVariantMap>

#include <cstdio>
#include <cstdlib>
#include <random>

using namespace tarock;

namespace {

int failures = 0;

void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::fprintf(stderr, "FAILED line %d: %s\n", line, what);
        if (++failures > 20)
            std::exit(1);
    }
}
#define CHECK(x) check((x), #x, __LINE__)

void report(const char* what, const QString& detail, int line)
{
    std::fprintf(stderr, "FAILED line %d: %s (%s)\n", line, what,
                 detail.toUtf8().constData());
    if (++failures > 20)
        std::exit(1);
}
#define CHECK_MSG(x, detail) do { if (!(x)) report(#x, (detail), __LINE__); } while (false)

CardList cards(std::initializer_list<const char*> keys)
{
    CardList list;
    for (const char* key : keys) {
        Card card;
        if (parseCardKey(key, card))
            list.push_back(card);
        else
            std::fprintf(stderr, "unknown card key %s\n", key);
    }
    return list;
}

// The practice deal of docs/design.md §7.6 / koenigrufen.md §11.7: the four
// hands plus the talon are exactly the 54 cards, without duplicates.
std::vector<CardList> practiceHands()
{
    std::vector<CardList> hands;
    hands.push_back(cards({"XX", "XVII", "XI", "VIII", "V", "H:K", "H:B", "D:3", "S:D", "S:9",
                           "C:K", "C:8"}));
    hands.push_back(cards({"Sk", "XVI", "XIII", "IX", "III", "H:R", "H:2", "D:D", "D:B", "S:B",
                           "C:D", "C:R"}));
    hands.push_back(cards({"XIX", "XV", "XII", "IIII", "II", "H:1", "H:3", "D:K", "D:2", "S:K",
                           "S:10", "C:10"}));
    hands.push_back(cards({"XXI", "XIV", "X", "VII", "I", "H:4", "D:1", "S:R", "S:8", "S:7",
                           "C:B", "C:7"}));
    return hands;
}

CardList practiceTalon()
{
    return cards({"XVIII", "VI", "D:4", "H:D", "D:R", "C:9"});
}

const char* const kLessonJson =
    "{\n"
    "  \"id\": \"kr-test-1\",\n"
    "  \"profile\": \"AT-KR-OOE-2023-04\",\n"
    "  \"module\": \"L5\",\n"
    "  \"title\": \"Mein erster Rufer\",\n"
    "  \"goals\": [\"Lizitation der Vorhand\", \"König rufen\"],\n"
    "  \"players\": 4,\n"
    "  \"seatNames\": [\"Du\", \"Berta\", \"Carl\", \"Dora\"],\n"
    "  \"dealer\": 3,\n"
    "  \"deal\": {\n"
    "    \"hands\": [\n"
    "      [\"XX\",\"XVII\",\"XI\",\"VIII\",\"V\",\"H:K\",\"H:B\",\"D:3\",\"S:D\",\"S:9\",\"C:K\",\"C:8\"],\n"
    "      [\"Sk\",\"XVI\",\"XIII\",\"IX\",\"III\",\"H:R\",\"H:2\",\"D:D\",\"D:B\",\"S:B\",\"C:D\",\"C:R\"],\n"
    "      [\"XIX\",\"XV\",\"XII\",\"IIII\",\"II\",\"H:1\",\"H:3\",\"D:K\",\"D:2\",\"S:K\",\"S:10\",\"C:10\"],\n"
    "      [\"XXI\",\"XIV\",\"X\",\"VII\",\"I\",\"H:4\",\"D:1\",\"S:R\",\"S:8\",\"S:7\",\"C:B\",\"C:7\"]\n"
    "    ],\n"
    "    \"talon\": [[\"XVIII\",\"VI\",\"D:4\"], [\"H:D\",\"D:R\",\"C:9\"]]\n"
    "  },\n"
    "  \"steps\": [\n"
    "    { \"id\": \"s1\", \"phase\": \"bidding\", \"seat\": 0,\n"
    "      \"expect\": { \"type\": \"vorhand\" },\n"
    "      \"text\": \"Mit „Vorhand“ hältst du dir alles offen.\",\n"
    "      \"highlight\": { \"hand\": [\"XX\",\"XVII\",\"XI\"] },\n"
    "      \"traps\": [ { \"action\": { \"type\": \"bid\", \"a\": \"SECHSERDREIER\" },\n"
    "                   \"text\": \"Für den Sechserdreier bräuchtest du mehr Tarock.\" } ] },\n"
    "    { \"id\": \"s2\", \"auto\": [\"pass\",\"pass\",\"pass\"],\n"
    "      \"text\": \"Berta, Carl und Dora sagen weiter.\" },\n"
    "    { \"id\": \"s3\", \"phase\": \"bidding\", \"seat\": 0,\n"
    "      \"expect\": { \"type\": \"bid\", \"a\": \"RUFER\" },\n"
    "      \"text\": \"Der Rufer ist das passende Spiel.\" },\n"
    "    { \"id\": \"s4\", \"phase\": \"call\", \"seat\": 0,\n"
    "      \"expect\": { \"type\": \"callking\", \"a\": \"S\" },\n"
    "      \"text\": \"In Pik hast du die Dame.\",\n"
    "      \"traps\": [ { \"action\": { \"type\": \"callking\", \"a\": \"H\" },\n"
    "                   \"reason\": \"E_EIGENER_KOENIG\" } ] }\n"
    "  ],\n"
    "  \"moral\": \"Das Spiel zu gewinnen ist nur die halbe Miete.\"\n"
    "}\n";

// Runs the four seats to the next decision of seat 0, choosing at random.
void playOthers(TarockCore& core, std::mt19937& rng)
{
    int guard = 0;
    while (!core.handOver() && core.actor() != 0 && ++guard < 200) {
        const std::vector<Action> legal = core.legalActions(core.actor());
        if (legal.empty())
            break;
        Action action = legal[rng() % legal.size()];
        if (core.phase() == Phase::Bidding && rng() % 10 < 7) {
            for (const Action& option : legal) {
                if (option.type == ActionType::Pass) {
                    action = option;
                    break;
                }
            }
        }
        if (!core.apply(core.actor(), action))
            break;
    }
}

// The scripted run both halves of the knowledge-limit test take.
QVariantList scriptedHints(TarockEngine& engine, const std::vector<CardList>& hands)
{
    QVariantList collected;
    LearnEngine* learn = engine.learn();
    TarockCore& core = engine.coreForLearning();
    core.newMatch(ProfileId::AtKrOoe2023, 4, 7u);
    core.dealFixed(hands, practiceTalon(), 3);   // dealer 3, so seat 0 is forehand

    const Action script[] = {
        Action(ActionType::OpenForehand),
        Action(ActionType::Bid, static_cast<std::int16_t>(ContractId::Rufer)),
        Action(ActionType::CallKing, 2),          // spades: seat 0 holds the queen
        Action(ActionType::TakeTalon, 0),
    };
    for (const Action& action : script) {
        // Everybody else acts first; then seat 0 is asked for a hint and only
        // afterwards makes the scripted move.
        int guard = 0;
        while (!core.handOver() && core.actor() != 0 && ++guard < 40) {
            const std::vector<Action> legal = core.legalActions(core.actor());
            bool done = false;
            for (const Action& option : legal) {
                if (option.type == ActionType::Pass || option.type == ActionType::Ready) {
                    done = core.apply(core.actor(), option);
                    break;
                }
            }
            if (!done)
                break;
        }
        if (core.actor() != 0)
            break;
        learn->requestHint();
        collected.append(learn->hint());
        if (!core.apply(0, action))
            break;
    }

    // Discarding: the first three cards the core accepts, so the choice does
    // not depend on anything but seat 0's own hand.
    if (core.phase() == Phase::Discard && core.actor() == 0) {
        learn->requestHint();
        collected.append(learn->hint());
        int placed = 0;
        for (Card card : toList(core.hand(0))) {
            if (placed >= 3)
                break;
            if (core.cardReason(0, card).ok()
                && core.apply(0, Action(ActionType::Discard, static_cast<std::int16_t>(card.id)))) {
                ++placed;
            }
        }
        core.apply(0, Action(ActionType::ConfirmDiscard));
    }

    int guard = 0;
    while (core.phase() == Phase::Announce && !core.handOver() && ++guard < 40) {
        if (core.actor() == 0) {
            learn->requestHint();
            collected.append(learn->hint());
        }
        if (!core.apply(core.actor(), Action(ActionType::Ready)))
            break;
    }
    if (core.phase() == Phase::Play && core.actor() == 0) {
        learn->requestHint();
        collected.append(learn->hint());
    }
    return collected;
}

} // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    // Never touch the settings of the person running the test.
    QTemporaryDir settingsDir;
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settingsDir.path());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settingsDir.path());

    // --- 1. every reason code has a sentence -------------------------------
    const QVector<ReasonCode> codes = ReasonText::allCodes();
    CHECK(codes.size() == 68);
    for (ReasonCode code : codes) {
        Reason reason;
        reason.code = code;
        // Plausible parameters, so that every place marker has something.
        reason.card = tarockCard(20);
        reason.cards.set(0);
        reason.cards.set(19);
        reason.cards.set(suitCard(Suit::Heart, 0).id);
        reason.seat = 1;
        reason.suit = 2;
        reason.bonus = static_cast<std::int16_t>(BonusId::Pagat);
        reason.contract = static_cast<std::int16_t>(ContractId::Rufer);
        reason.trick = 12;
        reason.count = 3;
        const QString text = ReasonText::textFor(reason);
        CHECK_MSG(!ReasonText::isPlaceholder(text), QString::fromLatin1(reasonKey(code)));
        CHECK_MSG(!text.contains(QLatin1String("%1")), QString::fromLatin1(reasonKey(code)));
        CHECK_MSG(!ReasonText::isPlaceholder(ReasonText::shortLabel(code)),
                  QString::fromLatin1(reasonKey(code)));
        CHECK_MSG(ReasonText::anchor(code).startsWith(QLatin1String("rules:at-kr-ooe#")),
                  QString::fromLatin1(reasonKey(code)));
        CHECK_MSG(ReasonText::severity(code) == static_cast<int>(severityOf(code)),
                  QString::fromLatin1(reasonKey(code)));
    }
    // The keys of §11.4 round-trip.
    CHECK(ReasonText::codeForSpecKey(QStringLiteral("E_FARBZWANG")) == ReasonCode::FollowSuit);
    CHECK(ReasonText::codeForSpecKey(QStringLiteral("E_EIGENER_KOENIG")) == ReasonCode::CallOwnKing);
    CHECK(ReasonText::specKey(ReasonCode::MustTarock) == QLatin1String("E_TAROCKZWANG"));

    // --- 2. the reference tables -------------------------------------------
    CHECK(RulesIndex::glossary(ProfileId::AtKrOoe2023).size() >= 50);
    CHECK(RulesIndex::commonMistakes(ProfileId::AtKrOoe2023).size() == 20);
    CHECK(!RulesIndex::term(ProfileId::AtKrOoe2023, QStringLiteral("Tarockzwang")).isEmpty());
    CHECK(!RulesIndex::term(ProfileId::AtKrOoe2023, QStringLiteral("Schuss")).isEmpty());  // synonym
    CHECK(RulesIndex::markup(ProfileId::AtKrOoe2023, QStringLiteral("Es gilt Tarockzwang."))
              .contains(QLatin1String("glossary:Tarockzwang")));
    CHECK(RulesIndex::stepCount() == 7);
    CHECK(RulesIndex::stepIndex(Phase::Call) == 3);
    for (int phase = 0; phase <= static_cast<int>(Phase::HandOver); ++phase) {
        const QVariantMap map = RulesIndex::explanation(ProfileId::AtKrOoe2023,
                                                        static_cast<Phase>(phase),
                                                        ContractId::Rufer);
        CHECK(!map.value(QStringLiteral("whatHappens")).toString().isEmpty());
        CHECK(!map.value(QStringLiteral("whatToDo")).toString().isEmpty());
    }
    CHECK(!RulesIndex::tariffTable(ProfileId::AtKrOoe2023).isEmpty());
    CHECK(!RulesIndex::contractPurpose(ProfileId::AtKrOoe2023, ContractId::Rufer).isEmpty());
    CHECK(!RulesIndex::bonusPurpose(ProfileId::AtKrOoe2023, BonusId::Pagat).isEmpty());

    // --- 3. why() answers without doing anything ---------------------------
    {
        TarockEngine engine;
        LearnEngine* learn = engine.learn();
        CHECK(learn != nullptr);
        TarockCore& core = engine.coreForLearning();
        core.newMatch(ProfileId::AtKrOoe2023, 4, 3u);
        core.dealFixed(practiceHands(), practiceTalon(), 3);

        const std::string before = core.serialize();
        const QVariantMap why = learn->why(QStringLiteral("pass"));
        CHECK(why.value(QStringLiteral("key")).toString()
              == QLatin1String("E_FOREHAND_MUST_SPEAK"));
        CHECK(!why.value(QStringLiteral("text")).toString().isEmpty());
        CHECK(why.value(QStringLiteral("severity")).toInt() == static_cast<int>(Severity::Error));
        // Asking may not change anything (design.md §7.2).
        CHECK(core.serialize() == before);

        // Drive the hand to the discard phase and ask about a king.
        CHECK(core.apply(0, Action(ActionType::OpenForehand)));
        for (int seat = 1; seat <= 3; ++seat)
            CHECK(core.apply(seat, Action(ActionType::Pass)));
        CHECK(core.apply(0, Action(ActionType::Bid,
                                   static_cast<std::int16_t>(ContractId::Rufer))));
        CHECK(core.phase() == Phase::Call);
        CHECK(core.apply(0, Action(ActionType::CallKing, 2)));
        CHECK(core.phase() == Phase::Talon);
        CHECK(core.apply(0, Action(ActionType::TakeTalon, 0)));
        CHECK(core.phase() == Phase::Discard);

        const Card clubKing = suitCard(Suit::Club, 0);
        CHECK(contains(core.hand(0), clubKing));
        const QVariantMap whyKing = learn->whyCard(static_cast<int>(clubKing.id));
        CHECK(whyKing.value(QStringLiteral("key")).toString() == QLatin1String("E_DISCARD_KING"));
        CHECK(whyKing.value(QStringLiteral("specKey")).toString()
              == QLatin1String("E_ABLAGE_KOENIG"));
        CHECK(whyKing.value(QStringLiteral("text")).toString().contains(QString::fromUtf8("König")));
        CHECK(contains(core.hand(0), clubKing));   // still in hand

        // The standing helper works in the middle of a hand.
        const QVariantMap standing = learn->standing();
        CHECK(standing.value(QStringLiteral("tarocksOut")).toInt() >= 0);
        CHECK(standing.value(QStringLiteral("thresholdUnits")).toInt() == 107);
        CHECK(!standing.value(QStringLiteral("talonNote")).toString().isEmpty());
        CHECK(standing.value(QStringLiteral("birds")).toList().size() == 6);
        CHECK(!learn->commonMistakes().isEmpty());
    }

    // --- 4. a hint in every phase of full hands, always a legal action ------
    {
        TarockEngine engine;
        LearnEngine* learn = engine.learn();
        learn->setLevel(LearnEngine::Learning);
        TarockCore& core = engine.coreForLearning();
        std::mt19937 rng(20260912);
        QSet<int> phasesSeen;
        int hints = 0;
        for (int match = 0; match < 8; ++match) {
            core.newMatch(ProfileId::AtKrOoe2023, 4, rng());
            for (int hand = 0; hand < 2; ++hand) {
                int guard = 0;
                while (!core.handOver() && ++guard < 400) {
                    playOthers(core, rng);
                    if (core.handOver() || core.actor() != 0)
                        break;
                    const std::vector<Action> legal = core.legalActions(0);
                    CHECK(!legal.empty());
                    if (legal.empty())
                        break;
                    learn->requestHint();
                    const QVariantMap hint = learn->hint();
                    CHECK_MSG(!hint.isEmpty(), QStringLiteral("phase %1").arg(static_cast<int>(core.phase())));
                    const QVariantMap action = hint.value(QStringLiteral("action")).toMap();
                    CHECK(!action.isEmpty());
                    CHECK(!hint.value(QStringLiteral("label")).toString().isEmpty());
                    CHECK(!hint.value(QStringLiteral("text")).toString().isEmpty());
                    CHECK(!hint.value(QStringLiteral("short")).toString().isEmpty());
                    const QString quality = hint.value(QStringLiteral("quality")).toString();
                    CHECK(quality == QLatin1String("best"));   // rank 0 is always "best"
                    // The recommendation has to be one of the legal actions.
                    bool legalHint = false;
                    for (const Action& option : legal) {
                        if (static_cast<int>(option.type) != 0
                            && option.a == action.value(QStringLiteral("a")).toInt()
                            && option.b == action.value(QStringLiteral("b")).toInt()) {
                            legalHint = true;
                            break;
                        }
                    }
                    CHECK_MSG(legalHint, hint.value(QStringLiteral("label")).toString());
                    phasesSeen.insert(static_cast<int>(core.phase()));
                    ++hints;

                    // whyGood() agrees with the hint for the recommended move.
                    const QVariantMap good = learn->whyGood(
                        action.value(QStringLiteral("type")).toString(),
                        action.value(QStringLiteral("a")).toInt(),
                        action.value(QStringLiteral("b")).toInt());
                    CHECK(!good.value(QStringLiteral("comparison")).toString().isEmpty());

                    Action chosen;
                    for (const Action& option : legal) {
                        if (option.a == action.value(QStringLiteral("a")).toInt()
                            && option.b == action.value(QStringLiteral("b")).toInt()) {
                            chosen = option;
                            break;
                        }
                    }
                    if (chosen.type == ActionType::None)
                        chosen = legal.front();
                    if (!core.apply(0, chosen))
                        break;
                }
                playOthers(core, rng);
                if (core.handOver()) {
                    const QVariantMap debrief = learn->debrief();
                    CHECK(debrief.value(QStringLiteral("handOver")).toBool());
                    CHECK(debrief.contains(QStringLiteral("items")));
                    if (!core.nextHand())
                        break;
                } else {
                    break;
                }
            }
        }
        CHECK(hints > 50);
        // Bidding, talon, discarding, announcing and card play all produced a
        // hint somewhere in those hands.
        CHECK(phasesSeen.contains(static_cast<int>(Phase::Bidding)));
        CHECK(phasesSeen.contains(static_cast<int>(Phase::Talon)));
        CHECK(phasesSeen.contains(static_cast<int>(Phase::Discard)));
        CHECK(phasesSeen.contains(static_cast<int>(Phase::Announce)));
        CHECK(phasesSeen.contains(static_cast<int>(Phase::Play)));
        CHECK(phasesSeen.contains(static_cast<int>(Phase::Call)));
    }

    // --- 5. the knowledge limit --------------------------------------------
    // The hint is computed from the local seat's own view only. Swapping the
    // hands of two other seats leaves the public course of the hand untouched,
    // so not one of the hints may change (koenigrufen.md §10.8).
    {
        std::vector<CardList> straight = practiceHands();
        std::vector<CardList> swapped = practiceHands();
        std::swap(swapped[1], swapped[2]);

        TarockEngine engineA;
        TarockEngine engineB;
        const QVariantList hintsA = scriptedHints(engineA, straight);
        const QVariantList hintsB = scriptedHints(engineB, swapped);
        CHECK(hintsA.size() >= 5);
        CHECK(hintsA.size() == hintsB.size());
        for (int i = 0; i < hintsA.size() && i < hintsB.size(); ++i) {
            CHECK_MSG(hintsA.at(i) == hintsB.at(i),
                      QStringLiteral("hint %1: %2 vs %3").arg(i)
                          .arg(hintsA.at(i).toMap().value(QStringLiteral("label")).toString(),
                               hintsB.at(i).toMap().value(QStringLiteral("label")).toString()));
        }
        // Sanity: the two deals really are different for the other seats.
        CHECK(engineA.core().hand(1) != engineB.core().hand(1));
        CHECK(engineA.core().hand(0) == engineB.core().hand(0));
    }

    // --- 6. the lesson loader ----------------------------------------------
    {
        Lesson lesson;
        QString error;
        CHECK_MSG(lesson.load(QByteArray(kLessonJson), &error), error);
        CHECK(lesson.id() == QLatin1String("kr-test-1"));
        CHECK(lesson.title() == QLatin1String("Mein erster Rufer"));
        CHECK(lesson.players() == 4);
        CHECK(lesson.dealer() == 3);
        CHECK(lesson.goals().size() == 2);
        CHECK(lesson.seatNames().size() == 4);
        CHECK(lesson.stepCount() == 4);
        CHECK(lesson.hands().size() == 4);
        CHECK(lesson.talon().size() == 6);
        CHECK(lesson.validateDeal(&error));
        CHECK(!lesson.moral().isEmpty());

        TarockEngine engine;
        LearnEngine* learn = engine.learn();
        TarockCore& core = engine.coreForLearning();
        CHECK_MSG(lesson.begin(core, &error), error);
        CHECK(lesson.running());
        CHECK(core.phase() == Phase::Bidding);
        CHECK(core.actor() == 0);
        const Lesson::Step* step = lesson.currentStep();
        CHECK(step != nullptr && step->id == QLatin1String("s1"));
        CHECK(lesson.stepMap().value(QStringLiteral("highlight")).toList().size() == 3);

        QVariantMap info;
        // The listed beginner mistake fires a trap and changes nothing.
        CHECK(lesson.offer(core, Action(ActionType::Bid,
                                        static_cast<std::int16_t>(ContractId::Sechserdreier)),
                           &info) == Lesson::Trap);
        CHECK(!info.value(QStringLiteral("text")).toString().isEmpty());
        CHECK(core.phase() == Phase::Bidding && core.actor() == 0);
        // Something legal but off script is refused softly.
        CHECK(lesson.offer(core, Action(ActionType::Bid,
                                        static_cast<std::int16_t>(ContractId::Dreier)),
                           &info) == Lesson::Rejected);
        CHECK(!info.value(QStringLiteral("text")).toString().isEmpty());
        // The expected action goes through, then the three passes run by
        // themselves and the lesson waits on the text step.
        CHECK(lesson.offer(core, Action(ActionType::OpenForehand), &info) == Lesson::Advanced);
        CHECK(lesson.currentStep() != nullptr
              && lesson.currentStep()->id == QLatin1String("s2"));
        CHECK(core.actor() == 0);        // everyone else has passed
        CHECK(lesson.advance(core));
        CHECK(lesson.currentStep()->id == QLatin1String("s3"));
        CHECK(lesson.offer(core, Action(ActionType::Bid,
                                        static_cast<std::int16_t>(ContractId::Rufer)),
                           &info) == Lesson::Advanced);
        CHECK(core.phase() == Phase::Call);
        CHECK(lesson.currentStep()->id == QLatin1String("s4"));
        // A trap given by reason key only.
        CHECK(lesson.offer(core, Action(ActionType::CallKing, 0), &info) == Lesson::Trap);
        CHECK(info.value(QStringLiteral("reason")).toString()
              == QLatin1String("E_EIGENER_KOENIG"));
        // Repeating a step replays the hand from the deal.
        CHECK(lesson.restartStep(core));
        CHECK(core.phase() == Phase::Call && core.actor() == 0);
        CHECK(lesson.offer(core, Action(ActionType::CallKing, 2), &info) == Lesson::Finished);
        CHECK(lesson.finished());
        Q_UNUSED(learn);
    }

    // --- 7. a broken lesson reports, it does not crash ----------------------
    {
        Lesson lesson;
        QString error;
        CHECK(!lesson.load(QByteArray("{ this is not json"), &error));
        CHECK(!error.isEmpty());
        CHECK(!lesson.loaded());
        CHECK(!lesson.load(QByteArray("{\"id\":\"x\",\"deal\":{\"hands\":[[\"XX\"]],"
                                      "\"talon\":[]},\"steps\":[]}"), &error));
        CHECK(!error.isEmpty());
        // A duplicated card is caught by the deal check.
        QByteArray duplicated(kLessonJson);
        duplicated.replace("\"S:9\"", "\"XX\"");
        CHECK(!lesson.load(duplicated, &error));
        CHECK(error.contains(QLatin1String("doppelt")));
        TarockCore core;
        CHECK(!lesson.begin(core, &error));
    }

    // --- 8. the level is a persisted setting --------------------------------
    {
        TarockEngine engine;
        engine.learn()->setLevel(LearnEngine::Novice);
        CHECK(engine.learn()->level() == LearnEngine::Novice);
        CHECK(engine.learn()->panelOpen());
        TarockEngine second;
        CHECK(second.learn()->level() == LearnEngine::Novice);
        second.learn()->setLevel(LearnEngine::Learning);
    }

    if (failures == 0)
        std::printf("test_learn: all checks passed\n");
    else
        std::fprintf(stderr, "test_learn: %d failures\n", failures);
    return failures == 0 ? 0 : 1;
}
