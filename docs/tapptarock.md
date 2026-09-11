# Tapp‑Tarock (Wiener Tappen) und Dreiertarock
## Implementierungsspezifikation für harbour‑tarock (Regelprofil `AT-TAPP-KLASSIK`)

Stand: 12.09.2026 · Zielplattformen: Sailfish OS (Qt 5.6 / QtQuick 2.6 / Silica), Android (Qt 6.11) · Engine: C++17

---

## 0. Vorbemerkungen, Quellenlage, Regelprofile

### 0.1 Was hier spezifiziert wird

Spezifiziert wird das **österreichische Tarockspiel zu dritt mit 54 Karten und sechskartigem Talon**, das
historisch **Tarok‑Tappen**, **Tappen**, **Tapper** und heute meist **Tapp‑Tarock** heißt. Es ist der Urahn
des Königrufens: „Das Tapp‑Tarock ist die wohl älteste Tarockvariante, bei der vier Reformen des Tarockspiels
gemeinsam durchgeführt waren“ – Reduktion auf 54 Blatt, Sküs als normaler höchster Tarock, französisches
Blatt, Prämie *Pagat Ultimo* [WP‑DE‑TAPP §Geschichte].

**Achtung Namensfalle.** Der im Auftrag verwendete Klammerzusatz „Dreiertarock“ ist **kein Synonym**:

* **Tapp‑Tarock** – das hier spezifizierte Grundspiel (Dreier / Unterer / Oberer / Solo).
* **Dreiertarock** – im weiten Sinn ein **Sammelbegriff** für alle Tarockspiele zu dritt
  [WP‑DE‑DREIER]; im engen Sinn (Mayr/Sedlaczek, Kärnten) ein **eigenes Spiel** mit den Lizitstufen
  *Einser … Fünfer / Solo / Solo Valat* und deutlich höheren Tarifen [WP‑EN‑DREIER]. Beides ist in §3.6 und
  als Profil `AT-DREIER-54` mitspezifiziert.
* **Illustriertes Tarock** – die „Königin“ der Dreierspiele: Tapp‑Tarock plus Überschusspunkte,
  *mit 40 / mit 50*, *ohne Trull / ohne Könige*, *Uhu*, *Trischaken* [WP‑DE‑ILLU], [WP‑EN‑ILLU].
  Profil `AT-TAPP-ILLU`, §3.7.
* **Point‑Tarock** – Tapp‑Tarock plus Punktegebote *mit 40 / 45 / 50 / 55*, **ohne** Zwei‑ und Einblatt
  [WP‑DE‑POINT]. Profil `AT-POINT`, §3.8.

Die Literatur vertauscht „Illustriertes Tarock“ und „Point‑Tarock“ regelmäßig (Beck 1965 vertauschte die
Namen gegenüber Löw 1954; Alscher, Bamberger und Dummett gebrauchen sie wieder anders) [WP‑DE‑POINT §Name].
**Die App verwendet ausschließlich die oben festgelegten Bedeutungen** und zeigt in der Profilauswahl den
Untertitel mit den Lizitstufen an, damit Spielkreise ihr Spiel wiedererkennen.

**Bettler, Piccolo, Trischaken.** Der Auftrag nennt diese Spiele. Befund der Quellenprüfung:

* **Trischaken** ist für das Dreierspiel **belegt** – bei Beck als eine von drei Möglichkeiten, wenn alle
  passen [WP‑DE‑ILLU §Vorgehen bei fehlender Spielansage]. Regeln in §3.5.3 und §6.8.
* **Bettler und Piccolo sind für Tapp‑Tarock in keiner der geprüften Quellen belegt.** Sie gehören zum
  Königrufen zu viert (dort aus L’Hombre über Boston eingewandert, Bettler Zwischenkriegszeit, Piccolo
  1920er/30er) [WP‑DE‑KR §Geschichte], [WP‑DE‑KR §Negativspiele]. Ein Dreierspiel **mit** Bettler existiert –
  das slowenische Tarock zu dritt (*berač*, *odprti berač*) [PAG‑SLO §The three‑player game]. Die App bietet
  Bettler/Piccolo deshalb als **klar gekennzeichnete Erweiterung** `AT-TAPP-NEGATIV` an (§3.9); die dort
  angegebenen Tarife sind eine **Entwurfsentscheidung, keine Quellenregel**, und tragen durchgehend die
  Markierung **⚠ nicht quellenbelegt**.

Es wird nichts erfunden; jede Regel trägt eine Quellenangabe, und jede Stelle, an der Quellen einander
widersprechen, ist in §9 aufgelistet.

### 0.2 Quellen

| Kürzel | Quelle | Rolle in dieser Spezifikation |
|---|---|---|
| **[ATT1821]** | *Theoretisch‑praktische Anweisung zur gründlichen Erlernung des beliebten Tarok‑Tappen‑Spiels*, Wien und Prag 1821, Carl Haas; Volltext‑Transkription von H.-J. Alscher auf tarock.info | **Älteste bekannte Regelquelle** des Spiels. Maßgeblich für §3.1 (Tapper), §4.2 (obere Talonhälfte), §5.3 (Honneurs „nicht verpaßt“), §7.2 (Überschusspunkte), §9 |
| **[WP‑EN‑TAPP]** | en.wikipedia.org, *Tapp Tarock* – Regelteil ausdrücklich „based on Mayr and Sedlaczek (2001), pp. 109–110“ | **Primäre Normquelle für den App‑Default.** Detaillierteste moderne Gesamtdarstellung inkl. Tariftabelle |
| **[WP‑DE‑TAPP]** | de.wikipedia.org, *Tapp‑Tarock* | Vom Auftrag benannt. Gut für Karten, Zählung, Kontra; **in §Spielansagen fehlerhaft** (vermischt Tapp‑Tarock mit Illustriertem Tarock) – siehe §9.3 |
| **[WP‑DE‑DREIER]** | de.wikipedia.org, *Dreiertarock* | Vom Auftrag benannt. Enthält **keine Regeln**, sondern die Begriffsabgrenzung (§0.1) |
| **[WP‑EN‑DREIER]** | en.wikipedia.org, *Dreiertarock* (nach Mayr/Sedlaczek 2008, S. 354–359) | Normquelle für Profil `AT-DREIER-54` (§3.6) |
| **[ALSCHER]** | Hans‑Joachim Alscher, *Tarock‑Varianten*, homer.members.pgv.at | **Kompakte österreichische Referenzliste**; einzige Quelle, die Tapp‑Tarock‑Tarife, Prämien und Kontra‑Systematik vollständig nebeneinanderstellt |
| **[KRUEPL]** | Bernhard Krüpl, *Tarockania*, Archivkopie vom 14.05.2007 (von pagat.com als Regelquelle verlinkt) | Vom Auftrag über pagat.com benannt. Belegt die *Unteren/Oberen* als **Talonhälften** |
| **[FURR]** | Jerry Neill Furr, *Tapp / Illustrated Tapp*, tarocchino.com, Archivkopie vom 11.02.2010; gedruckt in *Tarocchi* (2009) | Englischsprachige Gesamtdarstellung inkl. Illustrated Tapp; belegt die verdeckte Variante von *Unterer/Oberer* |
| **[PAG‑TAPP]** | pagat.com, *Tapp Tarock* (J. McLeod, Stand 17.01.2022) | Nur **Verweisseite**: „This three‑handed Austrian Tarock game is not often played“, Links auf [KRUEPL] und Wikipedia. **Die im Auftrag genannte URL `pagat.com/tarot/tapp.html` existiert nicht (HTTP 404); korrekt ist `pagat.com/tarot/tapptarock.html`** |
| **[WP‑DE‑ILLU]** | de.wikipedia.org, *Illustriertes Tarock* (nach Fritz Beck, *Tarock komplett*, Perlen‑Reihe 640, Wien 1972, plus „Grazer Runden“ = (G)) | **Einzige belegte Trischaken‑Regel für drei Spieler**; Normquelle für Profil `AT-TAPP-ILLU` |
| **[WP‑EN‑ILLU]** | en.wikipedia.org, *Illustrated Tarock* (Löw 1954; Beck 1972 in der Lesart Dummetts 1980; Mayr/Sedlaczek 2016) | Gegenprobe zu [WP‑DE‑ILLU]; weicht bei *Panzer* und *ohne Trull/Könige* ab (§9.19) |
| **[WP‑DE‑POINT]** | de.wikipedia.org, *Point‑Tarock* (nach Beck 1983, S. 135–143) | Normquelle für Profil `AT-POINT` |
| **[WP‑DE‑KR]** | de.wikipedia.org, *Königrufen* | Herkunft und Regeln der Negativspiele, Kaiserstich, Trischaken‑Abrechnungsvarianten (§3.9, §6.5, §7.6) |
| **[WP‑DE‑19]** | de.wikipedia.org, *Neunzehnerrufen* | Nur für §9.25 (Abgrenzung, wie im Auftrag verlangt) |
| **[PAG‑SLO]** | pagat.com, *Tarok (Slovenian)*, Abschnitt *The three‑player game* | **Einziges quellenbelegte Tarockspiel zu dritt mit Bettler**; Vorbild für `AT-TAPP-NEGATIV` und für den Kaiserstich |
| **[PAG‑DRLES]** | pagat.com, *Dreierles* (J. McLeod / U. Martin, Stand 16.11.2024) | Badisches Schwesterspiel; sauber dokumentierte Feldforschung, gut für Randfälle (Renonce, Räuber, Vierertisch) |
| **[MS‑TOC]** | Mayr/Sedlaczek, *Die Strategie des Tarockspiels*, 5. Aufl. 2016, Inhaltsverzeichnis (DNB 1041884893) | Belegt, dass Piccolo/Bettler/Trischaken dort **unter Königrufen**, nicht unter Dreiertarock stehen (§0.1) |

Zitierweise: `[WP‑EN‑TAPP §Bidding]`, `[ATT1821 §20]`, `[ALSCHER Tapptarock]`.

### 0.3 Regelprofile in der App

Die Engine implementiert das Regelwerk als **Profil** (Datenstruktur, kein `#ifdef`).

| Profil‑ID | Beschreibung | Status |
|---|---|---|
| `AT-TAPP-KLASSIK` | Tapp‑Tarock nach [WP‑EN‑TAPP] / [ALSCHER]: Dreier 3, Unterer 4, Oberer 5, Solo 8 | **Default** |
| `AT-TAPP-1821` | Historisches Tarok‑Tappen: Tapper, Dreyer, Solo, Consolation‑Tarif, doppelte Überschusspunkte | Option (§3.10, Lernmodus) |
| `AT-TAPP-ILLU` | Illustriertes Tarock nach Beck/Graz: Dreiblatt … Solo mit Multiplikatoren, Überschusspunkte, viele Prämien | Option (§3.7) |
| `AT-POINT` | Point‑Tarock nach Beck: Dreier/Unterer/Oberer + „mit 40/45/50/55“ | Option (§3.8) |
| `AT-DREIER-54` | Dreiertarock (Kärnten) nach Mayr/Sedlaczek: Einser … Fünfer, Solo, Solo Valat | Option (§3.6) |
| `AT-TAPP-42` | Tapp‑Tarock mit 42 Blatt, zusätzlich Zweyer und Einer | Option (§3.11) |
| `AT-TAPP-NEGATIV` | Aufsatz‑Flags: Piccolo, Bettler, Ouvert‑Formen | Option, **⚠ nicht quellenbelegt** (§3.9) |

Jeder in §9 aufgelistete strittige Punkt ist ein **einzelnes Profil‑Flag**. Jede profilabhängige Regel dieses
Dokuments trägt die Markierung **⚙ Flag: `flag_name`**.

### 0.4 Sprachliche Konventionen und Kartenkürzel

Österreichische Fachbegriffe bleiben im Original (Glossar §11.6).

* **Tarock:** `I … XXI` (römisch; auf den Karten steht `IIII` für 4) und `Sküs` (auch *Gstieß*, *Gschtieß*).
* **Farben:** `♥` Herz (Coeur), `♦` Karo (Careau), `♠` Pik (Pique), `♣` Kreuz (Treff).
* **Figuren:** `K` König, `D` Dame, `C` Cavall (Reiter), `B` Bube.
* **Skartindeln (Leere, Glatzen):** rot `A 2 3 4`, schwarz `10 9 8 7`.
* Beispiel: `♥K` = Herz‑König, `♠10` = Pik‑Zehner, `♦A` = Karo‑Ass.
* **Sitze:** `V` Vorhand, `M` Mittelhand, `H` Hinterhand (= Geber im Dreiertisch).
* **Spielrichtung:** durchgehend **gegen den Uhrzeigersinn**, also „nach rechts“.

---

## 1. Blatt, Rangfolgen, Kartenwerte, Zählung

### 1.1 Das Blatt: 54 Karten „Industrie und Glück“

Gespielt wird mit dem vollen österreichischen Tarockblatt zu **54 Karten**; es wird **nichts entfernt**
[WP‑DE‑TAPP §Karten], [WP‑EN‑TAPP §Cards], [ALSCHER Tapptarock].

```
Deck = 22 Tarock + 4 Farben × 8 Karten = 54 Karten
```

* **22 Tarock** (dauerhafte Trümpfe): `Sküs`, `XXI` … `I`
* **32 Farbkarten**: je 8 in ♥, ♦, ♠, ♣ – vier *Figuren* (K, D, C, B) und vier *Skartindeln*

Historisch entstand dieses Blatt aus dem 78er‑Tarock durch Streichung der sechs niedrigsten Farbkarten je
Farbe: in Herz und Karo fielen 10, 9, 8, 7, 6, 5 weg, in Pik und Treff As, 2, 3, 4, 5, 6
[WP‑DE‑TAPP §Farbkarten]. Genau daraus folgt die unterschiedliche Rangordnung in §1.3.

### 1.2 Tarockordnung

Absteigende Stichkraft:

```
Sküs > XXI > XX > XIX > XVIII > XVII > XVI > XV > XIV > XIII > XII > XI > X > IX > VIII > VII > VI > V > IIII > III > II > I
```

* Der **Sküs** trägt keine Nummer und wirkt „als wäre er ein Tarock XXII“ [KRUEPL §The tarocks]. Er ist
  **kein** Narr/Excuse des französischen Tarot: er unterliegt dem Tarockzwang, sticht normal und kann von
  nichts überstochen werden. Genau diese Umwandlung gilt als eine der vier österreichischen Tarockreformen
  [WP‑DE‑TAPP §Geschichte].
* Namen: `I` = **Pagat** (auch *Spatz*), `XXI` = **Mond**, `Sküs` = **Gstieß**.
  Die drei zusammen sind die **Trull** (aus frz. *tous les trois*), 1821 *Matadore* bzw. *tous les trois*
  genannt [ATT1821 §2], [WP‑DE‑TAPP §Tarock].
* Umgangssprachliche Namen weiterer Tarock (für Prämien der erweiterten Profile relevant):
  `II` = **Uhu**, `III` = **Kakadu** (auch *Kanari*, *Fleck*), `IIII` = **Marabu** (auch *Quapil*)
  [KRUEPL §Glossary].

### 1.3 Farbreihenfolgen – rot und schwarz sind verschieden

| Farben | Rangfolge hoch → niedrig |
|---|---|
| **Rot** (♥ Herz, ♦ Karo) | `K` > `D` > `C` > `B` > **`A` > `2` > `3` > `4`** |
| **Schwarz** (♠ Pik, ♣ Treff) | `K` > `D` > `C` > `B` > **`10` > `9` > `8` > `7`** |

In den schwarzen Farben sticht also der Zehner den Neuner, in den roten das As den Zweier
[ATT1821 §1], [WP‑DE‑TAPP §Farbkarten], [WP‑EN‑TAPP §Cards], [PAG‑DRLES §The Cards and their Values].

> **Merksatz für den Lernmodus:** *Schwarz zählt abwärts, Rot zählt aufwärts.* Bei Schwarz ist die **10**
> die höchste Leere, bei Rot das **As**; die kleinste Karte ist schwarz die **7**, rot die **4**.

### 1.4 Kartenwerte

Die 70 Punkte des Blattes sind in **Drittelpunkte** unterteilt [WP‑DE‑TAPP §Zählen]:

| Karte | Exakter Wert | Rechenwert | Anzahl | Summe exakt |
|---|---|---|---|---|
| Trullstücke `Sküs`, `XXI`, `I` und die vier Könige | **4⅓** | 5 | 7 | 30⅓ |
| Damen | **3⅓** | 4 | 4 | 13⅓ |
| Cavalle | **2⅓** | 3 | 4 | 9⅓ |
| Buben | **1⅓** | 2 | 4 | 5⅓ |
| „**Blatt**“ = alle übrigen Tarock (`II`–`XX`) und alle Skartindeln | **⅓** | 1 | 35 | 11⅔ |
| **Summe** | | | **54** | **70** |

Die Karten zu ⅓ Punkt heißen **Blatt**; *Blatt* und *Drittelpunkt* sind Synonyme [WP‑DE‑TAPP §Zählen].
Der Abzug von ⅔ pro Karte wertet die Bildkarten gegenüber den zahlreichen Blatt auf [WP‑DE‑TAPP §Zählen].

> **Warnung für Implementierer – Rechenfehler in der Quelle.** [WP‑DE‑TAPP §Zählen] enthält ein **falsch
> gerechnetes** Beispiel: „So sind etwa der Pagat, ein König und ein Cavall (5 + 5 + 3) − 2 + 2 − ⅔ = 12⅓
> wert.“ Richtig sind **11** Punkte (4⅓ + 4⅓ + 2⅓ = 11, bzw. 13 − 2 = 11). Das zweite Beispiel derselben
> Quelle („König, Bube, Skartindel = (5 + 2 + 1) − 2 = 6“) stimmt. Maßgeblich ist die Formel in §1.5; als
> Gegenprobe dienen die sechs korrekten Beispiele von 1821 [ATT1821 §4]: „König, Cavall, Zehner, nicht acht,
> sondern sieben; König, Dame, Scüs, nicht vierzehn, sondern zwölf; Mond, Pagat, König, nicht fünfzehn,
> sondern dreyzehn; drey Damen zehn, zwey Damen und ein Cavall neun, drey Buben vier … Drey leere Blätter
> zählen eins.“ Alle sechs erfüllen `3F − 2n` exakt und sind als Unit‑Tests zu übernehmen.

### 1.5 Das Zählverfahren („Zählung in Dreierlagen“)

**Regel** [WP‑DE‑TAPP §Zählen], [WP‑EN‑TAPP §Card points], [ATT1821 §4], [ALSCHER]:

1. Die gewonnenen Karten (Stiche **plus** Legage bzw. **plus** liegen gebliebener Talon) werden zu
   **Dreierlagen** gruppiert.
2. Je Dreierlage werden die Rechenwerte addiert und **2 abgezogen**.
3. Bleibt **eine** Karte übrig, werden von ihrem Rechenwert **⅔** abgezogen; bleiben **zwei** Karten übrig,
   werden von deren Summe **1⅓** abgezogen [WP‑DE‑TAPP §Zählen].
4. Die Teilsummen werden addiert.

**Exakte Formel für die Engine.** Für eine Kartenmenge mit *n* Karten und der Summe *F* der Rechenwerte gilt
in Dritteln

```
punkte_drittel = 3 · F − 2 · n          // ganzzahlig, keine Fließkommazahl
punkte         = punkte_drittel / 3
```

Probe für das ganze Blatt: F = 106, n = 54 → 3·106 − 2·54 = 318 − 108 = **210 Drittel = 70 Punkte**. ✔
Die Engine rechnet **ausschließlich in Dritteln** (`int`), rundet nie zwischendurch und zeigt Ergebnisse als
„39“ bzw. „35 ⅔“ an.

### 1.6 Durchgerechnetes Beispiel

Der Alleinspieler hat **8 Stiche** gemacht (24 Karten) und dazu seine **3 gelegten Karten** (Legage), also
27 Karten:

```
Sküs, XXI, I, ♥K, ♠K, ♦D, ♣D, ♥C, ♠B, ♣B  und  17 Blatt
```

**Dreierlagen:**

| Lage | Karten | Rechenwerte | − 2 | Ergebnis |
|---|---|---|---|---|
| 1 | Sküs, XXI, I | 5 + 5 + 5 = 15 | −2 | **13** |
| 2 | ♥K, ♠K, ♦D | 5 + 5 + 4 = 14 | −2 | **12** |
| 3 | ♣D, ♥C, ♠B | 4 + 3 + 2 = 9 | −2 | **7** |
| 4 | ♣B, Blatt, Blatt | 2 + 1 + 1 = 4 | −2 | **2** |
| 5–9 | je 3 Blatt | je 3 | je −2 | je 1 → **5** |
| | | | | **Summe 39** |

**Gegenprobe mit der Formel:** F = 15 + 14 + 9 + 4 + 15·1 = 57, n = 27 →
3·57 − 2·27 = 171 − 54 = 117 Drittel = **39 Punkte**. ✔
Die Gegner haben zwangsläufig 70 − 39 = **31 Punkte**.

### 1.7 Gewinnschwelle

* Der Alleinspieler braucht die **Mehrheit der 70 Punkte**. Traditionell formuliert:
  **„35 und zwei Blatt“ (35⅔)**, aufgerundet **36** [WP‑DE‑TAPP §Zählen], [WP‑EN‑TAPP §Card points],
  [WP‑EN‑DREIER §Card points].
* **35 : 35 ist *Remis* und gilt als Verlust des Alleinspielers**; ein Unentschieden gibt es nicht.
  „um es gewonnen zu haben, muß man wenigstens sechs und dreyßig zählen können“ [ATT1821 §5].
* Engine: `gewonnen ⟺ punkte_drittel_spieler ≥ 107`.
  **⚙ Flag `schwelle_drittel = 107 | 106`** – regional (u. a. vielerorts in der Steiermark, ebenso im
  polnischen Königrufen) genügen schon **35⅓** [WP‑DE‑KR §Das Zählen]. Default: **107**.

> **Wichtige Eigenschaft des Dreierspiels (implementierungsrelevant).** Bei drei Spielern umfasst jeder Stich
> 3 Karten, und der Alleinspieler legt 3 Karten (bzw. bei *Solo* keine, und der ganze 6er‑Talon geht an die
> Gegner). Beide Kartenstapel enthalten damit stets ein **Vielfaches von 3** Karten, und aus
> `3F − 2n` mit `n ≡ 0 (mod 3)` folgt: **die Punktestände sind immer ganzzahlig.** Drittelpunkte können im
> Default‑Profil gar nicht auftreten; „35 und zwei Blatt“ ist hier eine aus dem Königrufen übernommene
> Redensart. Drittel treten nur in den Profilen mit **Zwei‑ oder Einblatt‑Aufnahme** (`AT-TAPP-ILLU`,
> `AT-TAPP-42`) auf – dort ist §1.7 mit der Rundungsregel wirklich wirksam. Die Engine implementiert die
> Regel trotzdem einheitlich.

### 1.8 Hinweis zu abweichenden Zählverfahren

* **Vereinfachte Zählung** (nur für den Lernmodus, §11) nach [KRUEPL §How to count]: Jede Karte zählt ihren
  Rechenwert **einzeln**, ohne Dreierlagen und ohne Abzug – Trullstücke und Könige 5, Damen 4, Cavalle 3,
  Buben 2, **übrige Tarock 1**, **Skartindeln 0** („Important: When you count like this, Ace, Two, Three,
  Four, Ten, Nine, Eight and Seven have a value of 0 points!“). Summe des 54er‑Blattes:
  3·5 + 19·1 + 4·5 + 4·4 + 4·3 + 4·2 = **90**; Krüpl nennt als Schwelle **45** („The party with at least
  45 points wins a normal game“), also **45 von 90**.
  **⚠ Korrektur gegenüber früheren Fassungen dieses Dokuments:** Die dort genannte Schwelle „45 von 88“ war
  falsch; **88** ist die Kartensumme des **40er‑Blattes** von *Einfaches Tarock* / *Zwanzigerrufen*
  („Zählung einzeln, rote Asse und schwarze 10 zählen nichts, daher Gewinn mit 45/88“
  [ALSCHER Einfaches Tarock]) und gilt für Tapp‑Tarock nicht.
  **Nicht** für das reguläre Spiel verwenden, da die Tarife darauf nicht passen.
* **Alte Dreiertarock‑Zählung:** abgerundete Werte (4/3/2/1/0) plus **1 Punkt je Stich**
  [WP‑DE‑KR §Das Zählen]. Nur historische Notiz, nicht implementiert.
* **Dreierles (Baden)** zieht bei Restkarten pauschal **1** statt ⅔ bzw. 1⅓ ab
  [PAG‑DRLES §The Cards and their Values] – abweichend, nicht übernehmen.

---

## 2. Sitzordnung, Geben, Talon

### 2.1 Spieler, Sitzordnung, Richtung

* **Drei aktive Spieler.** Jeder Sitz kann ein Mensch oder ein Computerspieler sein.
* Sitzordnung und erster Geber werden **ausgelost** [WP‑DE‑TAPP §Vorrunde], [WP‑EN‑TAPP §Preliminaries].
  Historisches Losverfahren (für den Flavour‑Text im Lernmodus): je zwei Karten gleicher Farbe werden
  offen auf die Plätze gelegt und verdeckt gezogen; wer Tarock zieht, gibt zuerst [ATT1821 §10].
* **Geben und Spielen laufen gegen den Uhrzeigersinn.** Vorhand sitzt **rechts** vom Geber.
* Der Geber wechselt nach jedem Spiel **nach rechts**: „Immer der Spieler rechts vom Geber wird in der neuen
  Runde der neue Geber“ [WP‑DE‑TAPP §Vorrunde].
* Im Dreiertisch ist der Geber **Hinterhand** und spielt mit.

### 2.2 Vierter Spieler am Tisch

Historisch konnte ein vierter Mitspieler als **„König“** aussetzen: er gibt, wenn er an der Reihe ist,
**darf weder obere noch untere Talonhälfte vor dem Kauf sehen**, nimmt während des Spiels die Stiche der
Gegenpartei ein, zählt still mit, **gewinnt und verliert mit der Gegenpartei** des Alleinspielers und darf
sich keinerlei Winke erlauben [ATT1821 §9].

**⚙ Flag `vierter_spieler = aussetzer | koenig_1821 | aus`.** Default: **`aussetzer`** (moderne Praxis:
der Geber setzt aus, ist reiner Zuschauer, zahlt und kassiert nicht). Die Variante `koenig_1821` entspricht
dem 4‑Spieler‑Modus des badischen Dreierles, wo der nicht spielende Geber wie ein dritter Gegner abrechnet
[PAG‑DRLES §Players and Objective]. Die App unterstützt damit einen **Fünfertisch** genauso wie die
Vierervarianten: die Aussetzer‑Rotation ist ein gemeinsamer Engine‑Mechanismus.

### 2.3 Mischen, Abheben, Klopfen

1. Der Geber mischt; **der Spieler zu seiner Linken (Mittelhand) hebt ab**
   [WP‑EN‑TAPP §Dealing], [WP‑DE‑ILLU §Mischen und Teilen].
2. Statt abzuheben darf er **klopfen**. Dann erhält jeder Spieler seine **16 Karten in einem Paket**
   [WP‑EN‑TAPP §Dealing]. 1821 verlangt dafür zusätzlich die **Einwilligung der Vorhand** [ATT1821 §11].
   **⚙ Flag `klopfen_braucht_vorhand = true|false`.** Default: `false` (moderne Praxis).
3. Wurde nicht sauber abgehoben, wird neu gemischt und neu gegeben [ATT1821 §11].

**Grazer Klopf‑Variante (G)** [WP‑DE‑ILLU §Mischen und Teilen]: Nach dem Talon werden **drei Päckchen zu je
16 Karten** gelegt; Vorhand, dann Mittelhand wählen ein Päckchen, das dritte bleibt bei Hinterhand.
**⚙ Flag `klopfen_modus = ein_paket | paeckchenwahl`.** Default: `ein_paket`.

### 2.4 Geben

**Default‑Geberfolge** [WP‑DE‑TAPP §Geben], [WP‑EN‑TAPP §Dealing], [WP‑DE‑ILLU §Mischen und Teilen],
[ALSCHER Tapptarock] – **nicht** [ATT1821 §12], siehe Kasten unten:

```
1. 6 Karten verdeckt in die Tischmitte  → Talon (2 Päckchen zu 3, das zweite quer auf das erste)
2. 4 Karten an V, 4 an M, 4 an H
3. 4 Karten an V, 4 an M, 4 an H
4. 4 Karten an V, 4 an M, 4 an H
5. 4 Karten an V, 4 an M, 4 an H      → je 16 Karten
```

Damit sind 6 + 48 = 54 Karten verteilt. Die Reihenfolge der Talonkarten wird **nicht verändert**
[WP‑DE‑ILLU §Weiterer Verlauf].

**⚙ Flag `gebefolge`:**

| Wert | Ablauf | Quelle |
|---|---|---|
| `talon_zuerst_4er` (**Default**) | Talon, dann 4× reihum je 4 Karten | [WP‑DE‑TAPP §Geben], [WP‑EN‑TAPP §Dealing], [WP‑DE‑ILLU §Mischen und Teilen] („in vier Durchgängen à vier Karten“), [ALSCHER Tapptarock] |
| `talon_zuerst_8er` | Talon, dann 2× reihum je 8 Karten | [KRUEPL §Tappen] („16 cards in two rounds with 8 cards in each round“), [FURR §Deal], [WP‑DE‑POINT §Austeilen], [PAG‑DRLES §Deal] |
| `talon_zuerst_4x4_pro_spieler` | Talon, dann **je Spieler** nacheinander 2×4 Karten, in zwei Runden | **[ATT1821 §12]**, [PAG‑DRLES §Deal] (als Zählhilfe: „giving the first two batches of 4 to the player to the right, the next two to the next player“) |
| `talon_mitte_8er` | 8 je Spieler, dann Talon, dann 8 je Spieler | [WP‑EN‑DREIER §Dealing], [WP‑EN‑ILLU §Deal (2016)] |

> **⚠ Korrektur der Quellenzuordnung.** Frühere Fassungen führten [ATT1821 §12] als Beleg für
> `talon_zuerst_4er`. Das ist **falsch**. Die Quelle lautet: „Beym Kartengeben werden zuerst sechs Blätter …
> verdeckt als Talon auf die Seite gelegt, dann erhält **die ersten zwey Mahl vier**, der Mitspieler zu
> rechten, hierauf der mittlere, zuletzt der zur linken … dann jeder in derselben Ordnung **die zweyten zwey
> Mahl vier**; mithin hat jeder sechszehn Karten.“ 1821 gibt also **jedem Spieler nacheinander 8 Karten in
> zwei Päckchen zu 4**, und das zweimal – das ist weder 4×4 reihum noch 2×8 reihum, sondern der eigene Wert
> `talon_zuerst_4x4_pro_spieler`. Er ist im Profil `AT-TAPP-1821` Default.

