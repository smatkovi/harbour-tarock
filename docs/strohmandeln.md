# Strohmandeln (Strohmann‑Tarock, Zweiertarock)
## Implementierungsspezifikation für harbour‑tarock (Regelprofil `AT-STROH-MS-ERW`)

Stand: 12.09.2026 · Zielplattformen: Sailfish OS (Qt 5.6 / QtQuick 2.6 / Silica), Android (Qt 6.11) · Engine: C++17

---

## 0. Vorbemerkungen, Quellenlage, Regelprofile

### 0.1 Was hier spezifiziert wird

Spezifiziert wird **Strohmandeln** (auch *Strohmandel*, *Strohmanntarock*, *Strohmann‑Tarock*,
*Zweiertarock*; englisch *Strawman Tarock*), das klassische österreichische **Tarockspiel für genau zwei
Personen** mit dem vollen 54‑Blatt „Industrie und Glück“ (IuG).

Namensgebend sind die **Strohmänner**: drei Päckchen zu je vier Karten, die jeder Spieler vor sich liegen
hat. Sie sind gleichzeitig Talon‑Ersatz *und* verdeckter Teil des eigenen Blattes; ihre oberste Karte liegt
während des Spiels offen und wird mitgespielt. Das ist die eine Regel, die Strohmandeln von jedem anderen
Tarockspiel unterscheidet, und sie ist in §4 vollständig als Zustandsautomat ausformuliert.

Nicht spezifiziert wird hier das ebenfalls zweihändige, deutlich komplexere **Kosakeln** (auf
*Illustriertem Tarock* aufbauend, 1950er Jahre) – es ist ein eigenes Spiel und bekäme ein eigenes Dokument.

### 0.2 Wichtiger Hinweis zur vom Auftrag genannten Primärquelle

> **Die im Auftrag genannte Adresse `https://www.pagat.com/tarot/strohmandeln.html` existiert nicht.**

Nachgeprüft am 11./12.09.2026:

* `https://www.pagat.com/tarot/strohmandeln.html` → **HTTP 404**. Ebenso 404: `strohmandl.html`,
  `strohmann.html`, `strohmanntarock.html`, `zweiertarock.html`.
* Die Indexseite `https://www.pagat.com/tarot/` enthält **keinen** Link auf Strohmandeln (verlinkt sind aus
  Österreich nur `koenig.html`, `lungau.html`, `zwanzig.html`, `tapptarock.html`, `dappen.html`).
* `https://www.pagat.com/national/austria.html` **nennt** Strohmandeln („Strohmandeln (2 players) … less
  widespread“), aber als **Fettdruck ohne Hyperlink** – d. h. pagat weiß von dem Spiel, hat aber keine
  Regelseite dazu. Verwiesen wird stattdessen auf Dummett/McLeod, *A History of Games Played with the Tarot
  Pack*, Vol. 2.
* Die Wayback‑Machine kennt unter `pagat.com` **keine** jemals archivierte URL, die „stroh“ oder „kosak“
  enthält (CDX‑Abfrage über die gesamte Domain, leeres Ergebnis).

Die ungarische Seite scordatura.hu behauptet, „die Regeln des ‚originalen‘ Strohmandeln seien auf pagat.com
abrufbar“ – das ist nach obigem Befund **nicht (mehr) zutreffend**. Diese Spezifikation stützt sich daher auf
die unten genannten Quellen; pagat.com kommt nur als Beleg dafür vor, dass es die Seite nicht gibt.

### 0.3 Quellen (vollständige URLs in §12)

| Kürzel | Quelle | Rolle in dieser Spezifikation |
|---|---|---|
| **[MS‑WEB]** | *Strohmann‑Tarock (Strohmandeln)* auf `tarockspiele.com`, der Website zum Buch **Mayr/Sedlaczek, *Das große Tarockbuch*** (Perlen‑Reihe 642, Wien 2001). Domain heute geparkt; gelesen in der Wayback‑Fassung vom 29.01.2002 | **Primäre Normquelle** und Grundlage des App‑Defaults. Einzige Darstellung mit vollständigen Renonce‑Regeln, Prämientabelle (klassisch/erweitert) und ausführlicher Taktik |
| **[MS‑EN]** | Tarik Wareh, *Rules for Strohmandeln*, `wareh.org` – englische Übersetzung von [MS‑WEB]. Sie beruht erkennbar auf einer **späteren** Fassung derselben Seite (sie enthält zusätzlich die 40‑Karten‑Variante, die in der archivierten Fassung von 2002 fehlt). **Wareh bezeichnet sie nicht als autorisiert und nennt kein Jahr**; die Datierung „2007“ wäre eine Vermutung und wird hier nicht behauptet | Ergänzt [MS‑WEB]; Beleg für die 40‑Blatt‑Variante und für einige Präzisierungen |
| **[MS‑WZ]** | Wolfgang Mayr / Robert Sedlaczek, *Zweiertarock (Strohmandeln) mit 54 Karten: Wie sehen die idealen Regeln aus*, **Wiener Zeitung** vom 25.09.2007, 2 Seiten | Spätere, **normative Empfehlung derselben Autoren** („ideale Regeln“) – Hartwährungs‑Tarif in Cent, Quapil, Nullrunde |
| **[TIROL]** | Markus Mair, *Strohmandeln TAROCK*, Stand **Jänner 2025**, `tarock.tirol` (PDF, 1 Seite) | **Lebendes Vereinsregelwerk** eines aktiven Cup‑Veranstalters. Einziges Regelwerk mit **Ansagen** (still/angesagt) und mit **Trischaken** beim Passen |
| **[TIROL‑G]** | Markus Mair, *Regelvergleich – Grundsätzliche Überlegungen*, `tarock.tirol` (PDF) | Maßgeblich für §1.5 (Herkunft und Äquivalenz der Zählverfahren, „36 Punkte = 35/2 Blatt“) |
| **[BECK]** | **Fritz Beck**, *Tarock komplett. Alle Spiele*, Perlen‑Reihe Nr. 640, Wien 1972, S. 40–45 | Klassische Buchnorm. **Nur mittelbar gelesen** (über [DEWIKI] und [ENWIKI], die beide ausdrücklich Beck als Grundlage angeben) |
| **[DEWIKI]** | de.wikipedia.org, *Strohmandeln* | Referat von [BECK]; kurz, in den Wertzahlen mit [ENWIKI] **im Widerspruch** (§9.2) |
| **[ENWIKI]** | en.wikipedia.org, *Strohmandeln* | Ausführlich; Geschichte, Vergleichstabelle Beck/Furr/Mayr&Sedlaczek. **Enthält zwei belegbare Fehler**: Rostopschin „VII and VIII“ statt XVII/XVIII (§9.9) und eine Trull‑Prämie in der Furr‑Spalte, die [FURR] gar nicht kennt (§9.14) |
| **[FURR]** | Jerry Neill Furr, *Straw Man Tarock*, in: *Tarocchi. An introduction to the many games played with tarot cards*, Philebus 2009, Kap. 5.05, S. 109–111 (Volltext auf archive.org); identischer Text auf `tarocchino.com` (archiviert) | Englischsprachige Hausregel‑Fassung; weicht in Ausspielrecht und Prämien ab |
| **[BAMB]** | Johannes Bamberger, *Tarock: Die schönsten Varianten*, Perlen‑Reihe 640, 22. Aufl., Wien 2011, S. 23–28 | **Nicht im Volltext gelesen**; nur über [ENWIKI] referiert (folgt Beck, aber ohne „einfaches Spiel“) |
| **[DUMMETT]** | Michael Dummett, *The Game of Tarot*, Duckworth, London 1980, S. 457–459 | **Nicht im Volltext gelesen**; über [ENWIKI] für die Spielgeschichte referiert |
| **[HIST]** | *Neuestes Spielbuch*, Wien 1800, S. 145 f.; *Neuestes Allgemeines Spielbuch*, Wien 1829, S. 94; 2. Aufl. 1839, S. 121; S. Ulmann, *Das Buch der Familienspiele*, Wien/München/Pest 1890, S. 244 f. | Spielgeschichte (§9.1). **Nicht im Volltext gelesen**, über [ENWIKI] referiert |
| **[HU‑SCO]** | *Strohmandli*, `scordatura.hu` („StüsziVadász“), ungarische 42‑Blatt‑Fassung | Regionale Abweichung Ungarn (§9.12) |
| **[PAGAT]** | pagat.com (Tarot‑Index, Austria‑Seite) | Belegt nur, dass es dort **keine** Strohmandeln‑Regelseite gibt (§0.2) |

Zitierweise im Folgenden: `[MS‑WEB §Prämien]`, `[TIROL]`, `[ENWIKI Scoring]` usw.

### 0.4 Regelprofile in der App

Die Engine bildet das Regelwerk als **Profil‑Datenstruktur** ab (keine `#ifdef`, kein Sonderzweig im Code).
Ausgeliefert werden:

| Profil‑ID | Beschreibung | Status |
|---|---|---|
| `AT-STROH-MS-ERW` | **Mayr/Sedlaczek, erweiterte Variante** ([MS‑WEB]/[MS‑EN]): alle Prämien still, Grammel‑Punkt, Uhu, Kakadu, Rostopschin, kein Kontra | **Default** |
| `AT-STROH-MS-KLASS` | Mayr/Sedlaczek, klassische Variante: nur Trull/vier Könige **im Blatt angesagt**, Pagat still, Valat | Option |
| `AT-STROH-MS-WZ2007` | Mayr/Sedlaczek, „ideale Regeln“ 2007 ([MS‑WZ]): Hartwährung in Cent, zusätzlich Quapil, kein Grammel‑Punkt, kein Rostopschin | Option |
| `AT-STROH-TIROL-2025` | tarock.tirol ([TIROL]): nur Pagat ultimo / vier Könige / Trull, **still 1 – angesagt 2**, **Trischaken** wenn beide passen, Partie bis 11 | Option |
| `AT-STROH-BECK-1972` | Beck, wie in [ENWIKI] tabelliert (Trull/Königstrull **in der Hand**, Mondfang‑Prämie, bei Neugabe gibt die **alte Vorhand**) | Option |
| `EN-STROH-FURR-2009` | Furr ([FURR] Volltext, **nicht** [ENWIKI]s Tabelle): Aufnehmer spielt aus, „With 45“/„With 26“ (Verteidiger‑Schwelle 44), Rostopschin, **keine** Trull‑, Quapil‑ und Mondfang‑Prämie | Option |
| `AT-STROH-40` | 40‑Blatt‑Variante der Zwanzigerrufen‑Kreise ([MS‑EN]) | Option, §9.13 |

Jede profilabhängige Regel trägt unten die Markierung **⚙ Flag: `flag_name`**. Jeder in §9 genannte
Streitpunkt ist ein **einzelnes** Flag, damit Spielkreise ihre Hausregeln abbilden können.

### 0.5 Sprachliche Konventionen und Kartenkürzel

Österreichische Fachbegriffe bleiben im Original (Glossar §11.7).

* **Tarock** (Trümpfe): `Sküs`, `XXI` … `I`. Auf den Karten steht `IIII`, nicht `IV`; die App zeigt `IIII`.
* **Farben:** `♥` Herz (Cœur), `♦` Karo (Carreau), `♠` Pik (Pique), `♣` Treff (Trèfle).
* **Farbränge:** `K` König, `D` Dame, `C` Cavall (auch Reiter/Chevalier), `B` Bube (Valet).
  Rote Zahlkarten `A 2 3 4`, schwarze `10 9 8 7`.
* **Kartenkürzel:** `♠K` = Pik‑König, `♣10` = Treff‑Zehner, `XXI` = Mond, `I` = Pagat.
* **Rollen:** **Vorhand** = der Spieler, der *nicht* gegeben hat. **Geber** = der andere.
  **Aufnehmer** = wer das Spiel aufgenommen hat (es gibt in diesem Spiel keinen „Alleinspieler“ im
  Königrufen‑Sinn, weil ohnehin jeder für sich spielt).
* **Strohmann** = eines der drei Päckchen. **Deckblatt** = dessen offen aufliegende oberste Karte.
* **Leerkarte** (auch *Skartindel*) = jede Karte mit Zählwert 1, also Tarock `II`–`XX` und alle
  Farb‑Zahlkarten.
* **Zählkarte** = jede Karte mit Zählwert > 1, also `Sküs`, `XXI`, `I`, alle `K`, `D`, `C`, `B`.

---

## 1. Blatt, Rangfolgen, Kartenwerte, Zählung

### 1.1 Das Blatt: alle 54 Karten des IuG‑Decks

Strohmandeln verwendet das **vollständige** Tarockblatt „Industrie und Glück“ zu **54 Karten** – es wird
**nichts entfernt** [MS‑WEB], [DEWIKI], [ENWIKI], [TIROL], [FURR]:

```
Deck = 22 Tarock + 4 Farben × 8 Karten = 54 Karten
```

* **22 Tarock:** `Sküs` und `XXI` … `I`
* **32 Farbkarten:** je 8 in ♥, ♦, ♠, ♣

Das unterscheidet Strohmandeln von Zwanzigerrufen (40 Blatt) und vom ungarischen Tarokk (42 Blatt) und
bedeutet für die App: **dasselbe Kartenmaterial wie Königrufen**, kein zusätzliches Deckprofil
(Deckprofil `IuG-54`).

Jeder Spieler bekommt **27 Karten** (15 Hand + 12 Strohmann). Es gibt **keinen übrig bleibenden Talon**;
das gesamte Deck ist im Spiel.

### 1.2 Tarockordnung (Trumpffolge)

Absteigend:

```
Sküs > XXI > XX > XIX > XVIII > XVII > XVI > XV > XIV > XIII > XII > XI > X > IX > VIII > VII > VI > V > IIII > III > II > I
```

* Der **Sküs** (Narrenbild, ohne Zahl) ist der **höchste Tarock**. Er ist **kein** „Excuse“/„Narr“ wie im
  französischen Tarot: er sticht ganz normal, unterliegt dem Farb‑/Tarockzwang, kann aber von nichts
  gestochen werden [MS‑WEB §Zählwerte: „Der Sküs ist der höchste Trumpf“], [FURR: „the Fool, which is the
  highest trump“]. → §6.5.
* **`I` = Pagat** (auch *Spatz*), der **niedrigste** Tarock. Er ist zugleich ein Fünfer‑Zähler – das ist die
  Spannungsquelle des ganzen Spiels.
* **`XXI` = Mond.**
* **`II` = Uhu**, **`III` = Kakadu**, **`IIII` = Quapil** – nur relevant, wenn die entsprechenden Prämien
  aktiv sind (§5.4).
* **Trull** (Trullstücke, „Honneurs“) = `Sküs`, `XXI`, `I`. **Königstrull / vier Könige** = `♥K ♦K ♠K ♣K`.
* Tarock ist im Sinne der Spielregeln eine **fünfte Farbe**: „auf Tarock muss Tarock zugegeben werden“
  (§6.2).

### 1.3 Farbreihenfolgen – rot und schwarz sind unterschiedlich!

| Farbe | Rangfolge hoch → niedrig |
|---|---|
| **Schwarz** (♠ Pik, ♣ Treff) | `K` > `D` > `C` > `B` > **`10` > `9` > `8` > `7`** |
| **Rot** (♥ Herz, ♦ Karo) | `K` > `D` > `C` > `B` > **`A` > `2` > `3` > `4`** |

Beleg wörtlich: „König, Dame, Kavall, Bube; schwarze Farben: 10 bis 7; rote Farben: Ass bis 4“
[MS‑WEB], gleichlautend [ENWIKI Cards], [FURR: „using irrational ranking“].

Merksatz für den Lernmodus: **In Schwarz zählt die Zehn am meisten, in Rot das Ass** – in beiden Farben ist
die *jeweils erstgenannte* Zahlkarte die höchste, aber schwarz läuft abwärts (10‑9‑8‑7) und rot aufwärts
(A‑2‑3‑4). Diese Asymmetrie stammt aus dem historischen italienischen Blatt (runde Farben Becher/Münzen
gegen lange Farben Schwerter/Stäbe) und ist in **allen** Tarockspielen des IuG‑Decks gleich.

**Implementierungshinweis:** eine Rangtabelle pro Farbfamilie, keine Sonderfälle im Vergleichscode:

```cpp
// rank(card): je größer, desto stärker; nur innerhalb derselben Farbe vergleichbar
static constexpr int kSuitRank[] = { /*7/4*/0, /*8/3*/1, /*9/2*/2, /*10/A*/3, 4 /*B*/, 5 /*C*/, 6 /*D*/, 7 /*K*/ };
```

### 1.4 Kartenwerte

| Karte | Wert | Anzahl | Summe roh |
|---|---|---|---|
| Trullstücke: `Sküs`, `XXI`, `I` | **5** | 3 | 15 |
| Tarock `II`–`XX` | **1** | 19 | 19 |
| König `K` | **5** | 4 | 20 |
| Dame `D` | **4** | 4 | 16 |
| Cavall `C` | **3** | 4 | 12 |
| Bube `B` | **2** | 4 | 8 |
| Farb‑Zahlkarten (rot `A 2 3 4`, schwarz `10 9 8 7`) | **1** | 16 | 16 |
| **Summe** | | **54** | **106** |

[MS‑WEB], [DEWIKI], [ENWIKI], [FURR], [TIROL] stimmen hier vollständig überein.

### 1.5 Zählmethode (exakt)

Gezählt wird die **österreichische Dreierlagen‑Zählung**. Historisch stammt sie aus dem Dreier‑Tarock: dort
zählte jede Karte einen Punkt weniger als heute, dafür gab es pro Stich (= 3 Karten) einen Zusatzpunkt. Um
das doppelte Zählen zu vermeiden, erhöhte man die Kartenwerte um 1 und zog pro Dreierlage 2 Punkte ab – das
ergibt exakt dasselbe [TIROL‑G].

#### 1.5.1 Die exakte Definition (für die Engine maßgeblich)

> **Punktezahl eines Stichhaufens** `P = Σ Kartenwerte − (2/3) · Kartenanzahl`

Das ist die einzige Formel, die die Engine braucht. Sie wird **in Dritteln** gerechnet (Integer‑Arithmetik in
Dritteln, `p3 = 3·Σwert − 2·n`), niemals in `double`.

Probe fürs ganze Deck: `106 − (2/3)·54 = 106 − 36 = 70`. **Das Spiel hat 70 Punkte.**

#### 1.5.2 Die drei praktischen Verfahren am Tisch (alle äquivalent)

[MS‑WEB] nennt sie ausdrücklich: „in Dreierlagen mit 2 Punkte‑Abzug **oder** jeweils 1 Zählkarte und 2
Leerkarten **oder** in Bruchzahlen“.

**(a) Dreierlagen (Standard, ganzzahlig).** Karten zu Lagen à 3 stapeln, Werte addieren, pro vollständiger
Lage 2 Punkte abziehen. Für die unvollständige Restlage (1 oder 2 Karten) **1 Punkt** abziehen [TIROL],
[TIROL‑G]. Ergebnis: ganze Zahlen, Gewinnschwelle **36**.
*Beispiel:* `♠K + ♠C + XIII = 5+3+1 = 9 − 2 = 7`.

**(b) Zählkarte + zwei Leerkarten („blattgenau“).** Jede Zählkarte mit zwei Leerkarten zu einer Lage
kombinieren – dann zählt die Zählkarte **ihren vollen Wert** (denn `Wert + 1 + 1 − 2 = Wert`). Übrig
bleibende Leerkarten heißen **„Blatt“** und zählen je **⅓**. Ergebnis in der Form *„n Punkte und r Blatt“*,
Gewinnschwelle **35 und 2 Blatt**.

**(c) Bruchzahlen.** Jede Karte zählt `Wert − ⅔`:

| Karte | Bruchwert |
|---|---|
| `Sküs`, `XXI`, `I`, `K` | 4 ⅓ |
| `D` | 3 ⅓ |
| `C` | 2 ⅓ |
| `B` | 1 ⅓ |
| Leerkarte | ⅓ |

Gewinnschwelle **35 ⅔**.

#### 1.5.3 Umrechnung und Äquivalenz

* `„n Punkte und r Blatt“` ⟺ exakt `n + r/3` (r ∈ {0,1,2}).
* Verfahren (a) ist genau die **Rundung** von (c): `⅓ → 0`, `⅔ → 1`. [TIROL‑G] führt beide als **getrennte**
  althergebrachte Verfahren auf („bei unvollständigen Lagen wird nur ein Punkt abgezogen“ *bzw.* „Andere
  Spieler runden einfach die übriggebliebenen Karten zu vollen Punkten (2/3 = 1, 1/3 = 0)“). Sie liefern
  **immer** dasselbe Ergebnis; Beweis: mit `n = 3k + r` ist der exakte Wert `Σ − 2k − 2r/3`, also für `r = 1`
  `(Σ−2k−1) + ⅓` → abgerundet `Σ−2k−1`, für `r = 2` `(Σ−2k−2) + ⅔` → aufgerundet `Σ−2k−1`. Beides ist der
  Wert von Verfahren (a), das für die Restlage pauschal 1 abzieht. Welche Karten in der Restlage liegen,
  ist dabei irrelevant – nur ihre Anzahl zählt.
* Weil ein Spieler `n` und der andere `54 − n` Karten hat, gilt für die Restlagen zwingend
  `r_A = 0 ⟹ r_B = 0`, `r_A = 1 ⟹ r_B = 2` und umgekehrt. Andere Kombinationen sind ein Rechenfehler und
  müssen in der Engine assertiert werden.
* **Deshalb ist 36 (gerundet) exakt dasselbe wie 35/2 Blatt** – und *nicht* dasselbe wie 35/1 Blatt
  [TIROL‑G]: „darf das Spiel erst mit 35/2 Blatt gewonnen sein (und nicht schon mit 35/1)“.

```cpp
// Engine: Punkte in Dritteln, exakt und überlauffrei
int points3(const std::vector<Card>& pile) {          // Rückgabe = 3 × Punkte
    int raw = 0; for (const Card& c : pile) raw += value(c);
    return 3 * raw - 2 * int(pile.size());
}
inline int pointsRounded(int p3) {                     // Verfahren (a)
    return (p3 + 1) / 3;                               // p3 ≥ 0; ⅓ ab, ⅔ auf
}
inline std::pair<int,int> pointsAndBlatt(int p3) {     // Verfahren (b): {Punkte, Blatt}
    return { p3 / 3, p3 % 3 };
}
```

### 1.6 Durchgerechnetes Zählbeispiel

Spieler **A** hat am Ende folgende **26 Karten** in seinen Stichen:

> **Tarock (13):** `Sküs`, `XXI`, `XIX`, `XVII`, `XV`, `XIII`, `XI`, `IX`, `VII`, `V`, `III`, `II`, `I`
> **♥ (5):** `♥K`, `♥D`, `♥B`, `♥A`, `♥2`  **♦ (2):** `♦C`, `♦3`
> **♠ (3):** `♠C`, `♠10`, `♠9`  **♣ (3):** `♣D`, `♣8`, `♣7`

**Schritt 1 – Rohsumme.**

| Gruppe | Rechnung | Summe |
|---|---|---|
| Trullstücke `Sküs`,`XXI`,`I` | 3 × 5 | 15 |
| Tarock `XIX XVII XV XIII XI IX VII V III II` | 10 × 1 | 10 |
| `♥K` 5 + `♥D` 4 + `♥B` 2 + `♥A` 1 + `♥2` 1 | | 13 |
| `♦C` 3 + `♦3` 1 | | 4 |
| `♠C` 3 + `♠10` 1 + `♠9` 1 | | 5 |
| `♣D` 4 + `♣8` 1 + `♣7` 1 | | 6 |
| **Rohsumme Σ** | | **53** |

**Schritt 2 – Abzug.** A hat 26 Karten.

* **Exakt (c):** `53 − (2/3) · 26 = 53 − 17⅓ = ` **`35 ⅔`**
* **Dreierlagen (a):** 26 = 8 volle Lagen + 1 Restlage zu 2 Karten → `53 − 8·2 − 1 = 53 − 17 = ` **`36`**
* **Blattgenau (b):** A hat 9 Zählkarten (Summe 36) und 17 Leerkarten. Legt man 8 Lagen (= 24 Karten, darin
  alle 9 Zählkarten und 15 Leerkarten, Summe 51, minus 16) = **35**, bleiben **2 Blatt** übrig →
  **`35 und 2 Blatt`**.

**Schritt 3 – Gegenprobe beim Gegner B.** B hat die anderen **28 Karten**, ebenfalls mit Rohsumme 53
(10 Zählkarten Summe 35 + 18 Leerkarten).

* Exakt: `53 − (2/3) · 28 = 53 − 18⅔ = ` **`34 ⅓`**
* Dreierlagen: 28 = 9 volle Lagen + 1 Restkarte → `53 − 18 − 1 = ` **`34`**
* Blattgenau: **`34 und 1 Blatt`**

**Probe:** `35⅔ + 34⅓ = 70` ✔ und `36 + 34 = 70` ✔ und `(35,2 Blatt) + (34,1 Blatt) = 70` ✔.

**Ergebnis:** A hat **35 und 2 Blatt** erreicht – das ist die **knappst mögliche** Gewinnzahl. A gewinnt.

### 1.7 Gewinnschwelle

| Ergebnis exakt | blattgenau | gerundet | `p3` | Bewertung |
|---|---|---|---|---|
| ≥ 35 ⅔ | ≥ 35/2 Blatt | ≥ 36 | ≥ 107 | **gewonnen** |
| 35 ⅓ | 35/1 Blatt | 35 | 106 | **nicht** gewonnen – Unentschieden‑Zone (§1.7.1) |
| 35 | 35/0 Blatt | 35 | 105 | **nicht** gewonnen – Unentschieden‑Zone |
| 34 ⅔ | 34/2 Blatt | 35 | 104 | **nicht** gewonnen – Unentschieden‑Zone |
| ≤ 34 ⅓ | ≤ 34/1 Blatt | ≤ 34 | ≤ 103 | verloren (der Gegner hat ≥ 35 ⅔) |

**Achtung – häufiger Denkfehler:** `34 ⅔` ist **kein** verlorenes, sondern ein **unentschiedenes** Ergebnis,
denn der Gegner hat dann `35 ⅓` und erreicht die Schwelle ebenfalls nicht. Die Unentschieden‑Zone umfasst
in Dritteln genau `p3 ∈ {104, 105, 106}` (§1.7.1). Erst ab `p3 ≤ 103` hat der Gegner das Spiel wirklich
gewonnen. Die Engine prüft daher **nie** „verloren = nicht ≥ 107“, sondern immer beide Seiten getrennt.

**„Zum Gewinn eines Spiels sind mindestens 35 Punkte und 2 Blatt erforderlich.“** [MS‑WEB], wortgleich
[MS‑WZ]; „um zu gewinnen sind 36 Punkte erforderlich (= 35/2 Blatt bei blattgenauer Zählweise)“ [TIROL];
„To win the game the declarer needs 36 points (out of 70)“ [ENWIKI], [FURR].

#### 1.7.1 Die Unentschieden‑Zone

Weil die Schwelle bei 35 ⅔ liegt, gibt es genau **zwei** Ergebnislagen, in denen **keiner** der beiden
Spieler die Schwelle erreicht:

1. `35 : 35` (`p3` 105 : 105)
2. `35⅓ : 34⅔` bzw. `34⅔ : 35⅓` (= „35/1 Blatt zu 34/2 Blatt“; `p3` 106 : 104 bzw. 104 : 106)

In Dritteln also genau `p3 ∈ {104, 105, 106}` für **beide** Spieler gleichzeitig – drei Werte, zwei
Ergebnislagen.

[MS‑WZ] wörtlich: „Ein Ergebnis von 35 zu 35 Punkten, aber auch von 34 Punkten/2 Blatt zu 35 Punkten/1 Blatt
sowie umgekehrt, ist in einem einfachen Spiel ein Unentschieden.“

**⚙ Flag `draw_scope`:**

| Wert | Bedeutung | Quelle |
|---|---|---|
| `simple_only` | Unentschieden **nur im einfachen Spiel**; im aufgenommenen Spiel hat der Aufnehmer die Schwelle verfehlt und **verliert** | [MS‑WEB], [MS‑WZ] — **Default** |
| `always` | Unentschieden in jedem Spiel; es werden nur die Prämien verrechnet | [DEWIKI] („für ein solches werden nur Ansagen geschrieben oder ausgezahlt“) |
| `declarer_loses` | wie `always`, aber das aufgenommene Spiel gilt dem Aufnehmer als verloren | [DEWIKI], ausdrücklich als Variante genannt |

**Empfehlung: `simple_only`.** Der Aufnehmer hat das Spiel riskiert; verfehlt er 35/2, hat er sein Ziel nicht
erreicht. Das ist die Logik von [MS‑WEB] und vermeidet einen Sonderzweig in der Abrechnung.

Folge eines Unentschiedens: **Nullrunde**, §8.3.

---

## 2. Sitzordnung, Geben, Strohmänner legen

### 2.1 Zwei Sitze

Strohmandeln ist ein reines Zweipersonenspiel. In der App sind die beiden Sitze `SEAT_0` und `SEAT_1`; jeder
kann **Mensch oder Computerspieler** sein (auch beide – Demo‑Modus).

* **Vorhand** = wer **nicht** gegeben hat. **Geber** = wer gegeben hat.
* Die Vorhand hat sämtliche Vorrechte: sie erklärt sich zuerst (§3) und spielt zum ersten Stich aus (§6.1).
* Nach jedem Spiel wechselt das Geben. **Runde** = zwei Spiele, sodass jeder einmal gegeben hat (§8.1).

