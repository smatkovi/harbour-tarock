// Replays the three written practice hands of docs/koenigrufen.md §11.7
// through the rule engine and compares trick winners, card points and the
// settlement with the fixtures in tests/fixtures/.
#include "core/Scoring.h"
#include "core/TarockCore.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace tarock;

namespace {

int failures = 0;
void check(bool ok, const std::string& what, int line)
{
    if (!ok) {
        std::fprintf(stderr, "FAILED line %d: %s\n", line, what.c_str());
        ++failures;
    }
}
#define CHECK(x) check((x), #x, __LINE__)
#define CHECK_MSG(x, msg) check((x), (msg), __LINE__)

// A small JSON reader: the fixtures are flat objects, arrays, strings and
// integers, so a full parser would be overkill.
struct Json {
    enum class Type { Null, Bool, Number, String, Array, Object } type = Type::Null;
    bool boolean = false;
    long number = 0;
    std::string text;
    std::vector<Json> items;
    std::map<std::string, Json> fields;

    const Json& operator[](const std::string& key) const
    {
        static const Json empty;
        auto it = fields.find(key);
        return it == fields.end() ? empty : it->second;
    }
    const Json& operator[](std::size_t index) const
    {
        static const Json empty;
        return index < items.size() ? items[index] : empty;
    }
    std::size_t size() const { return items.size(); }
    bool has(const std::string& key) const { return fields.count(key) > 0; }
    int toInt() const { return static_cast<int>(number); }
};

struct Parser {
    const std::string& text;
    std::size_t pos = 0;

    void skip()
    {
        while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])))
            ++pos;
    }
    Json value()
    {
        skip();
        if (pos >= text.size())
            return Json();
        const char c = text[pos];
        if (c == '{')
            return object();
        if (c == '[')
            return array();
        if (c == '"') {
            Json json;
            json.type = Json::Type::String;
            json.text = string();
            return json;
        }
        if (text.compare(pos, 4, "true") == 0) {
            pos += 4;
            Json json; json.type = Json::Type::Bool; json.boolean = true; return json;
        }
        if (text.compare(pos, 5, "false") == 0) {
            pos += 5;
            Json json; json.type = Json::Type::Bool; json.boolean = false; return json;
        }
        if (text.compare(pos, 4, "null") == 0) {
            pos += 4;
            return Json();
        }
        Json json;
        json.type = Json::Type::Number;
        std::size_t end = pos;
        while (end < text.size() && (std::isdigit(static_cast<unsigned char>(text[end]))
                                     || text[end] == '-' || text[end] == '+' || text[end] == '.'))
            ++end;
        json.number = std::strtol(text.substr(pos, end - pos).c_str(), nullptr, 10);
        pos = end;
        return json;
    }
    std::string string()
    {
        std::string result;
        ++pos;  // opening quote
        while (pos < text.size() && text[pos] != '"') {
            if (text[pos] == '\\' && pos + 1 < text.size()) {
                ++pos;
                result += text[pos] == 'n' ? '\n' : text[pos];
            } else {
                result += text[pos];
            }
            ++pos;
        }
        ++pos;  // closing quote
        return result;
    }
    Json object()
    {
        Json json;
        json.type = Json::Type::Object;
        ++pos;
        while (true) {
            skip();
            if (pos >= text.size() || text[pos] == '}') { ++pos; break; }
            if (text[pos] == ',') { ++pos; continue; }
            const std::string key = string();
            skip();
            if (pos < text.size() && text[pos] == ':')
                ++pos;
            json.fields[key] = value();
        }
        return json;
    }
    Json array()
    {
        Json json;
        json.type = Json::Type::Array;
        ++pos;
        while (true) {
            skip();
            if (pos >= text.size() || text[pos] == ']') { ++pos; break; }
            if (text[pos] == ',') { ++pos; continue; }
            json.items.push_back(value());
        }
        return json;
    }
};

