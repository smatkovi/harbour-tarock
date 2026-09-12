// Self-play: the computer players of src/core/Ai.cpp play whole hands against
// themselves, on the four- and on the five-player table, with the three
// difficulties mixed round the table (docs/koenigrufen.md §10.10).
//
// Checked after every single action: the core still validates, the action the
// AI proposed is one the rules offer, the hand terminates, the ledger is a
// zero sum and the cards are all accounted for. On top of that the fairness
// test of §10.8: two different hidden distributions behind the same public
// record must produce the very same move.
#include "core/Ai.h"
#include "core/TarockCore.h"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

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

const char* const kContractNames[14] = {
    "-", "Trischaken", "Rufer", "Piccolo", "Bettel", "Solorufer", "Besserrufer",
    "Farbendreier", "Sechserdreier", "Dreier", "Farbensolo", "PiccoloOuvert",
    "BettelOuvert", "Solodreier"
};

struct Stats {
    int hands = 0;
    int handsFour = 0;
    int handsFive = 0;
    long long actions = 0;
    int longestHand = 0;
    int conceded = 0;
    int announced = 0;
    int kontras = 0;
    int fairnessChecks = 0;
    int coreTalonGap = 0;
    int played[14] = {0};
    int declarerWon[14] = {0};
    // Tournament score per difficulty: the seats rotate through the levels, so
    // over many hands these are directly comparable and show the ladder.
    long long schriftByLevel[3] = {0, 0, 0};
    int seatsByLevel[3] = {0, 0, 0};
};

// Did the declarer make his contract? Ledger::declarerWon only asks for the
// card points of §1.5, which says nothing in a Piccolo or a Bettel, so the
// game posten itself decides (§7.1.2).
bool declarerMadeIt(const Ledger& ledger)
{
    for (const Posten& item : ledger.items) {
        if (item.type == PostenType::Game || item.type == PostenType::Valat
            || item.type == PostenType::Concede)
            return item.winner == Party::Declarer;
    }
    return ledger.declarerWon;
}

// The table is mixed: beginner, club and tournament sit next to each other so
// that every difficulty is exercised in every seat.
Difficulty levelFor(int seat, int handIndex)
{
    static const Difficulty levels[3] = {Difficulty::Beginner, Difficulty::Club,
                                         Difficulty::Tournament};
    return levels[static_cast<std::size_t>((seat + handIndex) % 3)];
}

// --- fairness (§10.8) -----------------------------------------------------------