Die vier Varianten sind spieltechnisch gleichwertig; `talon_mitte_8er` erschwert das Erkennen von
Mischfehlern weniger gut und ist deshalb nicht Default.

### 2.5 Der Talon (das „Tapp“)

* **6 Karten, verdeckt, in zwei Hälften zu je 3.** Das Wort *Tapp* bezeichnet ursprünglich genau diese
  verdeckten Karten in der Tischmitte [WP‑DE‑TAPP §Etymologie]; die 1821er Quelle nennt sie *Scat*
  [ATT1821 §Erklärung].
* Die beiden Päckchen werden **kreuzweise** übereinandergelegt, damit *obere* und *untere* Hälfte eindeutig
  sind [FURR §Deal], [WP‑EN‑TAPP §Dealing]. Die App zeigt sie als zwei getrennte Stapel „obere Hälfte“ und
  „untere Hälfte“ an; ein Kreuzlege‑Bild ist in der UI nicht nötig.
* **Es gibt keine offenen Handkarten und keine offen ausgelegten Karten** im Tapp‑Tarock – weder beim Geben
  noch im Spiel. Offen wird nur (a) der Talon zur Aufnahme (§4), (b) ein erzwungen abgelegtes Tarock (§4.3)
  und (c) der Pagat bei angesagtem Ultimo, der sichtbar zur Linken liegt (§5.2).

### 2.6 Fehlgeben und Sonderfälle beim Geben

| Fall | Regel | Quelle | Flag |
|---|---|---|---|
| Falsche Kartenzahl, aufgedeckte Karte, Karte in den Talon geworfen | Neu geben durch denselben Geber; historisch zahlte der Geber die halbe Consolation | [ATT1821 §13], [PAG‑DRLES §Irregularities] | `fehlgeben_strafe = keine \| halber_grundwert` (Default `keine`) |
| Ein Spieler hat **kein einziges Tarock** | Er zeigt sein Blatt, es wird neu gegeben | dafür: [WP‑EN‑ILLU §Deal (Löw 1954)] („If, after the deal, a player has no Tarocks in his hand, he shows his hand and the dealer re‑deals“), [PAG‑SLO §Hand with no taroks] · **dagegen: [ATT1821 §47]** | `neugeben_ohne_tarock = true` (**Default `true`**, siehe Kasten) |
| Ein Spieler hat höchstens zwei Tarock, beide unter `X` | Annullierung möglich | [PAG‑SLO §Variations of the three player game] – dort ausdrücklich nur „**some** play that …“ | `neugeben_schwache_tarock = false` (Default) |

> **⚠ Quellenkonflikt, der in früheren Fassungen fehlte.** Die **älteste** Quelle lehnt die Neugabe bei
> tarockfreiem Blatt ausdrücklich ab: „Zu den, im Tarok‑Tappen **von der Willkühr eingeführten Regeln**,
> gehört in vielen Gegenden auch die, daß, wenn in einer Hand gar keine Taroks stehen, kein Spiel Statt
> finde. – Worauf sich dieser Grundsatz stütze? ist schwer einzusehen … **Die wahre Regel läßt dabey gar
> keinen Unterschied obwalten, man muß die Karten annehmen, wie der Zufall sie austheilt**“ [ATT1821 §47].
> 1821 argumentiert sogar, ein tarockfreies Blatt sei für die **Gegenpartei** vorteilhaft, weil sie „durch
> das Schmieren seiner zahlreichen Cavallerie um so mehr Schaden thun“ könne.
> Ebenfalls zu beachten: [PAG‑SLO §Hand with no taroks] koppelt die Neugabe an ein **Pflicht‑Klop**
> („there is a new deal by the same dealer, and a game of **compulsory klop** is played“) – die Neugabe
> steht dort also nicht allein.
> **Entscheidung:** Default bleibt `true` (moderne Praxis, verhindert Frustblätter im Anfängerbetrieb und
> ist die Regel beider modernen Dreier‑Quellen), aber das Flag ist in `AT-TAPP-1821` auf **`false`** zu
> setzen, und im Lernmodus ist die 1821er Gegenposition als Geschichtsnotiz zu zeigen.
> Im Profil `AT-TAPP-1821` ist zusätzlich die dort gerügte Hausregel **„Zehn Taroks als Honneur“** nicht
> implementiert: „Zehn Taroks als Honneur zu rechnen, ist bloß von der Willkühr eingeführt worden, und nur
> an einigen wenigen Orten gebräuchlich. Die ursprünglichen Regeln des Spieles melden nichts davon“
> [ATT1821 §46]. (Im badischen Dreierles ist genau diese Prämie – *Zehn Druck* – dagegen Standard
> [PAG‑DRLES §Card Combinations].)

---

## 3. Spielstufen und Lizitation

### 3.1 Die Spielstufen des Default‑Profils `AT-TAPP-KLASSIK`

In aufsteigender Reihenfolge – die Reihenfolge ist zugleich die **Lizitleiter**:

| # | Ansage | Talon für den Spieler | Wert | Quellen |
|---|---|---|---|---|
| (0) | *(Tapper)* | alle 6 Karten; wurde in der Regel **nicht ausgespielt**, der Ansager schrieb sich die halbe Dreier‑Consolation gut | 1 | [ATT1821 §5], [ALSCHER] – **Default: deaktiviert** |
| 1 | **Dreier** | **eine frei gewählte** Talonhälfte (3 Karten) | **3** | [ALSCHER], [WP‑EN‑TAPP], [KRUEPL], [FURR], [WP‑DE‑TAPP] |
| 2 | **Unterer** (*„die Unteren“*) | die **untere** Hälfte (3 Karten) | **4** | dieselben |
| 3 | **Oberer** (*„die Oberen“*) | die **obere** Hälfte (3 Karten) | **5** | dieselben |
| 4 | **Solo** | **kein** Talon; alle 6 Karten gehen an die Gegner | **8** | dieselben |

Ergänzend sind **Valat** (§5.5) und **Pagat ultimo** (§5.2) keine Lizitstufen, sondern Ansagen, die zum
gewonnenen Kontrakt hinzukommen.

> **„Der Tapper“.** Die Ansage *„ich tappe“* war die niedrigste Stufe des Ur‑Spiels und gab dem Namen des
> Spiels seinen Ursprung; weil sie bald aus der Mode kam, wurde das Spiel nach der nächsthöheren Stufe
> *Dreiern / Dreierl* benannt [WP‑DE‑TAPP §Etymologie]. Schon 1821 wurde ein Tapper üblicherweise nicht
> ausgespielt, sondern sofort ausbezahlt [ATT1821 §5]; 1920 heißt es, er werde „kaum noch gespielt“
> [WP‑EN‑TAPP §History]. **⚙ Flag `tapper = aus | sofort_ausbezahlt | ausgespielt`.** Default: `aus`.
> Bei `ausgespielt` entspricht der Tapper dem **Sechserdreier**: alle sechs Talonkarten werden aufgenommen,
> sechs Karten gelegt.

**Warum ist der *Oberer* mehr wert als der *Unterer*, obwohl beide Hälften gleichwertig sind?**
Sie sind es. Die Stufen bilden lediglich eine **Überbietungsleiter**: „‚Die Oberen‘ ist nicht schwieriger zu
spielen als ‚Die Unteren‘, es ist nur unwahrscheinlich, dass zwei Spieler so gute Karten haben“
[KRUEPL §Bidding]. Der Mehrwert bezahlt das Risiko, sich **vor Aufdecken des Talons** auf eine bestimmte
Hälfte festzulegen. Das ist im Lernmodus ausdrücklich zu erklären (§11.2).

### 3.2 Ablauf der Lizitation

1. **Vorhand eröffnet.** Sie sagt „Dreier!“ (bzw. eine höhere Stufe) oder **„Weiter!“** (= passen; historisch
   „passé“ oder „tappen“) [WP‑DE‑TAPP §Auktionen], [WP‑EN‑TAPP §Bidding], [KRUEPL §Bidding].
2. Danach ist **Mittelhand**, dann **Hinterhand**, dann wieder Vorhand an der Reihe (gegen den Uhrzeigersinn).
3. Ein Spieler, der an der Reihe ist, hat drei Möglichkeiten:
   * **Passen** („Weiter!“) – er scheidet **endgültig** aus der Lizitation aus
     [WP‑EN‑TAPP §Bidding], [WP‑DE‑POINT §Reizen].
   * **Überbieten** – er nennt die **nächsthöhere** Stufe. Ein Überspringen von Stufen ist nicht erlaubt,
     **Ausnahme: „Solo“ darf jederzeit sofort genannt werden** [WP‑EN‑TAPP §Bidding].
   * **Halten** („Halte ich!“) – nur, wenn er **vor** dem aktuellen Höchstbietenden zu sprechen hatte
     (Senioritätsprinzip). Er übernimmt den Kontrakt zum **gleichen Wert** und ist damit Höchstbietender;
     wird er erneut überboten, darf er erneut halten [WP‑EN‑TAPP §Bidding], [WP‑DE‑POINT §Reizen].
4. **Ende:** Die Lizitation endet, sobald **zwei Spieler nacheinander gepasst** haben
   [WP‑DE‑TAPP §Auktionen] – gleichbedeutend damit, dass nur noch ein Spieler im Bieten ist.
   Nach einem **Solo** endet die Lizitation sofort: „sobald das Wort Solo ertönt, hört jedes weitere
   Steigern auf“ [ATT1821 §E.3]; ein Solo kann **nicht gehalten** werden
   [WP‑EN‑ILLU §Auction (1954): „once a player announces a ''Solo'' the auction ends immediately; there is
   no option to take it over or hold“], [FURR §Bidding: „Any player may bid for Solo and end the bidding“].
   **⚙ Flag `solo_beendet_lizit = true`** (Default).

   > **⚠ Die Quelle von 1821 widerspricht sich hier selbst** (in früheren Fassungen nicht vermerkt).
   > §E.3 sagt, ein Solo beende jedes Steigern. §19 sagt dagegen ausdrücklich: „diesen [den Solo] darf die
   > Vorhand, **welche einen Tapper angesagt hat**, nicht mehr selbst spielen, **wohl aber, wenn sie sich zu
   > einem Dreyer gemeldet hätte**, und Einer aus der Hinterhand declarirte einen Solo.“ Nach §19 darf also
   > eine Vorhand, die zuvor einen **Dreyer** angesagt hat, einen fremden Solo **an sich ziehen**; nur nach
   > einem bloßen *Tapper* darf sie das nicht. Das ist ein echter Binnenwiderspruch der ältesten Quelle,
   > kein Übertragungsfehler. **Empfehlung: Default `solo_beendet_lizit = true`** (alle modernen Quellen);
   > im Profil `AT-TAPP-1821` steht das Flag auf **`false`**, und es gilt die Zusatzbedingung
   > **⚙ `solo_halten_nur_nach_dreyer = true`** – nur ein Spieler, der bereits einen *Dreyer* (nicht nur
   > einen *Tapper*) angesagt hatte, darf den Solo halten.
5. Der Höchstbietende ist der **Alleinspieler** („Spieler“, 1821 auch *Feind*); die beiden anderen sind die
   **Gegenspieler** (1821 *Freund, Nebenmann, Helfer*) [ATT1821 §Erklärung].

**⚙ Flag `halten = seniorität | nur_vorhand | aus`.** Default: **`seniorität`**.

Quellenlage (gegenüber früheren Fassungen berichtigt und ergänzt):

* **Nur die Vorhand darf halten:** [WP‑DE‑TAPP §Auktionen] („Er gibt ein Gebot ab und darf **als einziger**,
  wenn er wieder an der Reihe ist, das zuletzt abgegebene Gebot halten“) · **[KRUEPL §Bidding]** („Note that
  **the forehand** has a forth possibility to declare: ‚Ich halte‘“) · [ATT1821 §19] (nur die Vorhand zieht
  das Spiel an sich).
* **Jeder früher sprechende Spieler darf halten (Senioritätsprinzip):** [WP‑EN‑TAPP §Bidding] · [WP‑DE‑POINT
  §Reizen] („Ein Spieler, der von einem **nachsitzenden** Spieler überboten wurde, darf halten … und hat
  Priorität, sofern er nicht erneut überboten wird“) · [WP‑EN‑DREIER §Bidding] · [FURR §Bidding] („An
  **elder** player who is outbid may call ‚I hold‘“) · **[PAG‑SLO §The three‑player game]** („players whose
  first turn to speak was **earlier** have seniority in the bidding, and can equal the bids of junior
  players by holding“).

Damit steht es 3 : 5; der Default **`seniorität`** bleibt, ist aber knapper begründet, als §9.6 bisher
behauptete. Für Dreiertische ist der Unterschied klein: Wer nicht gepasst hat, hat auf seinem Zug immer
bereits geboten, sodass „früher gesprochen“ und „überboten worden“ in der Praxis zusammenfallen; der
sichtbare Unterschied betrifft nur den Fall, dass **Mittelhand** ein Gebot der **Hinterhand** halten will.
Siehe §9.6.

**Zustandsautomat (Engine).**

```cpp
struct BidState {
    int  level      = 0;      // 0 = noch kein Gebot
    int  holderSeat = -1;     // Sitz des Höchstbietenden
    std::array<bool,3> passed{false,false,false};
    int  turn       = SEAT_VORHAND;
};

// Rang: 0 = Vorhand, 1 = Mittelhand, 2 = Hinterhand (Sprechreihenfolge der 1. Runde)
bool mayHold(const BidState& s, int seat) {
    return s.level > 0 && !s.passed[seat] && seat != s.holderSeat
        && rank(seat) < rank(s.holderSeat);
}
bool mayBid(const BidState& s, int seat, int lvl) {
    if (s.passed[seat]) return false;
    if (lvl == LVL_SOLO) return lvl > s.level;          // Sprung erlaubt
    return lvl == s.level + 1;                          // sonst genau eine Stufe
}
bool auctionOver(const BidState& s) {
    int active = 0; for (int i=0;i<3;++i) if (!s.passed[i]) ++active;
    return (s.level > 0 && active == 1) || (s.level == 0 && active == 0);
}
```

**⚙ Flag `sprung_erlaubt = nur_solo | immer`.** Default: `nur_solo`.

### 3.3 Wer darf was ansagen?

* **Jeder Spieler** darf jede Spielstufe lizitieren; es gibt im Tapp‑Tarock **kein Vorhandspiel** und keine
  Muss‑Ansage (anders als im Königrufen, wo die Vorhand einen Rufer ansagen muss)
  [WP‑DE‑TAPP §Auktionen], [WP‑DE‑KR §Vorhandspiele].
* **Prämienansagen** (§5) macht im Default‑Profil **nur der Alleinspieler**; die Gegenspieler antworten mit
  „Gut“ oder **Kontra** [WP‑EN‑TAPP §Announcements], [WP‑DE‑ILLU §Weiterer Verlauf].
  **⚙ Flag `ultimo_ansage_durch = spieler | alle`** – bei Löw und Beck dürfen **alle** Spieler Pagat bzw.
  Uhu ansagen, die Prämie zählt dann individuell [WP‑EN‑ILLU §Announcements (1954)]. Default: `spieler`.
* **Handprämien** (Trull, Königstrull, Köpfe) gebühren **jedem Inhaber**, auch einem Gegenspieler
  [ATT1821 §2], [WP‑EN‑TAPP §Game values and bonuses].

### 3.4 Reihenfolge der Ansagephasen (verbindlich für die UI)

```
Lizitation
  └─ (Ansagen VOR Talonaufnahme)              nur in AT-TAPP-ILLU / AT-POINT / wenn flag_ansage_vor_kauf
        └─ Kontra der Gegner darauf
  └─ Talon aufdecken → aufnehmen → verlegen        (entfällt bei Solo und Negativspielen)
  └─ Spieler erklärt „Ich liege“
        └─ Ansagen NACH Talonaufnahme (Pagat ultimo, Valat, …)
              └─ Kontra der Gegner, im Uhrzeigersinn: „Gut“ oder „Kontra“
                    └─ Rekontra des Spielers  →  Subkontra der Gegner
  └─ Vorhand spielt zum ersten Stich aus
```

Belege: „Nach dem Legen des Spielers kann gekontert werden“ [WP‑DE‑TAPP §Kontra]; „Spieler erklärt sodann
‚Ich liege‘ … Die Gegenspieler erklären im Uhrzeigersinn ‚Gut‘ oder kontrieren. Ist der Vorgang des
Kontrierens abgeschlossen, spielt Vorhand zum ersten Stich aus“ [WP‑DE‑ILLU §Weiterer Verlauf].

**⚙ Flag `ansage_vor_kauf = aus | an`.** Default im Klassik‑Profil: **`aus`** (es gibt nur eine Ansagephase,
nach dem Verlegen). In `AT-TAPP-ILLU` und `AT-POINT`: `an`, mit erhöhten Prämienwerten (§5.9).

### 3.5 Wenn alle passen

Sagt **kein** Spieler ein Spiel an, gibt es drei quellenbelegte Möglichkeiten, über die sich die Runde vorher
einigen muss [WP‑DE‑ILLU §Vorgehen bei fehlender Spielansage]:

#### 3.5.1 Neu geben
Derselbe Geber (oder der nächste – **⚙ Flag `neugeben_geberwechsel`**, Default: derselbe) gibt neu.
Das ist die strikt klassische Lösung; sie kann zu mehreren Fehlgaben hintereinander führen.

#### 3.5.2 Rauben (Räuber)
Nachdem die ersten beiden Spieler gepasst haben, darf der **Geber** bis zu sechs für ihn passende Karten
**verdeckt** (d. h. für die anderen nicht einsehbar) aus dem Talon aussuchen und **für die nächste
Austeilung** in sein Blatt nehmen; er bekommt beim nächsten Geben entsprechend weniger Karten und **muss
dann als Erster mindestens „Dreiblatt“ ansagen** [WP‑DE‑TAPP §Variante]. Die Einzelheit „entsprechend
weniger Karten“ ist nicht erschlossen, sondern belegt: „Der nächste Teiler teilt ihm dann zu Beginn des
Teilens **keine oder entsprechend weniger Karten** zu. Wurden Karten aus dem Talon behalten, verpflichtet
dies, im nun folgenden Spiel **mindestens Dreiblatt** anzusagen“ [WP‑DE‑ILLU §Vorgehen bei fehlender
Spielansage, Variante (G)]. Dort nimmt allerdings **Hinterhand** den Talon **unaufgedeckt** auf und darf ihn
auch ganz weglegen.
Die badische Parallele heißt zwar ebenfalls *Räuber*, ist aber **etwas anderes**: dort wird der Blinde
beiseitegelegt und ein Spiel „jeder gegen jeden“ gespielt [PAG‑DRLES §Räuber] – das entspricht unserem
**Trischaken** (§3.5.3), nicht dem Rauben.
**⚙ Flag `rauben = aus | an`.** Default: `aus` – der Mechanismus greift über Spielgrenzen hinweg und ist im
LAN‑Betrieb und beim Wiederaufsetzen einer Partie unangenehm zu persistieren.

#### 3.5.3 Trischaken  ⟵ **Default**
Es wird gespielt, **jeder gegen jeden**, mit dem Ziel, möglichst **wenige** Punkte zu bekommen. Regeln
[WP‑DE‑ILLU §Vorgehen bei fehlender Spielansage]:

* **Stichzwang** (§6.7).
* **Tarock `I` (Pagat) darf erst als letztes Tarock im Blatt gespielt werden.**
* **Der Talon bleibt unberührt** – er wird weder aufgenommen noch gezählt. Damit sind nur **48 Karten** im
  Spiel und die Summe aller Punkte ist **kleiner als 70 und von Deal zu Deal verschieden** (70 minus dem Wert
  der sechs Talonkarten). Die App zeigt diese Summe nach dem Spiel an.
* **Es werden keine Prämien verrechnet.**
* Verlierer ist der Spieler mit den **meisten** Punkten. Abrechnung §7.6.

**⚙ Flag `alle_passen = trischaken | neu_geben | rauben`.** Default: **`trischaken`** – Empfehlung, weil es
die Partie deterministisch weiterführt (wichtig für Turnier‑ und LAN‑Modus) und weil Trischaken die einzige
quellenbelegte Nicht‑Neugabe‑Lösung für **drei** Spieler ist.
**⚙ Flag `trischaken_talon = aus | zuwaage | letzter_stich`. Default `aus`** (Beck). `zuwaage` = je eine
Talonkarte zu den ersten sechs Stichen, `letzter_stich` = der Talon geht an den Gewinner des letzten Stichs –
beides sind Königrufen‑Praktiken [WP‑DE‑KR §Trischaken] und **für drei Spieler nicht belegt**.

### 3.6 Profil `AT-DREIER-54` – Dreiertarock (Kärnten, Mayr/Sedlaczek)

Ein eigenständiges, heute vor allem in **Kärnten** und in Slowenien gespieltes Dreierspiel; 2013 eine der
fünf Varianten des Internationalen Piatnik‑Tarockturniers in Wien [WP‑EN‑DREIER].

* **Geben:** 8 Karten je Spieler, dann 2 × 3 Karten Talon, dann nochmals 8 Karten je Spieler.
* **Lizit:** Vorhand sagt **„Einser“** oder **„Weiter“**; die folgenden Spieler passen, halten oder erhöhen
  auf **Zweier, Dreier, Vierer, Fünfer** – jede Stufe erhöht nur den **Spielwert**, die Stufen müssen
  lückenlos gereizt werden. **Solo** und **Solo Valat** müssen bei der **ersten** Sprechgelegenheit angesagt
  werden.
* **Talon:** In allen Stufen *Einser … Fünfer* werden **beide Päckchen offen ausgelegt**, der Spieler nimmt
  **eines davon** auf und legt **drei** Karten ab. Der Talon‑Zugriff ist also in allen Stufen gleich;
  gereizt wird ausschließlich der Einsatz.
* **Spielregeln:** Farbzwang und Tarockzwang, **kein Stichzwang** [WP‑EN‑DREIER §Playing].
* **Gewinn:** 36 von 70.

| Kontrakt | Wert | Prämie | Bedeutung | still | angesagt |
|---|---|---|---|---|---|
| Einser | 30 | Tarocktrull | alle 3 Trullstücke **in den Stichen** | 10 | – |
| Zweier | 40 | Königstrull | alle vier Könige **in den Stichen** | 10 | – |
| Dreier | 50 | Mondfang | den Mond mit dem Sküs fangen | 10 | – |
| Vierer | 60 | Pagat ultimo | letzter Stich mit `I` | 40 | 80 |
| Fünfer | 70 | Uhu | vorletzter Stich mit `II` | 60 | 120 |
| Solo | 120 | Kike | drittletzter Stich mit `III` | 80 | 160 |
| Solo Valat | 960 | – | – | – | – |

[WP‑EN‑DREIER §Game values and bonuses]. Die Werte sind dort „in Cent“ gerechnet.
**Hinweis:** Der Name **„Kike“** für Tarock `III` ist nur in dieser Quelle belegt; in Österreich sonst
**Kakadu** [KRUEPL §Glossary], [WP‑DE‑KR]. Die App zeigt „Kakadu (Kike)“.
**Hinweis:** Trull und Königstrull zählen hier **in den Stichen**, im Tapp‑Tarock dagegen **in der Hand** –
siehe §9.12.

### 3.7 Profil `AT-TAPP-ILLU` – Illustriertes Tarock (Beck 1972 / Grazer Runden)

* **Lizitstufen und Multiplikatoren** [WP‑DE‑ILLU §Bewertung], [WP‑EN‑ILLU §Complex variant]:

| Kontrakt | Talonaufnahme | Multiplikator |
|---|---|---|
| **Dreiblatt** | obere **oder** untere 3 Karten | × 1 |
| **Zweiblatt** | oberes, mittleres **oder** unteres **Paar** | × 2 |
| **Einblatt** | **eine** beliebige Karte | × 3 |
| **Solo** | keine | × 4 |

  Es dürfen nur **nebeneinanderliegende** Talonkarten gewählt werden [WP‑DE‑ILLU §Weiterer Verlauf].
* **Spielwert:** angesagtes Spiel („Dreieransage“) **3**, gelöstes Spiel („Unterer“) **4**, gehaltenes Spiel
  **5**. Nur das **niedrigste** Gebot (Dreiblatt) darf gelöst werden, und nur **einmal**
  [WP‑EN‑ILLU §Auction (Beck)].
* **Überschusspunkte:** Punkte über 35 werden zum Spielwert addiert (bei Verlust die Punkte unter 35 zugunsten
  der Gegner) – ein mit 40 Points gewonnenes angesagtes Spiel zählt 3 + 5 = 8
  [WP‑DE‑ILLU §Punkte für das Spiel].
* **Prämien:** §5.9, Spalte `ILLU`.
* **Alle passen:** neu geben, **Trischaken** (§3.5.3) oder Grazer Karten‑Behalten.
* **Serie:** bis 100 Spielpunkte, +10 Bonus (§8.2).
* **Achtung:** Der *Unterer* des ILLU‑Profils ist **nicht** die untere Talonhälfte, sondern das „Lösen“
  eines fremden Gebots (Kasten in §4.2).

### 3.8 Profil `AT-POINT` – Point‑Tarock (Beck)

Identisch mit Tapp‑Tarock, aber [WP‑DE‑POINT §Unterschiede, §Ansagen]:

* **kein** Zweiblatt und **kein** Einblatt; Lizitstufen **Dreier 3, Unterer 4, Oberer 5**. Ein **Solo**
  wird in der Quelle zwar erwähnt – „Mit Ausnahme von ‚Solo‘ müssen die Kontrakte in aufsteigender
  Reihenfolge geboten werden“ [WP‑DE‑POINT §Reizen] –, aber **ohne Punktwert**;
  **⚙ Flag `point_solo = an` (Default), Wert 8** (aus dem Tapp‑Tarock übernommen, ⚠ nicht quellenbelegt);
* der Alleinspieler wählt hier **frei** zwischen den Talonhälften, unabhängig von der Lizitstufe
  („nimmt entweder die ersten oder die letzten drei Karten vom Talon“ [WP‑DE‑POINT §Reizen]) –
  das Profil setzt deshalb `talon_lizit_semantik = freie_wahl`;
* nach dem Talontausch sagt der Alleinspieler eine **Zielpunktzahl** an:

| Ansage | Spielpunkte |
|---|---|
| „mit 40“ | 3 |
| „mit 45“ | 6 |
| „mit 50“ | 10 |
| „mit 55“ | 15 |

* Die Ansagen sind **kumulativ**: „mit 50“ schließt „mit 45“ und „mit 40“ ein, und jede Teilansage wird
  **einzeln** abgerechnet und kann **einzeln kontriert** werden („Kontra die 50!“).
* **Beispiel aus der Quelle:** Ansage „mit 50“, erreicht 46 Points → der Spieler verliert die 50 (−10) und
  gewinnt die 45 (+6) und die 40 (+3), zahlt also **1 Punkt** an jeden Gegner [WP‑DE‑POINT §Gewinn].
  [FURR §Illustrated Tapp] rechnet dasselbe Beispiel mit 47 Points identisch.
* Boni: Pagat ultimo und Valat wie im Klassik‑Profil.

### 3.9 Profil‑Aufsatz `AT-TAPP-NEGATIV` – Piccolo, Bettler, Ouvert  ⚠ **nicht quellenbelegt**

> **Warnhinweis, der auch in der App angezeigt wird:** Für **Tapp‑Tarock** sind Bettler und Piccolo in keiner
> der in §0.2 geprüften Quellen belegt. Sie stammen aus dem **Königrufen** zu viert [WP‑DE‑KR §Negativspiele]
> bzw. – der Bettler – aus dem **slowenischen Tarock zu dritt** (*berač*, *odprti berač*)
> [PAG‑SLO §The three‑player game]. Die folgenden Tarife sind eine **Entwurfsentscheidung** von uns, keine
> überlieferte Regel.

| Ansage | Bedeutung | Vorschlagswert | Rang im Lizit |
|---|---|---|---|
| **Piccolo** | der Spieler macht **genau einen** Stich | 6 | über Oberer |
| **Bettler** (Bettel) | der Spieler macht **keinen** Stich | 7 | über Piccolo |
| **Solo** | (unverändert) | 8 | über Bettler |
| **Piccolo ouvert** | wie Piccolo, ab dem 2. Stich mit offenen Karten des Spielers | 10 | über Solo |
| **Bettler ouvert** | wie Bettler, ab dem 2. Stich offen | 12 | höchstes Spiel |

Begründung der Einordnung: Im slowenischen Dreierspiel liegt der *berač* **zwischen** „ena“ (der kleinsten
Talonaufnahme) und „solo brez talona“, der *odprti berač* über allem [PAG‑SLO]. Diese Ordnung wird
übernommen; Piccolo wird darunter eingehängt, weil er leichter ist als der Bettler.

**Regeln für alle Negativspiele** (übernommen von [WP‑DE‑KR §Negativspiele], [PAG‑SLO §Additional rules for
negative contracts]):

* **Der Talon nimmt nicht am Spiel teil** und wird **nicht gezählt**.
* **Der Alleinspieler spielt zum ersten Stich aus** (nicht die Vorhand).
* **Stichzwang**: Man muss die bisher höchste Karte des Stichs übertreffen, wenn man kann (§6.7).
* **Der Pagat darf nur gespielt werden, wenn er die einzig mögliche Karte ist** – als letztes Tarock im
  Blatt, als letzte Karte, oder wenn nur er den Stich gewinnen kann (Kaiserstich).
* Ein Negativspiel ist **sofort verloren**, sobald die Bedingung rechnerisch nicht mehr erfüllbar ist
  (Bettler: erster Stich des Spielers; Piccolo: zweiter Stich des Spielers oder letzter Stich ohne bisherigen
  Stich). Die Engine beendet das Spiel dann und rechnet ab – das spart Zeit und ist in Turnieren üblich.
* Kontra wirkt bei Negativspielen **individuell** je Gegenspieler [WP‑DE‑KR §Kontra].

**⚙ Flag `negativspiele = aus | piccolo_bettler | mit_ouvert`.** Default: **`aus`**.

### 3.10 Profil `AT-TAPP-1821` – historisches Tarok‑Tappen

