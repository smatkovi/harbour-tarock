// The computer players of docs/koenigrufen.md §10. Every decision is a pure
// function of what one seat may legally see: its own cards, the public course
// of the hand and what §10.7 lets it deduce from them. The hint engine of the
// learning mode calls the very same code, so every branch names the reason it
// acted on.
//
// This is the heuristic baseline of M3/M4. The PIMC search of §10.1 hooks in
// at pimcAdjust() below and is not implemented yet; until it is, Club and
// Tournament play the same cards and differ only in how much of the inference
// they use.
#include "Ai.h"

#include <algorithm>
#include <cstdlib>

namespace tarock {

const char* hintKey(HintReason reason)
{
    switch (reason) {
    case HintReason::None: return "none";
    case HintReason::HandTooWeak: return "hand_too_weak";
    case HintReason::HandStrongEnough: return "hand_strong_enough";
    case HintReason::BirdCountSufficient: return "bird_count_sufficient";
    case HintReason::SoloWorthIt: return "solo_worth_it";
    case HintReason::ContractEconomy: return "contract_economy";
    case HintReason::SeatPosition: return "seat_position";
    case HintReason::ForehandMustChoose: return "forehand_must_choose";
    case HintReason::CallFromGuardedQueen: return "call_from_guarded_queen";
    case HintReason::CallShortSuit: return "call_short_suit";
    case HintReason::PreferTarockOverPoints: return "prefer_tarock_over_points";
    case HintReason::VoidASuit: return "void_a_suit";
    case HintReason::KeepCalledSuit: return "keep_called_suit";
    case HintReason::ProtectKing: return "protect_king";
    case HintReason::TalonHalfRicher: return "talon_half_richer";
    case HintReason::BonusSafe: return "bonus_safe";
    case HintReason::BonusRisky: return "bonus_risky";
    case HintReason::KontraWorthwhile: return "kontra_worthwhile";
    case HintReason::SignalToPartner: return "signal_to_partner";
    case HintReason::NothingToAnnounce: return "nothing_to_announce";
    case HintReason::DrawTrumps: return "draw_trumps";
    case HintReason::SmearToPartner: return "smear_to_partner";
    case HintReason::DuckCheaply: return "duck_cheaply";
    case HintReason::KeepStopper: return "keep_stopper";
    case HintReason::LeadShortKing: return "lead_short_king";
    case HintReason::SaveBirdForTarget: return "save_bird_for_target";
    case HintReason::LastTrickPlan: return "last_trick_plan";
    case HintReason::CountSaysEnough: return "count_says_enough";
    case HintReason::CountSaysMustAttack: return "count_says_must_attack";
    case HintReason::OnlyOneCard: return "only_one_card";
    case HintReason::FollowSuitForced: return "follow_suit_forced";
    }
    return "none";
}

namespace {

// --- small card helpers -------------------------------------------------------

// Walking a CardSet without building a vector; the play scorers call this for
// every candidate card of every trick.
template <typename Fn>
void forEachCard(const CardSet& set, Fn fn)
{
    for (std::size_t id = 0; id < set.size(); ++id) {
        if (set.test(id))
            fn(Card(static_cast<std::uint8_t>(id)));
    }
}

int setCount(const CardSet& set) { return static_cast<int>(set.count()); }

// The trick comparison of §6.6, the same one the rule core plays by.
bool beatsCard(const ContractDef& def, Card candidate, Card best)
{
    const bool suitGame = def.play == PlayMode::SuitGame;
    if (candidate.tarock() && best.tarock())
        return candidate.tarockNo() > best.tarockNo();
    if (candidate.tarock())
        return !suitGame;
    if (best.tarock())
        return false;
    return candidate.suit() == best.suit() && power(candidate) > power(best);
}

// "Stecher" of §10.2: a tarock from XVI up, the cards that really take tricks.
bool stecher(Card card) { return card.tarock() && card.tarockNo() >= 16; }

bool isBird(BonusId bonus)
{
    return bonus == BonusId::Pagat || bonus == BonusId::Uhu || bonus == BonusId::Kakadu
            || bonus == BonusId::Quapil;
}

// --- what one hand is worth (§10.2) -------------------------------------------

struct HandFacts {
    int cards = 0;
    int tarocks = 0;
    int stechers = 0;
    int trull = 0;              // Sküs, XXI, I
    int kings = 0;
    int birds = 0;              // I..IIII
    int birdMask = 0;           // bit 1..4 for I..IIII
    int bidPoints = 0;          // the 10/12/16-point count of §10.2
    int cardPoints = 0;         // card points of the hand in profile units
    int lowCards = 0;           // cards that can hardly ever take a trick
    int bareFigures = 0;        // a figure with no card under it
    int suitWinners = 0;        // sure suit tricks — the measure of the Farbenspiele
    int voidSuits = 0;
    int shortSuits = 0;         // at most one card
    int suitLen[4] = {0, 0, 0, 0};
    bool king[4] = {false, false, false, false};
    bool queen[4] = {false, false, false, false};
    bool knight[4] = {false, false, false, false};
    bool jack[4] = {false, false, false, false};
    bool skues = false;
    bool mond = false;
    bool pagat = false;
};

HandFacts evaluate(const RuleProfile& profile, const CardSet& hand)
{
    HandFacts f;
    forEachCard(hand, [&](Card card) {
        ++f.cards;
        f.cardPoints += profile.cardValue(card);
        if (card.tarock()) {
            ++f.tarocks;
            f.bidPoints += stecher(card) ? 2 : 1;
            if (stecher(card))
                ++f.stechers;
            if (card.honour())
                ++f.trull;
            if (card.bird()) {
                ++f.birds;
                f.birdMask |= 1 << card.tarockNo();
            }
            f.skues = f.skues || card == kSkues;
            f.mond = f.mond || card == kMond;
            f.pagat = f.pagat || card == kPagat;
            if (card.tarockNo() <= 6)
                ++f.lowCards;
            return;
        }
        const int suit = static_cast<int>(card.suit());
        ++f.suitLen[suit];
        switch (card.slot()) {
        case 0: f.king[suit] = true; ++f.kings; ++f.bidPoints; break;
        case 1: f.queen[suit] = true; break;
        case 2: f.knight[suit] = true; break;
        case 3: f.jack[suit] = true; break;
        default: ++f.lowCards; break;
        }
    });
    for (int suit = 0; suit < 4; ++suit) {
        const int len = f.suitLen[suit];
        if (len == 0)
            ++f.voidSuits;
        if (len <= 1)
            ++f.shortSuits;
        const int figures = (f.king[suit] ? 1 : 0) + (f.queen[suit] ? 1 : 0)
                + (f.knight[suit] ? 1 : 0) + (f.jack[suit] ? 1 : 0);
        if (figures > 0 && len - figures == 0 && len <= 2)
            f.bareFigures += figures;
        // Winners from the top down: the king takes, king and queen take two,
        // and a closed long suit takes what is left of it (§10.2 Farbenspiele).
        int top = 0;
        if (f.king[suit]) {
            top = 1;
            if (f.queen[suit]) {
                top = 2;
                if (f.knight[suit]) {
                    top = 3;
                    if (f.jack[suit])
                        top = 4;
                }
            }
        }
        f.suitWinners += top;
        if (f.king[suit] && len >= 5)
            f.suitWinners += len - 4;
    }
    return f;
}

// --- the seat's view of the hand ----------------------------------------------

struct Scored {
    int score = 0;
    HintReason reason = HintReason::None;
    Reason detail;
};

// A card one of our own announcements binds to a later trick (§6.5).
struct Bound {
    Card card;
    int trick = 0;
    bool announced = false;     // false: a silent bonus we would like to keep
};

// Everything one seat may look at while it decides, gathered once per call.
// Nothing in here is ever read from another seat's hand: the only hand this
// struct touches is core.hand(seat) (§10.8).
struct View {
    const TarockCore* core = nullptr;
    const RuleProfile* profile = nullptr;
    const ContractDef* def = nullptr;      // null while no contract stands
    const Inference* inf = nullptr;
    Difficulty level = Difficulty::Club;
    int seat = -1;
    int budgetMs = 0;

