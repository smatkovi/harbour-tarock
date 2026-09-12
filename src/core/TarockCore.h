// The rule state machine: it holds one hand of Tarock and applies the actions
// of the players. It is deterministic and free of Qt, so the engine, the
// computer players, the LAN guests and the tests all run the same code.
//
// Every refusal comes back as a Reason, never as a silent "false": the
// learning mode turns those into sentences, and the tests assert on them.
#pragma once

#include "Action.h"
#include "Card.h"
#include "Reason.h"
#include "RuleProfile.h"
#include "Scoring.h"

#include <array>
#include <random>
#include <string>
#include <vector>

namespace tarock {

enum class Phase : std::uint8_t {
    Deal = 0, Bidding = 1, Call = 2, Talon = 3, Discard = 4,
    Announce = 5, Play = 6, Scoring = 7, HandOver = 8
};

// What a player announced during the announcement phase. Scoring.h turns
// these into "Posten" together with the game itself.
struct Declaration {
    BonusId bonus = BonusId::None;   // None = the game itself
    int seat = -1;                   // who announced it (-1 = silent/game)
    bool declarerSide = true;        // which party it belongs to
    bool announced = false;          // announced, or found silently at the end
    int level = 1;                   // 1, 2 (Kontra), 4 (Re), 8 (Sub)
    int suit = -1;                   // named suit for king ultimo
};

class TarockCore {
public:
    static constexpr int MaxSeats = 5;

    TarockCore() = default;
    TarockCore(ProfileId profile, int players, std::uint32_t seed, const FlagSet& flags = {});

    void newMatch(ProfileId profile, int players, std::uint32_t seed, const FlagSet& flags = {});
    bool nextHand();
    // Fixed deals for the practice hands of the learning mode and for tests.
    void dealFixed(const std::vector<CardList>& hands, const CardList& talon, int dealer);

    const RuleProfile& profile() const { return *m_profile; }
    const FlagSet& flags() const { return m_flags; }
    int players() const { return m_players; }
    int activePlayers() const { return m_players == 5 ? 4 : m_players; }
    int sittingOut() const { return m_players == 5 ? m_dealer : -1; }
    bool active(int seat) const { return seat >= 0 && seat < m_players && seat != sittingOut(); }
    int dealer() const { return m_dealer; }
    int forehand() const;
    int nextSeat(int seat) const;   // skips the seat that sits out
    int nextBidder(int seat) const; // skips seats that have passed

    Phase phase() const { return m_phase; }
    int actor() const { return m_turn; }
    int handNumber() const { return m_hand; }

    const CardSet& hand(int seat) const { return m_hands[static_cast<std::size_t>(seat)]; }
    const CardList& trick() const { return m_trick; }
    int trickNumber() const { return m_trickNumber; }        // 1-based, 0 before the first
    int trickLeader() const { return m_leader; }
    const CardSet& won(int seat) const { return m_won[static_cast<std::size_t>(seat)]; }
    int trickWinner(int trick) const;                        // -1 if not played yet

    const CardSet& talonHalf(int half) const { return m_talonHalf[static_cast<std::size_t>(half)]; }
    const CardSet& talonToDefenders() const { return m_talonToDefenders; }
    bool talonOpen() const { return m_talonOpen; }
    const CardSet& discardTray(int seat) const { return m_tray[static_cast<std::size_t>(seat)]; }
    const CardSet& discards(int seat) const { return m_discards[static_cast<std::size_t>(seat)]; }

    ContractId contract() const { return m_contract; }
    const ContractDef& contractDef() const { return profile().contract(m_contract); }
    int declarer() const { return m_declarer; }
    int partner() const { return m_partner; }                // known to the core only
    bool partnerKnown() const { return m_partnerKnown; }     // true once it is public
    int calledSuit() const { return m_calledSuit; }
    Card calledKing() const;
    bool declarerSide(int seat) const;                       // core view, not a player's view
    int bidHolder() const { return m_bidHolder; }
    bool hasPassed(int seat) const { return (m_passed >> seat) & 1; }
    const std::vector<Declaration>& declarations() const { return m_declarations; }
    bool conceded() const { return m_conceded; }

    // --- decisions -------------------------------------------------------
    std::vector<Action> legalActions(int seat) const;
    Reason check(int seat, const Action& action) const;
    bool apply(int seat, const Action& action, Reason* reason = nullptr);
    // Cards this seat may play right now, with the reason why the others may not.
    CardSet playableCards(int seat) const;
    Reason cardReason(int seat, Card card) const;

    bool handOver() const { return m_phase == Phase::Scoring || m_phase == Phase::HandOver; }
    // Available once the hand is over: what everybody pays and why.
    const Ledger& ledger() const { return m_ledger; }
    HandResult handResult() const;
    // Card points of a party in profile units, talon and discards included.
    CountResult partyPoints(bool declarerSide) const;
    CardSet partyCards(bool declarerSide) const;

    // --- LAN and persistence ---------------------------------------------
    void rotateSeats(int offset);
    std::string serialize() const;
    bool restore(const std::string& data);
    bool validate(std::string* error = nullptr) const;

private:
    friend struct CoreAccess;

    void startHand();
    void beginBidding();
    void finishBidding();
    void beginCall();
    void beginTalon();
    void beginAnnounce();
    // How the announcement round continues after an action.
    enum class AnnounceStep { Confirm, Announced, Doubled };
    void advanceAnnounce(int seat, AnnounceStep step);
    void beginPlay();
    void finishTrick();
    void finishHand();
    void setPhase(Phase phase);
    bool takeTalonHalf(int half);
    void dealTalonToWinner();

    const RuleProfile* m_profile = &RuleProfile::get(ProfileId::AtKrOoe2023);
    FlagSet m_flags;
    int m_players = 4;
    int m_dealer = 0;
    int m_hand = 0;
    Phase m_phase = Phase::Deal;
    int m_turn = -1;

    std::array<CardSet, MaxSeats> m_hands{};
    std::array<CardSet, MaxSeats> m_won{};
    std::array<CardSet, MaxSeats> m_tray{};        // cards picked for discarding
    std::array<CardSet, MaxSeats> m_discards{};    // confirmed discards, count for their tricks
    std::array<CardSet, 2> m_talonHalf{};
    // What the declarer left lying belongs to the defenders as a party, not
    // to one seat, so that a rotated copy of the hand stays identical.
    CardSet m_talonToDefenders{};
    bool m_talonOpen = false;
    bool m_talonTaken = false;

    CardList m_trick;
    int m_leader = 0;
    int m_trickNumber = 0;
    std::array<std::int8_t, 12> m_trickWinners{};
    std::array<CardList, 12> m_trickCards{};
    // Negative games are doubled against each opponent separately (§7.5).
    std::array<int, MaxSeats> m_seatKontra{{1, 1, 1, 1, 1}};
    Ledger m_ledger;

    // bidding
    int m_passed = 0;             // bit per seat
    ContractId m_bid = ContractId::None;
    int m_bidHolder = -1;
    bool m_forehandSpoke = false;

    // contract
    ContractId m_contract = ContractId::None;
    int m_declarer = -1;
    int m_partner = -1;
    bool m_partnerKnown = false;
    int m_calledSuit = -1;
    bool m_calledFourthKing = false;
    bool m_conceded = false;
    std::vector<Declaration> m_declarations;
    int m_announceReady = 0;      // bit per seat: "Ich liege"

    std::mt19937 m_rng{1};
};

} // namespace tarock