### 2.2 Wer gibt zuerst

Es wird abgehoben bzw. gelost; danach wird abwechselnd geteilt [DEWIKI: „Es wird gelost, welcher der beiden
Spieler mit dem Teilen beginnt, anschließend wird abwechselnd geteilt“], [ENWIKI: „The deck is cut to decide
which of the two players deals first“], [FURR: „First Dealer is chosen at random“].

**App:** Beim ersten Spiel einer Partie zieht jeder verdeckt eine Karte; der **höhere Tarock bzw. die höhere
Karte** gibt (UI‑Animation, überspringbar). Danach strikter Wechsel.

### 2.3 Mischen, Abheben, Geben

1. Der Geber mischt, der Gegner hebt ab.
2. **Handkarten:** Der Geber gibt in **3 Durchgängen je 5 Karten**, beginnend bei der **Vorhand**
   → jeder hat **15 Handkarten** [MS‑WEB], [DEWIKI], [TIROL], [FURR].
3. **Strohmänner:** Die restlichen **24 Karten** werden als **6 Päckchen à 4 verdeckten Karten** ausgelegt,
   drei vor jeden Spieler. Sie werden ebenso **wechselweise** ausgeteilt: *Päckchen 1 → Vorhand,
   Päckchen 2 → Geber, Päckchen 3 → Vorhand, …* [MS‑WEB: „so dass die Vorhand das erste, der Geber das zweite
   usw. erhält“].
4. Jeder Spieler hat damit **27 Karten** = 15 Hand + 3 × 4 Strohmann. Es bleibt **kein Talon** übrig.

Die drei Päckchen eines Spielers liegen nebeneinander; die App nummeriert sie aus Sicht ihres Besitzers von
links (`S1`) nach rechts (`S3`). Das ist relevant, weil die Erstaufdeckung „von links nach rechts“ erfolgt
[DEWIKI], [ENWIKI].

### 2.4 Zeitpunkt des Strohmänner‑Legens — **⚙ Flag `strawmen_dealt_before_bidding`**

| Wert | Ablauf | Quelle |
|---|---|---|
| `true` | Hand **und** Strohmänner werden in einem Zug gegeben; danach erklärt sich die Vorhand | [MS‑WEB], [TIROL], [FURR] — **Default** |
| `false` | Zuerst nur die 15+15 Handkarten; erst **nachdem** ein Spiel aufgenommen wurde, werden die Strohmänner gelegt | [DEWIKI]/[BECK] („Nimmt einer das Spiel auf …, werden vor jeden Spieler je drei Päckchen à vier Karten Talon gelegt“); [MS‑WEB] empfiehlt das ausdrücklich für den Fall, dass das einfache Spiel gar nicht gespielt wird |

**Empfehlung: `true`.** Spielentscheidend ist der Unterschied nicht (niemand sieht die Strohmänner vor der
Erklärung), aber `true` erlaubt eine einzige Gebe‑Animation und ein einfacheres Zustandsmodell. `false` ist
nur zusammen mit `redeal_if_both_pass = true` (§3.2) sinnvoll.

### 2.5 Zusammenwerfen ohne Tarock (Neugabe)

> „Hat ein Spieler nach Austeilen der Karten und **vor Aufdecken der Strohmänner** kein Tarock im
> Handblatt, so kann er zusammenwerfen. In diesem Fall gibt **derselbe Spieler** neu.“ [MS‑WEB], [MS‑WZ]

* Das Recht ist **fakultativ** („kann“) und steht **beiden** Spielern zu.
* Maßgeblich ist allein das **Handblatt** (15 Karten); Tarock in den eigenen Strohmännern zählen nicht.
* Es muss **vor** der Erstaufdeckung (§4.2) ausgeübt werden. In der App: eigener Zustand
  `AWAIT_THROW_IN` mit Timeout, nur sichtbar, wenn die Bedingung tatsächlich erfüllt ist.
* Nach dem Zusammenwerfen gibt **derselbe** Geber erneut; die Geber‑Rotation verschiebt sich nicht.

**⚙ Flag `throw_in_no_tarock` (Default `true`).** [TIROL] und [FURR] erwähnen die Regel nicht; im Profil
`AT-STROH-TIROL-2025` ist sie daher `false`.

### 2.6 Vergeben (Misdeal)

Strafe: **1 Spielpunkt** an den Gegner, in einer verdoppelten Runde **2 Spielpunkte** [MS‑WEB].
Danach gibt derselbe Spieler neu.

Die App vergibt nie falsch; die Regel ist trotzdem zu implementieren, weil sie in physischen Turnier‑Modi
(LAN mit Papierabrechnung) nachgetragen werden kann → Menüpunkt „Strafpunkt eintragen“.

### 2.7 Sicht‑ und Netzwerkmodell (LAN, Server ist autoritativ)

Weil Strohmandeln mehrere Sichtbarkeitsstufen hat, ist der Server **zwingend** autoritativ. Pro Karte gilt
genau einer der Zustände:

| Zustand | sichtbar für | Beispiel |
|---|---|---|
| `HAND_PRIVATE` | nur Besitzer | die 15 Handkarten |
| `HAND_KNOWN` | Besitzer **und Gegner** | ein Tarock/König, der offen aus einem Strohmann ins Blatt genommen wurde (§4.3) |
| `PILE_HIDDEN` | niemand | Karten unter dem Deckblatt |
| `PILE_FACEUP` | **beide** | das Deckblatt |
| `HAND_SECRET_LAST` | nur Besitzer | die **letzte** Karte eines Päckchens, verdeckt aufgenommen (§4.5) |
| `PLAYED` / `TRICK_TAKEN` | beide | gespielte Karten |

`HAND_KNOWN` ist die für die KI wertvollste Information des Spiels: Der Gegner **weiß**, welche Tarock und
Könige du aus den Strohmännern gezogen hast (§10.1). Der Client darf `PILE_HIDDEN` und `HAND_SECRET_LAST`
**nie** erhalten; der Server sendet dafür nur Platzhalter mit Kartenanzahl.

---

## 3. Die Spielstufen (Verträge) und die Spielaufnahme

### 3.1 Es gibt nur zwei (bzw. drei) Spielstufen

Strohmandeln kennt **kein Lizit** im Sinne von Königrufen – keine Spielstufenleiter, kein Überbieten, kein
Halten. Es gibt nur die Frage: **Nimmt jemand auf?**

| # | Spielstufe | Wer erklärt | Spielziel | Wert (Default `AT-STROH-MS-ERW`) |
|---|---|---|---|---|
| 1 | **Einfaches Spiel** (beide haben gepasst) | – | mehr als die Hälfte der Punkte (≥ 35/2 Blatt) | 1 Spielpunkt an den Gewinner; 2 bei ≥ 45 Punkten; Unentschieden möglich |
| 2 | **Aufgenommenes Spiel** | Vorhand zuerst, dann Geber | der **Aufnehmer** braucht ≥ 35/2 Blatt | Aufnehmer gewinnt **3**, verliert **4**; 4 bzw. 5 mit Grammel‑Punkt |
| 3 | *(nur `AT-STROH-TIROL-2025`)* **Trischaken** (beide haben gepasst) | – | **möglichst wenige** Punkte | 2 Spielpunkte an den Gewinner |

Die Stufen stehen in dieser Reihenfolge, weil das aufgenommene Spiel das einfache Spiel **ersetzt**, sobald
sich jemand erklärt. Es gibt **keine höhere Stufe** – kein Solo, kein Farbenspiel, kein Negativspiel
(außer Trischaken im Tiroler Profil), keine Ouvert‑Spiele.

### 3.2 Ablauf der Erklärung („Lizit“)

```
             ┌────────────────────────┐
  Geben ───▶ │  AWAIT_FOREHAND        │
             └─────────┬──────────────┘
              „Ich nehme auf!“ │ „Weiter!“ / „Ich passe!“
                   ▼           ▼
        AUFGENOMMENES     ┌────────────────────┐
        SPIEL (Vorhand)   │  AWAIT_DEALER      │
                          └───────┬────────────┘
                „Ich nehme auf!“  │  „Ich passe!“
                       ▼          ▼
             AUFGENOMMENES    EINFACHES SPIEL
             SPIEL (Geber)    (bzw. Neugabe / Trischaken)
```

* **Vorhand erklärt sich zuerst.** Wortlaut am Tisch: **„Ich nehme auf!“** bzw. **„Weiter!“** oder
  **„Ich passe!“** [MS‑WEB], [MS‑WZ], [ENWIKI].
* Passt die Vorhand, **darf** (nicht muss) der Geber aufnehmen.
* Passen beide, kommt es zum **einfachen Spiel** – es wird also trotzdem gespielt [MS‑WEB], [MS‑WZ],
  [DEWIKI], [FURR].
* **Die Erklärung ist unwiderruflich.** Es gibt **kein Überbieten**, kein „Halten“, keine zweite Runde.
  Sobald ein Spieler aufgenommen hat, endet die Erklärungsphase sofort – auch für den anderen.
* Die Erklärung erfolgt **vor** der Erstaufdeckung der Strohmänner [TIROL: „bevor der Strohmann aufgedeckt
  wird“]. Das ist wesentlich: Man entscheidet auf Basis von 15 von 27 eigenen Karten (§10.2).

**⚙ Flag `redeal_if_both_pass`:**

| Wert | Verhalten bei „beide passen“ | Quelle |
|---|---|---|
| `simple_game` | Es wird ein **einfaches Spiel** gespielt | [MS‑WEB], [MS‑WZ], [FURR] — **Default** |
| `redeal` | Karten zusammenwerfen, es wird **neu gegeben** (wer gibt: siehe `redeal_dealer`) | „Alte Tarockbücher empfehlen, ein einfaches Spiel gar nicht zu spielen, sondern die Karten zusammenzuwerfen“ [MS‑WEB], [MS‑EN]; so auch [BAMB] laut [ENWIKI]; [DEWIKI]/[BECK] nennt es als Hauptregel mit dem einfachen Spiel als Vereinbarung |
| `trischaken` | Es wird **Trischaken** gespielt (§5.9) | [TIROL] |

**⚙ Flag `redeal_dealer` (nur bei `redeal_if_both_pass = redeal` wirksam) — hier widersprechen sich die
Quellen ausdrücklich:**

| Wert | Wer gibt neu | Quelle |
|---|---|---|
| `same_dealer` | **derselbe Geber** | [MS‑WEB]: „Anschließend gibt **derselbe Geber** neu.“; [MS‑EN]: „Subsequently, the same dealer deals again.“ — **Default**, weil die Profilfamilie `MS-*` die Normquelle dieser Spezifikation ist |
| `forehand_deals` | die **bisherige Vorhand**, d. h. das Geben rotiert normal weiter | [DEWIKI]/[BECK]: „es wird **von der alten Vorhand** neu gegeben“; [ENWIKI]: „a new hand dealt by the player who was forehand“ |

Der Unterschied ist **nicht** kosmetisch: `same_dealer` lässt den Vorhandvorteil (zuerst erklären, zuerst
ausspielen) beim selben Spieler, bis endlich jemand aufnimmt, `forehand_deals` reicht ihn weiter. Im Profil
`AT-STROH-BECK-1972` ist daher `forehand_deals` gesetzt. Vgl. dagegen §2.5: beim **Zusammenwerfen ohne
Tarock** sagen [MS‑WEB] *und* [MS‑WZ] übereinstimmend „derselbe Spieler gibt neu“, dort gibt es keinen
Widerspruch.

**Empfehlung: `simple_game`.** Eine App, die Karten wegwirft, fühlt sich für Anfänger frustrierend an; das
einfache Spiel ist zudem das beste Lernvehikel (kein Risiko, volle Spielmechanik).

### 3.3 Wertetabelle der Spielstufen nach Profil

| Ereignis | `MS-ERW` (Default) | `MS-KLASS` | `MS-WZ2007` | `TIROL-2025` | `BECK-1972` | `FURR-2009` |
|---|---|---|---|---|---|---|
| Einfaches Spiel gewonnen | **1** | 2 | 2 (20 ct) | – *(Trischaken 2)* | 1 | 2 |
| Einfaches Spiel, Sieger ≥ 45 Punkte | **2** | – | – | – | – | – |
| Einfaches Spiel unentschieden | 0, Nullrunde | 0, Nullrunde | 0, Nullrunde | – | 0 | – |
| Aufgenommenes Spiel **gewonnen** | **3** | 3 | 3 (30 ct) | 2 | 2 | 3 |
| Aufgenommenes Spiel **verloren** (an den Gegner) | **4** | 4 | 4 (40 ct) | 3 | 3 | 4 |
| Aufnehmer ≥ 45 Punkte | **4** | – | – | – | – | 4 |
| Aufnehmer **unter 26** Punkten (M&S) bzw. **26 oder weniger** (Furr) | **5** | – | – | – | – | 5 |

Quellen: [MS‑WEB §Die Werte der Spielansagen und der Prämien] (Spalten 1 und 2, Zeilenbeschriftungen dort
wörtlich „Aufnehmer mind. 45 Punkte“ / „Aufnehmer unter 26 Punkte“); [MS‑WZ]
(„Einfaches Spiel gewonnen/verloren: 20 Cent … Aufgenommenes Spiel gewonnen: 30 Cent, verloren: 40 Cent“);
[TIROL] („Spiel gewonnen 2 / Spiel verloren 3“); [ENWIKI Scoring] und [FURR S. 110] für Beck und Furr.

**Achtung, die letzte Zeile fasst zwei *verschiedene* Schwellen zusammen** (deshalb die Doppelbeschriftung):

* **M&S** ([MS‑WEB]): „Aufnehmer **unter** 26 Punkte“, d. h. Aufnehmer ≤ 25 ⟺ Gegner ≥ 45. Die Zeile ist nur
  die Kehrseite des Grammel‑Punkts (§5.6), den der Gegner erzielt.
* **Furr** ([FURR]): „With 26: If Declarer takes **26 card points or less**: Scores 5 for the defender“,
  d. h. Aufnehmer ≤ 26 ⟺ Gegner ≥ 44. Furrs Verteidiger‑Großgewinn liegt also **einen Punkt tiefer**;
  [ENWIKI] gibt das konsistent als „Major win by defender: Win by defender with **44** or more points“
  wieder. Furrs Gewinner‑Schwelle „With 45“ (Aufnehmer ≥ 45) stimmt dagegen mit M&S überein.
  Die Asymmetrie ist bei Furr systematisch: der Verteidiger gewinnt das Spiel schon mit 35 Punkten, also
  liegen auch seine Großgewinn‑Schwellen um 1 niedriger.

Die Engine bildet das über zwei getrennte Flags ab (§5.6): `grand_point_threshold` (eigene Punkte, Default
45) und `grand_point_defender_threshold` (Default = `grand_point_threshold`, im Profil `FURR-2009` = 44).

**Lesart „verloren“:** Der Wert ist immer **positiv für den Gewinner**. Verliert der Aufnehmer, bekommt sein
Gegner 4 (nicht 3) – die asymmetrische Bestrafung des Risikos ist der Kern des Spiels: *„Der Aufnehmer hat
die Chance, 1 Spielpunkt mehr als in einem einfachen Spiel zu gewinnen, und er riskiert, 2 Spielpunkte
gegenüber einem einfachen Spiel zu verlieren.“* [MS‑WEB]

**Achtung Profilinkonsistenz (dokumentierter Widerspruch):** In `MS-ERW` ist das einfache Spiel nur **1**
wert, weil die Erhöhung auf **2** dem Grammel‑Punkt vorbehalten ist (§5.6); in [MS‑WZ] – derselben Autoren,
sechs Jahre später – ist es **2** und es gibt gar keinen Grammel‑Punkt. Beides ist belegt; die App bildet
beide als eigene Profile ab. Siehe §9.4.

### 3.4 Wer darf was – Übersicht

| Recht | Vorhand | Geber |
|---|---|---|
| Sich zuerst erklären | ✔ | – |
| Aufnehmen | ✔ (zuerst) | ✔ (nur wenn Vorhand passt) |
| Zusammenwerfen ohne Tarock (§2.5) | ✔ | ✔ |
| Erstes Ausspiel (§6.1) | ✔ **immer** | – |
| Prämien (§5) | ✔ | ✔ — Prämien hängen **nicht** an der Aufnehmerrolle |

Das ist der große Unterschied zu Königrufen: Prämien sind hier **keine Ansagen einer Partei**, sondern
Leistungen, die **jeder** Spieler für sich erzielen kann – auch der Nicht‑Aufnehmer, auch im einfachen Spiel.

---

## 4. Die Strohmänner (entspricht dem Talon anderer Tarockspiele)

Dies ist das Herzstück des Spiels und der Teil, der am häufigsten falsch wiedergegeben wird. Die folgende
Darstellung folgt [MS‑WEB] (ausführlichste Fassung), abgeglichen mit [DEWIKI]/[BECK], [TIROL], [ENWIKI] und
[FURR]; Abweichungen sind markiert.

### 4.1 Zustand eines Päckchens

Ein Strohmann ist ein **Stapel** (LIFO von oben) mit 0–4 Karten und einem Flag:

```cpp
struct Strawman {
    std::vector<Card> cards;   // cards[0] = oberste Karte
    bool faceUp = false;       // true ⇒ cards[0] ist Deckblatt (öffentlich, spielbar)
};
```

Invarianten (im Server zu assertieren):

* `faceUp == true` ⟹ `cards.size() >= 2` und `cards[0]` ist **weder Tarock noch König**.
* `cards.size() == 1` kommt **nie** in einem stabilen Zustand vor – die letzte Karte wandert sofort auf die
  Hand (§4.5).
* `cards.empty()` ⟹ das Päckchen ist aufgebraucht und verschwindet aus der UI.

### 4.2 Erstaufdeckung (vor dem ersten Stich)

Nach der Erklärung (§3.2) decken **beide** Spieler ihre Strohmänner auf, **jeder für sich, von links nach
rechts** [DEWIKI], [ENWIKI], [MS‑WEB]:

```
für jedes Päckchen p von links nach rechts:
    schleife:
        wenn p leer:                       weiter mit nächstem Päckchen
        wenn p genau 1 Karte enthält:      Karte VERDECKT auf die Hand (§4.5); Päckchen weg; weiter
        c := oberste Karte von p
        c offen umdrehen, der Gegner muss sie sehen können
        wenn c Tarock oder König:          c auf die Hand nehmen (Zustand HAND_KNOWN); schleife wiederholen
        sonst:                             p.faceUp := true (c bleibt als Deckblatt liegen); weiter
```

Wörtlich [MS‑WEB]: *„Ist diese Karte ein Tarock oder ein König, so nimmt er sie ins Blatt auf. Handelt es
sich um eine Farbkarte, die niedriger als ein König ist, so bleibt diese Karte offen als Deckblatt am
jeweiligen Strohmann liegen. Jeder Spieler kann in einer Folge so lange Karten aufnehmen, bis er auf eine
Farbkarte stößt, die niedriger als ein König ist. Genauso verfährt er mit den zwei anderen Strohmännern.“*

**Aufnahmekriterium exakt:** aufgenommen wird **jeder Tarock** (also auch `II`, auch der `Pagat`) und
**jeder König**. Liegen bleibt **jede Farbkarte niedriger als König**, also `D`, `C`, `B` und alle
Zahlkarten. **Damen bleiben liegen** – das ist spieltaktisch zentral (§10.3).

**Reihenfolge zwischen den Spielern:** Beide Prozeduren sind unabhängig. Am Tisch macht man es nacheinander;
in der App laufen sie **gleichzeitig animiert**, weil sie sich nicht beeinflussen. Für das LAN‑Protokoll:
ein einziges `StrawmenRevealed`‑Event mit beiden Ergebnislisten.

**⚙ Flag `open_cards_taken_immediately`** (nur kosmetisch, kein Regelunterschied):
`true` (Default, [MS‑WEB]) – Tarock/Könige wandern sofort ins Blatt, nachdem der Gegner sie gesehen hat;
`false` ([DEWIKI]/[BECK]) – sie werden **offen zur Seite gelegt** und erst nach Abschluss aller drei
Päckchen ins Blatt genommen. Das Ergebnis ist identisch; [MS‑WEB] empfiehlt `false` sogar als
Tischpraxis („Es empfiehlt sich daher, die Karten zunächst offen zur Seite zu legen, damit sie der
Gegenspieler in Ruhe betrachten kann“). **Die App zeigt sie 2 Sekunden groß an und legt sie dann ins Blatt.**

### 4.3 Nachdecken während des Spiels

> „Wenn ein Spieler das Deckblatt durch Ausspielen, Stechen oder Zugeben benutzt hat, muss er **sofort nach
> Umlegen des betreffenden Stiches** die darunter gelegene Karte des Päckchens für den Gegner sichtbar
> aufdecken und sie, falls es ein Tarock oder ein König ist, in sein Blatt aufnehmen.“ [MS‑WEB]

Daraus folgen zwei harte Implementierungsregeln:

1. **Zeitpunkt.** Nachgedeckt wird **erst, nachdem der Stich vollständig ist und eingezogen wurde** – nicht
   sofort beim Legen der Karte. Sonst bekäme der Gegner vor seiner eigenen Antwortkarte eine Information,
   die ihm nicht zusteht. Die Engine hat dafür eine eigene Phase `TRICK_RESOLVED → REVEAL_PENDING`.
2. **Prozedur.** Identisch mit §4.2 (dieselbe Schleife auf genau diesem Päckchen).

Reihenfolge, wenn **beide** Spieler in demselben Stich ein Deckblatt verbraucht haben: erst der
Stichgewinner, dann der Verlierer (deterministisch, damit LAN und Replay bitgleich sind). Regeltechnisch ist
die Reihenfolge irrelevant, weil die Päckchen unabhängig sind.

### 4.4 Die offen liegenden Deckblätter gehören zum Blatt

> „Mit den offen auf den Strohmännern aufliegenden Karten kann der Spieler ebenso verfahren wie mit den
> Karten in seiner Hand. Er kann sie ausspielen, kann mit ihnen stechen oder sie zugeben.“ [MS‑WEB]

Die Menge der spielbaren Karten ist also

```
spielbar(Spieler) = Handkarten ∪ { Deckblatt(p) : p ∈ Strohmänner, p.faceUp }
```

und **auf dieser Vereinigung** werden Farbzwang und Tarockzwang ausgewertet (§6.2). Die Karten **unter** dem
Deckblatt zählen nicht mit – wer die Farbe nur verdeckt im Päckchen hat, ist im Sinne der Regel „blank“.

### 4.5 Die letzte Karte eines Päckchens

> „Die letzte Karte eines Strohmannes kann **sofort aufgenommen** werden, auch wenn sie sich nicht für die
> Aufnahme ins Handblatt eignen würde.“ [MS‑WEB]; „Die letzte Karte eines Pakets wird **sofort aufgenommen**,
> auch wenn es sich um eine Farbkarte handelt, die niedriger als ein König ist.“ [MS‑WZ] — „Die letzte Karte
> des jeweiligen Stapels wird **nicht aufgedeckt**. Man darf sie, **ohne sie zu zeigen**, ins Blatt
> aufnehmen.“ [TIROL] — „Die letzte Karte eines jeden Päckchens wird ungeachtet ihres Wertes **ohne
> Aufdecken** ins Blatt des Spielers genommen.“ [DEWIKI]/[BECK]

Also: Sobald ein Päckchen nur noch **eine** Karte enthält, geht diese Karte **verdeckt** (Zustand
`HAND_SECRET_LAST`) auf die Hand – unabhängig davon, was sie ist. Das Päckchen ist damit verbraucht.

**Quellenlage genau:** Dass die letzte Karte **nicht gezeigt** wird, sagen ausdrücklich nur [TIROL] und
[DEWIKI]/[BECK]. [MS‑WEB], [MS‑EN] und [MS‑WZ] regeln an dieser Stelle **nur** die Aufnahmebedingung („auch
wenn sie sich nicht eignen würde“) und **schweigen** zum Vorzeigen; ihre allgemeine Prozedur lautet sonst
„für den Gegner sichtbar aufdecken **und** ggf. aufnehmen“, sodass sich die Gegenlesart („wird wie jede
andere Karte gezeigt und dann trotzdem aufgenommen“) aus ihrem Wortlaut nicht ausschließen lässt.
**⚙ Flag `last_card_is_secret`, Default `true`** ([TIROL], [DEWIKI]/[BECK]). Bei `false` ist die Karte
`HAND_KNOWN` statt `HAND_SECRET_LAST`; die ganze Informationsasymmetrie dieses Spiels (§10.1, §11.5 Fehler 10)
hängt an diesem Flag.

Das ist die einzige Stelle, an der ein Spieler Information gewinnt, die der Gegner **nicht** hat, und sie ist
der Grund, warum Kartenzählen in Strohmandeln so wichtig ist: Der Gegner weiß nur, **dass** eine Karte auf
die Hand ging, nicht **welche**.

**⚙ Flag `last_card_stays_faceDown_on_table`** ([MS‑WEB] nennt sie ausdrücklich als abweichende Regel):
`false` (Default) = wie oben; `true` = die letzte Karte **bleibt verdeckt auf dem Tisch liegen**, bis sie
gespielt wird. [MS‑WEB]: *„Für den Spielverlauf ist dies ohne Bedeutung, es fordert lediglich das
Erinnerungsvermögen der Spieler heraus.“*

**Implementierungsfolge, präzise:** Die Aussage „ohne Bedeutung für den Spielverlauf“ ist **nur dann**
richtig, wenn die verdeckt liegende Tischkarte für Farb‑ und Tarockzwang wie eine **Handkarte** behandelt
wird – sonst hätte die Variante andere legale Züge und wäre eben nicht äquivalent. Die Engine implementiert
`true` deshalb genau so: die Karte wandert regeltechnisch ins Blatt (Menge `spielbar`, §4.4) und liegt nur
in der UI weiter auf dem Tisch. Damit ist die Variante **rein kosmetisch**, wie [MS‑WEB] es behauptet.
**Empfehlung: `false`** (Default), die App sortiert die Karte ins Blatt ein und markiert sie kurz.

### 4.6 Es gibt **kein Drücken / keinen Skart**

In Strohmandeln wird **nichts abgelegt**. Es gibt keinen Talon, aus dem man tauscht, folglich auch keine
Verbote „keine Könige, keine Trullstücke, Tarock nur offen“, wie sie aus Königrufen und Tapp‑Tarock bekannt
sind. Alle 54 Karten kommen in Stiche.

Diese Aussage ist eine **Negativfeststellung** und in keiner Quelle ausdrücklich formuliert – sie ergibt sich
zwingend daraus, dass alle 54 Karten ausgeteilt werden und keine Quelle ein Ablegen erwähnt. Die App muss
daher an dieser Stelle **keine** Ablage‑Validierung implementieren (im Gegensatz zu allen anderen
Tarock‑Profilen der Engine).

### 4.7 Grenzfälle

| Fall | Behandlung | Quelle |
|---|---|---|
| Alle 4 Karten eines Päckchens sind Tarock/König | Karten 1–3 werden offen genommen, Karte 4 verdeckt; das Päckchen ist **schon vor dem ersten Stich weg** | [MS‑WEB], [ENWIKI]/[FURR] („until either the Straw Men are gone (which is rare but can happen)“) |
| Ein Spieler nimmt gleich ein oder zwei ganze Päckchen auf, der andere keine einzige Karte | ausdrücklich vorgesehen und normal | [MS‑WEB]: „Es kann vorkommen, dass auf diese Weise ein oder zwei Päckchen auf einmal in die Handkarten des einen Spielers wandern …“ |
| Ein Spieler hat **alle Handkarten verausgabt** und spielt nur noch von den Strohmännern | erlaubt und regelkonform – aber taktisch die schlimmste Lage | [MS‑WEB]: „Es kann übrigens vorkommen, dass ein Spieler alle Karten aus der Hand verausgabt hat und dann nur mehr von den Strohmännern zu Ende spielt bzw. spielen muss.“ |
| Ein Spieler hat **keine Strohmänner mehr**, der andere schon | normal; der eine spielt mit offenem, der andere mit teilweise verdecktem Blatt | – |
| Versehentlich **zwei** Karten gleichzeitig aufgedeckt, und die **erste** ist kein König und kein Trullstück | Die **zweite** Karte dem Gegner zeigen und sie dann **wieder verdeckt** an ihren Platz zurücklegen; die erste bleibt als Deckblatt liegen bzw. wird nach §4.2 behandelt | [MS‑WEB §Renonceregeln] |

Die App kann den letzten Fall nicht auslösen; er ist nur für den Papier‑/Turniermodus dokumentiert.

**Zwei Genauigkeitshinweise zum letzten Fall** (beide beim Nachprüfen der Quelle aufgefallen):

1. **Nur die zweite Karte geht zurück.** [MS‑WEB] im Original: *„so muss er auch **diese zweite Karte** dem
   Gegenspieler zeigen und **sie** dann wieder verdeckt auf den Strohmann zurücklegen“* – Singular, also
   ausschließlich die zweite Karte. Die englische Übersetzung [MS‑EN] gibt das mit *„he must show his
   opponent **these two cards** and lay **them** back face-down“* im Plural wieder; das ist ein
   Übersetzungsfehler und ergäbe zudem eine andere Regel (die erste Karte müsste dann erneut aufgedeckt
   werden). **Maßgeblich ist das deutsche Original.**
