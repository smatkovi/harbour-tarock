# Lektionen des Lernmodus

Hier liegt der Lernstoff der App: pro Regelprofil ein Ordner mit Lektionsdateien,
dazu `schema.json` als verbindliches Datenformat.

```
assets/lessons/
├── schema.json                  JSON Schema 2020-12 für eine Lektionsdatei
├── README.md                    dieses Dokument
└── at-kr-ooe/           Königrufen, Oberösterreich 4/2023
    ├── index.json               Kursreihenfolge mit Titeln und Kurzbeschreibungen
    ├── kr-l0.json … kr-l7.json  die acht Kursmodule
    └── kr-practice-1..3.json    die drei Übungspartien aus koenigrufen.md §11.7
```

Maßgeblich ist `docs/design.md` §7.6; die Inhalte stammen aus `docs/koenigrufen.md`
§11. `schema.json` ist die maschinenlesbare Fassung von §7.6 — was dort nicht
beschrieben ist, gehört nicht in eine Lektionsdatei. Nur die Lektionen werden
gegen das Schema geprüft, `index.json` nicht.

## Aufbau einer Lektionsdatei

| Feld | Bedeutung |
|---|---|
| `id` | eindeutig, gleich dem Dateinamen ohne `.json` |
| `profile` | `RuleProfile::key()`, hier immer `AT-KR-OOE-2023-04` |
| `module` | Kursmodul `L0`–`L7` |
| `title`, `goals`, `source` | Titel, Lernziele, Belegstelle in der Spezifikation |
| `players`, `seatNames`, `dealer` | der Tisch; **Sitz 0 ist immer der Lernende** |
| `deal` | festes Blatt: `hands` (4 × 12 Karten) und `talon` (2 × 3) |
| `steps` | die Schrittfolge, siehe unten |
| `result` | Sollzustand nach der Hand — Grundlage des Regressionstests |
| `moral` | ein Satz Lehre am Ende der Übungspartie |

Kartenschlüssel sind exakt die von `src/core/Card.h` (`cardKey` / `parseCardKey`):
`I` … `XXI`, `Sk`, und `<Farbe>:<Rang>` mit Farbe `H D S C` und Rang `K D R B`
plus `1 2 3 4` (rot) bzw. `10 9 8 7` (schwarz) — also `H:K`, `S:10`, `D:1`.
Spielnamen sind `ContractDef::key`, Prämien `BonusDef::key`, Grundcodes die
Spec-Schlüssel aus `koenigrufen.md` §11.4 (`E_TAROCKZWANG`, `W_VOGEL_RESERVIERT`, …).

### Schritte

Ein Schritt ist entweder ein **Lesenschritt** (nur `text`), ein **Automatikschritt**
(`auto`), ein **Entscheidungsschritt** (`expect`) oder ein **Stich** (`moves`).

| Feld | Bedeutung |
|---|---|
| `id` | eindeutig in der Lektion (`s1`, `t7`, …) |
| `phase` | `bidding`, `call`, `talon`, `discard`, `announce`, `play`, … |
| `seat` | wer entscheidet; bei `expect` immer 0 |
| `expect` | genau diese Aktion führt weiter; jede andere wird sanft abgelehnt |
| `auto` | Züge der Mitspieler, die ohne Zutun ablaufen |
| `text` | die Erklärung — sie sagt **warum**, nicht nur was |
| `highlight` | Karten, die das Panel hervorhebt (`hand`, `talon`, `trick`) |
| `compare` | die beiden Talonhälften mit je einer Notiz |
| `traps` | erwartete Anfängerfehler, siehe unten |
| `anchor` | Regelanker für „Mehr dazu“, z. B. `#6.7` |
| `moves`, `lead`, `expectWinner`, `trickUnits` | ein vollständiger Stich mit Prüfwerten |

Aktionen werden in Kleinschreibung notiert und bilden `ActionType` aus
`src/core/Action.h` ab: `vorhand`, `pass`, `hold`, `bid` (`a` = Spiel),
`callking` (`a` = Farbe), `taketalon` (`a` = 0 | 1 | −1), `discard` (`card`),
`discardSet` (`cards`, die ganze Ablage auf einmal), `announce` (`a` = Prämie),
`kontra` (`a` = Posten, `b` = 2 | 4 | 8), `ready`, `play` (`card`).

`auto` kennt zwei Formen: die Kurzform `["pass","pass","pass"]` (die Mitspieler
der Reihe nach) und die Langform mit Sitz und Argumenten, die auch für den Sitz
des Lernenden erlaubt ist:

```json
"auto": [{"seat": 2, "type": "kontra", "a": "PAGAT", "b": 2}]
```