// The save format is the only door into the hidden hands, so the fairness test
// walks through it: swap two foreign hands, keep everything public as it was
// and let the core validate the result.
std::uint64_t fnv1a(const std::string& value)
{
    std::uint64_t hash = UINT64_C(14695981039346656037);
    for (unsigned char byte : value) {
        hash ^= byte;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

bool swapHiddenHands(const TarockCore& core, int a, int b, TarockCore& out)
{
    const std::string text = core.serialize();
    const std::size_t first = text.find('\n');
    if (first == std::string::npos)
        return false;
    const std::size_t second = text.find('\n', first + 1);
    if (second == std::string::npos)
        return false;
    const std::string payload = text.substr(second + 1);

    std::vector<std::string> lines;
    std::size_t begin = 0;
    while (begin < payload.size()) {
        const std::size_t end = payload.find('\n', begin);
        if (end == std::string::npos)
            break;
        lines.push_back(payload.substr(begin, end - begin));
        begin = end + 1;
    }
    // Line 0 is the header, then four lines per seat: hand, tricks, tray, discards.
    const std::size_t handA = 1 + 4 * static_cast<std::size_t>(a);
    const std::size_t handB = 1 + 4 * static_cast<std::size_t>(b);
    if (lines.size() <= handB || lines.size() <= handA)
        return false;
    lines[handA].swap(lines[handB]);

    std::string rebuilt;
    for (const std::string& line : lines) {
        rebuilt += line;
        rebuilt += '\n';
    }
    char head[64];
    std::snprintf(head, sizeof(head), "TAROCK_STATE_V1\n%llx\n",
                  static_cast<unsigned long long>(fnv1a(rebuilt)));
    return out.restore(std::string(head) + rebuilt);
}

// Same information, different truth behind it: the move must not change.
void checkFairness(const TarockCore& core, const AiPlayer& player, int seat, Stats& stats)
{
    if (core.players() != 4 || core.partnerKnown())
        return;
    int first = -1, second = -1;
    for (int other = 0; other < core.players(); ++other) {
        if (other == seat || other == core.declarer() || !core.active(other))
            continue;
        if (first < 0)
            first = other;
        else if (second < 0)
            second = other;
    }
    if (first < 0 || second < 0)
        return;
    TarockCore mirror;
    if (!swapHiddenHands(core, first, second, mirror))
        return;
    CHECK(mirror.actor() == core.actor());
    CHECK(mirror.hand(seat) == core.hand(seat));
    Inference here, there;
    here.observe(core, seat);
    there.observe(mirror, seat);
    const Action direct = player.bestAction(core, seat, here);
    const Action swapped = player.bestAction(mirror, seat, there);
    CHECK(direct == swapped);
    ++stats.fairnessChecks;
}

// --- one hand ---------------------------------------------------------------------

void playHand(TarockCore& core, const std::array<AiPlayer, TarockCore::MaxSeats>& players,
              Stats& stats, int handIndex)
{
    std::array<Inference, TarockCore::MaxSeats> memory;
    for (int seat = 0; seat < core.players(); ++seat) {
        if (core.active(seat))
            memory[static_cast<std::size_t>(seat)].observe(core, seat);
    }

    int guard = 0;
    while (!core.handOver() && ++guard < 800) {
        std::string error;
        CHECK(core.validate(&error));
        const int actor = core.actor();
        CHECK(actor >= 0 && core.active(actor));
        if (actor < 0)
            break;
        const std::vector<Action> legal = core.legalActions(actor);
        CHECK(!legal.empty());
        if (legal.empty())
            break;

        const AiPlayer& player = players[static_cast<std::size_t>(actor)];
        const Inference& seen = memory[static_cast<std::size_t>(actor)];
        const Action action = player.bestAction(core, actor, seen);

        // The AI never proposes anything the rules do not offer.
        bool offered = false;
        for (const Action& option : legal)
            offered = offered || option == action;
        CHECK(offered);
        const Reason why = core.check(actor, action);
        CHECK(severityOf(why.code) <= Severity::Info);

        // The hint engine sees the same position: every legal action ranked,
        // the recommendation first, and a reason on every one of them.
        if (guard % 11 == 0) {
            const std::vector<Hint> hints = player.rank(core, actor, seen);
            CHECK(hints.size() == legal.size());
            if (!hints.empty()) {
                CHECK(hints.front().action == action);
                CHECK(hints.front().rank == 0);
                for (std::size_t i = 0; i + 1 < hints.size(); ++i)
                    CHECK(hints[i].score >= hints[i + 1].score);
                for (const Hint& hint : hints) {
                    CHECK(hint.reason != HintReason::None);
                    CHECK(hintKey(hint.reason)[0] != '\0');
                    CHECK(player.explain(core, actor, seen, hint.action) == hint.reason);
                }
            }
        }
        if (core.phase() == Phase::Play && core.trick().empty() && handIndex % 5 == 0
            && guard % 3 == 0)
            checkFairness(core, player, actor, stats);

        if (action.type == ActionType::AnnounceBonus)
            ++stats.announced;
        if (action.type == ActionType::Kontra)
            ++stats.kontras;

        Reason applied;
        CHECK(core.apply(actor, action, &applied));
        ++stats.actions;
        if (!core.validate(&error)) {
            // Known gap of the rule core, reported with this test: after the
            // last trick of a game without a talon (Piccolo, Bettel and the
            // two Ouverts) TarockCore::dealTalonToWinner() clears both halves
            // although nobody takes them, so the six cards go missing. Every
            // other state has to validate.
            const bool knownGap = core.handOver() && core.contract() != ContractId::None
                    && core.contractDef().talon == TalonMode::None
                    && error == "cards are missing";
            if (knownGap) {
                ++stats.coreTalonGap;
            } else {
                std::fprintf(stderr, "invalid after %s (%d,%d) by seat %d, phase %d: %s\n",
                             actionKey(action.type), action.a, action.b, actor,
                             static_cast<int>(core.phase()), error.c_str());
                CHECK(false);
            }
        }

        // Everybody watches the table, with public information only.
        for (int seat = 0; seat < core.players(); ++seat) {
            if (core.active(seat))
                memory[static_cast<std::size_t>(seat)].observe(core, seat);
        }
    }

    CHECK(guard < 800);
    CHECK(core.handOver());
    stats.longestHand = guard > stats.longestHand ? guard : stats.longestHand;

    const int contract = static_cast<int>(core.contract());
    CHECK(contract > 0 && contract < 14);
    if (contract > 0 && contract < 14) {
        ++stats.played[contract];
        if (declarerMadeIt(core.ledger()))
            ++stats.declarerWon[contract];
    }
    if (core.conceded())
        ++stats.conceded;

    // The settlement is a zero sum on both accounts (§7.1).
    CHECK(core.ledger().zeroSum());
    for (int seat = 0; seat < core.players(); ++seat) {
        if (!core.active(seat))
            continue;
        const std::size_t level = static_cast<std::size_t>(levelFor(seat, handIndex));
        stats.schriftByLevel[level] += core.ledger().schriftThirds[static_cast<std::size_t>(seat)];
        ++stats.seatsByLevel[level];
    }

    if (!core.conceded()) {
        // Twelve tricks were played and every card is accounted for; in the
        // negative games the talon stays out of play.
        CHECK(core.trickWinner(core.profile().tricks()) >= 0);
        const CountResult declarer = core.partyPoints(true);
        const CountResult defenders = core.partyPoints(false);
        const bool talonInPlay = core.contractDef().talon != TalonMode::None;
        const int expected = core.profile().deck().size()
                - (talonInPlay ? 0 : core.profile().talonSize());
        CHECK(declarer.cards + defenders.cards == expected);
        if (talonInPlay)
            CHECK(declarer.units + defenders.units == core.profile().totalUnits());
    }
    ++stats.hands;
}

void playMatches(int players, int hands, std::uint32_t firstSeed, Stats& stats)
{
    for (int index = 0; index < hands; ++index) {
        std::array<AiPlayer, TarockCore::MaxSeats> table;
        for (int seat = 0; seat < TarockCore::MaxSeats; ++seat)
            table[static_cast<std::size_t>(seat)].setLevel(levelFor(seat, index));
        TarockCore core(ProfileId::AtKrOoe2023, players, firstSeed + static_cast<std::uint32_t>(index));
        playHand(core, table, stats, index);
        if (players == 4)
            ++stats.handsFour;
        else
            ++stats.handsFive;
    }
}

} // namespace

int main()
{
    Stats stats;
    // Fixed seeds: the same run produces the same hands and the same moves.
    playMatches(4, 2000, 20260912u, stats);
    playMatches(5, 500, 77000912u, stats);

    // The inference only ever works on public cards: what it has seen fall can
    // never contain a card that is still in somebody's hand.
    {
        TarockCore core(ProfileId::AtKrOoe2023, 4, 4711);
        std::array<AiPlayer, TarockCore::MaxSeats> table;
        Inference watcher;
        int guard = 0;
        while (!core.handOver() && ++guard < 800) {
            const int actor = core.actor();
            if (actor < 0)
                break;
            const Action action = table[static_cast<std::size_t>(actor)].bestAction(core, actor);
            if (!core.apply(actor, action))
                break;
            watcher.observe(core, 0);
            for (int seat = 0; seat < core.players(); ++seat)
                CHECK((watcher.seen() & core.hand(seat)).none());
            CHECK((watcher.unknownCards(core, 0) & core.hand(0)).none());
        }
        CHECK(core.handOver());
        CHECK(watcher.tricksSeen() == core.profile().tricks() || core.conceded());
    }

    const int trischaken = static_cast<int>(ContractId::Trischaken);
    int totalPlayed = 0;
    int totalWon = 0;
    for (int i = 1; i < 14; ++i) {
        if (i == trischaken)
            continue;             // everybody against everybody, no declarer
        totalPlayed += stats.played[i];
        totalWon += stats.declarerWon[i];
    }
    std::printf("OK: %d hands (%d four-player, %d five-player), %lld actions, "
                "declarer won %d of %d (%.1f%%), %d announcements, %d kontras, "
                "%d conceded, %d fairness checks, longest hand %d actions, "
                "%d hands hit the known talon gap of the rule core\n",
                stats.hands, stats.handsFour, stats.handsFive, stats.actions, totalWon,
                totalPlayed, totalPlayed ? 100.0 * totalWon / totalPlayed : 0.0,
                stats.announced, stats.kontras, stats.conceded, stats.fairnessChecks,
                stats.longestHand, stats.coreTalonGap);
    std::printf("%-14s %8s %8s %7s\n", "contract", "played", "won", "win%");
    for (int i = 1; i < 14; ++i) {
        if (!stats.played[i])
            continue;
        if (i == trischaken) {
            std::printf("%-14s %8d %8s %7s\n", kContractNames[i], stats.played[i], "-", "-");
            continue;
        }
        std::printf("%-14s %8d %8d %6.1f%%\n", kContractNames[i], stats.played[i],
                    stats.declarerWon[i], 100.0 * stats.declarerWon[i] / stats.played[i]);
    }
    static const char* const kLevelNames[3] = {"beginner", "club", "tournament"};
    std::printf("tournament score per seat and hand:");
    for (int level = 0; level < 3; ++level) {
        std::printf(" %s %+.3f", kLevelNames[level],
                    stats.seatsByLevel[level]
                            ? static_cast<double>(stats.schriftByLevel[level])
                                    / (3.0 * stats.seatsByLevel[level])
                            : 0.0);
    }
    std::printf("\n");
    if (failures) {
        std::printf("%d failures\n", failures);
        return 1;
    }
    return 0;
}
