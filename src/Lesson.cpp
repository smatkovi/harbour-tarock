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
#include "Lesson.h"

#include "ReasonText.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <string>
#include <vector>

using tarock::Action;
using tarock::ActionType;
using tarock::BonusDef;
using tarock::BonusId;
using tarock::Card;
using tarock::CardList;
using tarock::ContractDef;
using tarock::ContractId;
using tarock::Declaration;
using tarock::Phase;
using tarock::ProfileId;
using tarock::RuleProfile;
using tarock::Suit;
using tarock::TarockCore;

namespace {

struct ActionName {
    ActionType type;
    const char* name;
};

// The same names TarockEngine::act() speaks, so a lesson script and the QML
// side use one vocabulary (docs/design.md §5).
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
    // The lessons of assets/lessons/README.md call it "announce"; both names
    // mean the same action, and actionNameOf() keeps answering "bonus".
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

QString actionNameOf(ActionType type)
{
    for (const ActionName& entry : kActionNames) {
        if (entry.type == type)
            return QString::fromLatin1(entry.name);
    }
    return QString();
}

ProfileId profileIdFor(const QString& key)
{
    return key.startsWith(QLatin1String("HU"), Qt::CaseInsensitive)
        ? ProfileId::HuIlluItvb2019 : ProfileId::AtKrOoe2023;
}

bool parseCard(const QString& key, Card* card)
{
    if (key.isEmpty())
        return false;
    Card parsed;
    if (!tarock::parseCardKey(key.toStdString(), parsed))
        return false;
    *card = parsed;
    return true;
}

int contractIdFor(ProfileId profile, const QString& key)
{
    const RuleProfile& rules = RuleProfile::get(profile);
    for (const ContractDef& def : rules.contracts()) {
        if (key.compare(QLatin1String(def.key), Qt::CaseInsensitive) == 0)
            return static_cast<int>(def.id);
    }
    return -1;
}

int bonusIdFor(ProfileId profile, const QString& key)
{
    const RuleProfile& rules = RuleProfile::get(profile);
    for (const BonusDef& def : rules.bonuses()) {
        if (key.compare(QLatin1String(def.key), Qt::CaseInsensitive) == 0)
            return static_cast<int>(def.id);
    }
    return -1;
}

int suitIdFor(const QString& key)
{
    const QString upper = key.toUpper();
    if (upper == QLatin1String("H")) return 0;
    if (upper == QLatin1String("D")) return 1;
    if (upper == QLatin1String("S")) return 2;
    if (upper == QLatin1String("C")) return 3;
    return -1;
}

// "a" and "b" may be numbers or the stable text keys of the specification:
// a contract ("RUFER"), a suit ("S"), a bonus ("PAGAT") or a card ("XVIII").
// Which declaration of the running hand a lesson means by "GAME" or the key of
// a bonus. The index is the `a` of a Kontra action (TarockEngine::kontraTargets).
int kontraIndexFor(const TarockCore& core, ProfileId profile, const QString& key)
{
    const bool game = key.compare(QLatin1String("GAME"), Qt::CaseInsensitive) == 0
                      || key.compare(QLatin1String("SPIEL"), Qt::CaseInsensitive) == 0;
    const int bonus = game ? -1 : bonusIdFor(profile, key);
    if (!game && bonus < 0)
        return -1;
    const std::vector<Declaration>& declarations = core.declarations();
    for (std::size_t i = 0; i < declarations.size(); ++i) {
        const Declaration& declaration = declarations[i];
        if (game ? declaration.bonus == BonusId::None
                 : static_cast<int>(declaration.bonus) == bonus)
            return static_cast<int>(i);
    }
    return -1;
}

int resolveArgument(ProfileId profile, ActionType type, const QVariant& value, bool* ok)
{
    *ok = true;
    if (!value.isValid() || value.isNull())
        return -1;
    if (value.type() == QVariant::Int || value.type() == QVariant::LongLong
        || value.type() == QVariant::Double) {
        return value.toInt();
    }
    const QString key = value.toString().trimmed();
    if (key.isEmpty())
        return -1;
    switch (type) {
    case ActionType::Bid:
    case ActionType::Hold: {
        const int id = contractIdFor(profile, key);
        *ok = id >= 0;
        return id;
    }
    case ActionType::CallKing: {
        if (key.compare(QLatin1String("fourth"), Qt::CaseInsensitive) == 0)
            return -1;
        const int suit = suitIdFor(key);
        *ok = suit >= 0;
        return suit;
    }
    case ActionType::AnnounceBonus: {
        const int id = bonusIdFor(profile, key);
        *ok = id >= 0;
        return id;
    }
    case ActionType::PlayCard:
    case ActionType::Discard: {
        Card card;
        *ok = parseCard(key, &card);
        return *ok ? static_cast<int>(card.id) : -1;
    }
    default:
        break;
    }
    bool numeric = false;
    const int number = key.toInt(&numeric);
    *ok = numeric;
    return numeric ? number : -1;
}

QString fail(QString* error, const QString& message)
{
    if (error)
        *error = message;
    return message;
}

} // namespace

