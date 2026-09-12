# Regression fixtures

`kr_hand1.json`, `kr_hand2.json` and `kr_hand3.json` are the three fully written
practice hands of `docs/koenigrufen.md` §11.7 in machine-readable form: a fixed
deal, the complete action sequence and the expected result. They are the
regression net for the Königrufen rules (`docs/design.md` §11.3, milestone M2);
the learning mode gets its own, wordier files under `lessons/` later.

| File | §11.7 | Contract | What it exercises |
|---|---|---|---|
| `kr_hand1.json` | Übungsspiel 1 | Rufer | forehand bidding, king call, talon and discard, Farb-/Tarockzwang, silent Trull and four kings |
| `kr_hand2.json` | Übungsspiel 2 | Piccolo | bidding from the back, negative play, Stichzwang, Pagat rule, individual Kontra |
| `kr_hand3.json` | Übungsspiel 3 | Besserrufer | mandatory bird announcement, Kontra on a bonus, reserved Pagat, Pagat ultimo |

## Conventions

- **Cards** use the stable keys of `src/core/Card.h` (`cardKey` / `parseCardKey`):
  `"I" … "XXI"`, `"Sk"`, and `"<suit>:<rank>"` with suit `H D S C` and rank
  `K D R B` plus `1 2 3 4` (red) or `10 9 8 7` (black) — `"H:K"`, `"S:10"`, `"D:1"`.
- **Seats** are 0–3 in playing order, i.e. counter-clockwise at the table
  (`docs/koenigrufen.md` §0.3). `seats` gives the names used in the spec, which
  are the same four in every hand; `dealer` and `forehand` change per hand, and
  `forehand == (dealer + 1) % 4`.
- All amounts are whole points; card points are kept in **thirds**
  (`units = 3·Σ value − 2·cards`, §1.5), because only thirds decide the game.

## Top level

| Field | Meaning |
|---|---|
| `id`, `title`, `source` | file name, the spec's title, and where it comes from |
| `profile` | profile key, always `"AT-KR-OOE-2023-04"` (`RuleProfile::key()`) |
| `players`, `seats`, `dealer`, `forehand` | table (four players, nobody sitting out) |
| `hands` | the 12 dealt cards of seats 0…3, before the talon |
| `talon` | the two halves of three, in deal order (§2.4) |
| `actions` | every decision in order, see below |
| `expected` | what the engine must produce afterwards |

`hands` plus `talon` are exactly the 54 cards of the deck, each one once.

## `actions`

Each entry is `{"seat": s, "type": T, …}`; the type maps one-to-one onto
`ActionType` in `src/core/Action.h`.

| `type` | extra field | `ActionType` |
|---|---|---|
| `OPEN_FOREHAND` | — | `OpenForehand` ("Vorhand", no game yet) |
| `PASS` | — | `Pass` ("weiter" / "gut" in the bidding) |
| `BID` | `a`: contract key (`ContractDef::key`) | `Bid` |
| `CALL_KING` | `suit`: `"H" "D" "S" "C"` | `CallKing` |
| `TAKE_TALON` | `half`: 0 or 1 | `TakeTalon` |
| `DISCARD` | `card` | `Discard` (one toggle per card) |
| `CONFIRM_DISCARD` | — | `ConfirmDiscard` |
| `ANNOUNCE` | `bonus`: bonus key (`BonusDef::key`) | `AnnounceBonus` |
| `READY` | — | `Ready` ("Ich liege" for the declarer, "gut" for the others) |
| `KONTRA` | `posten`: `"GAME"` or a bonus key, `level`: 2 \| 4 \| 8 | `Kontra` |
| `PLAY` | `card` | `PlayCard` |

`PLAY` actions come in playing order, four per trick, twelve tricks; the first
card of a trick is the lead, and the next trick starts with its winner.

## `expected`

- `contract`, `declarer`, `partner` (−1 when the declarer plays alone),
  `declarerParty`, `defenderParty`, and `payment`: `"2v2"` or `"1v3"` (§7.1.3).
- `tricks`: one entry per trick with `n`, `leader`, `winner` (§6.6) and `units`,
  the card points of the four cards in thirds.
- `count`: `declarer*` and `defender*` are cards, thirds, and the same value as
  players say it — `points` plus `blatt`, so 125 thirds read as 41/2. Ownership
  of discard and of the talon half left lying follows §1.8, so
  `declarerCards + defenderCards + neutralCards == 54` and the thirds add up to
  `totalUnits == 210`. `neutral*` is the talon of a negative game, which counts
  for nobody. `decidedByCardPoints` is false exactly for the negative games;
  where it is true, `gameWon == (declarerUnits >= threshold)` with the threshold
  107 = 35/2 of §1.6; where it is false, `threshold` is `null` and the Piccolo
  needs `declarerTricks == 1` instead (§1.6, §7.5).
- `posten`: the game and every bonus, each settled separately (§7.1.2). `value`
  is the tariff per player (§7.2, §7.3 — a silent bonus is half an announced
  one), `winner` is the party that receives it, and `announced` / `achieved`
  describe a bonus. `kontra` is the money factor per seat (§7.8):
  - `kontraMode: "party"` — a positive game, where one Kontra binds all four
    players, so the four factors are equal;
  - `kontraMode: "individual"` — a negative game, where every opponent doubles
    only their own account with the declarer (§7.5). The declarer's own entry
    stays 1; their money for the posten is the negated sum of the other three.
- `schrift` and `geld` are the per-seat totals, both zero-sum. They differ only
  where a posten was doubled, because a Kontra never touches the Schrift (§7.8).

## What a test must check

The three files were checked against these rules before being committed:

1. `hands` + `talon` are the 54 cards, no card twice, 12 per seat.
2. Every played card was in that seat's hand (after the talon half was taken and
   the discard removed), and every play obeys §6.7 — follow suit, then
   Tarockzwang; in the Piccolo additionally Stichzwang and "the Pagat only as the
   last tarock".
3. Every `tricks[].winner` follows §6.6, every `leader` is the previous winner,
   and every `units` is the count of §1.5.
4. The card points add up to 210 thirds, and the ownership of discard and talon
   follows §1.8.
5. Trull, four kings and Pagat ultimo are exactly those in `posten`, judged from
   the cards the parties actually end up with (§5.1, §5.2).
6. `schrift` and `geld` recomputed from `posten` per §7.1/§7.8 give the arrays in
   the file, and both sum to zero.

## Known gap

§11.7 prints the talon of Übungsspiel 2 as one row of six cards, because a
Piccolo never touches it. `kr_hand2.json` splits it into the two halves of the
deal (§2.4) in the printed order; which three lie in which half is arbitrary and
cannot affect any expected value, since that talon counts for nobody.