2. **Die Bedingung ist im Original zu eng formuliert.** [MS‑WEB] schreibt „kein König und kein **Trullstück**“.
   Nach §4.2 wird aber **jeder** Tarock aufgenommen, nicht nur `Sküs`/`XXI`/`I`. Der Sinn der Bedingung ist:
   *Die erste Karte darf keine Karte sein, die ohnehin aufgenommen wird* – sonst wäre die zweite Karte
   regulär das neue Deckblatt und müsste gar nicht zurück. Die Engine prüft daher
   `!isTarock(c1) && !isKing(c1)` und **nicht** die wörtliche Trullstück‑Bedingung. Das ist eine
   **Auslegung dieser Spezifikation**, keine Quellenangabe.

### 4.8 Referenz‑Pseudocode

```cpp
// Gibt die Karten zurück, die in dieser Aufdeckfolge OFFEN gezeigt wurden.
std::vector<Card> revealPile(Player& pl, Strawman& p) {
    std::vector<Card> shown;
    for (;;) {
        if (p.cards.empty()) return shown;
        if (p.cards.size() == 1) {                 // §4.5 letzte Karte: verdeckt auf die Hand
            pl.hand.push_back(take(p));            // Sichtbarkeit: HAND_SECRET_LAST
            return shown;
        }
        const Card& c = p.cards.front();
        if (isTarock(c) || isKing(c)) {            // §4.2 Aufnahmekriterium
            shown.push_back(c);
            pl.hand.push_back(take(p));            // Sichtbarkeit: HAND_KNOWN
            continue;
        }
        p.faceUp = true;                           // Deckblatt
        shown.push_back(c);
        return shown;
    }
}

void initialReveal(Player& pl) {                   // §4.2 links → rechts
    for (Strawman& p : pl.piles) revealPile(pl, p);
}

void afterTrickResolved(Player& winner, Player& loser) {   // §4.3
    for (Player* pl : { &winner, &loser })
        for (Strawman& p : pl->piles)
            if (!p.cards.empty() && !p.faceUp) revealPile(*pl, p);
}
```

---

## 5. Ansagen und Prämien

### 5.1 Grundsatz: In Strohmandeln wird (fast) alles **still** gespielt

[MS‑WEB] und [MS‑WZ] begründen das ausführlich und identisch:

> „Ein **angesagter Pagat ist nicht vorgesehen**, weil der Erfolg oder Misserfolg in vielen Fällen davon
> abhinge, ob in den letzten Strohmänner‑Karten des Gegners Tarock liegen. Schon ein stiller Pagat ultimo
> ist schwer genug.“ [MS‑WEB]
>
> „Eine Bewertung nach dem Muster des Königrufens ist deswegen nicht sinnvoll, weil sich im Endspiel oft
> einige Tarock verdeckt in den Strohmännern befinden und ein Austarockieren daher nur bedingt gelingen
> kann. Auch Trull in den Stichen und Vier Könige in den Stichen können nur still gespielt werden.“ [MS‑WZ]

Im **Default‑Profil `AT-STROH-MS-ERW` gibt es daher überhaupt keine Ansagen** – alle Prämien sind still
(„sang‑ und klanglos“). Das vereinfacht UI, KI und Netzwerkprotokoll erheblich.

Ausnahmen in anderen Profilen:

* `AT-STROH-MS-KLASS`: **Trull im Blatt** und **vier Könige im Blatt** sind **ansagbar** (§5.2).
* `AT-STROH-TIROL-2025`: **Pagat ultimo, vier Könige, Trull** sind jeweils **still 1 / angesagt 2** (§5.2.3).
* Der **Rostopschin** (§5.5) ist eine Sonderform: er wird **während** des Spiels durch zwei Rufworte
  „konstituiert“ und kann nicht vorher angesagt werden.

### 5.2 Trull und vier Könige (Königstrull)

Es gibt **zwei grundverschiedene Auffassungen**, was prämiert wird. Beide sind belegt.

#### 5.2.1 Variante A – „im Blatt, angesagt“ (klassisch)

> „Zusätzlich können **Trull im Blatt** und **vier Könige im Blatt** angesagt werden, dies ist jedoch nicht
> verpflichtend. **Ob sich diese Karten am Ende auch in den Stichen des Spielers befinden, ist für den
> Gewinn dieser Prämienansage nicht von Bedeutung.** Die Ansagen müssen spätestens dann erfolgen, wenn die
> erste Karte der Trull oder der erste der vier Könige gespielt wird.“ [MS‑WEB], gleichlautend
> [DEWIKI]/[BECK]: „kann er beim Spielen der ersten Karte derselben diese ansagen“.

* Prämiert wird der **Besitz**, nicht der Heimbringen‑Erfolg. Die Prämie kann also **nicht verloren gehen**.
* **Was ist „im Blatt“?** Handkarten **und** offen aufliegende Deckblätter (§4.4) – aber nicht die verdeckten
  Karten unter den Deckblättern. Diese Präzisierung findet sich in **keiner** Quelle ausdrücklich; sie folgt
  aus dem Grundsatz, dass die Deckblätter zum Blatt gehören. **⚙ Flag `bonus_hand_includes_faceup`,
  Default `true`.** → §9.7, offene Frage.
* **Ansagezeitpunkt / Verfallsklausel:** spätestens beim Spielen der **ersten** dazugehörigen Karte. Wer eine
  Trullkarte oder einen König gespielt hat, ohne anzusagen, hat die Prämie **verwirkt**.
* **Taktik daraus (sehr wichtig fürs Lernen):** *„Für Spieler, die zu Beginn drei Könige im Blatt haben, kann
  es daher ratsam sein, diese zunächst zurückzuhalten, denn es könnte ja der vierte König im Verlauf des
  Spiels in einem der Strohmänner auftauchen. Gleiches gilt für jene Fälle, wo nur eine Karte auf die Trull
  fehlt.“* [MS‑WEB] — Das ist die einzige echte Ansage‑Taktik des Spiels.

#### 5.2.2 Variante B – „in den Stichen, still“ (erweitert, **Default**)

> „Da die Ansage von vier Königen im Blatt bzw. der Trull im Blatt in den heutigen Spielformen für 4 Spieler
> immer seltener werden, hat es sich eingebürgert, stattdessen auch im Strohmann‑Tarock eine Prämie für
> **still gespielte vier Könige in den Stichen** und für die **still gespielte Trull in den Stichen**
> vorzusehen.“ [MS‑WEB], ebenso [MS‑WZ], [ENWIKI] (Spalten Furr und Mayr&Sedlaczek), [TIROL].

* Prämiert wird, wer am Ende **alle drei Trullstücke** bzw. **alle vier Könige** in **seinen Stichen** hat.
* Keine Ansage, keine Verfallsklausel, keine Timing‑Regel – reine Auswertung nach dem letzten Stich.
* [TIROL] formuliert es wörtlich so: *„Trull … ein Spieler hat Mond, Sküs und Pagat in den Stichen“*,
  *„Vier Könige … ein Spieler hat alle 4 Könige in den Stichen“*.

#### 5.2.3 Variante C – Tirol: still **oder** angesagt

[TIROL] kombiniert beides: prämiert wird **in den Stichen**, aber man darf es **ansagen** und verdoppelt
damit den Wert:

| Zusatz | still | angesagt |
|---|---|---|
| Pagat ultimo | 1 | 2 |
| Vier Könige | 1 | 2 |
| Trull | 1 | 2 |

Der Ansagezeitpunkt ist in [TIROL] **nicht** geregelt. **Empfehlung für das Profil
`AT-STROH-TIROL-2025`:** Analog zu Königrufen – Ansage **vor dem ersten Ausspiel** (also unmittelbar nach
der Erstaufdeckung der Strohmänner), danach nicht mehr. Eine misslungene Ansage kostet den angesagten Wert,
und zwar **an den Gegner**. Das ist eine **begründete Ergänzung, keine Quellenangabe** → §9.11.

### 5.3 Pagat ultimo und abgefangener Pagat

> „Es ist möglich, den Tarock I ultimo, d. h. **als letzte Karte** zu spielen. Dies bringt (je nachdem ob er
> überstochen wird oder nicht) den Gewinn oder Verlust des einfachen Spielwertes.“ [DEWIKI]
>
> „**Pagat ultimo**, der nur still gespielt werden kann, und ein **im letzten Stich abgefangener Pagat**
> kosten ebenfalls 1 Spielpunkt.“ [MS‑WEB]
>
> „it is normal for bonuses for playing the ‚birds‘ to their respective tricks to go to the opponent if they
> fail to win. For example, if the Pagat is played to the last trick and lost, the winner of the trick gets
> the *Pagat ultimo* bonus.“ [ENWIKI]

**Exakte Definition (Default):**

| Ereignis im **27. (letzten) Stich** | Prämie |
|---|---|
| Ein Spieler **spielt** den Pagat und **gewinnt** den Stich | +1 an ihn (*Pagat ultimo*) |
| Ein Spieler **spielt** den Pagat und **verliert** den Stich | +1 an den **Gegner** (*abgefangener Pagat / Pagat gefangen*) |
| Der Pagat ist im letzten Stich **nicht dabei** | keine Prämie |

* „Spielt“ umfasst **ausspielen und zugeben** – es kommt nicht darauf an, wer eröffnet hat.
* Der Pagat ist der **niedrigste** Tarock. Er gewinnt den letzten Stich also nur, wenn der Gegner
  **überhaupt keinen Tarock** mehr zulegen kann – d. h. keinen auf der Hand **und** keinen als Deckblatt.
  Genau deshalb ist der Pagat ultimo in Strohmandeln so schwierig (§10.6).
* **Kein Pagat‑Ultimo‑Zwang:** Wer den Pagat im letzten Stich hält, *muss* ihn spielen (er hat ja nur noch
  eine Karte), aber niemand ist verpflichtet, darauf hinzuspielen.
* **⚙ Flag `pagat_capture_bonus`** (Default `true`, [MS‑WEB]). In `TIROL-2025` und `BECK-1972` ist nur der
  gelungene Pagat ultimo prämiert; ein abgefangener Pagat bringt dem Fänger dort **nichts** (belegt für
  [TIROL]: „Pagat ultimo … Tarock I macht den letzten Stich“, keine Fang‑Zeile). [DEWIKI]/[BECK] spricht
  hingegen von „Gewinn **oder Verlust**“ – also doch symmetrisch. Widerspruch → §9.5.

### 5.4 Die „Vögel“: Uhu, Kakadu, Quapil

| Name | Karte | Stich | Wert (still) | Belegt in |
|---|---|---|---|---|
| **Pagat ultimo** | `I` | 27. (letzter) | 1 | alle Quellen |
| **Uhu** | `II` | 26. (vorletzter) | 1 | [MS‑WEB], [MS‑WZ], [ENWIKI] (Furr, M&S), [FURR: „An Owl“] |
| **Kakadu** | `III` | 25. (drittletzter) | 1 | [MS‑WEB], [MS‑WZ], [ENWIKI], [FURR: „A Cockatoo“] |
| **Quapil** | `IIII` | 24. (viertletzter) | 1 | **nur** [MS‑WZ] und [ENWIKI] (Spalte M&S) |

> „Wer gewohnt ist, beim Königrufen mit **Uhu** und **Kakadu** zu spielen, wird Freude damit haben, wenn er
> diese beiden Ansagen auch in das Regelwerk von Strohmandeln aufnimmt. Diesfalls ist zu empfehlen, dass ein
> stiller Uhu und ein stiller Kakadu je 1 Spielpunkt zählen – also genausoviel wie ein stiller Pagat.“
> [MS‑WEB]
>
> „Wird mit **Uhu, Kakadu und ev. auch Quapil** gespielt, so zählen diese genauso wie der Pagat ultimo
> jeweils 10 Cent.“ [MS‑WZ]

* Für Uhu/Kakadu/Quapil gilt die **gleiche Spiegelregel** wie beim Pagat: wird der Vogel im „seinem“ Stich
  gespielt und verloren, geht die Prämie an den Gegner [ENWIKI]. **⚙ Flag `bird_capture_bonus`,
  Default `true`.**
* [TIROL] schließt sie ausdrücklich aus: **„keine Säcke, keine Vögel, kein Kontra“**.
* [BECK]/[DEWIKI] kennt sie nur als Nebenbemerkung: *„Es gibt auch die Spielweise, dass der als vorletzte
  Karte gespielte Tarock II in gleicher Weise gewertet wird.“*
* **⚙ Flags:** `bonus_uhu` (Default `true`), `bonus_kakadu` (Default `true`), `bonus_quapil`
  (Default **`false`** – nur [MS‑WZ]; die Wahrscheinlichkeit, `IIII` bis zum 24. Stich zu halten und dann zu
  gewinnen, ist so gering, dass sie die Endspiel‑KI unnötig verkompliziert).

**Reihenfolge‑Regel („höherer Vogel zuerst“), die es hier *nicht* gibt.** In Königrufen gibt es Regeln
darüber, welcher angesagte Vogel bei Kollisionen Vorrang hat. Da in Strohmandeln **alle** Vögel still sind
und ein Spieler theoretisch Pagat, Uhu und Kakadu in den drei letzten Stichen gewinnen kann, werden sie
schlicht **einzeln und unabhängig** ausgewertet und addiert (kein Vorrang, keine Verdrängung). Keine Quelle
sagt etwas anderes.

### 5.5 Rostopschin

Ein Kuriosum mit Wiener Zwischenkriegs‑Provenienz (beschrieben in Friedrich Torbergs *Die Tante Jolesch*)
[MS‑WEB].

**Definition:** Ein Spieler macht **zwei aufeinanderfolgende Stiche** mit **Tarock XVII** und **Tarock
XVIII** und ruft dabei beim Spielen der `XVII` **„Ross!“** und beim Spielen der `XVIII` **„Topschin!“**.

*Schreibweise des Rufworts:* [MS‑WEB] und [MS‑EN] schreiben **„Ross!“**, [FURR] und [ENWIKI] **„Ros“**. Der
Name geht auf den russischen Grafen **Rostoptschin** zurück; beide Schreibungen sind belegt. Die App zeigt
im Profil `MS-ERW` „Ross!“, im Profil `FURR-2009` „Ros!“ – **⚙ Flag `rostopschin_call_spelling`.**

Präzisierungen [MS‑WEB]:

* Die Rufworte sind **konstitutiv**: *„Wer Tarock XVII und Tarock XVIII hintereinander spielt, **ohne** Ross!
  und Topschin! zu sagen, hat diese Prämie nicht gespielt. Er kann sie also auch nicht verlieren, wenn eine
  der beiden Karten (oder beide Karten) gestochen werden.“*
* Der **erste** der beiden Stiche kann auch dadurch zustande kommen, dass man mit der `XVII` **sticht**
  (also nicht ausspielt) und dazu „Ross!“ sagt; anschließend spielt man mit „Topschin!“ die `XVIII` aus und
  gewinnt auch diesen Stich.
* **Beide** Stiche müssen gewonnen werden, und zwar **unmittelbar hintereinander**.
* Wert: **1 Spielpunkt**. Ein **angesagter** Rostopschin zu Spielbeginn ist ausdrücklich **nicht** vorgesehen
  [MS‑WEB].
* Der Rostopschin ist damit die einzige **einseitig risikofreie** Prämie des Spiels.

**⚙ Flag `bonus_rostopschin`** (Default `true` im Profil `MS-ERW`; `false` in allen anderen).
**⚙ Flag `rostopschin_cards`** = `{XVII, XVIII}` (Default). → **Achtung, Quellenfehler:** [ENWIKI] gibt in
der Scoring‑Tabelle „VII and VIII“ an und beruft sich dabei auf [FURR]; [FURR] schreibt im Volltext aber
**„the XVII and the XVIII of trumps“**, und [MS‑WEB] ebenso. **Richtig ist XVII/XVIII** (§9.9).

**UI:** Zwei Ruf‑Buttons „Ross!“ (nur aktiv, wenn `XVII` legal spielbar ist) und „Topschin!“ (nur aktiv nach
erfolgreichem „Ross!“‑Stich und wenn `XVIII` legal spielbar ist). Der Computerspieler ruft nur, wenn er die
beiden Stiche für sicher hält (§10.7).

### 5.6 Grand Point / Grammel‑Punkt

> „Eine interessante Bereicherung des Strohmandelns ist der **Grand Point (Grammel‑Punkt)**. Diese Regel
> bedeutet, dass ein Spieler, der **mindestens 45 Punkte** in seinen Stichen erzielt, eine Prämie von **1
> Spielpunkt** erhält. Diesfalls hat sein Gegner 25 Punkte oder weniger. **Hat der Gegner mindestens 26
> Punkte, so muss er keinen Grammel‑Punkt zahlen.“** [MS‑WEB]

* **Die Schwelle ist in *ganzen* (gerundeten) Punkten zu prüfen: `pointsRounded(p3) >= 45`, gleichwertig
  `p3 >= 134`** (= exakt `44 ⅔` oder mehr). Gegenprobe: Gegner `p3 <= 76`, gerundet ≤ 25.
  **Begründung (korrigiert gegenüber einer früheren Fassung dieser Spezifikation, die `p3 >= 135` verlangte):**
  [MS‑WEB] stellt zwei Sätze nebeneinander, die nur in **ganzzahliger** Arithmetik einander lückenlos
  ergänzen – „Diesfalls hat sein Gegner **25 Punkte oder weniger**“ und „Hat der Gegner **mindestens 26**
  Punkte, so muss er keinen Grammel‑Punkt zahlen“. Zwischen „25 oder weniger“ und „mindestens 26“ ist kein
  Platz für „25 und 1 Blatt“; M&S rechnen an dieser Stelle also in Dreierlagen mit ganzen Punkten (§1.5.2a).
  Genau dann gilt die Äquivalenz exakt:

  | `p3` eigen | gerundet eigen | `p3` Gegner | gerundet Gegner | Grammel‑Punkt |
  |---|---|---|---|---|
  | ≥ 135 | ≥ 45 | ≤ 75 | ≤ 25 | ja |
  | **134** | **45** | **76** | **25** | **ja** (dieser Fall entscheidet die Frage) |
  | 133 | 44 | 77 | 26 | nein |

  Der Streitfall ist also genau ein einziger: `44 ⅔ : 25 ⅓`. Er wird am Tisch als **45 : 25** gezählt und
  ist damit ein Grammel‑Punkt. **⚙ Flag `grand_point_uses_rounded_points`, Default `true`.** Wer strikt
  exakt rechnen will (`p3 >= 135`, also „45 volle Punkte und 0 Blatt“), setzt es auf `false`. Der
  Unterschied tritt nur bei **genau einem** der 211 möglichen `p3`‑Werte auf; wie oft das in der Praxis
  vorkommt, ist nicht erhoben.
* Der Grammel‑Punkt gilt für **jeden** Spieler, auch für den Nicht‑Aufnehmer, auch im einfachen Spiel.
* In der Tabelle von [MS‑WEB] ist er nicht als eigene Prämienzeile geführt, sondern als **erhöhter
  Spielwert**: einfaches Spiel 1 → 2, aufgenommenes Spiel gewonnen 3 → 4, verloren 4 → 5. Beides ist
  arithmetisch dasselbe (+1); die Engine implementiert ihn als **Prämie**, weil das die Kontra‑Verdopplung
  (§5.10) sauber hält.
* [FURR] hat eine **ähnliche, aber nicht identische** Regel: **„With 45: If Declarer takes at least 45 card
  points. Scores 4“** und **„With 26: If Declarer takes 26 card points or less: Scores 5 for the defender“**.
  **Grenzfalldifferenz:** Furr sagt „26 **or less**“ (Gegner ≥ **44**), M&S sagen „unter 26“ bzw. „25 oder
  weniger“ (Gegner ≥ **45**). Zweitens ist Furrs Regel **an die Aufnehmerrolle gebunden** und kennt keinen
  Großgewinn im einfachen Spiel, während der Grammel‑Punkt bei M&S rollenunabhängig ist (§3.3). → §9.6.
  **Default: M&S‑Lesart.**

**⚙ Flag `bonus_grand_point`** (Default `true`), **⚙ Flag `grand_point_threshold`** (Default `45`),
**⚙ Flag `grand_point_defender_threshold`** (Default = `grand_point_threshold`; im Profil `FURR-2009` `44`),
**⚙ Flag `grand_point_role_bound`** (Default `false`; im Profil `FURR-2009` `true`).

### 5.7 Valat

* **Valat** = **alle 27 Stiche** eines Spiels.
* Wert: **12 Spielpunkte**, **still** [MS‑WEB], [MS‑WZ] („Ein stiller Valat wird mit 120 Cent bewertet“),
  [ENWIKI] (Furr: „Slam … 12“), [FURR].
* Ein **angesagter** Valat existiert nicht.
* Der Valat **ersetzt** die Spielwertung **nicht**, sondern tritt hinzu: Wer Valat macht, hat zwangsläufig
  70:0 und damit auch das Spiel und alle anderen Prämien gewonnen. **⚙ Flag `valat_replaces_game`,
  Default `false`** (keine Quelle sagt etwas Gegenteiliges; additive Auswertung ist die einfachere und in
  [MS‑WEB]s Tabelle als eigene Zeile geführte Lösung).
* [MS‑WEB]: „kommt aber äußerst selten vor“. Für die App relevant: Der Valat‑Check ist trivial
  (`tricksWon == 27`), muss aber vorhanden sein, weil Anfänger gegen die Computerstufe „leicht“ durchaus
  einen Valat schaffen können.

### 5.8 Mondfang — im Default **keine Prämie**

> „Ein **Mondfang** wird **nicht gesondert prämiert**, ist aber oft spielentscheidend, weil es um einen
> Fünferzähler geht.“ [MS‑WEB]

Dagegen:

* [DEWIKI]/[BECK]: *„2 Werteinheiten: für den gefangenen Mond, für den Spieler, der den Mond mit Sküs
  übersticht“*
* [ENWIKI], „Further scoring options“: *„1 game point: for a Mondfang, i.e. for the player who captures the
  Mond with the Sküs“*

**⚙ Flag `bonus_mondfang`** (Default **`false`** = M&S; im Profil `BECK-1972` `true` mit Wert 1).
**Empfehlung: `false`.** M&S' Argument ist überzeugend – der Mond ist ein Fünferzähler, sein Verlust ist
schon Strafe genug, und eine Zusatzprämie macht das Endspiel um den Sküs unangenehm deterministisch.
Begriffsklärung: **Mondfang** = `XXI` wird vom `Sküs` gestochen. Das ist die **einzige** Möglichkeit, den
Mond zu fangen.

### 5.9 Trischaken (nur Profil `AT-STROH-TIROL-2025`)

[TIROL] sieht vor: **Passen beide Spieler, wird getrischakt.**

| Eigenschaft | Regel | Quelle |
|---|---|---|
| Ziel | **möglichst wenige** Punkte machen | [TIROL] |
| Wert | 2 Spielpunkte an den Spieler mit weniger Punkten | [TIROL] |
| Spielzwang | **mit Stichzwang**: wer überstechen kann, muss überstechen (§5.9.2) | [TIROL]; Definition aus den Königrufen‑Regelwerken |
| Ausspielfreiheit | „bunt gemischt“ = **keine Austarockier‑Pflicht** (§5.9.1) | [TIROL]; Bedeutung aus den Königrufen‑Regelwerken |
| Prämien | **keine** | Analogieschluss (Negativspiele kennen keine Prämien) |
| Bemerkung im Original | „möglichst wenig Punkte / **bunt gemischt**; mit Stichzwang“ | [TIROL] |

#### 5.9.1 „bunt gemischt“ — **geklärt**, kein offener Punkt mehr

„Bunt gemischt“ ist **kein** beschreibender Nebensatz, sondern ein **stehender österreichischer Fachausdruck
der Trischaken‑Regel**. Er bedeutet:

> „Es wird ‚bunt gemischt‘ gespielt, das heißt, **es braucht nicht austarockiert zu werden**, sondern jeder
> Ausspieler hat **die Wahl, ob er Tarock oder Farbe anspielt**.“

Belegt gleichlautend in mehreren Königrufen‑Regelwerken (u. a. Linzer Tarock Akademie, *Königrufen –
Grundkurs 1*, Kap. 11; Regelsammlung `tarock-konigrufen8.webnode.at/trischaken/`; ebenso in der
Schwesterspezifikation `koenigrufen.md` §6.4 aus [OÖ23 Trischaken 1]). Der Ausdruck grenzt Trischaken gegen
jene Negativ‑ und Farbenspiele ab, in denen eine **Austarockier‑Pflicht** oder ein Tarock‑Ausspielverbot
besteht.

**Folge für Strohmandeln:** In Strohmandeln gibt es ohnehin keine Austarockier‑Pflicht (§6.2), das Merkmal
ist hier also **inhaltsleer** und ändert nichts. Insbesondere bedeutet „bunt gemischt“ **nicht**, dass die
Strohmänner entfielen oder die 24 Restkarten anders verteilt würden – die frühere Lesart (b) dieser
Spezifikation war ein Missverständnis und ist gestrichen. Trischaken wird mit den **normalen Strohmännern**
gespielt.

#### 5.9.2 Stichzwang — präzise Definition

**Stichzwang heißt „überstechen, wenn möglich“, nicht „die höchste Karte spielen“.** Maßgebliche
Formulierung aus der Schwesterspezifikation `koenigrufen.md` §6.4 (dort belegt aus [OÖ23 Negativ 1] und
[LTA‑GK1 Kap. 8]): *„Man muss die höchste im Stich liegende Karte überstechen, sofern das unter Beachtung des
Farbzwangs möglich ist.“*

Im Zweierspiel vereinfacht sich das, weil bei der Zugabe **genau eine** Karte im Stich liegt:

```
Sei L die ausgespielte Karte, legal = Ergebnis der Kaskade aus §6.2 (Farbzwang → Tarockzwang → frei).
W := { c ∈ legal : c schlägt L nach §6.4 }
wenn W ≠ ∅  ⟹  legal := W          // überstechen ist Pflicht
sonst       ⟹  legal bleibt        // freie Wahl im Rahmen der Zwänge
```

Der **Ausspieler** ist frei; der Stichzwang trifft nur den Zugebenden. Innerhalb von `W` ist die Wahl frei –
man muss **nicht** die höchste Karte nehmen. (Die frühere Fassung dieser Spezifikation verlangte die
„höchstmögliche Karte“; das ist strenger als jede Quelle und war ein Fehler.)

#### 5.9.3 Pagat‑Regel im Trischaken — **⚙ Flag `trischaken_pagat_last_tarock`**

In **allen** Negativspielen der österreichischen Königrufen‑Regelwerke – Trischaken eingeschlossen – gilt:
*„Der Pagat darf erst als letztes Tarock gespielt oder zugegeben werden.“* Das steht auch in **Markus Mairs
eigenem** Königrufen‑Kursskript (`tarock.tirol`, Abschnitt „Die Negativspiele“: „Kein Talon, keine Zusätze
und Stichzwang. … Der Pagat darf erst als letztes Tarock gespielt werden.“), also beim **selben Autor**, von
dem das Strohmandeln‑Blatt [TIROL] stammt.

Das Strohmandeln‑Blatt [TIROL] selbst erwähnt die Regel **nicht**. Zwei Lesarten:

| Wert | Verhalten | Begründung |
|---|---|---|
| `true` | Der Pagat darf nur gespielt werden, wenn er der letzte Tarock im Blatt (Hand ∪ Deckblätter) ist; diese Regel **geht dem Stichzwang vor** | Systematik des Autors; in seinem Königrufen‑Regelwerk gilt sie für jedes Negativspiel — **empfohlen** |
| `false` | keine Pagat‑Beschränkung | [TIROL] nennt sie im Strohmandeln‑Blatt nicht; *argumentum e silentio* |

**Empfehlung: `true`**, aber **klar als Analogieschluss markiert** – das Strohmandeln‑Blatt selbst schweigt.
→ §9.10, weiterhin Rückfragepunkt.

#### 5.9.4 Was in [TIROL] wirklich steht, und was Ergänzung ist

| Aussage | Quelle |
|---|---|
| Passen beide, wird getrischakt | [TIROL] wörtlich |
| Wert 2 Spielpunkte | [TIROL] wörtlich |
| „möglichst wenig Punkte“ | [TIROL] wörtlich |
| „bunt gemischt“ | [TIROL] wörtlich; Bedeutung aus den Königrufen‑Regelwerken (§5.9.1) |
| „mit Stichzwang“ | [TIROL] wörtlich; Definition aus den Königrufen‑Regelwerken (§5.9.2) |
| Strohmänner werden wie sonst gelegt und aufgedeckt | **Ergänzung dieser Spezifikation** (konservative Lesart) |
| Pagat‑Regel | **Analogieschluss** aus dem Königrufen‑Regelwerk desselben Autors (§5.9.3) |
| Gleichstand (`p3` 105 : 105) → 0 : 0 | **Ergänzung dieser Spezifikation**; [TIROL] regelt den Fall nicht |
| Prämien im Trischaken | **keine** – Analogieschluss aus „in Negativspielen gibt es keine Prämien“ |

### 5.10 Kontra, Re, Sub — im Default **abgeschaltet**

> „Es ist **nicht zweckmäßig**, ein Kontra in den Regeln vorzusehen, es sei denn, man entscheidet sich, dass
> ein **Aufnehmer** kontriert werden kann, ansonsten aber kein Kontra möglich ist. **Im Kontra werden die
> Spielpunkte verdoppelt.** Da der Spielaufnehmer im Verlustfall ohnedies einen zusätzlichen Punkt verliert,
> entsteht bei Handhabung der Kontrierungsregel das Problem, dass der Spielaufnehmer **8 Punkte verlieren
> und nur 6 Punkte gewinnen** kann. Außerdem ist ein Kontra deshalb riskant, weil das Spiel durch Aufdecken
> der Strohmänner‑Karten oft eine überraschende Wendung nimmt.“ [MS‑WEB]