// --- loading ------------------------------------------------------------------------

bool Lesson::loadFile(const QString& path, QString* error)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        fail(error, tr("Lektion „%1“ lässt sich nicht öffnen: %2").arg(path, file.errorString()));
        return false;
    }
    const QByteArray data = file.readAll();
    file.close();
    return load(data, error);
}

bool Lesson::load(const QByteArray& json, QString* error)
{
    m_loaded = false;
    m_running = false;
    m_steps.clear();
    m_hands.clear();
    m_talon.clear();
    m_journal.clear();
    m_stepStart.clear();
    m_index = 0;
    m_moveCursor = 0;

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(json, &parseError);
    if (document.isNull() || !document.isObject()) {
        fail(error, tr("Die Lektion ist kein gültiges JSON: %1 (Position %2)")
             .arg(parseError.errorString()).arg(parseError.offset));
        return false;
    }
    const QVariantMap root = document.object().toVariantMap();

    m_id = root.value(QStringLiteral("id")).toString();
    if (m_id.isEmpty()) {
        fail(error, tr("Der Lektion fehlt das Feld „id“."));
        return false;
    }
    m_profile = profileIdFor(root.value(QStringLiteral("profile")).toString());
    m_title = root.value(QStringLiteral("title")).toString();
    m_module = root.value(QStringLiteral("module")).toString();
    m_moral = root.value(QStringLiteral("moral")).toString();
    m_goals = root.value(QStringLiteral("goals")).toStringList();
    m_seatNames = root.value(QStringLiteral("seatNames")).toStringList();
    m_players = root.value(QStringLiteral("players"), 4).toInt();
    if (m_players != 4 && m_players != 5) {
        fail(error, tr("„players“ muss 4 oder 5 sein, ist aber %1.").arg(m_players));
        return false;
    }
    m_dealer = root.value(QStringLiteral("dealer"), 0).toInt();
    if (m_dealer < 0 || m_dealer >= m_players) {
        fail(error, tr("„dealer“ liegt außerhalb des Tisches: %1.").arg(m_dealer));
        return false;
    }
    m_localSeat = root.value(QStringLiteral("localSeat"), 0).toInt();
    m_result = root.value(QStringLiteral("result")).toMap();

    const QVariantMap deal = root.value(QStringLiteral("deal")).toMap();
    if (deal.isEmpty()) {
        fail(error, tr("Der Lektion fehlt das Feld „deal“."));
        return false;
    }
    const QVariantList hands = deal.value(QStringLiteral("hands")).toList();
    for (const QVariant& item : hands) {
        CardList cards;
        const QStringList keys = item.toStringList();
        for (const QString& key : keys) {
            Card card;
            if (!parseCard(key, &card)) {
                fail(error, tr("Unbekannte Karte „%1“ in „deal.hands“.").arg(key));
                return false;
            }
            cards.push_back(card);
        }
        m_hands.push_back(cards);
    }
    const QVariantList talon = deal.value(QStringLiteral("talon")).toList();
    for (const QVariant& half : talon) {
        // Both [["XVIII","VI","D:4"], […]] and one flat list are accepted.
        const QStringList keys = half.type() == QVariant::List ? half.toStringList()
                                                               : QStringList(half.toString());
        for (const QString& key : keys) {
            Card card;
            if (!parseCard(key, &card)) {
                fail(error, tr("Unbekannte Karte „%1“ in „deal.talon“.").arg(key));
                return false;
            }
            m_talon.push_back(card);
        }
    }
    if (!validateDeal(error))
        return false;

    if (!parseSteps(root.value(QStringLiteral("steps")).toList(), error))
        return false;

    m_loaded = true;
    return true;
}