Json readFile(const std::string& path)
{
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    const std::string text = buffer.str();
    Parser parser{text};
    return parser.value();
}

CardList cardsOf(const Json& array)
{
    CardList cards;
    for (std::size_t i = 0; i < array.size(); ++i) {
        Card card;
        if (parseCardKey(array[i].text, card))
            cards.push_back(card);
        else
            std::fprintf(stderr, "unknown card key '%s'\n", array[i].text.c_str());
    }
    return cards;
}

ContractId contractOf(const RuleProfile& profile, const std::string& key)
{
    for (const ContractDef& def : profile.contracts()) {
        if (key == def.key)
            return def.id;
    }
    return ContractId::None;
}

BonusId bonusOf(const RuleProfile& profile, const std::string& key)
{
    for (const BonusDef& def : profile.bonuses()) {
        if (key == def.key)
            return def.id;
    }
    return BonusId::None;
}

int suitOf(const std::string& key)
{
    if (key == "H") return 0;
    if (key == "D") return 1;
    if (key == "S") return 2;
    if (key == "C") return 3;
    return -1;
}

void replay(const std::string& path)
{
    const Json fixture = readFile(path);
    const std::string id = fixture["id"].text;
    if (id.empty()) {
        std::fprintf(stderr, "FAILED: %s could not be read\n", path.c_str());
        ++failures;
        return;
    }
    const RuleProfile& profile = RuleProfile::get(ProfileId::AtKrOoe2023);

    std::vector<CardList> hands;
    for (std::size_t i = 0; i < fixture["hands"].size(); ++i)
        hands.push_back(cardsOf(fixture["hands"][i]));
    CardList talon = cardsOf(fixture["talon"][0]);
    for (Card card : cardsOf(fixture["talon"][1]))
        talon.push_back(card);

    TarockCore core(ProfileId::AtKrOoe2023, fixture["players"].toInt(), 1);
    core.dealFixed(hands, talon, fixture["dealer"].toInt());
    CHECK_MSG(core.forehand() == fixture["forehand"].toInt(), id + ": forehand");

    const Json& actions = fixture["actions"];
    for (std::size_t i = 0; i < actions.size(); ++i) {
        const Json& entry = actions[i];
        const std::string type = entry["type"].text;
        const int seat = entry["seat"].toInt();
        Action action;
        if (type == "OPEN_FOREHAND") {
            action = Action(ActionType::OpenForehand);
        } else if (type == "PASS") {
            action = Action(ActionType::Pass);
        } else if (type == "HOLD") {
            action = Action(ActionType::Hold);
        } else if (type == "BID") {
            action = Action(ActionType::Bid, static_cast<std::int16_t>(contractOf(profile, entry["a"].text)));
        } else if (type == "CALL_KING") {
            action = entry.has("fourth") ? Action(ActionType::CallKing, -1, 1)
                                         : Action(ActionType::CallKing,
                                                  static_cast<std::int16_t>(suitOf(entry["suit"].text)));
        } else if (type == "TAKE_TALON") {
            action = Action(ActionType::TakeTalon, static_cast<std::int16_t>(entry["half"].toInt()));
        } else if (type == "DISCARD" || type == "PLAY") {
            Card card;
            parseCardKey(entry["card"].text, card);
            action = Action(type == "PLAY" ? ActionType::PlayCard : ActionType::Discard, card.id);
        } else if (type == "CONFIRM_DISCARD") {
            action = Action(ActionType::ConfirmDiscard);
        } else if (type == "ANNOUNCE") {
            action = Action(ActionType::AnnounceBonus,
                            static_cast<std::int16_t>(bonusOf(profile, entry["bonus"].text)));
        } else if (type == "KONTRA") {
            // The fixtures name the item; the engine counts them in order.
            std::int16_t index = 0;
            const std::string target = entry["posten"].text;
            for (std::size_t k = 0; k < core.declarations().size(); ++k) {
                const Declaration& declaration = core.declarations()[k];
                const bool isGame = declaration.bonus == BonusId::None;
                if ((target == "GAME" && isGame)
                    || (!isGame && target == profile.bonus(declaration.bonus).key))
                    index = static_cast<std::int16_t>(k);
            }
            action = Action(ActionType::Kontra, index);
        } else if (type == "READY") {
            action = Action(ActionType::Ready);
        } else if (type == "CONCEDE") {
            action = Action(ActionType::Concede);
        } else {
            CHECK_MSG(false, id + ": unknown action " + type);
            continue;
        }
        Reason reason;
        const bool ok = core.apply(seat, action, &reason);
        CHECK_MSG(ok, id + ": action " + std::to_string(i) + " " + type + " refused with "
                          + reasonKey(reason.code));
        if (!ok)
            return;
    }

    // --- results ----------------------------------------------------------
    const Json& expected = fixture["expected"];
    CHECK_MSG(core.handOver(), id + ": the hand did not finish");
    CHECK_MSG(profile.contract(core.contract()).key == expected["contract"].text,
              id + ": contract " + std::string(profile.contract(core.contract()).key));
    CHECK_MSG(core.declarer() == expected["declarer"].toInt(), id + ": declarer");
    if (expected.has("partner"))
        CHECK_MSG(core.partner() == expected["partner"].toInt(), id + ": partner");

    const Json& tricks = expected["tricks"];
    for (std::size_t i = 0; i < tricks.size(); ++i) {
        const int winner = tricks[i].type == Json::Type::Object ? tricks[i]["winner"].toInt()
                                                                : tricks[i].toInt();
        CHECK_MSG(core.trickWinner(static_cast<int>(i) + 1) == winner,
                  id + ": winner of trick " + std::to_string(i + 1) + " is "
                      + std::to_string(core.trickWinner(static_cast<int>(i) + 1)) + ", expected "
                      + std::to_string(winner));
    }

    const Json& count = expected["count"];
    if (count.has("declarerUnits")) {
        CHECK_MSG(core.partyPoints(true).units == count["declarerUnits"].toInt(),
                  id + ": declarer thirds " + std::to_string(core.partyPoints(true).units)
                      + ", expected " + std::to_string(count["declarerUnits"].toInt()));
        CHECK_MSG(core.partyPoints(false).units == count["defenderUnits"].toInt(),
                  id + ": defender thirds " + std::to_string(core.partyPoints(false).units));
    }

    const Ledger& ledger = core.ledger();
    CHECK_MSG(ledger.zeroSum(), id + ": the ledger does not add up to zero");
    for (const char* account : {"schrift", "geld"}) {
        const Json& values = expected[account];
        for (std::size_t seat = 0; seat < values.size(); ++seat) {
            const int actual = std::string(account) == "schrift"
                    ? ledger.schrift[seat] : ledger.geld[seat];
            CHECK_MSG(actual == values[seat].toInt(),
                      id + ": " + account + " of seat " + std::to_string(seat) + " is "
                          + std::to_string(actual) + ", expected " + std::to_string(values[seat].toInt()));
        }
    }
    std::printf("  %s: %s replayed\n", id.c_str(), fixture["title"].text.c_str());
}

} // namespace

int main(int argc, char** argv)
{
    // The fixtures live in the source tree; CMake passes its location, so the
    // test does not depend on the directory it is started from. An explicit
    // argument still wins.
#ifdef TAROCK_DATA_DIR
    const std::string fallback = std::string(TAROCK_DATA_DIR) + "/tests/fixtures";
#else
    const std::string fallback = "tests/fixtures";
#endif
    const std::string dir = argc > 1 ? argv[1] : fallback;
    std::printf("Replaying the practice hands of docs/koenigrufen.md §11.7\n");
    for (const char* name : {"kr_hand1.json", "kr_hand2.json", "kr_hand3.json"})
        replay(dir + "/" + name);
    if (failures) {
        std::printf("%d failures\n", failures);
        return 1;
    }
    std::printf("OK\n");
    return 0;
}