[TIROL] ebenso: **„kein Kontra“**. [MS‑WZ]: *„Es ist nicht zweckmäßig ein Kontra in den Regeln vorzusehen“*.

**⚙ Flag `kontra_enabled`, Default `false`.** Wird es aktiviert (`AT-STROH-MS-ERW+KONTRA`), gilt genau:

| Stufe | Ruf | Wer darf | Zeitpunkt | Wirkung |
|---|---|---|---|---|
| **Kontra** | „Kontra!“ | **nur der Gegner des Aufnehmers** | ab Ende der Erstaufdeckung bis **vor dem ersten Ausspiel** | Spielwert × 2 |
| **Re** | „Re!“ | nur der Aufnehmer | unmittelbar nach dem Kontra | × 4 |
| **Sub** | „Sub!“ | nur der Gegner | unmittelbar nach dem Re | × 8 |

* **Nur das Spiel wird verdoppelt, nicht die Prämien.** Das ist eine **Festlegung dieser Spezifikation**;
  [MS‑WEB] sagt nur „Im Kontra werden die Spielpunkte verdoppelt“, was den Spielwert meint (die Prämien
  heißen dort durchgehend „Prämien“, nicht „Spielpunkte“). → §9.8.
* Im **einfachen Spiel** gibt es **kein Kontra** (es gibt keinen Aufnehmer, den man kontrieren könnte).
* Der Zeitpunkt „vor dem ersten Ausspiel“ ist eine **Ergänzung dieser Spezifikation**; [MS‑WEB] nennt keinen.
  Begründung: Später wäre es wegen der laufenden Strohmann‑Aufdeckung willkürlich.

### 5.11 Gesamt‑Prämientabelle nach Profil

| Prämie | `MS-ERW` ⭐ | `MS-KLASS` | `MS-WZ2007` | `TIROL-2025` | `BECK-1972` | `FURR-2009` |
|---|---|---|---|---|---|---|
| Trull **im Blatt, angesagt** | – | 1 | – | – | 1 | – |
| Vier Könige **im Blatt, angesagt** | – | 1 | – | – | 1 | – |
| Trull **in den Stichen, still** | **1** | – | 1 | 1 (ang. 2) | – | – |
| Vier Könige **in den Stichen, still** | **1** | – | 1 | 1 (ang. 2) | – | – |
| Pagat ultimo, still | **1** | 1 | 1 | 1 (ang. 2) | 1 | 1 |
| Pagat im letzten Stich abgefangen | **1** | 1 | – | – | 1 (str.) | – |
| Uhu (`II`, vorletzter Stich) | **1** | – | 1 | – | – | 1 |
| Kakadu (`III`, drittletzter) | **1** | – | 1 | – | – | 1 |
| Quapil (`IIII`, viertletzter) | – | – | 1 | – | – | – |
| Rostopschin (`XVII`+`XVIII`, mit Rufworten) | **1** | – | – | – | – | 1 |
| Grammel‑Punkt / Großgewinn ≥ 45 eigene Punkte | **1** (jeder Spieler, auch im einfachen Spiel) | – | – | – | – | 1 (**nur der Aufnehmer**; Verteidiger‑Schwelle **44**) |
| Mondfang (`XXI` mit `Sküs` gestochen) | – | – | – | – | 1–2 | – |
| Valat (alle 27 Stiche), still | **12** | 12 | 12 | – | – | 12 |
| Kontra / Re / Sub | aus | aus | aus | aus | – | aus |

⭐ = Default. „–“ = im Profil nicht vorhanden. „(str.)“ = strittig, siehe §9.5.

**Zur Furr‑Spalte:** Sie folgt dem **Volltext** von [FURR] (S. 110 f.), nicht der Tabelle in [ENWIKI].
Insbesondere hat Furr **keine** Trull‑ und **keine** Königstrull‑Prämie – [ENWIKI] schreibt ihm dort
fälschlich eine zu (§9.14). Ebenso fehlen bei ihm Quapil und Mondfang, und sein Grammel‑Äquivalent ist an
die Aufnehmerrolle gebunden und beim Verteidiger um einen Punkt tiefer angesetzt (§9.6).

**Zur Beck‑Spalte:** Die Werte stammen aus [ENWIKI]; [DEWIKI] gibt für dieselbe Quelle (Beck 1972,
S. 40–45) durchgehend **2** statt 1 an (Trull, Pagat ultimo, Mondfang) und kennt keinen erhöhten Wert für
ein verlorenes aufgenommenes Spiel. Beck lag für diese Spezifikation **nicht im Volltext** vor; der
Widerspruch ist in §9.2 dokumentiert und bleibt offen.

### 5.12 Zeitpunkte auf einen Blick

| Zeitpunkt | Was hier möglich ist |
|---|---|
| nach dem Geben, vor der Erstaufdeckung | Zusammenwerfen ohne Tarock (§2.5); Erklärung „Ich nehme auf!“/„Ich passe!“ (§3.2) |
| nach der Erstaufdeckung, vor dem 1. Ausspiel | Ansagen im Profil `TIROL-2025` (§5.2.3); Kontra, falls aktiviert (§5.10) |
| beim Spielen der **ersten** Trullkarte / des **ersten** Königs | Ansage „Trull“ / „Vier Könige“ im Profil `MS-KLASS` (§5.2.1) |
| beim Spielen der `XVII` bzw. `XVIII` | Rufworte „Ross!“ / „Topschin!“ (§5.5) |
| nach dem letzten Stich | Auswertung **aller** stillen Prämien (§7.2) |

---

## 6. Spielregeln (die Spielweise)

### 6.1 Ausspiel und Stichfolge

* **Die Vorhand spielt zum ersten Stich aus – immer**, unabhängig davon, wer aufgenommen hat
  [MS‑WEB: „Die Vorhand spielt aus“], [DEWIKI: „Vorhand spielt immer zum ersten Stich aus“],
  [TIROL: „Die Vorhand beginnt mit dem ersten Ausspiel“].
* Danach spielt **der Gewinner des letzten Stiches** aus [MS‑WEB], [DEWIKI].
* Es werden **alle 27 Stiche** ausgespielt, bis alle Karten weg sind. Ein vorzeitiges Beenden gibt es
  nicht (Ausnahme: Renonce, §6.7).

**⚙ Flag `first_lead`:** `forehand` (Default) | `declarer` — Letzteres nur im Profil `FURR-2009`
([FURR]: *„If both players passed, then Dealer’s opponent leads to the first trick. Otherwise it is
Declarer.“*). Furr ist hier der einzige Beleg und steht gegen drei unabhängige deutschsprachige Quellen;
**Empfehlung: `forehand`.** → §9.3.

### 6.2 Legale Züge: Farbzwang und Tarockzwang, **kein** Stichzwang

Die maßgebliche Formulierung [MS‑WEB]:

> „Es gilt **Farbzwang**, wer die angespielte Farbe nicht bedienen kann, muss **Tarock** spielen. Ist auch
> das nicht möglich, kann er **jede beliebige** Farbkarte spielen.“

und [DEWIKI]:

> „Es herrscht **Farbzwang (auch für Tarock)**, jedoch **kein Stichzwang**; kann aber die ausgespielte Farbe
> nicht bedient werden, **muss mit Tarock gestochen werden** (Tarock‑ oder Trumpfzwang).“

Daraus die **exakte Entscheidungskaskade** (auf der Menge `spielbar = Hand ∪ offene Deckblätter`, §4.4):

```
Sei L die angespielte Karte, S = spielbar(Spieler).

1. FARBZWANG.   F := { c ∈ S : Farbe(c) == Farbe(L) }        // Tarock gilt als eigene "Farbe"
   Wenn F ≠ ∅            ⟹ legal = F.
2. TAROCKZWANG. T := { c ∈ S : c ist Tarock }
   Wenn T ≠ ∅            ⟹ legal = T.
3. FREIE WAHL.            ⟹ legal = S.
```

Zusatzregeln, die es hier **nicht** gibt (ausdrücklich festgehalten, weil sie in Nachbarspielen existieren):

* **Kein Stichzwang.** Man muss den Stich nie gewinnen [DEWIKI], [TIROL: „Es gibt keinen Stichzwang“].
  (Ausnahme: Trischaken, §5.9.)
* **Kein Überstichzwang.** Wer Tarock zugeben muss, darf einen **niedrigeren** Tarock zugeben, als der
  angespielte ist. Keine Quelle fordert Übertrumpfen; [TIROL] sagt es ausdrücklich („Es gibt keinen
  Stichzwang“), [MS‑WEB] nennt nur Farb‑ und Tarockzwang.
* **Kein Tarock‑Ausspielverbot**, kein Farbausspielzwang, keine Pagat‑Zurückhaltepflicht.
  (**Ausnahme:** im Trischaken des Profils `AT-STROH-TIROL-2025` gilt nach `trischaken_pagat_last_tarock`
  die Negativspiel‑Regel „Pagat nur als letzter Tarock“, §5.9.3 – und sie geht dem Stichzwang vor.)
* **Keine Sonderrolle für `Sküs`** – kein Excuse, kein Tauschrecht, kein Straflos‑Zugeben (§6.5).

### 6.3 Farbzwang und das Deckblatt (variantenspezifisch)

Der Zwang wird auf der **Vereinigung** von Hand und Deckblättern ausgewertet. [MS‑WEB] formuliert das
operativ:

> „Kann ein Spieler die vom Gegner angespielte Farbe **nicht aus der Hand** bedienen, muss er dies **von den
> Strohmännern** tun, falls dort eine Karte der entsprechenden Farbe als Deckblatt offen auf einem Päckchen
> aufliegt. Kann ein Spieler die vom Gegner angespielte Farbe **nicht von den Strohmännern** bedienen, muss
> er eine Karte aus der Hand spielen.“

und [DEWIKI] allgemeiner:

> „Kann ein Spieler aus der Hand nicht korrekt bedienen, muss er es nach Möglichkeit von den Strohmännern
> aus tun, **und umgekehrt**.“

Vier Fälle, die die App unterscheiden muss:

| Fall | Hand hat die Farbe | Deckblatt hat die Farbe | Erlaubt ist |
|---|---|---|---|
| A | ja | nein | **nur** die Handkarten dieser Farbe |
| B | nein | ja | **nur** das/die Deckblätter dieser Farbe |
| C | ja | ja | **beides** – freie Wahl (der Spieler entscheidet) |
| D | nein | nein | Tarockzwang, wieder über Hand ∪ Deckblätter |

Fall C ist eine Stelle, an der Anfänger am häufigsten stocken; die App sollte beide Quellen optisch
gleichwertig markieren (§11.4).

**Verdeckte Karten zählen nie.** Ein König, der unter einem Deckblatt schläft, macht den Spieler **nicht**
bedienpflichtig. Das ist der Grund, warum in Strohmandeln ein König überhaupt gefangen werden kann.

### 6.4 Stichgewinn

> „Den Stich gewinnt jener Spieler, der die höchste Karte in der angespielten Farbe gespielt hat, falls auch
> Tarock gespielt wurde, die höchste Tarock.“ [MS‑WEB]

Also, für zwei Karten `L` (ausgespielt) und `F` (zugegeben):

```
wenn F Tarock und L Tarock:      höherer Tarock gewinnt
sonst wenn F Tarock:             F gewinnt      (Tarock sticht jede Farbkarte)
sonst wenn L Tarock:             L gewinnt
sonst wenn Farbe(F) == Farbe(L): höherer Rang der Farbe gewinnt
sonst:                           L gewinnt      (Fehlfarbe abgeworfen, Fall D ohne Tarock)
```

**Es gibt keinen Kaiserstich** (`Sküs`+`XXI`+`I` im selben Stich), weil ein Stich hier nur zwei Karten hat.

**Stichhaltung:** *„Üblicherweise werden die Stiche nicht getrennt abgelegt, sondern einfach aufeinander
gestapelt.“* [MS‑WEB] — Für die Abrechnung spielt das keine Rolle; die App führt den Stichhaufen als Liste
und zeigt auf Wunsch den letzten Stich.

### 6.5 Sküs und Pagat – ausdrücklich **keine** Sonderregeln

| Karte | Rolle in Strohmandeln |
|---|---|
| **`Sküs`** | schlicht der **höchste Tarock** [MS‑WEB], [FURR]. Kein Excuse, kein „Narr“, keine Ausnahme vom Farb‑/Tarockzwang, kein Zurücknehmen gegen eine Leerkarte, keine Sonderbehandlung im letzten Stich. Er kann **nicht gestochen** werden. |
| **`XXI` (Mond)** | normaler zweithöchster Tarock. Kann **nur** vom `Sküs` gefangen werden (**Mondfang**, §5.8). |
| **`I` (Pagat)** | der **niedrigste** Tarock und zugleich ein Fünfer‑Zähler. Er verliert gegen **jeden** anderen Tarock, sticht aber **jede** Farbkarte. Einzige Sonderregel: die Prämie *Pagat ultimo* (§5.3). Keine Pflicht, ihn als letzten Tarock zu spielen; kein Schutz. |

Das ist für Spieler wichtig, die vom französischen Tarot kommen – dort ist der Narr eine Ausnahmekarte. In
der gesamten österreichischen Tarockfamilie („Typ III“ nach Dummett/McLeod) ist er es **nicht**.

### 6.6 Einsichtsrechte

* „Jeder Spieler hat das Recht, in den **unmittelbar zuvor** gespielten Stich des Gegners Einsicht zu
  nehmen.“ [MS‑WEB]
* „In den meisten Runden ist es auch zulässig, dass ein Spieler während eines Spieles **seine eigenen Stiche
  nochmals einsieht**, doch gilt dieses **Nachwassern** als unfein.“ [MS‑WEB]

**App:** Button „Letzter Stich“ immer aktiv. Button „Eigene Stiche“ per Einstellung
**⚙ Flag `allow_reviewing_own_tricks`** (Default `true` im Lern‑ und Einzelspielermodus, Default `false` im
Turniermodus). Im LAN wird die Nutzung dem Gegner **nicht** angezeigt (es ist keine Regelverletzung).

### 6.7 Renonce (Regelverstoß) und sonstige Fehler

[MS‑WEB §Renonceregeln], die einzige Quelle dazu:

| Fehler | Folge |
|---|---|
| Farbe nicht bedient **oder** Fehlfarbe nicht mit Tarock gestochen | **Renonce.** Der Gegner **gewinnt das Spiel und alle bisher realisierten Prämien**, ohne weiterspielen zu müssen. Der Renoncierende darf seine bisher realisierten Prämien **nicht** verrechnen. Der Gegner **kann** stattdessen verlangen, dass weitergespielt und korrigiert wird, um sich die Chance auf weitere Prämien (z. B. Grammel‑Punkt) zu wahren. |
| Mit Tarock gestochen, **obwohl** die passende Farbe als Deckblatt aufliegt | **Kein Renonce!** Weil der Gegner die aufliegenden Karten ebenfalls sieht, ist er verpflichtet, sofort auf den Fehler hinzuweisen und Korrektur zu verlangen. *„Der Fehler selbst bleibt sanktionslos.“* |
| Versehentlich **zwei** Karten gespielt | Eine der beiden darf zurückgenommen werden. *„Hier gilt der Grundsatz, dass es jedem Spieler unbenommen bleiben kann, Informationen über sein Blatt preiszugeben und sich damit selbst zu schaden.“* |
| Versehentlich **zwei** Strohmann‑Karten aufgedeckt (erste ist kein König/Trullstück) | Beide dem Gegner zeigen, beide wieder verdeckt zurücklegen (§4.7) |
| **Vergeben** (Misdeal) | Strafe **1 Spielpunkt** (in einer verdoppelten Runde **2**), Neugabe durch denselben Geber |

**Für die App:** Regelverstöße sind **unmöglich** – die Engine bietet nur legale Züge an. Die Renonce‑Regeln
sind trotzdem zu implementieren, und zwar für zwei Zwecke:

1. **Lernmodus:** Der Erklärtext für „warum ist diese Karte nicht erlaubt“ (§11.4) beruft sich auf sie.
2. **Turnier‑/Papiermodus (LAN mit physischen Karten):** Schiedsrichterfunktion „Renonce eintragen“.

Die zweite Zeile der Tabelle ist bemerkenswert und verdient einen eigenen Hinweis im Lernmodus: **Das
Übersehen des eigenen Deckblatts ist in Strohmandeln straffrei**, weil beide Spieler es sehen. Die App
verhindert den Zug ohnehin und zeigt stattdessen den Hinweistext aus §11.4.

### 6.8 Referenz‑Pseudocode für legale Züge

```cpp
enum class Src { Hand, Pile };
struct Move { Src src; int index; Card card; };   // index = Handindex oder Päckchenindex

std::vector<Move> playable(const Player& p) {
    std::vector<Move> out;
    for (int i = 0; i < (int)p.hand.size(); ++i)
        out.push_back({ Src::Hand, i, p.hand[i] });
    for (int i = 0; i < (int)p.piles.size(); ++i)
        if (p.piles[i].faceUp && !p.piles[i].cards.empty())      // nur das Deckblatt!
            out.push_back({ Src::Pile, i, p.piles[i].cards.front() });
    return out;
}

std::vector<Move> legalMoves(const Player& p, std::optional<Card> led, const Profile& prof) {
    std::vector<Move> all = playable(p);
    if (!led) return all;                                        // Ausspiel: alles erlaubt
    auto keep = [&](auto pred) {
        std::vector<Move> r;
        std::copy_if(all.begin(), all.end(), std::back_inserter(r),
                     [&](const Move& m){ return pred(m.card); });
        return r;
    };
    auto same = keep([&](const Card& c){ return suitOf(c) == suitOf(*led); });      // Tarock = eigene Farbe
    if (!same.empty()) return prof.trischaken ? forceWin(same, *led, p, prof) : same;  // §5.9.2
    auto tar  = keep(isTarock);
    if (!tar.empty())  return prof.trischaken ? forceWin(tar,  *led, p, prof) : tar;
    return all;
}

// §5.9.2/§5.9.3 – nur im Trischaken-Profil.
// Reihenfolge ist normativ: die Pagat-Regel geht dem Stichzwang VOR.
std::vector<Move> forceWin(std::vector<Move> cand, const Card& led, const Player& p,
                           const Profile& prof) {
    if (prof.trischaken_pagat_last_tarock && countTarocks(playable(p)) > 1) {
        auto noPagat = without(cand, PAGAT);      // Pagat nur, wenn er der letzte Tarock ist
        if (!noPagat.empty()) cand = noPagat;
    }
    std::vector<Move> w;                          // überstechen, wenn möglich
    std::copy_if(cand.begin(), cand.end(), std::back_inserter(w),
                 [&](const Move& m){ return beats(m.card, led); });
    return w.empty() ? cand : w;                  // innerhalb von w ist die Wahl FREI
}
```

`forceWin()` filtert im Trischaken‑Profil auf die **stichgewinnenden** Karten, falls es welche gibt – es
erzwingt **nicht** die höchste Karte (§5.9.2). Die Pagat‑Beschränkung wird **vor** dem Stichzwang angewandt:
könnte man nur mit dem Pagat überstechen, besitzt aber noch weitere Tarock, spielt man ein anderes Tarock
und verzichtet auf den Stich.

---

## 7. Abrechnung

### 7.1 Modell

* **Nullsumme je Spieler.** Was der eine gewinnt, verliert der andere. Ein Spiel erzeugt für jeden Sitz einen
  Integer `delta[seat]` mit `delta[0] + delta[1] == 0`.
* **Einheit:** *Spielpunkt*. In der Hartwährungs‑Variante [MS‑WZ] entspricht **1 Spielpunkt = 10 Cent**
  („einfaches Spiel 20 Cent“ = 2 Spielpunkte usw.). **⚙ Flag `hard_score_cents_per_point`, Default `10`,
  Anzeige optional.**
* **Schreibweise:** [TIROL] schreibt **nur die Pluspunkte** an; die App führt intern immer beide Vorzeichen
  und zeigt je nach Profil die eine oder andere Darstellung.

### 7.2 Auswertungsreihenfolge (verbindlicher Algorithmus)

```
1.  Renonce?  → §6.7; Spiel + alle Prämien des Gegners; Ende.
2.  Kartenpunkte: p3[s] = 3·Σwert(Stiche[s]) − 2·|Stiche[s]|      (§1.5.1)
    Assert: p3[0] + p3[1] == 210
3.  Spielwertung:
      wenn aufgenommenes Spiel:
          Aufnehmer gewinnt  ⟺ p3[Aufnehmer] >= 107            // 35⅔
          gewonnen  → +game_won        an den Aufnehmer
          verloren  → +game_lost       an den Gegner
      wenn einfaches Spiel:
          wenn p3[0] >= 107 → +simple  an Sitz 0
          sonst wenn p3[1] >= 107 → +simple an Sitz 1
          sonst → Unentschieden (p3 ∈ {104,105,106} bei BEIDEN): 0:0, Nullrunde setzen  (§1.7.1, §8.3)
          // p3[0] >= 107 UND p3[1] >= 107 ist unmöglich (107+107 = 214 > 210) – trotzdem assertieren
      wenn Trischaken (TIROL):
          Spieler mit dem KLEINEREN p3 → +2 ; bei Gleichstand (105:105) 0:0
4.  Grammel-Punkt (falls aktiv):
      rollenunabhängig (M&S, Default):  für jeden Sitz s mit pointsRounded(p3[s]) >= 45   // p3 >= 134
      rollengebunden (FURR-2009):       Aufnehmer  >= 45 gerundet  → +1 an den Aufnehmer
                                        Aufnehmer  <= 26 gerundet  → +1 an den Gegner    // Gegner >= 44
5.  Trull / vier Könige:
      Profil "in den Stichen":  wer alle 3 Trullstücke bzw. alle 4 Könige in den Stichen hat → +wert
      Profil "im Blatt, angesagt": wer gültig angesagt hat → +wert (unverlierbar)
      Profil TIROL: still +1, angesagt +2; misslungene Ansage −2 (Empfehlung, §5.2.3)
6.  Vögel (nur, wenn aktiv), je einzeln und unabhängig:
      Pagat  : Karte I   im  27. Stich → Gewinner des Stiches +1
      Uhu    : Karte II  im  26. Stich → Gewinner des Stiches +1
      Kakadu : Karte III im  25. Stich → Gewinner des Stiches +1
      Quapil : Karte IIII im 24. Stich → Gewinner des Stiches +1
      (nur, wenn die Karte in diesem Stich tatsächlich gespielt wurde)
7.  Rostopschin (falls aktiv): Rufworte korrekt + beide Stiche gewonnen → +1
8.  Mondfang (falls aktiv): XXI wurde vom Sküs gestochen → +wert an den Sküs-Spieler
9.  Valat: tricksWon[s] == 27 → +12
10. Kontra-Multiplikator (falls aktiv): NUR der Spielwert aus Schritt 3 wird × 2/4/8 genommen.
11. Rundenverdopplung (§8.3): ALLES (Spiel + Prämien) × 2, wenn die laufende Runde doppelt zählt.
12. delta[s] = Σ eigene Gutschriften − Σ gegnerische Gutschriften;  Assert: delta[0] + delta[1] == 0
```

**Wichtig zu Schritt 11:** Die Verdopplung nach Unentschieden/Nullergebnis betrifft *„die nächste Runde,
bestehend aus 2 Spielen“* [MS‑WEB] – also **beide** folgenden Spiele, nicht nur das nächste.

### 7.3 Abrechnung jedes Vertrags und jeder Prämie, gewonnen und verloren (Default‑Profil)

Alle Werte sind die **Gutschrift beim begünstigten Spieler**; der andere Spieler bekommt denselben Betrag
abgezogen.

| # | Ereignis | Begünstigter | Wert |
|---|---|---|---|
| 1 | **Einfaches Spiel gewonnen** (≥ 35/2 Blatt) | der Gewinner | +1 |
| 2 | **Einfaches Spiel**, niemand ≥ 35/2 Blatt | – | 0:0, **Nullrunde** |
| 3 | **Aufgenommenes Spiel gewonnen** | Aufnehmer | +3 |
| 4 | **Aufgenommenes Spiel verloren** | Gegner | +4 |
| 5 | **Grammel‑Punkt** (eigene Stiche ≥ 45 Punkte) | der Erreicher | +1 |
| 6 | **Trull in den Stichen** (`Sküs`+`XXI`+`I`) | der Besitzer | +1 |
| 7 | **Vier Könige in den Stichen** | der Besitzer | +1 |
| 8 | **Pagat ultimo gelungen** | der Pagat‑Spieler | +1 |
| 9 | **Pagat im letzten Stich abgefangen** | der Fänger | +1 |
| 10 | **Uhu gelungen** (`II` gewinnt Stich 26) | der Uhu‑Spieler | +1 |
| 11 | **Uhu misslungen** (`II` in Stich 26 gespielt und verloren) | der Fänger | +1 |
| 12 | **Kakadu gelungen / misslungen** (`III`, Stich 25) | analog | +1 |
| 13 | **Rostopschin** (Rufworte + 2 Stiche) | der Rufer | +1 |
| 14 | Rostopschin **ohne** Rufworte oder misslungen | – | 0 (unverlierbar) |
| 15 | **Valat** (27 Stiche) | der Valat‑Spieler | +12 |
| 16 | **Vergeben** | der Gegner des Gebers | +1 (in doppelter Runde +2) |
| 17 | **Renonce** | der Gegner | Spiel + **alle bis dahin realisierten Prämien**; Prämien des Renoncierenden verfallen |

**Maximale Einzelabrechnung** im Default‑Profil (ohne Kontra, ohne Rundenverdopplung):
Valat 12 + Spiel 3 + Grammel 1 + Trull 1 + 4 Könige 1 + Pagat 1 + Uhu 1 + Kakadu 1 + Rostopschin 1 = **22**.
(Ein Valat impliziert alle Fangprämien beim selben Spieler.) Der Wertebereich `int8_t` reicht damit nicht;
`int16_t` ist ausreichend, die Engine verwendet `int32_t`.

### 7.4 Durchgerechnete Abrechnungsbeispiele

**Beispiel 1 – knapper Gewinn des Aufnehmers (Zahlen aus §1.6).**
Vorhand hat aufgenommen und erreicht `35 ⅔` (= 35/2 Blatt = gerundet 36), der Geber `34 ⅓`.
Vorhand hat außerdem `Sküs`, `XXI` und `I` in den Stichen (Trull), aber nur drei Könige.
Der Pagat war nicht im letzten Stich.

| Posten | Vorhand | Geber |
|---|---|---|
| Aufgenommenes Spiel gewonnen | +3 | −3 |
| Grammel‑Punkt (35⅔ < 45) | 0 | 0 |
| Trull in den Stichen | +1 | −1 |
| Vier Könige | 0 | 0 |
| **Summe** | **+4** | **−4** |

**Beispiel 2 – der Aufnehmer scheitert knapp.**
Der Geber hat aufgenommen und erreicht `35 ⅓` (= 35/1 Blatt). Das ist **weniger** als 35/2 Blatt → verloren
(Flag `draw_scope = simple_only`, §1.7.1). Die Vorhand hat `34 ⅔`, außerdem die vier Könige in den Stichen;
der Geber hat den Pagat im letzten Stich gespielt und verloren.

| Posten | Geber (Aufnehmer) | Vorhand |
|---|---|---|
| Aufgenommenes Spiel verloren | −4 | +4 |
| Vier Könige in den Stichen | 0 | +1 |
| Pagat im letzten Stich abgefangen | −1 | +1 |
| **Summe** | **−6** | **+6** |

Das illustriert die Asymmetrie: Der Aufnehmer verliert 6, obwohl er nur um **⅓ Punkt** unter der Schwelle
lag. Genau davor warnt [MS‑WEB] beim Aufnahmeentscheid (§10.2).

**Beispiel 3 – einfaches Spiel mit Unentschieden.**
Beide haben gepasst. Endstand `35 : 35`. Niemand erreicht 35/2 Blatt.
Prämien: die Vorhand hat den Uhu gemacht (`II` gewinnt Stich 26).

| Posten | Vorhand | Geber |
|---|---|---|
| Einfaches Spiel | 0 | 0 |
| Uhu | +1 | −1 |
| **Summe** | **+1** | **−1** |
| **Folge** | die **nächste Runde (2 Spiele) zählt doppelt** (§8.3) | |

Hinweis: Auch ein Nettoergebnis von **0:0** („Nullergebnis“) löst die Verdopplung aus [MS‑WEB]. Hier ist das
Nettoergebnis wegen des Uhu **nicht** 0, aber das **Unentschieden im Spiel selbst** löst sie ebenfalls aus:
*„Ist ein einfaches Spiel vorgesehen, so gibt es auch die Regel, dass dieses unentschieden gewertet wird,
wenn keiner der Spieler 35 Punkte und 2 Blatt erreicht. … In diesen Fällen zählt die nächste Runde,
bestehend aus 2 Spielen, doppelt.“* Beide Auslöser sind unabhängig voneinander zu prüfen.

**Beispiel 4 – Valat.**
Die Vorhand hat aufgenommen und macht alle 27 Stiche.

| Posten | Vorhand | Geber |
|---|---|---|
| Aufgenommenes Spiel gewonnen (70 : 0) | +3 | −3 |
| Grammel‑Punkt (70 ≥ 45) | +1 | −1 |
| Trull in den Stichen | +1 | −1 |
| Vier Könige in den Stichen | +1 | −1 |
| Pagat ultimo (der Pagat ist zwangsläufig irgendwo, aber nur im 27. Stich zählt er) | +1 falls dort | ggf. −1 |
| Valat | +12 | −12 |
| **Summe (ohne Vögel)** | **+18** | **−18** |

---

## 8. Matchstruktur (für die App)

### 8.1 Die Runde