bool Lesson::validateDeal(QString* error) const
{
    const RuleProfile& rules = RuleProfile::get(m_profile);
    const int dealt = m_players == 5 ? 4 : m_players;
    if (static_cast<int>(m_hands.size()) != dealt) {
        fail(error, tr("„deal.hands“ hat %1 Blätter, gebraucht werden %2.")
             .arg(static_cast<int>(m_hands.size())).arg(dealt));
        return false;
    }
    tarock::CardSet seen;
    int total = 0;
    for (std::size_t seat = 0; seat < m_hands.size(); ++seat) {
        if (static_cast<int>(m_hands[seat].size()) != rules.handCards()) {
            fail(error, tr("Blatt %1 hat %2 Karten, gebraucht werden %3.")
                 .arg(static_cast<int>(seat))
                 .arg(static_cast<int>(m_hands[seat].size())).arg(rules.handCards()));
            return false;
        }
        for (Card card : m_hands[seat]) {
            if (tarock::contains(seen, card)) {
                fail(error, tr("Die Karte %1 kommt doppelt vor.").arg(ReasonText::cardName(card)));
                return false;
            }
            tarock::add(seen, card);
            ++total;
        }
    }
    if (static_cast<int>(m_talon.size()) != rules.talonSize()) {
        fail(error, tr("Der Talon hat %1 Karten, gebraucht werden %2.")
             .arg(static_cast<int>(m_talon.size())).arg(rules.talonSize()));
        return false;
    }
    for (Card card : m_talon) {
        if (tarock::contains(seen, card)) {
            fail(error, tr("Die Karte %1 kommt doppelt vor.").arg(ReasonText::cardName(card)));
            return false;
        }
        tarock::add(seen, card);
        ++total;
    }
    const int deckSize = rules.handCards() * dealt + rules.talonSize();
    if (total != deckSize) {
        fail(error, tr("Das Blatt ergibt %1 Karten statt %2.").arg(total).arg(deckSize));
        return false;
    }
    return true;
}

// --- parsing the steps --------------------------------------------------------------

bool Lesson::parseSteps(const QVariantList& raw, QString* error)
{
    if (raw.isEmpty()) {
        fail(error, tr("Die Lektion hat keine Schritte."));
        return false;
    }
    for (int i = 0; i < raw.size(); ++i) {
        Step step;
        if (!parseStep(raw.at(i).toMap(), i, &step, error))
            return false;
        m_steps.append(step);
    }
    return true;
}