Für den Geschichtsteil des Lernmodus und als spielbarer Modus:

* Ansagen: nur **Tapper**, **Dreyer**, **Solo** [ATT1821 §Einleitung].
* **Dreyer:** der Spieler kauft die **drei obersten** Talonkarten – **keine Wahl** und **ohne Aufdecken des
  Talons** [ATT1821 §20]. Im Profil steht deshalb `talon_aufdecken = nur_bei_dreier` faktisch auf „nie“:
  **⚙ `talon_1821 = oben_verdeckt`**.
* **Tapper:** **nur die Vorhand** darf ihn ansagen – „Tapper sagt **nur die Vorhand** an, und wird, wenn
  niemand etwas Besseres versuchen will, gewöhnlich nicht gespielt, sondern die Vorhand schreibt sich die
  Hälfte der festgesetzten Dreyer‑Consolation auf“ [ATT1821 §Erklärung, §5, §19].
  **⚙ Flag `tapper_nur_vorhand = true`** (nur in diesem Profil).
  Regionale Zusatzregel, von der Quelle gerügt: mancherorts musste man für einen Tapper in der Vorhand
  „wenigstens eine Honneur haben“; 1821 nennt das „von der Willkühr geschaffen“ und rät ab
  [ATT1821 §19 Anmerkung]. **Nicht implementieren.**
* **Kein Kontra.** Die Quelle von 1821 kennt **weder Kontra noch Rekontra noch Subkontra**; die einzigen
  Vorkommen von „contra“ sind *contra ultimo* (§5.2) und *contra Volat* (die Gegenpartei macht den Volat).
  Im Profil `AT-TAPP-1821` ist `kontra_max_stufe = 0`.
* **Consolation:** ein vor der Partie vereinbarter Grundwert, üblich **50, 40 oder 30** [ATT1821 §5].
* **Überschusspunkte doppelt:** „Jeder Point, der über die fünf und dreyßig ist, wird im Dreyer doppelt
  gerechnet“ – mit 37 Points ist „um zwei gewonnen“, bei Consolation 50 schreibt man **54**
  [ATT1821 §5].
* **Solo:** Consolation **doppelt**, Punkte **vierfach**; ein um 3 gewonnener Solo ergibt 112 [ATT1821 §6].
* **Honneurs:** *tous les trois* und *vier Könige* zählen je eine einfache Consolation, im Solo ebenfalls nur
  die einfache [ATT1821 §2, §6].
* **Pagat ultimo:** still = Consolation, angesagt = doppelt; im Solo still = doppelt, angesagt = vierfach
  [ATT1821 §7]. **Contra ultimo** (ein Gegenspieler macht ihn) zählt für **beide** Gegenspieler.
* **Volat:** „so viel hunderte, als die angenommene Consolation Zehner hat“ – bei Consolation 50 also **500**,
  Solo‑Volat **1000**; Honneurs und Pagat ultimo werden **zusätzlich** gerechnet [ATT1821 §8]. Ein Volat wird
  **nie angesagt**; misslingt er, bleibt es ein gewonnener Dreyer/Solo.
* **Remis** (35 : 35): der Spieler schreibt den Gegnern die volle Consolation gut [ATT1821 §Erklärung, §5].

### 3.11 Profil `AT-TAPP-42` – Tapp‑Tarock mit 42 Blatt

„Das Tarok‑Tappen wird in einigen Gegenden mit vier und fünfzig, in anderen nur mit zwey und vierzig Karten
gespielt“ [ATT1821 §Einleitung]; verbreitet war das vor allem „in Ungarn, Böhmen und Mähren, besonders auf
dem flachen Lande“ [ATT1821 §E.3].

* **Blatt:** Es fehlen in Herz und Karo die `2, 3, 4`, in Pik und Treff die `7, 8, 9`
  [ATT1821 §E]. Bleiben 22 Tarock + 4 × 5 Farbkarten = **42 Karten**.
* **Geben:** 6 Talon + 3 × 12 Karten, und zwar **zwei Runden zu je zweimal drei Karten pro Spieler**:
  „Da nur zwölf Blätter ausgegeben werden, gibt man zwey Mahl herum und zwar jedes Mahl – nach zuvor
  gelegten sechs Karten in den Talon – zwey Mahl drey Karten jedem der Mitspieler, um sich bestens vor allem
  Vergeben zu bewahren“ [ATT1821 §E.7]. **⚙ Der Kartensatz ist derselbe wie beim ungarischen Tarokk**
  (`HU-42`), die App braucht kein zusätzliches Deck.
* **Zusätzliche Stufen und die vollständigen Tarife der Quelle** [ATT1821 §Erklärung, §E.3, §E.5]
  (die Quelle rechnet im Beispiel mit **Dreyer‑Consolation 30**):

| Ansage | Kauf/Legage | Consolation | Volat |
|---|---|---|---|
| Tapper | – (nur Vorhand) | halbe Dreyer‑Consolation = **15** | – |
| **Dreyer** (*Präference*) | 3 Karten | **30** | **300** |
| **Zweyer** (*Due*) | 2 Karten | **50** | – |
| **Einer** (*Uno*) | 1 Karte | **100** | – |
| **Solo** | keine | **150** | **600** |

* **Punktemultiplikatoren** (in früheren Fassungen dieses Dokuments **fehlend**): „Die gewonnenen oder
  verlorenen Points werden im **Dreyer doppelt, im Zweyer dreyfach, im Einer vierfach, im Solo aber
  sechsfach** gezählt“ [ATT1821 §E.6]. (Im 54er‑Spiel sind es nur ×2 im Dreyer und ×4 im Solo, §3.10.)
* **Kartensumme und Gewinnschwelle sind andere** (in früheren Fassungen **fehlend**): „Da zwölf leere
  Blätter aus dem Spiele beseitigt werden, zählt die ganze Karte auch vier Points weniger, mithin **sechs
  und sechzig**, **drey und dreyßig ist Remi**, **vier und dreyßig um eins gewonnen**“ [ATT1821 §E.6].
  Engine: `gesamtpunkte_drittel = 198`, `schwelle_drittel = 102` (= 34 Punkte). Probe: 12 entfernte Blatt
  × ⅓ = 4 Punkte, 70 − 4 = 66 ✔
* **Frei zu vereinbaren** bleiben in diesem Profil „der Werth von vier Königen, tous les trois, schweigender
  und angesagter Pagat ultimo; eben so die Strafe für das Vergeben der Karten“ [ATT1821 §E.6]; die App
  schlägt die 54er‑Werte vor.
* Warnung der Quelle: Mit 42 Blatt sind Tarock und Honneurs im Verhältnis zu den Leeren zu zahlreich, die
  Spiele fallen hoch aus und der **Volat wird viel leichter**; man soll deshalb einen **niedrigeren
  Maßstab** ansetzen [ATT1821 §E.2, §E.3]. Genau deshalb setzt die Quelle den Volat hier auf 300/600 statt
  auf 500/1000 wie im 54er‑Spiel: „Den Volat haben wir absichtlich geringer angenommen“ [ATT1821 §E.7].
* Die Punkte‑Stapel enthalten hier keine Vielfachen von 3 mehr (Zweyer/Einer) – die Drittelregel aus §1.7
  wird wirksam.

---

## 4. Der Talon: Aufnahme, Vorzeigepflicht, Legage

### 4.1 Grundprinzip

Der Talon zerfällt in zwei Hälften zu je 3 Karten.

* Der Alleinspieler nimmt **genau eine Hälfte** (3 Karten) in sein Blatt und legt dafür **3 Karten verdeckt**
  ab (die **Legage**, 1821 *Scat*).
* **Die Legage zählt zu den Stichen des Alleinspielers.**
* **Die nicht genommene Hälfte zählt zu den Stichen der Gegenspieler**, wird verdeckt beiseitegelegt und
  **darf bis zum Ende des Spiels von niemandem angesehen werden**.
* Bei **Solo** bleibt der **ganze Talon** liegen und zählt **vollständig für die Gegenspieler**.

Belege: [WP‑EN‑TAPP §Bidding], [ALSCHER Tapptarock], [KRUEPL §Playing], [FURR §Bidding],
[ATT1821 §22], [WP‑DE‑TAPP §Anzahl Talonkarten].

> 1821 formuliert es so: „Die Points, welche der Spieler in den Scat legt, zählen ihm eben sowohl, als der
> Gegenpartey jene, welche in dem liegen gebliebenen Talon befindlich sind, nur darf diesen, Keiner von den
> drey zusammen Spielenden eher, als nach dem letzten Stiche ansehen“ [ATT1821 §22].

### 4.2 Welche Hälfte bekommt der Spieler? — der strittigste Punkt des Spiels

Die Quellen kennen **drei** Lesarten:

| Lesart | Dreier | Unterer | Oberer | Talon offen? | Quellen |
|---|---|---|---|---|---|
| **A – Hälfte festgelegt** ⟵ **Default** | freie Wahl zwischen beiden Hälften | **muss** die untere Hälfte nehmen | **muss** die obere Hälfte nehmen | bei [KRUEPL] ja, bei [FURR]/Unger/[WP‑DE‑KR] **nein** (§9.5) | [KRUEPL §Bidding], [FURR §Bidding], Unger 1923 und Piatnik‑Beiblatt nach [WP‑EN‑TAPP Fn.], Parlett 2008 nach [WP‑EN‑TAPP Fn.], Alscher 2003 nach [WP‑EN‑TAPP Fn.], [WP‑DE‑KR §Oberer, Unterer], **[WP‑DE‑POINT §Reizen Fn.]** |
| **B – freie Wahl immer** | freie Wahl | freie Wahl, nur höherer Tarif | freie Wahl, nur höherer Tarif | ja | [WP‑EN‑TAPP §Bidding Haupttext] (= Mayr/Sedlaczek 2001), [ALSCHER] („oder wie Dreier“), [WP‑EN‑DREIER §Bidding] |
| **C – Blattzahl** | 3 Karten | = *Zweiblatt*: 2 Karten | = *Einblatt*: 1 Karte | ja | [WP‑DE‑TAPP §Anzahl Talonkarten] |

**Empfehlung und Default: Lesart A.** Begründung:

1. Nur unter A haben die Namen *Unterer* und *Oberer* überhaupt eine Bedeutung – sie bezeichnen **die
   Talonhälfte**, nicht die Kartenzahl [KRUEPL §Glossary: „Die Unteren – the lower three cards of the talon“].
2. A ist von den meisten voneinander unabhängigen Quellen belegt (Unger 1923, Piatnik, Parlett, Alscher,
   Krüpl, Furr) und wird auch in der Königrufen‑Literatur ausdrücklich als die im **Dreiertarock** bekannte
   Variante benannt [WP‑DE‑KR §Weniger häufig gespielte Varianten].
3. A macht die drei Stufen mechanisch unterscheidbar, was für Lizit‑KI und Lernmodus wertvoll ist. Das
   Mehrrisiko ist real, weil man sich **vor** dem Aufdecken auf eine Hälfte festlegt.
4. Lesart C ist mit hoher Wahrscheinlichkeit eine **Verwechslung mit dem Illustrierten Tarock**
   (dort heißen die Stufen tatsächlich Dreiblatt/Zweiblatt/Einblatt); [WP‑DE‑TAPP] setzt dort sogar
   „Unterer (Zweiblatt)“ und „Oberer (Einblatt)“ gleich, was historisch nicht haltbar ist. Siehe §9.3.
5. **Nachgetragener Beleg:** [WP‑DE‑POINT §Reizen] beschreibt für Point‑Tarock die freie Wahl („nimmt
   entweder die ersten oder die letzten drei Karten“), merkt dazu aber in einer Fußnote ausdrücklich an:
   „Die Namen der beiden höheren Gebote – *Unterer* und *Oberer* – … beziehen sich möglicherweise auf eine
   Zeit, in der der Alleinspieler die untere bzw. obere Hälfte des Talons aufnehmen musste. Diese Regel wird
   **manchmal noch in anderen Varianten des Tarocks zu dritt verwendet**.“ Damit stützt selbst eine Quelle
   der Lesart B die historische Begründung von Lesart A.

> **Achtung Namensgleichheit (Fallstrick für die Profilimplementierung).** Im **Illustrierten Tarock** hat
> das Wort *Unterer* eine **völlig andere** Bedeutung: dort ist es die Ansage, ein von einem anderen
> Spieler angesagtes Spiel **selbst spielen zu wollen** („das Spiel lösen“), und erhöht den Spielwert von 3
> auf 4 [WP‑DE‑ILLU §Lizitation], [WP‑EN‑ILLU §Auction (Beck)]. Mit der Talonhälfte hat der *Unterer* des
> ILLU‑Profils **nichts** zu tun. Die Engine darf die beiden Begriffe niemals auf denselben Enum‑Wert
> abbilden; in der UI ist im ILLU‑Profil „Lösen“ statt „Unterer“ anzuzeigen.

**⚙ Flag `talon_lizit_semantik = haelfte_fest | freie_wahl | blattzahl`.** Default: `haelfte_fest`.
**⚙ Flag `talon_aufdecken = immer | nur_bei_dreier`.** Default: `immer`.
Bei `nur_bei_dreier` entspricht die App der ältesten Praxis: in *Unterer* und *Oberer* nimmt der Spieler
seine Hälfte **ohne Aufdecken**, und die andere Hälfte wird erst am Ende geöffnet
[FURR §Bidding: „Bottom 3: As ‚3 Cards‘ but Declarer takes the bottom 3 cards without revealing the stock“],
[WP‑DE‑KR §Oberer, Unterer].

### 4.3 Was darf nicht gelegt werden?

**Verbindliche Legage‑Regeln** [ALSCHER Tapptarock], [WP‑DE‑TAPP §Anzahl Talonkarten],
[WP‑EN‑TAPP §Bidding], [ATT1821 §22], [FURR §Bidding], [WP‑DE‑ILLU §Weiterer Verlauf],
[WP‑EN‑ILLU §Auction]:

1. **Keine Könige.**
2. **Kein Trullstück** (`Sküs`, `XXI`, `I`).
3. **Tarock nur im Notfall**, d. h. nur, wenn der Spieler nach Aufnahme der Talonhälfte nicht genügend
   erlaubte Farbkarten (Nicht‑Könige) besitzt – **und dann offen**: das gelegte Tarock wird den
   Gegenspielern **angezeigt**. Grund: „ist jedoch seine Karte von der Art, daß er genöthigt wird, Tarok
   zu verwerfen, so muß er es **anzeigen**, weil die Taroks unter aufmerksamen Spielern, besonders des
   Pagats wegen nachgezählt werden“ [ATT1821 §22]. Die modernen Quellen verlangen darüber hinaus, die
   Karte selbst zu **zeigen** („they must be shown to the defenders“ [WP‑EN‑TAPP §Bidding]; „andere
   Tarockkarten dürfen nur dann – und zwar **offen** – abgelegt werden“ [WP‑DE‑TAPP]); die App zeigt die
   Karte offen. **⚙ Flag `erzwungenes_tarock = offen_zeigen | nur_anzahl_ansagen`**, Default
   `offen_zeigen`; `nur_anzahl_ansagen` entspricht dem Wortlaut von 1821.

> **⚠ Berichtigte Quellenangabe.** Frühere Fassungen führten **[PAG‑SLO]** als Beleg für Regel 3. Das ist
> **falsch**: Im slowenischen Tarok dürfen Tarock **frei** abgelegt werden, sie müssen nur offen liegen –
> „Cards that are worth 5 points (kings and cards of the trula) may **never** be discarded; **other trumps
> can be discarded freely** but must be discarded face up, so that all the players know how many trumps are
> in play“ [PAG‑SLO §Exchanging cards with the talon]. Umgekehrt kennen [KRUEPL §Playing] („these cards …
> must not be a king or a tarock“) und [PAG‑DRLES §Card Exchange] („Kings and trumps cannot be
> discarded“) **gar keine** Notfallausnahme. Die drei Positionen sind:
> **⚙ Flag `legage_tarock = notfall_offen (Default, AT/Tapp) | frei_offen (slowenisch) | verboten
> (Dreierles, Krüpl)`.**

**Algorithmus (Engine).**

```cpp
// hand:  19 Karten nach Talonaufnahme (bzw. 22 beim Sechserdreier)
// return: Menge der Karten, die JETZT legal abgelegt werden dürfen
CardSet legalDiscards(const CardSet& hand, int stillToDiscard) {
    CardSet plain;                       // erlaubte Nicht-Tarock
    for (Card c : hand)
        if (!c.isTarock() && !c.isKing()) plain.insert(c);

    if (plain.size() >= stillToDiscard) return plain;          // Regelfall

    // Notfall: Tarock müssen mit, aber nie Trullstücke
    CardSet forced = plain;
    for (Card c : hand)
        if (c.isTarock() && !c.isTrull()) forced.insert(c);
    return forced;                       // diese Tarock werden OFFEN gelegt
}
```

**Pathologischer Fall.** Enthält die Hand nach `legalDiscards` immer noch zu wenige Karten (nur Könige und
Trullstücke), ist die Legage unmöglich. Das ist mit 19 Karten praktisch ausgeschlossen – es gibt nur 7 solche
Karten; [PAG‑DRLES §Card Exchange] merkt an, ein Spieler mit einem solchen Blatt würde ohnehin Solo bieten.
Die Engine behandelt den Fall dennoch: **⚙ Flag `legage_notfall = trull_erlaubt_offen | koenig_erlaubt_offen`**
(Default `trull_erlaubt_offen`; das Trullstück wird dann offen gelegt und ein Hinweis protokolliert).

**Fehler bei der Legage** (zu viele, zu wenige, unerlaubte Karte): §6.10.

### 4.4 Wer deckt auf, wer sieht was?

* Der **Alleinspieler** deckt beide Hälften auf, **ohne die Reihenfolge der Karten zu verändern**
  [WP‑DE‑ILLU §Weiterer Verlauf].
* Die **Legage bleibt verdeckt** und wird erst am Ende gezählt; **Ausnahme:** erzwungene Tarock (§4.3).
* Die **liegen gebliebene Hälfte** bleibt bis zum Ende verdeckt [ATT1821 §22]; im Dreierles‑Brauch dürfen die
  Gegner sie ansehen, ohne sie zu tauschen [PAG‑DRLES §Card Exchange] – **⚙ Flag
  `restliche_talonhaelfte = verdeckt | gegner_sehen_sie`**, Default `verdeckt`.
* **Der Alleinspieler darf die aufgenommene Hälfte nicht nachträglich offenlegen**: „so entdecke ihn der
  Spieler nie vor dem Ende der Taille“ [ATT1821 §20].
* Ein **Vierter am Tisch** darf den Talon vor dem Kauf **nicht** sehen [ATT1821 §9].
* **Höflichkeitsbrauch:** Im badischen Dreierles legt **ein Gegenspieler** die Talonkarten offen hin, nicht
  der Spieler selbst [PAG‑DRLES §Card Exchange]. Für die App irrelevant, aber als Flavour‑Text im Lernmodus
  brauchbar.

### 4.5 Sonderfälle der Talonbehandlung

| Kontrakt | Aufnahme | Legage | Rest des Talons |
|---|---|---|---|
| Dreier / Unterer / Oberer | 3 Karten | 3 Karten, zählen für den Spieler | 3 Karten an die Gegner |
| Solo | – | – | **alle 6** an die Gegner |
| Sechserdreier / Tapper `ausgespielt` | alle 6 | 6 Karten | – |
| Trischaken | – | – | **bleibt aus dem Spiel** und wird **nicht gezählt** |
| Negativspiele (`AT-TAPP-NEGATIV`) | – | – | **bleibt aus dem Spiel** und wird **nicht gezählt** |
| Zweiblatt (`AT-TAPP-ILLU`) | 2 Karten (oberes, mittleres oder unteres Paar) | 2 | 4 an die Gegner |
| Einblatt (`AT-TAPP-ILLU`) | 1 beliebige Karte | 1 | 5 an die Gegner |

Beim Zwei‑ und Einblatt dürfen nur **nebeneinanderliegende** Karten genommen werden
[WP‑DE‑ILLU §Weiterer Verlauf].

### 4.6 Ein bereits 1821 gerügter Hausbrauch

„Wo ganz gegen alle Raison die darin befindlichen Points dem Spieler gelten, ist es oft leichter Solo als
einen Dreyer anzusagen“ – manche Runden rechneten den Talon im Solo **dem Spieler** zu, was den Solo
entwertet [ATT1821 §40]. **Nicht implementieren.** Ebenfalls gerügt: den Talon im Solo zu teilen, wobei der
Spieler weder legen noch den Kauf ansehen darf, aber die Punkte mitzählt [ATT1821 §40]. Beides ist in §9.8
als abgelehnte Variante vermerkt.

---

## 5. Ansagen, Prämien, Kontra

### 5.1 Ansagezeitpunkte

| Zeitpunkt | Was kann angesagt werden | Wirkung |
|---|---|---|
| Während der Lizitation | nur Spielstufen | – |
| **Vor** der Talonaufnahme (**⚙ nur bei `ansage_vor_kauf = an`**) | Prämien | erhöhter Wert (§5.9) |
| Nach dem Verlegen, mit „**Ich liege**“ | Prämien | Normalwert für Ansagen |
| **Nach** dem Verlegen, von den Gegnern | Kontra | Verdopplung (§5.8) |
| Nach dem Spiel („nachgemeldet“) | **nur** Handprämien und stille Prämien | einfacher Wert |

Prämien, die **vor** der Talonaufnahme angesagt werden, zählen mehr, weil der Spieler sein Blatt noch nicht
verbessert hat und die Gegner früher informiert sind [WP‑DE‑ILLU §Prämien Allgemeines].

### 5.2 Pagat ultimo

**Definition.** Der Alleinspieler gewinnt den **letzten Stich** mit dem **Pagat** (Tarock `I`)
[WP‑EN‑TAPP §Announcements], [ALSCHER], [ATT1821 §7].

| Kontrakt | still | angesagt |
|---|---|---|
| Dreier / Unterer / Oberer | **4** | **8** |
| Solo | **8** | **16** |

[ALSCHER Tapptarock: „Pagat ultimo: 4, angesagt 8 Punkte“ und „Solo: 8 Punkte (kein Talon; **verdoppelt
Pagat ultimo**)“]; [WP‑EN‑TAPP §Game values and bonuses: 4/8, „Pagat Ultimo in Solo“ 8/16];
[KRUEPL §Scoring: „Pagat ultimo unannounced 4 / announced 8“ – ohne Solo‑Verdopplung];
[FURR §Scores: 4/8, dort jedoch mit **vertauschten Beschriftungen**, siehe §9.16].

> **Abweichung, die nicht übersehen werden darf:** [WP‑DE‑TAPP §Punkte‑ und Kartenansagen] schreibt
> „Im Solo ist er angesagt **24** Punkte wert und still 8 Punkte“. 24 ist weder das Doppelte von 8 noch das
> Doppelte des angesagten Wertes im Dreier; die Zahl passt in kein Schema und steht gegen [ALSCHER],
> [WP‑EN‑TAPP] und [KRUEPL]. **Die App rechnet 8 / 16.** Ausführlich in §9.15.

**Weitere Regeln:**

* Die Ansage erfolgt, indem der Spieler den Pagat **offen zu seiner Linken liegen lässt**, „damit die
  Gegenpartey gehörig unterrichtet sey“ [ATT1821 §23]. Die App zeigt die Karte entsprechend offen an.
  (In manchen Runden wird nur mündlich angesagt – **⚙ Flag `ultimo_pagat_offen = true|false`**,
  Default `true`, rein kosmetisch.)
* Ein **angesagter** Pagat ultimo gilt **nur als gewonnen, wenn er gelingt**. Geht der Pagat vorher verloren
  – gefangen, oder durch Tarockzwang früher erzwungen –, ist die Ansage **verloren**, und **jeder** der
  beiden Gegenspieler kassiert den vollen Prämienbetrag [WP‑EN‑TAPP §Card points].
* **Contra ultimo** (klassischer Ausdruck): Gewinnt ein **Gegenspieler** den letzten Stich mit dem Pagat, so
  schreiben **beide** Gegenspieler die (stille) Prämie an, „weil man annehmen muß, daß der Andere durch sein
  kluges Mitwirken dazu behilflich war“ [ATT1821 §7].
* **Der Pagat ultimo ist unabhängig vom Spielausgang**: er kann gewonnen werden, obwohl das Spiel verloren
  geht, und umgekehrt – „Vier Könige und tous les trois zählen alle Mahl zu Gunsten des Besitzers …,
  **das Spiel mag gewonnen oder verloren seyn, gleich viel, eben so der Pagat ultimo, angesagt oder nicht
  angesagt**“ [ATT1821 §48]. Die Quelle rechnet dazu ein Beispiel vor, das die App als Testfall übernimmt:
  Der Spieler **verliert** einen Dreyer um 5 Points, hatte aber *tous les trois* in der Hand und macht den
  **stillen** Pagat ultimo; bei Consolation 50 stehen 50 + 5·2 = 60 für die Gegner gegen 50 + 50 = 100 für
  ihn, „so schreibt er noch **40**“ [ATT1821 §48].
* **Verlorene Ansage kostet so viel, wie das Gelingen gebracht hätte:** „Denn eben das, was das Gelingen
  gewinnt, kostet das Verlieren desselben“ [ATT1821 §7] – also der **angesagte** Wert, nicht der stille
  (siehe Beispiel 3 in §7.4).
* **Strategiehinweis der Quelle für den Lernmodus:** „Ist dieß nicht mehr möglich, so gibt man den Pagat
  lieber vor dem letzten Stiche zu, oder spielt ihn selbst aus, als daß man ihn bis zuletzt aufhebt und
  einbüßt, denn auf jene Art verliert man nur fünf, auf diese aber … fünf und fünfzig“ [ATT1821 §23].

### 5.3 Handprämien: Trull, Königstrull, Köpfe

| Prämie | Bedingung | Wert (Klassik) |
|---|---|---|
| **Trull** (Tarocktrull, *tous les trois*) | `Sküs`, `XXI` und `I` **im Blatt** | **3** |
| **Königstrull** | alle **vier Könige** im Blatt | **3** |
| **Köpfe** | **zwei** der drei Trullstücke im Blatt | **1** |