    CardSet hand;                          // own cards, without the discard tray
    HandFacts facts;

    ContractId contract = ContractId::None;
    bool negative = false;
    bool suitGame = false;
    bool tarockGame = false;
    int calledSuit = -1;
    bool iAmDeclarer = false;
    bool iAmPartner = false;               // I hold the called king
    bool myDeclarerSide = false;

    bool inPlay = false;
    bool leading = true;
    Suit lead = Suit::Tarock;
    Card best;
    int bestSeat = -1;
    int myIndex = 0;
    int trickUnits = 0;
    int trickNumber = 0;
    int tricksLeft = 12;
    int lastTrick = 12;
    bool mustFollow = false;               // every legal card follows the lead

    int myUnits = 0;
    int theirUnits = 0;
    bool enough = false;
    bool mustAttack = false;

    CardSet unknown;                       // may still sit in another hand
    int topUnknownTarock = 0;
    int topUnknownPower[4] = {0, 0, 0, 0};

    std::array<Bound, 6> bound{};
    int boundCount = 0;
    bool opponentBirdDue = false;
    bool iAnnouncedBird = false;
    int declarerTricks = 0;
};

// Which side a seat plays on, seen from `me`: +1 my side, -1 the other side,
// 0 not deducible yet. Only public facts and my own cards go in (§10.8): the
// partnership becomes public when the called king falls or its holder says
// "König ultimo", and until then the declarer only knows himself.
int sideOf(const View& v, int other)
{
    const TarockCore& core = *v.core;
    if (other == v.seat)
        return 1;
    if (!v.def || v.def->partner == PartnerMode::EveryoneAlone)
        return -1;                                  // Trischaken: everyone alone
    const bool caller = v.def->partner == PartnerMode::CallKing;
    if (other == core.declarer())
        return v.myDeclarerSide ? 1 : -1;
    if (!caller)
        return v.myDeclarerSide ? -1 : 1;           // the declarer plays alone
    if (core.partnerKnown() && core.partner() >= 0) {
        const bool otherIsDeclarerSide = other == core.partner();
        return otherIsDeclarerSide == v.myDeclarerSide ? 1 : -1;
    }
    if (v.iAmPartner)
        return -1;                                  // I am the partner, the rest are opponents
    return 0;                                       // still hidden
}

// A defender does not know which of the two other seats holds the called king,
// but two out of three of them play with him — enough to smear carefully.
bool probablyMine(const View& v, int other)
{
    return !v.myDeclarerSide && other != v.core->declarer() && other != v.seat;
}

// --- bidding (§10.2) ------------------------------------------------------------

// How far a hand exceeds what a contract asks for. Negative means too weak;
// the bidder takes the highest contract that is not. The thresholds are the
// table of §10.2, read before the talon.
// Die Blattbewertung des Dreierspiels (docs/tapptarock.md §10.2), in
// Zehnteln gerechnet, damit es ohne Kommazahlen geht. Die "Mittelstecher"
// (X bis XV) stehen nicht in den HandFacts; sie ergeben sich als die Tarock,
// die weder Stecher (>= XVI) noch niedrig (<= VI) sind.
int tappScore(const HandFacts& f)
{
    const int highs = f.stechers - (f.skues ? 1 : 0) - (f.mond ? 1 : 0);
    const int middles = std::max(0, f.tarocks - f.stechers - f.lowCards);
    return 16 * (f.tarocks - 7)
            + (f.skues ? 30 : 0) + (f.mond ? 22 : 0) + (f.pagat ? 10 : 0)
            + 9 * std::max(0, highs)
            + 4 * middles
            + 11 * f.kings
            + 8 * f.voidSuits
            - 7 * f.bareFigures;
}

// Strohmandeln (strohmandeln.md §10.2): gezählt wird der Wert der "sicheren
// Stecher" in Dritteln -- Trullstück und König 4 1/3, eine gedeckte Dame
// 3 1/3, ein gedeckter Cavall 2 1/3. Mayer & Schiffner nennen 20 bis 25 Punkte
// als den Bereich, in dem sich die Aufnahme lohnt, und mindestens sechs bis
// sieben Tarock als Bedingung. Nachgerechnet gibt die Funktion für die beiden
// Blätter des Originals 19 2/3 und 24 Punkte.
int strohScore(const HandFacts& f)
{
    int thirds = 0;
    if (f.skues)
        thirds += 13;
    if (f.mond)
        thirds += 13;
    // Der Pagat ist im Strohmandeln "meist heimzubringen" und zählt mit.
    if (f.pagat)
        thirds += 13;
    for (int suit = 0; suit < 4; ++suit) {
        if (f.king[suit])
            thirds += 13;
        // Eine Dame zählt, wenn der eigene König dabei ist oder sie mehrfach
        // besetzt ist; ein Cavall erst hinter König und Dame.
        if (f.queen[suit] && (f.king[suit] || f.suitLen[suit] >= 3))
            thirds += 10;
        if (f.knight[suit] && f.king[suit] && f.queen[suit])
            thirds += 7;
    }
    return thirds;
}

int contractMargin(ContractId id, const HandFacts& f)
{
    switch (id) {
    case ContractId::Trischaken:
        // Few and low tarocks, no kings: there is nothing to win here, so the
        // point is to lose nothing.
        return 2 - f.kings * 3 - f.stechers * 3 - std::max(0, f.tarocks - 4) - f.cardPoints / 8;
    case ContractId::Rufer:
        // Several tarocks with a Stecher, plus a Trull piece or a king.
        return (f.tarocks - 4) + f.stechers + (f.trull > 0 || f.kings > 0 ? 1 : -2);
    case ContractId::Piccolo:
    case ContractId::PiccoloOuvert: {
        // One planned trick, otherwise small tarocks and many low cards.
        const bool shortKing = (f.king[0] && f.suitLen[0] <= 2) || (f.king[1] && f.suitLen[1] <= 2)
                || (f.king[2] && f.suitLen[2] <= 2) || (f.king[3] && f.suitLen[3] <= 2);
        if (!f.skues && !f.mond && !shortKing)
            return -100;
        const int spare = f.tarocks - (f.skues || f.mond ? 1 : 0);
        const int margin = std::min(std::min(f.lowCards - 6, 3 - spare), 1 - f.bareFigures);
        return id == ContractId::Piccolo ? margin : margin - 4;
    }
    case ContractId::Bettel:
    case ContractId::BettelOuvert: {
        // Four-suited or without tarocks, no bare figures, many low cards.
        if (f.tarocks > 1 || (f.tarocks == 1 && !f.pagat))
            return -100;
        const int margin = std::min(f.lowCards - 7, 1 - f.bareFigures) - f.kings * 2;
        return id == ContractId::Bettel ? margin : margin - 4;
    }
    case ContractId::Solorufer:
        // 5–6 tarocks with Sküs or Mond, a king, many suits and bare in one of
        // them: the game is played without a talon, so the bare suit that lets
        // the partner come in is not optional.
        if ((!f.skues && !f.mond) || f.kings < 1 || f.tarocks < 5 || f.voidSuits < 1)
            return -100;
        return std::min(f.tarocks - 5, f.shortSuits - 1);
    case ContractId::Besserrufer: {
        // Pagat 6 tarocks with 1–2 Stechern, Uhu 7/2–3, Kakadu 8–9/≥3,
        // Quapil 9–10/≥3. The bid itself is one and the same in this profile.
        if (f.birds == 0)
            return -100;
        static const int needTarock[5] = {0, 6, 7, 8, 9};
        static const int needStecher[5] = {0, 1, 2, 3, 3};
        int best = -100;
        for (int bird = 1; bird <= 4; ++bird) {
            if (!(f.birdMask & (1 << bird)))
                continue;
            best = std::max(best, std::min(f.tarocks - needTarock[bird],
                                           f.stechers - needStecher[bird]));
        }
        return best;
    }
    case ContractId::Farbendreier:
        // Six or seven sure tricks; the talon half brings one more.
        return f.suitWinners + 1 - 6 - std::max(0, f.tarocks - 4);
    case ContractId::Sechserdreier: {
        // The 10-point rule, Sküs with six tarocks or Mond with seven, and
        // never without a Trull piece unless the hand is all tarock.
        if (f.trull == 0 && f.tarocks < 9)
            return -100;
        const bool top = (f.skues && f.tarocks >= 6) || (f.mond && f.tarocks >= 7) || f.tarocks >= 9;
        if (!top)
            return -100;
        return f.bidPoints - 10;
    }
    case ContractId::Dreier:
        // The 12-point rule: 6–7 tarocks, 2–3 Stecher and a Trull piece.
        if (f.trull == 0 || f.tarocks < 6 || f.stechers < 2)
            return -100;
        return f.bidPoints - 12;
    case ContractId::Farbensolo:
        // About eight sure tricks and three kings.
        return std::min(f.suitWinners - 8, f.kings - 3);
    case ContractId::Solodreier:
        // The 16-point rule: 9–10 tarocks with 4–5 Stechern.
        if (f.tarocks < 9 || f.stechers < 4)
            return -100;
        return f.bidPoints - 16;
    // --- Tapp-Tarock zu dritt (tapptarock.md §10.3) -----------------------
    case ContractId::TappDreier:
        // Ab Score 2,0, und nur mit sieben Tarock oder zwei Trullstücken.
        if (f.tarocks < 7 && f.trull < 2)
            return -100;
        return (tappScore(f) - 20) / 10;
    case ContractId::TappUnterer:
    case ContractId::TappOberer:
        // Ab Score 4,0 -- aber nie als Eröffnung: die beiden sind nicht
        // stärker als der Dreier, nur teurer (§3.1). Das entscheidet
        // scoreBid(), hier steht nur die Blattstärke.
        return (tappScore(f) - 40) / 10;
    case ContractId::TappSolo:
        // Ab Score 8,5, und nur mit elf Tarock samt Sküs oder zehn mit Sküs
        // und XXI: der ganze Talon geht an die Gegner.
        if (!((f.tarocks >= 11 && f.skues) || (f.tarocks >= 10 && f.skues && f.mond)))
            return -100;
        return (tappScore(f) - 85) / 10;
    // --- Strohmandeln zu zweit (strohmandeln.md §10.2) --------------------
    case ContractId::StrohAufgenommen: {
        // Ohne sechs Tarock wird nicht aufgenommen, was immer das Blatt sonst
        // hergibt; darüber entscheidet der Stecherwert. Die Schwelle sind die
        // 20 Punkte aus §10.2. Sie ist absichtlich hoch: gewonnen zählt das
        // aufgenommene Spiel 3, verloren 4 für den Gegner (§3.3), es lohnt
        // sich also erst ab gut 57 von hundert Aussichten.
        if (f.tarocks < 6)
            return -100;
        const int over = strohScore(f) - 60;
        // Abgerundet, damit die 19 2/3 des ersten Beispiels aus §10.2 knapp
        // darunter bleiben -- aus dem letzten Sitz heraus reicht es dann doch,
        // und genau das nennt die Quelle "risikofreudig".
        return over >= 0 ? over / 3 : (over - 2) / 3;
    }
    case ContractId::StrohEinfach:
        return -100;      // das sagt niemand an, es wird das Spiel von selbst
    default:
        return -100;      // the Hungarian contracts have their own profile
    }
}

bool isSoloContract(ContractId id)
{
    return id == ContractId::Solorufer || id == ContractId::Farbensolo
            || id == ContractId::Solodreier;
}

// Which game stands on the table. The core keeps the standing bid to itself,
// so we read it back through its refusals: a contract that is not a forehand
// game is "too low" exactly while its rank is at or below the bid, and the
// game one may hold is never a forehand game (§3.3.4).
ContractId standingBid(const TarockCore& core, int seat)
{
    int rank = 0;
    ContractId id = ContractId::None;
    for (const ContractDef& def : core.profile().contracts()) {
        if (def.bidFlags & ForehandOnly)
            continue;
        const Action bid(ActionType::Bid, static_cast<std::int16_t>(def.id));
        if (core.check(seat, bid).code != ReasonCode::BidTooLow)
            continue;
        if (def.rank > rank) {
            rank = def.rank;
            id = def.id;
        }
    }
    return id;
}

// Bids score above "weiter" as soon as the hand carries them; the higher
// contract wins when two of them do, which is the economy rule of §10.2.
Scored scoreBid(const View& v, const Action& action)
{
    Scored out;
    const TarockCore& core = *v.core;
    const int forehand = core.forehand();
    const bool isForehand = v.seat == forehand;

    // "Be more careful as the first, bolder as the last" (§10.2): how many
    // seats still have to speak after me in this round. It shifts the
    // threshold of the table by one point, which is what the rule of thumb
    // means: the same hand is a game in the last seat and none in the first.
    int after = 0;
    for (int step = 1, seat = core.nextSeat(v.seat); step < core.players();
         ++step, seat = core.nextSeat(seat)) {
        if (core.active(seat) && !core.hasPassed(seat))
            ++after;
    }
    const int position = after == 0 ? 1 : (after >= 2 ? -1 : 0);

    if (action.type == ActionType::Pass) {
        out.score = 50;
        out.reason = HintReason::HandTooWeak;
        return out;
    }
    if (action.type == ActionType::OpenForehand) {
        // "Vorhand" keeps every game open except the Sechserdreier, which can
        // only be the opening word (§3.3.2), so it is almost always right.
        out.score = 400;
        out.reason = HintReason::ForehandMustChoose;
        return out;
    }
    if (action.type == ActionType::Hold) {
        // Holding means playing the game that stands, without overbidding it.
        const ContractId id = standingBid(core, v.seat);
        if (id == ContractId::None) {
            out.score = 40;
            out.reason = HintReason::HandTooWeak;
            return out;
        }
        const int margin = contractMargin(id, v.facts) + position;
        out.detail.contract = static_cast<std::int16_t>(id);
        out.score = margin >= 0 ? 110 + core.profile().contract(id).rank * 6 + margin * 4
                                : 50 + margin;
        out.reason = margin >= 0 ? HintReason::ContractEconomy : HintReason::HandTooWeak;
        return out;
    }
    if (action.type != ActionType::Bid)
        return out;

    const ContractId id = static_cast<ContractId>(action.a);
    const ContractDef& def = core.profile().contract(id);
    const int margin = contractMargin(id, v.facts) + position;
    out.detail.contract = static_cast<std::int16_t>(id);
    out.detail.count = static_cast<std::int16_t>(v.facts.bidPoints);
    if (margin < 0) {
        // The forehand may not pass while no game stands (§3.3.1), so a weak
        // hand still has to name the least bad of them.
        out.score = 50 + margin;
        out.reason = isForehand && core.bidHolder() < 0 ? HintReason::ForehandMustChoose
                                                        : HintReason::HandTooWeak;
        return out;
    }
    out.score = 100 + def.rank * 6 + margin * 4;
    if ((id == ContractId::TappUnterer || id == ContractId::TappOberer)
        && core.bidHolder() < 0) {
        // Als Eröffnung nie: wer ein Blatt für den Oberen hat, sagt den
        // billigeren Dreier an (tapptarock.md §10.3).
        out.score = 60;
        out.reason = HintReason::ContractEconomy;
        return out;
    }
    if (id == ContractId::Sechserdreier) {
        // Only the opening word can be a Sechserdreier; the Dreier is still
        // open later, so speak it now only when the Dreier is out of reach.
        if (contractMargin(ContractId::Dreier, v.facts) + position < 0)
            out.score = 500 + margin * 4;
    }
    out.reason = id == ContractId::Besserrufer ? HintReason::BirdCountSufficient
            : isSoloContract(id) ? HintReason::SoloWorthIt
            : isForehand && !core.hasPassed(v.seat) && core.bidHolder() < 0
              ? HintReason::ForehandMustChoose
            : position != 0 && margin <= 1 ? HintReason::SeatPosition
            : HintReason::HandStrongEnough;
    return out;
}

// --- calling a king (§10.3) ------------------------------------------------------

Scored scoreCall(const View& v, const Action& action)
{
    Scored out;
    if (action.b == 1) {
        // With three kings the fourth is called without naming the suit: it
        // tells the partner at once that this is no tarock hand.
        out.score = 300;
        out.reason = HintReason::SignalToPartner;
        return out;
    }
    const int suit = action.a;
    if (suit < 0 || suit > 3)
        return out;
    const int len = v.facts.suitLen[suit];
    const bool solo = v.contract == ContractId::Solorufer;
    int score = 100;
    out.detail.suit = static_cast<std::int16_t>(suit);
    if (len == 0) {
        // Never call out of a suit: one card of it has to stay in hand.
        score -= 60;
        out.reason = HintReason::CallShortSuit;
    } else if (v.facts.queen[suit]) {
        // A guarded queen is the classic call; in a Solorufer a bare queen is
        // better, because the partner can announce the king more easily.
        if (len >= 2) {
            score += solo ? 30 : 45;
            out.reason = HintReason::CallFromGuardedQueen;
        } else {
            score += solo ? 50 : 25;
            out.reason = HintReason::CallFromGuardedQueen;
        }
    } else if (v.facts.knight[suit]) {
        score += 22;
        out.reason = HintReason::CallFromGuardedQueen;
    } else {
        out.reason = HintReason::CallShortSuit;
    }
    // Short, but never bare: two or three cards are what the sources ask for.
    score += len == 2 ? 14 : len == 3 ? 9 : len == 1 ? 4 : 0;
    out.score = score;
    return out;
}

// --- talon (§10.4) ---------------------------------------------------------------

int talonCardValue(const View& v, Card card)
{
    if (v.suitGame) {
        // Tarocks take no suit trick here, kings and queens do.
        if (card.tarock())
            return 2;
        if (card.king())
            return 34;
        return 6 + power(card) * 2;
    }
    if (card.tarock()) {
        int value = card.honour() ? 40 : stecher(card) ? 26 : 13;
        if (card.bird())
            value += 6;               // a bird out of the talon can be announced on top
        return value;
    }
    if (card.king())
        return 24;
    return v.profile->cardValue(card) * 3;
}

Scored scoreTalon(const View& v, const Action& action)
{
    Scored out;
    if (action.type == ActionType::Concede) {
        // "Schleifen" pays game and announced bird at once (§7.7 a); only a
        // hand that cannot play alone against three gives up that cheaply.
        const bool hopeless = v.facts.bidPoints <= 5 && v.facts.stechers == 0;
        out.score = hopeless ? 260 : -100;
        out.reason = HintReason::HandTooWeak;
        return out;
    }
    if (action.type != ActionType::TakeTalon)
        return out;
    if (action.a < 0) {
        out.score = 100;              // Sechserdreier: all six, no choice
        out.reason = HintReason::TalonHalfRicher;
        return out;
    }
    const CardSet& half = v.core->talonHalf(action.a);
    int value = 0;
    forEachCard(half, [&](Card card) {
        value += talonCardValue(v, card);
        if (!card.tarock() && static_cast<int>(card.suit()) == v.calledSuit)
            value += card.king() ? 30 : 8;   // the called king in hand ends the search
        out.detail.cards.set(card.id);
    });
    out.score = 100 + value;
    out.reason = HintReason::TalonHalfRicher;
    return out;
}

// --- discarding (§4.6, §10.4) -----------------------------------------------------

Scored scoreDiscard(const View& v, const Action& action)
{
    Scored out;
    if (action.type == ActionType::ConfirmDiscard) {
        out.score = 1000;
        out.reason = HintReason::VoidASuit;
        return out;
    }
    const Card card(static_cast<std::uint8_t>(action.a));
    out.detail.card = card;
    // Ask the rules first: kings, Trull pieces and — while suit cards are
    // left — tarocks never go down (§4.6).
    const Reason why = v.core->cardReason(v.seat, card);
    if (severityOf(why.code) == Severity::Error) {
        out.score = -10000;
        out.detail = why;
        return out;
    }
    if (contains(v.core->discardTray(v.seat), card)) {
        out.score = -1000;            // taking a card back out undoes our own plan
        out.reason = HintReason::KeepStopper;
        return out;
    }
    int score = 0;
    if (card.tarock()) {
        // Only when nothing else is left, and then the small ones first; the
        // tarock has to be shown openly, so it also tells the table too much.
        score = -100 - card.tarockNo();
        out.reason = HintReason::PreferTarockOverPoints;
    } else {
        const int suit = static_cast<int>(card.suit());
        const int len = v.facts.suitLen[suit];
        score = 40 + (len <= 3 ? (4 - len) * 14 : 0);        // discard narrow
        score += v.profile->cardValue(card) * 5;             // banked points count for us
        out.reason = len <= 3 ? HintReason::VoidASuit : HintReason::PreferTarockOverPoints;
        if (v.facts.king[suit]) {
            score -= 45;                                     // the king needs its guards
            out.reason = HintReason::ProtectKing;
        }
        if (suit == v.calledSuit) {
            score -= 55;                                     // keep the called suit
            out.reason = HintReason::KeepCalledSuit;
        }
        if (card.slot() == 1 && len <= 2)
            score -= 12;                                     // a guarded queen is a trick
    }
    if (severityOf(why.code) == Severity::Info) {
        score -= 20;
        out.detail = why;
    }
    out.score = score;
    return out;
}

// --- announcements and kontra (§10.5) ----------------------------------------------

// The bird a Besserrufer has to announce (§5.3): the one its tarock length
// carries best, and among equals the cheapest one to lose.
BonusId mandatoryBirdOf(const HandFacts& f)
{
    static const int needTarock[5] = {0, 6, 7, 8, 9};
    static const int needStecher[5] = {0, 1, 2, 3, 3};
    static const BonusId ids[5] = {BonusId::None, BonusId::Pagat, BonusId::Uhu,
                                   BonusId::Kakadu, BonusId::Quapil};
    BonusId best = BonusId::None;
    int bestMargin = -1000;
    for (int bird = 1; bird <= 4; ++bird) {
        if (!(f.birdMask & (1 << bird)))
            continue;
        const int margin = std::min(f.tarocks - needTarock[bird], f.stechers - needStecher[bird]);
        if (margin > bestMargin) {
            bestMargin = margin;
            best = ids[bird];
        }
    }
    return best;
}

Scored scoreAnnounce(const View& v, const Action& action)
{
    Scored out;
    if (action.type == ActionType::Ready) {
        out.score = 100;              // the baseline every announcement has to beat
        out.reason = HintReason::NothingToAnnounce;
        return out;
    }
    if (action.type == ActionType::Kontra) {
        const std::vector<Declaration>& list = v.core->declarations();
        if (action.a < 0 || action.a >= static_cast<std::int16_t>(list.size()))
            return out;
        const Declaration& item = list[static_cast<std::size_t>(action.a)];
        out.detail.bonus = static_cast<std::int16_t>(item.bonus);
        const HandFacts& f = v.facts;
        bool worth = false;
        if (item.bonus == BonusId::None) {
            // Against a Dreier 6–7 tarocks with two Stechern and as few suits
            // as possible, against a Besserrufer one or two Stecher are
            // enough but the hand should cover all four suits.
            worth = v.contract == ContractId::Besserrufer
                    ? (f.tarocks >= 6 && f.stechers >= 1 && f.voidSuits == 0)
                    : (f.tarocks >= 6 && f.stechers >= 2 && f.shortSuits >= 2);
        } else if (item.bonus == BonusId::KingUltimo) {
            // Three cards of the called suit plus a Stecher, or the prospect
            // of the last trick.
            worth = v.calledSuit >= 0 && f.suitLen[v.calledSuit] >= 3 && f.stechers >= 1;
        } else if (isBird(item.bonus)) {
            // A bird only comes home in the ninth to twelfth trick, so it
            // takes a hand that still holds a high tarock at the end.
            worth = f.tarocks >= 6 && f.stechers >= 2;
        } else if (item.bonus == BonusId::Valat) {
            worth = f.stechers >= 1 || f.kings >= 1;
        } else {
            worth = f.stechers >= 2;
        }
        // Rekontra and Subkontra are left alone: the core only lets the party
        // that did not announce raise a posten, so every further step would
        // come from the same side again (§5.6).
        if (item.level >= 2)
            worth = false;
        if (v.level == Difficulty::Beginner)
            worth = worth && f.tarocks >= 8 && f.stechers >= 3;   // §10.9
        out.score = worth ? 250 : 0;
        out.reason = worth ? HintReason::KontraWorthwhile : HintReason::BonusRisky;
        return out;
    }
    if (action.type != ActionType::AnnounceBonus)
        return out;

    const BonusId bonus = static_cast<BonusId>(action.a);
    const HandFacts& f = v.facts;
    out.detail.bonus = static_cast<std::int16_t>(bonus);
    bool safe = false;
    int margin = 0;
    switch (bonus) {
    case BonusId::Pagat:
        // Losing the Pagat costs five card points on top, so only a hand that
        // still holds the table at the twelfth trick says it (§10.5).
        safe = f.pagat && f.tarocks >= 8 && f.skues && f.stechers >= 3;
        margin = f.tarocks - 8;
        break;
    case BonusId::Uhu:
    case BonusId::Kakadu:
    case BonusId::Quapil:
        // As a defender the big birds stay silent and wait for the endgame.
        safe = v.myDeclarerSide && f.tarocks >= 9 && f.stechers >= 4;
        margin = f.tarocks - 9;
        break;
    case BonusId::KingUltimo:
        // The king at least three deep plus three or four tarocks with a Stecher.
        safe = v.calledSuit >= 0 && f.suitLen[v.calledSuit] >= 3 && f.tarocks >= 3
                && f.stechers >= 1;
        margin = f.suitLen[v.calledSuit >= 0 ? v.calledSuit : 0] - 3;
        break;
    case BonusId::Trull:
        safe = f.trull == 3 && f.tarocks >= 6;
        margin = f.tarocks - 6;
        break;
    case BonusId::AllKings:
        // Four kings in hand, or three with the fourth one called.
        safe = f.kings == 4
                || (f.kings == 3 && v.iAmDeclarer && v.calledSuit >= 0
                    && !f.king[v.calledSuit] && f.tarocks >= 6);
        margin = f.tarocks - 6;
        break;
    case BonusId::Valat:
        safe = f.tarocks >= 10 && f.stechers >= 5 && f.kings >= 2;
        margin = f.tarocks - 10;
        break;
    default:
        safe = false;
        break;
    }
    if (v.level == Difficulty::Beginner && bonus != BonusId::KingUltimo)
        safe = safe && f.tarocks >= 9;        // the beginner keeps quiet (§10.9)
    out.score = safe ? 200 + margin * 8 : 10;
    out.reason = safe ? HintReason::BonusSafe : HintReason::BonusRisky;
    // The Besserrufer owes the table the bird it held before the talon; the
    // obligation beats every judgement about how safe it is (§5.3).
    if (v.contract == ContractId::Besserrufer && v.iAmDeclarer && !v.iAnnouncedBird
        && bonus == mandatoryBirdOf(f)) {
        out.score = 900;
        out.reason = HintReason::BirdCountSufficient;
    }
    return out;
}

// --- card play (§10.6, §10.7) --------------------------------------------------------

// What it costs to give a card away now. High tarocks and Trull pieces are the
// stoppers one keeps, birds are worth their own prize later.
int cardCost(const View& v, Card card)
{
    if (card.tarock()) {
        if (v.suitGame)
            return 2;                         // it takes no suit trick anyway (§6.3)
        int cost = card.tarockNo();
        if (card.honour())
            cost += 24;
        if (card.bird() && !v.myDeclarerSide)
            cost += 10;                       // a silent bird for the endgame (§10.5)
        return cost;
    }
    int cost = v.profile->cardValue(card) * 3;
    if (card.king())
        cost += 18;
    return cost;
}

// True when nothing that could beat this card can still be in another hand —
// the same public-only test §6.5 uses, never a look at a foreign hand.
bool safeWinner(const View& v, Card card)
{
    if (card.tarock())
        return v.topUnknownTarock <= card.tarockNo();
    if (!v.suitGame && v.topUnknownTarock > 0)
        return false;                          // anybody void in the suit may trump
    return v.topUnknownPower[static_cast<int>(card.suit())] <= power(card);
}

// Could a card still in another hand beat this one? Used the other way round
// in the negative games, where taking the trick is the thing to avoid.
bool stillBeatable(const View& v, Card card)
{
    if (card.tarock())
        return v.topUnknownTarock > card.tarockNo();
    if (v.topUnknownTarock > 0)
        return true;
    return v.topUnknownPower[static_cast<int>(card.suit())] > power(card);
}

// Hook for the PIMC/ISMCTS search of §10.1: determinise the unknown cards,
// score every determinisation with the settlement of §7 and average. Until
// that milestone lands it changes nothing, so the heuristic score stands.
bool pimcAdjust(const View& view, const Action& action, int* score)
{
    (void)view;
    (void)action;
    (void)score;
    return false;
}

Scored scoreNegativePlay(const View& v, Card card)
{
    Scored out;
    out.detail.card = card;
    const int value = v.profile->cardValue(card);
    const int rankInTrick = card.tarock() ? card.tarockNo() : power(card);
    const bool beatable = stillBeatable(v, card);

    // What this seat wants from the trick. In Piccolo and Bettel only the
    // declarer's tricks count, so the defenders steer his count (§10.6).
    int desire = -1;
    if (v.def->play == PlayMode::Trischaken) {
        desire = -1;                                      // "Jungfrau": no trick at all
    } else if (v.iAmDeclarer) {
        // The Piccolo takes its one trick with the card that is in the way
        // anyway and is a Bettel from then on (§10.6).
        const bool piccolo = v.contract == ContractId::Piccolo
                || v.contract == ContractId::PiccoloOuvert;
        desire = piccolo && v.declarerTricks == 0 ? 1 : -1;
    } else {
        const bool piccolo = v.contract == ContractId::Piccolo
                || v.contract == ContractId::PiccoloOuvert;
        const bool declarerWinning = v.bestSeat == v.core->declarer();
        if (piccolo && v.declarerTricks == 0)
            desire = 1;                                   // deny him his one trick
        else
            desire = declarerWinning ? -1 : 1;            // hang a second one on him
    }

    if (v.leading) {
        // Whoever wants the trick leads the card nobody can take away any
        // more; whoever does not wants the most dangerous card that somebody
        // can still beat — "die gefährlichste Karte zuerst entschärfen".
        if (desire > 0) {
            out.score = 200 + rankInTrick * 3 - (beatable ? 40 : 0);
            out.reason = HintReason::LeadShortKing;
        } else {
            out.score = (beatable ? 140 : 20) + rankInTrick * 2 + value * 4;
            out.reason = beatable ? HintReason::DuckCheaply : HintReason::KeepStopper;
        }
        return out;
    }

    const bool wins = beatsCard(*v.def, card, v.best);
    const bool points = v.def->play == PlayMode::Trischaken;
    if (desire > 0) {
        out.score = wins ? 180 - (points ? value * 6 : 0) : 40 - rankInTrick;
        out.reason = wins ? HintReason::CountSaysMustAttack : HintReason::DuckCheaply;
        return out;
    }
    if (!wins) {
        // Take no trick and get rid of the counters while doing it.
        out.score = 160 + value * 8 + rankInTrick;
        out.reason = HintReason::DuckCheaply;
        return out;
    }
    // Forced to take it: as cheaply as possible when points decide, otherwise
    // with the card that was most in the way.
    out.score = 20 + (points ? -value * 8 - rankInTrick : rankInTrick * 2 - value * 4);
    out.reason = HintReason::KeepStopper;
    return out;
}

Scored scoreLead(const View& v, Card card)
{
    Scored out;
    out.detail.card = card;
    if (v.suitGame) {
        // Tarocks take no suit trick (§6.3), so they are pure discards here:
        // start the strong suit from the top and keep them for later.
        if (card.tarock()) {
            out.score = 20 - card.tarockNo();
            out.reason = HintReason::DuckCheaply;
            return out;
        }
        const int suit = static_cast<int>(card.suit());
        out.score = 90 + v.facts.suitLen[suit] * 7 + power(card) * 3
                - (card.king() && v.facts.suitLen[suit] <= 2 ? 30 : 0);
        out.reason = card.king() ? HintReason::LeadShortKing : HintReason::SignalToPartner;
        return out;
    }

    if (card.tarock()) {
        // Which tarock the opening is made of depends on the contract (§10.6):
        // Solodreier high from the start, Dreier and Sechserdreier around XV
        // and never under X, Besserrufer small.
        int target = 15;
        if (v.contract == ContractId::Solodreier)
            target = 22;
        else if (v.contract == ContractId::Besserrufer)
            target = 5;
        else if (!v.iAmDeclarer)
            target = 12;
        int score = 40;
        if (v.iAmDeclarer && v.facts.tarocks >= 5)
            score = 170;                                  // draw the trumps
        score -= std::abs(card.tarockNo() - target) * 4;
        if (score > 100 && card.tarockNo() < 10 && v.contract != ContractId::Besserrufer)
            score -= 20;                                  // "nie unter X"
        out.score = score - cardCost(v, card) / 3;
        out.reason = HintReason::DrawTrumps;
        return out;
    }

    const int suit = static_cast<int>(card.suit());
    const int len = v.facts.suitLen[suit];
    int score = 70 + len * 7 + power(card) * 2;
    if (card.king()) {
        // The long suit is led king first; short kings are not led at all.
        score += len >= 4 ? 20 : -45;
        out.reason = HintReason::LeadShortKing;
    } else {
        out.reason = HintReason::SignalToPartner;
    }
    if (suit == v.calledSuit) {
        if (v.iAmPartner) {
            score -= 70;                                  // the king waits for the last trick
            out.reason = HintReason::KeepCalledSuit;
        } else if (!v.myDeclarerSide) {
            score -= 35;                                  // defenders avoid the called suit
            out.reason = HintReason::KeepCalledSuit;
        } else {
            score -= 10;
        }
    }
    if (v.iAmDeclarer && v.facts.tarocks >= 5)
        score -= 60;                                      // trumps first
    if (v.inf->isVoid(v.core->nextSeat(v.seat), suit))
        score -= 12;                                      // the next seat trumps it
    out.score = score;
    return out;
}

Scored scoreFollow(const View& v, Card card)
{
    Scored out;
    out.detail.card = card;
    const bool wins = beatsCard(*v.def, card, v.best);
    const bool last = v.myIndex == v.core->activePlayers() - 1;
    const bool sure = wins && (last || safeWinner(v, card));
    const int value = v.profile->cardValue(card);
    const int cost = cardCost(v, card);

    int worth = v.trickUnits * 2 + 10;
    if (v.opponentBirdDue)
        worth += 70;                                      // their bird must not come home
    if (v.mustAttack)
        worth += 25;
    else if (v.enough)
        worth -= 20;

    const int side = sideOf(v, v.bestSeat);
    const bool mateLeads = side > 0 || (side == 0 && probablyMine(v, v.bestSeat));
    if (mateLeads) {
        const int weight = side > 0 ? 11 : 5;             // a guess is smeared more carefully
        if (wins) {
            out.score = 25 - cost / 2;
            out.reason = HintReason::KeepStopper;
        } else {
            out.score = 90 + value * weight - cost / 3;
            out.reason = HintReason::SmearToPartner;
        }
        return out;
    }
    if (wins) {
        // "Second man as small as he can, third man as big as he can": a win
        // that two seats can still take away is worth much less (§10.6).
        const int behind = v.core->activePlayers() - 1 - v.myIndex;
        out.score = (sure ? 120 : 75 - behind * 14) + worth - cost;
        out.reason = v.mustAttack ? HintReason::CountSaysMustAttack
                : card.tarock() ? HintReason::DrawTrumps
                : HintReason::CountSaysMustAttack;
        return out;
    }
    out.score = 60 - value * 8 - cost / 4;
    out.reason = v.enough ? HintReason::CountSaysEnough
            : v.mustFollow ? HintReason::FollowSuitForced
            : HintReason::DuckCheaply;
    return out;
}

Scored scorePlay(const View& v, const Action& action)
{
    const Card card(static_cast<std::uint8_t>(action.a));
    // A card one of our announcements binds to a later trick stays in hand
    // until that trick (§6.5); among several the higher bird goes first.
    for (int i = 0; i < v.boundCount; ++i) {
        if (v.bound[static_cast<std::size_t>(i)].card != card)
            continue;
        Scored out;
        out.detail.card = card;
        const Bound& bound = v.bound[static_cast<std::size_t>(i)];
        if (v.trickNumber >= bound.trick) {
            out.score = 900;
            out.reason = HintReason::LastTrickPlan;
            return out;
        }
        out.score = (bound.announced ? -400 : -120) + card.tarockNo();
        out.reason = HintReason::SaveBirdForTarget;
        return out;
    }
    Scored out = v.negative ? scoreNegativePlay(v, card)
            : v.leading ? scoreLead(v, card)
            : scoreFollow(v, card);
    if (v.level == Difficulty::Tournament)
        pimcAdjust(v, action, &out.score);
    return out;
}

// The beginner plays without any inference and takes the first reasonable
// card: follow suit low, take the trick only when the card is already the one
// on top of the hand (§10.9).
Scored scoreBeginnerPlay(const View& v, const Action& action)
{
    const Card card(static_cast<std::uint8_t>(action.a));
    Scored out;
    out.detail.card = card;
    if (v.negative)
        return scoreNegativePlay(v, card);
    const int value = v.profile->cardValue(card);
    if (v.leading) {
        out.score = 50 - value * 4 + (card.tarock() ? 10 : 0);
        out.reason = card.tarock() ? HintReason::DrawTrumps : HintReason::SignalToPartner;
        return out;
    }
    const bool wins = beatsCard(*v.def, card, v.best);
    out.score = wins ? 70 + v.trickUnits - card.tarockNo() : 40 - value * 5;
    out.reason = wins ? HintReason::CountSaysMustAttack : HintReason::DuckCheaply;
    return out;
}

// --- putting the view together ----------------------------------------------------

void fillParties(View& v)
{
    const TarockCore& core = *v.core;
    v.calledSuit = core.calledSuit();
    v.iAmDeclarer = core.declarer() == v.seat;
    const bool caller = v.def && v.def->partner == PartnerMode::CallKing;
    // The only hidden fact a seat may use about itself: it holds the called
    // king, so it is the partner (§4.1.5).
    v.iAmPartner = caller && v.calledSuit >= 0 && !v.iAmDeclarer
            && (contains(core.hand(v.seat), core.calledKing())
                || (core.partnerKnown() && core.partner() == v.seat));
    v.myDeclarerSide = v.iAmDeclarer || v.iAmPartner;
}

void fillTrick(View& v)
{
    const TarockCore& core = *v.core;
    const CardList& trick = core.trick();
    v.trickNumber = core.trickNumber();
    v.lastTrick = core.profile().tricks();
    v.tricksLeft = v.lastTrick - v.trickNumber + 1;
    v.leading = trick.empty();
    v.myIndex = static_cast<int>(trick.size());
    if (trick.empty())
        return;
    v.lead = trick.front().suit();
    v.best = trick.front();
    int seat = core.trickLeader();
    v.bestSeat = seat;
    for (std::size_t i = 0; i < trick.size(); ++i) {
        const Card card = trick[i];
        v.trickUnits += core.profile().cardValue(card);
        if (beatsCard(*v.def, card, v.best)) {
            v.best = card;
            v.bestSeat = seat;
        }
        seat = core.nextSeat(seat);
    }
    // Whether we may choose at all: when every playable card follows the lead
    // the rules, not the plan, picked them.
    const CardSet playable = core.playableCards(v.seat);
    bool allFollow = true;
    forEachCard(playable, [&](Card card) { allFollow = allFollow && card.suit() == v.lead; });
    v.mustFollow = allFollow && setCount(playable) > 1;
}

void fillCounting(View& v)
{
    const TarockCore& core = *v.core;
    // Card points are counted from the tricks that lie face down on the
    // table: which seat took them is public, which party they belong to is
    // only public once the partnership is (§4.1.5).
    for (int seat = 0; seat < core.players(); ++seat) {
        if (!core.active(seat))
            continue;
        const int side = sideOf(v, seat);
        if (side == 0)
            continue;
        // Tricks are taken in the open, the discards of another seat are not:
        // only our own count here (§4.7).
        CardSet cards = core.won(seat);
        if (seat == v.seat)
            cards |= core.discards(seat);
        const int units = core.profile().count(cards).units;
        if (side > 0)
            v.myUnits += units;
        else
            v.theirUnits += units;
    }
    v.enough = v.myUnits >= core.profile().winThreshold();
    v.mustAttack = !v.enough
            && (v.theirUnits >= core.profile().totalUnits() - core.profile().winThreshold()
                || (v.tricksLeft <= 4 && v.myUnits * 2 < core.profile().winThreshold()));
}

void fillAnnouncements(View& v)
{
    const TarockCore& core = *v.core;
    for (const Declaration& item : core.declarations()) {
        if (item.bonus == BonusId::None)
            continue;
        const BonusDef& def = core.profile().bonus(item.bonus);
        const bool mine = item.seat == v.seat;
        if (mine && isBird(item.bonus))
            v.iAnnouncedBird = true;
        if (def.kind != BonusKind::TrickWithCard)
            continue;
        const Card card = item.bonus == BonusId::KingUltimo ? core.calledKing() : def.boundCard;
        if (!card.valid())
            continue;
        if (mine || (item.bonus == BonusId::KingUltimo && v.iAmPartner)) {
            if (contains(v.hand, card) && v.boundCount < static_cast<int>(v.bound.size())) {
                Bound bound;
                bound.card = card;
                bound.trick = def.targetTrick;
                bound.announced = true;
                v.bound[static_cast<std::size_t>(v.boundCount++)] = bound;
            }
            continue;
        }
        // An announcement of the other side is a trick we want to break.
        if (sideOf(v, item.seat) <= 0 && def.targetTrick == v.trickNumber)
            v.opponentBirdDue = true;
    }
    // A silent king ultimo is worth a point as well: the partner keeps the
    // called king for the last trick even without saying anything (§5.5).
    if (v.iAmPartner && v.calledSuit >= 0 && contains(v.hand, core.calledKing())
        && v.boundCount < static_cast<int>(v.bound.size())) {
        bool already = false;
        for (int i = 0; i < v.boundCount; ++i)
            already = already || v.bound[static_cast<std::size_t>(i)].card == core.calledKing();
        if (!already) {
            Bound bound;
            bound.card = core.calledKing();
            bound.trick = core.profile().tricks();
            bound.announced = false;
            v.bound[static_cast<std::size_t>(v.boundCount++)] = bound;
        }
    }
}

View makeView(const TarockCore& core, int seat, const Inference& inference, Difficulty level,
              int budgetMs)
{
    View v;
    v.core = &core;
    v.profile = &core.profile();
    v.inf = &inference;
    v.level = level;
    v.seat = seat;
    v.budgetMs = budgetMs;
    v.hand = core.hand(seat);
    if (core.phase() == Phase::Discard) {
        // What we would be left with: the tray is already spoken for.
        forEachCard(core.discardTray(seat), [&](Card card) { remove(v.hand, card); });
    }
    v.facts = evaluate(core.profile(), v.hand);
    v.contract = core.contract();
    if (v.contract != ContractId::None) {
        v.def = &core.contractDef();
        v.negative = v.def->play == PlayMode::Negative || v.def->play == PlayMode::Trischaken;
        v.suitGame = v.def->play == PlayMode::SuitGame;
        v.tarockGame = v.def->play == PlayMode::Positive;
        fillParties(v);
    }
    v.unknown = inference.unknownCards(core, seat);
    forEachCard(v.unknown, [&](Card card) {
        if (card.tarock())
            v.topUnknownTarock = std::max(v.topUnknownTarock, card.tarockNo());
        else
            v.topUnknownPower[static_cast<int>(card.suit())] =
                    std::max(v.topUnknownPower[static_cast<int>(card.suit())], power(card));
    });
    if (core.phase() == Phase::Play && v.def) {
        v.inPlay = true;
        fillTrick(v);
        fillCounting(v);
        for (int trick = 1; trick < core.trickNumber(); ++trick)
            v.declarerTricks += core.trickWinner(trick) == core.declarer() ? 1 : 0;
    }
    if (v.def)
        fillAnnouncements(v);
    return v;
}

// One legal action, judged. Every branch names the reason the learning mode
// will show.
Scored judge(const View& v, const Action& action)
{
    switch (v.core->phase()) {
    case Phase::Bidding:
        return scoreBid(v, action);
    case Phase::Call:
        return scoreCall(v, action);
    case Phase::Talon:
        return scoreTalon(v, action);
    case Phase::Discard:
        return scoreDiscard(v, action);
    case Phase::Announce:
        return scoreAnnounce(v, action);
    case Phase::Play:
        if (action.type != ActionType::PlayCard || !v.def)
            break;
        return v.level == Difficulty::Beginner ? scoreBeginnerPlay(v, action)
                                               : scorePlay(v, action);
    default:
        break;
    }
    return Scored();
}

} // namespace

// --- inference (§10.7, §10.8) -------------------------------------------------------

void Inference::markVoid(int seat, int suit)
{
    if (seat < 0 || seat >= TarockCore::MaxSeats || suit < 0 || suit > 4)
        return;
    m_void[static_cast<std::size_t>(seat)] |= static_cast<std::uint8_t>(1 << suit);
}

void Inference::noteCard(int seat, int leadSuit, Card card)
{
    if (leadSuit < 0 || !card.valid())
        return;
    const int suit = static_cast<int>(card.suit());
    if (suit == leadSuit)
        return;
    // It did not follow, so the suit is gone from that hand; a suit card on a
    // suit lead also says there is no tarock left (Tarockzwang, §6.2).
    markVoid(seat, leadSuit);
    if (!card.tarock())
        markVoid(seat, static_cast<int>(Suit::Tarock));
}

bool Inference::isVoid(int seat, int suit) const
{
    if (seat < 0 || seat >= TarockCore::MaxSeats || suit < 0 || suit > 4)
        return false;
    return (m_void[static_cast<std::size_t>(seat)] >> suit) & 1;
}

void Inference::observe(const TarockCore& core, int seat)
{
    // Public facts only: the cards that have fallen, the running trick in
    // playing order and the talon half left lying. A foreign hand is never
    // read here — the rest of the AI has no other source either (§10.8).
    if (!core.active(seat))
        return;
    if (core.trickNumber() == 0) {
        m_seen.reset();
        m_void.fill(0);
        m_tricksSeen = 0;
        m_trickNumber = 0;
        m_inTrick = 0;
        m_leadSeat = -1;
        m_leadSuit = -1;
        return;
    }

    CardSet fallen;
    for (int other = 0; other < core.players(); ++other)
        fallen |= core.won(other);
    for (Card card : core.trick())
        add(fallen, card);

    const CardList& trick = core.trick();
    const bool trickEnded = core.trickNumber() != m_trickNumber
            || static_cast<int>(trick.size()) < m_inTrick;
    if (trickEnded) {
        // The cards of the trick we were following that we did not see fall.
        // Only when exactly one is missing does the seat that played it
        // follow from the playing order; otherwise nothing is concluded.
        CardSet tail = fallen & ~m_seen;
        for (Card card : trick)
            remove(tail, card);
        if (setCount(tail) == 1 && m_leadSeat >= 0 && m_leadSuit >= 0
            && m_inTrick == core.activePlayers() - 1) {
            int player = m_leadSeat;
            for (int i = 0; i < m_inTrick; ++i)
                player = core.nextSeat(player);
            const int owner = player;
            forEachCard(tail, [&](Card card) { noteCard(owner, m_leadSuit, card); });
        }
        m_inTrick = 0;
        m_leadSeat = -1;
        m_leadSuit = -1;
    }

    if (!trick.empty()) {
        const int leadSuit = static_cast<int>(trick.front().suit());
        int player = core.trickLeader();
        for (std::size_t i = 0; i < trick.size(); ++i) {
            if (static_cast<int>(i) >= m_inTrick)
                noteCard(player, leadSuit, trick[i]);
            player = core.nextSeat(player);
        }
        m_leadSeat = static_cast<std::int8_t>(core.trickLeader());
        m_leadSuit = static_cast<std::int8_t>(leadSuit);
    }
    m_inTrick = static_cast<int>(trick.size());
    m_trickNumber = core.trickNumber();
    m_seen = fallen;
    m_tricksSeen = core.handOver() ? core.profile().tricks() : core.trickNumber() - 1;
}

CardSet Inference::unknownCards(const TarockCore& core, int seat) const
{
    CardSet unknown = core.profile().deck().cards();
    unknown &= ~m_seen;
    if (core.active(seat)) {
        unknown &= ~core.hand(seat);          // my own cards
        unknown &= ~core.discards(seat);      // and what I put away myself
    }
    if (core.talonOpen()) {
        // The half the declarer left lying was shown to every active seat.
        unknown &= ~core.talonHalf(0);
        unknown &= ~core.talonHalf(1);
    }
    return unknown;
}

// --- the player -----------------------------------------------------------------------

AiPlayer::AiPlayer(Difficulty level) : m_level(level) {}

Action AiPlayer::bestAction(const TarockCore& core, int seat, int budgetMs) const
{
    Inference inference;
    inference.observe(core, seat);
    return bestAction(core, seat, inference, budgetMs);
}

Action AiPlayer::bestAction(const TarockCore& core, int seat, const Inference& inference,
                            int budgetMs) const
{
    const std::vector<Hint> hints = rank(core, seat, inference, budgetMs);
    return hints.empty() ? Action() : hints.front().action;
}

std::vector<Hint> AiPlayer::rank(const TarockCore& core, int seat, int budgetMs) const
{
    Inference inference;
    inference.observe(core, seat);
    return rank(core, seat, inference, budgetMs);
}

std::vector<Hint> AiPlayer::rank(const TarockCore& core, int seat, const Inference& inference,
                                 int budgetMs) const
{
    std::vector<Hint> hints;
    const std::vector<Action> legal = core.legalActions(seat);
    if (legal.empty())
        return hints;
    // The beginner looks at the table without any inference (§10.9).
    Inference blind;
    const Inference& used = m_level == Difficulty::Beginner ? blind : inference;
    const View view = makeView(core, seat, used, m_level, budgetMs);
    hints.reserve(legal.size());
    for (const Action& action : legal) {
        const Scored scored = judge(view, action);
        Hint hint;
        hint.action = action;
        hint.score = scored.score;
        hint.reason = scored.reason;
        hint.detail = scored.detail;
        hints.push_back(hint);
    }
    if (hints.size() == 1 && hints.front().action.type == ActionType::PlayCard)
        hints.front().reason = HintReason::OnlyOneCard;
    // A stable sort keeps the order the rule core offered the actions in, so
    // the same position always produces the same recommendation.
    std::stable_sort(hints.begin(), hints.end(),
                     [](const Hint& a, const Hint& b) { return a.score > b.score; });
    for (std::size_t i = 0; i < hints.size(); ++i)
        hints[i].rank = static_cast<int>(i);
    return hints;
}

HintReason AiPlayer::explain(const TarockCore& core, int seat, const Action& action) const
{
    Inference inference;
    inference.observe(core, seat);
    return explain(core, seat, inference, action);
}

HintReason AiPlayer::explain(const TarockCore& core, int seat, const Inference& inference,
                             const Action& action) const
{
    Inference blind;
    const Inference& used = m_level == Difficulty::Beginner ? blind : inference;
    const View view = makeView(core, seat, used, m_level, 0);
    if (core.phase() == Phase::Play && action.type == ActionType::PlayCard
        && setCount(core.playableCards(seat)) == 1)
        return HintReason::OnlyOneCard;
    return judge(view, action).reason;
}

} // namespace tarock