bool Lesson::parseStep(const QVariantMap& raw, int position, Step* step, QString* error)
{
    step->id = raw.value(QStringLiteral("id")).toString();
    if (step->id.isEmpty())
        step->id = QStringLiteral("s%1").arg(position + 1);
    step->phase = raw.value(QStringLiteral("phase")).toString();
    step->seat = raw.value(QStringLiteral("seat"), m_localSeat).toInt();
    step->text = raw.value(QStringLiteral("text")).toString();
    step->trick = raw.value(QStringLiteral("trick"), -1).toInt();
    step->lead = raw.value(QStringLiteral("lead"), -1).toInt();
    step->expectWinner = raw.value(QStringLiteral("expectWinner"), -1).toInt();
    step->trickUnits = raw.value(QStringLiteral("trickUnits"), -1).toInt();
    step->compare = raw.value(QStringLiteral("compare")).toList();

    const QVariantMap highlight = raw.value(QStringLiteral("highlight")).toMap();
    const QStringList highlighted = highlight.value(QStringLiteral("hand")).toStringList();
    for (const QString& key : highlighted) {
        Card card;
        if (!parseCard(key, &card)) {
            fail(error, tr("Schritt %1: unbekannte Karte „%2“ in „highlight“.")
                 .arg(step->id, key));
            return false;
        }
        step->highlight.push_back(card);
    }

    const QVariantList autos = raw.value(QStringLiteral("auto")).toList();
    for (const QVariant& item : autos) {
        AutoAction entry;
        if (item.type() == QVariant::Map) {
            const QVariantMap map = item.toMap();
            entry.seat = map.value(QStringLiteral("seat"), -1).toInt();
            if (entry.seat >= m_players) {
                fail(error, tr("Schritt %1: „auto“ nennt den Sitz %2, den es nicht gibt.")
                     .arg(step->id).arg(entry.seat));
                return false;
            }
            const QString type = map.value(QStringLiteral("type")).toString();
            // The whole discard at once, the same shorthand "discardSet" that
            // `expect` uses; it runs as one Discard per card plus the
            // confirmation.
            if (type == QLatin1String("discardSet")) {
                const QStringList keys = map.value(QStringLiteral("cards")).toStringList();
                for (const QString& key : keys) {
                    Card card;
                    if (!parseCard(key, &card)) {
                        fail(error, tr("Schritt %1: unbekannte Karte „%2“ in „auto“.")
                             .arg(step->id, key));
                        return false;
                    }
                    entry.cards.push_back(card);
                }
                if (entry.cards.empty()) {
                    fail(error, tr("Schritt %1: „discardSet“ ohne Karten.").arg(step->id));
                    return false;
                }
                entry.discardSet = true;
                step->autoActions.append(entry);
                continue;
            }
            entry.action.type = actionTypeOf(type);
            bool ok = true;
            const QVariant argument = map.value(QStringLiteral("a"));
            if (entry.action.type == ActionType::Kontra && argument.type() == QVariant::String) {
                entry.kontraTarget = argument.toString().trimmed();
                entry.action.a = -1;
            } else {
                entry.action.a = static_cast<std::int16_t>(
                    resolveArgument(m_profile, entry.action.type, argument, &ok));
            }
            if (!ok) {
                fail(error, tr("Schritt %1: „auto“ hat einen unbekannten Parameter.").arg(step->id));
                return false;
            }
            entry.action.b = static_cast<std::int16_t>(
                resolveArgument(m_profile, entry.action.type, map.value(QStringLiteral("b")), &ok));
            if (entry.action.type == ActionType::None) {
                fail(error, tr("Schritt %1: „auto“ nennt eine unbekannte Aktion „%2“.")
                     .arg(step->id, type));
                return false;
            }
        } else {
            entry.action.type = actionTypeOf(item.toString());
            if (entry.action.type == ActionType::None) {
                fail(error, tr("Schritt %1: „auto“ nennt eine unbekannte Aktion „%2“.")
                     .arg(step->id, item.toString()));
                return false;
            }
        }
        step->autoActions.append(entry);
    }

    const QVariantList moves = raw.value(QStringLiteral("moves")).toList();
    for (const QVariant& item : moves) {
        const QVariantMap map = item.toMap();
        Move move;
        move.seat = map.value(QStringLiteral("seat"), -1).toInt();
        if (!parseCard(map.value(QStringLiteral("card")).toString(), &move.card)) {
            fail(error, tr("Schritt %1: unbekannte Karte in „moves“.").arg(step->id));
            return false;
        }
        if (move.seat < 0 || move.seat >= m_players) {
            fail(error, tr("Schritt %1: „moves“ nennt den Sitz %2, den es nicht gibt.")
                 .arg(step->id).arg(move.seat));
            return false;
        }
        step->moves.append(move);
    }

    if (raw.contains(QStringLiteral("expect")) && !parseExpect(raw, step, error))
        return false;
    return parseTraps(raw.value(QStringLiteral("traps")).toList(), step, error);
}