Quellen: Trull und Königstrull [ATT1821 §2], [ALSCHER Tapptarock], [WP‑EN‑TAPP §Game values and bonuses] ·
**Köpfe nur** [ALSCHER Tapptarock: „Köpfe (2 Trullstücke): 1 Punkt“], [WP‑EN‑TAPP: „Köpfe (‚heads‘) – Two
honours in one's opening hand – 1“].

> **⚠ Berichtigte Quellenangabe.** Frühere Fassungen zitierten **[ATT1821 §2]** auch für die *Köpfe*. Das
> ist falsch: 1821 kennt **nur** die vollen Kombinationen – „Sind diese drey beysammen in einer Hand …, so
> zählen sie eben so viel als die festgesetzte Consolation, ingleichen die vier Könige beysammen“
> [ATT1821 §2]. Eine Prämie für **zwei** Trullstücke gibt es dort nicht. Die *Köpfe* sind eine spätere
> Zutat; belegt sind sie für Tapp‑Tarock allein durch [ALSCHER] und [WP‑EN‑TAPP], für das Illustrierte
> Tarock durch [WP‑DE‑ILLU] (dort 2 Punkte). **Ebenfalls berichtigt:** [KRUEPL] und [FURR] führen für
> *Tappen* **überhaupt keine** Handprämien auf – ihre Tariftabellen enthalten nur Spiel, Pagat ultimo und
> Valat. Sie dürfen für Trull/Königstrull/Köpfe **nicht** als Beleg zitiert werden (siehe §5.9, §9.31).

**Regeln:**

* Die Prämien gebühren **jedem Inhaber**, auch einem **Gegenspieler**, und werden dann **allein**
  von diesem kassiert – „so gereichen sie dem jedesmahligen Inhaber im Aufschreiben allein zum Vortheile“
  [ATT1821 §2, §7].
* Sie müssen **geltend gemacht werden**: „wenn man diesen Vortheil des Zählens erlangen will, weder die
  einen, noch die andern verpaßt werden dürfen“ [ATT1821 §2]. Die App fragt nach dem Spiel automatisch ab
  und markiert im Lernmodus, wer welche Prämie hatte.
* **Wann wird gemeldet? Erst nach dem Spiel, nicht vorher** – und das ist ausdrücklich so gemeint:
  „Wenn eine Taille zu Ende ist, zählt man seine Points, und **sagt seine Honneurs an**, wenn man deren
  gehabt hat … Die Einführung, dieß **vor** dem Anfange des Spieles zu thun, wie es Manche verlangen, ist
  eben so **falsch als abgeschmackt**; – falsch: weil dadurch die Karte zu sehr verrathen wird“
  [ATT1821 §49]. Dieselbe Praxis im badischen Dreierles: Druckrolle und Königsrolle werden „not announced
  in advance but simply claimed at the end of the play (**nachgemeldet**)“ [PAG‑DRLES §Card Combinations].
  Im Illustrierten Tarock ist es umgekehrt lohnend, sie **vorher** anzusagen (§5.9).
  **⚙ Flag `handpraemien_meldung = nach_spiel (Default) | vor_spiel_moeglich`.**
* **Meldefrist:** „unter strengen Spielern darf das außer Acht gelassene, sobald einmahl **die neue Karte
  ausgegeben ist**, nicht mehr nachgetragen werden“ [ATT1821 §50]. Die App setzt die Frist auf das Ende
  des Abrechnungsdialogs des betreffenden Spiels und protokolliert sie.
* **Trull und Köpfe schließen einander aus** (drei Trullstücke ⇒ Trull, nicht zusätzlich Köpfe).
  Trull und Königstrull **kumulieren** [ATT1821 §5: „sieben Honneurs beysammen“].
* **Maßgeblicher Zeitpunkt:** das Blatt **nach** der Talonaufnahme und **vor** dem ersten Stich
  [WP‑EN‑ILLU §Bonuses: „in the hand before play starts“]. Ein Trullstück, das mit der Legage weggeht, kann
  gar nicht weggehen (Legageverbot §4.3); ein **König** kann es ebenfalls nicht.
  **Gegenposition:** [WP‑EN‑TAPP §Game values and bonuses] sagt dreimal „in one's **opening** hand“, was
  auch das **ausgeteilte** Blatt vor der Talonaufnahme meinen kann. Praktisch ist der Unterschied klein –
  er betrifft nur den Alleinspieler, der ein Trullstück oder einen König **aus dem Talon** dazubekommt.
  **⚙ Flag `handpraemie_stichtag = nach_talon (Default) | wie_ausgeteilt`.** Default `nach_talon`, weil
  nur so ein Gegenspieler und der Alleinspieler nach derselben Regel behandelt werden und weil 1821 die
  Honneurs an den Zustand „schon vor dem Legen in der Hand“ knüpft [ATT1821 §14].
* **Einmal erworben, bleibt die Prämie:** Sie zählt auch dann, wenn die Karten später in fremde Stiche
  fallen – „So bleiben im Gleichen tous les trois und vier Könige in einer Hand **jedes in seinem Werthe,
  wenn auch Mond und Pagat oder die Könige alle vier verloren gingen**“ [ATT1821 §48].
  1821 hält ausdrücklich fest, dass Honneurs auch dann zählen, wenn beim Legen ein Fehler passierte, „weil
  jene Honneurs oder Matadors schon vor dem Legen in der Hand waren“ [ATT1821 §14].
* **Nicht kontrierbar** – es sind Tatsachenfeststellungen [WP‑DE‑KR §Prämien aus der Hand].

**⚙ Flag `trull_bedingung = hand | stiche`.** Default: **`hand`** (Tapp‑Tarock, 1821). Im Profil
`AT-DREIER-54` steht es auf `stiche` („taking all 3 honours in tricks“) [WP‑EN‑DREIER]; im Königrufen
ebenfalls „in Stichen“ [ALSCHER Königrufen]. Siehe §9.12.

### 5.4 Weitere Prämien der erweiterten Profile

| Prämie | Bedingung | Profile |
|---|---|---|
| **Uhu** (Pre‑Ultimo) | **vorletzter** Stich mit Tarock `II` | `ILLU`, `AT-DREIER-54` |
| **Kakadu / Kike** | **drittletzter** Stich mit Tarock `III` | `AT-DREIER-54` |
| **Mondfang** | der `XXI` (Mond) wird vom `Sküs` überstochen | `ILLU` (Grazer Variante), `AT-DREIER-54` |
| **Absolut / „mit 40“** | der Spieler erreicht ≥ 40 Points | `ILLU`, `AT-POINT` (als „mit 40“) |
| **Panzer / „mit 50“** | der Spieler erreicht ≥ 50 Points | `ILLU`, `AT-POINT` |
| **Ohne Trull** | der Spieler gewinnt **ohne** Trullstück im Ausgangsblatt | `ILLU` |
| **Ohne Könige** | der Spieler gewinnt **ohne** König im Ausgangsblatt | `ILLU` |

**Mondfang.** Grazer Regel: „Wird Tarock XXI vom Sküs überstochen, so wird dies mit +5 Punkten für den
Stecher des Mondes und mit −5 Punkten für den Spieler des Mondes gewertet“ [WP‑DE‑ILLU §Prämie für den
Mondfang]. Es ist also eine **individuelle** Strafe, kein Parteienereignis – dasselbe Prinzip wie im
slowenischen Tarock, wo der Mondbesitzer 20 bzw. 21 Punkte allein verliert [PAG‑SLO §Captured Mond].
Beck kennt die Prämie **nicht** [WP‑DE‑ILLU]. **⚙ Flag `mondfang = aus | grazer_5 | individuell_slo`.**
Default im Klassik‑Profil: **`aus`** – kein Tapp‑Tarock‑Beleg.

**Absolut / Grand point.** [WP‑DE‑TAPP §Punkte‑ und Kartenansagen] führt für Tapp‑Tarock **Absolut**
(mind. 40 Points) und **Grand point** („Grammenboi“, 50 Points) an, **ohne Punktwerte**; die Werte stehen nur
im Illustrierten Tarock (§5.9). **⚙ Flag `punkteansagen = aus | illu | point`.** Default im Klassik‑Profil:
`aus`.

### 5.5 Valat

**Definition.** Der Alleinspieler macht **alle 16 Stiche**; die Gegenpartei bekommt keinen einzigen
[ATT1821 §Erklärung], [WP‑EN‑TAPP §Announcements].

**Default‑Abrechnung (`valat_modus = multiplikator`)** [ALSCHER Tapptarock], [KRUEPL §Scoring],
[FURR §Scores]:

```
still    : Spielwert × 4
angesagt : Spielwert × 8
```

Also z. B. Dreier 3 → stiller Valat 12, angesagter Valat 24; Solo 8 → 32 bzw. 64.

**⚙ Flag `valat_modus = multiplikator | fest`.** Bei `fest` gelten die Absolutwerte **12 still / 24 angesagt**
[WP‑DE‑TAPP §Punkte‑ und Kartenansagen], [WP‑EN‑TAPP §Game values]. Default: **`multiplikator`**, weil
[ALSCHER], [KRUEPL] und [FURR] übereinstimmen und weil ein Valat im Solo verdientermaßen mehr wert ist als
einer im Dreier. Siehe §9.13.

**Weitere Regeln:**

* Ein Valat kann auch von der **Gegenpartei** gemacht werden, wenn der Alleinspieler mit schwacher Karte
  einen Dreier nahm und sich verrechnete [ATT1821 §Erklärung]. Werte spiegelbildlich.
* **Misslungener angesagter Valat:** Das Spiel bleibt als normaler Kontrakt bestehen und wird nach Punkten
  abgerechnet; die Valat‑Ansage ist verloren und geht an die Gegner [ATT1821 §8 Anmerkung].
  Nach der Grazer Regel gilt ein nicht durchgebrachter Valat für die Gegner **mit den Points der Karten, die
  im Spiel waren**, als gewonnen, also 70 minus dem im Talon verbliebenen Wert [WP‑DE‑ILLU §Abrechnung des
  Valats].
* **Beim gelungenen Valat werden die Kartenpunkte gar nicht gezählt**, und die Gegner bekommen den liegen
  gebliebenen Talon nicht gutgeschrieben [ATT1821 §8 Anmerkung]. Die App zeigt trotzdem die Punkte an
  (Lernwert), rechnet aber nur den Valat ab.
* Ein Valat zu dritt ist **sehr selten**, ein angesagter noch seltener [WP‑DE‑ILLU §Valat],
  [WP‑EN‑ILLU §Strategy].

### 5.6 Pflicht‑ und Vorhandansagen: gibt es nicht

Anders als im Königrufen (Vorhandspiele, Muss‑Rufer) und im Zwanzigerrufen (Pflichtkontra bei Trull in der
Hand) [ALSCHER Zwanzigerrufen] kennt Tapp‑Tarock **keine** Pflichtansage. Jede Prämie ist freiwillig.

### 5.7 Ansagen der Gegenspieler

Im Default‑Profil sagen die Gegenspieler **nichts** an; sie antworten nur „Gut“ oder „Kontra“. Sie können
Prämien aber **still** gewinnen:

* **Pagat ultimo** durch einen Gegenspieler (§5.2, *contra ultimo*) – beide Gegner kassieren.
* **Handprämien** (§5.3) – der Inhaber kassiert allein.
* **Valat** der Gegenpartei (§5.5).

**⚙ Flag `gegner_ansagen = aus | pagat_uhu`.** Bei `pagat_uhu` dürfen auch Gegenspieler Pagat (und im
ILLU‑Profil Uhu) ansagen; die Prämie zählt dann **individuell**, nicht für die Partei
[WP‑EN‑ILLU §Announcements (1954)]. Default: `aus`.

### 5.8 Kontra, Rekontra, Subkontra

* **Kontra** wird von einem Gegenspieler gegeben, **nachdem der Spieler „Ich liege“ erklärt hat**, d. h. nach
  Talonaufnahme und Legage [WP‑DE‑TAPP §Kontra], [WP‑DE‑ILLU §Weiterer Verlauf].
* Steigerung: **Kontra → Rekontra → Subkontra**, jeweils **Verdopplung**: ×2, ×4, ×8
  [WP‑DE‑TAPP §Kontra], [ALSCHER].
* **Spiel und Prämien werden getrennt kontriert** [ALSCHER Tapptarock: „Kontra / Rekontra etc. (getrennt für
  Spiel und Prämien)“], [WP‑DE‑ILLU §Kontra Grazer Variante], [FURR §Illustrated Tapp: „one of the defenders
  may call ‚contra‘ **to either the bid or to any announcement or part of it**“]. Die App führt für jedes
  kontrierbare Objekt (Spiel, jede einzelne Prämie) einen eigenen Kontra‑Zähler.
  **Gegenposition:** Im badischen Dreierles verdoppelt das Klopfen **nur den Spielwert** und lässt
  Kombinationen und *Pfeife* unberührt – „This doubles the game point score for the game (**but does not
  affect the extra scores for combinations and Pfeife**)“ [PAG‑DRLES §Knocking]. **⚙ Flag
  `kontra_umfang = spiel_und_praemien_getrennt (Default) | nur_spiel`.**
* **Ein Kontra eines Gegenspielers gilt für beide Gegenspieler** (Grazer Regel: „Kontra gilt auch für den
  Partner“) [WP‑DE‑ILLU §Kontra]; ebenso [PAG‑DRLES §Knocking]: „A knock by one opponent doubles the payment
  to or from **both** opponents.“ Beck lässt die Frage offen [WP‑DE‑ILLU §Kontra Variante nach Beck].
  **⚙ Flag `kontra_gilt_fuer_partner = true|false`**, Default `true`.
  In **Negativspielen** kontriert jeder für sich [WP‑DE‑KR §Kontra].
* **Wer darf das Subkontra geben?** [WP‑DE‑TAPP §Kontra] und [WP‑DE‑ILLU] sagen nur „die gegnerische
  Partei“; [FURR] präzisiert „**any other player** may finish with ‚sub‑contra‘“. Die App lässt das
  Subkontra von **jedem** Gegenspieler zu, auch von dem, der nicht kontriert hat.
  **⚙ Flag `subkontra_durch = jeder_gegner (Default) | nur_kontrierender`.**
* **Beim Trischaken gibt es kein Kontra**, weil jeder gegen jeden spielt [WP‑DE‑KR §Kontra].
* **Wer darf kontrieren?** Im badischen Dreierles darf ein Gegner, der selbst die Gelegenheit zum Dreier
  hatte und passte, **nicht** kontrieren [PAG‑DRLES §Knocking]. **⚙ Flag
  `kontra_nach_pass_verboten = false`** (Default: jeder Gegner darf kontrieren).
* **⚙ Flag `kontra_max_stufe = 3`** (Kontra/Rekontra/Subkontra). Manche Runden gehen bis ×64
  [WP‑DE‑KR §Kontra].
* Im Profil `AT-TAPP-ILLU` kann zusätzlich **vor** der Talonaufnahme kontriert werden. **Hier widersprechen
  sich die beiden Beck‑Referate:** [WP‑EN‑ILLU §Announcements (Beck)] sagt „A contra at this stage
  **quadruples** the value of the announcement“; [WP‑DE‑ILLU §Kontra Variante nach Beck] sagt dagegen
  „Kontras können vor oder nach Talonaufnahme erfolgen, **immer mit Verdoppelung** aller Spiel‑ und
  Prämienwerte“. Die Grazer Runden lassen ein Kontra ohnehin erst nach „Ich liege“ zu.
  **⚙ Flag `kontra_vor_kauf = aus (Default in KLASSIK) | verdoppelt (WP‑DE‑ILLU) | vervierfacht
  (WP‑EN‑ILLU)`.** Default im ILLU‑Profil: **`verdoppelt`**, weil die deutschsprachige Wiedergabe näher an
  Becks Text steht und weil das ILLU‑Profil auch sonst [WP‑DE‑ILLU] folgt (§9.19). Siehe §9.32.

### 5.9 Prämientabelle über alle Profile

Alle Werte sind **Rohwerte je Gegenspieler**, vor Kontra‑ und (im ILLU‑Profil) vor Blattzahl‑Multiplikatoren.

| Prämie | `KLASSIK` still / angesagt | `ILLU` still / n. Kauf / vor Kauf | `AT-DREIER-54` still / angesagt |
|---|---|---|---|
| Pagat ultimo | 4 / 8 (Solo 8 / 16) | 5 / 10 / 20 | 40 / 80 |
| Uhu (Tarock II, vorletzter Stich) | – | 5 / 10 / 20 ⚙ | 60 / 120 |
| Kakadu (Kike, Tarock III) | – | – | 80 / 160 |
| Tarocktrull (Hand) | 3 / – | **4 / 4 / 8** | 10 / – (in Stichen) |
| Königstrull (Hand) | 3 / – | **5 / 5 / 10** | 10 / – (in Stichen) |
| Köpfe (2 Trullstücke, Hand) | 1 / – | **2 / 2 / 4** | – |
| Ohne Trull | – | **4 / 4 / 8** | – |
| Ohne Könige | – | **5 / 5 / 10** | – |
| Absolut / „mit 40“ | – | 5 / 5 / 10 | – |
| Panzer / „mit 50“ | – | 10 / 10 / 20 | – |
| Mondfang | – ⚙ | 5 (Graz) | 10 |
| Valat | ×4 / ×8 des Spielwerts | 12 / 24 / 48 (Graz: ×6 / ×12) | (Solo Valat = eigener Kontrakt, 960) |

Quellen der Spalten: `KLASSIK` Spiel + Pagat ultimo + Valat [ALSCHER], [WP‑EN‑TAPP], [KRUEPL], [FURR];
**Handprämien nur** [ALSCHER], [WP‑EN‑TAPP] (siehe Kasten in §5.3) · `ILLU` [WP‑DE‑ILLU §Prämien] ·
`AT-DREIER-54` [WP‑EN‑DREIER §Game values and bonuses].

> **⚠ Korrigierte ILLU‑Werte (fett).** Frühere Fassungen verdoppelten auch bei den **Kartenkombinations‑**
> und **Ohne‑Prämien** den Wert für die Ansage *nach* dem Talonkauf. Das ist falsch. [WP‑DE‑ILLU §Prämien
> Allgemeines] unterscheidet ausdrücklich zwei Staffeln:
> „Nach Talonkauf, vor Spielbeginn: **doppelter Wert (für Ultimoansagen), einfacher Wert für übrige
> Prämien**“ – „Vor Talonkauf: **vierfacher Wert (für Ultimoansagen), doppelter Wert für übrige Prämien**“.
> Die Grundwerte sind Königstrull 5, Tarocktrull 4, Köpfe 2, ohne Könige 5, ohne Trull 4.
> Bestätigt durch **drei durchgerechnete Beispiele der Quelle** [WP‑DE‑ILLU §Beispiel für einen
> Spielverlauf]: Köpfe *nach* Talonkauf = **2** („… und hatte Köpfe nach Talonkauf angesagt: 4 + 2 + 2 = 8“
> sowie „Spieler B hatte Köpfe nach Talonkauf angesagt: 2 × 3 = 6“), Köpfe *vor* Talonkauf = **4**
> („… hatte Köpfe vor Talonkauf angesagt: (3 + 4 + 4) × 2 = 22“). [WP‑EN‑ILLU §Announcements and bonuses
> (Beck)] geht noch weiter und führt Tarocktrull, Königstrull und *2 Tarock Honours* **ausschließlich als
> stille** Prämien (Spalten „Before/After Pickup“ leer).

**Bekannte Widersprüche in den ILLU‑Werten** (siehe §9.19): [WP‑EN‑ILLU] gibt für *Panzer* 20 / 40 statt
10 / 20, für *ohne Trull* 4 / 8 / 16 und für *ohne Könige* 5 / 10 / 15. Die App verwendet die deutschen Werte
[WP‑DE‑ILLU] und stellt die englischen als Unterprofil `AT-TAPP-ILLU-DUMMETT` bereit.

**Valat im ILLU‑Profil – zwei Abrechnungsarten der Quelle** [WP‑DE‑ILLU §Valat]:
* **Beck, Variante a) ⟵ Default:** Festwerte 12 (still) / 24 (nach Kauf) / 48 (vor Kauf), „zusätzlich zu
  sonstigen Punkten aus dem Spiel und für Prämien“;
* **Beck, Variante b):** dieselben Zahlen als **Multiplikatoren** – von Beck selbst als „umstritten“
  bezeichnet und nicht empfohlen;
* **Grazer Runden:** stiller Valat = **sechsfache**, angesagter Valat (nur nach Talonaufnahme) =
  **zwölffache** Werte, multipliziert werden Spielwert, Überschusspunkte, Kombinations‑ und Ultimoprämien,
  und zwar zusätzlich zum Blattfaktor.
  **⚙ Flag `valat_illu = fest_12_24_48 (Default) | beck_multiplikator | graz_6_12`.**

---

## 6. Spielregeln

### 6.1 Ausspiel und Stichfolge

* **Die Vorhand spielt zum ersten Stich aus**, unabhängig davon, wer Alleinspieler ist
  [ALSCHER Tapptarock: „erstes Ausspiel Vorhandspieler“], [WP‑EN‑TAPP §Play],
  [WP‑DE‑ILLU §Weiterer Verlauf].
  **⚙ Flag `erstes_ausspiel = vorhand | spieler`.** Default: `vorhand`.
  [FURR §Play] lässt den Alleinspieler ausspielen – Minderheitsposition, siehe §9.9.
  **Bei Negativspielen spielt immer der Alleinspieler aus** [WP‑DE‑KR §Negativspiele], §3.9.
* Danach spielt stets der **Gewinner des letzten Stichs** aus [WP‑DE‑ILLU §Weiterer Verlauf].
* Ein Stich besteht aus **drei** Karten; es gibt **16 Stiche**.
* Stiche werden **verdeckt** abgelegt. Wer darf sie ansehen?
  Beck: nur die **eigenen** Stiche [WP‑EN‑ILLU §Play (Beck)]. Mayr/Sedlaczek 2016: die eigenen Stiche und
  Talonkarten jederzeit, den **letzten** Stich des Gegners bis zum nächsten Ausspiel
  [WP‑EN‑ILLU §Play (2016)].
  **⚙ Flag `stichansicht = eigene | eigene_plus_letzter`.** Default: `eigene_plus_letzter`.

### 6.2 Stichgewinn

1. Enthält der Stich **mindestens ein Tarock**, gewinnt das **höchste Tarock**.
2. Sonst gewinnt die **höchste Karte der ausgespielten Farbe**.
3. Eine Karte einer **anderen** Farbe gewinnt nie.

[WP‑DE‑ILLU §Stichregeln], [WP‑EN‑ILLU §Play], [KRUEPL §General rules].
Ausnahme: **Kaiserstich**, §6.5 (optional).

### 6.3 Bedienzwang: Farbzwang und Tarockzwang

**Es gilt Farbzwang und Tarockzwang (Trumpfzwang)** [WP‑DE‑TAPP §Stechrunden].

1. **Farbzwang:** Wer die ausgespielte Farbe hat, **muss** sie bedienen.
2. **Tarockzwang:** Wer die ausgespielte Farbe nicht (mehr) hat, **muss Tarock zugeben**, wenn er Tarock hat.
3. Wer weder die Farbe noch Tarock hat, darf **eine beliebige Karte** abwerfen; sie kann den Stich nicht
   gewinnen.
4. Wird **Tarock ausgespielt**, so ist Tarock die „Farbe“: wer Tarock hat, muss Tarock zugeben.

[KRUEPL §General rules], [WP‑DE‑ILLU §Stichregeln], [WP‑EN‑TAPP §Play], [ATT1821 §1].

### 6.4 Kein Stichzwang in den positiven Spielen

**Man muss den Stich nicht übernehmen.** Farbzwang und Tarockzwang gelten, „aber kein Stichzwang“
[WP‑DE‑ILLU §Stichregeln], [WP‑EN‑DREIER §Playing], [PAG‑DRLES §Play of the Cards: „there is never any
obligation to beat the cards previously played“].

Konkret: Wird ein hohes Tarock ausgespielt, darf man ein **niedrigeres** Tarock zugeben (*schmieren*,
*abwerfen*); wird eine Dame ausgespielt und man hat König und Buben derselben Farbe, darf man den Buben geben.
Diese Freiheit ist **das** taktische Kernstück des Spiels (§10.4) und im Lernmodus ausdrücklich zu erklären,
weil Anfänger sie regelmäßig mit dem Stichzwang aus Skat/Schafkopf verwechseln (§11.4).

**Stichzwang gilt nur** in den Negativspielen und im Trischaken (§6.7).

### 6.5 Sonderrollen von Sküs, Mond und Pagat

* **Sküs.** Höchster Tarock, kann von nichts überstochen werden [ATT1821 §3]. **Keine** Sonderrechte: er
  unterliegt Farb‑ und Tarockzwang wie jedes andere Tarock, er ist nicht „Excuse“, wird nicht zurückgegeben
  und rettet sich nicht aus einem Stich.
* **Mond (`XXI`).** Zweithöchster Tarock; man muss mit ihm vorsichtig sein, „weil, wenn er durch den Sküs
  genommen wird, fünf Points verliert“ [ATT1821 §3]. Optionale Prämie *Mondfang*, §5.4.
* **Pagat (`I`).** Niedrigster Tarock, „der sogar vom Zweyer genommen wird, gilt aber ebenfalls wie Sküs und
  Mond fünf Points, und ist … die wichtigste Karte im Spiele“ [ATT1821 §3]. Regeln:
  * **In positiven Spielen keine Spielbeschränkung.** Der Pagat darf jederzeit gespielt werden.
  * **In Negativspielen und im Trischaken** darf der Pagat **erst als letztes Tarock im Blatt** gespielt oder
    zugegeben werden [WP‑DE‑KR §Stechrunden], [WP‑DE‑ILLU §Trischaken]. Erweiterte Formulierung aus dem
    slowenischen Regelwerk: „You are not allowed to play the pagat unless it's the only card you can play –
    if it's your last card, if it's your only trump, or if it's the only card that will win the trick“
    [PAG‑SLO §Additional rules for negative contracts].
* **Kaiserstich (Märchenstich).** Optionale Sonderregel: Fallen **alle drei Trullstücke im selben Stich**,
  so gewinnt der **Pagat** [WP‑DE‑KR §Stechrunden], [PAG‑SLO §The Emperor trick].
  **⚙ Flag `kaiserstich = aus | an | an_ohne_negativ`.** **Default: `aus`** – für Tapp‑Tarock ist die Regel
  in **keiner** Quelle belegt; sie ist Königrufen‑ und Slowenien‑Praxis. Ist sie eingeschaltet, sind zwei
  Unterfragen zu klären (beide als Flags):
  * `kaiserstich_reihenfolge = egal | mond_skues_pagat` – manche Runden verlangen eine bestimmte Reihenfolge
    im Stich [WP‑DE‑KR §Stechrunden];
  * Zusammenspiel mit dem Mondfang: fällt der Mondfang weg, wenn der Pagat den Stich holt?
    [WP‑DE‑KR] hält das ausdrücklich für ungeklärt. Default in der App: **Mondfang bleibt bestehen**, weil
    der Mond tatsächlich vom Sküs überstochen wurde.
* **„Vogel frei“.** Slowenische Variante: Wer den Pagat als **einziges** Tarock erhält, darf ihn ansagen; er
  ist dann vom Bedienzwang befreit und gewinnt jeden Stich, in den er gespielt wird
  [PAG‑SLO §Variations of the three player game]. **⚙ Flag `vogel_frei = aus`** (Default), nicht für
  Tapp‑Tarock belegt.

### 6.6 Die Funktion der erlaubten Züge

```cpp
// Vollständige Zugregel für alle Profile und Kontrakte.
CardSet legalPlays(const Hand& h, const Trick& t, const Contract& c)
{
    if (t.empty()) {
        // Ausspiel: alles erlaubt, ausser Pagat-Sperre in Negativspielen
        return applyPagatRule(h.all(), h, t, c);
    }

    Suit led = t.ledSuit();                       // TAROCK oder eine der 4 Farben
    CardSet cand = h.ofSuit(led);                 // 1. Farbzwang / Tarockzwang bei Tarockausspiel
    if (cand.empty() && led != Suit::Tarock)
        cand = h.tarocks();                       // 2. Tarockzwang
    if (cand.empty())
        cand = h.all();                           // 3. frei abwerfen

    if (c.stichzwang) {                           // 4. nur Negativspiele + Trischaken
        Card best = t.currentWinner();
        CardSet beating = cand.beating(best, led);
        if (!beating.empty()) cand = beating;
    }
    return applyPagatRule(cand, h, t, c);
}

CardSet applyPagatRule(CardSet cand, const Hand& h, const Trick& t, const Contract& c)
{
    if (!c.pagatLastTarock) return cand;          // positive Spiele: keine Sperre
    if (!cand.contains(PAGAT)) return cand;
    if (h.tarocks().size() == 1) return cand;     // Pagat ist das letzte Tarock -> erlaubt
    if (cand.size() == 1) return cand;            // Pagat ist der einzig moegliche Zug
    if (c.kaiserstich && wouldWinByKaiserstich(t)) return cand;   // §6.5
    cand.erase(PAGAT);
    return cand;
}
```

Alle Zugprüfungen laufen **serverseitig** (LAN, §8.6); der Client berechnet dieselbe Menge nur zur Anzeige
der spielbaren Karten.

### 6.7 Stichzwang in Negativspielen und im Trischaken

**Regel:** „Man muss die bisher höchste Karte auf dem Tisch übertreffen, wenn man kann“
[PAG‑SLO §Additional rules for negative contracts], [WP‑DE‑KR §Negativspiele].

Die Reihenfolge der Zwänge ist **Farbzwang → Tarockzwang → Stichzwang**: Der Stichzwang wirkt erst
**innerhalb** der durch Farb‑ und Tarockzwang bereits eingeschränkten Menge. Man ist also **nicht**
verpflichtet, mit Tarock zu stechen, wenn man die Farbe bedienen kann – wohl aber, innerhalb der Farbe die
höchste bisher gespielte Karte zu überbieten, sofern das geht.

### 6.8 Trischaken im Einzelnen

Regeln [WP‑DE‑ILLU §Vorgehen bei fehlender Spielansage], ergänzt um die ausdrücklich als solche markierten
Königrufen‑Optionen:

1. **Jeder spielt für sich.** Es gibt keinen Alleinspieler und keine Partei.
2. **Vorhand spielt zum ersten Stich aus.**
3. **Stichzwang** (§6.7), zusätzlich zu Farb‑ und Tarockzwang.
4. **Der Pagat darf erst als letztes Tarock gespielt werden.**
5. **Der Talon bleibt unberührt und wird nicht gezählt** (⚙ `trischaken_talon`, §3.5.3).
6. **Keine Prämien.** Kein Kontra.
7. **Ziel: möglichst wenige Punkte.** Verlierer ist, wer die **meisten** Punkte hat.

**Optionale Verschärfungen** (Königrufen‑Praxis, **⚙ jeweils eigenes Flag, alle Default `aus`)**
[WP‑DE‑KR §Variationen beim Trischaken]:

* `trischaken_von_oben` – der Ausspielende muss mit seinem **höchsten Tarock** herauskommen;
* `trischaken_tarockpflicht` – es muss Tarock ausgespielt werden, solange man welches hat
  [ALSCHER Königrufen];
* `trischaken_ohne_stichzwang`;
* `trischaken_tarock_invertiert` – Tarock stechen in umgekehrter Reihenfolge.

**Badische Varianten der Trullstück‑Sperre** (statt der einfachen Pagat‑Sperre aus Punkt 4), alle
Default `aus` [PAG‑DRLES §Räuber, §Variations]:

* `trischaken_pfeife_dritter_tarockstich` (Steinmauern) – der Pagat **muss** zum **dritten Stich, zu dem
  Tarock ausgespielt wird**, gelegt werden, sofern möglich; zusätzlich darf der `XXI` nicht in einen Stich
  gegeben werden, der bereits den Sküs enthält, außer der Inhaber hat kein anderes Tarock;
* `trischaken_trull_zwangsfolge` (Iffezheim) – Sküs und `XXI` **müssen** in den ersten beiden Stichen fallen
  (aber nicht beide im selben), der Pagat im dritten;
* `trischaken_ohne_sperre` (Achertal/„Ramsch“) – **keine** Beschränkungen für Sküs, `XXI` und `I`.

Diese drei schließen einander und Punkt 4 aus; die App lässt genau eine Einstellung zu.

### 6.9 Farbenspiele: gibt es im Tapp‑Tarock nicht

Farbendreier/Farberl (Tarock stechen nicht) sind für Zwanzigerrufen, Einfaches Tarock und Königrufen belegt
[ALSCHER], für **Tapp‑Tarock nicht**. Nicht implementieren. **⚙ Flag `farbenspiele = aus`** (fix).

### 6.10 Renonce (Regelverstoß) und Unregelmäßigkeiten

| Fall | Regel im Default‑Profil | Quelle |
|---|---|---|
| **Nicht bedient / Stichzwang verletzt** | Das Spiel wird abgebrochen. Der Fehler machende Spieler zahlt an **jeden** Mitspieler das, was dieser bei gewonnenem Spiel und gewonnenen angesagten Prämien erhalten hätte. | [WP‑DE‑KR §Renonce] |
| **Renonce durch den Alleinspieler** (ILLU‑Profil) | Spiel und alle Prämienansagen gelten als verloren, das Spiel wird mit **10 Überschusspunkten** zugunsten der Gegner gewertet. | [WP‑DE‑ILLU §Renonce] |
| **Renonce durch einen Gegenspieler** (ILLU‑Profil) | Spiel gilt für den Alleinspieler als gewonnen, mit 10 Überschusspunkten, samt allen Prämien. | [WP‑DE‑ILLU §Renonce] |
| **Korrigierbarer Fehlwurf** | Eine falsche Karte darf **ohne Strafe** zurückgenommen werden, solange keine weitere Karte gespielt wurde bzw. der Stich noch nicht umgedreht ist. | [PAG‑DRLES §Irregularities and Penalties] |
| **Zu viele oder zu wenige Karten gelegt** | Gewinnt der Spieler, schreibt er **nichts** an; verliert er, schreiben die Gegner so an, als wäre alles ordnungsgemäß gewesen. Ein **gewonnener** Pagat ultimo des Spielers kann in diesem Fall nicht zählen, ein *contra ultimo* der Gegner aber sehr wohl. Handprämien zählen weiter, „weil jene Honneurs … schon vor dem Legen in der Hand waren“. | [ATT1821 §14] |
| **Legage ganz vergessen** | Wird es nach dem ersten Stich bemerkt, endet das Spiel sofort; die Gegner schreiben wie bei Remis an. | [ATT1821 §14] |
| **Zwei aneinanderhängende Karten gespielt** | Detaillierte Strafregelung mit Consolationsbeträgen. | [ATT1821 §15, §16, §17] |