Die Langform dient vor allem als Vorspann: Module, die mitten in einer Hand
einsteigen (L4, L5, L6), spulen die vorangegangenen Entscheidungen in einem
einzigen Automatikschritt ab.

### Der Fallenmechanismus

Eine Falle beschreibt einen Fehler, den Anfänger an genau dieser Stelle
tatsächlich machen — die Liste dafür ist `koenigrufen.md` §11.5.

```json
"traps": [
  { "card": "C:K", "reason": "E_ABLAGE_KOENIG",
    "text": "Fünf Punkte sicher in die Ablage – das wäre zu einfach. Könige darf man nie verlegen …" },
  { "action": {"type": "taketalon", "a": 1},
    "text": "Sechs sichere Punkte klingen verlockend. Aber der XVIII ist ein Stich, den du sonst nicht hast …" },
  { "text": "„35 Punkte, also die Hälfte – gewonnen!“ Es braucht 35 und 2 Blatt …" }
]
```

Drei Arten, nach dem, was der Lernende tut:

1. **Falle mit `reason`** — der Kern lehnt die Aktion ohnehin ab und liefert
   diesen Grundcode; die Lektion ergänzt nur den Kontext. Solche Fallen sind
   zugleich Regressionstest: `test_lessons.cpp` prüft, dass `TarockCore::check()`
   die Aktion mit **genau** diesem Code ablehnt (`docs/design.md` §11.3, Punkt 6).
   Der Code muss deshalb in `koenigrufen.md` §11.4 stehen und zur Situation
   passen. Beispiel: `announce TRULL` ohne Trullstück ist **keine** solche Falle,
   weil Trull und Alle Könige „auf Verdacht“ angesagt werden dürfen (§5.3).
2. **Falle mit `action` oder `card`, aber ohne `reason`** — die Aktion wäre
   erlaubt, ist hier aber falsch. Die App führt sie nicht aus, sondern zeigt den
   Text; der Lernende bleibt im Schritt.
3. **Falle ohne `action` und ohne `card`** — ein reiner Denkfehler, den man nicht
   antippen kann („35 Punkte reichen“, „der Talon gehört niemandem“). Das Panel
   zeigt ihn als „Häufiger Irrtum“ zum Schritt.

Der Text ist Pflichtprogramm, auch wenn ein `reason` dabeisteht: Er soll
erklären, was der Lernende vermutlich gedacht hat und was stattdessen gilt —
nie tadeln. Der Kurztext des Grundcodes kommt aus `src/ReasonText.cpp`, die
Falle liefert die Begründung für **diese** Stelle.

## Eine Lektion hinzufügen

1. Datei in `assets/lessons/<profil>/<id>.json` anlegen, `id` = Dateiname.
2. Blatt eintragen, falls die Lektion eines braucht: 4 × 12 Handkarten plus
   zwei Talonhälften zu drei, zusammen genau die 54 Karten ohne Dublette.
   Für eine Übungspartie das Blatt der passenden Fixture aus
   `tests/fixtures/kr_hand*.json` übernehmen, damit Lektion und Regressionstest
   dieselbe Hand beschreiben.
3. Schritte schreiben. Jeder Text nennt den Grund, nicht nur die Handlung —
   „Der Rufer ist das passende Spiel: billig, aber mit Partner“ statt „Sage
   Rufer an“. Regelbegriffe bleiben im Original (Vorhand, Sküs, Talon,
   Stichzwang); sie werden vom `LearnPanel` automatisch mit dem Glossar
   verlinkt.
4. Fallen ergänzen: für jeden Anfängerfehler aus §11.5, der zum Thema des
   Schritts gehört.
5. In `index.json` eintragen (Reihenfolge, Titel, ein Satz Beschreibung).
6. Prüfen:

```sh
python3 - <<'EOF'
import json, glob, jsonschema
schema = json.load(open('assets/lessons/schema.json'))
for path in glob.glob('assets/lessons/*/*.json'):
    if path.endswith('index.json'):
        continue
    jsonschema.Draft202012Validator(schema).validate(json.load(open(path)))
    print('ok', path)
EOF
```

Zusätzlich prüft `test_lessons.cpp` jede Datei gegen die Engine: Blatt, Legalität
jedes Zuges, Stichgewinner, Kartenpunkte, Abrechnung und die Grundcodes der
Fallen (`docs/design.md` §11.3).

## Übersetzungen

Freie Texte gehen nicht durch Qt Linguist, weil `lupdate` JSON nicht sieht.
Stattdessen überschreibt eine Overlay-Datei einzelne Felder nach `id`, zum
Beispiel `at-kr-ooe/kr-practice-1.hu.json`. Fehlt ein Overlay, erscheint
der deutsche Originaltext.