bool Lesson::parseExpect(const QVariantMap& raw, Step* step, QString* error)
{
    const QVariantMap expect = raw.value(QStringLiteral("expect")).toMap();
    step->expectType = expect.value(QStringLiteral("type")).toString();
    if (step->expectType.isEmpty()) {
        fail(error, tr("Schritt %1: „expect“ ohne „type“.").arg(step->id));
        return false;
    }
    // "discardSet" is not an action of the core but the whole discard tray:
    // the learner picks exactly these cards and confirms.
    if (step->expectType == QLatin1String("discardSet")) {
        const QStringList keys = expect.value(QStringLiteral("cards")).toStringList();
        for (const QString& key : keys) {
            Card card;
            if (!parseCard(key, &card)) {
                fail(error, tr("Schritt %1: unbekannte Karte „%2“ in „expect.cards“.")
                     .arg(step->id, key));
                return false;
            }
            step->expectCards.push_back(card);
        }
        if (step->expectCards.empty()) {
            fail(error, tr("Schritt %1: „discardSet“ ohne Karten.").arg(step->id));
            return false;
        }
        step->hasExpect = true;
        return true;
    }
    step->expect.type = actionTypeOf(step->expectType);
    if (step->expect.type == ActionType::None) {
        fail(error, tr("Schritt %1: „expect“ nennt eine unbekannte Aktion „%2“.")
             .arg(step->id, step->expectType));
        return false;
    }
    bool ok = true;
    step->expect.a = static_cast<std::int16_t>(
        resolveArgument(m_profile, step->expect.type, expect.value(QStringLiteral("a")), &ok));
    if (!ok) {
        fail(error, tr("Schritt %1: „expect.a“ ist unbekannt („%2“).")
             .arg(step->id, expect.value(QStringLiteral("a")).toString()));
        return false;
    }
    step->expect.b = static_cast<std::int16_t>(
        resolveArgument(m_profile, step->expect.type, expect.value(QStringLiteral("b")), &ok));
    if (!ok) {
        fail(error, tr("Schritt %1: „expect.b“ ist unbekannt.").arg(step->id));
        return false;
    }
    // "callking" with the fourth king: {"type":"callking","a":"fourth"}.
    if (step->expect.type == ActionType::CallKing
        && expect.value(QStringLiteral("a")).toString()
               .compare(QLatin1String("fourth"), Qt::CaseInsensitive) == 0) {
        step->expect.b = 1;
    }
    step->hasExpect = true;
    return true;
}

bool Lesson::parseTraps(const QVariantList& raw, Step* step, QString* error)
{
    for (const QVariant& item : raw) {
        const QVariantMap map = item.toMap();
        TrapDef trap;
        trap.reasonKey = map.value(QStringLiteral("reason")).toString();
        trap.text = map.value(QStringLiteral("text")).toString();
        if (map.contains(QStringLiteral("card"))) {
            if (!parseCard(map.value(QStringLiteral("card")).toString(), &trap.card)) {
                fail(error, tr("Schritt %1: unbekannte Karte in „traps“.").arg(step->id));
                return false;
            }
        }
        if (map.contains(QStringLiteral("action"))) {
            const QVariantMap action = map.value(QStringLiteral("action")).toMap();
            trap.action.type = actionTypeOf(action.value(QStringLiteral("type")).toString());
            if (trap.action.type == ActionType::None) {
                fail(error, tr("Schritt %1: „traps“ nennt eine unbekannte Aktion.").arg(step->id));
                return false;
            }
            bool ok = true;
            const QVariant argument = action.value(QStringLiteral("a"));
            if (trap.action.type == ActionType::Kontra && argument.type() == QVariant::String) {
                trap.kontraTarget = argument.toString().trimmed();
                trap.action.a = -1;
            } else {
                trap.action.a = static_cast<std::int16_t>(
                    resolveArgument(m_profile, trap.action.type, argument, &ok));
            }
            if (!ok) {
                fail(error, tr("Schritt %1: „traps“ hat einen unbekannten Parameter.").arg(step->id));
                return false;
            }
            trap.action.b = static_cast<std::int16_t>(
                resolveArgument(m_profile, trap.action.type, action.value(QStringLiteral("b")), &ok));
        }
        // Neither an action nor a card: the misconception of
        // assets/lessons/README.md that cannot be tapped. It needs a text,
        // which the next check insists on, and is shown with the step.
        if (trap.reasonKey.isEmpty() && trap.text.isEmpty()) {
            fail(error, tr("Schritt %1: eine Falle nennt weder „reason“ noch „text“.")
                 .arg(step->id));
            return false;
        }
        step->traps.append(trap);
    }
    return true;
}

// --- the state machine --------------------------------------------------------------

const Lesson::Step* Lesson::currentStep() const
{
    if (!m_loaded || m_index < 0 || m_index >= m_steps.size())
        return nullptr;
    return &m_steps.at(m_index);
}

