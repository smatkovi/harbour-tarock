// Card model, deck and counting: the numbers of docs/koenigrufen.md §1 and
// docs/hungarian.md §1 must come out exactly.
#include "core/Card.h"
#include "core/Deck.h"
#include "core/RuleProfile.h"

#include <cstdio>
#include <cstdlib>
#include <random>
#include <set>

using namespace tarock;

namespace {
int failures = 0;
void check(bool ok, const char* what, int line)
{
    if (!ok) {
        std::fprintf(stderr, "FAILED line %d: %s\n", line, what);
        ++failures;
    }
}
#define CHECK(x) check((x), #x, __LINE__)
} // namespace

int main()
{
    // --- card numbering ----------------------------------------------------
    CHECK(kSkues.tarock() && kSkues.tarockNo() == 22);
    CHECK(kMond.tarockNo() == 21 && kPagat.tarockNo() == 1);
    CHECK(kPagat.honour() && kMond.honour() && kSkues.honour());
    CHECK(kPagat.bird() && tarockCard(4).bird() && !tarockCard(5).bird());
    CHECK(suitCard(Suit::Heart, 0).king() && suitCard(Suit::Heart, 0).red());
    CHECK(!suitCard(Suit::Spade, 0).red());
    // Red: ace beats the two; black: ten is the highest low card.
    CHECK(power(suitCard(Suit::Heart, 4)) > power(suitCard(Suit::Heart, 5)));
    CHECK(power(suitCard(Suit::Heart, 0)) == 8 && power(suitCard(Suit::Club, 7)) == 1);
    CHECK(power(kSkues) == 22 && power(kPagat) == 1);

    // --- card keys ---------------------------------------------------------
    CHECK(cardKey(kSkues) == "Sk" && cardKey(kMond) == "XXI" && cardKey(kPagat) == "I");
    CHECK(cardKey(suitCard(Suit::Heart, 0)) == "H:K");
    CHECK(cardKey(suitCard(Suit::Heart, 4)) == "H:1");
    CHECK(cardKey(suitCard(Suit::Spade, 4)) == "S:10");
    CHECK(cardKey(suitCard(Suit::Club, 7)) == "C:7");
    for (int id = 0; id < 54; ++id) {
        const Card card(static_cast<std::uint8_t>(id));
        Card parsed;
        CHECK(parseCardKey(cardKey(card), parsed) && parsed == card);
    }

    // --- decks -------------------------------------------------------------
    const RuleProfile& kr = RuleProfile::get(ProfileId::AtKrOoe2023);
    CHECK(kr.deck().size() == 54 && toList(kr.deck().cards()).size() == 54);
    CHECK(kr.deck().contains(suitCard(Suit::Heart, 7)));

    // --- counting: Königrufen counts in thirds -----------------------------
    CountResult all = kr.count(kr.deck().cards());
    CHECK(all.cards == 54);
    CHECK(all.units == 210);                 // 3 * 106 - 2 * 54
    CHECK(kr.winThreshold() == 107);
    // The four honours plus a king are 5 each: 5 cards, 25 rounded points.
    CardSet fives;
    add(fives, kSkues); add(fives, kMond); add(fives, kPagat);
    add(fives, suitCard(Suit::Heart, 0)); add(fives, suitCard(Suit::Spade, 0));
    CountResult high = kr.count(fives);
    CHECK(high.cards == 5 && high.units == 3 * 25 - 2 * 5);   // 65 thirds = 21 2/3 points
    CHECK(high.points() == 21);
    // A trick of three blank cards is worth one point, the classic example.
    CardSet blanks;
    add(blanks, tarockCard(5)); add(blanks, tarockCard(6)); add(blanks, suitCard(Suit::Club, 7));
    CHECK(kr.count(blanks).units == 3);

    // --- dealing -----------------------------------------------------------
    std::mt19937 rng(7);
    for (int round = 0; round < 200; ++round) {
        Deal d = deal(kr.deck(), kr.dealPlan(), 4, rng);
        CHECK(d.talon.size() == 6);
        std::set<int> seen;
        for (const CardList& hand : d.hands) {
            CHECK(hand.size() == 12);
            for (Card card : hand)
                CHECK(seen.insert(card.id).second);
        }
        for (Card card : d.talon)
            CHECK(seen.insert(card.id).second);
        CHECK(seen.size() == 54);
    }

    // --- contract table ----------------------------------------------------
    CHECK(kr.contracts().size() == 13);
    CHECK(kr.contract(ContractId::Rufer).baseValue == 1);
    CHECK(kr.contract(ContractId::Solodreier).baseValue == 8);
    CHECK(kr.contract(ContractId::Sechserdreier).lostValue == 8);
    CHECK(kr.contract(ContractId::Bettel).play == PlayMode::Negative);
    CHECK(kr.contract(ContractId::Farbensolo).play == PlayMode::SuitGame);
    CHECK(kr.contract(ContractId::Rufer).partner == PartnerMode::CallKing);
    CHECK((kr.contract(ContractId::Rufer).bonusMask & bonusBit(BonusId::KingUltimo)) != 0);
    CHECK((kr.contract(ContractId::Dreier).bonusMask & bonusBit(BonusId::KingUltimo)) == 0);
    CHECK((kr.contract(ContractId::Bettel).bonusMask) == 0);
    CHECK(kr.bonus(BonusId::Uhu).silent == 2 && kr.bonus(BonusId::Uhu).announced == 4);
    CHECK(kr.bonus(BonusId::Quapil).targetTrick == 9);
    CHECK(kr.bonus(BonusId::Pagat).boundCard == kPagat);
    CHECK(kr.bonus(BonusId::Valat).multiplier);

    // --- Hungarian profile --------------------------------------------------
    const RuleProfile& hu = RuleProfile::get(ProfileId::HuIlluItvb2019);
    CHECK(hu.deck().size() == 42);
    CHECK(!hu.deck().contains(suitCard(Suit::Heart, 5)));   // red two is out
    CHECK(hu.deck().contains(suitCard(Suit::Heart, 4)));    // red ace is the lowest
    CHECK(hu.deck().contains(suitCard(Suit::Spade, 4)));    // black ten is the lowest
    CountResult huAll = hu.count(hu.deck().cards());
    CHECK(huAll.cards == 42 && huAll.units == 94);          // plain points, no thirds
    CHECK(hu.winThreshold() == 48 && hu.tricks() == 9);
    CHECK(hu.contracts().size() == 4);
    CHECK(hu.contract(ContractId::Harom).talonToDeclarer == 3);
    CHECK(hu.contract(ContractId::Szolo).baseValue == 4 && hu.contract(ContractId::Szolo).talonToDeclarer == 0);
    CHECK(hu.bonus(BonusId::Pagat).announced == 10 && hu.bonus(BonusId::Pagat).silent == 5);
    CHECK(hu.bonus(BonusId::XXIFogas).announced == 42 && hu.bonus(BonusId::XXIFogas).kind == BonusKind::Capture);
    CHECK(hu.bonus(BonusId::Duplajatek).count == 71 && hu.bonus(BonusId::Duplajatek).multiplier);
    CHECK(hu.bonus(BonusId::Nagymadar).targetTrick == 7 && hu.bonus(BonusId::Nagymadar).boundCard == kSkues);
    std::mt19937 huRng(11);
    for (int round = 0; round < 100; ++round) {
        Deal d = deal(hu.deck(), hu.dealPlan(), 4, huRng);
        CHECK(d.talon.size() == 6);
        std::set<int> seen;
        for (const CardList& hand : d.hands) {
            CHECK(hand.size() == 9);
            for (Card card : hand)
                CHECK(seen.insert(card.id).second);
        }
        for (Card card : d.talon)
            CHECK(seen.insert(card.id).second);
        CHECK(seen.size() == 42);
    }

    if (failures) {
        std::printf("%d failures\n", failures);
        return 1;
    }
    std::printf("OK: cards, decks, counting and both profile tables\n");
    return 0;
}