**Empfehlung für die App.** In der **Turniereinstellung** gilt die harte Regel (Spielabbruch, Zahlung an
alle). In der **Freundschaftseinstellung** (Default) verhindert die UI Regelverstöße technisch – nicht
spielbare Karten sind ausgegraut –, sodass ein Renonce für Menschen gar nicht auftreten kann; die Regel wird
nur im Lernmodus erklärt. **⚙ Flag `renonce = ui_verhindert | turnier`.** Default: `ui_verhindert`.

---

## 7. Abrechnung

### 7.1 Grundprinzip

> **„The game value or bonus is paid by each defender to the declarer if the latter won it. Otherwise the
> declarer pays it to each defender.“** [WP‑EN‑TAPP §Game values and bonuses]

Alle Beträge sind also **Beträge je Gegenspieler**. Daraus folgt die Nullsummen‑Abrechnung je Spieler:

```
Spieler       :  +2 · Betrag      (bzw. −2 · Betrag)
je Gegenspieler:  −1 · Betrag      (bzw. +1 · Betrag)
```

Die Engine führt für jedes Spiel eine **Liste von Zahlungsposten** `{von, an, betrag, grund}` und bildet
daraus den Spielstand. So bleiben individuelle Posten (Handprämien, Mondfang, Renonce) sauber getrennt von
Parteienposten, und die Summe aller Deltas ist stets **0** – die App prüft das nach jedem Spiel per
Assertion.

### 7.2 Die Abrechnungsformel

```
Ergebnis je Gegenspieler  =  Σ  (Posten_i · Kontrafaktor_i · Blattfaktor)
```

| Größe | `KLASSIK` | `ILLU` | `AT-DREIER-54` | `1821` |
|---|---|---|---|---|
| **Spielwert** | 3 / 4 / 5 / 8 | 3 (angesagt) / 4 (gelöst) / 5 (gehalten) | 30 … 70 / 120 / 960 | Consolation (30–50), Solo doppelt |
| **Überschusspunkte** | **keine** | Points über bzw. unter 35 | keine | Points über 35, im Dreyer **doppelt**, im Solo **vierfach** |
| **Blattfaktor** | keiner | ×1 Dreiblatt, ×2 Zweiblatt, ×3 Einblatt, ×4 Solo | keiner | keiner |
| **Kontrafaktor** | ×2 / ×4 / ×8 | ×2 / ×4 / ×8; vor Kauf ⚙ `kontra_vor_kauf` (§5.8) | ×2 / ×4 / ×8 | **kein Kontra** [ATT1821] |
| **Prämien** | §5.9 | §5.9, ebenfalls mit Blattfaktor | §3.6 | §3.10 |

**⚙ Flag `ueberschusspunkte = aus | einfach | doppelt_1821`.** Default im Klassik‑Profil: **`aus`**.
Begründung: [ALSCHER], [WP‑EN‑TAPP], [KRUEPL] und [FURR] geben für Tapp‑Tarock **Pauschaltarife ohne**
Überschusspunkte. [WP‑DE‑TAPP §Spielbewertung] beschreibt dagegen die Überschussrechnung – das ist wieder
die Vermengung mit dem Illustrierten Tarock (§9.3). Siehe §9.14.

### 7.3 Vollständige Wertetabelle `AT-TAPP-KLASSIK`

Beträge **je Gegenspieler**:

| Posten | gewonnen | verloren |
|---|---|---|
| Dreier | +3 | −3 |
| Unterer | +4 | −4 |
| Oberer | +5 | −5 |
| Solo | +8 | −8 |
| Pagat ultimo still (Dreier/Unterer/Oberer) | +4 | −4 |
| Pagat ultimo angesagt | +8 | −8 |
| Pagat ultimo still im Solo | +8 | −8 |
| Pagat ultimo angesagt im Solo | +16 | −16 |
| Trull (Hand) | +3 an den Inhaber, von jedem anderen | – |
| Königstrull (Hand) | +3 an den Inhaber, von jedem anderen | – |
| Köpfe (Hand) | +1 an den Inhaber, von jedem anderen | – |
| Valat still | +4 × Spielwert | −4 × Spielwert |
| Valat angesagt | +8 × Spielwert | −8 × Spielwert |

### 7.4 Sieben durchgerechnete Beispiele

Alle Beispiele: Alleinspieler **A**, Gegenspieler **B** und **C**.

**Beispiel 1 – einfacher Dreier.** A gewinnt einen Dreier mit 39 Points.
Posten: Spiel 3. → A **+6**, B **−3**, C **−3**. Summe 0 ✔

**Beispiel 2 – Dreier mit stillem Pagat ultimo und Köpfen.** A gewinnt mit 39, macht den letzten Stich mit
dem Pagat, ohne ihn angesagt zu haben, und hatte `Sküs` + `XXI` im Blatt.
Posten je Gegner: Spiel 3 + Pagat still 4 + Köpfe 1 = 8. → A **+16**, B **−8**, C **−8**. ✔

**Beispiel 3 – Solo mit verlorener Ultimo‑Ansage.** A spielt Solo, sagt Pagat ultimo an, verliert den Pagat
im 12. Stich, gewinnt das Spiel aber mit 41 Points.
Spiel: +8 je Gegner. Pagat ultimo angesagt im Solo: −16 je Gegner.
Je Gegner: 8 − 16 = **−8**. → A **−16**, B **+8**, C **+8**. ✔
Das ist die wichtigste Lehre aus [ATT1821 §23]: Ein misslungener angesagter Ultimo kostet mehr als das
Spiel wert ist.

**Beispiel 4 – Oberer mit Kontra, verloren.** A spielt einen Oberer, B gibt Kontra, A erreicht nur 33.
Spiel 5 × 2 (Kontra) = 10 je Gegner, zulasten A.
→ A **−20**, B **+10**, C **+10**. ✔ (Das Kontra von B gilt auch für C, ⚙ `kontra_gilt_fuer_partner`.)

**Beispiel 5 – Remis.** A spielt einen Dreier und erreicht **genau 35** (Gegner ebenfalls 35).
35 < 36 ⇒ **verloren**. → A **−6**, B **+3**, C **+3**. ✔

**Beispiel 6 – stiller Valat im Dreier.** A macht alle 16 Stiche, ohne Valat angesagt zu haben, und hatte
die Trull im Blatt.
Valat still = 4 × Spielwert 3 = 12 je Gegner; Trull 3 an A von jedem.
→ je Gegner 12 + 3 = 15. A **+30**, B **−15**, C **−15**. ✔
Die Kartenpunkte werden nicht gezählt, der Spielwert **nicht zusätzlich** angeschrieben
[ATT1821 §8 Anmerkung].

**Beispiel 7 – Handprämie beim Gegner.** A gewinnt einen Unteren mit 37; **C** hatte alle vier Könige.
Posten: Spiel 4 je Gegner an A; Königstrull 3 an C von A **und** von B.
→ A: +4 +4 −3 = **+5**; B: −4 −3 = **−7**; C: −4 +3 +3 = **+2**. Summe 0 ✔
Genau dieser Fall – „Es kann durchaus sein, dass Spieler A Punkte für das gewonnene Spiel erhält, Spieler B
und C Punkte für einen … Pagat Ultimo“ – ist der Grund für die zeilenweise Notation je Spieler
[WP‑DE‑ILLU §Notation].

### 7.5 Abrechnung im Profil `AT-TAPP-ILLU`

```
Rohwert = Spielwert (3 | 4 | 5) + Überschusspunkte + Σ Prämien
Ergebnis je Gegner = Rohwert · Blattfaktor (1|2|3|4) · Kontrafaktor
```

Beispiele aus der Quelle [WP‑DE‑ILLU §Beispiel für einen Spielverlauf]:

* A gewinnt ein **angesagtes Dreiblatt mit 45 Points**, „40“ nach Talonkauf angesagt:
  3 + 10 + 5 = **18**.
* C gewinnt ein **angesagtes Zweiblatt mit 39 Points**, „Köpfe“ **vor** Talonkauf angesagt:
  (3 + 4 + 4) × 2 = **22**.
* A gewinnt ein **angesagtes Einblatt mit 50 Points**, „50“ nach Talonkauf angesagt:
  (3 + 15 + 10) × 3 = **84**; B hatte „Köpfe“ nach Talonkauf angesagt: 2 × 3 = **6**.
* B verliert ein **gehaltenes Dreiblatt mit 33 Points**: B **0**, C schreibt 4 + 2 = **6**, A schreibt
  4 + 2 = 6 plus angesagte Köpfe 2 = **8**.

Diese Beispiele zeigen, dass im ILLU‑Profil **nicht** in der Nullsummenlogik von §7.1 abgerechnet wird,
sondern jeder Spieler **für sich** anschreibt. Die Engine bildet das über den Schalter
**⚙ `abrechnung = nullsumme_je_gegner | schreiben_je_spieler`** ab; Default in `KLASSIK`:
`nullsumme_je_gegner`, in `ILLU`: `schreiben_je_spieler`.

### 7.6 Abrechnung des Trischakens

**Quellenwerte** [WP‑DE‑ILLU §Punkte für das Spiel]:

```
gewonnenes Trischaken            :  +5
verlorenes Trischaken            : −10
gewonnen ohne Stich (Jungfrau)   :  +5 zusätzlich
```

**Nullsummen‑Implementierung (Default).** Verlierer ist der Spieler mit den **meisten** Punkten; er zahlt
**5 an jeden** der beiden anderen (5 + 5 = 10 ✔). Ein Spieler **ohne Stich** erhält vom Verlierer
**zusätzlich 5**.

**Gleichstände:**

* **Zwei Spieler gleichauf an der Spitze:** **beide** sind Verlierer und zahlen **jeder 5** an den dritten
  Spieler; dieser erhält also 10. Beleg: „Verlierer des Trischakens ist der Spieler mit den meisten Punkten.
  Das können punktegleich auch zwei oder drei Spieler sein. **Alle Verlierer zahlen allen anderen Spielern
  je 10 Cent**“ [WP‑DE‑KR §Abrechnung beim Trischaken].
  **⚠ Berichtigte Quellenangabe:** Frühere Fassungen beriefen sich hier auf [PAG‑DRLES §Räuber]. Dreierles
  regelt es **gegenteilig**: „the principle is that the players **not** involved in the tie should **still
  receive 2 game points each**. So if two players tie for most points the loss is **shared** between the two
  losers, and each pays **1** game point … to the third player.“ Auf unsere Tarife übertragen bekäme der
  dritte Spieler also unverändert **5**, und jeder Verlierer zahlte **2½**.
  **⚙ Flag `trischaken_gleichstand = jeder_zahlt_voll (Default, [WP‑DE‑KR]) | zahlung_geteilt
  ([PAG‑DRLES])`.** Bei `zahlung_geteilt` rechnet die Engine Trischaken‑Beträge intern in **Halben**
  (Faktor 2), damit keine Rundung nötig ist.
* **Alle drei gleichauf:** keine Zahlung – so auch [PAG‑DRLES §Räuber] für den Dreiertisch („there is of
  course no payment at a 3‑player table“).

**⚙ Flag `trischaken_modus`** mit den im Königrufen belegten Alternativen
[WP‑DE‑KR §Abrechnung beim Trischaken, §Vorhandspiele]:

| Wert | Bedeutung |
|---|---|
| `meister_zahlt_allen` (**Default**) | der Punktehöchste zahlt allen |
| `zwei_schlechtere_zahlen` | die beiden mit mehr Punkten zahlen an den mit weniger |
| `bester_kassiert` | der Punkteniedrigste kassiert von allen |
| `durchschnitt` | über/unter dem Durchschnitt bestimmt Zahlung |
| `nur_stichzahl` | statt Punkten zählt die **Anzahl** der Stiche |

**⚙ Zusatzflags:** `trischaken_buergermeister` (wer allein die absolute Punktemehrheit hat, also mindestens
35 und 2 Blatt, zahlt **doppelt**), `trischaken_jungfrau` (wer keinen Stich hat, kassiert extra),
`trischaken_vorhand_strafe` (verliert die Vorhand, zahlt sie immer allein und doppelt)
[WP‑DE‑KR §Abrechnung beim Trischaken]. Default: `buergermeister = an`, `jungfrau = an`,
`vorhand_strafe = aus`.

**Achtung Schwellenwert.** Weil der Talon beim Trischaken draußen bleibt (§3.5.3), sind **nicht 70** Punkte
im Spiel. Die „Bürgermeister“-Schwelle von 35 ⅔ ist deshalb **absolut** zu verstehen (mehr als die Hälfte
von 70), nicht relativ zur tatsächlich gespielten Punktsumme. Die App zeigt beides an.

### 7.7 Abrechnung der Negativspiele (`AT-TAPP-NEGATIV`)

Es gibt **keine Überschusspunkte und keine Prämien**; der Alleinspieler gewinnt oder verliert schlicht den
Kontraktwert [PAG‑SLO §The Scoring: „In the higher contracts, beggar and above, the declarer simply either
wins or loses the value of the contract“]. Kontra wirkt **individuell** je Gegenspieler.

### 7.8 Darstellung im UI

* **Spielprotokoll** je Deal: Kontrakt, Kontra‑Stufe, Kartenpunkte beider Seiten (in ganzen Punkten und,
  falls nötig, mit Drittel), Liste aller Posten mit Begründung, Deltas je Spieler.
* **Laufende Tabelle** mit einer Zeile je Spiel und Spalten je Spieler, wie in [WP‑DE‑ILLU §Notation]
  beschrieben (`A.: 18, 26, 110 | 120`).
* **Geldumrechnung:** frei einstellbarer Faktor, Vorschlag **10 Cent je Spielpunkt**
  [WP‑DE‑KR §Spieltabelle], [PAG‑DRLES §Players and Objective]; im ILLU‑Profil 1–2 Cent
  [WP‑EN‑ILLU §Scoring (2016)].

---

## 8. Partie‑ und Matchstruktur

### 8.1 Begriffe

| Begriff | Definition |
|---|---|
| **Spiel** (Deal, *Taille*) | Ein Geben mit Lizitation, Talon, 16 Stichen und Abrechnung. „Taille, ein einzelnes Spiel“ [ATT1821 §Erklärung] |
| **Runde** (*Tour*) | Drei Spiele, sodass **jeder Spieler einmal gegeben** hat. „Tour, drey Taillen (mit dem Könige vier), daß jeder der Mitspielenden ein Mahl die Vorhand erhält“ [ATT1821 §Erklärung]; „A round consists of one deal by each player“ [PAG‑DRLES §Ending a Session] |
| **Serie** | Mehrere Runden bis zu einem Abbruchkriterium |
| **Sitzung** | Alle Serien eines Abends, danach Geldausgleich |

Eine Sitzung soll „am Ende einer beliebigen Runde“ enden können, damit jeder gleich oft gegeben hat
[PAG‑DRLES §Ending a Session].

**Historisches Ausstiegsverfahren** (Vorbild für den Modus „Partie höflich beenden“): Wer aufhören will,
kann das „nicht auf der Stelle, sondern er soll auf die **letzten drey Touren** (drey Mahl um und um, also
unter drey Spielern **neun**, unter vieren zwölf Taillen) einladen; derjenige, welcher die ersten Karten
ausgegeben hat, erhält nun **die letzte Vorhand**, und dann ist das Spiel geendigt; kommt aber nichts als
ein allgemeiner Tapper zum letzten Mahle, so wird gewöhnlich noch eine Tour gemacht“ [ATT1821 §51].
**⚙ Flag `hoefliches_ende = aus | drei_touren`** (Default `aus`; im Rundenmodus als Button „Noch drei
Runden, dann Schluss“ anzubieten). Das slowenische Gegenstück ist die *škis round* [PAG‑SLO §The škis
round], das badische das „Räuber oder Solo“ der Schlussrunde (§8.3).

### 8.2 Abbruchkriterien (empfohlene App‑Modi)

| Modus | Ende | Geeignet für |
|---|---|---|
| **Rundenmodus** (**Default**) | nach *n* Runden, Default **n = 8** (= 24 Spiele, ca. 45–60 min) | Turnier, LAN, Statistik |
| **Zielpunktmodus** | sobald ein Spieler **100 Spielpunkte** erreicht; wer 100 oder mehr hat, erhält **+10 Bonus**; Sieger ist der Spieler mit der höchsten Punktzahl | `AT-TAPP-ILLU` (Quellenregel: [WP‑DE‑ILLU §Notation], [WP‑EN‑ILLU §Scoring]) |
| **Freie Partie** | jederzeit beendbar, laufender Saldo | Gelegenheitsspiel |
| **Lernmodus** | fest skriptierte Deals (§11.7) | Einsteiger |

Im Zielpunktmodus wird der **+10‑Bonus nicht vergeben, wenn die 100 durch eine Renonce erreicht wurden**
[WP‑DE‑ILLU §Notation].

### 8.3 Schlussrunde („Räuber oder Solo“)

Optionaler Brauch aus dem badischen Schwesterspiel: In der **letzten Runde** sind nur noch **Trischaken oder
Solo** erlaubt, und der Trischaken‑Grundwert wird erhöht [PAG‑DRLES §Ending a Session].
**⚙ Flag `schlussrunde = aus | trischaken_oder_solo`.** Default: `aus`.

### 8.4 Sitzplatz‑ und Geberrotation

* Erster Geber per Los (§2.1); danach wandert das Geben **nach rechts**.
* Bei **vier oder fünf Personen am Tisch** rotiert zusätzlich der Aussetzer (§2.2). Die Rotation ist ein
  gemeinsamer Engine‑Mechanismus für alle Vierer‑ und Dreiervarianten der App.
* Nach einer Serie kann neu ausgelost werden (**⚙ Flag `neu_auslosen_je_serie`**, Default `aus`).

### 8.5 Persistenz

Zu speichern ist (Sailfish: `~/.local/share/harbour-tarock/`, Android: App‑internal storage):

* Profil‑ID und alle abweichenden Flags,
* Sitzungs‑ und Serienstände, Spielprotokolle (für „Letztes Spiel ansehen“ und Statistik),
* der **vollständige Zustand des laufenden Spiels** inklusive verdeckter Information (Hände, Talon, Legage),
  damit eine Partie unterbrochen und fortgesetzt werden kann. Format: kompaktes JSON, ein Deal pro Objekt,
  Karten als Ganzzahl 0–53.
* Bei aktivem `rauben` (§3.5.2) zusätzlich die vom Geber vorgehaltenen Karten.

### 8.6 LAN‑Mehrspielerbetrieb

* **Der Server ist maßgeblich.** Es gibt verdeckte Information (Talon, Legage, Hände); Clients erhalten nur
  ihre eigene Sicht plus die öffentlichen Ereignisse.
* **Öffentlich** sind: Lizitgebote, die aufgedeckten Talonhälften, die Anzahl gelegter Karten, **erzwungen
  offen gelegte Tarock** (§4.3), der offen ausgelegte Pagat bei angesagtem Ultimo (§5.2), alle Ansagen und
  Kontras, alle gespielten Karten, die Abrechnung.
* **Nicht öffentlich:** Handkarten, der Inhalt der Legage, die nicht genommene Talonhälfte (bis Spielende),
  die Reihenfolge, in der ein Spieler seine Karten sortiert.
* Ein **Aussetzer** ist ein eigener Sitzzustand: er sieht **den Talon nicht** [ATT1821 §9] und darf keine
  Nachrichten in den Spielkanal senden, solange ein Spiel läuft.
* **Zuschauer** sehen nur die öffentliche Sicht.
* Reconnect: Der Server hält den Spielzustand; ein Client kann jederzeit resynchronisieren. Timeouts führen
  zur Übernahme des Sitzes durch den Computerspieler (**⚙ `timeout_sekunden`**, Default 90).

### 8.7 Computerspieler an jedem Sitz

Jeder Sitz ist unabhängig als Mensch oder KI konfigurierbar, auch mitten in einer Sitzung. Die KI erhält
**ausschließlich** die öffentliche Sicht plus ihr eigenes Blatt – kein Blick in fremde Hände, kein Blick in
den Talon. Das ist eine harte Architekturvorgabe: Die KI benutzt dieselbe `PlayerView`-Struktur wie ein
Netzwerk‑Client (§10.1).

---

## 9. Randfälle, strittige Punkte, regionale Unterschiede

Jede Zeile ist ein Profil‑Flag. **Fett** = Empfehlung und Default der App.