bool Lesson::begin(TarockCore& core, QString* error)
{
    if (!m_loaded) {
        fail(error, tr("Es ist keine Lektion geladen."));
        return false;
    }
    // The seed only matters for hands the lesson does not deal itself.
    core.newMatch(m_profile, m_players, 1u);
    core.dealFixed(m_hands, m_talon, m_dealer);
    m_journal.clear();
    m_stepStart.clear();
    m_index = 0;
    m_moveCursor = 0;
    m_autoCursor = 0;
    m_expectDone = false;
    m_stepStart.append(0);
    m_running = true;
    runAutomatic(core);
    return true;
}

void Lesson::stop()
{
    m_running = false;
}

bool Lesson::apply(TarockCore& core, int seat, const Action& action)
{
    tarock::Reason reason;
    if (!core.apply(seat, action, &reason))
        return false;
    Applied entry;
    entry.seat = seat;
    entry.action = action;
    m_journal.append(entry);
    return true;
}

bool Lesson::applyAuto(TarockCore& core, int seat, const AutoAction& entry)
{
    if (!entry.kontraTarget.isEmpty()) {
        const int index = kontraIndexFor(core, m_profile, entry.kontraTarget);
        if (index < 0)
            return false;
        Action action = entry.action;
        action.a = static_cast<std::int16_t>(index);
        return apply(core, seat, action);
    }
    if (!entry.discardSet)
        return apply(core, seat, entry.action);
    for (Card card : entry.cards) {
        if (!apply(core, seat, Action(ActionType::Discard, static_cast<std::int16_t>(card.id))))
            return false;
    }
    return apply(core, seat, Action(ActionType::ConfirmDiscard));
}

void Lesson::enterStep(int index, TarockCore& core)
{
    m_index = index;
    m_moveCursor = 0;
    m_autoCursor = 0;
    m_expectDone = false;
    while (m_stepStart.size() <= m_index)
        m_stepStart.append(m_journal.size());
    m_stepStart[m_index] = m_journal.size();
    runAutomatic(core);
}

void Lesson::runAutomatic(TarockCore& core)
{
    if (!m_running)
        return;
    const Step* step = currentStep();
    if (!step)
        return;
    bool progress = true;
    while (progress && !core.handOver()) {
        progress = false;
        const int actor = core.actor();
        if (actor < 0)
            break;
        // The scripted moves of a trick: everything that is not the learner's
        // own seat runs by itself.
        if (m_moveCursor < step->moves.size()) {
            const Move& move = step->moves.at(m_moveCursor);
            if (move.seat != m_localSeat && move.seat == actor
                && apply(core, actor, Action(ActionType::PlayCard,
                                             static_cast<std::int16_t>(move.card.id)))) {
                ++m_moveCursor;
                progress = true;
                continue;
            }
        }
        // The `auto` actions: the short form runs for whoever is to act as
        // long as that is not the learner, the long form only for the seat it
        // names — the learner's own included, which is how a module winds a
        // hand forward to the point it wants to talk about.
        if (m_autoCursor < step->autoActions.size()) {
            const AutoAction& entry = step->autoActions.at(m_autoCursor);
            const bool due = entry.seat < 0 ? actor != m_localSeat : actor == entry.seat;
            if (due) {
                if (applyAuto(core, actor, entry)) {
                    ++m_autoCursor;
                    progress = true;
                    continue;
                }
                // A scripted action the core refuses stops the automatic part
                // rather than looping; the panel then shows the step unchanged.
                break;
            }
        }
    }
}

bool Lesson::stepComplete() const
{
    const Step* step = currentStep();
    if (!step)
        return false;
    if (m_moveCursor < step->moves.size())
        return false;
    if (m_autoCursor < step->autoActions.size())
        return false;
    return !step->hasExpect || m_expectDone;
}

const Lesson::Move* Lesson::pendingLocalMove() const
{
    const Step* step = currentStep();
    if (!step || m_moveCursor >= step->moves.size())
        return nullptr;
    const Move& move = step->moves.at(m_moveCursor);
    return move.seat == m_localSeat ? &move : nullptr;
}