> „Ergibt sich nach Addition von Spiel und Prämien eine Nullsumme (Nullergebnis), so zählt **die nächste
> Runde, bestehend aus 2 Spielen**, doppelt.“ [MS‑WEB], [MS‑WZ]

Die **Runde** ist also die natürliche Zähleinheit: **2 Spiele**, sodass jeder Spieler einmal gegeben hat.
Das ist auch fairnesstechnisch zwingend, weil die Vorhand einen Vorteil hat (sie erklärt sich zuerst und
spielt aus).

### 8.2 Partieformate

| Format | Definition | Quelle / Empfehlung |
|---|---|---|
| **Punktepartie (Default)** | **Wer zuerst 11 Punkte erreicht, hat eine Partie gewonnen.** Notiert werden nur die Pluspunkte | [TIROL] |
| Rundenpartie | feste Zahl von Runden (z. B. 6 Runden = 12 Spiele); Sieger ist, wer die höhere Summe hat | Empfehlung dieser Spezifikation |
| Cent‑Partie | Abrechnung in Hartwährung, 1 Spielpunkt = 10 Cent, offenes Ende | [MS‑WZ] |
| Einzelspiel | genau ein Spiel (nur Lernmodus / „schnelle Runde“) | Empfehlung |

**Empfehlung für die App:** **Punktepartie bis 11** als Default für den Einzelspieler‑ und LAN‑Modus (kurz,
klar, endet nach typisch 4–6 Spielen), Rundenpartie über 6 Runden als „Turnier“‑Option, Einzelspiel im
Lernmodus.

Wichtig bei „bis 11“: Weil nur Pluspunkte geschrieben werden, ist es ein **Wettlauf**, kein Nullsummen‑Konto.
Die App führt beide Zählwerke parallel (`race[seat]` für die Partie, `net[seat]` für die Statistik) und zeigt
je nach Profil eines von beiden. Der **Partieabbruch** erfolgt sofort nach der Abrechnung des Spiels, in dem
11 erreicht wurde – auch mitten in einer Runde.

**⚙ Flag `match_format` ∈ {`race11`, `rounds`, `cents`, `single`}, Default `race11`.**
**⚙ Flag `match_target`, Default `11`.**

### 8.3 Verdoppelte Runde („Nullrunde“)

Auslöser (beide unabhängig prüfen, [MS‑WEB], [MS‑WZ]):

1. **Unentschieden** im einfachen Spiel (§1.7.1).
2. **Nullergebnis**: Spiel + Prämien ergeben in Summe 0:0.

Wirkung: **die nächste Runde – also die nächsten zwei Spiele – zählt doppelt.** Auch die Strafe für Vergeben
verdoppelt sich in einer solchen Runde auf 2 Punkte [MS‑WEB].

Offene Detailfragen, die die Quellen nicht beantworten, und die Festlegungen dieser Spezifikation:

| Frage | Festlegung |
|---|---|
| Was, wenn eine verdoppelte Runde **erneut** ein Nullergebnis liefert? | Der Multiplikator **stapelt sich nicht**; die nächste Runde zählt wieder ×2, nicht ×4. (Konservativ, verhindert Ausreißer.) **⚙ Flag `null_round_stacks`, Default `false`** |
| Verdoppelt sich das **laufende** oder das **nächste** Spiel, wenn das Nullergebnis mitten in einer Runde auftritt? | Die **nächste vollständige Runde** (2 Spiele), beginnend mit dem nächsten Spiel. Läuft gerade eine Runde, wird der verbleibende Teil normal gewertet |
| Zählt die Verdopplung auch für die „bis 11“‑Wertung? | Ja – es ist derselbe Wert |

### 8.4 Persistenz, Abbruch, Wiederaufnahme

* Der Partiezustand (Profil, Punktestände, Geberrotation, aktiver Multiplikator, Nullrunden‑Zähler) wird nach
  **jedem** Spiel gespeichert; ein abgebrochenes Spiel wird nicht gespeichert (das Spiel wird verworfen, das
  Geben bleibt beim selben Spieler).
* Im LAN gilt: Trennt sich ein Client, wird sein Sitz für 120 s reserviert; danach kann ihn ein
  Computerspieler übernehmen (Einstellung) oder die Partie endet unentschieden.

### 8.5 Statistiken (Wiederspielwert)

Wegen der zwei Sichtbarkeitsstufen ist Strohmandeln statistisch interessanter als die meisten Zweierspiele.
Empfohlene Kennzahlen pro Spieler:

* **Aufnahmequote** und **Trefferquote der Aufnahme** (wie oft gewinnt der Aufnehmer?)
* **Ø Stecherpunkte bei Aufnahme** (§10.2) – zeigt, ob jemand zu mutig oder zu zaghaft aufnimmt
* **Ø Kartenpunkte** und Verteilungshistogramm um die 35er‑Schwelle
* **Strohmann‑Abbaugeschwindigkeit**: nach welchem Stich sind die eigenen 6 bzw. 3 Päckchen aufgebraucht?
  (M&S' zentrale Taktikempfehlung, §10.3 – eine direkt messbare Lernkurve)
* **Pagat‑Bilanz**: heimgebracht / abgefangen / verloren
* Prämienzähler je Prämie

---

## 9. Grenzfälle, regionale Abweichungen, Empfehlungen

Jeder Punkt: Was steht in den Quellen, wo widersprechen sie sich, und **was die App tut** (▶ **Empfehlung**).

### 9.1 Zwei völlig verschiedene Spiele mit demselben Namen (historisch)

Das heutige Strohmandeln ist **nicht** das Spiel, das 1829 so hieß [ENWIKI, mit Berufung auf [HIST] und
[DUMMETT]]:

| Zeit | Spiel |
|---|---|
| 1800 | *„Taroc en deux“* (78 Blatt): ein verdeckter Dummy von 25 Karten wird gegeben, aber **nie benutzt** |
| 1829 | *„Tarok‑Tappen unter zwei Personen oder das sogenannte Strohmandel“* (54 Blatt): der **Stichgewinner nimmt jeweils die oberste Karte des Strohmann‑Stapels** und legt sie zu seinen Stichkarten. Sehr glücksabhängig |
| 1839 | Ein Wiener Spielbuch urteilt, das Spiel hänge mehr vom Glück als vom Können ab und werde „wohl nur von Leuten gewählt, die das edle Piquetspiel gar nicht kennen“ |
| ≤ 1890 | Ulmann beschreibt bereits die **heutige** Form: drei Päckchen zu vier Karten als anfangs unbekannter Teil des eigenen Blattes. [MS‑WEB]: *„Strohmann‑Tarock dürfte spätestens in den 1880er Jahren entstanden sein“* |

▶ **Empfehlung:** Nur die moderne Form implementieren. Die historische 1829er‑Form ist als Randnotiz im
Lernmodus (§11.6 Glossar, Eintrag „Strohmann“) erwähnenswert, aber kein Spielmodus.

### 9.2 Beck: 1/2/3 oder 2? (Wertwiderspruch zwischen den beiden Wikipedias)

| Quelle (beide berufen sich auf [BECK] S. 40–45) | Spielwerte | Prämienwerte |
|---|---|---|
| [ENWIKI] | einfaches Spiel 1; aufgenommen gewonnen **2**, verloren **3** | Trull/Königstrull **im Blatt** 1, Pagat ultimo 1, Mondfang 1 |
| [DEWIKI] | „1 Werteinheit für ein gewonnenes Spiel, wenn das Spiel **nicht** aufgenommen wurde; **2 Werteinheiten**, wenn aufgenommen“ | Trull/Königstrull **2**, Pagat als letzte Karte **2**, gefangener Mond **2** |

[DEWIKI] ist auch **in sich** widersprüchlich: im Fließtext heißt es, die Prämie betrage „den **einfachen
Spielwert**“, in der Tabelle stehen dann 2 Werteinheiten, während ein nicht aufgenommenes gewonnenes Spiel
nur 1 wert ist. [DEWIKI] nennt außerdem **keinen** erhöhten Wert für ein vom Aufnehmer **verlorenes** Spiel,
was der in allen anderen Quellen tragenden Risiko‑Asymmetrie widerspricht.

▶ **Empfehlung:** Im Profil `AT-STROH-BECK-1972` die **[ENWIKI]‑Werte** verwenden (1 / 2 / 3, Prämien je 1).
Sie sind granularer, passen zur Systematik aller anderen Quellen und stehen in der neueren, besser belegten
Darstellung. **Der Widerspruch ist im Profiltext der App zu vermerken**, weil er nur durch Einsicht in
Beck 1972 S. 40–45 aufzulösen wäre – ein Buch, das für diese Spezifikation **nicht im Volltext vorlag**.

### 9.3 Wer spielt zum ersten Stich aus?

| Quelle | Regel |
|---|---|
| [MS‑WEB], [MS‑WZ], [DEWIKI]/[BECK], [TIROL] | **Immer die Vorhand** |
| [FURR] | Bei „beide passen“ die Vorhand, sonst **der Aufnehmer** |

▶ **Empfehlung: immer die Vorhand** (`first_lead = forehand`). Vier unabhängige deutschsprachige Quellen
gegen eine englische Hausregelfassung. Furrs Regel ist zudem im aufgenommenen Spiel ein spürbarer
Zusatzvorteil für den Aufnehmer, den die Wertetabelle nicht einpreist.

### 9.4 Wert des einfachen Spiels: 1 oder 2?

| Quelle | Wert |
|---|---|
| [MS‑WEB] klassische Variante | 2 |
| [MS‑WEB] erweiterte Variante | 1 (+1 bei ≥ 45 Punkten ⇒ 2) |
| [MS‑WZ] (2007, dieselben Autoren) | 2 (20 Cent), kein Grammel‑Punkt |
| [FURR] | 2 |
| [ENWIKI]/[BECK] | 1 |

▶ **Empfehlung:** Im Default `MS-ERW` bei **1 + Grammel‑Punkt** bleiben (die Tabelle in [MS‑WEB] ist in sich
geschlossen). Wer ohne Grammel‑Punkt spielt, nimmt Profil `MS-WZ2007` mit 2. Die App darf die beiden Werte
**nicht** mischen, sonst ist das einfache Spiel gegenüber dem aufgenommenen falsch skaliert.

### 9.5 Ist ein abgefangener Pagat prämiert?

| Quelle | Regel |
|---|---|
| [MS‑WEB] | „Pagat ultimo … **und ein im letzten Stich abgefangener Pagat** kosten ebenfalls 1 Spielpunkt“ → **ja** |
| [ENWIKI] | „it is normal for bonuses for playing the ‚birds‘ … to go to the opponent if they fail to win“ → **ja**, und zwar auch für Uhu/Kakadu |
| [DEWIKI]/[BECK] | „Dies bringt (je nachdem ob er überstochen wird oder nicht) den **Gewinn oder Verlust** des einfachen Spielwertes“ → **ja** |
| [TIROL] | nennt nur „Pagat ultimo … Tarock I macht den letzten Stich“ → **wohl nein** |
| [MS‑WZ] | nennt nur „Pagat, still: 10 Cent“ → unklar |

▶ **Empfehlung: ja** (`pagat_capture_bonus = true`, `bird_capture_bonus = true`). Drei Quellen dafür, eine
schweigt. Die Spiegelregel ist außerdem für die KI wichtig: ohne sie gäbe es keinen Anreiz, den gegnerischen
Pagat im letzten Stich zu jagen, und das Endspiel würde flach.

### 9.6 Grammel‑Punkt: Schwelle beim Gegner 25 oder 26? Und: gerundet oder exakt?

| Quelle | Formulierung |
|---|---|
| [MS‑WEB], [MS‑EN] | „mindestens 45 Punkte … Diesfalls hat sein Gegner **25 Punkte oder weniger**. Hat der Gegner **mindestens 26** Punkte, so muss er keinen Grammel‑Punkt zahlen“ |
| [FURR] | „With 45: If Declarer takes **at least 45** card points. Scores 4“ / „With 26: If Declarer takes **26 card points or less**: Scores 5 for the defender“ |
| [ENWIKI] | „Major win by declarer: Win by declarer with **45 or more** points“ / „Major win by defender: Win by defender with **44 or more** points“ (Furr‑Spalte) |

**Zwei getrennte Fragen, die frühere Fassungen dieser Spezifikation vermischt haben:**

**(a) Welche Schwelle?** Bei 70 Gesamtpunkten sind „ich ≥ 45“ und „Gegner ≤ 25“ **äquivalent**
(45 + 25 = 70). Furrs „Aufnehmer ≤ 26“ entspricht dagegen „Gegner ≥ **44**“ – das ist eine **andere**
Regel und zu Furrs übriger Systematik konsistent (bei ihm gewinnt der Verteidiger das Spiel schon mit 35,
also liegen auch seine Großgewinn‑Schwellen um 1 tiefer). ▶ **Default: M&S** (`grand_point_threshold = 45`,
`grand_point_defender_threshold = 45`); im Profil `FURR-2009` `grand_point_defender_threshold = 44`.

**(b) Gerundet oder exakt gerechnet?** Hier lag ein **echter Fehler** in der vorigen Fassung
(`p3 >= 135`). Maßgeblich ist die **gerundete** Zahl: `pointsRounded(p3) >= 45`, d. h. **`p3 >= 134`**.
Der Grund ist [MS‑WEB] selbst – „25 oder weniger“ und „mindestens 26“ lassen keinen Zwischenraum für
„25 und 1 Blatt“, die Autoren rechnen dort also in ganzen Punkten (Dreierlagen, §1.5.2a). Genau ein Fall
unterscheidet die beiden Lesarten: `44 ⅔ : 25 ⅓`, am Tisch gezählt als **45 : 25**. Volle Herleitung samt
Tabelle in §5.6. ▶ **Default: `grand_point_uses_rounded_points = true` (p3 ≥ 134).**

### 9.7 Zählt das Deckblatt zum „Blatt“ bei Trull‑/Königsansagen?

Keine Quelle sagt es ausdrücklich. Argumente für **ja**: [MS‑WEB] stellt die Deckblätter für **alle**
Spielhandlungen den Handkarten gleich („kann der Spieler ebenso verfahren wie mit den Karten in seiner
Hand“). Argument für **nein**: Die Ansage heißt ausdrücklich „im **Blatt**“, und die Deckblätter liegen auf
dem Tisch, nicht im Blatt.

▶ **Empfehlung: ja** (`bonus_hand_includes_faceup = true`). Die Deckblätter sind nach [MS‑WEB] Teil des
Blattes; jede andere Lesart erzeugt eine willkürliche Ausnahme. **Betrifft nur die Profile `MS-KLASS` und
`BECK-1972`** – im Default ist die Frage gegenstandslos, weil dort „in den Stichen“ gezählt wird.
→ **Offene Frage für die Rückfrage bei Spielkreisen.**

### 9.8 Verdoppelt Kontra auch die Prämien?

[MS‑WEB] sagt nur „Im Kontra werden die **Spielpunkte** verdoppelt“. In Königrufen werden Spiel und Prämien
**getrennt** kontriert.

▶ **Empfehlung:** Kontra verdoppelt **nur den Spielwert**, nicht die Prämien
(**⚙ Flag `kontra_doubles_bonuses = false`**). Und: Kontra ist im Default ohnehin **abgeschaltet**, weil
[MS‑WEB], [MS‑WZ] und [TIROL] übereinstimmend davon abraten (§5.10).

### 9.9 Rostopschin: XVII/XVIII oder VII/VIII?

**Belegbarer Fehler in [ENWIKI].** Die dortige Scoring‑Tabelle sagt „Taking successive tricks with **VII and
VIII**“ und zitiert dafür [FURR]. Der Volltext von [FURR] (archive.org, S. 111) sagt jedoch wörtlich:
*„taking consecutive tricks with the **XVII and the XVIII** of trumps“*. [MS‑WEB] sagt ebenfalls
**XVII/XVIII**.

▶ **Empfehlung: `XVII` und `XVIII`.** Zwei Primärquellen gegen einen Transkriptionsfehler im
Wikipedia‑Tabellenzelleninhalt. Beim Nachprüfen (siehe **Prüfprotokoll**) wurde der Volltext von [FURR]
erneut eingesehen und bestätigt: *„Rostopschin: This is a fun bonus for taking consecutive tricks with the
**XVII and the XVIII** of trumps. To win it, you must call ‚Ros‘ when playing the XVII to a trick and then
‚Topschin‘ when playing the XVIII.“* Es ist nicht der einzige Fehler in dieser Tabelle → §9.14.

### 9.10 Trischaken im Zweierspiel: „bunt gemischt“ und Stichzwang

Nur [TIROL] kennt Trischaken beim Strohmandeln; die Kurzbemerkung *„möglichst wenig Punkte / bunt gemischt;
mit Stichzwang“* ist aus sich heraus nicht verständlich. **Zwei der drei Unklarheiten sind inzwischen
geklärt** (§5.9.1–5.9.3):

| Teilfrage | Status |
|---|---|
| „bunt gemischt“ | **geklärt.** Fachausdruck: „es braucht nicht austarockiert zu werden, jeder Ausspieler hat die Wahl, ob er Tarock oder Farbe anspielt“ (LTA‑GK1 Kap. 11; `tarock-konigrufen8.webnode.at/trischaken/`; `koenigrufen.md` §6.4). In Strohmandeln ohne praktische Wirkung, weil es dort ohnehin keine Austarockier‑Pflicht gibt |
| „mit Stichzwang“ | **geklärt.** = *überstechen, wenn unter Farb‑/Tarockzwang möglich* – **nicht** „die höchste Karte spielen“. Innerhalb der stichgewinnenden Karten ist die Wahl frei |
| Pagat‑Regel | **offen.** In den Negativspielen desselben Autors gilt „Der Pagat darf erst als letztes Tarock gespielt werden“; das Strohmandeln‑Blatt schweigt. ⚙ `trischaken_pagat_last_tarock`, vorläufig `true` |
| Gleichstand | **offen.** [TIROL] regelt ihn nicht; vorläufig 0 : 0 und Nullrunde |

▶ **Empfehlung:** Normales Strohmandeln mit Strohmännern, umgekehrtes Zählziel, Stichzwang nach §5.9.2,
Pagat‑Regel nach §5.9.3. Vor Auslieferung des Profils als „Tiroler Regeln“ bleibt eine Rückfrage bei
Markus Mair (tarock.tirol) zu Pagat‑Regel und Gleichstand empfohlen.

### 9.11 Ansagezeitpunkt im Tiroler Profil

[TIROL] tabelliert „still 1 / angesagt 2“, nennt aber **keinen** Ansagezeitpunkt und keine Strafe für eine
misslungene Ansage.

▶ **Empfehlung (Ergänzung dieser Spezifikation, klar als solche markiert):** Ansage **nach der
Erstaufdeckung und vor dem ersten Ausspiel**; misslungene Ansage = angesagter Wert an den Gegner. Das ist die
in Königrufen übliche Systematik. **Offene Frage.**

### 9.12 Ungarn: „Strohmandli“ mit 42 Blatt

> „Strohmandeln wird heute nicht nur in Österreich, sondern auch in Ungarn gespielt. Dort heißt es
> **Strohmandli**. In anderen Ländern der ehemaligen Monarchie scheint es nicht verbreitet zu sein.“
> [MS‑WEB]

Die dort verbreitete Fassung [HU‑SCO] ist ein **eigenständiges Spiel**:

| Merkmal | Österreich (diese Spec) | Ungarn [HU‑SCO] |
|---|---|---|
| Blatt | 54 | **42** (wie ungarisches Tarokk) |
| Handkarten | 15 | **12** |
| Strohmänner | 3 Päckchen à **4** | 3 Päckchen à **3** |
| Aufnehmen | ja | **nein** – es gibt keinen Aufnehmer und kein Lizit |
| Gewinnschwelle | 35/2 Blatt von 70 | **48** von 94 (einfache Addition, ohne Dreierlagen) |
| Figuren | Trull, 4 Könige, Pagat, Uhu … | Játék 48/Kassza 60/Duplajáték 71, Volát, Pagátfogás, Uhu, Pagátulti, Fej, Tulétroá, Négy király |
| Aufdeckregel | identisch (Tarock/König aufnehmen, sonst Deckblatt) | **identisch** |

▶ **Empfehlung:** **Nicht** implementieren. Die Strohmann‑Mechanik ist dieselbe, alles andere ist ein
anderes Spiel; es gehört eher zur ungarischen Regelfamilie (`/home/defaultuser/ps/tarock-spec/hungarian.md`)
als hierher. Falls es später gewünscht wird: eigenes Profil `HU-STROHMANDLI-42`, Deckprofil `HU-42`.

### 9.13 Die 40‑Blatt‑Variante der Zwanzigerrufen‑Kreise

> „Strohmann‑Tarock wie oben beschrieben wird von Tarockspielern gespielt, die zu viert Königrufen spielen.
> Wer zu viert **Zwanzigerrufen** mit vierzig Karten spielt, wird auch Strohmann‑Tarock mit **40 Karten**
> spielen. Jeder Spieler erhält **10 Karten**, und die Strohmänner bestehen aus **einem Päckchen zu vier und
> zwei Päckchen zu drei** Karten. Im Übrigen gelten die Regeln des Zwanzigerrufens und die der oben
> beschriebenen Variante von Strohmann‑Tarock entsprechend.“ [MS‑EN]

Rechenprobe: 40 Karten − 2 × 10 Handkarten = 20 Karten = 2 × (4 + 3 + 3) ✔. Jeder Spieler hat 20 Karten,
es gibt 20 Stiche.

**Zusammensetzung des 40‑Blatts** (aus den Zwanzigerrufen‑Regeln, nicht aus [MS‑EN], das nur „die Regeln des
Zwanzigerrufens gelten entsprechend“ sagt):

* **20 Tarock:** `Sküs`, `XXI`…`IIII`, `I` – entfernt werden **`Tarock II` und `Tarock III`**.
* **20 Farbkarten:** je Farbe `K`, `D`, `C`, `B` und **eine „Glatze“**. Üblich sind `♠10 ♣10 ♥A ♦A`;
  manche Kreise nehmen stattdessen `♥4 ♦4`, weil die leichter zu erkennen sind. Entfernt werden also
  `♠9 ♠8 ♠7`, `♣9 ♣8 ♣7`, `♥2 ♥3 ♥4`, `♦2 ♦3 ♦4` (bei der Glatzen‑Variante entsprechend `♥A ♥2 ♥3` usw.).

**Wertung und Zählung ändern sich gegenüber dem 54er‑Spiel grundlegend** – die frühere Behauptung „Kartenwerte
und Zählung bleiben“ war falsch:

| | Strohmandeln 54 | Strohmandeln 40 (Zwanzigerrufen‑Konventionen) |
|---|---|---|
| Trull `Sküs XXI I` | 5 | 5 |
| übrige Tarock | 1 | 1 |
| `K` / `D` / `C` / `B` | 5 / 4 / 3 / 2 | 5 / 4 / 3 / 2 |
| Zahlkarten | 1 | **Glatzen zählen 0**; andere Zahlkarten gibt es nicht |
| Zählverfahren | Dreierlagen −2 (§1.5) | **einfache Addition**, kein Drittel‑Abzug, kein Stichbonus |
| Gesamtpunkte | 70 | **88** (= 3·5 + 17·1 + 4·(5+4+3+2+0)) |
| Gewinnschwelle | 35/2 Blatt (36) | **45** („mehr als die Hälfte“) |

* **Uhu und Kakadu entfallen ersatzlos**, weil `II` und `III` nicht im Blatt sind. Ein **Quapil** (`IIII`)
  wäre technisch möglich, ist aber in keiner Quelle für das 40er‑Spiel belegt.
* Der **Märchenstich/Kaiserstich** (`Sküs`+`XXI`+`I` im selben Stich → Pagat gewinnt), den Zwanzigerrufen
  kennt, kann im Zweierspiel **nicht** auftreten (ein Stich hat nur zwei Karten).
* Die **Grammel‑Punkt‑Schwelle 45** des 54er‑Spiels ist im 40er‑Spiel **sinnlos**, weil 45 dort bereits die
  Gewinnschwelle ist. Sie müsste neu kalibriert werden; proportional übertragen (45 von 70 ≙ 64,3 %) käme man
  auf **57 von 88**. Dafür gibt es aber **keine Quelle**, und die Engine darf so etwas nicht stillschweigend
  erfinden – im Profil `AT-STROH-40` ist `bonus_grand_point` daher vorerst `false`.

▶ **Empfehlung:** Profil `AT-STROH-40` **vorsehen, aber nicht in der ersten Version ausliefern**. Deck,
Kartenwerte, Zählverfahren und Gewinnschwelle stehen jetzt fest; **offen bleiben** die Prämientarife
(Grammel‑Punkt‑Schwelle, Valat‑Wert bei 88 statt 70 Punkten) sowie die Frage, ob die Zwanzigerrufen‑Kreise
überhaupt mit Prämien spielen. Erst nach Fertigstellung der Zwanzigerrufen‑Spezifikation entscheiden.

### 9.14 [ENWIKI]s Furr‑Spalte: drei Abweichungen vom Volltext

Beim Abgleich der Scoring‑Tabelle in [ENWIKI] mit dem **Volltext** von [FURR] (Kap. 5.05, S. 109–111) fallen
drei Unstimmigkeiten auf. Die App richtet sich im Profil `EN-STROH-FURR-2009` nach dem **Volltext**.

| Zeile in [ENWIKI] | [ENWIKI] | [FURR] im Volltext | Bewertung |
|---|---|---|---|
| *Trull or Royal Trull (tricks)* | Furr **1** | **Kommt in Furrs Bonusliste überhaupt nicht vor.** Sie lautet vollständig: „With 45 … With 26 … Slam … The Birds (Pagat Ultimo, An Owl, A Cockatoo) … Rostopschin“ | **Fehler in [ENWIKI]** |
| *Rostopschin* | „Taking successive tricks with **VII and VIII**“ | „taking consecutive tricks with the **XVII and the XVIII** of trumps“ | **Fehler in [ENWIKI]** (§9.9) |
| *Quapil* | Furr **–** | korrekt: Furr kennt den Quapil nicht („omits the Quapil“, so auch [ENWIKI] im Fließtext) | stimmt |

Ergänzend, weil in [ENWIKI]s Tabelle nicht sichtbar:

* [FURR] kennt **keinen Mondfang** und **keine Ansagen**: *„however, none of them may be announced!“* – mit
  der einzigen Ausnahme des Rostopschin, der ohne die Rufworte „Ros!“/„Topschin!“ gar nicht zustande kommt.
* [FURR] lässt bei fehlender Farbe **und** fehlendem Tarock **jede** Karte zu: *„If they cannot do either,
  then they may play any card – though it will not win.“* Deckungsgleich mit [MS‑WEB] (§6.2).
* [FURR]s Zählregel ist eine Kurzform der Dreierlagen: *„two odd empty cards count for 1 and one odd empty
  card counts for nothing“* – rechnerisch identisch mit §1.5.2a, aber nur für Leerkarten formuliert. Die
  Engine verwendet die allgemeine Formel aus §1.5.1.

▶ **Empfehlung:** Im Profil `EN-STROH-FURR-2009` **keine** Trull‑/Königstrull‑Prämie (so bereits §5.11),
Rostopschin mit `XVII`/`XVIII`, kein Quapil, kein Mondfang. **Der Widerspruch zu [ENWIKI] ist im Profiltext
der App zu vermerken**, damit Nutzer, die die Wikipedia‑Tabelle kennen, ihn nicht für einen App‑Fehler halten.

### 9.15 Zusammenfassung aller Profil‑Flags

| Flag | Default | §  |
|---|---|---|
| `strawmen_dealt_before_bidding` | `true` | 2.4 |
| `throw_in_no_tarock` | `true` | 2.5 |
| `redeal_if_both_pass` | `simple_game` | 3.2 |
| `redeal_dealer` | `same_dealer` | 3.2 |
| `draw_scope` | `simple_only` | 1.7.1 |
| `first_lead` | `forehand` | 6.1 |
| `open_cards_taken_immediately` | `true` | 4.2 |
| `last_card_is_secret` | `true` | 4.5 |
| `last_card_stays_faceDown_on_table` | `false` | 4.5 |
| `bonus_hand_includes_faceup` | `true` | 5.2.1 / 9.7 |
| `bonus_trull_mode` | `tricks_silent` | 5.2 |
| `bonus_kings_mode` | `tricks_silent` | 5.2 |
| `pagat_capture_bonus` | `true` | 5.3 |
| `bonus_uhu` / `bonus_kakadu` | `true` | 5.4 |
| `bonus_quapil` | `false` | 5.4 |
| `bird_capture_bonus` | `true` | 5.4 |
| `bonus_rostopschin` | `true` | 5.5 |
| `rostopschin_cards` | `{XVII, XVIII}` | 5.5 / 9.9 |
| `rostopschin_call_spelling` | `"Ross"` | 5.5 |
| `bonus_grand_point` | `true` | 5.6 |
| `grand_point_threshold` | `45` | 5.6 |
| `grand_point_defender_threshold` | `45` (FURR: `44`) | 5.6 / 9.6 |
| `grand_point_role_bound` | `false` (FURR: `true`) | 5.6 / 9.6 |
| `grand_point_uses_rounded_points` | `true` | 5.6 / 9.6 |
| `bonus_mondfang` | `false` | 5.8 |
| `valat_replaces_game` | `false` | 5.7 |
| `kontra_enabled` | `false` | 5.10 |
| `kontra_doubles_bonuses` | `false` | 9.8 |
| `trischaken_on_both_pass` | `false` | 5.9 |
| `trischaken_pagat_last_tarock` | `true` | 5.9.3 |
| `null_round_stacks` | `false` | 8.3 |
| `match_format` / `match_target` | `race11` / `11` | 8.2 |
| `allow_reviewing_own_tricks` | `true` | 6.6 |
| `hard_score_cents_per_point` | `10` | 7.1 |

---

## 10. Hinweise für den Computergegner

### 10.1 Das Informationsmodell – der Schlüssel zum ganzen Spiel

Strohmandeln ist ein Spiel mit **asymmetrischer, sich im Verlauf auflösender** unvollständiger Information.
Die KI muss folgende Mengen strikt getrennt führen:

| Menge | Inhalt | Für die KI |
|---|---|---|
| `myHand` | eigene Handkarten | bekannt |
| `myPilesHidden` | eigene Karten unter den eigenen Deckblättern | **unbekannt, auch für mich!** |
| `myFaceUp` | eigene Deckblätter | bekannt, dem Gegner ebenfalls |
| `oppFaceUp` | gegnerische Deckblätter | **bekannt** – wertvollste öffentliche Information |
| `oppKnownHand` | Karten, die der Gegner **offen** aus seinen Strohmännern genommen hat (Tarock/Könige) | **sicher beim Gegner in der Hand** (bis er sie spielt) |
| `oppSecretCount` | Anzahl der verdeckt aufgenommenen letzten Päckchenkarten des Gegners | Zähler; Inhalt unbekannt |
| `played` | alle bereits gespielten Karten | bekannt |
| `unseen` | Rest = 54 − (alles Bekannte) | Kandidatenmenge für Sampling |

Die Folgerung für die Zugsuche: **Perfect‑Information‑Monte‑Carlo (PIMC) / Determinisierung** funktioniert
hier sehr gut, weil `unseen` mit jedem Stich schrumpft und ab etwa Stich 15 fast leer ist.

**Sampling‑Constraints** (zwingend, sonst produziert die KI unmögliche Welten):

1. Jede Karte in `oppKnownHand` liegt sicher in der gegnerischen Hand.
2. `oppFaceUp` ist fix und nicht umverteilbar.
3. Die Anzahl der Karten pro gegnerischem Päckchen ist bekannt (sie ist öffentlich sichtbar).
4. **Void‑Inferenz:** Hat der Gegner in einer Farbe mit Tarock gestochen oder abgeworfen, obwohl die Farbe
   angespielt war, ist er in dieser Farbe **in Hand und Deckblättern** blank gewesen – aber er kann sie
   später aus einem Päckchen nachbekommen. Die Void‑Markierung muss daher **mit dem nächsten Nachdecken
   dieses Spielers verfallen**. Das ist die subtilste Inferenzregel des Spiels und wird von Gelegenheits‑KIs
   regelmäßig falsch gemacht.
5. Die Zählkartenbilanz: `Σ Punkte(unseen) = 70·3/3 − Punkte(played) − Punkte(bekannte Hände)`.

### 10.2 Die Aufnahme‑Entscheidung

Die einzige echte Bietentscheidung des Spiels. [MS‑WEB] gibt eine erstaunlich präzise Heuristik:

> „Wir haben festgestellt, dass man ein aufgenommenes Spiel riskieren kann, wenn man **sichere Stecher im
> Blatt hat, deren Wert 20 bis 25 Punkte** ausmacht. Kommt man nur auf knappe 20 Punkte, so ist die Aufnahme
> eher riskant, kommt man in den Bereich von 25 Punkte oder sogar darüber, so sind die Aussichten auf einen
> Erfolg sehr groß.“
>
> „Wichtig ist allerdings, dass du auch **mindestens 6 bis 7 Tarock** besitzt.“

**Was ein „sicherer Stecher“ ist** [MS‑WEB]:

* `Sküs` und `XXI` (Mond) – immer.
* Der `Pagat` – *„der im Strohmann‑Tarock meist heimzubringen ist“*.
* **Könige** – und die **Damen derselben Farbe**.
* **Mehrfach besetzte Damen** auch ohne den passenden König.

Bewertet wird in **Bruchzahlen** (§1.5.2c): Trullstück/König = 4 ⅓, Dame = 3 ⅓, Cavall = 2 ⅓.

**Die beiden Beispiele aus [MS‑WEB] (wörtlich nachgerechnet):**

| Blatt | Rechnung | Wert | Urteil |
|---|---|---|---|
| `Sküs`, `XXI`, `♥K`, `♥D`, mehrfach besetzte `♣D` | 4⅓ + 4⅓ + 4⅓ + 3⅓ + 3⅓ | **19 ⅔** | *„Risikofreudige Spieler werden in diesem Fall aufnehmen und auf überdurchschnittlich gute Karten in den Strohmännern hoffen. Stellen sich diese nicht ein, ist das Spiel verloren.“* |
| `Sküs`, `I`, `♠K`, `♠D`, `♣K`, gut besetzte `♥D` | 4⅓ + 4⅓ + 4⅓ + 3⅓ + 4⅓ + 3⅓ | **24** | *„In diesem Fall … wirst du wohl aufnehmen. Die fehlenden Punkte auf das Spielziel (35 Punkte und 2 Blatt) sollten sich in den meisten Fällen einstellen.“* |

**Implementierungsvorschlag (Stufe „normal“):**

```cpp
// Erwartungswert-Schätzung VOR der Erstaufdeckung. Nur 15 von 27 eigenen Karten sind bekannt.
Fraction stecherwert(const Hand& h) {
    Fraction v = 0;
    for (Card c : h) {
        if (c == SKUES || c == XXI || c == PAGAT)        v += F(13,3);       // 4⅓
        else if (isKing(c))                              v += F(13,3);
        else if (isQueen(c) && (hasKing(h, suit(c)) || suitLength(h, suit(c)) >= 3))
                                                         v += F(10,3);       // 3⅓
        else if (isCavall(c) && hasKing(h,suit(c)) && hasQueen(h,suit(c)))
                                                         v += F(7,3);        // 2⅓
    }
    return v;
}
bool shouldTakeUp(const Hand& h, int level) {
    int tarocks = countTarocks(h);
    Fraction v  = stecherwert(h);
    if (tarocks < 5) return false;                       // fast immer Selbstmord
    Fraction threshold = (level == EASY)   ? F(70,3)     // ~23⅓ – zaghaft
                       : (level == NORMAL) ? F(65,3)     // ~21⅔
                                           : F(61,3);    // ~20⅓ – aggressiv
    if (tarocks < 7) threshold += F(5,3);                // Tarockmangel-Aufschlag
    return v >= threshold;
}
```

Der Geber darf etwas aggressiver sein als die Vorhand, weil er sonst das einfache Spiel bekommt und in
`MS-ERW` nur 1 statt 3 Punkte gewinnen kann.

### 10.3 Die Strohmänner‑Heuristiken (das eigentliche Spiel)

Alle folgenden Regeln stammen wörtlich aus [MS‑WEB §Spieltaktische Hinweise] und sind als
Prioritätsliste direkt implementierbar.

**H1 – Eigene Strohmänner schnell abbauen.**
*„Es ist wichtig, dass du deine Strohmänner rasch abspielst, damit du volle Einsicht in alle deine Karten
gewinnst.“* → Bonus auf jeden Zug, der ein Deckblatt verbraucht; Gewichtung mit der Zahl der noch verdeckten
Karten.

**H2 – Den Gegner am Abbau hindern.**
*„Umgekehrt wird es fast immer günstig sein, den Gegner am Abspielen seiner Strohmänner zu hindern … Hat dein
Gegenspieler beispielsweise Herz und Karo als oberste Karten auf seinen Strohmännern aufliegen, dann wirst du
Pik oder Treff spielen, im Idealfall von deinen Strohmännern.“*
→ **Zwei Ziele in einem Zug:** spiele eine Farbe, in der der Gegner **kein** Deckblatt hat, und spiele sie
**von deinem eigenen Deckblatt**. Das ist der stärkste Einzelzug der ganzen Heuristik.

**H3 – Niemals in die Strohmann‑Falle laufen.**
*„Du solltest gegen Ende eines Spiels jedenfalls nie in die Situation kommen, dass dein Gegner schon alle
Strohmänner‑Karten aufgenommen hat, während du alle deine Handkarten verausgabt hast und deshalb gezwungen
bist, von deinen Strohmännern wegzuspielen.“*
→ Harte Nebenbedingung: `|Hand| > 0` solange `Σ Päckchenkarten > 0`. Verletzung mit hoher Strafe bewerten.

**H4 – Aufliegende Damen sofort wegspielen.**
*„Eine auf den Strohmännern aufliegende Dame solltest du sofort wegspielen, auch wenn du nicht den König im
Handblatt hast … Außerdem ist zu Beginn des Spiels die Wahrscheinlichkeit recht groß, dass der König in einem
der Strohmänner schläft und du daher die Dame heimbringst. (**In 4 von 10 Fällen** wird es dir gelingen.)“*
**Ausnahme:** *„Du wirst nur dann darauf verzichten, die aufliegende Dame wegzuspielen, wenn dein Gegner in
genau dieser Farbe **mindestens 2 niedrige Karten** aufliegen hat.“*
Sinngemäß gilt dasselbe für aufliegende **Cavalle**.

**H5 – Schlafender König erkannt? Farbe leerspielen.**
*„Spielst du eine aufliegende Dame aus und dein Gegner gibt eine niedrige Farbkarte zu, dann war also ein
schlafender König dafür verantwortlich. Jetzt heißt es, die restlichen Karten dieser Farbe rasch wegspielen.
Sollte dein Gegner den König später aus seinen Strohmännern aufnehmen, wirst du diesen König mit Tarock
stechen.“*

**H6 – Von zwei aufliegenden Karten einer Farbe die höhere spielen.**
*„Liegen Dame und Kavall oder Kavall und Bube einer Farbe auf zwei deiner Strohmänner auf, so spielst du in
der Regel die höhere Karte. Das gleiche gilt für Skartindeln.“*

**H7 – Figur aus der Hand, wenn nur Leerkarten aufliegen.**
*„Liegen bei dir zwei oder drei Skartindeln einer Farbe auf, von welcher du eine der höheren Figuren (König,
Dame oder Kavall) in den Handkarten besitzt, während dein Gegner **kein Blatt dieser Farbe** aufliegen hat,
so spiele die Figur aus den Handkarten.“*

**H8 – Reihenfolge König/Dame/Cavall.**
*„Hast du den König in der Hand und die Dame derselben Farbe aufliegen, so empfiehlt es sich, **zuvor die
Dame wegzuspielen**; sitzen beide Figuren in der Hand, während der Kavall derselben Farbe auf einem deiner
Päckchen liegt, so wird der Kavall rasch weggespielt.“*

**H9 – Bedienen: vom Deckblatt, wenn beides nur Leerkarten sind.**
*„Spielt dein Gegner eine Farbe an, von der du nur Skartindeln sowohl in der Hand als auf den Strohmännern
hast, so bedienst du **von den Strohmännern**. Liegt auf einem Päckchen der Bube dieser Farbe, und hat der
Gegner eine höhere Figur gebracht, so bedienst du **mit dem Buben**, selbst dann, wenn du auch ein oder zwei
Skartindeln in den Händen hast.“*

**H10 – Nicht auf dem Deckblatt sitzen bleiben.**
*„Schwache Strohmännerspieler lieben es, keine Karten von ihren Päckchen wegzuspielen, wenn sie in einer
Farbe blank sind. Sie erhoffen sich, dass der Gegner diese Farbe bringen wird … Aber wahrscheinlich wird der
Gegner misstrauisch sein.“* → Gegenmittel für die KI **als Gegner**: eine „problematische“ Farbe erst spielen,
wenn der Großteil der gegnerischen Päckchen abgebaut ist.

**H11 – Die Finte mit dem Cavall.**
*„Eine beliebte Finte besteht darin, den ausgespielten gegnerischen König mit dem Kavall zu bedienen, wenn man
neben diesem auch noch ein Skartindel im Handblatt besitzt … Dieses Manöver ist allerdings so durchsichtig,
dass es nur ein oder zwei Mal pro Partie angewendet werden kann.“*
→ Nur auf der Stufe „schwer“, und höchstens zweimal pro Partie (Zähler im KI‑Zustand).

### 10.4 Farbenspiel: „Nach‑hinten‑Spielen“

*„Ein Kriterium des Strohmann‑Tarock ist es, sich bei **nicht geschlossenen Farben nach hinten zu spielen**.
Ein Ass oder einen Zehner des Gegners wirst du nicht gleich mit dem König stechen, denn dieser könnte einen
fetteren Stich bringen … Du wirst also, wenn du nicht mit einer niederen Figur stechen kannst, den König
**schinden** und ein Skartindel zugeben. **Aber bedenke, dass zu Beginn des Spiels im Durchschnitt 3 Karten
einer Farbe in den Strohmännern liegen.** Du darfst daher anfangs die Taktik des Nach‑hinten‑Spielens auch
nicht übertreiben, ansonsten kann es passieren, dass dein König mit Tarock gestochen wird.“* [MS‑WEB]

Die Zahl „3 Karten je Farbe in den Strohmännern“ ist die wichtigste Faustregel der Eröffnungsphase:
24 Strohmannkarten, davon im Erwartungswert 24·(8/54) ≈ 3,6 pro Farbe.

**Der Zugzwang‑Grundsatz:**
*„Wer als erster beginnt, die umkämpfte Farbe zu spielen, wird am Ende eine schlechtere Punktebilanz in
dieser Farbe aufweisen.“* → Die KI bewertet jede Farbe als `contested` (etwa gleich verteilt) oder `solid`
(eigene Oberhand) und zieht **contested** Farben so spät wie möglich an.

*„Natürlich bleibt es dem Geschick des Spielers überlassen, welche von mehreren umkämpften Farben er durch
frühzeitiges Anziehen aufgibt. Die Kunst des Strohmandelns besteht darin, in diesen Fällen die richtigen
Prioritäten zu setzen.“*

### 10.5 Tarockspiel

*„Jener Spieler, der in Tarock stärker ist, wird zwischendurch **tarockieren**, ehe er eine nicht geschlossene
Farbe anzieht. … Du ziehst daher immer von einer **geschlossenen Reihe** Tarock an, egal von welcher Höhe.
Hast du III, VI, IX, XIV, XV, XVI, XVII, XX und Sküs, so spielst du die geschlossene Reihe Tarock XIV bis
Tarock XVIII. Findet sich in deinen Tarock kein Schluss, weil du beispielsweise V, VII, XI, XVI, XVIII hast,
so spielst du eines der **niederen** Tarock, um mit den höheren später Stiche zu machen.“* [MS‑WEB]

*(Hinweis: Das Beispiel im Original ist unsauber – „XIV bis XVIII“ nennt Karten, die im aufgezählten Blatt
nicht alle vorkommen; gemeint ist die längste zusammenhängende eigene Tarockreihe.)*

Implementierung: `longestClosedRun(myTarocks, unseenTarocks)` – die längste Folge, in der kein
ungesehener Tarock dazwischenliegt. Von dieser Reihe **von oben** abziehen.

### 10.6 Pagat: fangen oder heimbringen

*„Nehmen wir an, du hast **mehr als 11 Tarock** im Blatt, darunter wahrscheinlich eine längere geschlossene
Reihe, doch es fehlt dir der Pagat. In diesem Fall wirst du versuchen, auf den Pagat Jagd zu machen …
Besitzt du selbst den Pagat, neben nur wenigen Tarock, sagen wir **7 oder noch weniger**, so versäume es
nicht, ihn bei nächster Gelegenheit **einzustechen**, um dann das Hauptaugenmerk auf das Farbenspiel richten
zu können.“* [MS‑WEB]

Zum **stillen Pagat ultimo**:

*„Nur wenn du nach dem ersten Aufdecken der Strohmänner mehr als die Hälfte aller im Spiel befindlichen
Tarock, also **mehr als 11 Tarock**, in deinem Handblatt hast, lohnt es sich, auf diese stille Prämie zu
spekulieren. Aber auch dann wirst du mit Austarockieren nur in den seltensten Fällen zum Ziel kommen. **Du
brauchst zusätzlich zumindest eine lange Farbe**, mit der du dem Gegner Tarock ziehst.“* [MS‑WEB]

Gegenmaßnahme (nur, wenn das Spiel ohnehin verloren ist!):

*„Gibt es Anzeichen, dass dein Gegner den stillen Pagat ultimo spielen will, kann es sich lohnen, das
Abspielen der Strohmänner **hinauszuzögern**. Findest du in den letzten Karten der Strohmänner einige Tarock,
kann es dir unter Umständen gelingen, den Pagat abzufangen … Allerdings empfiehlt sich diese Taktik nur dann,
wenn das Spiel bereits verloren ist. **Wird mit Grand Point gespielt, ist davon überhaupt abzuraten.“*
[MS‑WEB]

Das ist eine direkt implementierbare Bedingung:
`if (estimatedOwnPoints < 30 && !grandPointActive && oppLikelyPagatUltimo) delayStrawmen();`

### 10.7 Mondfang, Rostopschin, Endspiel

**Mondfang‑Abwehr:** *„Du behältst dir ein oder zwei **kleine Tarock**, um den Gegner in den Stich zu
bringen. Dieser spielt dann eine Tarock nach, die du mit dem Mond stichst.“* [MS‑WEB]

**`II` und `III` aufheben:** *„Routinierte Strohmann‑Tarockierer heben sich Tarock II und Tarock III ohnedies
auf, nicht nur deshalb, um beim Gegner Tarock zu lösen, sondern auch für den Fall, dass der stille Uhu oder
der stille Kakadu spielbar sind.“* [MS‑WEB]

**Rostopschin:** Die KI ruft „Ross!“ nur, wenn nach PIMC‑Sampling in ≥ 80 % der Welten **beide** Stiche
gewinnbar sind (es kostet nichts, ihn nicht zu rufen – §5.5).

**Mondopfer (Stufe „schwer“):** *„Sind bereits alle hohen Tarock bis auf den Sküs gefallen und es ist dir
bereits klar, dass du das Spiel gewonnen hast, lohnt sich ein **Mondopfer**. Du spielst den Mond aus und
hoffst, dass dein Gegner mit dem Sküs sticht … Natürlich darfst du die Taktik des Mondopfers nur dann
anwenden, wenn du bereits den Pagat heimgebracht hast, denn sonst würde dein Gegner die stille Trull
gewinnen.“* [MS‑WEB]

### 10.8 Architekturempfehlung und Stufen

| Stufe | Verfahren | Stärke/Ziel |
|---|---|---|
| **Anfänger** | reine Regelheuristik H1–H9, keine Inferenz, zufällige Fehler mit p = 0,15 | verliert freundlich, spielt aber nie illegal |
| **Normal** | Heuristik + Void‑Inferenz + Kartenzählung; Aufnahmeschwelle 21⅔ | etwa Clubniveau |
| **Schwer** | PIMC: 40 Welten × Alpha‑Beta über das Restspiel (ab Stich 15 exakt), Heuristik als Zugordnung; H10/H11 aktiv; Mondopfer | soll den Autor schlagen |

**Budget Sailfish/Jolla C2 (Referenzgerät):** 250 ms pro Zug, 1 Thread. Ab Stich 18 ist das Restspiel
(≤ 10 Karten je Seite) mit Alpha‑Beta + Transpositionstabelle **exakt** lösbar; davor Sampling.
**Wichtig:** Die eigenen verdeckten Päckchenkarten müssen beim Sampling **mitgezogen** werden – die KI kennt
ihr eigenes Blatt nicht vollständig. Das unterscheidet Strohmandeln von jedem anderen Tarock und ist die
häufigste Fehlerquelle bei der Portierung einer Königrufen‑KI.

### 10.9 Bewertungsfunktion (Blattbewertung im Spiel)

```
score = 3·(eigene Kartenpunkte − 35⅔)                      // Hauptziel
      + 4·P(Trull in den Stichen)  + 4·P(vier Könige)
      + 3·P(Pagat ultimo)          − 3·P(Pagat gefangen)
      + 2·P(Grammel-Punkt)
      + 1.5·(eigene sichere Stecher)                        // §10.2
      + 1.0·(gegnerische verdeckte Päckchenkarten)          // H2: Unsicherheit beim Gegner ist gut
      − 1.0·(eigene verdeckte Päckchenkarten)               // H1
      − 8·[eigene Hand leer, aber Päckchen noch voll]       // H3, harte Strafe
```

---

## 11. Lernmodus

Alle Texte dieses Kapitels sind **Auslieferungstexte**: Sie gehen wörtlich so in die App (Datei
`i18n/de/learn_strohmandeln.ts`). Sprachstil: Du‑Form, kurze Sätze, keine Fachbegriffe ohne Erklärung, jeder
Fachbegriff beim ersten Auftreten **fett** und im Glossar (§11.6).

### 11.1 Was ist Strohmandeln? (Einstiegstext)

> **Strohmandeln ist Tarock für zwei.**
>
> Tarock spielt man sonst zu dritt oder zu viert. Strohmandeln ist die alte österreichische Art, es zu zweit
> zu spielen – und es ist kein Notbehelf, sondern ein eigenständiges, sehr feines Spiel.
>
> Das Besondere: Ein Teil deiner eigenen Karten ist **dir selbst noch unbekannt**. Vor dir liegen drei kleine
> Stapel, die **Strohmänner**. Von jedem siehst du nur die oberste Karte. Was darunter liegt, weißt du nicht –
> aber es gehört dir.
>
> Und umgekehrt: Du siehst auch die obersten Karten deines Gegners. Du weißt also ein Stück weit, was er hat,
> und er weiß ein Stück weit, was du hast. Genau daraus entsteht der Reiz.
>
> **Das Ziel:** Du willst in deinen Stichen mehr als die Hälfte aller Punkte sammeln. Das ganze Spiel hat
> 70 Punkte. Du brauchst **36** davon.

### 11.2 Die Phasen des Spiels, einzeln erklärt

#### Phase 1 – Geben

> Es wird mit dem vollen Tarockblatt zu **54 Karten** gespielt – dem gleichen Blatt wie beim Königrufen.
> Nichts wird weggelegt.
>
> Der Geber gibt jedem **dreimal fünf Karten**. Das sind deine **Handkarten**, 15 Stück.
> Die restlichen 24 Karten legt er als **sechs Päckchen zu je vier Karten** auf den Tisch – drei vor dich,
> drei vor deinen Gegner. Das sind die **Strohmänner**.
>
> Du hast also insgesamt 27 Karten: 15 in der Hand, 12 auf dem Tisch.
>
> **Sonderfall:** Hast du in deinen 15 Handkarten **kein einziges Tarock**, darfst du „zusammenwerfen“ –
> dann wird neu gegeben, und zwar von demselben Geber.

#### Phase 2 – Aufnehmen oder passen

> Jetzt musst du dich entscheiden, **bevor** irgendeine Strohmann‑Karte umgedreht wird.
>
> * **„Ich nehme auf!“** – Du erklärst, dass du mehr als die Hälfte der Punkte machen willst.
>   Gelingt es: **+3**. Misslingt es: **dein Gegner bekommt 4**.
> * **„Ich passe!“** (oder „Weiter!“) – Du willst nicht.
>
> Die **Vorhand** (das ist der Spieler, der *nicht* gegeben hat) entscheidet zuerst. Passt sie, darf der Geber
> aufnehmen. Passen beide, wird trotzdem gespielt – dann heißt es **einfaches Spiel**, und wer gewinnt,
> bekommt nur **1** Punkt. Es gibt kein Bieten und kein Überbieten: Sobald einer aufgenommen hat, ist die
> Sache entschieden.
>
> **Warum ist das schwer?** Weil du erst 15 von deinen 27 Karten kennst. Fast jede zweite Karte, die dir
> gehört, hast du noch nie gesehen.

#### Phase 3 – Die Strohmänner aufdecken

> Jetzt drehst du die oberste Karte deines **linken** Päckchens um, so dass dein Gegner sie sehen kann.
>
> * Ist es ein **Tarock** oder ein **König** → du nimmst sie auf die Hand und drehst gleich die nächste um.
> * Ist es irgendeine **andere Farbkarte** (Dame, Cavall, Bube, Zahlkarte) → sie **bleibt offen liegen**.
>   Diese offene Karte heißt **Deckblatt**.
>
> Dasselbe machst du mit dem mittleren und dem rechten Päckchen. Danach ist dein Gegner dran – oder in der
> App gleichzeitig.
>
> Es kann gut sein, dass du zwei Tarock und einen König bekommst und dein Gegner gar nichts. Das ist normal
> und gehört dazu.
>
> **Merke:** Damen bleiben liegen! Nur Könige und Tarock wandern in die Hand.

#### Phase 4 – Spielen

> Die **Vorhand** spielt die erste Karte. Danach spielt immer der aus, der den letzten Stich gewonnen hat.
>
> Du darfst spielen:
> * jede Karte aus deiner **Hand**, und
> * jedes **Deckblatt**, also jede offen aufliegende oberste Strohmann‑Karte.
>
> Die Deckblätter sind vollwertige Karten. Du kannst mit ihnen ausspielen, stechen und zugeben.
>
> **Sobald du ein Deckblatt gespielt hast**, drehst du – nach dem Stich – die nächste Karte dieses Päckchens
> um. Ist es wieder ein Tarock oder ein König, nimmst du sie auf und drehst weiter.
>
> **Die letzte Karte eines Päckchens** nimmst du **verdeckt** auf die Hand – dein Gegner sieht sie **nicht**.
> Das ist ein kleiner, aber wertvoller Vorteil.

#### Phase 5 – Zählen

> Alle 27 Stiche werden ausgespielt. Dann zählt jeder seine Stichkarten.
>
> **Die Kartenwerte:**
>
> | Karte | Punkte |
> |---|---|
> | Sküs, Mond (XXI), Pagat (I), jeder König | **5** |
> | Dame | **4** |
> | Cavall (Reiter) | **3** |
> | Bube | **2** |
> | alle übrigen Tarock und alle Zahlkarten | **1** |
>
> **So wird gezählt:** Leg deine Stichkarten in Stapeln zu **drei** Karten aus. Zähle die Punkte jedes Stapels
> zusammen und **zieh 2 ab**. Von einem unvollständigen Reststapel ziehst du **1 ab**.
>
> *Beispiel:* König + Cavall + XIII = 5 + 3 + 1 = 9, minus 2 = **7 Punkte**.
>
> So kommt man auf genau **70 Punkte** im ganzen Spiel. Du brauchst **36**.

### 11.3 Wann nehme ich auf? (Entscheidungshilfe)

> **Zähl deine sicheren Stecher.** Das sind Karten, mit denen du ziemlich sicher einen Stich machst:
>
> * **Sküs** und **Mond (XXI)** – immer sicher.
> * Der **Pagat (I)** – gilt hier als sicher, weil du ihn meistens heimbringst.
> * Jeder **König**.
> * Eine **Dame**, wenn du den König derselben Farbe auch hast – oder wenn du von dieser Farbe mindestens
>   drei Karten hast.
>
> **Und jetzt rechne in „Bruchpunkten“:**
> Sküs / Mond / Pagat / König = **4 ⅓** · Dame = **3 ⅓** · Cavall = **2 ⅓**
>
> | Summe | Entscheidung |
> |---|---|
> | unter 20 | **passen** |
> | 20 – 22 | riskant – nur mit mindestens 7 Tarock |
> | 23 – 25 | **aufnehmen** |
> | über 25 | **unbedingt aufnehmen** |
>
> **Die zweite Bedingung:** Du brauchst **mindestens 6 bis 7 Tarock** in der Hand. Wer mit zwei oder drei
> Tarock aufnimmt, dem stechen die Farbkönige weg.
>
> **Beispiel A:** Sküs, Mond, ♥K, ♥D und dreimal Treff mit der Dame
> → 4⅓ + 4⅓ + 4⅓ + 3⅓ + 3⅓ = **19 ⅔**. Das ist **knapp zu wenig** – wer mutig ist, nimmt trotzdem auf und
> hofft auf gute Strohmänner.
>
> **Beispiel B:** Sküs, Pagat, ♠K, ♠D, ♣K und dreimal Herz mit der Dame
> → 4⅓ + 4⅓ + 4⅓ + 3⅓ + 4⅓ + 3⅓ = **24**. Das ist ein klares **Aufnehmen**.
>
> **Und wenn ich passe?** Dann gibt es entweder ein **einfaches Spiel** (nur 1 Punkt wert, aber auch
> risikolos) – oder dein Gegner nimmt auf, und du spielst gegen ihn. Auch dabei kannst du gewinnen: wenn er
> scheitert, bekommst **du 4**.

### 11.4 „Warum ist diese Karte nicht erlaubt?“ – die exakten Meldungen

Die App blockiert unzulässige Karten. Tippt der Spieler dennoch darauf, erscheint **genau** einer der
folgenden Texte als Kurzhinweis (Toast, 3 s) mit optionalem „Mehr erfahren“. Platzhalter in `{}`.

| # | Auslöser | Text |
|---|---|---|
| **E1** | Farbzwang, Farbe ist in der **Hand** vorhanden | **„{Farbe} musst du bedienen.“**<br>*Mehr:* „Du hast noch {Karten} in der Hand. Wenn du die angespielte Farbe hast, musst du sie auch spielen – auch dann, wenn du den Stich damit verlierst.“ |
| **E2** | Farbzwang, Farbe liegt **nur als Deckblatt** auf | **„{Farbe} musst du bedienen – vom Strohmann.“**<br>*Mehr:* „Auf deinem {n}. Strohmann liegt {Karte} offen. Offen aufliegende Karten gehören zu deinem Blatt wie Handkarten. Du hast diese Farbe also.“ |
| **E3** | Farbzwang, Farbe in Hand **und** als Deckblatt | *(kein Fehler – beide Quellen werden grün markiert)*<br>Hinweis: „Du kannst {Karte aus der Hand} spielen **oder** das Deckblatt {Karte}. Beides ist erlaubt.“ |
| **E4** | Tarockzwang (Farbe fehlt, Tarock vorhanden) | **„Du hast kein {Farbe} – dann musst du Tarock spielen.“**<br>*Mehr:* „Wer die angespielte Farbe nicht hat, muss stechen. Das nennt man Tarockzwang. Du darfst dir aussuchen, welches Tarock – auch ein niedriges.“ |
| **E5** | Tarock wurde angespielt | **„Auf Tarock musst du Tarock zugeben.“**<br>*Mehr:* „Tarock ist hier wie eine fünfte Farbe. Solange du ein Tarock hast, musst du eines zulegen.“ |
| **E6** | Versuch, eine **verdeckte** Strohmann‑Karte zu spielen | **„Diese Karte liegt verdeckt.“**<br>*Mehr:* „Nur die oberste, offen liegende Karte eines Strohmanns kannst du spielen. Was darunter liegt, kommt erst zum Vorschein, wenn du das Deckblatt gespielt hast.“ |
| **E7** | Versuch, ein **gegnerisches** Deckblatt zu spielen | **„Das ist ein Strohmann deines Gegners.“** |
| **E8** | Spieler ist nicht am Zug | **„Dein Gegner ist am Zug.“** |
| **E9** | Trischaken‑Profil, Stichzwang verletzt | **„Beim Trischaken musst du den Stich nehmen, wenn du kannst.“**<br>*Mehr:* „{Karten} würden den Stich gewinnen. Eine davon musst du spielen – **welche, darfst du dir aussuchen**; du musst nicht die höchste nehmen.“ |
| **E9b** | Trischaken‑Profil, Pagat gespielt, obwohl noch anderes Tarock da ist | **„Den Pagat darfst du erst als letztes Tarock spielen.“**<br>*Mehr:* „Du hast noch {n} andere Tarock: {Karten}. Diese Regel ist stärker als der Stichzwang – könntest du nur mit dem Pagat überstechen, gibst du den Stich lieber her.“ |
| **E10** | Ansage zu spät (Profil `MS-KLASS`) | **„Zu spät für die Ansage {Trull / Vier Könige}.“**<br>*Mehr:* „Du hast schon {Karte} gespielt. Die Ansage muss spätestens dann erfolgen, wenn du die **erste** Karte dieser Gruppe spielst.“ |
| **E11** | Ansage ohne die nötigen Karten | **„Dafür fehlt dir {Karte}.“** |
| **E12** | „Topschin!“ ohne vorheriges „Ross!“ | **„Zuerst musst du die XVII mit ‚Ross!‘ spielen und diesen Stich gewinnen.“** |
| **E13** | „Ross!“, aber die XVII ist gerade nicht spielbar | **„Die XVII kannst du jetzt nicht spielen.“** |
| **E14** | Kontra, aber schon ausgespielt (Profil mit Kontra) | **„Kontra geht nur vor dem ersten Ausspiel.“** |
| **E15** | Zusammenwerfen, obwohl Tarock in der Hand | **„Zusammenwerfen darfst du nur ohne jedes Tarock.“**<br>*Mehr:* „Du hast {n} Tarock in der Hand: {Karten}.“ |

**Positive Hinweise** (kein Fehler, sondern Coaching – nur im Lernmodus, abschaltbar):

| # | Situation | Text |
|---|---|---|
| **C1** | Spieler will den Pagat zugeben, obwohl ein höheres Tarock verfügbar ist | „**Vorsicht:** Der Pagat ist das **niedrigste** Tarock. Er verliert gegen jedes andere Tarock – und er ist 5 Punkte wert.“ |
| **C2** | Spieler kann ein Deckblatt spielen, wählt aber eine Handkarte | „Tipp: Spiel lieber vom Strohmann. Je schneller die Päckchen weg sind, desto besser kennst du dein eigenes Blatt.“ |
| **C3** | Eine Dame liegt als Deckblatt auf und der Gegner hat in dieser Farbe höchstens ein niedriges Deckblatt | „Tipp: Spiel die aufliegende **Dame** – in vier von zehn Fällen schläft der König noch in einem Strohmann.“ |
| **C4** | Alle Handkarten wären verbraucht, Päckchen aber noch voll | „**Achtung:** Wenn du keine Handkarten mehr hast, musst du alles von den Strohmännern spielen – und dein Gegner sieht jede deiner Karten im Voraus.“ |
| **C5** | Spieler hat noch `II` oder `III` und es sind ≤ 3 Stiche übrig | „Tipp: Mit der II kannst du im **vorletzten** Stich den Uhu machen, mit der III im drittletzten den Kakadu.“ |

### 11.5 Typische Anfängerfehler

> **1. Zu früh aufnehmen.**
> Du hast sechs Tarock und den Sküs – das fühlt sich gut an, ist aber zu wenig. Zähl die **sicheren Stecher**
> (§11.3). Unter 20 Punkten: passen. Ein verlorenes aufgenommenes Spiel kostet 4, ein gewonnenes bringt nur 3.
>
> **2. Auf den Strohmännern sitzen bleiben.**
> Der häufigste Fehler überhaupt. Wer seine Deckblätter nicht wegspielt, kennt sein eigenes Blatt nicht und
> steht am Ende mit lauter offenen Karten da, die der Gegner abzählen kann. **Spiel deine Strohmänner rasch
> ab.**
>
> **3. Die aufliegende Dame aufheben.**
> „Ich warte, bis ich den König habe“ – den bekommst du meistens nicht. Spiel die Dame früh: Solange noch
> viele Karten verdeckt sind, schläft der König oft selbst in einem Strohmann.
>
> **4. Den Pagat zu lange behalten.**
> Der Pagat ultimo ist reizvoll, aber selten. Wenn du **7 oder weniger Tarock** hast: Stich ihn bei der
> nächsten guten Gelegenheit ein und sichere dir die 5 Punkte.
>
> **5. Den Pagat zum Stechen verwenden.**
> Wer mit dem Pagat sticht, gewinnt den Stich zwar (jedes Tarock schlägt jede Farbkarte) – aber dann ist er
> weg, und der Pagat ultimo ist dahin. Nimm ein kleines, wertloses Tarock.
>
> **6. Die umkämpfte Farbe zuerst anspielen.**
> Wer eine Farbe zuerst anzieht, in der beide etwa gleich stark sind, verliert dort meistens die
> Punktebilanz. Warte ab, bis der Gegner anfangen muss.
>
> **7. Alle Handkarten verausgaben.**
> Sorg dafür, dass du bis zum Schluss wenigstens eine Handkarte behältst. Sonst diktiert dein Gegner das
> Endspiel.
>
> **8. Den Mond ungeschützt ausspielen.**
> Solange der Gegner den Sküs haben kann, ist der Mond gefährdet. Behalt dir ein oder zwei **kleine** Tarock,
> damit der Gegner ausspielen muss – dann stichst du mit dem Mond, statt ihn herzuschenken.
>
> **9. Vergessen, dass verdeckte Karten nicht zählen.**
> Ein König unter deinem Deckblatt macht dich **nicht** bedienpflichtig. Und umgekehrt: Dein Gegner kann in
> einer Farbe „blank“ wirken und sie zwei Stiche später doch haben, weil er sie aus dem Päckchen bekommen hat.
>
> **10. Die letzte Päckchenkarte verschenken.**
> Sie kommt **verdeckt** auf deine Hand. Dein Gegner weiß nicht, was es ist. Merk sie dir – das ist dein
> einziger Informationsvorsprung.

### 11.6 Glossar

| Begriff | Erklärung |
|---|---|
| **Aufnehmen** | Erklären, dass man das Spiel gewinnen will („Ich nehme auf!“). Bringt 3, kostet 4. |
| **Aufnehmer** | Der Spieler, der aufgenommen hat. |
| **Blatt** | (1) Die Karten, die man zur Verfügung hat: Handkarten **und** Deckblätter. (2) Beim Zählen: eine übrig gebliebene Leerkarte („35 Punkte und 2 Blatt“). |
| **Cavall** | Die Reiterkarte zwischen Dame und Bube. 3 Punkte. Auch *Reiter* oder *Chevalier*. |
| **Deckblatt** | Die offen liegende oberste Karte eines Strohmanns. Sie gehört zum Blatt und wird mitgespielt. |
| **Dreierlagen** | Die österreichische Zählweise: Karten zu dritt stapeln, Punkte addieren, 2 abziehen. |
| **Einfaches Spiel** | Das Spiel, das gespielt wird, wenn beide gepasst haben. Nur 1 Punkt wert. |
| **Farbzwang** | Man muss die angespielte Farbe bedienen, wenn man sie hat. |
| **Fehlfarbe** | Eine Farbe, die man nicht hat. |
| **Geber** | Wer gegeben hat. Er erklärt sich als Zweiter. |
| **Grammel‑Punkt** | Zusatzpunkt für mindestens 45 Kartenpunkte. Auch *Grand Point*. |
| **Kakadu** | Prämie: den drittletzten Stich mit dem Tarock III gewinnen. |
| **Königstrull** | Alle vier Könige. Auch *Vier Könige*. |
| **Leerkarte** | Jede Karte, die nur 1 Punkt zählt: Tarock II–XX und alle Zahlkarten. Auch *Skartindel*. |
| **Mond** | Das Tarock XXI, das zweithöchste. 5 Punkte. |
| **Mondfang** | Den Mond mit dem Sküs stechen – die einzige Art, ihn zu fangen. |
| **Nachwassern** | Die eigenen Stiche während des Spiels noch einmal ansehen. Erlaubt, gilt aber als unfein. |
| **Nullrunde** | Die nächste Runde (2 Spiele) zählt doppelt, weil ein Spiel unentschieden oder die Abrechnung 0:0 war. |
| **Pagat** | Das Tarock I, das niedrigste. 5 Punkte. Auch *Spatz*. |
| **Pagat ultimo** | Prämie: den letzten Stich mit dem Pagat gewinnen. |
| **Renonce** | Regelverstoß beim Bedienen. Der Gegner gewinnt Spiel und Prämien. |
| **Rostopschin** | Prämie: zwei Stiche hintereinander mit XVII und XVIII, dazu „Ross!“ und „Topschin!“ rufen. |
| **Runde** | Zwei Spiele – eines, in dem ich gebe, und eines, in dem du gibst. |
| **Sküs** | Die Narrenkarte. Hier ist sie schlicht das **höchste Tarock** – kein Sonderfall wie im französischen Tarot. 5 Punkte. |
| **Stich** | Zwei Karten, eine von jedem. Wer die höhere spielt, nimmt sie. |
| **Strohmann** | Eines der drei Päckchen zu vier Karten vor einem Spieler. Der Name kommt von einer viel älteren Fassung des Spiels (um 1829), in der ein Kartenstapel als „Strohmann“ – als stummer Mitspieler – fungierte. |
| **Tarock** | (1) Die 22 Trumpfkarten. (2) Das Spiel selbst. |
| **Tarockzwang** | Wer die angespielte Farbe nicht hat, muss ein Tarock spielen. |
| **Trischaken** | Ein „Negativspiel“: Wer **weniger** Punkte macht, gewinnt. Nur in manchen Regelwerken. |
| **Trull** | Die drei Fünfer‑Tarock Sküs, Mond und Pagat zusammen. |
| **Uhu** | Prämie: den vorletzten Stich mit dem Tarock II gewinnen. |
| **Valat** | Alle 27 Stiche. 12 Punkte. |
| **Vorhand** | Wer **nicht** gegeben hat. Sie erklärt sich zuerst und spielt die erste Karte aus. |
| **Zählkarte** | Jede Karte, die mehr als 1 Punkt zählt. |
| **Zusammenwerfen** | Neugabe verlangen, weil man kein einziges Tarock in der Hand hat. |

### 11.7 Drei Übungshände

Die drei Hände sind **feste Gaben** (deterministisch, kein Zufall). Sie werden in der App als geführte
Szenarien gespielt: Der Lernende sieht den Text, tippt die vorgesehene Karte, und nur diese ist aktiv;
tippt er woanders hin, erscheint der passende Text aus §11.4. Alle Zahlen unten sind nachgerechnet.

Datenformat (`learn/strohmandeln/hand1.json` usw.):

```json
{ "id": "stroh-1", "profile": "AT-STROH-MS-ERW", "dealer": "seat1",
  "seat0": { "hand": ["SKUES","T1","T18","T15","T12","T8","T4","SK","SD","S8","CK","C9","HD","H3","H4"],
             "piles": [["DK","T20","SB","D2"], ["CD","T11","HA","D4"], ["C10","T6","HK","S7"]] },
  "seat1": { "hand": ["T21","T19","T17","T14","T10","T7","T3","DD","DC","DA","HC","HB","CC","CB","S10"],
             "piles": [["SC","T16","T13","D3"], ["DB","T9","T5","C8"], ["H2","T2","S9","C7"]] },
  "steps": [ … ] }
```

---

#### Übungshand 1 — „Aufnehmen oder passen?“ und die erste Aufdeckung

**Lernziel:** Blattbewertung vor der Erklärung; was beim Aufdecken passiert; warum man aufliegende Damen
sofort spielt.

**Du bist Vorhand.** Deine 15 Handkarten:

> **Tarock:** `Sküs` `XVIII` `XV` `XII` `VIII` `IIII` `I (Pagat)`
> **♠ Pik:** `K` `D` `8` **♣ Treff:** `K` `9` **♥ Herz:** `D` `3` `4`

*(Der Gegner hat: `XXI` `XIX` `XVII` `XIV` `X` `VII` `III`, `♦D ♦C ♦A`, `♥C ♥B`, `♣C ♣B`, `♠10` — im
Lernmodus zunächst verdeckt, am Ende zur Nachbesprechung aufdeckbar.)*

**Schritt 1 — Text:**
> „Schau dir dein Blatt an. Zähl die **sicheren Stecher**:
> Sküs 4⅓, Pagat 4⅓, ♠K 4⅓, ♠D 3⅓ (du hast den König dazu), ♣K 4⅓, ♥D 3⅓ (du hast drei Herz).
> **Summe: 24.** Und du hast **7 Tarock**.
> Das ist ein klares Blatt zum Aufnehmen.“
> **Aktion:** Button **„Ich nehme auf!“**

**Schritt 2 — Text:**
> „Jetzt werden die Strohmänner aufgedeckt. Sieh zu.“
>
> * **1. Päckchen:** `♦K` → **König**, kommt auf die Hand. Nächste Karte `XX` → **Tarock**, kommt auf die
>   Hand. Nächste Karte `♠B` → Bube, **bleibt offen liegen**.
> * **2. Päckchen:** `♣D` → Dame, **bleibt offen liegen**. *(Damen werden nicht aufgenommen!)*
> * **3. Päckchen:** `♣10` → Zahlkarte, **bleibt offen liegen**.
>
> „Du hast zwei Karten dazubekommen – den ♦König und das XX. Jetzt hast du **17 Handkarten** und drei
> Deckblätter: **♠B**, **♣D**, **♣10**.
>
> Dein Gegner hatte weniger Glück: Bei ihm liegen oben `♠C`, `♦B` und `♥2` – **kein einziges Tarock, kein
> König**. Er bekommt vorerst gar nichts dazu. Das kommt öfter vor, als man denkt.“

**Schritt 3 — Stich 1. Text:**
> „Du spielst aus. **Spiel die ♣Dame von deinem Strohmann.**
> Drei Gründe:
> 1. Du hast den **♣König** in der Hand – erst die Dame, dann den König (sonst bleibt die Dame hängen).
> 2. Der Gegner hat kein Treff aufliegen; er muss aus der Hand bedienen.
> 3. Du baust deinen Strohmann ab und siehst, was darunter liegt.“
>
> **Aktion:** `♣D` (Deckblatt Päckchen 2).
> **Gegner legt:** `♣B`. **Du gewinnst den Stich** (4 + 2 = 6 Rohpunkte).
> **Danach wird nachgedeckt:** unter der ♣Dame liegt `XI` → **Tarock** → auf die Hand. Darunter `♥A` → bleibt
> offen liegen.

**Schritt 4 — Stich 2. Text:**
> „Jetzt der **♣König** aus der Hand. Der Gegner hat noch Treff (den Cavall) und **muss bedienen** – er kann
> nicht mit Tarock stechen. Ein sicherer, fetter Stich.“
>
> **Aktion:** `♣K`. **Gegner legt:** `♣C`. **Du gewinnst** (5 + 3 = 8 Rohpunkte).
>
> *(Hinweis für Fortgeschrittene: Dein `♣9` spielst du **nicht** nach – der Gegner ist jetzt in Treff blank
> und würde stechen.)*

**Schritt 5 — Stich 3. Text:**
> „Jetzt gibst du absichtlich einen kleinen Stich her: **Spiel das ♥Ass vom Strohmann.**
> Du wirst ihn verlieren – aber danach ist dieses Päckchen leer, und die letzte Karte kommt **verdeckt** auf
> deine Hand. Dein Gegner weiß nicht, was du bekommst. Genau das ist der Deal in Strohmandeln: ein paar
> Punkte gegen Wissen.“
>
> **Aktion:** `♥A` (Deckblatt). **Gegner legt:** `♥B` – der Bube ist in Rot höher als das Ass, **der Gegner
> gewinnt** (1 + 2 = 3 Rohpunkte).
> **Danach:** Im 2. Päckchen liegt nur noch eine Karte (`♦4`). Sie kommt **verdeckt** auf deine Hand. Das
> Päckchen ist weg.

**Schlussbild:**
> „**Stand nach drei Stichen:** Du 11 ⅓ Punkte, Gegner 1 ⅔.
> Von deinen drei Strohmännern ist einer schon weg, und du hast vier Karten dazugewonnen, die dein Gegner
> nur teilweise kennt. So sieht ein guter Start aus.“

*(Rechenprobe: Du hast 4 Stichkarten mit Rohwert 4+2+5+3 = 14 → 14 − ⅔·4 = 11 ⅓.
Gegner 2 Karten mit 1+2 = 3 → 3 − ⅔·2 = 1 ⅔.)*

---

#### Übungshand 2 — „Die vier Zwänge“

**Lernziel:** Farbzwang aus der Hand, Farbzwang vom Deckblatt, freie Wahl zwischen beiden, Tarockzwang,
freies Abwerfen. Jede der Meldungen E1, E2, E3, E4, E5 kommt genau einmal vor.

**Feste Stellung, sechs Stiche vor Schluss.** Der Gegner ist am Ausspiel.

> **Du hast (6 Karten):**
> Hand: `♠B` `XVIII` `VIII` `♦A`
> 1 Strohmann mit 2 Karten, Deckblatt **`♦3`**
>
> **Dein Gegner hat (6 Karten, alle in der Hand, seine Strohmänner sind aufgebraucht):**
> `♠D` `♦K` `♥K` `♣K` `XXI` `♥C`

**Stich 1 — Gegner spielt `♠D`.**
> **Erlaubt ist nur:** `♠B`.
> Text: „**Pik musst du bedienen.** Du hast den ♠Buben in der Hand – und nur den. Dass unter deinem Deckblatt
> vielleicht noch Pik liegt, spielt keine Rolle: **verdeckte Karten zählen nicht.**“
> *(Tippt der Spieler auf XVIII → **E1**.)*
> Gegner gewinnt.

**Stich 2 — Gegner spielt `♦K`.**
> **Erlaubt sind zwei Karten:** `♦A` (Hand) **oder** `♦3` (Deckblatt).
> Text: „Karo hast du **zweimal**: einmal in der Hand, einmal als Deckblatt. **Beides ist erlaubt.**
> Nimm das **Deckblatt `♦3`** – so baust du deinen Strohmann ab, und die Karte darunter kommt dir zugute.“
> *(Beide Karten werden grün markiert → **E3**.)*
> **Aktion:** `♦3`. Gegner gewinnt (der König ist höher).
> **Danach:** Im Päckchen liegt nur noch **eine** Karte. Sie kommt **verdeckt** auf deine Hand – es ist die
> `♥4`. Dein Päckchen ist weg.

**Stich 3 — Gegner spielt `♥K`.**
> **Erlaubt ist nur:** `♥4`.
> Text: „**Herz musst du bedienen.** Die ♥4 hast du gerade eben verdeckt aufgenommen – jetzt musst du sie
> hergeben. Auch das gehört zum Spiel.“
> *(Tippt der Spieler auf VIII → **E1**.)*
> Gegner gewinnt.

**Stich 4 — Gegner spielt `♣K`.**
> **Erlaubt sind:** `XVIII` oder `VIII`.
> Text: „**Du hast kein Treff – dann musst du Tarock spielen.** Das nennt man **Tarockzwang**.
> Welches Tarock, darfst du dir aussuchen: **Nimm das kleinere, die VIII.** Sie reicht völlig –
> **jedes** Tarock schlägt **jede** Farbkarte, auch der Pagat.“
> *(Tippt der Spieler auf `♦A` → **E4**.)*
> **Aktion:** `VIII`. **Du gewinnst** (5 + 1 = 6 Rohpunkte) – ein Königsstich für ein wertloses Tarock.

**Stich 5 — du spielst aus.**
> Text: „Du hast noch `XVIII` und `♦A`. Spiel das `XVIII` – vielleicht ist der Mond schon weg.“
> **Aktion:** `XVIII`. **Gegner legt:** `XXI` (Mond) – er **musste** Tarock bedienen und hatte nur den Mond.
> Er gewinnt.
> *(Nachbesprechung: „**Auf Tarock muss Tarock zugegeben werden** – Tarock ist hier wie eine fünfte Farbe.
> Dein Gegner hatte keine Wahl. Das ist Meldung **E5**.“)*

**Stich 6 — Gegner spielt `♥C`.**
> **Erlaubt ist:** `♦A` – deine letzte Karte.
> Text: „Du hast **weder Herz noch Tarock**. Dann darfst du **jede beliebige** Karte abwerfen. Sie kann den
> Stich nicht gewinnen – wirf also die billigste. Hier hast du ohnehin nur noch eine.“
> Gegner gewinnt.

**Schlussbild:**
> „Von diesen sechs Stichen hast du einen gemacht (6 Rohpunkte), der Gegner fünf (28 Rohpunkte).
> Das war Absicht: Diese Hand sollte dir die **Zwänge** zeigen, nicht das Gewinnen.
>
> **Die Regel in einem Satz:** *Farbe bedienen – sonst Tarock – sonst irgendetwas.*
> Und: **Dein Deckblatt zählt wie eine Handkarte, die Karte darunter zählt gar nicht.**“

---

#### Übungshand 3 — „Pagat ultimo“

**Lernziel:** Endspiel mit dem Pagat; warum man den Pagat **nicht** zum Stechen verwendet; wie man Tarock
zieht; Mond und Sküs.

**Feste Stellung, vier Stiche vor Schluss (Stich 24 von 27).** Du bist am Ausspiel.

> **Dein Blatt (4 Karten, alle in der Hand – deine Strohmänner sind aufgebraucht):**
> `XXI (Mond)` `XX` `XIX` `I (Pagat)`
>
> **Dein Gegner (4 Karten):**
> `Sküs` `XVIII` `♠K` `♥D`
>
> **Stand:** Du **18 ⅔** Punkte · Gegner **29 ⅔** Punkte
> Von den vier Königen liegen `♥K` und `♦K` schon in seinen Stichen, der `♣K` in deinen.
> Du hast das Spiel **aufgenommen** – du brauchst **35 Punkte und 2 Blatt**.

**Schritt 1 — Text (die eigentliche Lektion):**
> „Es sieht schlecht aus: Du liegst 11 Punkte zurück. Aber alle vier restlichen Stiche sind fett, und du hast
> den Pagat.
>
> **Was du nicht tun darfst:**
> * **Nicht den Pagat ausspielen!** Er ist das **niedrigste** Tarock; der Gegner sticht ihn mit allem, was er
>   hat.
> * **Nicht den Mond ausspielen!** Der Gegner hat noch den Sküs – er würde den Mond fangen (5 Punkte weg).
>
> **Was du tun musst:** Dem Gegner die Tarock **ziehen**, damit im letzten Stich keines mehr übrig ist.
> **Spiel das XIX** – dein billigstes hohes Tarock.“
>
> **Aktion:** `XIX`.
> **Gegner legt:** `Sküs` und gewinnt (1 + 5 = 6 Rohpunkte).
> Text: „Er hat den Sküs geopfert, um den Stich zu machen. Gut für dich: **Der Sküs ist jetzt weg** – dein
> Mond ist ab sofort sicher.“
>
> *(Variante, falls der Gegner stattdessen `XVIII` legt: Du gewinnst diesen Stich, spielst danach `XX`, er
> muss den `Sküs` legen und gewinnt – das Ergebnis ist **exakt dasselbe**. Die App spielt beide Linien
> durch und zeigt sie in der Nachbesprechung.)*

**Schritt 2 — Stich 25. Gegner spielt `♠K`.**
> **Erlaubt sind:** `XXI`, `XX`, `I` – du hast kein Pik, also **Tarockzwang**.
> Text: „**Hier entscheidet sich alles.** Du musst Tarock spielen. Nimm das `XX`.
> * Der **Pagat** würde den Stich zwar auch gewinnen – aber dann ist er weg, und der Pagat ultimo ist
>   dahin. Nie, nie den Pagat verheizen.
> * Der **Mond** wäre Verschwendung; du brauchst ihn für den nächsten Stich nicht.“
>
> *(Tippt der Spieler auf `I` → **C1**: „Vorsicht: Der Pagat ist das niedrigste Tarock …“, zusätzlich der
> Lernhinweis „Damit verlierst du den Pagat ultimo!“)*
>
> **Aktion:** `XX`. **Du gewinnst** (5 + 1 = 6 Rohpunkte).

**Schritt 3 — Stich 26. Du spielst aus.**
> Text: „Der Gegner hat noch zwei Karten: ein Tarock und eine Farbkarte. **Zieh ihm das letzte Tarock:
> Spiel den Mond.** Er ist jetzt unschlagbar, weil der Sküs schon gefallen ist.“
>
> **Aktion:** `XXI`. **Gegner legt:** `XVIII` (er **muss** Tarock bedienen). **Du gewinnst** (5 + 1 = 6).

**Schritt 4 — Stich 27, der letzte.**
> Text: „Jetzt. Der Gegner hat nur noch die `♥Dame` – **kein Tarock mehr**. **Spiel den Pagat.**“
>
> **Aktion:** `I`. **Gegner legt:** `♥D`. **Du gewinnst** (5 + 4 = 9 Rohpunkte) – **Pagat ultimo!**

**Schlussabrechnung (nachgerechnet):**

| | Du | Gegner |
|---|---|---|
| Karten in den Stichen | 26 | 28 |
| Rohsumme | 53 | 53 |
| **Kartenpunkte** | `53 − ⅔·26 =` **35 ⅔** (= 35 und 2 Blatt = gerundet 36) | `53 − ⅔·28 =` **34 ⅓** (= 34) |
| Aufgenommenes Spiel gewonnen | **+3** | −3 |
| Pagat ultimo | **+1** | −1 |
| Grammel‑Punkt (ab 45) | – | – |
| Trull (du hast Mond + Pagat, der Sküs liegt beim Gegner) | – | – |
| Vier Könige (2 : 2) | – | – |
| **Summe** | **+4** | **−4** |

> „**Du hast mit dem knappst möglichen Ergebnis gewonnen:** 35 Punkte und 2 Blatt. Ein Blatt weniger – also
> 35 und 1 Blatt – und das Spiel wäre verloren gewesen, obwohl du mehr Punkte gehabt hättest als dein
> Gegner. Deshalb ist in Strohmandeln jeder einzelne Stich wichtig, auch der kleinste.“

---

## 12. Quellenverzeichnis (vollständige URLs, Abruf 11./12.09.2026)

**Hauptquellen**

* **[MS‑WEB]** *Strohmann‑Tarock (Strohmandeln)*, tarockspiele.com (Website zu Mayr/Sedlaczek,
  *Das große Tarockbuch*, Perlen‑Reihe 642, Wien 2001). Domain heute geparkt; gelesene Fassung:
  `https://web.archive.org/web/20020129045230/http://www.tarockspiele.com:80/strohmann.html`
* **[MS‑EN]** Tarik Wareh, *Rules for Strohmandeln* (englische Übersetzung einer späteren Fassung von
  [MS‑WEB], mit 40‑Karten‑Variante):
  `https://web.archive.org/web/2015/http://www.wareh.org/cards/strohmandeln.html`
  (Original `http://www.wareh.org/cards/strohmandeln.html` ist derzeit nicht erreichbar)
* **[MS‑WZ]** Wolfgang Mayr / Robert Sedlaczek, *Zweiertarock (Strohmandeln) mit 54 Karten: Wie sehen die
  idealen Regeln aus*, Wiener Zeitung, 25.09.2007:
  Seite 1: `https://www.tagblatt-wienerzeitung.at/startseite/archiv/95356_Wie-sehen-die-idealen-Regeln-aus.html`
  Seite 2: `https://www.tagblatt-wienerzeitung.at/startseite/archiv/95356_Wie-sehen-die-idealen-Regeln-aus.html?em_cnt_page=2`
  (die alte Adresse `https://www.wienerzeitung.at/startseite/archiv/95356_...` liefert für Seite 2 HTTP 410)
* **[TIROL]** Markus Mair, *Strohmandeln TAROCK*, Jänner 2025:
  `http://www.tarock.tirol/tarock-regeln/tarock-regeln-strohmandln.pdf`
* **[TIROL‑G]** Markus Mair, *Regelvergleich – Grundsätzliche Überlegungen* (Zählweise, „36 Punkte =
  35/2 Blatt“): `http://www.tarock.tirol/tarock-regeln/tarock-regeln-grundsaetzliches.pdf`
  Regelübersicht des Vereins: `http://www.tarock.tirol/tarock-regeln.html`
* **[FURR]** Jerry Neill Furr, *Tarocchi. An introduction to the many games played with tarot cards*,
  Philebus 2009, Kap. 5.05 „Straw Man Tarock“, S. 109–111:
  `https://archive.org/details/tarocchi-philebus/page/108/mode/2up`
  Volltext: `https://ia800904.us.archive.org/22/items/tarocchi-philebus/Tarocchi_Philebus_djvu.txt`
  Identischer Text auf der Website des Autors:
  `https://web.archive.org/web/20100210072253/http://www.tarocchino.com:80/page33.html`

**Enzyklopädisch**

* **[DEWIKI]** `https://de.wikipedia.org/wiki/Strohmandeln`
* **[ENWIKI]** `https://en.wikipedia.org/wiki/Strohmandeln`
* Verwandtes zweihändiges Tarockspiel: `https://en.wikipedia.org/wiki/Kosakeln`
* Blattbeschreibung: `https://de.wikipedia.org/wiki/K%C3%B6nigrufen`

**Für die Begriffe „bunt gemischt“ und „Stichzwang“ (Trischaken, §5.9) herangezogen**

* Linzer Tarock Akademie, *Königrufen – Grundkurs 1*, Stand 01/25, Kap. 8 und 11:
  `https://www.linzer-tarock-akademie.at/fileadmin/user_upload/skriptum/a_Skriptum_GK1_2025_1.pdf`
* Markus Mair, *Tarock‑Kurs* (Königrufen), Abschnitt „Die Negativspiele“ — **derselbe Autor wie [TIROL]**,
  Beleg für „Der Pagat darf erst als letztes Tarock gespielt werden“ und „Stichzwang“ in allen
  Negativspielen einschließlich Trischaken: `http://www.tarock.tirol/tarock-regeln/tarock-regeln-kurs.pdf`
* Regelsammlung *Trischaken*: `https://tarock-konigrufen8.webnode.at/trischaken/` — wörtlich: „Es wird
  ‚bunt gemischt‘ gespielt, das heißt, es braucht nicht austarockiert zu werden, sondern jeder Ausspieler
  hat die Wahl, ob er Tarock oder Farbe anspielt.“
* Schwesterspezifikation dieses Projekts: `/home/defaultuser/ps/tarock-spec/koenigrufen.md` §6.4

**Für die 40‑Blatt‑Variante (§9.13) herangezogen**

* `https://en.wikipedia.org/wiki/Zwanzigerrufen` — Deckzusammensetzung (Tarock II und III entfernt,
  je Farbe K/D/C/B + „Glatze“), Glatzen = 0 Punkte, einfache Addition ohne Drittel, 88 Gesamtpunkte,
  Gewinnschwelle 45
* `https://www.pagat.com/tarot/zwanzig.html` (Übersichtsseite; verweist für die Regeln weiter)

**Regionale Abweichungen**

* **[HU‑SCO]** *Strohmandli* (ungarische 42‑Blatt‑Fassung), StüsziVadász / scordatura.hu:
  `https://www.scordatura.hu/stuszivadasz/paros.html` und
  `https://www.scordatura.hu/stuszivadasz/tarokk.html`
* Übersichtsseite mit Einordnung in die Tarockfamilie: `http://homer.members.pgv.at/tarock/tarockvarianten.htm`

**Negativbelege (siehe §0.2)**

* `https://www.pagat.com/tarot/strohmandeln.html` → **HTTP 404** (die im Auftrag genannte Primärquelle
  existiert nicht)
* `https://www.pagat.com/tarot/` → Tarock‑Index **ohne** Strohmandeln‑Eintrag
* `https://www.pagat.com/national/austria.html` → nennt Strohmandeln, verlinkt aber keine Regelseite
* Wayback‑CDX über die Domain pagat.com, gefiltert auf `stroh|kosak` → **leer**

**Nur mittelbar benutzte Literatur** (nicht im Volltext eingesehen; hier zitiert, wie [ENWIKI]/[DEWIKI] sie
angeben)

* Fritz Beck, *Tarock komplett. Alle Spiele*, Perlen‑Reihe Nr. 640, Wien 1972, S. 40–45
* Johannes Bamberger, *Tarock: Die schönsten Varianten*, Perlen‑Reihe 640, 22. Aufl., Wien 2011, S. 23–28
* Hans Löw, *Tarock komplett*, 2. Aufl., Pechan, Wien 1956, S. 102–110 (Kosakeln)
* Michael Dummett, *The Game of Tarot*, Duckworth, London 1980, S. 457–459
* Michael Dummett / John McLeod, *A History of Games Played with the Tarot Pack*, Vol. 2, Edwin Mellen
  Press 2004
* Wolfgang Mayr / Robert Sedlaczek, *Die Strategie des Tarock Spiels*, Atelier, Wien 2008, S. 380
* S. Ulmann, *Das Buch der Familienspiele*, A. Hartleben, Wien/München/Pest 1890, S. 244 f.
* *Neuestes Spielbuch*, Wien 1800, S. 145 f.; *Neuestes Allgemeines Spielbuch*, C. Haas, Wien 1829, S. 94;
  2. Aufl. 1839, S. 121
* Friedrich Torberg, *Die Tante Jolesch oder der Untergang des Abendlandes in Anekdoten* (Herkunft des
  Rostopschin)

**Nicht verwendet** (geprüft und verworfen)

* `https://gist.github.com/sbeyer/d245d0b534ebb54952dd` – Regelnotiz eines Nichtspielers, vom Autor selbst als
  „unvollständig und mit Sicherheit fehlerhaft“ bezeichnet; enthält u. a. ein falsches Blatt (rote Zahlkarten
  in allen vier Farben) und keine Prämien.
* `https://funcardgames.net/de/strawman_tarot/` – Seite antwortete bei allen Abrufversuchen nicht
  (Verbindung abgelehnt); Inhalt nicht überprüfbar.

---

## 13. Offene Fragen (Stand 12.09.2026)

| # | Frage | Betrifft | Vorschlag |
|---|---|---|---|
| 1 | Die im Auftrag genannte Primärquelle **pagat.com/tarot/strohmandeln.html existiert nicht** (404, auch nie archiviert). Ist eine andere Quelle gemeint, oder ist die Adresse nur falsch erinnert? | §0.2, gesamte Quellenbasis | Spezifikation stützt sich stattdessen auf [MS‑WEB]/[MS‑EN]/[MS‑WZ] + [TIROL] |
| 2 | **Beck 1972** liegt nicht im Volltext vor; [DEWIKI] und [ENWIKI] widersprechen einander in den Wertzahlen und [DEWIKI] widerspricht sich selbst | §9.2, Profil `BECK-1972` | Antiquarisch beschaffen (Perlen‑Reihe 640, S. 40–45) |
| 3 | **Bamberger 2011** (Perlen‑Reihe 640, 22. Aufl., S. 23–28) nur über [ENWIKI] referiert | evtl. eigenes Profil | beschaffen |
| 4 | ~~Was bedeutet **„bunt gemischt“**?~~ **Erledigt** (§5.9.1): stehender Fachausdruck = „es braucht nicht austarockiert zu werden“. **Offen bleibt**, ob im Tiroler Trischaken die Negativspiel‑Regel „Pagat nur als letzter Tarock“ gilt (im Königrufen‑Regelwerk desselben Autors ja, im Strohmandeln‑Blatt nicht erwähnt) und wie ein **Punktegleichstand** gewertet wird | §5.9.3, §9.10 | Rückfrage bei Markus Mair (tarock.tirol) vor Auslieferung des Tiroler Profils; vorläufig Pagat‑Regel `true`, Gleichstand 0 : 0 |
| 5 | **Ansagezeitpunkt und Strafe** für misslungene Ansagen im Tiroler Profil sind nicht geregelt | §5.2.3, §9.11 | Rückfrage; vorläufig „vor dem ersten Ausspiel, misslungen = Wert an den Gegner“ |
| 6 | Zählen **Deckblätter** zum „Blatt“ bei den Ansagen *Trull im Blatt* / *vier Könige im Blatt*? Keine Quelle sagt es | §5.2.1, §9.7 | vorläufig ja; Rückfrage bei Spielkreisen |
| 7 | **40‑Blatt‑Variante:** Deck, Kartenwerte (Glatzen = 0), einfache Addition, 88 Gesamtpunkte und Schwelle 45 sind inzwischen geklärt (§9.13). **Offen bleiben** die Prämientarife: Grammel‑Punkt‑Schwelle (45 ist dort schon die Gewinnschwelle) und Valat‑Wert bei 88 statt 70 Punkten; ebenso, ob die Zwanzigerrufen‑Kreise überhaupt mit Prämien strohmandeln | §9.13, Profil `AT-STROH-40` | erst nach Fertigstellung der Zwanzigerrufen‑Spezifikation entscheiden |
| 8 | Verdoppelt **Kontra** auch die Prämien? Quelle sagt nur „die Spielpunkte“ | §5.10, §9.8 | vorläufig nein; Kontra ist ohnehin per Default aus |
| 9 | Stapelt sich die **Nullrunden‑Verdopplung** bei mehreren Unentschieden hintereinander? | §8.3 | vorläufig nein (`null_round_stacks = false`) |
| 10 | Gilt die Spiegelprämie („Vogel gefangen“) auch für **Uhu und Kakadu**, oder nur für den Pagat? [ENWIKI] sagt „normalerweise ja“, die deutschen Quellen schweigen | §5.4, §9.5 | vorläufig ja |
| 11 | Wer gibt neu, wenn **beide passen** und mit `redeal` gespielt wird? [MS‑WEB]/[MS‑EN] sagen „derselbe Geber“, [DEWIKI]/[BECK] und [ENWIKI] „die alte Vorhand“ | §3.2, Flag `redeal_dealer` | vorläufig `same_dealer` im Default‑Profil, `forehand_deals` im Profil `BECK-1972` |
| 12 | Wird die **letzte Karte eines Päckchens** gezeigt, bevor sie aufgenommen wird? [TIROL] und [DEWIKI]/[BECK] sagen ausdrücklich nein, [MS‑WEB]/[MS‑EN]/[MS‑WZ] regeln nur die Aufnahmebedingung und schweigen zum Vorzeigen | §4.5, Flag `last_card_is_secret` | vorläufig verdeckt (`true`); das gesamte Informationsmodell §10.1 hängt daran |
| 13 | **Grammel‑Punkt** in Dreierlagen (`p3 ≥ 134`) oder blattgenau (`p3 ≥ 135`)? Genau ein Fall unterscheidet die Lesarten: `44 ⅔ : 25 ⅓` | §5.6, §9.6 | `grand_point_uses_rounded_points = true`, weil M&S' zwei Sätze „25 oder weniger“ / „mindestens 26“ nur ganzzahlig lückenlos sind |

---

## 14. Prüfprotokoll (unabhängige Regelprüfung, 12.09.2026)

Diese Spezifikation wurde von einem zweiten, skeptischen Prüfer gegen die **im Dokument selbst genannten
Primärquellen** abgeglichen. Alle unten genannten Quellen wurden dafür **neu abgerufen und im Volltext
gelesen**, nicht aus dem Bestand übernommen.

### 14.1 Geprüfte Quellen

| Quelle | Abruf | Ergebnis |
|---|---|---|
| `pagat.com/tarot/strohmandeln.html` | 12.09.2026 | **HTTP 404 bestätigt** |
| `pagat.com/tarot/` (Tarock‑Index) | 12.09.2026 | **bestätigt:** verlinkt aus Österreich nur `koenig.html`, `lungau.html`, `zwanzig.html`, `tapptarock.html`, `kirchberg.html` (Doppen) – **kein** Strohmandeln |
| `pagat.com/national/austria.html` | 12.09.2026 | **bestätigt:** „**Strohmandeln** (2 players) … less widespread“, **Fettdruck ohne Hyperlink**; als Beleg genannt Dummett/McLeod, *A History of Games Played with the Tarot Pack*, Vol. 2 |
| [MS‑WEB] (tarockspiele.com, Wayback 29.01.2002) | 12.09.2026, Volltext | Alle Zitate in §1–§7 und §10 **wörtlich verifiziert**, einschließlich Prämientabelle, Renonceregeln und Taktikteil |
| [MS‑EN] (wareh.org) | 12.09.2026, Volltext | verifiziert; **eine Übersetzungsabweichung** gefunden (§4.7) |
| [MS‑WZ] (Wiener Zeitung 25.09.2007, Seiten 1 und 2) | 12.09.2026, Volltext | verifiziert: 20/30/40 Cent, Prämien je 10 Cent, Valat 120, **Quapil ja, Grammel‑Punkt nein, Rostopschin nein, Kontra nein**, Nullrunde |
| [TIROL] (tarock.tirol, Jänner 2025) | 12.09.2026, Volltext (1 Seite) | verifiziert: 2/3, Trischaken 2, Zusätze still 1 / angesagt 2, Partie bis 11, „bei unvollständigen Lagen wird nur ein Punkt abgezogen“ |
| [TIROL‑G] (Regelvergleich) | 12.09.2026, Volltext | verifiziert: Herkunft der Dreierlagen‑Zählung, „35/2 Blatt und nicht schon 35/1“ |
| [FURR] (Tarocchi, S. 109–111) | 12.09.2026, Volltext | verifiziert – und **drei Abweichungen zur [ENWIKI]‑Tabelle** festgestellt (§9.14) |
| [DEWIKI] | 12.09.2026, Quelltext | verifiziert |
| [ENWIKI] | 12.09.2026, Quelltext | verifiziert; **zwei Fehler** bestätigt (§9.9, §9.14) |
| [HU‑SCO] (scordatura.hu) | 12.09.2026, Volltext | §9.12 verifiziert: 42 Blatt, 12 Handkarten, 3 × 3 Strohmann, kein Aufnehmer, Játék 48 / Kassza 60 / Duplajáték 71 von 94, Volát 6, Pagátfogás 5, Uhu 5, Pagátulti 5; die Behauptung, die Original‑Strohmandeln‑Regeln stünden auf pagat.com, steht dort tatsächlich – und ist falsch |
| **neu:** `en.wikipedia.org/wiki/Zwanzigerrufen` | 12.09.2026 | Grundlage der korrigierten §9.13 |
| **neu:** LTA *Grundkurs 1*, Mairs *Tarock‑Kurs*, `tarock-konigrufen8.webnode.at/trischaken/`, `koenigrufen.md` §6.4 | 12.09.2026 | Grundlage der neuen §5.9.1–5.9.3 |

**Nicht nachprüfbar:** Die CDX‑Abfrage der Wayback‑Machine über die gesamte Domain `pagat.com` (§0.2, letzter
Spiegelstrich) konnte im Prüflauf nicht wiederholt werden, weil `web.archive.org` nicht erreichbar war. Die
drei übrigen Negativbelege zu pagat.com sind dagegen frisch bestätigt und tragen die Aussage von §0.2
allein. — Ebenfalls weiterhin **nicht im Volltext** eingesehen: [BECK] 1972, [BAMB] 2011, [DUMMETT] 1980,
[HIST]; die Spezifikation kennzeichnet das durchgehend.

### 14.2 Korrekturen (sachliche Fehler)

| # | Fundstelle | Was falsch war | Was jetzt dasteht |
|---|---|---|---|
| **K1** | §1.7 Schwellentabelle | Zeile „≤ 34 ⅔ → **verloren**“ widersprach §1.7.1: bei `34 ⅔` hat der Gegner `35 ⅓` und erreicht die Schwelle ebenfalls nicht – das ist ein **Unentschieden**, keine Niederlage | Tabelle um die Zeile `34 ⅔` ergänzt, Verlustgrenze auf `≤ 34 ⅓` (`p3 ≤ 103`) korrigiert, `p3`‑Spalte und expliziter Warnhinweis ergänzt |
| **K2** | §5.6, §7.2 Schritt 4, §9.6 | Grammel‑Punkt‑Schwelle als `points3 >= 135` angegeben. Das schließt den Fall `44 ⅔ : 25 ⅓` aus, der am Tisch als **45 : 25** gezählt wird und nach [MS‑WEB] einen Grammel‑Punkt ergibt | `p3 >= 134` (= `pointsRounded ≥ 45`), mit Herleitung: M&S' Sätze „Gegner 25 oder weniger“ und „Gegner mindestens 26“ sind nur in ganzzahliger Arithmetik lückenlos. Neues ⚙ `grand_point_uses_rounded_points` für die strikte Lesart |
| **K3** | §3.3, §5.6, §5.11 | Die Zeile „Aufnehmer < 26 Punkte“ trug in der Furr‑Spalte den Wert 5, obwohl [FURR] „26 **or less**“ schreibt – das ist die Schwelle „Gegner ≥ **44**“, nicht „≥ 45“. Außerdem ist Furrs Regel an die Aufnehmerrolle gebunden, der Grammel‑Punkt bei M&S nicht | Zeile doppelt beschriftet, beide Schwellen ausgeschrieben, neue ⚙ `grand_point_defender_threshold` (FURR: 44) und ⚙ `grand_point_role_bound` (FURR: `true`) |
| **K4** | §3.2 Flag `redeal_if_both_pass` | Der Wert `redeal` war als „**derselbe Geber** gibt neu“ beschrieben und dabei u. a. auf [DEWIKI] gestützt. [DEWIKI] sagt das Gegenteil: „es wird **von der alten Vorhand** neu gegeben“; [ENWIKI] ebenso | Widerspruch dokumentiert, neues ⚙ `redeal_dealer` ∈ {`same_dealer`, `forehand_deals`}; `forehand_deals` im Profil `BECK-1972`. Abgegrenzt gegen §2.5 (Zusammenwerfen ohne Tarock), wo beide M&S‑Quellen eindeutig „derselbe Spieler“ sagen |
| **K5** | §5.9, §9.10 | „bunt gemischt“ war als ungeklärt geführt, mit einer Lesart (b), die die Strohmänner abgeschafft hätte. Der Ausdruck ist ein **stehender Fachterminus**: „es braucht nicht austarockiert zu werden“ | Neue §5.9.1 mit Beleg; Lesart (b) gestrichen; §9.10 auf die verbleibenden Punkte (Pagat‑Regel, Gleichstand) reduziert |
| **K6** | §5.9 „Stichzwang im Zweierspiel“ | Definiert als „die **höchstmögliche** Karte spielen, sofern damit der Stich gewonnen werden kann“. Das ist strenger als jede Quelle: Stichzwang heißt **überstechen, wenn möglich**; innerhalb der stichgewinnenden Karten ist die Wahl frei | Neue §5.9.2 mit Pseudocode; §6.8 `forceWin()` ausformuliert; Lernmodus‑Meldung E9 präzisiert |
| **K7** | §9.13 (40‑Blatt‑Variante) | „Kartenwerte und Zählung bleiben“ – falsch. Zwanzigerrufen hat **Glatzen mit 0 Punkten**, zählt durch **einfache Addition** ohne Drittel, kommt auf **88** Punkte und hat die Schwelle **45** | Vergleichstabelle 54 ↔ 40 ergänzt, Glatzen‑Regel benannt, Gesamtpunkte und Schwelle belegt; zusätzlich angemerkt, dass die Grammel‑Schwelle 45 dort sinnlos wäre und der Märchenstich im Zweierspiel nicht auftreten kann. Die Liste der entfernten Karten war **richtig** |
| **K8** | §4.7 / §6.7 (zwei Karten aufgedeckt) | „**Beide** Karten zeigen, **beide** wieder verdeckt zurücklegen“ – das folgt der englischen Übersetzung [MS‑EN] („these two cards … lay them back“). Das deutsche Original [MS‑WEB] steht im **Singular**: „so muss er auch **diese zweite Karte** … zeigen und **sie** dann wieder verdeckt … zurücklegen“ | Auf das deutsche Original korrigiert, Übersetzungsfehler dokumentiert; zusätzlich die zu enge Quellenbedingung „kein König und kein Trullstück“ als Auslegung auf „kein Tarock und kein König“ präzisiert |
| **K9** | §0.3 [MS‑EN] | „**autorisierte** englische Übersetzung … aus einer späteren Fassung (**2007**)“ – Wareh erhebt keinen Autorisierungsanspruch und nennt kein Jahr | Auf das Nachweisbare zurückgenommen: Übersetzung einer erkennbar späteren Fassung (enthält die 40‑Karten‑Variante), ohne Jahresangabe, ohne Autorisierungsbehauptung |

### 14.3 Ergänzungen (Lücken, nicht Fehler)

| # | Fundstelle | Ergänzung |
|---|---|---|
| **E1** | §9.14 (neu) | Systematischer Abgleich von [ENWIKI]s Furr‑Spalte mit [FURR]s Volltext. **Zweiter belegbarer [ENWIKI]‑Fehler:** dort steht „Trull or Royal Trull (tricks) – Furr 1“, obwohl Furrs Bonusliste (With 45, With 26, Slam, The Birds, Rostopschin) **keine** Trull‑Prämie enthält. Ebenso festgehalten: Furr kennt keine Ansagen („none of them may be announced!“), keinen Mondfang, keinen Quapil |
| **E2** | §5.9.3 (neu) | Pagat‑Regel im Trischaken. In **Markus Mairs eigenem** Königrufen‑Kursskript gilt für alle Negativspiele „Der Pagat darf erst als letztes Tarock gespielt werden“; sein Strohmandeln‑Blatt schweigt. Als ⚙ `trischaken_pagat_last_tarock` (Default `true`) aufgenommen und **ausdrücklich als Analogieschluss markiert**. Sie geht dem Stichzwang vor |
| **E3** | §5.9.4 (neu) | Trennungstabelle „Was in [TIROL] wirklich steht / was Ergänzung dieser Spezifikation ist“ |
| **E4** | §4.5 | Quellenlage zur **verdeckten** letzten Päckchenkarte präzisiert: ausdrücklich belegt nur bei [TIROL] und [DEWIKI]/[BECK]; [MS‑WEB]/[MS‑EN]/[MS‑WZ] regeln dort nur die Aufnahmebedingung und schweigen zum Vorzeigen. Neues ⚙ `last_card_is_secret`, weil das gesamte Informationsmodell (§10.1) daran hängt |
| **E5** | §4.5 | Die Variante „letzte Karte bleibt verdeckt am Tisch“ ist nur dann – wie [MS‑WEB] behauptet – „ohne Bedeutung für den Spielverlauf“, wenn die Tischkarte für Farb‑/Tarockzwang wie eine Handkarte zählt. Die frühere Fassung nahm das Gegenteil an und machte die Variante damit regelwirksam. Jetzt als rein kosmetisch implementiert |
| **E6** | §1.5.3 | Beweis ergänzt, dass „Restlage pauschal −1“ und „⅓ ab, ⅔ auf“ **immer** dasselbe Ergebnis liefern (`n = 3k + r`); [TIROL‑G] führt beide als getrennte Verfahren, ohne die Äquivalenz zu zeigen |
| **E7** | §1.7.1, §7.2 | Unentschieden‑Zone in Dritteln ausgeschrieben (`p3 ∈ {104, 105, 106}`) und der Assert ergänzt, dass beide Spieler nie gleichzeitig ≥ 107 haben können |
| **E8** | §5.5 | Schreibweise des Rufworts: [MS‑WEB]/[MS‑EN] „**Ross!**“, [FURR]/[ENWIKI] „**Ros**“. Beides belegt, ⚙ `rostopschin_call_spelling` |
| **E9** | §11.4 | Neue Lernmodus‑Meldung **E9b** für die Trischaken‑Pagat‑Regel; **E9** um den Hinweis ergänzt, dass man innerhalb der stichgewinnenden Karten frei wählen darf |
| **E10** | §13 | Vier neue offene Fragen (Nr. 11–13) aufgenommen, Nr. 4 und 7 auf ihren verbleibenden Rest reduziert |

### 14.4 Nachgerechnet und **bestätigt** (keine Änderung nötig)

* **§1.4 Kartenwerte:** 3·5 + 19·1 + 4·5 + 4·4 + 4·3 + 4·2 + 16·1 = **106** Rohpunkte auf **54** Karten;
  `106 − ⅔·54 = 70` ✔
* **§1.6 Zählbeispiel:** 26 Karten, Rohsumme 53 → `p3 = 3·53 − 2·26 = 107` = `35 ⅔` = „35 und 2 Blatt“ =
  gerundet 36; Gegenprobe 28 Karten, Rohsumme 53 → `p3 = 103` = `34 ⅓`; Summe 210 ✔. Auch die Zwischenschritte
  (9 Zählkarten Summe 36 + 17 Leerkarten; 8 Lagen zu 24 Karten, Summe 51 − 16 = 35, Rest 2 Blatt) stimmen ✔
* **§1.5.3 Pseudocode:** `pointsRounded(p3) = (p3+1)/3` liefert für alle drei Restlagen‑Fälle exakt das
  Ergebnis des Dreierlagen‑Verfahrens ✔
* **§7.3 Maximalabrechnung:** 12 + 3 + 1 + 1 + 1 + 1 + 1 + 1 + 1 = **22** ✔
* **§7.4 Beispiele 1–4:** alle vier Summen stimmen ✔
* **§10.2 Blattbeispiele aus [MS‑WEB]:** 4⅓·3 + 3⅓·2 = **19 ⅔** ✔ und 4⅓·4 + 3⅓·2 = **24** ✔
* **§10.4 Faustregel:** 24 · 8/54 = 3,56 ≈ „3 Karten je Farbe in den Strohmännern“ ✔
* **§11.7 Übungshand 1:** die JSON‑Gabe enthält **alle 54 Karten genau einmal**, 27 je Spieler; die
  Aufdeckfolge (♦K, XX → Hand; ♠B, ♣D, ♣10 als Deckblätter; beim Gegner ♠C, ♦B, ♥2 und nichts aufzunehmen)
  folgt zwingend aus den Päckchen; die drei Stiche und der Zwischenstand `11 ⅓ : 1 ⅔` sind korrekt ✔
* **§11.7 Übungshand 2:** alle sechs Stiche sind legal und erzwungen wie beschrieben; die Rangfolge
  „♥B schlägt ♥A“ (rot: K > D > C > B > A > 2 > 3 > 4) stimmt; Endstand 6 : 28 Rohpunkte ✔
* **§11.7 Übungshand 3:** Ausgangsstand `18 ⅔ : 29 ⅔` plus die 8 Restkarten (Rohwert 27) ergibt exakt 70;
  Schlussabrechnung `35 ⅔ : 34 ⅓`, Trull‑ und Königsbilanz (2 : 2) stimmen ✔
* **§9.12 Ungarn:** 42 Blatt, 12 Handkarten, 3 × 3 Strohmann, kein Aufnehmer, Schwelle 48 von 94 ✔
* **§6.2 Zugkaskade** und **§6.4 Stichgewinn** decken sich wörtlich mit [MS‑WEB], [DEWIKI] und [FURR] ✔
* **§6.7 Renonceregeln** decken sich wörtlich mit [MS‑WEB], einschließlich der bemerkenswerten Regel, dass
  das Übersehen des **eigenen Deckblatts** sanktionslos bleibt ✔
* **§2.5** Zusammenwerfen ohne Tarock: „derselbe Spieler gibt neu“ steht wörtlich in [MS‑WEB] **und**
  [MS‑WZ] ✔

### 14.5 Was unsicher bleibt

1. **Beck 1972 ist und bleibt ungeprüft.** Der Werte­widerspruch zwischen [DEWIKI] (Prämien je 2, kein
   erhöhter Verlustwert) und [ENWIKI] (Prämien je 1, Werte 1/2/3) lässt sich **nur** am Buch auflösen. Das
   Profil `AT-STROH-BECK-1972` ist deshalb eine **Rekonstruktion**, keine Wiedergabe. §9.2 bleibt offen.
2. **Die letzte Päckchenkarte** – gezeigt oder nicht? Zwei Quellen sagen ausdrücklich „nicht gezeigt“, die
   Normquelle dieser Spezifikation schweigt. Da das gesamte Informationsmodell daran hängt, ist das die
   spielmechanisch folgenreichste offene Frage (§13 Nr. 12).
3. **Grammel‑Punkt bei `44 ⅔ : 25 ⅓`.** Die hier gewählte Lesart ist gut begründet, aber nicht wörtlich
   belegt; kein Regelwerk behandelt den Grenzfall ausdrücklich (§13 Nr. 13).
4. **Tiroler Trischaken:** Pagat‑Regel und Gleichstandswertung sind Analogieschlüsse. Das Profil sollte
   nicht als „Tiroler Regeln“ ausgeliefert werden, bevor der Autor bestätigt hat (§13 Nr. 4).
5. **Ansagezeitpunkt im Tiroler Profil** (§9.11) und **Deckblatt als Teil des „Blattes“ bei den
   klassischen Ansagen** (§9.7) sind weiterhin reine Festlegungen dieser Spezifikation.
6. **Kontra‑Details** (§9.8) und **Nullrunden‑Stapelung** (§8.3) sind Konventionen ohne Quellendeckung;
   beide betreffen im Default abgeschaltete bzw. seltene Fälle.
7. **40‑Blatt‑Prämientarife** (§9.13): Deck und Zählung stehen jetzt fest, die Tarife nicht.
8. **Wayback‑CDX‑Negativbeleg** (§0.2) war im Prüflauf nicht wiederholbar; die Kernaussage wird aber von
   drei frisch bestätigten Negativbelegen getragen.