| # | Frage | Positionen der Quellen | Empfehlung |
|---|---|---|---|
| 9.1 | Heißt das Spiel „Tapp‑Tarock“ oder „Dreiertarock“? | *Dreiertarock* ist ein Sammelbegriff [WP‑DE‑DREIER] bzw. ein eigenes Kärntner Spiel [WP‑EN‑DREIER] | **Beide getrennt anbieten** (§0.1, §3.6) |
| 9.2 | pagat.com als Regelquelle | [PAG‑TAPP] ist nur eine Verweisseite; die im Auftrag genannte URL `tapp.html` liefert **HTTP 404** | Als Quelle **`tapptarock.html`** zitieren; inhaltlich auf [KRUEPL] und Wikipedia stützen |
| 9.3 | Bedeuten *Unterer/Oberer* Talonhälften oder Kartenzahlen? | Hälften: [KRUEPL], [FURR], Unger 1923, Piatnik, Parlett, Alscher, [WP‑DE‑KR] · Kartenzahlen (Zweiblatt/Einblatt): [WP‑DE‑TAPP] | **Hälften** (`talon_lizit_semantik = haelfte_fest`), §4.2 |
| 9.4 | Darf der Spieler beim *Unterer/Oberer* trotzdem frei wählen? | frei: [WP‑EN‑TAPP Haupttext], [ALSCHER] „oder wie Dreier“, [WP‑EN‑DREIER] · festgelegt: siehe 9.3 | **Festgelegt**; `freie_wahl` als Flag |
| 9.5 | Wird der Talon beim *Unterer/Oberer* aufgedeckt? | ja: [WP‑EN‑TAPP], **[KRUEPL §Playing]** („The talon is exposed (if it's not a ‚Solo‘)“), [WP‑DE‑POINT], [WP‑EN‑DREIER] · nein: [FURR §Bidding] („without revealing the stock“), Unger 1923, [WP‑DE‑KR §Oberer, Unterer], **[ATT1821 §20]** (der Dreyer kauft die drei obersten Blätter **ohne** Aufdecken) | **Ja** (`talon_aufdecken = immer`) – bessere UI, gleiche Information für alle; in `AT-TAPP-1821` **nein** |
| 9.6 | Wer darf ein Gebot halten? | nur Vorhand: [WP‑DE‑TAPP], **[KRUEPL §Bidding]**, [ATT1821 §19] · jeder früher Sprechende: [WP‑EN‑TAPP], [WP‑DE‑POINT], [WP‑EN‑DREIER], [FURR], **[PAG‑SLO]** | **Senioritätsprinzip** (3 : 5; Unterschied zeigt sich nur, wenn Mittelhand ein Gebot der Hinterhand halten will) |
| 9.7 | Darf man Stufen überspringen? | nein, außer Solo: [WP‑EN‑TAPP] · je ein Gebot pro Spieler: [PAG‑DRLES] | **Nein, außer Solo** |
| 9.8 | Wem gehört der Talon im Solo? | den **Gegnern** [ATT1821 §40], [ALSCHER], [WP‑EN‑TAPP] · Hausbrauch: dem Spieler, von [ATT1821] ausdrücklich gerügt | **Den Gegnern**; Hausbrauch nicht implementieren |
| 9.9 | Wer spielt zum ersten Stich aus? | Vorhand: [ALSCHER], [WP‑EN‑TAPP], [WP‑DE‑ILLU] · Alleinspieler: [FURR] | **Vorhand**; in Negativspielen der Alleinspieler |
| 9.10 | Gibt es Stichzwang? | nein in positiven Spielen: [WP‑DE‑ILLU], [WP‑EN‑DREIER], [PAG‑DRLES] · ja in Negativspielen: [WP‑DE‑KR], [PAG‑SLO] | **Kein Stichzwang**, außer Negativspiele und Trischaken |
| 9.11 | Gilt der Kaiserstich? | im Königrufen „oft gespielte Sonderregel“ [WP‑DE‑KR], im slowenischen Tarok Standard [PAG‑SLO] · für Tapp‑Tarock **nirgends belegt** | **Aus** (`kaiserstich = aus`) |
| 9.12 | Trull/Königstrull: in der Hand oder in den Stichen? | Hand: [ATT1821 §2], [WP‑EN‑TAPP], [WP‑DE‑ILLU] · Stiche: [WP‑EN‑DREIER], [ALSCHER Königrufen] | **In der Hand** für Tapp‑Tarock; `stiche` für `AT-DREIER-54` |
| 9.13 | Valat: Multiplikator oder Festwert? | ×4 / ×8: [ALSCHER], [KRUEPL], [FURR] · 12 / 24: [WP‑DE‑TAPP], [WP‑EN‑TAPP] | **Multiplikator** |
| 9.14 | Überschusspunkte im Tapp‑Tarock? | nein: [ALSCHER], [WP‑EN‑TAPP], [KRUEPL], [FURR] · ja: [WP‑DE‑TAPP], [ATT1821] (doppelt), [WP‑DE‑ILLU] | **Nein** im Klassik‑Profil |
| 9.15 | Pagat ultimo im Solo | ×2: [ALSCHER], [WP‑EN‑TAPP] (8/16) · [WP‑DE‑TAPP] nennt **still 8, angesagt 24** – arithmetisch inkonsistent | **8 / 16**; die 24 ist mit hoher Wahrscheinlichkeit ein Übertragungsfehler |
| 9.16 | Vertauschte Ultimo‑Werte bei [FURR] | [FURR §Scores] listet „Declared Ultimo 4 / Undeclared Ultimo 8“ – umgekehrt zu allen anderen Quellen und zum eigenen Fließtext | **Offensichtlicher Tippfehler**; 4 still / 8 angesagt |
| 9.17 | Wieviel zählt ein Tapper? | halbe Dreier‑Consolation [ATT1821] · 1 Punkt [ALSCHER] | Default **aus**; bei `sofort_ausbezahlt` **1,5** (= halber Dreier) |
| 9.18 | Was passiert, wenn alle passen? | neu geben / Trischaken / Karten behalten [WP‑DE‑ILLU] · Rauben [WP‑DE‑TAPP] · Klop [PAG‑SLO] · *Räuber* [PAG‑DRLES] – **das ist kein Rauben, sondern ein Trischaken**: „the blind is set aside and not used or counted and a Räuber (robber) is played, in which the player who takes most card points loses“ | **Trischaken** |
| 9.19 | ILLU‑Prämienwerte | [WP‑DE‑ILLU] vs. [WP‑EN‑ILLU]/Dummett: Panzer 10/20 vs. 20/40; ohne Trull 4/8 vs. 4/8/16; ohne Könige 5/10 vs. 5/10/15 | **[WP‑DE‑ILLU]**; Unterprofil `AT-TAPP-ILLU-DUMMETT` für die englische Lesart |
| 9.20 | Trischaken: Talon im Spiel? | draußen: [WP‑DE‑ILLU §Vorgehen bei fehlender Spielansage] (drei Spieler), [PAG‑DRLES §Räuber] (drei Spieler: „the blind is set aside and **not used or counted**“) · Zuwaage zu den ersten 6 Stichen: [WP‑DE‑KR], **[PAG‑SLO §Klop] – und zwar auch im Dreierspiel**, denn dort gilt „the rules of play are the same, except where specified below“, und die Klop‑Beschreibung lautet „in each of the **first six tricks** a card is turned up from the top of the talon and added to the trick as a ‚gift‘ … usually called a ‚vitamin‘“ · an den letzten Stich: [WP‑DE‑KR] | **Draußen** (zwei Dreier‑Quellen). **⚠ Berichtigt:** Die frühere Begründung, die Zuwaage sei „für drei Spieler nicht belegt“, ist falsch – [PAG‑SLO] belegt sie für das slowenische Dreierspiel |
| 9.21 | Trischaken: wer zahlt? | fünf verschiedene Prinzipien [WP‑DE‑KR] | **Punktehöchster zahlt allen**, Jungfrau und Bürgermeister aktiv |
| 9.22 | Bettler/Piccolo im Dreierspiel | nur im Königrufen [WP‑DE‑KR], [MS‑TOC] bzw. slowenisch [PAG‑SLO]; **kein** Tapp‑Beleg | **Aus**; Erweiterungsprofil mit Warnhinweis |
| 9.23 | Mondfang | Grazer ILLU‑Regel ±5 [WP‑DE‑ILLU] · Kärntner Dreiertarock 10 [WP‑EN‑DREIER] · slowenisch −20/−21 individuell [PAG‑SLO] · Beck kennt sie nicht | **Aus** im Klassik‑Profil |
| 9.24 | Gewinnschwelle 35⅓ oder 35⅔? | 35⅔ Standard, 35⅓ regional (Steiermark, Polen) [WP‑DE‑KR] | **35⅔ / 36**; im Default‑Profil ohnehin irrelevant (§1.7) |
| 9.25 | Verhältnis zu **Neunzehnerrufen** | Vier Spieler, gleiches 54er‑Blatt, **12 Karten je Spieler**, Talon 2 × 3; der Partner wird über **Tarock XIX** statt über einen König gerufen; die Siegpunkte sind nach Kartenpunkten **abgestuft**; **Prämien aus der Hand sind Pflichtansage**, sie zu vergessen ist Renonce [WP‑DE‑19] | Eigene Variante, **nicht** in dieses Profil mischen. Für die Engine relevant: dieselbe Kartenmenge, dasselbe Zählverfahren, dieselbe Talonmechanik |
| 9.26 | Kann der Spieler die Legage nicht ausführen? | praktisch unmöglich [PAG‑DRLES] | Trullstück offen legen, protokollieren |
| 9.27 | Vierter/fünfter Spieler am Tisch | „König“ mit Mitgewinn [ATT1821] · reiner Aussetzer (moderne Turnierpraxis) · Dreierles: Geber zahlt wie ein dritter Gegner [PAG‑DRLES] | **Aussetzer** |
| 9.28 | Dürfen Gegner Prämien ansagen? | nur Spieler: [WP‑EN‑TAPP] · alle: [WP‑EN‑ILLU] (Löw, Beck) | **Nur Spieler** im Klassik‑Profil |
| 9.29 | Rote Skartindeln: As oder Vierer als niedrigste? | Beim 54er‑Blatt sind **alle vier** (A, 2, 3, 4) vorhanden; die Frage stellt sich nur beim 42er‑Blatt | 54er: keine Frage; 42er: **A** ist die höchste rote Leere |
| 9.30 | Sonderregel für Blätter ohne Tarock | Neugabe: [WP‑EN‑ILLU §Deal (Löw 1954)], [PAG‑SLO §Hand with no taroks] (dort **plus Pflicht‑Klop**) · Annullierung bei ≤ 2 kleinen Tarock: [PAG‑SLO §Variations] (nur „some play“) · **ausdrücklich dagegen: [ATT1821 §47]** („von der Willkühr eingeführt … Die wahre Regel läßt dabey gar keinen Unterschied obwalten“) | **Neugabe bei null Tarock** als Default beibehalten, aber in `AT-TAPP-1821` **aus**; die schärfere Regel bleibt aus (§2.6) |
| 9.31 | Gibt es im Tapp‑Tarock überhaupt Handprämien? | ja: [ALSCHER Tapptarock] (Trull 3, Königstrull 3, Köpfe 1), [WP‑EN‑TAPP] (3/3/1), [ATT1821 §2] (Trull und Könige, **keine Köpfe**) · **nicht erwähnt**: [KRUEPL §Tappen], [FURR §Scores] – beide listen nur Spiel, Pagat ultimo und Valat, [WP‑DE‑TAPP] nennt sie ebenfalls nicht | **Ja**, mit den Werten 3 / 3 / 1; die Belege sind aber **dünner**, als frühere Fassungen suggerierten (§5.3). **⚙ Flag `handpraemien = an (Default) \| aus`** für Runden, die nach [KRUEPL]/[FURR] spielen |
| 9.32 | ILLU: Kontra **vor** Talonkauf – verdoppelt oder vervierfacht? | verdoppelt: [WP‑DE‑ILLU §Kontra Variante nach Beck] · vervierfacht: [WP‑EN‑ILLU §Announcements (Beck)] · gar kein Kontra vor dem Kauf: Grazer Runden [WP‑DE‑ILLU §Kontra] | **Verdoppelt** (`kontra_vor_kauf = verdoppelt`), §5.8 |
| 9.33 | ILLU: Wert des **gehaltenen** Spiels – 5 oder 4? | 5: [WP‑DE‑ILLU §Punkte für das Spiel] und [WP‑EN‑ILLU §Scoring] · 4: die **eigenen durchgerechneten Beispiele** von [WP‑DE‑ILLU §Beispiel für einen Spielverlauf] rechnen ein „gehaltenes Dreiblatt“ zweimal mit 4 | **5** (die Regeltabelle schlägt das Rechenbeispiel); die Beispiele in §7.5 sind wörtlich aus der Quelle übernommen und deshalb inkonsistent – im Code gilt die Tabelle |
| 9.34 | Darf ein Solo gehalten werden? | nein: [ATT1821 §E.3], [WP‑EN‑ILLU §Auction (1954)], [FURR §Bidding], moderne Praxis · **ja, durch einen Spieler, der zuvor einen Dreyer ansagte**: [ATT1821 §19] | **Nein** (`solo_beendet_lizit = true`); in `AT-TAPP-1821` die §19‑Regel, §3.2 |
| 9.35 | Was ist die „vereinfachte Zählung“ wert? | 45 von **90** bei 54 Blatt [KRUEPL §How to count] · 45 von **88** bei 40 Blatt (*Einfaches Tarock*, *Zwanzigerrufen*) [ALSCHER] | **45 / 90**; frühere Fassungen verwechselten die beiden Blätter (§1.8) |

---

## 10. Hinweise für den heuristischen Computerspieler

### 10.1 Architektur

```cpp
struct PlayerView {                 // identisch fuer KI und Netzwerk-Client
    Hand            hand;           // nur die eigenen Karten
    ContractState   contract;       // Stufe, Ansagen, Kontra, wer ist Alleinspieler
    std::vector<Trick> played;      // alle vollstaendigen Stiche
    Trick           current;
    TalonInfo       talon;          // was oeffentlich sichtbar ist
    std::array<int,3> cardCount;    // Restkartenzahl je Sitz
    OpenDiscards    forcedTarocks;  // offen gelegte Tarock (§4.3)
};
```

Die KI besteht aus vier voneinander unabhängigen Modulen: **Blattbewertung** (§10.2), **Lizit** (§10.3),
**Legage** (§10.4) und **Kartenspiel** (§10.5). Alle vier sind rein funktional und damit testbar.

### 10.2 Blattbewertung

Eine für das Dreierspiel brauchbare Punktschätzung des eigenen Blatts:

```
Score =   1.6 · (#Tarock − 7)                       // Tarockzahl ist der wichtigste Faktor
        + 3.0 · [Sküs] + 2.2 · [XXI] + 1.0 · [I]
        + 0.9 · #(Tarock ≥ XVI außer Sküs/XXI)      // "Stecher"
        + 0.4 · #(Tarock X … XV)                    // "Mittelstecher"
        + 1.1 · #Könige
        + 0.5 · #(Damen mit Cavall besetzt)
        + 0.8 · #(leere Farben)                     // sofort stechen koennen
        − 0.7 · #(blanke Damen)                     // fallen fast sicher
        − 0.9 · [XXI ohne Sküs und mit ≤ 2 hohen Deckungstarock]   // Mondfang-Risiko
```

Kalibrierung: 16 von 54 Karten sind **29,6 %** des Blattes; ein Durchschnittsblatt hat **6,5 Tarock**.
Wer 9 oder mehr Tarock hat, hat eine echte Tarockmehrheit gegen **einen** Gegner, aber nicht gegen beide
zusammen – das ist der zentrale Unterschied zum Vierer‑Spiel (§10.7).

### 10.3 Lizitentscheidung

| Ansage | Schwelle (Score) | Zusatzbedingung |
|---|---|---|
| **Weiter** | < 2,0 | – |
| **Dreier** | ≥ 2,0 | mindestens 7 Tarock **oder** 2 Trullstücke |
| **Unterer / Oberer** | ≥ 4,0 | nur, um ein fremdes Gebot zu überbieten; nie als Eröffnung, wenn ein Dreier reicht |
| **Solo** | ≥ 8,5 | ≥ 11 Tarock **und** Sküs **oder** (≥ 10 Tarock und Sküs + XXI); der Talon geht an die Gegner, das kostet im Mittel **3,9 Punkte** |
| **Valat ansagen** | – | nur, wenn eine vollständige Gewinnsequenz berechnet wurde (§10.6) |

**Halten** (Senioritätsrecht): halten, wenn `Score ≥ Schwelle(Stufe) − 0,5`; der Positionsvorteil ist real,
weil die Vorhand ausspielt.

**Wichtige Asymmetrie für die Lizit‑KI:** Die *Oberen* und *Unteren* sind **nicht** stärker als der Dreier,
nur teurer (§3.1). Ein KI‑Spieler darf sie deshalb nie „aus Blattstärke“ ansagen, sondern nur als Überbot.

### 10.4 Talonwahl und Legage

1. **Talonwahl** (nur beim Dreier): Beide Hälften bewerten mit
   `nutzen(h) = Σ Kartenwert(c) + 1.5 · #Tarock(c) + 2.0 · #Trullstücke(c) + 1.0 · #Könige(c)
                − 1.2 · #(neue Farbe, in der man bisher leer war)`.
   Der letzte Term ist entscheidend und wird von Anfängern übersehen: Eine Talonkarte in einer Farbe, in der
   man **leer** war, zerstört die Möglichkeit, diese Farbe zu stechen.
2. **Legage** aus `legalDiscards` (§4.3):
   * Priorität 1: **eine Farbe vollständig leeren**, in der man 1–2 kleine Karten hält.
   * Priorität 2: **blanke Damen und blanke Cavalle** ablegen – sie fallen ohnehin, bringen aber gelegt
     3–4 sichere Punkte.
   * Priorität 3: maximale Punkte pro abgelegter Karte.
   * **Nie** die letzte Karte einer Farbe legen, in der man den König hält (der König wird sonst
     abgeschossen).
   * Historischer Rat: „nur ist immer dabey die vorzügliche Rücksicht zu nehmen, daß man von einer – wo
     möglich von zwey Farben – keine in der Hand behalte, um die von der Gegenpartey ausgespielten Figuren
     derselben mit Tarock einstechen zu können“ [ATT1821 §21].
   * Gegenregel bei kurzem Tarock: Wer wenige Tarock hat, soll **nicht** leer machen – er kann die Farbe
     ohnehin nicht stechen und verliert nur die Kontrolle.
3. **Erzwungene Tarockablage** vermeiden: Sie verrät Information (§4.3) und kostet die KI einen Stecher.

### 10.5 Kartenspiel

**Als Alleinspieler:**

* **Tarockschlagen von oben:** Solange man mehr Tarock hat als jeder einzelne Gegner, hohe Tarock
  ausspielen, um die gegnerischen Tarock zu ziehen und die eigenen Farbkönige abzusichern.
  Rechenregel: Bei *t* eigenen und *g* gegnerischen Tarock sind nach *k* Tarockrunden noch `g − 2k`
  gegnerische Tarock im Spiel.
* **Könige anspielen:** Im Solo lohnt es, einen König der Farbe anzuspielen, von der man die meisten
  Nebenkarten hat; kommt er durch, die Farbe nachspielen [ATT1821 §41].
* **Mond schützen:** Den `XXI` erst spielen, wenn der Sküs draußen ist, oder wenn man ihn mit hohen Tarock
  decken kann. Nie den Mond in eine Position bringen, in der die Hinterhand mit dem Sküs sitzt.
* **Pagat:** Nur ansagen, wenn `#eigene Tarock ≥ ceil(#gegnerische Tarock / 2) + 1` **und** man die
  Ausspielkontrolle behält. Sonst still spielen. Verliert man die Kontrolle, den Pagat **früh** abgeben
  (−5 statt −55, [ATT1821 §23]).

**Als Gegenspieler:**

* Die beiden Gegner sind **immer** Partner, es gibt keine verdeckte Partnerschaft (anders als im
  Königrufen). Die gesamte Inferenzarbeit entfällt – dafür ist die Koordination ohne Absprache das Thema:
  „Warum sich die zwei Gegner gut koordinieren müssen“ [MS‑TOC S. 355].
* **Den Alleinspieler in die Mitte bringen:** Wenn der Spieler in der Mitte sitzt, spielt der Vordermann
  klein an, damit der Hintermann überstechen kann.
* **Schmieren:** Punktkarten in Stiche legen, die der Partner sicher gewinnt. Regel: nur schmieren, wenn der
  Partner den Stich bereits hält **und** der dritte Spieler nicht mehr überstechen kann.
* **Anschießen:** Dem Alleinspieler Tarock herausziehen, solange man in Summe mehr Tarock hat.
* **Gegen einen angesagten Pagat ultimo:** Farben spielen, in denen der Spieler leer ist, um ihn zum
  Tarockzugeben zu zwingen und den Pagat zu blanken; oder die Ausspielkontrolle vor dem letzten Stich
  erobern.
* **Unter dem König ausspielen:** Hält ein Gegner eine Farbe, in der der Spieler vermutlich den König hat,
  spielt er **klein** an, damit der Partner mit einer Figur „lauern“ kann.

### 10.6 Endspiel und exakte Suche

Ab **acht verbleibenden Stichen** (24 Karten) ist eine vollständige Alpha‑Beta‑Suche über
Wahrscheinlichkeitsverteilungen zu teuer. Empfohlen:

* **Ab 5 Reststichen:** Perfect‑Information‑Monte‑Carlo (PIMC) über *n* konsistente Kartenverteilungen
  (Default *n* = 30 auf Sailfish‑Hardware, 100 auf Android), je Verteilung vollständige Alpha‑Beta‑Suche.
  Konsistenz heißt: alle bisher gezeigten Renoncen (ein Spieler hat eine Farbe nicht bedient ⇒ er hat sie
  nicht) und alle offen gelegten Tarock werden als Constraints eingehalten.
* **Ab 3 Reststichen** oder bei angesagtem Pagat ultimo/Valat: **exakte** Suche über alle konsistenten
  Verteilungen, sofern deren Zahl unter einer Schranke liegt.
* Zwischen 16 und 6 Reststichen: regelbasiert nach §10.5, mit einem Tarock‑Zählwerk als einziger
  „Buchhaltung“.

### 10.7 Was das Dreierspiel von Königrufen unterscheidet (KI‑relevant)

1. **Keine verdeckte Partnerschaft** – keine Rufkarten‑Inferenz nötig.
2. **Zwei gegen einen**: Die Gegner haben zusammen im Mittel **13 Tarock** gegen 6,5 des Spielers. Ein Solo
   braucht daher viel mehr als im Vierer‑Spiel.
3. **Der Talon ist verhältnismäßig groß** (6 von 54 = 11 %) und geht bei Solo komplett an die Gegner – im
   Mittel **7,8 Punkte**.
4. **16 Stiche** statt 12: Das Endspiel ist länger, Pagat ultimo ist schwerer abzusichern.
5. Ein **Valat** ist zu dritt fast unmöglich [WP‑EN‑ILLU §Strategy]; die KI sagt ihn nur bei bewiesener
   Gewinnsequenz an.

### 10.8 Schwierigkeitsstufen

| Stufe | Blattbewertung | Spiel | Gedächtnis |
|---|---|---|---|
| **Anfänger** | Score + Rauschen σ = 2,0 | nur §10.5‑Regeln, ohne Tarockzählwerk | vergisst 30 % der gespielten Karten |
| **Fortgeschritten** | Score + σ = 0,8 | §10.5 vollständig, Tarockzählwerk | exakt |
| **Meister** | Score exakt | §10.5 + PIMC ab 5 Reststichen | exakt, plus Renoncen‑Constraints |

Die Stufe **Anfänger** darf Fehler machen, aber **nie Regelverstöße** – sie zieht nur aus `legalPlays`.

### 10.9 Trischaken‑Heuristik

* Ziel ist, **nicht** der Punktehöchste zu sein – nicht, null Stiche zu machen (das ist nur ein Bonus).
* **Früh die hohen Tarock loswerden**, solange andere noch stechen können; wegen des Stichzwangs zieht man
  sonst am Ende alle Punkte ein.
* **Farben anspielen, in denen man lang ist** – man bedient dann selbst und kann klein bleiben.
* **Den Pagat** so lange wie möglich halten (er darf ohnehin erst als letztes Tarock fallen), aber
  einkalkulieren, dass er 5 Punkte kostet, wenn man ihn einstechen muss.
* Wer bereits deutlich vorn (d. h. punktearm) liegt, spielt **defensiv**; wer schon viele Punkte hat, muss
  **aggressiv** einen der beiden anderen überholen lassen.

---

## 11. Lernmodus

Der Lernmodus ist ein eigener Spielmodus mit fest vorgegebenen Blättern, Schritt‑für‑Schritt‑Text und der
Möglichkeit, jederzeit zurückzuspringen. Alle Texte in diesem Abschnitt sind **wörtlich so in der App
anzuzeigen** (Anpassung nur nach Lektorat) und liegen als Übersetzungsschlüssel `learn.*` vor.

### 11.1 Die Phasen eines Spiels, in einfacher Sprache

**Phase 0 – Die Karten kennenlernen.**

> Tarock wird mit 54 Karten gespielt. Es gibt vier Farben – Herz, Karo, Pik und Kreuz – mit je acht Karten,
> und dazu 22 Tarock. Die Tarock sind Trümpfe: sie stechen jede Farbkarte.
>
> In jeder Farbe gibt es vier Bilder: König, Dame, Cavall (das ist der Reiter auf dem Pferd) und Bube.
> Darunter kommen vier kleine Karten, die **Skartindeln** heißen. Und jetzt gut aufpassen, das ist die
> einzige wirklich verzwickte Stelle: **Bei Pik und Kreuz ist die 10 die höchste kleine Karte und die 7 die
> niedrigste. Bei Herz und Karo ist es umgekehrt: das As ist die höchste, die 4 die niedrigste.**
> Merksatz: *Schwarz zählt abwärts, Rot zählt aufwärts.*
>
> Drei Tarock haben Namen: der **Sküs** (auch Gstieß) ist der höchste Tarock überhaupt, der **Mond** ist der
> XXI, und der **Pagat** ist der I – der niedrigste Tarock, und trotzdem die wichtigste Karte im ganzen
> Spiel. Diese drei zusammen heißen die **Trull**.

**Phase 1 – Geben.**

> Einer gibt. Er legt zuerst sechs Karten verdeckt in die Mitte: das ist der **Talon**, auf Wienerisch das
> **Tapp** – daher heißt das Spiel Tapp‑Tarock. Der Talon liegt als zwei Päckchen zu je drei Karten da, eine
> **obere** und eine **untere** Hälfte. Dann bekommt jeder Spieler 16 Karten.
>
> Gespielt wird gegen den Uhrzeigersinn, also nach rechts. Der Spieler rechts vom Geber heißt **Vorhand**,
> der nächste **Mittelhand**, der Geber selbst **Hinterhand**.

**Phase 2 – Lizitieren (Ansagen).**

> Jetzt wird ausgehandelt, wer **allein gegen die beiden anderen** spielt. Anders als bei vielen Kartenspielen
> gibt es hier keine Partner, die sich suchen müssen: Einer spielt allein, zwei spielen zusammen.
>
> Die Vorhand fängt an. Sie sagt entweder **„Weiter!“** (das heißt: ich will nicht) oder sie sagt ein Spiel
> an. Dann ist der Nächste dran. Wer einmal „Weiter“ gesagt hat, ist raus und darf nichts mehr sagen.
>
> Die Spiele der Reihe nach, von leicht nach schwer:
> **Dreier** (3 Punkte) – **Unterer** (4) – **Oberer** (5) – **Solo** (8).
> Man darf immer nur **eine Stufe höher** gehen. Nur „Solo“ darf man jederzeit sofort sagen.
>
> Und es gibt das **Halten**: Wenn du vor jemandem gesprochen hast und der überbietet dich, darfst du sagen
> **„Halte ich!“**. Dann spielst **du** das Spiel, zum selben Preis. Du hast den Vortritt, weil du früher
> dran warst.

**Phase 3 – Der Talon.**

> Der Alleinspieler deckt die zwei Talonhälften auf. Bei einem **Dreier** darf er sich eine der beiden
> aussuchen. Bei einem **Unteren** muss er die untere nehmen, bei einem **Oberen** die obere – das hat er
> ja vorher angesagt, ohne die Karten zu kennen. Deshalb sind diese Spiele mehr wert.
>
> Er nimmt die drei Karten auf, hat also 19, und legt dafür **drei Karten verdeckt zurück**. Das heißt
> **Verlegen** oder **Legage**. Diese drei Karten gehören ihm – ihre Punkte zählen am Ende zu seinen
> Stichen. Die drei Karten der anderen Talonhälfte gehören den Gegnern.
>
> Drei Sachen darf man **nicht** verlegen: **keinen König**, **kein Trullstück** (Sküs, Mond, Pagat) und
> **kein Tarock** – außer, es geht gar nicht anders; dann muss man das Tarock **offen** hinlegen, damit
> alle es sehen.
>
> Beim **Solo** rührt man den Talon gar nicht an: Alle sechs Karten gehören den Gegnern. Deswegen ist das
> Solo so schwer.

**Phase 4 – Ansagen und Kontra.**

> Wenn der Alleinspieler verlegt hat, sagt er **„Ich liege“**. Vorher oder jetzt kann er noch eine Prämie
> ansagen, meistens den **Pagat ultimo**: „Ich mache den letzten Stich mit dem Pagat.“ Das bringt doppelt so
> viel wie ohne Ansage – aber wenn es schiefgeht, zahlt er genauso viel.
>
> Dann sagen die beiden Gegner reihum **„Gut“** oder **„Kontra“**. Kontra verdoppelt. Der Alleinspieler darf
> mit **„Rekontra“** noch einmal verdoppeln, die Gegner mit **„Subkontra“** ein drittes Mal.

**Phase 5 – Spielen.**

> Die **Vorhand** spielt zur ersten Karte aus – auch dann, wenn sie gar nicht der Alleinspieler ist. Danach
> spielt immer der aus, der den letzten Stich gewonnen hat.
>
> Drei Regeln, und zwar in genau dieser Reihenfolge:
> 1. **Farbzwang:** Hast du die ausgespielte Farbe, musst du sie bedienen.
> 2. **Tarockzwang:** Hast du die Farbe nicht, musst du ein Tarock spielen.
> 3. Hast du beides nicht, darfst du irgendeine Karte abwerfen – die gewinnt dann natürlich nicht.
>
> **Aber:** Du musst den Stich **nicht** gewinnen! Du darfst ein kleineres Tarock zugeben, obwohl du ein
> größeres hättest. Das ist der wichtigste Unterschied zu vielen anderen Kartenspielen und der Grund, warum
> Tarock so viel Spaß macht.

**Phase 6 – Zählen.**

> Jede Seite legt ihre Stiche zusammen – der Alleinspieler dazu seine drei verlegten Karten, die Gegner dazu
> die übrig gebliebene Talonhälfte. Dann wird in **Dreierlagen** gezählt: immer drei Karten zusammen, ihre
> Werte addieren, **2 abziehen**.
>
> Die Werte: Sküs, Mond, Pagat und die vier Könige zählen **5**, Damen **4**, Cavalle **3**, Buben **2**,
> alles andere **1**.
>
> Zusammen sind es 70 Punkte. Der Alleinspieler braucht **36**, um zu gewinnen. Bei 35 zu 35 hat er
> **verloren** – ein Unentschieden gibt es nicht.

### 11.2 Welches Spiel sage ich wann an?

> **Zähl zuerst deine Tarock.** Du hast 16 Karten von 54. Im Schnitt sind das **6 bis 7 Tarock**. Alles ab
> 8 ist gut, alles ab 10 ist stark.
>
> **Dann schau auf die Trull.** Sküs und Mond sind die beiden Karten, mit denen du Stiche sicher gewinnst.
> Wer den Sküs hat, hat den einzigen Tarock, den niemand überstechen kann.
>
> **Dann zähl die Könige.** Jeder König sind 5 Punkte – so viel wie ein Trullstück.

| Dein Blatt | Ansage | Warum |
|---|---|---|
| Weniger als 7 Tarock, kein Trullstück | **Weiter** | Zwei Gegner haben zusammen fast doppelt so viele Tarock wie du. Passen ist keine Schande |
| 7–8 Tarock, dazu Sküs oder Mond, 2 Könige | **Dreier** | Der Normalfall. Du darfst dir die bessere Talonhälfte aussuchen |
| Ein Blatt, mit dem du einen Dreier spielen würdest, aber ein anderer war schneller | **Unterer**, dann **Oberer** | Nicht weil das Blatt besser ist, sondern weil du das Spiel haben willst. Du zahlst dafür mit dem Risiko, die falsche Hälfte zu kriegen |
| 10–11 Tarock **mit** Sküs, oder 11+ Tarock | **Solo** | Nur dann. Denk daran: Beim Solo bekommen die Gegner alle sechs Talonkarten geschenkt, im Schnitt sind das fast 8 Punkte |
| Alles an Tarock von oben herunter, kein Loch | **Solo** und dann **Valat** ansagen | Kommt praktisch nie vor. Zu dritt ist ein Valat fast unmöglich |

> **Und der Pagat ultimo?** Den sagst du nur an, wenn du genug Tarock hast, um **alle** gegnerischen Tarock
> herauszuziehen, bevor der letzte Stich kommt. Rechne so: Zieh deine Tarockzahl von 22 ab – so viele haben
> die Gegner zusammen. Jede Tarockrunde, die du ausspielst, holt **zwei** davon herunter. Wenn du 11 Tarock
> hast, haben die Gegner 11; nach sechs Runden sind sie leer, und du hast noch fünf Tarock – der Pagat ist
> sicher. Wenn du nur 8 Tarock hast, haben die Gegner 14, und du wirst sie nicht los.
>
> **Wenn es schiefgeht:** Gib den Pagat lieber früh ab, statt ihn bis zum Schluss aufzuheben und dann zu
> verlieren. So kostet er dich 5 Punkte statt der ganzen Prämie. Das steht schon im ältesten Tarockbuch von
> 1821 – dort heißt es, so verliere man „nur fünf“ statt „fünf und fünfzig“.

### 11.3 „Warum darf ich diese Karte nicht spielen?“

Die App sperrt unerlaubte Karten. Tippt der Spieler eine gesperrte Karte an, erscheint **genau einer** der
folgenden Texte. Die Texte sind vollständig – jede Sperre, die die Engine kennt, hat hier ihren Wortlaut.

| Code | Auslöser | Anzuzeigender Text |
|---|---|---|
| `FARBZWANG` | Spieler hat die ausgespielte Farbe noch im Blatt | **„Farbzwang.“** Es wurde **{Farbe}** ausgespielt, und du hast noch **{Farbe}** auf der Hand. Du musst **{Farbe}** bedienen. Gewinnen musst du den Stich aber nicht – such dir aus, welche **{Farbe}**-Karte du gibst. |
| `TAROCKZWANG` | Spieler hat die Farbe nicht, aber Tarock | **„Tarockzwang.“** Du hast kein **{Farbe}** mehr. Dann musst du ein **Tarock** zugeben. Auch hier gilt: du musst den Stich nicht gewinnen, ein kleines Tarock reicht. |
| `TAROCK_AUSGESPIELT` | Tarock ist ausgespielt, Spieler hat Tarock | **„Es wurde Tarock ausgespielt.“** Tarock ist hier wie eine Farbe: Wenn du Tarock hast, musst du Tarock zugeben. |
| `STICHZWANG` | Negativspiel/Trischaken, Spieler könnte überbieten | **„Stichzwang.“** In diesem Spiel musst du die bisher höchste Karte im Stich übertreffen, wenn du kannst. Du hast **{Karte}** – damit kommst du über **{aktuelle Siegerkarte}** drüber, also musst du sie auch spielen. |
| `STICHZWANG_FARBE` | Stichzwang innerhalb der bedienten Farbe | **„Stichzwang in der Farbe.“** Du musst **{Farbe}** bedienen **und** dabei **{aktuelle Siegerkarte}** überbieten, solange du eine höhere **{Farbe}**-Karte hast. |
| `PAGAT_LETZTES_TAROCK` | Negativspiel/Trischaken, Pagat gewählt, noch andere Tarock | **„Der Pagat muss warten.“** In diesem Spiel darfst du den Pagat erst spielen, wenn er dein **letztes Tarock** ist. Du hast noch **{n}** andere Tarock. |
| `LEGAGE_KOENIG` | König in die Legage gelegt | **„Könige darfst du nicht verlegen.“** Ein König ist 5 Punkte wert; die Regel verbietet, ihn einfach beiseitezulegen. Leg lieber eine kleine Karte ab. |
| `LEGAGE_TRULL` | Sküs/Mond/Pagat in die Legage gelegt | **„Trullstücke darfst du nicht verlegen.“** Sküs, Mond und Pagat bleiben im Spiel – sie sind die drei wichtigsten Karten und je 5 Punkte wert. |
| `LEGAGE_TAROCK` | Tarock gelegt, obwohl erlaubte Farbkarten vorhanden | **„Tarock darfst du nur im Notfall verlegen.“** Du hast noch **{n}** Farbkarten, die du legen darfst. Ein Tarock dürftest du nur dann ablegen, wenn dir gar nichts anderes bleibt – und dann müsstest du es **offen** hinlegen. |
| `LEGAGE_TAROCK_OFFEN` | Tarock muss gelegt werden (Notfall) – Hinweis, keine Sperre | **„Dieses Tarock wird offen gelegt.“** Du hast keine andere Karte, die du verlegen dürftest. Deshalb kommt das Tarock **offen** auf den Tisch – die Gegner dürfen sehen, welches es ist. |
| `LEGAGE_ANZAHL` | Falsche Anzahl Karten in der Legage | **„Es müssen genau {n} Karten sein.“** Du hast **{m}** ausgewählt. Tipp eine Karte an, um sie wieder zurückzunehmen. |
| `LIZIT_STUFE` | Gebot übersprungen | **„Immer nur eine Stufe höher.“** Das letzte Gebot war **{Stufe}**. Du kannst also **{nächste Stufe}** sagen, „Weiter“ – oder gleich **Solo**, das darf man jederzeit. |
| `LIZIT_PASSED` | Spieler hat schon gepasst | **„Du hast schon ‚Weiter‘ gesagt.“** Wer einmal gepasst hat, ist für dieses Spiel aus dem Lizit draußen. |
| `LIZIT_HALTEN` | Halten ohne Senioritätsrecht | **„Halten darf nur, wer früher gesprochen hat.“** **{Spieler}** hat vor dir angesagt, deshalb kannst du sein Gebot nicht halten. Du müsstest höher bieten oder passen. |
| `KONTRA_ZEITPUNKT` | Kontra vor „Ich liege“ | **„Noch nicht.“** Kontra gibt man erst, wenn der Alleinspieler verlegt hat und **„Ich liege“** sagt – vorher weißt du ja nicht, wie gut sein Blatt geworden ist. |
| `KONTRA_SPIELER` | Alleinspieler will kontrieren | **„Kontra gibt nur die Gegenseite.“** Du bist der Alleinspieler. Wenn die Gegner Kontra geben, darfst **du** mit **Rekontra** antworten. |
| `ULTIMO_ZEITPUNKT` | Pagat‑Ansage nach dem ersten Stich | **„Zu spät.“** Den Pagat ultimo muss man ansagen, **bevor** die erste Karte gespielt wird. Du kannst ihn immer noch **still** machen – dann zählt er die Hälfte. |

**Mechanik der Meldung.** Die Engine liefert zu jeder gesperrten Karte einen `RejectReason { code, params }`;
die UI setzt den Text zusammen. Im Modus „ohne Hilfe“ wird die Karte nur ausgegraut, ohne Text.

### 11.4 Typische Anfängerfehler

1. **„Ich muss doch stechen!“** – Nein. In den normalen Spielen gibt es **keinen Stichzwang** (§6.4).
   Wer das aus Schnapsen, Skat oder Schafkopf mitbringt, verschenkt seine hohen Tarock in Stichen, die gar
   nichts wert sind.
2. **Die roten und schwarzen kleinen Karten verwechseln.** Bei Pik/Kreuz gewinnt die 10 gegen die 9, bei
   Herz/Karo gewinnt das As gegen die 2 (§1.3). Die App zeigt im Lernmodus die Rangfolge der ausgespielten
   Farbe als kleine Leiste am Rand an.
3. **Den Cavall für einen Buben halten.** Der Cavall sitzt auf einem Pferd, der Bube steht und hält eine
   Hellebarde; der König trägt Krone und Zepter.
4. **Den Mond zu früh spielen.** Wer den `XXI` ausspielt, während der Sküs noch im Spiel ist, riskiert
   5 Punkte und in manchen Profilen die Mondfang‑Prämie (§5.4).
5. **Beim Verlegen die falsche Karte behalten.** Eine **blanke Dame** fällt fast immer; sie ist gelegt
   4 sichere Punkte wert. Umgekehrt: Nie die letzte Karte einer Farbe legen, in der man den König hat
   (§10.4).
6. **Beim Verlegen keine Farbe leer machen.** Wer von einer Farbe gar nichts mehr hat, darf sie mit Tarock
   stechen – der Rat steht schon 1821 in der Quelle [ATT1821 §21].
7. **Pagat ultimo aus Übermut ansagen.** Die häufigste teure Fehlentscheidung. Rechenregel §11.2.
8. **Den Pagat blank bis zum Schluss aufheben.** Lieber früh abgeben (§5.2).
9. **Vergessen, Handprämien anzumelden.** Trull, Königstrull und Köpfe zählen nur, wenn man sie meldet – sie
   dürfen nicht „verpaßt“ werden [ATT1821 §2]. Die App fragt automatisch, im Lernmodus mit Erklärung.
10. **Als Gegenspieler alleine spielen.** Die beiden Gegner sind **immer** Partner. Wer seinem Partner keine
    Punkte in sichere Stiche schmiert, verschenkt das Spiel.
11. **Vergessen, dass die Legage dem Spieler gehört.** Punkte, die man verlegt, sind nicht weg – sie zählen
    am Ende zu den eigenen Stichen (§4.1).
12. **35 für einen Sieg halten.** 35 : 35 ist **verloren** (§1.7).

### 11.5 Zusatzmodule des Lernmodus

* **Zählschule.** Eine Übung, in der Kartenstapel in Dreierlagen zerlegt und die Summen abgefragt werden.
  Drei Schwierigkeitsgrade: Rechenwerte ablesen → Dreierlagen bilden → im Kopf mitzählen während des Spiels.
* **Tarockzähler.** Ein optionaler Anzeiger „noch X Tarock im Spiel, davon Y bei den Gegnern“. In der Stufe
  *Anfänger* dauerhaft eingeblendet, in *Fortgeschritten* nur auf Anforderung, in *Meister* gar nicht.
  Die Quelle von 1821 nennt das Mitzählen der Tarock „immer sehr vortheilhaft, bey Absichten auf den Pagat
  aber unerläßlich“ [ATT1821 §23].
* **Nachbesprechung.** Nach jedem Spiel eine Seite: Was hätte die Meister‑KI angesagt? Welche Karte war der
  Wendepunkt? Wie viele Punkte lagen in der nicht genommenen Talonhälfte?
* **Geschichtsseite.** Kurztexte zur Herkunft (Wien um 1800, Kaffeehäuser, Café Central, Herzmanovsky‑Orlando,
  Tapp = die verdeckten Karten in der Mitte) mit dem Profil `AT-TAPP-1821` als spielbarer Kostprobe.

### 11.6 Glossar

| Begriff | Erklärung |
|---|---|
| **Anlaufen lassen** | Mit guter Karte in der Vorhand einen Tapper ansagen, damit sich ein anderer mit schwächerem Blatt zu einem Dreier verleiten lässt [ATT1821 §Erklärung] |
| **Beißer** | Die Tarock, die nicht mehr überstochen werden können [ATT1821 §Erklärung] |
| **Bekennen** | Eine Farbe oder Tarock nicht verleugnen, sondern richtig zugeben, solange man sie hat [ATT1821 §Erklärung] |
| **Bey Hofe speisen** | Scherzwort für Könige, die mit Tarock gestochen werden, weil ihre Farbe bei einem Gegner fehlt [ATT1821 §Erklärung] |
| **Blatt** | Zwei Bedeutungen: (a) die eigenen Handkarten, (b) eine Karte im Wert von ⅓ Punkt, also jedes Tarock außer der Trull und jede Skartindel |
| **Cavall** (Kavall, Reiter) | Die Figur zwischen Dame und Bube, erkennbar am Pferd; 3 Punkte |
| **Cavallerie** | Sammelname für die Figuren der Farben; „eine Karte mit viel Cavallerie und wenig oder gar keinen Taroks ist gut zum Schmieren“ [ATT1821 §Erklärung] |
| **Consolation** | Der 1821 vor der Partie vereinbarte Grundwert eines Spiels (50, 40 oder 30); Vorläufer der heutigen Spielwerte [ATT1821 §5] |
| **Contra ultimo** | Ein Gegenspieler macht den letzten Stich mit dem Pagat |
| **Dreier** | Das Grundspiel: der Alleinspieler nimmt drei Talonkarten |
| **Dreierlage** | Drei Karten, die beim Zählen zusammengefasst werden; von ihrer Summe werden 2 abgezogen |
| **Farbzwang** | Pflicht, die ausgespielte Farbe zu bedienen |
| **Force** | (a) die meisten Karten einer Farbe halten und die anderen dadurch zum Stechen zwingen; (b) eine ununterbrochene Folge hoher Tarock [ATT1821 §Erklärung] |
| **Für die Küche sorgen** | Mit schlechtem Blatt möglichst viele Augen in die Legage packen, z. B. drei Damen [ATT1821 §Erklärung] |
| **Gstieß / Gschtieß** | Anderer Name für den Sküs |
| **Heim gehen / nach Hause nehmen** | Den Pagat, den König oder den Mond rechtzeitig in Sicherheit bringen, statt auf mehr zu warten [ATT1821 §Erklärung] |
| **Halten** | „Halte ich!“ – ein früher sprechender Spieler übernimmt das Gebot eines späteren |
| **Hinterhand** | Der Geber; spricht als Letzter |
| **Honneurs** | Ältere Bezeichnung für die hohen Karten; 1821 für die vier Könige, *Matadors* für die Trull |
| **Ich liege** | Ansage des Alleinspielers, dass er verlegt hat und das Spiel beginnen kann |
| **Kaiserstich** (Märchenstich) | Optionale Sonderregel: Fallen alle drei Trullstücke in einen Stich, gewinnt der Pagat |
| **Köpfe** | Zwei der drei Trullstücke im Blatt |
| **Kontra** | Verdopplung durch die Gegenseite; Antwort *Rekontra*, darauf *Subkontra* |
| **Legage / Verlegen** | Die drei Karten, die der Alleinspieler nach der Talonaufnahme verdeckt zurücklegt; sie zählen zu seinen Stichen |
| **Lizitation** | Das Bieten um das Spiel |
| **Mittelhand** | Der Spieler zwischen Vorhand und Hinterhand |
| **Mond** | Tarock XXI |
| **Mondfang** | Der Mond wird vom Sküs überstochen |
| **Pagat** (Spatz) | Tarock I |
| **Pagat ultimo** | Den letzten Stich mit dem Pagat gewinnen |
| **Prämie** | Zusatzpunkte für eine Ansage oder eine Kartenkombination |
| **Remis** | 35 : 35 – gilt als Verlust des Alleinspielers |
| **Renonce** | Regelverstoß, etwa nicht bedienen |
| **Reitzkarte** | Ein Blatt, das einen Dreier verspricht (ein paar Könige, ein Matador, hohe Tarock, ein Natur‑Scat) und trotzdem verloren geht [ATT1821 §Erklärung] |
| **Scat** | Bezeichnung von 1821 für (a) die sechs Talonkarten, (b) die Legage, (c) eine Farbe, in der man nach dem Verlegen leer ist. **Natur‑Scat** = schon beim Geben fehlende Farbe; **Scatinel** = die einzige verbliebene Karte einer Farbe [ATT1821 §Erklärung] |
| **Schinden** (lauern, necken) | Mit dem König auf die Dame warten, um ihr den Stich wegzunehmen [ATT1821 §Erklärung] |
| **Schlafen** | Von Karten, die im Talon liegen; eine ganze Farbe kann „schlafen“ [ATT1821 §Erklärung] |
| **Schmieren** | Dem Partner Punktkarten in einen sicheren Stich legen; 1821: „das Zuwerfen der Figuren auf die Stiche des Helfers, wenn man weder die verlangte Farbe noch Taroks hat“ |
| **Skartindel** (Skatindel, Glatze) | Die vier kleinen Karten einer Farbe |
| **Sküs** | Der höchste Tarock; hat **keine** Sonderrolle wie der Narr im französischen Tarot |
| **Solo** | Spiel ohne Talon; alle sechs Talonkarten zählen für die Gegner |
| **Stich** | Drei Karten, eine von jedem Spieler |
| **Stichzwang** | Pflicht, den Stich zu übernehmen – gilt **nur** in Negativspielen und im Trischaken |
| **Talon / Tapp** | Die sechs verdeckten Karten in der Tischmitte, in zwei Hälften zu drei |
| **Tapper** | Die historische, heute nicht mehr gespielte niedrigste Ansage mit allen sechs Talonkarten |
| **Tarock** | (a) das Spiel, (b) eine der 22 Trumpfkarten |
| **Tarockzwang** | Pflicht, Tarock zuzugeben, wenn man die ausgespielte Farbe nicht hat |
| **Trischaken** | Spiel jeder gegen jeden, wenn alle passen; wer die meisten Punkte hat, verliert |
| **Trull** | Sküs, Mond und Pagat zusammen; von frz. *tous les trois* |
| **Uhu / Kakadu / Marabu** | Spitznamen für die Tarock II, III und IIII |
| **Valat** | Alle 16 Stiche gewinnen |
| **Vorhand** | Der Spieler rechts vom Geber; eröffnet Lizit und Spiel |
| **Weiter!** | Passen |

---

### 11.7 Drei Übungspartien mit festem Blatt

Die Deals sind **fest** (Seed 0 der Lernmodus‑Deals) und vollständig geprüft: 3 × 16 Handkarten + 6 Talon =
54 Karten, keine Karte doppelt, keine fehlt. Die Sitze heißen **V** (Vorhand, das ist der Lernende),
**M** (Mittelhand) und **H** (Hinterhand, der Geber). Gespielt wird gegen den Uhrzeigersinn: V → M → H.
Profil: `AT-TAPP-KLASSIK`.

Notation der Schritte: **[Anzeige]** = Text, den die App zeigt; **[Aktion]** = was passiert bzw. was der
Lernende antippen soll.

---

#### Übungspartie 1 – „Mein erster Dreier“

**Lernziel:** Lizitieren, Talonhälften bewerten, richtig verlegen, Farb‑ und Tarockzwang, Zählen.

| Sitz | Tarock | ♥ | ♦ | ♠ | ♣ |
|---|---|---|---|---|---|
| **V** | Sküs, XXI, XVIII, XV, XII, IX, V, II | K, D, 4 | 3 | K, 9 | B, 8 |
| M | XX, XVII, XIV, XI, VIII, IIII, I | C, B | A, D, 2 | D, B | K, 10 |
| H | XIX, XVI, XIII, X, VII, VI, III | A, 2, 3 | C, B | C, 10 | D, C |
| **Talon oben** | – | – | **♦K, ♦4** | **♠7** | – |
| **Talon unten** | – | – | – | **♠8** | **♣9, ♣7** |

**Schritt 1 – Blatt ansehen.**
> **[Anzeige]** Schau dir dein Blatt an. Zähl zuerst die Tarock: **acht** – das ist überdurchschnittlich,
> der Schnitt liegt bei sechs oder sieben. Und du hast die beiden stärksten: den **Sküs** und den **Mond**.
> Dazu zwei Könige. Das ist ein Blatt für einen Dreier.

**Schritt 2 – Lizit.**
> **[Aktion]** Tippe **„Dreier!“**.
> **[Anzeige]** Du sagst als Vorhand den Dreier an. Damit bietest du an, allein gegen die beiden anderen zu
> spielen und dir eine Talonhälfte auszusuchen.

> **[Anzeige]** M sagt **„Weiter!“**, H sagt **„Weiter!“**. Beide passen – ihre Blätter sind zu schwach.
> Zwei „Weiter“ hintereinander beenden das Lizit. **Du bist der Alleinspieler.**

**Schritt 3 – Talon.**
> **[Anzeige]** Beide Talonhälften werden aufgedeckt. Rechne kurz mit:
> **Obere Hälfte: ♦König, ♦4, ♠7** – das sind 5 + 1 + 1 = 7, minus 2 macht **5 Punkte**.
> **Untere Hälfte: ♠8, ♣9, ♣7** – das sind 1 + 1 + 1 = 3, minus 2 macht **1 Punkt**.
> **[Aktion]** Nimm die **obere** Hälfte.
> **[Anzeige]** Richtig. Der Karo‑König ist allein schon 5 Punkte wert und gibt dir einen dritten König.
> Die untere Hälfte wandert verdeckt zu den Gegnern und zählt am Ende für sie – ein Punkt, damit kannst du
> leben.

**Schritt 4 – Verlegen.**
> **[Anzeige]** Du hast jetzt 19 Karten und musst drei verdeckt zurücklegen. Die zählen später **für dich**,
> sie sind also nicht verloren. Verboten sind Könige, Trullstücke und Tarock.
> **[Aktion]** Leg **♣Bube, ♣8 und ♥4** weg.
> **[Anzeige]** Sehr gut, und zwar aus zwei Gründen:
> **Erstens** bist du damit in **Kreuz blank** – du hast keine Kreuzkarte mehr. Wenn ein Gegner Kreuz
> ausspielt, darfst du mit Tarock stechen. Genau das rät schon das Tarockbuch von 1821.
> **Zweitens** sicherst du dir 2 + 1 + 1 = 4 Kartenpunkte, ohne dafür einen Stich machen zu müssen.
> Deine Könige bleiben gedeckt: Der ♥König hat die ♥Dame hinter sich, der ♦König die ♦3 und die ♦4, der
> ♠König die ♠9 und die ♠7.

**Schritt 5 – „Ich liege“.**
> **[Anzeige]** Du sagst **„Ich liege“**. Jetzt dürften die Gegner kontrieren – beide sagen **„Gut“**.
> Eine Prämie sagst du nicht an: Den Pagat hast du gar nicht, der liegt bei M.

**Schritt 6 – Stich 1.**
> **[Aktion]** Spiel den **Sküs** aus.
> **[Anzeige]** Weil du Tarock ausgespielt hast, **müssen beide Gegner Tarock zugeben** – das ist der
> Tarockzwang. Sie geben ihre kleinsten: M den IIII, H den III.
> Der Stich gehört dir: 5 + 1 + 1 = 7, minus 2 = **5 Punkte**.
> So zieht man den Gegnern die Tarock heraus, damit sie später deine Könige nicht mehr stechen können.

**Schritt 7 – Stich 2.**
> **[Aktion]** Spiel den **Mond (XXI)**.
> **[Anzeige]** Der Sküs ist weg – also kann dir jetzt niemand mehr den Mond wegnehmen. **Jetzt** ist der
> richtige Moment. M gibt den VIII, H den VI. Wieder **5 Punkte** für dich. Stand: 10.

**Schritt 8 – Die wichtigste Entscheidung.**
> **[Anzeige]** Weiter Tarock schlagen? Rechne nach: Du hattest acht Tarock, die Gegner zusammen vierzehn.
> Vier sind gefallen, zehn sind noch bei ihnen – und darunter der **XX** und der **XIX**, die beide höher
> sind als alles, was du noch hast. Wenn du jetzt den XVIII ausspielst, verschenkst du ihn.
> **[Aktion]** Spiel stattdessen den **♦König**.
> **[Anzeige]** M hat Karo (A, D, 2), H hat Karo (C, B) – **beide müssen bedienen**, und keiner hat etwas
> Höheres als den König. M wirft die ♦2, H den ♦Buben. 5 + 1 + 2 = 8, minus 2 = **6 Punkte**. Stand: 16.

**Schritt 9 – Stich 4.**
> **[Aktion]** Spiel den **♠König**.
> **[Anzeige]** Dasselbe noch einmal: M hat Pik (D, B), H hat Pik (C, 10), beide müssen bedienen. M wirft
> den ♠Buben, H die ♠10. 5 + 2 + 1 = 8, minus 2 = **6 Punkte**. Stand: 22.

**Schritt 10 – Zwischenbilanz.**
> **[Anzeige]** Vier Stiche plus deine Legage: 5 + 5 + 6 + 6 = 22, dazu die Legage (♣Bube, ♣8, ♥4 = 2 + 1 + 1
> = 4, minus 2 = **2**). Macht **24 Punkte**. Du brauchst **36**. Zwölf Stiche sind noch offen, und du hast
> noch sechs Tarock und den ♥König.
> **Zum Schluss nicht vergessen:** Du hattest **Sküs und Mond** im Blatt – das sind die **Köpfe**, zwei
> Trullstücke, und die bringen dir **1 Punkt extra** von jedem Gegner. Aber nur, wenn du sie **anmeldest**.
> **[Aktion]** Ab hier spielst du selbst weiter.

---

#### Übungspartie 2 – „Der angesagte Pagat ultimo“

**Lernziel:** Solo erkennen, Tarock zählen, den Pagat ultimo ansagen und absichern.

| Sitz | Tarock | ♥ | ♦ | ♠ | ♣ |
|---|---|---|---|---|---|
| **V** | Sküs, XXI, XX, XIX, XVII, XV, XIII, XI, IX, VII, **I** | K, C | – | K | D, 9 |
| M | XVIII, XIV, X, VI, IIII | D, B, A, 2 | K, C, A | D, 10 | K, 7 |
| H | XVI, XII, VIII, V, III, II | 3, 4 | D, B | C, B, 9 | C, B, 10 |
| **Talon oben** | – | – | **♦2** | **♠7** | **♣8** |
| **Talon unten** | – | – | **♦3, ♦4** | **♠8** | – |

**Schritt 1 – Blatt ansehen.**
> **[Anzeige]** **Elf Tarock.** Und darunter die komplette **Trull**: Sküs, Mond und Pagat. Dazu bist du in
> **Karo blank** – du hast keine einzige Karokarte, kannst also jedes Karo stechen.
> Rechne mit: 22 Tarock gibt es. Elf hast du, elf haben die beiden Gegner **zusammen**.

**Schritt 2 – Dreier oder Solo?**
> **[Anzeige]** Beim Solo rührst du den Talon nicht an, und alle sechs Karten zählen für die Gegner – im
> Schnitt kostet dich das acht Punkte. Dafür ist das Solo **8 statt 3** wert, und ein angesagter Pagat
> ultimo zählt darin **16 statt 8**. Mit elf Tarock und der ganzen Trull ist das Blatt stark genug.
> **[Aktion]** Tippe **„Solo!“**.
> **[Anzeige]** Ein Solo darf man jederzeit sofort ansagen, und es beendet das Lizit auf der Stelle. M und H
> kommen gar nicht mehr zu Wort.

**Schritt 3 – Die Ansage.**
> **[Anzeige]** Jetzt die Rechnung für den **Pagat ultimo**: Die Gegner haben **elf** Tarock. Jedes Mal, wenn
> du Tarock ausspielst, geben sie **zwei** ab. Nach **sechs** Tarockrunden sind ihre elf Tarock weg – und du
> hast dann noch **fünf** Tarock übrig, darunter den Pagat. Ab da kann dir niemand mehr dazwischenfahren.
> **[Aktion]** Sag **„Pagat ultimo“** an und leg den Pagat offen vor dich hin.
> **[Anzeige]** So macht man das seit 1821: Der Pagat liegt offen links neben dir, „damit die Gegenpartey
> gehörig unterrichtet sey“. Angesagt ist er **16** wert statt 8 – aber wenn er dir verloren geht, zahlst du
> dieselben 16 an **jeden** Gegner.

**Schritt 4 – Kontra.**
> **[Anzeige]** M und H sagen beide **„Gut“**. Sie sehen elf Tarock bei dir und glauben dir die Ansage.

**Schritt 5 bis 8 – Tarock herunterschlagen.**
> **[Aktion]** Spiel **Sküs**, dann **XXI**, dann **XX**, dann **XIX** – vier Runden hintereinander.
> **[Anzeige]** Jede Runde nimmt den Gegnern zwei Tarock ab. Nach vier Runden sind acht ihrer elf Tarock
> gefallen; drei sind noch draußen. Deine Stiche: 5, 5, 1 und 1 – zusammen **12 Punkte**.
> Spiel immer **von oben herunter**, solange du der Höchste bist. So verlierst du keinen Stich und keinen
> Punkt.

**Schritt 9 – Ein Stich geht verloren.**
> **[Aktion]** Spiel den **XVII**.
> **[Anzeige]** M legt den **XVIII** darauf – der ist höher, den hattest du nicht. H gibt seinen XVI dazu,
> und M gewinnt den Stich mit **1 Punkt**.
> Das ist kein Fehler, sondern der Preis: Du musstest die restlichen Tarock herausziehen, und dafür durfte
> auch ein Stich verloren gehen. Entscheidend ist: M hat jetzt **kein Tarock mehr**.

**Schritt 10 – Der Tarockzwang arbeitet für dich.**
> **[Anzeige]** M spielt den **♦König** aus. Du hast kein Karo – also **musst** du Tarock zugeben
> (Tarockzwang). Das trifft sich gut.
> **[Aktion]** Stich mit dem **XV**.
> **[Anzeige]** H muss Karo bedienen und wirft den ♦Buben. Der Stich gehört dir: 5 + 1 + 2 = 8, minus 2 =
> **6 Punkte**.

**Schritt 11 – Das letzte gegnerische Tarock.**
> **[Aktion]** Spiel den **XIII**.
> **[Anzeige]** H gibt seinen letzten Tarock, den XII; M hat keinen mehr und wirft eine Farbkarte ab.
> **Jetzt sind alle Tarock der Gegner draußen.** Du hast noch **XI, IX, VII** und den **Pagat**.
> Ab hier ist der Pagat ultimo sicher: Spiel deine drei restlichen Tarock, dann deine Farbkarten, und heb
> den Pagat für den **allerletzten** Stich auf.

**Schritt 12 – Wenn es anders gelaufen wäre.**
> **[Anzeige]** Merk dir die Gegenprobe: Hättest du nur **acht** Tarock, hätten die Gegner **vierzehn**.
> Vier Runden nehmen ihnen acht ab, sechs blieben übrig – und deine Tarock wären alle. Dann darfst du den
> Pagat **nicht** ansagen. Und wenn eine angesagte Ultimo unterwegs kippt: **Gib den Pagat lieber früh ab.**
> Fünf Punkte verlieren ist besser als die ganze Prämie.

---

#### Übungspartie 3 – „Trischaken: Wer die meisten Punkte hat, verliert“

**Lernziel:** Passen üben, Stichzwang verstehen, die Pagat‑Sperre, Punkte vermeiden.

| Sitz | Tarock | ♥ | ♦ | ♠ | ♣ |
|---|---|---|---|---|---|
| **V** | XIV, X, VI, IIII, II | D, B, 3 | C, 4 | D, B, 9, 7 | C, 10 |
| M | XVI, XI, VIII, V, **I** | C, A, 2 | B, A, 3 | C, 10 | K, B, 7 |
| H | XIX, XV, XII, IX, VII, III | K, 4 | K, D, 2 | K, 8 | D, 9, 8 |
| **Talon** | Sküs, XXI, XX, XVIII, XVII, XIII | – | – | – | – |

**Schritt 1 – Warum alle passen.**
> **[Anzeige]** Fünf Tarock, das höchste ist der XIV, kein Trullstück, ein paar Damen. Damit gewinnst du
> gegen zwei Gegner keine 36 Punkte.
> **[Aktion]** Sag **„Weiter!“**.
> **[Anzeige]** M sagt „Weiter!“ – fünf Tarock, der Pagat und sonst wenig. H sagt „Weiter!“ – sechs Tarock
> und drei Könige, aber ohne Trullstück fällt sein Blatt leicht.
> **Alle drei haben gepasst.** Der Grund liegt im Talon: **Sküs, Mond, XX, XVIII, XVII und XIII schlafen dort**
> – die sechs stärksten Tarock sind gar nicht ausgeteilt worden. Niemand konnte ein Spiel ansagen.

**Schritt 2 – Trischaken.**
> **[Anzeige]** Wenn alle passen, wird **Trischaken** gespielt: **jeder gegen jeden**. Es gibt keinen
> Alleinspieler und keine Partner. **Wer am Ende die meisten Punkte in seinen Stichen hat, verliert** und
> zahlt an die beiden anderen.
> Drei Dinge sind jetzt anders:
> 1. **Der Talon bleibt liegen** und wird nicht gezählt. Es sind also nur **60 Punkte** im Spiel, nicht 70.
> 2. Es gilt **Stichzwang**: Du musst die höchste Karte im Stich übertreffen, wenn du kannst.
> 3. Der **Pagat** darf erst gespielt werden, wenn er das letzte Tarock im Blatt ist.
> Prämien gibt es keine, Kontra auch nicht.

**Schritt 3 – Ausspielen.**
> **[Anzeige]** Du bist Vorhand und spielst aus. Die Kunst ist, **klein** zu bleiben und die Stiche den
> anderen zu überlassen. Spiel eine niedrige Karte aus einer Farbe, von der du viele hast – dann musst du
> später selbst bedienen und kannst klein bleiben.
> **[Aktion]** Spiel die **♠7**.
> **[Anzeige]** Die ♠7 ist die **niedrigste** Pikkarte, die es gibt. Und jetzt sieh zu, was der Stichzwang
> anrichtet.

**Schritt 4 – Der Stichzwang schlägt zu.**
> **[Anzeige]** M hat ♠Cavall und ♠10. Er **muss** die ♠7 überbieten, nimmt aber die kleinere Möglichkeit:
> die **♠10**.
> H hat ♠König und ♠8. Die ♠8 ist **niedriger** als die ♠10 – damit kommt er nicht drüber. Also **muss** er
> den **♠König** legen. Fünf Punkte, gegen seinen Willen.
> Der Stich: 1 + 1 + 5 = 7, minus 2 = **5 Punkte** – und die gehören **H**.
> Genau darum geht es beim Trischaken: Man zwingt die anderen, die dicken Karten einzusammeln.

**Schritt 5 – Und jetzt trifft es dich.**
> **[Anzeige]** H spielt aus und will natürlich auch klein bleiben: er legt die **♦2**.
> Du hast ♦Cavall und ♦4. **Achtung, Farbreihenfolge:** In Rot heißt es K > D > C > B > **A > 2 > 3 > 4**.
> Die ♦4 ist also **niedriger** als die ♦2 – damit kommst du nicht drüber. Der ♦Cavall schon.
> **[Aktion]** Du musst den **♦Cavall** spielen.
> **[Anzeige]** Der Stichzwang lässt dir keine Wahl. M kann den Cavall nicht überbieten (er hat nur ♦Bube,
> ♦As und ♦3) und wirft sein ♦As ab. Der Stich: 1 + 3 + 1 = 5, minus 2 = **3 Punkte** – für **dich**.
> Stand: H 5, du 3, M 0.

**Schritt 6 – Die Pagat‑Sperre.**
> **[Anzeige]** M hat den **Pagat** und noch vier andere Tarock. Er **darf** ihn jetzt nicht spielen, auch
> wenn er wollte – erst wenn er sein letztes Tarock ist. Der Pagat ist 5 Punkte wert; für M ist das eine
> tickende Uhr. Merk dir das für das Gegenspiel: Wer den Pagat hat, ist im Trischaken im Nachteil.
> **[Aktion]** Ab hier spielst du selbst weiter. Ziel: **nicht** derjenige mit den meisten Punkten sein.

**Schritt 7 – Abrechnung erklären.**
> **[Anzeige]** Am Ende zahlt der Spieler mit den meisten Punkten **5 an jeden** der beiden anderen. Wer
> **gar keinen** Stich gemacht hat, bekommt vom Verlierer noch **5 extra** – das ist die *Jungfrau*.
> Und wer allein mehr als die Hälfte aller 70 Punkte eingesammelt hat, zahlt **doppelt**; der heißt dann
> *Bürgermeister*.

---

## 12. Quellenverzeichnis mit vollständigen URLs

**Tapp‑Tarock und Dreiertarock**

* [ATT1821] *Theoretisch‑praktische Anweisung zur gründlichen Erlernung des beliebten Tarok‑Tappen‑Spiels*,
  Wien und Prag 1821, Carl Haas – Volltext:
  https://www.tarock.info/Tapptarock_1821.htm
  (Original bei Google Books: https://books.google.com/books?id=amxiAAAAcAAJ)
* [PAG‑TAPP] https://www.pagat.com/tarot/tapptarock.html
  (die im Auftrag genannte URL https://www.pagat.com/tarot/tapp.html liefert **HTTP 404**)
* [WP‑DE‑TAPP] https://de.wikipedia.org/wiki/Tapp-Tarock
* [WP‑DE‑DREIER] https://de.wikipedia.org/wiki/Dreiertarock
* [WP‑EN‑TAPP] https://en.wikipedia.org/wiki/Tapp_Tarock
* [WP‑EN‑DREIER] https://en.wikipedia.org/wiki/Dreiertarock
* [KRUEPL] https://web.archive.org/web/20070514081801/http://www.webit.at/bk/tarock.html
* [FURR] https://web.archive.org/web/20100211171500/http://www.tarocchino.com/page29.html
* [ALSCHER] http://homer.members.pgv.at/tarock/tarockvarianten.htm
* [MS‑TOC] https://d-nb.info/1041884893/04 (Inhaltsverzeichnis von Mayr/Sedlaczek,
  *Die Strategie des Tarockspiels*, 5. Aufl., Edition Atelier, Wien 2016, ISBN 978‑3‑902498‑22‑9)

**Verwandte Dreierspiele**

* [WP‑DE‑ILLU] https://de.wikipedia.org/wiki/Illustriertes_Tarock
* [WP‑EN‑ILLU] https://en.wikipedia.org/wiki/Illustrated_Tarock
* [WP‑DE‑POINT] https://de.wikipedia.org/wiki/Point-Tarock
* [PAG‑SLO] https://www.pagat.com/tarot/sltarok.html
* [PAG‑DRLES] https://www.pagat.com/tarot/dreierles.html

**Abgrenzung, Herkunft der Negativspiele, Kaiserstich**

* [WP‑DE‑KR] https://de.wikipedia.org/wiki/K%C3%B6nigrufen
* [WP‑DE‑19] https://de.wikipedia.org/wiki/Neunzehnerrufen
* https://en.wikipedia.org/wiki/Neunzehnerrufen
* https://www.pagat.com/tarot/koenig.html
* https://www.pagat.com/tarot/taroky.html

**Gedruckte Werke, in den Onlinequellen als Grundlage genannt** (nicht selbst eingesehen – als solche
gekennzeichnet, wo sie zitiert werden)

* Wolfgang Mayr, Robert Sedlaczek: *Das große Tarock‑Buch*, Perlen‑Reihe 642, Zsolnay, Wien 2001,
  ISBN 3‑85223‑462‑X, S. 105–114 (Grundlage von [WP‑EN‑TAPP])
* Wolfgang Mayr, Robert Sedlaczek: *Die Strategie des Tarockspiels*, Edition Atelier, Wien 2008/2016,
  S. 354–359 (Grundlage von [WP‑EN‑DREIER])
* Fritz Beck: *Tarock komplett. Alle Spiele*, Perlen‑Reihe 640, Wien 1972, S. 69–93 und 135–143
  (Grundlage von [WP‑DE‑ILLU] und [WP‑DE‑POINT])
* Hans Löw: *20 Kartenspiele*, 2. Aufl., Pechan, Wien 1954, S. 31–36
* Franz Unger: *Kleines Lehrbuch des Tarockspiels*, Piatnik, Wien 1923, S. 8–9
  (Beleg für die verdeckte Unterer/Oberer‑Regel, zitiert nach [WP‑EN‑TAPP])
* Hans‑Joachim Alscher (Hrsg.): *„Tarock“ mein einziges Vergnügen*, Wien 2003, S. 169–170
* David Parlett: *The Penguin Book of Card Games*, Penguin, London 2008, S. 369
* Michael Dummett: *The Game of Tarot*, Duckworth, London 1980, S. 439–440, 455, 479–488
* Hugo Kastner, Gerald Kador Folkvord: *Die große Humboldt‑Enzyklopädie der Kartenspiele*, Humboldt,
  Baden‑Baden 2005, S. 246–248
* Jerry Neill Furr: *Tarocchi: An introduction to the many games played with tarot cards*, 2009, S. 133–138

---

## Anhang A – Zusammenfassung der Profil‑Flags

| Flag | Werte | Default |
|---|---|---|
| `schwelle_drittel` | 107 \| 106 | **107** |
| `vierter_spieler` | aussetzer \| koenig_1821 \| aus | **aussetzer** |
| `klopfen_braucht_vorhand` | bool | **false** |
| `klopfen_modus` | ein_paket \| paeckchenwahl | **ein_paket** |
| `gebefolge` | talon_zuerst_4er \| talon_zuerst_8er \| talon_zuerst_4x4_pro_spieler \| talon_mitte_8er | **talon_zuerst_4er** (1821: `talon_zuerst_4x4_pro_spieler`) |
| `fehlgeben_strafe` | keine \| halber_grundwert | **keine** |
| `neugeben_ohne_tarock` | bool | **true** (1821: `false`) |
| `neugeben_schwache_tarock` | bool | **false** |
| `tapper` | aus \| sofort_ausbezahlt \| ausgespielt | **aus** |
| `tapper_nur_vorhand` | bool | **true** (nur in `AT-TAPP-1821` wirksam) |
| `halten` | seniorität \| nur_vorhand \| aus | **seniorität** |
| `sprung_erlaubt` | nur_solo \| immer | **nur_solo** |
| `solo_beendet_lizit` | bool | **true** (1821: `false`) |
| `solo_halten_nur_nach_dreyer` | bool | **true** (nur wirksam, wenn `solo_beendet_lizit = false`) |
| `alle_passen` | trischaken \| neu_geben \| rauben | **trischaken** |
| `rauben` | aus \| an | **aus** |
| `trischaken_talon` | aus \| zuwaage \| letzter_stich | **aus** |
| `trischaken_modus` | meister_zahlt_allen \| zwei_schlechtere_zahlen \| bester_kassiert \| durchschnitt \| nur_stichzahl | **meister_zahlt_allen** |
| `trischaken_buergermeister` / `_jungfrau` / `_vorhand_strafe` | bool | **an / an / aus** |
| `trischaken_von_oben` / `_tarockpflicht` / `_ohne_stichzwang` / `_tarock_invertiert` | bool | **alle aus** |
| `trischaken_pfeife_dritter_tarockstich` / `_trull_zwangsfolge` / `_ohne_sperre` | bool (schließen einander aus) | **alle aus** |
| `trischaken_gleichstand` | jeder_zahlt_voll \| zahlung_geteilt | **jeder_zahlt_voll** |
| `talon_lizit_semantik` | haelfte_fest \| freie_wahl \| blattzahl | **haelfte_fest** (POINT: `freie_wahl`) |
| `talon_aufdecken` | immer \| nur_bei_dreier | **immer** (1821: nie, siehe `talon_1821`) |
| `talon_1821` | oben_verdeckt | nur in `AT-TAPP-1821` |
| `restliche_talonhaelfte` | verdeckt \| gegner_sehen_sie | **verdeckt** |
| `legage_tarock` | notfall_offen \| frei_offen \| verboten | **notfall_offen** |
| `erzwungenes_tarock` | offen_zeigen \| nur_anzahl_ansagen | **offen_zeigen** |
| `legage_notfall` | trull_erlaubt_offen \| koenig_erlaubt_offen | **trull_erlaubt_offen** |
| `ansage_vor_kauf` | aus \| an | **aus** (ILLU/POINT: an) |
| `ultimo_ansage_durch` | spieler \| alle | **spieler** |
| `ultimo_pagat_offen` | bool | **true** |
| `gegner_ansagen` | aus \| pagat_uhu | **aus** |
| `handpraemien` | an \| aus | **an** |
| `handpraemien_meldung` | nach_spiel \| vor_spiel_moeglich | **nach_spiel** |
| `handpraemie_stichtag` | nach_talon \| wie_ausgeteilt | **nach_talon** |
| `trull_bedingung` | hand \| stiche | **hand** |
| `valat_modus` | multiplikator \| fest | **multiplikator** |
| `valat_illu` | fest_12_24_48 \| beck_multiplikator \| graz_6_12 | **fest_12_24_48** |
| `mondfang` | aus \| grazer_5 \| individuell_slo | **aus** |
| `punkteansagen` | aus \| illu \| point | **aus** |
| `kontra_gilt_fuer_partner` | bool | **true** |
| `kontra_nach_pass_verboten` | bool | **false** |
| `kontra_max_stufe` | 0–6 | **3** (1821: **0**) |
| `kontra_umfang` | spiel_und_praemien_getrennt \| nur_spiel | **spiel_und_praemien_getrennt** |
| `kontra_vor_kauf` | aus \| verdoppelt \| vervierfacht | **aus** (ILLU: `verdoppelt`) |
| `subkontra_durch` | jeder_gegner \| nur_kontrierender | **jeder_gegner** |
| `erstes_ausspiel` | vorhand \| spieler | **vorhand** |
| `stichansicht` | eigene \| eigene_plus_letzter | **eigene_plus_letzter** |
| `kaiserstich` | aus \| an \| an_ohne_negativ | **aus** |
| `kaiserstich_reihenfolge` | egal \| mond_skues_pagat | **egal** |
| `vogel_frei` | bool | **false** |
| `farbenspiele` | aus | **aus** (fix) |
| `negativspiele` | aus \| piccolo_bettler \| mit_ouvert | **aus** |
| `ueberschusspunkte` | aus \| einfach \| doppelt_1821 | **aus** |
| `abrechnung` | nullsumme_je_gegner \| schreiben_je_spieler | **nullsumme_je_gegner** |
| `renonce` | ui_verhindert \| turnier | **ui_verhindert** |
| `schlussrunde` | aus \| trischaken_oder_solo | **aus** |
| `hoefliches_ende` | aus \| drei_touren | **aus** |
| `neu_auslosen_je_serie` | bool | **false** |
| `timeout_sekunden` | int | **90** |

Zusätzlich profilspezifisch (nur `AT-TAPP-42`, aus [ATT1821 §E]): `gesamtpunkte_drittel = 198`,
`schwelle_drittel = 102`, Punktemultiplikatoren `2 / 3 / 4 / 6` für Dreyer / Zweyer / Einer / Solo.

## Anhang B – Offene Punkte für die weitere Recherche

1. **Mayr/Sedlaczek im Original.** [WP‑EN‑TAPP] und [WP‑EN‑DREIER] referieren *Das große Tarock‑Buch* (2001)
   und *Die Strategie des Tarockspiels* (2008/2016). Beide Bücher wurden **nicht eingesehen**. Sie würden
   §4.2 (Talonsemantik), §7.2 (Überschusspunkte) und §5.5 (Valat) endgültig klären.
2. **Alscher 2003, S. 169–170** enthält laut [WP‑EN‑TAPP] die Unterer/Oberer‑Variante ausdrücklich als
   Verschärfungsmöglichkeit – der Originaltext wäre für §4.2 die stärkste Quelle.
3. **Unger 1923 (Piatnik), S. 8–9** ist der älteste Beleg für den Übergang von der verdeckten zur offenen
   Talonpraxis („in some circles“) und sollte für §9.5 geprüft werden.
4. **Kärntner Dreiertarock in der Praxis.** Für `AT-DREIER-54` fehlt eine österreichische Primärquelle
   (Verein, Turnierregelwerk); die Tarife stammen ausschließlich aus [WP‑EN‑DREIER]. Auch der Name „Kike“
   für Tarock III ist nur dort belegt.
5. **Bettler/Piccolo zu dritt.** Es wäre wertvoll, einen realen österreichischen Dreierkreis zu finden, der
   Negativspiele spielt, um `AT-TAPP-NEGATIV` auf eine Quelle zu stellen statt auf unsere Tarifschätzung.

---

## Anhang C – Prüfprotokoll (unabhängige Regelprüfung, 12.09.2026)

Diese Fassung wurde von einem zweiten, skeptisch eingestellten Prüfer Regel für Regel gegen die in §0.2
und §12 genannten Quellen geprüft. **Alle Quellen wurden erneut abgerufen und im Volltext gelesen**, nicht
aus dem Gedächtnis zitiert:

| Quelle | Prüfform | Befund zur Erreichbarkeit |
|---|---|---|
| [ATT1821] | Volltext‑Transkription `tarock.info/Tapptarock_1821.htm` (HTTP 200), §§ 1–53 und Teil E §§ 1–7 gelesen | erreichbar, vollständig |
| [WP‑EN‑TAPP] | Wikitext‑Rohabruf (`action=raw`), Tariftabelle Zelle für Zelle verglichen | erreichbar |
| [WP‑DE‑TAPP] | Wikitext‑Rohabruf | erreichbar |
| [ALSCHER] | `homer.members.pgv.at/tarock/tarockvarianten.htm` (HTTP 200), Abschnitte *Tapptarock* und *Einfaches Tarock* | erreichbar |
| [KRUEPL] | Archivkopie 14.05.2007 (HTTP 200), Abschnitte *General rules*, *How to count*, *Tappen*, *Glossary* | erreichbar |
| [FURR] | Archivkopie 11.02.2010 (HTTP 200), vollständig | erreichbar |
| [PAG‑TAPP] | `pagat.com/tarot/tapptarock.html` → **HTTP 200**; `pagat.com/tarot/tapp.html` → **HTTP 404** | §9.2 bestätigt |
| [WP‑EN‑DREIER], [WP‑DE‑ILLU], [WP‑EN‑ILLU], [WP‑DE‑POINT], [WP‑DE‑KR] | Wikitext‑Rohabrufe | erreichbar |
| [PAG‑SLO], [PAG‑DRLES] | HTTP 200, vollständig | erreichbar |
| Mayr/Sedlaczek, Beck, Löw, Unger, Dummett, Parlett, Alscher 2003 | **nicht eingesehen** (Druckwerke) | unverändert offen, Anhang B |

### C.1 Berichtigte Fehler

| # | Stelle | Was falsch war | Was jetzt dasteht |
|---|---|---|---|
| 1 | §1.8 | „Vereinfachte Zählung … alles andere 0; Gewinnschwelle **45 von 88**“ | Bei [KRUEPL] zählen die übrigen Tarock **1**, nur die Skartindeln 0; die Summe des 54er‑Blattes ist **90**, die Schwelle **45 von 90**. Die 88 ist die Summe des **40er**‑Blattes von *Einfaches Tarock*/*Zwanzigerrufen* [ALSCHER] und gehört nicht hierher. Neue Zeile §9.35 |
| 2 | §2.4 | [ATT1821 §12] als Beleg für `talon_zuerst_4er` („4× je 4 reihum“) | 1821 gibt **jedem Spieler nacheinander 2×4 Karten**, und das zweimal. Neuer Wert `talon_zuerst_4x4_pro_spieler`, Default für `AT-TAPP-1821`. [KRUEPL] und [PAG‑DRLES] als Belege für `talon_zuerst_8er` ergänzt |
| 3 | §4.3 | [PAG‑SLO] als Beleg für „Tarock nur im Notfall ablegen“ | Im slowenischen Tarok dürfen Tarock **frei** abgelegt werden (nur offen). Zitat eingesetzt, Beleg gestrichen; neues Flag `legage_tarock` mit den drei belegten Positionen (Notfall/frei/verboten) |
| 4 | §5.3 | [ATT1821 §2] als Beleg für die Prämie **Köpfe** | 1821 kennt nur *tous les trois* und *vier Könige*, keine Zweierkombination. Köpfe stehen allein bei [ALSCHER] und [WP‑EN‑TAPP]. Zusätzlich vermerkt: [KRUEPL] und [FURR] führen für Tappen **gar keine** Handprämien. Neue Zeile §9.31, neues Flag `handpraemien` |
| 5 | §5.9 | ILLU‑Werte für Tarocktrull / Königstrull / Köpfe / ohne Trull / ohne Könige als **4/8/8, 5/10/10, 2/4/4, 4/8/8, 5/10/10** | Richtig sind **4/4/8, 5/5/10, 2/2/4, 4/4/8, 5/5/10**. [WP‑DE‑ILLU §Prämien Allgemeines] verdoppelt Kombinationsprämien erst bei Ansage **vor** dem Kauf; bestätigt durch drei eigene Rechenbeispiele der Quelle (Köpfe nach Kauf = 2, vor Kauf = 4) |
| 6 | §7.6 | Gleichstandsregel im Trischaken „analog [PAG‑DRLES §Räuber]“ | Dreierles regelt es **gegenteilig** (die Verlierer teilen sich die Zahlung, der Dritte bekommt unverändert seinen Betrag). Die im Dokument beschriebene Regel stammt aus [WP‑DE‑KR §Abrechnung beim Trischaken] („Alle Verlierer zahlen allen anderen Spielern je 10 Cent“). Beleg korrigiert, Dreierles‑Variante als Flag `trischaken_gleichstand` aufgenommen |
| 7 | §9.20 | „Zuwaage zum Trischaken … für drei Spieler **nicht belegt**“ | Doch: [PAG‑SLO §Klop] beschreibt die Talonkarte zu jedem der ersten sechs Stiche, und der Abschnitt *The three player game* erklärt die Spielregeln des Viererspiels für anwendbar. Zeile umgeschrieben; Empfehlung (Talon bleibt draußen) bleibt, jetzt mit zwei statt einer Dreier‑Quelle ([WP‑DE‑ILLU] **und** [PAG‑DRLES §Räuber]) |
| 8 | §9.18, §3.5.2 | Der badische *Räuber* wurde als Parallele zum **Rauben** geführt | Der Dreierles‑*Räuber* ist ein **Trischaken** („the blind is set aside and not used or counted“), kein Kartenmitnehmen. Die echte Parallele zum Rauben ist die Grazer Variante (G) in [WP‑DE‑ILLU], die jetzt wörtlich zitiert wird |
| 9 | §3.7 | Querverweis „Serie: bis 100 Spielpunkte (§8.3)“ | richtig ist **§8.2** |
| 10 | §9.6 | Quellenlage zum Halterecht als 1 : 3 dargestellt | Tatsächlich 3 : 5. [KRUEPL] gehört auf die Seite „nur Vorhand“ („**the forehand** has a forth possibility … ‚Ich halte‘“), [ATT1821 §19] ebenfalls; [FURR] und [PAG‑SLO] auf die Senioritätsseite. Default unverändert, Begründung berichtigt |
| 11 | §9.5 | [KRUEPL] fehlte bei der Frage, ob der Talon aufgedeckt wird | [KRUEPL §Playing]: „The talon is exposed (if it's not a ‚Solo‘)“ → Seite „ja“. [ATT1821 §20] → Seite „nein“ (der Dreyer kauft die drei obersten Blätter ungesehen) |

### C.2 Gefüllte Lücken

| # | Stelle | Ergänzung |
|---|---|---|
| 12 | §1.4 | Warnkasten zum **Rechenfehler in [WP‑DE‑TAPP §Zählen]** („Pagat + König + Cavall = 12⅓“; richtig 11) samt den sechs korrekten Beispielen aus [ATT1821 §4] als Unit‑Test‑Vorlage |
| 13 | §2.6 | [ATT1821 §47] **widerspricht** der Neugabe bei tarockfreiem Blatt ausdrücklich; [PAG‑SLO] koppelt sie an ein Pflicht‑Klop. Default bleibt, Profil `AT-TAPP-1821` bekommt `neugeben_ohne_tarock = false`. Ebenfalls ergänzt: die von 1821 gerügte Hausregel „Zehn Taroks als Honneur“ [ATT1821 §46] – im badischen Dreierles dagegen Standard (*Zehn Druck*) |
| 14 | §3.2 / §9.34 | **Binnenwiderspruch der ältesten Quelle**: §E.3 („sobald das Wort Solo ertönt, hört jedes weitere Steigern auf“) gegen §19 (eine Vorhand, die einen *Dreyer* angesagt hat, darf einen fremden Solo selbst spielen). Neues Flag `solo_halten_nur_nach_dreyer` |
| 15 | §3.10 | *Tapper* darf 1821 **nur die Vorhand** ansagen; die regionale Zusatzbedingung „wenigstens eine Honneur“ wird von der Quelle gerügt; **1821 kennt kein Kontra** (`kontra_max_stufe = 0` im Profil); der Dreyer‑Kauf erfolgt **ohne Aufdecken** |
| 16 | §3.11 | Der 42er‑Teil war unvollständig. Ergänzt aus [ATT1821 §E.5–E.7]: **Solo‑Consolation 150**, Präference‑Volat **300**, Solo‑Volat **600**; Punktemultiplikatoren **Dreyer ×2, Zweyer ×3, Einer ×4, Solo ×6**; **Kartensumme 66**, **Remis bei 33**, **34 = um eins gewonnen** (`gesamtpunkte_drittel = 198`, `schwelle_drittel = 102`); Gebeverfahren (zwei Runden zu je 2×3 Karten) |
| 17 | §4.2 | [WP‑DE‑POINT §Reizen Fn.] als zusätzlicher Beleg für die historische Talonhälften‑Bedeutung von *Unterer/Oberer*; **Warnkasten zur Namensgleichheit**: im Illustrierten Tarock ist der *Unterer* das „Lösen“ eines fremden Gebots und hat mit dem Talon nichts zu tun |
| 18 | §5.2 | [ATT1821 §48] als Beleg dafür, dass Pagat ultimo und Handprämien **unabhängig vom Spielausgang** zählen, samt durchgerechnetem Quellenbeispiel (verlorener Dreyer um 5 + Trull + stiller Ultimo = „noch 40“); [ATT1821 §7] als Beleg, dass eine verlorene Ansage **den angesagten** Betrag kostet; ausdrücklicher Hinweis auf die abweichende Zahl **24** in [WP‑DE‑TAPP] |
| 19 | §5.3 | **Meldezeitpunkt** der Handprämien: erst nach der Taille, nicht vorher – [ATT1821 §49] nennt das Vorab‑Ansagen „falsch als abgeschmackt“; [PAG‑DRLES] „nachgemeldet“. **Meldefrist** bis zur nächsten Kartengabe [ATT1821 §50]. Gegenposition „opening hand“ [WP‑EN‑TAPP] als Flag `handpraemie_stichtag`. Fortbestand der Prämie trotz später verlorener Karten [ATT1821 §48] |
| 20 | §5.8 | [FURR] als Beleg für getrennte Kontras auf Spiel und einzelne Ansagen und für das Subkontra durch „any other player“; [PAG‑DRLES] als Gegenposition (Kontra verdoppelt **nur** den Spielwert) und als zweiter Beleg dafür, dass ein Kontra für beide Gegner gilt; **Widerspruch [WP‑DE‑ILLU] ↔ [WP‑EN‑ILLU]** beim Kontra vor dem Talonkauf (verdoppelt vs. vervierfacht), neue Zeile §9.32 |
| 21 | §5.9 | Die drei belegten Valat‑Abrechnungen des ILLU‑Profils (Beck a: 12/24/48 fest · Beck b: dieselben Zahlen als Multiplikator, von Beck selbst nicht empfohlen · Graz: ×6/×12) als Flag `valat_illu` |
| 22 | §6.8 | Die drei badischen Trullstück‑Sperren für das Trischaken (Steinmauern, Iffezheim, Achertal/Ramsch) aus [PAG‑DRLES] als zusätzliche, einander ausschließende Optionen |
| 23 | §8.1 | [ATT1821 §Erklärung] definiert *Taille* und *Tour* (drei Taillen) wörtlich so, wie das Dokument „Spiel“ und „Runde“ verwendet; **§51** liefert ein historisches Ausstiegsverfahren („auf die letzten drey Touren einladen“), neues Flag `hoefliches_ende` |
| 24 | §11.6 | Glossar um 14 belegte Fachbegriffe aus [ATT1821 §Erklärung] erweitert: *Anlaufen lassen, Beißer, Bekennen, Bey Hofe speisen, Cavallerie, Consolation, Force, Für die Küche sorgen, Heim gehen, Reitzkarte, Natur‑Scat/Scatinel, Schinden, Schlafen, Schmieren* |
| 25 | §9.33 | Neu: [WP‑DE‑ILLU] widerspricht sich selbst beim Wert des **gehaltenen** Spiels (Tabelle 5, Rechenbeispiele 4). Die Beispiele in §7.5 sind wörtliche Quellenzitate und deshalb inkonsistent; im Code gilt die Tabelle |

### C.3 Nachgerechnet und für richtig befunden (keine Änderung)

* **Zählverfahren §1.5/§1.6.** `3F − 2n` liefert für das ganze Blatt 210 Drittel = 70 Punkte; das
  durchgerechnete Beispiel (F = 57, n = 27 → 39) stimmt, ebenso die Gegenprobe 70 − 39 = 31.
* **Alle sechs Zählbeispiele von [ATT1821 §4]** erfüllen die Formel exakt.
* **Tarifzahlen `AT-TAPP-KLASSIK`** (Dreier 3 / Unterer 4 / Oberer 5 / Solo 8; Pagat ultimo 4 / 8, im Solo
  8 / 16; Trull 3, Königstrull 3, Köpfe 1; Valat ×4 / ×8) stimmen mit [ALSCHER] und [WP‑EN‑TAPP] überein.
  [KRUEPL] und [FURR] bestätigen Spiel‑ und Valat‑Werte; §9.16 (vertauschte Ultimo‑Beschriftung bei [FURR])
  ist am Original verifiziert – der Fließtext derselben Seite sagt, Ansagen brächten „extra points“.
* **`AT-DREIER-54` §3.6**: Gebefolge, Lizit, Talonbehandlung, „kein Stichzwang“, Gewinn 36/70 und die
  komplette Tarif‑ und Prämientabelle (30/40/50/60/70/120/960; Trull 10, Königstrull 10, Mondfang 10,
  Pagat 40/80, Uhu 60/120, Kike 80/160) stimmen Zelle für Zelle mit [WP‑EN‑DREIER].
* **`AT-POINT` §3.8**: Stufen 3/4/5, Ansagen 3/6/10/15, kumulative Wirkung, Teilkontra („Kontra die 50!“)
  und das Rechenbeispiel (mit 50 angesagt, 46 erreicht → 10 − 9 = 1 an jeden Gegner) stimmen mit
  [WP‑DE‑POINT]; [FURR §Illustrated Tapp] rechnet dasselbe mit 47 Points identisch.
* **ILLU‑Rechenbeispiele §7.5** sind wörtlich und richtig aus [WP‑DE‑ILLU §Beispiel für einen Spielverlauf]
  übernommen (18 · 22 · 84 + 6 · 6/8).
* **Trischaken‑Grundwerte** 5 / −10 / +5 (Jungfrau) stimmen mit [WP‑DE‑ILLU §Punkte für das Spiel] und
  [WP‑EN‑ILLU §Scoring].
* **Kaiserstich §6.5, Vogel frei, Negativspiel‑Regeln, Mondfang‑Strafe 20/21** wörtlich gegen [PAG‑SLO]
  geprüft; die Einordnung „für Tapp‑Tarock nirgends belegt“ hält stand.
* **Die drei Übungspartien §11.7 wurden Karte für Karte nachgezählt.** Jede der drei Verteilungen enthält
  genau 3 × 16 + 6 = 54 Karten, jede der 22 Tarock und jede der 32 Farbkarten **genau einmal**, keine
  doppelt, keine fehlend. Auch die im Text genannten Stichwerte stimmen: Partie 1 (5, 5, 6, 6, Legage 2 →
  24), Partie 2 (5, 5, 1, 1 → 12; Karostich 6), Partie 3 (♠‑Stich 5, ♦‑Stich 3). Der Satz „nur **60**
  Punkte im Spiel“ in Partie 3 ist korrekt: der Talon (Sküs, XXI, XX, XVIII, XVII, XIII) ist nach
  `3·14 − 2·6 = 30` Drittel genau **10** Punkte wert, 70 − 10 = 60.
* **Sitzordnung.** „Der Spieler zu seiner Linken (Mittelhand) hebt ab“ ist bei Spiel gegen den
  Uhrzeigersinn korrekt: Vorhand sitzt rechts vom Geber, Mittelhand folglich links von ihm. Bestätigt durch
  [ATT1821 §10, §11] („zur Linken hat abheben lassen“ / Vorhand = „der Spieler zur Rechten“),
  [WP‑EN‑TAPP], [WP‑DE‑ILLU], [PAG‑SLO], [PAG‑DRLES].
* **§9.2** (pagat.com‑URL), **§9.15** (Solo‑Ultimo 24 vs. 16), **§9.16** ([FURR]‑Vertauschung),
  **§9.19** (ILLU‑Wertdifferenzen Panzer / ohne Trull / ohne Könige) am Original bestätigt.

### C.4 Was unsicher bleibt

1. **Die Talonsemantik von *Unterer* und *Oberer* (§4.2) ist und bleibt der schwächste Punkt.** Die
   Mehrheit der Belege für Lesart A (Unger 1923, Piatnik‑Beiblatt, Parlett 2008, Alscher 2003) ist uns nur
   **über die Fußnote von [WP‑EN‑TAPP] zugänglich**, nicht im Original. Direkt gelesen haben wir nur
   [KRUEPL] und [FURR]. Der Haupttext von [WP‑EN‑TAPP] (= Mayr/Sedlaczek 2001) sagt ausdrücklich:
   „Today there is **no difference in the procedure** – the declarer may choose either packet – however,
   the game value is different in each case.“ Der Default `haelfte_fest` ist damit eine **bewusste
   Entscheidung für die historische Lesart**, nicht die heute verbreitetste Praxis. Das Flag muss in der
   Profilauswahl sichtbar und mit einem Satz erklärt sein.
2. **Valat: Multiplikator oder Festwert (§5.5, §9.13).** Es steht 3 : 2 ([ALSCHER], [KRUEPL], [FURR] gegen
   [WP‑DE‑TAPP], [WP‑EN‑TAPP]). Beide Seiten sind in sich schlüssig; die Entscheidung für den Multiplikator
   bleibt eine Wertung. Anmerkung: Die englische Tabelle führt „Valat 12“ und „Valat announced 24“ in der
   **Kontrakt**spalte, nicht bei den Prämien – das spricht dafür, dass Mayr/Sedlaczek den Valat als eigenes
   Spiel und nicht als Zuschlag auffassen. Klärbar nur am Buch.
3. **Handprämien im Tapp‑Tarock (§9.31).** Dass [KRUEPL], [FURR] und [WP‑DE‑TAPP] Trull, Königstrull und
   Köpfe **überhaupt nicht** nennen, ist kein Zufallsbefund: möglicherweise gehören sie nicht zum
   Kernbestand des Spiels, sondern kamen aus dem Königrufen dazu. 1821 kennt Trull und Könige, aber nicht
   die Köpfe. Die App behält sie (Flag `handpraemien`), die Quellenbasis ist jedoch schmäler, als früher
   dargestellt.
4. **Die Solo‑Verdopplung des Pagat ultimo** (8/16) steht nur bei [ALSCHER] und [WP‑EN‑TAPP]; [KRUEPL] und
   [FURR] kennen sie nicht, [WP‑DE‑TAPP] nennt eine dritte, arithmetisch unerklärliche Zahl (24).
5. **Die genaue Staffelung der ILLU‑Prämien** (§5.9) hängt vollständig an der Wiedergabe Becks durch zwei
   Wikipedia‑Artikel, die einander an mehreren Stellen widersprechen (Panzer, ohne Trull, ohne Könige,
   Kontra vor Kauf, Wert des gehaltenen Spiels). Ohne Beck 1972/1983 im Original bleibt das Profil
   `AT-TAPP-ILLU` die am schwächsten abgesicherte Komponente dieses Dokuments. Die Korrektur in C.1 Nr. 5
   ist allerdings **hart belegt**, weil die deutsche Quelle ihre eigenen Werte in drei Beispielen
   nachrechnet.
6. **`AT-TAPP-NEGATIV` (§3.9)** bleibt unverändert **nicht quellenbelegt**; die Warnmarkierung ist
   berechtigt und wurde nicht abgeschwächt. Geprüft und bestätigt wurde nur die Einordnung im slowenischen
   Dreierspiel (berač 70, solo brez talona 80, odprti berač 90 laut *Variations*).
7. **Kartenwertetabelle mit 42 Blatt.** [ATT1821 §E.6] gibt Consolation und Prämienwerte ausdrücklich zur
   freien Vereinbarung frei; die in §3.11 genannten Zahlen sind die **Vorschlagswerte des Verfassers von
   1821**, keine feststehenden Tarife.
8. **Nicht eingesehen und weiterhin offen** (unverändert aus Anhang B): Mayr/Sedlaczek 2001 und 2008/2016,
   Beck 1972/1983, Löw 1954, Unger 1923, Alscher 2003, Dummett 1980, Parlett 2008. Sie würden §4.2, §5.5,
   §7.2 und die gesamte ILLU‑Staffelung endgültig klären.