bool Lesson::matchesExpect(const Step& step, const Action& action) const
{
    if (!step.hasExpect)
        return false;
    if (step.expectType == QLatin1String("discardSet")) {
        if (action.type == ActionType::Discard) {
            const Card card(static_cast<std::uint8_t>(action.a));
            for (Card wanted : step.expectCards) {
                if (wanted == card)
                    return true;
            }
            return false;
        }
        return action.type == ActionType::ConfirmDiscard;
    }
    if (action.type != step.expect.type)
        return false;
    // A step that does not name a parameter accepts any.
    if (step.expect.a >= 0 && action.a != step.expect.a)
        return false;
    if (step.expect.b >= 0 && action.b != step.expect.b)
        return false;
    return true;
}

const Lesson::TrapDef* Lesson::trapFor(const TarockCore& core, const Step& step,
                                       const Action& action) const
{
    for (const TrapDef& trap : step.traps) {
        // The misconception without action and card is shown with the step,
        // never sprung by a move.
        if (trap.action.type == ActionType::None && !trap.card.valid())
            continue;
        if (trap.action.type != ActionType::None) {
            if (trap.action.type != action.type)
                continue;
            if (!trap.kontraTarget.isEmpty()
                && kontraIndexFor(core, m_profile, trap.kontraTarget) != action.a)
                continue;
            if ((trap.action.a < 0 || trap.action.a == action.a)
                && (trap.action.b < 0 || trap.action.b == action.b))
                return &trap;
            continue;
        }
        // A trap given by card alone catches both playing and discarding it.
        if ((action.type == ActionType::PlayCard || action.type == ActionType::Discard)
            && static_cast<int>(trap.card.id) == static_cast<int>(action.a))
            return &trap;
    }
    return nullptr;
}

Lesson::Outcome Lesson::offer(TarockCore& core, const Action& action, QVariantMap* info)
{
    if (info)
        info->clear();
    if (!m_running || !m_loaded)
        return NotRunning;
    const Step* step = currentStep();
    if (!step)
        return NotRunning;

    if (const TrapDef* trap = trapFor(core, *step, action)) {
        if (info) {
            info->insert(QStringLiteral("step"), step->id);
            info->insert(QStringLiteral("reason"), trap->reasonKey);
            info->insert(QStringLiteral("text"), trap->text);
            info->insert(QStringLiteral("action"), actionNameOf(action.type));
            info->insert(QStringLiteral("a"), static_cast<int>(action.a));
            info->insert(QStringLiteral("b"), static_cast<int>(action.b));
        }
        return Trap;
    }

    // A trick step: the learner owes exactly the card the script names.
    if (const Move* move = pendingLocalMove()) {
        if (action.type != ActionType::PlayCard
            || static_cast<int>(action.a) != static_cast<int>(move->card.id)) {
            if (info)
                info->insert(QStringLiteral("text"),
                             tr("In dieser Übung spielen wir %1.")
                                 .arg(ReasonText::cardName(move->card)));
            return Rejected;
        }
        if (!apply(core, m_localSeat, action)) {
            if (info)
                info->insert(QStringLiteral("text"), tr("Diese Karte ist hier nicht erlaubt."));
            return Rejected;
        }
        ++m_moveCursor;
        runAutomatic(core);
        if (!stepComplete())
            return Advanced;
        enterStep(m_index + 1, core);
        return finished() ? Finished : Advanced;
    }

    if (!matchesExpect(*step, action)) {
        if (info) {
            info->insert(QStringLiteral("step"), step->id);
            info->insert(QStringLiteral("text"),
                         step->hasExpect
                             ? tr("In dieser Übung machen wir etwas anderes: %1").arg(step->text)
                             : tr("In dieser Übung ist jetzt nichts zu tun – tipp auf „Weiter“."));
        }
        return Rejected;
    }

    if (!apply(core, m_localSeat, action)) {
        if (info)
            info->insert(QStringLiteral("text"), tr("Diese Aktion lässt die Regel hier nicht zu."));
        return Rejected;
    }
    if (step->expectType != QLatin1String("discardSet")
        || action.type == ActionType::ConfirmDiscard) {
        m_expectDone = true;
    }
    runAutomatic(core);
    if (!stepComplete())
        return Advanced;
    enterStep(m_index + 1, core);
    return finished() ? Finished : Advanced;
}

bool Lesson::advance(TarockCore& core)
{
    if (!m_running || !m_loaded || finished())
        return false;
    if (!stepComplete())
        return false;
    enterStep(m_index + 1, core);
    return true;
}

bool Lesson::restartStep(TarockCore& core)
{
    if (!m_running || !m_loaded)
        return false;
    const int target = m_index;
    const int upTo = target < m_stepStart.size() ? m_stepStart.at(target) : m_journal.size();
    const QVector<Applied> journal = m_journal;
    core.newMatch(m_profile, m_players, 1u);
    core.dealFixed(m_hands, m_talon, m_dealer);
    m_journal.clear();
    for (int i = 0; i < upTo && i < journal.size(); ++i) {
        if (!apply(core, journal.at(i).seat, journal.at(i).action))
            return false;
    }
    m_index = target;
    m_moveCursor = 0;
    m_autoCursor = 0;
    m_expectDone = false;
    while (m_stepStart.size() <= m_index)
        m_stepStart.append(m_journal.size());
    m_stepStart[m_index] = m_journal.size();
    runAutomatic(core);
    return true;
}

// --- what QML sees ------------------------------------------------------------------

QVariantMap Lesson::stepMap() const
{
    QVariantMap map;
    const Step* step = currentStep();
    map.insert(QStringLiteral("lessonId"), m_id);
    map.insert(QStringLiteral("index"), m_index);
    map.insert(QStringLiteral("count"), m_steps.size());
    map.insert(QStringLiteral("finished"), finished());
    if (!step)
        return map;
    map.insert(QStringLiteral("id"), step->id);
    map.insert(QStringLiteral("phase"), step->phase);
    map.insert(QStringLiteral("seat"), step->seat);
    map.insert(QStringLiteral("text"), step->text);
    map.insert(QStringLiteral("trick"), step->trick);
    map.insert(QStringLiteral("expectWinner"), step->expectWinner);
    map.insert(QStringLiteral("trickUnits"), step->trickUnits);
    map.insert(QStringLiteral("compare"), step->compare);
    map.insert(QStringLiteral("complete"), stepComplete());
    QVariantList highlight;
    for (Card card : step->highlight)
        highlight.append(static_cast<int>(card.id));
    map.insert(QStringLiteral("highlight"), highlight);
    if (step->hasExpect) {
        QVariantMap expect;
        expect.insert(QStringLiteral("type"), step->expectType);
        expect.insert(QStringLiteral("a"), static_cast<int>(step->expect.a));
        expect.insert(QStringLiteral("b"), static_cast<int>(step->expect.b));
        QVariantList cards;
        for (Card card : step->expectCards)
            cards.append(static_cast<int>(card.id));
        expect.insert(QStringLiteral("cards"), cards);
        map.insert(QStringLiteral("expect"), expect);
    }
    if (const Move* move = pendingLocalMove())
        map.insert(QStringLiteral("expectCard"), static_cast<int>(move->card.id));
    // The misconceptions of this step: they belong to the text, not to a move.
    QVariantList notes;
    for (const TrapDef& trap : step->traps) {
        if (trap.action.type != ActionType::None || trap.card.valid())
            continue;
        QVariantMap note;
        note.insert(QStringLiteral("text"), trap.text);
        if (!trap.reasonKey.isEmpty())
            note.insert(QStringLiteral("reason"), trap.reasonKey);
        notes.append(note);
    }
    map.insert(QStringLiteral("notes"), notes);
    return map;
}

QVariantMap Lesson::toMap() const
{
    QVariantMap map;
    map.insert(QStringLiteral("id"), m_id);
    map.insert(QStringLiteral("title"), m_title);
    map.insert(QStringLiteral("module"), m_module);
    map.insert(QStringLiteral("moral"), m_moral);
    map.insert(QStringLiteral("goals"), m_goals);
    map.insert(QStringLiteral("seatNames"), m_seatNames);
    map.insert(QStringLiteral("players"), m_players);
    map.insert(QStringLiteral("dealer"), m_dealer);
    map.insert(QStringLiteral("localSeat"), m_localSeat);
    map.insert(QStringLiteral("stepCount"), m_steps.size());
    map.insert(QStringLiteral("result"), m_result);
    return map;
}
