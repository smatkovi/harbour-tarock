# Königrufen – Regelspezifikation für harbour-tarock

**Regelprofil:** „OÖ 4/2023“ – *Oberösterreichische Tarockregeln – Königrufen, Version 4/2023*, ergänzt um die *OÖ-Renonceregeln 11/2025* und die *OÖ-Taroquette 4/2026*
**Profil-ID (Vorschlag):** `AT-KR-OOE-2023-04`
**Zielplattformen:** Sailfish OS (Qt 5.6/QtQuick 2.6/Silica) und Android (Qt 6.11), C++17-Regelkern, Computerspieler, LAN
**Dokumentstand:** 11.09.2026, quellengeprüft und korrigiert am 12.09.2026 (siehe Abschnitt 12 „Prüfprotokoll“) · Sprache Deutsch, Regelbegriffe im Original

Dieses Dokument ist die verbindliche Vorlage für Regel-Engine, Wertung, Computerspieler, LAN-Server und Lernmodus. Es ist so geschrieben, dass jede Regel ohne Rückgriff auf andere Texte implementiert werden kann. Wo die Quellen schweigen oder sich widersprechen, ist das markiert und eine App-Festlegung angegeben. Ergänzt wurde nichts, was nicht belegt ist; die Belegstelle steht jeweils in eckigen Klammern.

**Inhalt**

0. Grundlagen dieses Dokuments (Quellen, Kennzeichnungen, Notation)
1. Kartensatz, Rangfolgen, Kartenwerte, Zählen
2. Tisch, Geben, Talon, Vierer- und Fünfertisch
3. Lizitation
4. Partner, Talon und Verlegen
5. Ansagen (Prämien) und Kontra
6. Spielregeln im Stich
7. Abrechnung
8. Partiestruktur (Turnier- und App-Modi)
9. Sonderfälle, offene Punkte und regionale Abweichungen
10. Hinweise für den Computerspieler
11. Inhalte für den Lernmodus (einschließlich drei Übungsspielen)
12. Prüfprotokoll (Quellenprüfung 12.09.2026)
    · Anhang A: Quellen · Anhang B: Offene Punkte auf einen Blick

---

## 0. Grundlagen dieses Dokuments

### 0.1 Regelprofil, Aktualität und Rangfolge der Quellen

1. **Primärquelle [OÖ23]:** *Version 4/2023 Oberösterreichische Tarockregeln – Königrufen* (2 Seiten, Autor Franz Kienast). Seit April 2023 spielen alle oberösterreichischen Cups danach: Mühlviertler Cup (= Raiffeisen Tarockcup Austria, Gasthof Haudum), Hausruckviertler Tarockcup und Sauwald Tarockcup [HRC]. Ob die Fassung noch aktuell ist, wurde am 11.09.2026 geprüft und am 12.09.2026 nachgeprüft:
   - Das PDF auf haudum.at ist unverändert; die PDF-Metadaten nennen `/Author KIENAST Franz`, `/CreationDate D:20230913`. Inhaltsgleich sind die HRC-Fassung und die Fassung der Linzer Tarock Akademie.
   - Die HRC-Seite nennt weiterhin 4/2023 als gültige Fassung. Hinzugekommen sind nur die *OÖ-Renonceregeln 11/2025* [OÖR25] und die *OÖ-Taroquette 4/2026* [OÖT26]; beide sind dort ausdrücklich mit diesen Versionsangaben verlinkt.
   - Der Cup-Regelvergleich von Markus Mair, Stand März 2026 [MAIR-CUPS], führt die OÖ-Tarife unverändert.
   - **Fazit:** 4/2023 ist die neueste Spielregelfassung.
   - Sämtliche in diesem Dokument als `[OÖ23 …]` zitierten Sätze wurden am 12.09.2026 gegen den Volltext des PDF geprüft (Abschnitt 12).
2. **OÖ-Folgedokumente:** [OÖR25] (verbindliche Auslegung der Renonce, mit Rechenbeispielen, die auch die Grundwertung bestätigen) und [OÖT26] (Etikette, u. a. Talon für den Fünften tabu).
3. **OÖ-nahe Erläuterungen,** wo [OÖ23] knapp ist:
   - Skripten der Linzer Tarock Akademie [LTA-GK1], [LTA-GK2], [LTA-F1], [LTA-S]. Sie beruhen ausdrücklich „auf den oberösterreichischen Tarockregeln – Königrufen“, und die Akademie ist Mitautorin von [OÖR25].
   - Turnierregeln des Österreich-Finales 2021 [ÖF21] und 2010 [ÖF10]: Vorgängertexte desselben Autors (Franz Kienast), weitgehend wortgleich mit [OÖ23]. Wo [OÖ23] davon abweicht, gilt [OÖ23].
4. **Allgemeine Beschreibungen zum Gegenprüfen:** [PAGAT] (pagat.com, Stand 1.9.2026) und [WIKI] (deutschsprachige Wikipedia, „Königrufen“, abgerufen 11.09.2026).
5. **Nur zum Vergleich** (andere Regelprofile): [MAIR-*] (Tirol), [KTC] (Kärnten), [STMK] (Steiermark), [PEPI] (private OÖ-nahe Runde).

**Füllregel für Lücken:** Zuerst 2 und 3; wenn beide schweigen, 4 unter Hinweis auf 5. Jede so gefüllte Stelle ist mit **[OFFEN]** oder **[EMPFEHLUNG]** markiert.

### 0.2 Kennzeichnungen

| Marke | Bedeutung |
|---|---|
| `[OÖ23 Allg. 3]` | Quelle und Fundstelle. Kapitel in [OÖ23]: *Allg.* (Allgemeines 1–9), *Vorhand* (1–3), *Trischaken* (1–4), *Negativ* (Negativspiele 1–3), *Farben* (Farbenspiele 1–4), *Ansagen* (Ansagen/Prämien 1–7), *Renonce* (1–8), *Tarif* (Kopftabellen „Spiele und Punkte“, „Ansagen/Prämien und Punkte“). |
| **[OFFEN]** | Die Quellen regeln den Punkt nicht eindeutig. |
| **[ABWEICHUNG]** | Die Quellen widersprechen sich ausdrücklich. |
| **[EMPFEHLUNG]** | Festlegung für die App. Soll als Profiloption abschaltbar sein, wenn sie von einer Quelle abweicht. |

### 0.3 Notation

- **Sitzplätze:** Gespielt, gegeben und gesprochen wird **gegen den Uhrzeigersinn** [PAGAT; LTA-GK1 Kap. 5]. In der Engine sind die aktiven Plätze in Spielreihenfolge indiziert. „Nächster Spieler“ heißt immer: rechts vom aktuellen.
- **Karten-IDs:**
  - Tarock `I, II, III, IIII, V … XX, XXI, Sk`. Tarock 4 heißt in allen OÖ-Texten „IIII“, nicht „IV“.
  - Farbkarten: Farbsymbol + Rang, z. B. `♥K`, `♦1`, `♠10`, `♣7`.
  - Figuren: **K** König, **D** Dame, **R** Reiter (Cavall), **B** Bube.
- **Punkte:** „P/B“ bedeutet *P Punkte und B Blatt* (B ∈ {0, 1, 2}, ein Blatt = ⅓ Punkt). Beispiel: 35/2 = 35⅔. Die Engine rechnet in **Dritteln**, ganze Partie = 210 Drittel.
- **„Spiel“** hat drei Bedeutungen [WIKI]: das Kartenspiel selbst, eine Spielansage (z. B. „Dreier“) und eine Runde vom Mischen bis zur Abrechnung. Im Code heißen sie `Contract` (Spielansage) und `Hand` (einzelnes Spiel). Mehrere Hands bilden ein **Radl** (siehe 2.6).

---

## 1. Kartensatz, Rangfolgen, Kartenwerte, Zählen

### 1.1 Kartensatz

54 Karten im Bild „Industrie und Glück“: **22 Tarock** und **32 Farbkarten**, je 8 in **♥ Herz, ♦ Karo** (rot) und **♠ Pik, ♣ Treff/Kreuz** (schwarz) [WIKI; LTA-GK1 Kap. 2]. Alle vier Farben sind gleichrangig; keine Farbe sticht eine andere.

### 1.2 Rangfolge in den Farben

**Rote und schwarze Farben sind verschieden geordnet** [LTA-GK1 Kap. 2; PAGAT; WIKI]:

| Stichkraft (hoch → niedrig) | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
|---|---|---|---|---|---|---|---|---|
| **rot** ♥ ♦ | K | D | R | B | **1** (As) | **2** | **3** | **4** |
| **schwarz** ♠ ♣ | K | D | R | B | **10** | **9** | **8** | **7** |

- Die vier kleinen Karten jeder Farbe heißen *Skartins*, *Skartindeln*, *Blätter* oder *Glatzen*.
- In Rot sticht das **As die 2** und die **4 ist die niedrigste Karte**. In Schwarz ist die **7 die niedrigste**.
- Hintergrund: Beim Kürzen des 78er-Blatts fielen in Rot 10–5 weg, in Schwarz As–6. Die alte Umkehrung der roten Zahlenkarten blieb erhalten [WIKI].
- Engine: `rankPower(card)` liefert je Farbe 8…1 in obiger Reihenfolge.

### 1.3 Tarock (Trümpfe)

- **Rangfolge:** Sk (Sküs, „Gstieß“, ohne Zahl, faktisch XXII) > XXI > XX > … > II > I [LTA-GK1 Kap. 2; WIKI]. Jedes Tarock sticht jede Farbkarte, außer in den Farbenspielen (6.3).
- **Namen:**

| Karte | Name(n) | Rolle im OÖ-Profil |
|---|---|---|
| Sk | Sküs, Gstieß | höchstes Tarock, Trullstück |
| XXI | Mond | Trullstück |
| I | Pagat (Spatz) | Trullstück, Vogel („Pagat ultimo“, letzter Stich) |
| II | Uhu | Vogel (vorletzter Stich) |
| III | Kakadu (Pelikan) | Vogel (drittletzter Stich) |
| IIII | Quapil (Vierer); in [LTA-GK1] auch „Wildsau“; anderswo „Marabu“ | Vogel (viertletzter Stich) |

- **Trull** = Sk + XXI + I.
- **Vögel („Vogerl“)** = I, II, III, IIII. [OÖ23 Ansagen] nennt sie **„Ansager“**. Vögel über IIII (V „Geier“ usw.) gibt es im OÖ-Profil nicht [OÖ23 Tarif].
- **Stecher:** umgangssprachlich die hohen Tarock (XVI–Sk nach [MAIR-KURS]), kein Regelbegriff.
- **Keine Sonderwirkungen:** Es gibt „weder Kaiserstich noch Fänge“ [OÖ23 Allg. 3]. Fallen Sk, XXI und I in einen Stich, gewinnt der Sküs. Mondfang, Sküsfang und Königfang werden nicht bewertet.

### 1.4 Kartenwerte

| Karten | Anzahl | Wert zum Zählen („aufgerundet“) | exakter Wert | Bezeichnung |
|---|---|---|---|---|
| Könige, Sk, XXI, I | 7 | 5 | 4⅓ | 5er-Zähler |
| Damen | 4 | 4 | 3⅓ | 4er-Zähler |
| Reiter | 4 | 3 | 2⅓ | 3er-Zähler |
| Buben | 4 | 2 | 1⅓ | 2er-Zähler |
| alle übrigen (19 Tarock II–XX, 16 Skartins) | 35 | 1 | ⅓ | „Blatt“ |

Quellen: [LTA-GK1 Kap. 3; WIKI; PAGAT]. Summe über alle 54 Karten: 70 Punkte = 210 Drittel.

### 1.5 Zählmethode (exakt implementieren)

**Tischmethode (OÖ-Skriptum):** Man zählt immer drei Karten zusammen [LTA-GK1 Kap. 3]:
- ein Zähler + zwei Blätter → Wert des Zählers;
- zwei Zähler + ein Blatt → Summe der beiden Zähler − 1;
- drei Zähler → Summe − 2;
- drei Blätter → 1 Punkt;
- Restkarten legt man so, dass nur ein oder zwei **Blätter** übrig bleiben. Sie werden als „1 Blatt“ bzw. „2 Blatt“ angesagt.

Gleichwertig: Summe der aufgerundeten Werte minus 2 je volles Dreierpaket. Eine übrige Karte zählt ⅔ weniger, zwei übrige 1⅓ weniger [WIKI: „Bleibt am Schluss eine Karte übrig, so wird von ihrem Wert ⅔ abgezogen; bleiben zwei Karten übrig, so werden von ihrem Wert 1⅓ Punkt abgezogen“]. [PAGAT] beschreibt denselben Vorgang gerundet („one or two odd cards left over … are worth one point less than their total value“); das ist die Tischnäherung, nicht die blattgenaue Rechnung. Für die Engine gilt die Drittelrechnung.

**Engine-Formel** (identisch zur Tischmethode, ohne Umsortieren):

```
drittel(Kartenmenge M) = 3 · Σ_{c∈M} wert(c)  −  2 · |M|
Punkte = drittel div 3      Blatt = drittel mod 3
```

Begründung: Jede Karte ist exakt (3·wert − 2)/3 Punkte wert. Ein Stich (4 Karten) ist also 3·Σwert − 8 Drittel wert. Damit lässt sich beim Mitzählen Stich für Stich addieren.

### 1.6 Gewinnschwelle

- **Positive Spiele** (Rufer-, Dreier- und Farbenspiele): Die Spielerpartei gewinnt das Spiel mit **mindestens 35 Punkten und 2 Blatt (= 107 Drittel)**, in [OÖ23 Allg. 3] „36 Punkte (= 35 Punkte und 2 Blatt)“. **35/1 reicht nicht; 35:35 ist verloren**, ein Unentschieden gibt es nicht [WIKI].
  - Warum 35/2 und nicht 35/1: Bei der alten Zählweise in Dreierlagen gibt es nur ganze Punkte. Die blattgenaue Zählung soll zum selben Ergebnis kommen, deshalb ist die Grenze 35/2 [MAIR-GRUND].
  - **[ABWEICHUNG]** „Lediglich regional genügen auch 35⅓ Punkte, um das Spiel zu gewinnen, etwa vielerorts in der Steiermark“ [WIKI]. Das betrifft private Runden: Das Regelwerk des Steirischen Tarockcups verlangt ausdrücklich „35 Punkte und 2 Blatt“ [STMK]. Für das OÖ-Profil ohnehin nicht relevant.
- **Trischaken:** „blattgenau“ zählen [OÖ23 Trischaken 1]. Verglichen werden die exakten Drittel. „Das Spiel haben“ (Bürgermeister) heißt ≥ 107 Drittel [OÖ23 Trischaken 2].
- **Piccolo, Bettel, Ouvertspiele:** Es zählen nur Stiche, keine Punkte (3.2).

### 1.7 Rechenbeispiele

**Beispiel A – kleiner Stapel (8 Karten):** ♥K, ♠D, VII | Sk, ♦B, ♣10 | XV, ♥R

| Paket | Karten | Rechnung | Ergebnis |
|---|---|---|---|
| 1 | ♥K (5), ♠D (4), VII (1) | zwei Zähler: 5 + 4 − 1 | 8 |
| 2 | Sk (5), ♦B (2), ♣10 (1) | zwei Zähler: 5 + 2 − 1 | 6 |
| Rest | XV (1), ♥R (3) | zwei Restkarten: 1 + 3 − 1⅓ | 2⅔ |
| | | **Summe** | **16 Punkte 2 Blatt** |

- Nach Tischmethode umgelegt: {♥K, ♠D, ♥R} = 5 + 4 + 3 − 2 = 10 und {Sk, ♦B, VII} = 5 + 2 − 1 = 6. Rest ♣10 und XV = „2 Blatt“, zusammen 16/2.
- Engine: Σwert = 22, n = 8 → 3·22 − 2·8 = 50 Drittel = 16/2. ✔

**Beispiel B – Endabrechnung eines ganzen Spiels** (Übungsspiel 1 in 11.7):
- **Spielerpartei (Du + Carl):** 35 Karten, nämlich 8 Stiche (32 Karten) + 3 Karten Ablage.
  - Zähler: ♠K ♣K ♥K ♦K (4 × 5), ♣D ♦D (2 × 4), ♣R ♥R (2 × 3), ♠B ♣B ♥B ♦B (4 × 2) → 42.
  - Dazu 23 Blätter → Σwert = 65.
  - 35 Karten = 11 Dreierpakete + 2 Restkarten: 65 − 22 − 1⅓ = **41⅔ = 41/2**.
  - Engine: 3·65 − 70 = 125 Drittel.
- **Gegenpartei:** 19 Karten (4 Stiche + liegen gelassene Talonhälfte).
  - Zähler: I, Sk, XXI (3 × 5), ♠D ♥D (2 × 4), ♠R ♦R (2 × 3) → 29.
  - Dazu 12 Blätter → Σwert = 41.
  - 19 Karten = 6 Pakete + 1 Rest: 41 − 12 − ⅔ = **28⅓ = 28/1**.
  - Engine: 3·41 − 38 = 85 Drittel.
- **Kontrolle:** 125 + 85 = 210 Drittel = 70 Punkte. 125 ≥ 107 → Spiel gewonnen.

### 1.8 Wem gehören welche Karten beim Zählen?

| Spielart | Ablage (verlegte Karten) | nicht gewählte Talonhälfte / Talon |
|---|---|---|
| Rufer, Besserrufer, Dreier, Farbendreier | zählt zu den Stichen des Spielers | zu den Stichen der Gegner [LTA-GK1 Kap. 7: „Bestand der Talon aus 3 Karten, zählt die verbleibende Talonhälfte zu den Stichen der Gegner“; WIKI] |
| Sechserdreier | 6 verlegte Karten zum Spieler [LTA-GK1 Kap. 7; STMK] | — (ganzer Talon aufgenommen) |
| Solorufer, Solodreier, Farbensolo | — | ganzer Talon zu den Gegnern [OÖ23 Farben 3; LTA-GK1 Kap. 9/10]. Ausnahme: Liegt beim Solorufer der gerufene König im Talon, gehört die Hälfte **mit dem König** dem Spieler [OÖ23 Allg. 7]. |
| Piccolo, Bettel, Piccolo/Bettel ouvert | — | Talon bleibt verdeckt und zählt nicht (Stichziel) [LTA-GK1 Kap. 11] |
| Trischaken | — | Talon zum Gewinner des letzten Stichs [OÖ23 Trischaken 1] |

- Die Karten des Talons gelten als „in den Stichen“ der jeweiligen Partei, auch für Trull und Alle Könige. Das gilt auch, wenn diese Partei keinen Stich hat [MAIR-AUSL: „Der Talon gehört in Solospielen zu den Stichen der Gegner (auch wenn diese keinen Stich haben). Das gilt auch für die liegengelassenen Talonkarten in Spielen mit Talon. Für die Besitzprämien (vier Könige, Trull) ist es egal, ob sich die entsprechenden Karten im Talon oder in den Stichen befinden“]; OÖ-konform, weil der Talon „zu den Stichen der Gegner zählt“ [LTA-GK1 Kap. 7].
- Sonderfall Valat: siehe 9, Punkt V-3.

---

## 2. Tisch, Geben, Talon, Vierer- und Fünfertisch

### 2.1 Rollen

| Rolle | Definition |
|---|---|
| **Geber** | mischt und teilt aus. Am Vierertisch spielt er mit, am Fünfertisch setzt er aus. |
| **Vorhand** | sitzt rechts vom Geber, spricht zuerst, spielt in positiven Spielen zum ersten Stich aus [LTA-GK1 Kap. 6]. Wird im nächsten Spiel Geber [WIKI]. |
| **Spieler** (Spielersteher) | hat die Lizitation gewonnen. |
| **Partner** | Inhaber des gerufenen Königs (nur Ruferspiele). |
| **Gegner** | alle anderen aktiven Spieler. |
| **Fünfter / Kiebitz** | am Fünfertisch der aussetzende Geber. |
| **Hintermann / Abheber** | sitzt links vom Geber. |

### 2.2 Erster Geber

„Wer die höchste Karte hebt, ist erster Geber.“ [OÖ23 Allg. 1] Eine Rangfolge der Karten beim Heben ist nicht festgelegt.
**[EMPFEHLUNG]** Die App lost den ersten Geber zufällig aus. Die Animation „Karte heben“ ist optional: Tarock schlägt Farbe, sonst Stichkraft, Gleichstand wird neu gehoben.

### 2.3 Mischen und Abheben

- Gründlich mischen. Der Hintermann (links vom Geber) hebt ab, „Klopfen“ ist verboten [OÖ23 Allg. 1].
- Die Taroquette verlangt: mindestens zweimal abheben, je mindestens 3 Blatt, beim Austeilen keine Karte zeigen [OÖT26 1].
- Der Geber darf „weder das Deckblatt noch seine ersten sechs Karten anschauen“ [OÖ23 Allg. 1].

**[EMPFEHLUNG]**
- Die Engine mischt mit einem kryptographisch geseedeten Zufallsgenerator.
- Im LAN mischt ausschließlich der Server. Abheben ist reine Animation.
- Für Übungsspiele und Replays gibt es einen deterministischen Seed bzw. vorgegebene Blätter.

### 2.4 Austeilen

[OÖ23 Allg. 1] „Der Talon wird in der Mitte gegeben.“ Präzisiert in [LTA-GK1 Kap. 5] und [KTC Allgemeines]:

1. Beginnend bei der Vorhand, gegen den Uhrzeigersinn, erhält jeder **aktive** Spieler **6 Karten**. Empfohlen in zwei Päckchen zu je 3.
2. Dann kommen **6 Karten als Talon** in die Tischmitte, als **zwei Hälften zu je 3 Karten** [KTC; WIKI].
3. Dann erhält jeder aktive Spieler, wieder ab der Vorhand, **nochmals 6 Karten**.

Engine-Algorithmus (Deck nach dem Mischen als Liste `d[0..53]`, aktive Spieler `a[0..3]` ab Vorhand):

```
i = 0
für Runde in {1,2}:
    für p in a[0..3]: hand[p] += d[i .. i+5]; i += 6
    wenn Runde == 1: talonHaelfte[0] = d[i..i+2]; talonHaelfte[1] = d[i+3..i+5]; i += 6
```

Jeder Spieler hat 12 Karten, der Talon 6 in zwei festen Hälften. Die Hälften bleiben bis zum Spielende getrennt, weil sie getrennt aufgenommen, liegen gelassen oder zugeordnet werden (4.3, 4.4).

### 2.5 Jedes Blatt wird gespielt

„Jedes Blatt muss gespielt werden (auch ohne Tarock und Könige).“ [OÖ23 Allg. 1] Es gibt **kein Zusammenwerfen** und kein Neugeben wegen schlechter Karten. Falsch gegebene Karten werden neu gegeben, wenn der Fehler vor Beginn der Lizitation bemerkt wird [OÖ23 Renonce 6]; in der App kann das nicht vorkommen.

### 2.6 Geberwechsel, Radl

- Nach jedem Spiel wechselt der Geber gegen den Uhrzeigersinn: **Der bisherige Vorhandspieler wird Geber** [LTA-GK1 Kap. 5: „Das nächste Spiel gibt der rechts vom letzten Geber sitzende Spieler“; WIKI].
- **Ein Radl** ist ein voller Umlauf, bei dem jeder einmal gibt: **4 Spiele am Vierertisch** [WIKI: „Vier Spiele bilden eine Runde oder ein Radl, wobei jeder Spieler einmal Kartengeber ist“]. **[EMPFEHLUNG]** Am Fünfertisch entsprechend **5 Spiele**; das ist die logische Fortschreibung (jeder gibt und setzt genau einmal aus), in den Quellen aber nicht ausdrücklich belegt.
- „Doppelradl“ oder Sonderrunden gibt es im OÖ-Turnierprofil nicht; private Optionen siehe 8.3.

### 2.7 Fünfertisch

- **Wer aussetzt:** Am Fünfertisch spielen nur vier; **der Geber setzt aus** [PAGAT; WIKI; LTA-GK1 Kap. 5]. Er teilt nur an die vier anderen aus [PAGAT] und erhält in diesem Spiel 0 Punkte.
- **Rotation:** unverändert. Neuer Geber ist der bisherige Vorhandspieler, also setzt jeder Spieler genau einmal pro Radl (5 Spiele) aus.
- **Geheimhaltung:** „Für den Geber bei Fünfertischen ist der Talon tabu. Er darf sich nicht ins Spiel einmischen („Kiebitz halt's Maul“).“ [OÖ23 Allg. 8] „Der Talon ist auch für den 5. Spieler tabu!“ [OÖT26 2]. Über das laufende Spiel darf „weder durch Reden oder sonstige Äußerungen noch durch Mimik, Gestik oder sonst wie etwa durch extra ‚deutliches‘ Spielen einer Karte“ etwas verraten werden [OÖT26 6]; „Kiebitze dürfen sich nicht ins Spiel einmischen; jegliche Äußerungen zum Spiel sind strikt untersagt (das gilt auch für den ‚Fünften‘)“ [OÖT26 7].
- **Renonce des Fünften:** Er müsste die vier Spieler schadlos halten [OÖR25 B.4]. In der App ausgeschlossen (siehe unten).

**[EMPFEHLUNG] Informationsmodell für Zuschauer** (serverseitig gefiltert, gilt auch für Kiebitze im LAN):

| Information | aktiver Spieler | Fünfter / Zuschauer während des Spiels |
|---|---|---|
| eigene Handkarten | ja | hat keine |
| fremde Handkarten | nein (Ausnahme: Ouvertspiele ab dem 2. Ausspiel) | nein (Ouvert-Hände: ja, sie liegen offen am Tisch) |
| offen aufgedeckter Talon (Rufer, Besserrufer, Dreier, Farbendreier) | ja während der Talonphase; die liegen gelassene Hälfte bis zum Ende des 1. Stichs, danach wird sie umgedreht [OÖT26 3] | **nein**, der Talon ist tabu |
| Solo-, Sechser- und Negativ-Talon | nein | nein |
| offen verlegte Karten (Tarock bzw. Farbkarten beim Farbendreier) | ja | nein (Ablage gehört zum Talonvorgang) |
| gerufener König (Farbe), Ansagen, Kontras, gespielte Karten, letzter Stich | ja | ja |
| Wer ist Partner? | nur der Partner selbst | nein |
| Chat / Emotes | nein während des Spiels (ausgenommen die Beratung der Gegner im Ouvertspiel, 6.4) | nein |
| nach Spielende: alle Karten, Talon, Ablage | ja (Rückschau) | ja |

Der Fünfte sieht damit nichts, was ein am Tisch schweigender Kiebitz nicht auch sehen dürfte.

### 2.8 Sichtbarkeit während des Spiels (alle Spieler)

- **Letzter Stich:** Jeder darf den letzten Stich ansehen [ÖF21 Allg.; OÖT26 3].
- **Eigene Stiche:**
  - **[ABWEICHUNG]** [LTA-GK1 Kap. 8]: „Jeder Spieler darf sich seine eigenen Stiche und die seines Partners immer anschauen.“ [MAIR-CUPS] vermerkt für OÖ „Stiche anschauen erlaubt (nachwassern): ja“. [ÖF21]: „Die Stiche der anderen Spieler dürfen nicht angesehen werden.“ (Das verbietet nur fremde Stiche.)
  - **[EMPFEHLUNG]** Eigene Stiche jederzeit einsehbar; fremde nur der jeweils letzte Stich.
  - Partnerstiche nur, wenn die Partnerschaft für den Betrachter feststeht, also für den Partner selbst (er kennt die Partnerschaft von Beginn an) und für den Spieler, sobald der gerufene König gefallen ist.
- **Nicht gewählte Talonhälfte:** „Liegengebliebene Talonkarten werden nach dem ersten Stich zur Gänze umgedreht.“ [OÖT26 3]. Bis zum Ende des 1. Stichs offen, danach verdeckt. **[EMPFEHLUNG]** Im Lernmodus als Einblendung „Erinnerung“ abrufbar.
- **Eigene Ablage:** Der Spieler darf seine Ablage einsehen (er kennt sie ohnehin). Offen verlegte Tarock bzw. Farbkarten bleiben für alle sichtbar vermerkt.

---

## 3. Lizitation

### 3.1 Die Spiele in Lizitationsreihenfolge

Die OÖ-Spiele sind „streng hierarchisch gereiht“. Die Reihenfolge der Tariftabelle in [OÖ23 Tarif] **ist** die Lizitationsreihenfolge [LTA-GK1 Kap. 4; MAIR-LIZ, Spalte OÖ]. Aufsteigend:

| Rang | Spiel (Synonyme) | Wert (Schrift) | Art | Partner | Talon | Wer darf bieten | 1. Ausspiel | Prämien | Kontra |
|---|---|---|---|---|---|---|---|---|---|
| 1 | **Trischaken** (Fahren) | 1 (Sonderwertung 7.6) | negativ, jeder gegen jeden | – | bleibt verdeckt, am Ende an den Gewinner des letzten Stichs | nur Vorhand, nur *hintennach* | Vorhand | keine | keines |
| 2 | **Rufer** (Vorhand-Rufer) | 1 | positiv, Ruferspiel | gerufener König | Hälfte aus offenem Talon | nur Vorhand, nur *hintennach* | Vorhand | alle | gilt für alle |
| 3 | **Piccolo** | 2 | negativ | allein | nicht verwendet | alle | Spieler | keine | einzeln |
| 4 | **Bettel** (Bettler) | 2 | negativ | allein | nicht verwendet | alle | Spieler | keine | einzeln |
| 5 | **Solorufer** | 2 | positiv, Ruferspiel, Solo | gerufener König | kein Talon (an Gegner) | alle | Vorhand | alle, **doppelt** | gilt für alle |
| 6 | **Besserrufer** (A-Rufer) | 1 + Vogel | positiv, Ruferspiel | gerufener König | Hälfte aus offenem Talon | alle (Vogel im Blatt nötig) | Vorhand | alle; ursprünglicher Vogel ist Pflicht | gilt für alle |
| 7 | **Farbendreier** | 3 | positiv, Farbenspiel | allein | Hälfte aus offenem Talon | alle | Vorhand | nur Alle Könige, Valat | gilt für alle |
| 8 | **Sechserdreier** (Sechser) | 4, verloren 8 | positiv, Dreierspiel | allein | alle 6 verdeckt | nur Vorhand, nur *vorneweg* | Vorhand | alle außer König ultimo | gilt für alle |
| 9 | **Dreier** | 4 | positiv, Dreierspiel | allein | Hälfte aus offenem Talon | alle | Vorhand | alle außer König ultimo | gilt für alle |
| 10 | **Farbensolo** | 6 | positiv, Farbenspiel, Solo | allein | kein Talon (an Gegner) | alle | Vorhand | nur Alle Könige (doppelt), Valat | gilt für alle |
| 11 | **Piccolo ouvert** | 6 | negativ, offen | allein | nicht verwendet | alle | Spieler | keine | einzeln |
| 12 | **Bettel ouvert** | 7 | negativ, offen | allein | nicht verwendet | alle | Spieler | keine | einzeln |
| 13 | **Solodreier** | 8 | positiv, Dreierspiel, Solo | allein | kein Talon (an Gegner) | alle | Vorhand | alle außer König ultimo, **doppelt**, sofort ansagen | gilt für alle |

**Belege:**
- Tarif: [OÖ23 Tarif; LTA-GK1 Kap. 4; ÖF21].
- Wer bieten darf: [OÖ23 Vorhand 1].
- Erstes Ausspiel: [OÖ23 Negativ 1; Farben 1; LTA-GK1 Kap. 9].
- Prämien je Spiel: [LTA-GK1 Kap. 10/11; LTA-F1; ÖF21 Ansagen].
- Kontra: [OÖ23 Allg. 5].
- Doppelte Prämien in Solospielen: [OÖ23 Ansagen 1].

**Anmerkungen:**
- **Rang 1 und 2:** Trischaken und Rufer kommen nur ohne andere Gebote zustande. Ihre Reihenfolge ist daher bedeutungslos.
- **Besserrufer:** [OÖ23 Tarif] schreibt „A-Rufer“, der Text „Besserrufer“, [LTA-GK1] „Besser-Rufer (A-Rufer)“ („A“ steht für *Ansager*).
  - Im OÖ-Profil gibt es **ein einziges** Besserrufer-Gebot, gleich welcher Vogel [MAIR-CUPS: „Besserrufer nicht aufgeteilt – OÖ: ja“]. Es gibt also keine Einzelgebote Pagatrufer, Uhurufer usw.
  - Ein Besserrufer mit Quapil steht in der Lizitation genauso unter dem Farbendreier wie einer mit Pagat.
  - Gewertet werden Spiel (1) und Vogel (als angesagte Prämie) getrennt (7.3).
- **Nicht im OÖ-Profil:** Zwiccolo, Triccolo, „Bei“-Spiele, Einzel-Vogelrufer, Besserdreier, Farbensechser, Farbenrufer, Supra, Absolut/Säcke [OÖ23 Tarif; MAIR-CUPS; LTA-F1 „Es gibt keine sog. Bei-Spiele“]. Zwiccolo und Farben-Sechser nennt [LTA-S] als verbreitete private Ergänzungen, siehe 9.

### 3.2 Kurzbeschreibung der Spielziele

- **Rufer:** Spieler ruft einen König als Partner und nimmt eine Talonhälfte. Die Partei braucht 35/2. Nur Vorhand, nur nach dreimal „weiter“ [OÖ23 Vorhand 1; LTA-GK1 Kap. 10].
- **Solorufer:** wie Rufer, aber ohne Talon; der Talon zählt zu den Gegnern. Alle Prämien zählen doppelt. Typisches Hauptziel ist ein vom Partner angesagter König ultimo (4 Punkte); das Spiel (2) ist Nebenziel [LTA-GK1 Kap. 10].
- **Besserrufer:** wie Rufer, der Spieler muss aber den Vogel ansagen, den er schon vor der Talonaufnahme im Blatt hatte [OÖ23 Ansagen 4; LTA-GK1 Kap. 10]. Hauptziel ist der Vogel, das Spiel zählt nur 1.
- **Dreier:** allein gegen drei, eine Talonhälfte, 35/2.
- **Sechserdreier:** allein gegen drei. Der Spieler nimmt alle sechs Talonkarten verdeckt auf und verlegt sechs. Verloren zählt das Spiel doppelt [ÖF21 Vorhand; LTA-GK1 Kap. 10].
- **Solodreier:** allein gegen drei ohne Talon, höchstes Spiel. Prämien doppelt, Ansagen unmittelbar mit dem Gebot [OÖ23 Ansagen 3].
- **Farbendreier / Farbensolo:** allein gegen drei, mit Talonhälfte bzw. ohne Talon. **Tarock sticht keine Farbe**; Tarock darf erst ausgespielt werden, wenn man keine Farbkarte mehr hat [OÖ23 Farben 1–3].
- **Piccolo:** allein, **genau ein** Stich. **Bettel:** allein, **kein** Stich. Stichzwang, der Spieler spielt aus [OÖ23 Negativ 1–2; LTA-GK1 Kap. 11].
- **Piccolo ouvert / Bettel ouvert:** Nach dem ersten Stich und vor dem zweiten Ausspiel legen **alle** Spieler ihre Karten offen auf. Die drei Gegner dürfen sich beraten [OÖ23 Negativ 3].
- **Trischaken:** jeder gegen jeden. Wer die meisten Punkte hat, verliert [OÖ23 Trischaken 1–3].

### 3.3 Rechte und Pflichten der Vorhand

1. **Eröffnung:** Die Vorhand beginnt mit „**Vorhand**“ oder mit einer Spielansage [OÖ23 Allg. 2].
   - „Vorhand“ (auch „meine Vorhand“, „mein Spiel“) sagt noch kein Spiel an, sondern hält alle Möglichkeiten offen [WIKI; LTA-F1 Kap. 2].
   - **[EMPFEHLUNG]** Die Vorhand kann beim ersten Sprechen nicht passen; so ausdrücklich [KTC], in [OÖ23] durch „beginnt mit ‚Vorhand‘ oder einer Spielansage“ vorausgesetzt.
2. **Sechserdreier nur vorneweg:** Er kann nur als erstes Wort der Vorhand angesagt werden, später nicht mehr [OÖ23 Vorhand 1; LTA-F1 Kap. 2].
3. **Rufer und Trischaken nur hintennach:** Sie sind nur möglich, wenn die Vorhand „Vorhand“ gesagt hat und alle drei anderen „weiter“ gesagt haben.
   - In diesem Fall darf die Vorhand **jedes Spiel außer dem Sechserdreier** wählen, also auch Rufer oder Trischaken [OÖ23 Vorhand 1; LTA-GK1 Kap. 10; LTA-F1 Kap. 2].
   - Hat irgendwer ein Spiel angesagt, sind Rufer und Trischaken nicht mehr möglich [LTA-GK1 Kap. 11].
4. **Halten:** Die Vorhand darf das zuletzt lizitierte (höchste) Spiel „halten“, d. h. selbst spielen, ohne es überbieten zu müssen [OÖ23 Vorhand 2; LTA-GK1 Kap. 6; LTA-F1 Kap. 2].
   - Das Halterecht hat **nur** die Vorhand [MAIR-CUPS: „Vorhand: Spiel halten (bei mir) – OÖ ja, Tirol –, Wien –“].
5. **Alle vier Könige:** Kein Ruferspiel (Rufer, Solorufer, Besserrufer). Die Vorhand darf trotzdem nicht „weitergeben“ [OÖ23 Vorhand 3]. Sagen alle „weiter“, muss sie ein anderes Spiel spielen (z. B. Trischaken, Dreier, Farbensolo) [LTA-GK1 Kap. 10].
   - **[ABWEICHUNG]** [ÖF10] erlaubte noch „Gebe die Vorhand weiter“, [OÖ23] nicht mehr.
6. Die Vorhand darf **alle** Spiele bieten, nicht nur Vorhandspiele [MAIR-CUPS: „Vorhand darf nur Vorhandspiele ansagen – OÖ: –“].
7. In **allen positiven Spielen** spielt die Vorhand zum ersten Stich aus, auch als Gegner.

### 3.4 Die übrigen Spieler

- Wer an der Reihe ist, sagt entweder ein **höheres** Spiel an oder passt. [OÖ23 Allg. 2] kennt dafür nur das Wort **„gut“** („Wer kein Spiel anmelden oder überbieten will, muss passen (‚gut‘)“); am Tisch und in den Lehrtexten ist **„weiter“** gleichbedeutend und mindestens ebenso üblich [LTA-GK1 Kap. 6: „akzeptieren das Spiel des Vorgängers mit ‚gut‘ bzw. ‚weiter‘“; WIKI: „Wer kein Spiel anmeldet, sagt Weiter!“; KTC: „dreimal Weiter!“]. Die App akzeptiert beides und beschriftet den Knopf mit „weiter“.
- **Passen ist endgültig** [LTA-GK1 Kap. 6; WIKI].
- Alle Spiele außer den drei Vorhandspielen dürfen „vorneweg und hintennach“ angesagt werden [OÖ23 Vorhand 1]: beim ersten Sprechen oder später, solange man nicht gepasst hat.
- Gleich hoch bieten darf nur die Vorhand (Halten). Ein eigenes Gebot darf man nicht selbst überbieten [MAIR-KURS]; das ergibt sich im Automaten von selbst.

### 3.5 Lizitation als Zustandsautomat

```
Eingabe: aktive Plätze a[0..3] (a[0] = Vorhand), Handkarten
Zustand: aktiv[4] = true; hoechstes = null  // (spiel, bieter)
         vorhandGesagt = false; andereHabenGeboten = false

// 1. Eröffnung der Vorhand (kein Passen)
optionen(a[0]) = {"Vorhand"} ∪ {Sechserdreier} ∪ { s ∈ ALLGEMEIN | erlaubtFuerBlatt(s, a[0]) }
   wenn "Vorhand": vorhandGesagt = true
   sonst: hoechstes = (s, a[0])

// 2. Reihum ab a[1], zyklisch, übersprungen werden inaktive Plätze
solange nicht beendet:
   p = nächster aktiver Platz
   wenn hoechstes != null und hoechstes.bieter == p:   // alle anderen haben seit p's Gebot gepasst
        beenden(spiel = hoechstes.spiel, spieler = p)
   wenn p == a[0]:                                     // Vorhand am Zug
        optionen = {weiter}
                 ∪ { halten | hoechstes != null }                      // gleiches Spiel, Bieter wird a[0]
                 ∪ { s ∈ ALLGEMEIN | rang(s) > rang(hoechstes) ∧ erlaubtFuerBlatt(s, p) }
        // (nach "Vorhand" ohne fremdes Gebot kommt die Vorhand hier nicht mehr an, siehe Endebedingung B)
   sonst:
        optionen = {weiter} ∪ { s ∈ ALLGEMEIN | rang(s) > rang(hoechstes) ∧ erlaubtFuerBlatt(s, p) }
   weiter  -> aktiv[p] = false
   Gebot s -> hoechstes = (s, p); wenn p != a[0]: andereHabenGeboten = true
   halten  -> hoechstes = (hoechstes.spiel, a[0])

   Endebedingung A: genau ein aktiver Platz übrig und hoechstes != null -> Spiel = hoechstes
   Endebedingung B: vorhandGesagt ∧ hoechstes == null ∧ a[1],a[2],a[3] inaktiv
                    -> Vorhand wählt aus ALLGEMEIN ∪ {Rufer, Trischaken} (ohne Sechserdreier),
                       jeweils mit erlaubtFuerBlatt
   Endebedingung C: hoechstes.spiel == Solodreier ∧ (Bieter == a[0] ∨ a[0] inaktiv)
                    -> sofort beendet (kann nicht überboten und nicht mehr gehalten werden)

ALLGEMEIN = {Piccolo, Bettel, Solorufer, Besserrufer, Farbendreier, Dreier, Farbensolo,
             Piccolo ouvert, Bettel ouvert, Solodreier}
rang(null) = 0; rang(Sechserdreier) liegt zwischen Farbendreier und Dreier (Tabelle 3.1)
erlaubtFuerBlatt(s, p):
   s ∈ {Rufer, Solorufer, Besserrufer} -> p hat höchstens 3 Könige
   s == Besserrufer                    -> p hat mindestens einen von I, II, III, IIII
   sonst true
```

- **Terminierung:** Jedes Gebot ist streng höher, halten ist pro Gebotsstufe nur einmal möglich. Der Automat endet also spätestens nach 13 Stufen.
- **Endebedingung C:** Siehe **[OFFEN] L-4** in 9. Die Vorhand kann einen Solodreier halten, solange sie noch aktiv ist.

### 3.6 Beispiele (OÖ-Reihenfolge)

Spieler A (Vorhand), B, C, D (Geber) in Spielreihenfolge.

1. **Normalfall:** A „Vorhand“ · B „weiter“ · C „Bettel“ · D „Solorufer“ · A „weiter“ · C „Farbendreier“ · D „weiter“.
   → **C spielt Farbendreier**, A spielt zum ersten Stich aus.
   - D hätte nach C's Bettel keinen Piccolo mehr sagen dürfen (Rang 3 < 4).
   - Anders als in manchen Wiener Tabellen liegt der Bettel in OÖ **unter** dem Solorufer.
2. **Halten:** A „Vorhand“ · B „Piccolo“ · C „weiter“ · D „Besserrufer“ · A „ich halte“ · B „weiter“ · D „weiter“.
   → **A spielt Besserrufer.** D hätte gegen A's Halten ein höheres Spiel ab Farbendreier bieten müssen.
3. **Sechserdreier überboten:** A „Sechserdreier“ · B „weiter“ · C „Dreier“ · D „weiter“ · A „ich halte“ · C „weiter“.
   → **A spielt einen (normalen) Dreier** mit einer Talonhälfte.
   - Der Dreier überbietet den Sechserdreier (Rang 9 > 8), der Farbendreier nicht [MAIR-GRUND: „In OÖ überbietet der Sechserdreier den Farbendreier“].
   - Dass die Vorhand den Dreier nach eigenem Sechser halten darf, folgt aus [OÖ23 Vorhand 2]; es ist nirgends gesondert erwähnt.
4. **Nur die Vorhand:** A „Vorhand“ · B, C, D „weiter“.
   → A wählt Rufer, Trischaken oder jedes andere Spiel außer dem Sechserdreier.
5. **Vorhand mit Gebot:** A „Piccolo“ · B, C, D „weiter“ → **A spielt Piccolo.**
   - Ein einmal angesagtes Spiel wird gespielt. Auf Rufer oder Trischaken kann A nicht mehr zurück, denn beide gibt es nur nach „Vorhand“.

### 3.7 Blattbedingungen für Gebote (von der App erzwungen)

- **Besserrufer:** Nur mit mindestens einem Vogel (I, II, III oder IIII) im Blatt **vor** der Talonaufnahme [OÖ23 Ansagen 4; LTA-GK1 Kap. 10: „Der Spieler muss das geplante Vogerl schon beim Rufen in der Karte haben“]. Am Tisch ist ein Verstoß Renonce (je 3 Punkte an die Mitspieler) [OÖ23 Ansagen 4; OÖR25].
- **Ruferspiele:** nicht mit allen vier Königen [OÖ23 Vorhand 3; Allg. 6; MAIR-AUSL].
- Die App bietet unzulässige Gebote nicht an bzw. lehnt sie mit Erklärung ab (11.4). **Renonce kann in der App nicht entstehen.** Die Renonceregeln sind nur zur Vollständigkeit in 7.9 wiedergegeben.

### 3.8 Sprechformeln (UI-Texte)

| Handlung | Formel | Beleg |
|---|---|---|
| Vorhand eröffnet ohne Spiel | „Vorhand“ | [OÖ23 Allg. 2] |
| Passen | „gut“ (so [OÖ23 Allg. 2]); gleichbedeutend „weiter“ | [OÖ23 Allg. 2; LTA-GK1 Kap. 6; WIKI] |
| Gebot | Spielname, z. B. „Dreier“ | [KTC Etikette] |
| Halten (nur Vorhand) | „ich halte“ / „bei mir“ | [OÖ23 Vorhand 2; MAIR-CUPS] |
| König rufen | „Der Pik-König“ bzw. „der vierte König“ | [OÖ23 Allg. 6] |
| Ansagen abschließen | „Ich liege“ | [OÖ23 Allg. 2] |
| Ansage zur Kenntnis nehmen | „gut“ | [OÖ23 Allg. 2] |
| Kontra / Rekontra / Subkontra | „Kontra“ („gespritzt“, „Schuss“) / „Rekontra“ („retour“) / „Subkontra“ („nochmals gespritzt“) | [OÖ23 Allg. 5; LTA-GK1 Kap. 8] |

---

## 4. Partner, Talon und Verlegen

### 4.1 Königsruf (Rufer, Solorufer, Besserrufer)

1. **Zeitpunkt:** unmittelbar nach Ende der Lizitation, **vor** dem Aufdecken des Talons.
   - Belege: [WIKI]: „ruft der Spielersteher einen Partner. Erst danach wählt er eine Hälfte des Talons“. [KTC Renonce]: vor Lizitationsende darf niemand „den Talon öffnen oder einen König rufen“.
   - Ansageformel „Besser-Rufer mit Pagat, der Pik König hilft mir, ich liege“ [LTA-GK1 Kap. 6]. Die Farbe wird am Ende wiederholt [OÖT26 3].
2. **Wahl:** Der Spieler nennt eine Farbe (♥, ♦, ♠, ♣). Partner ist, wer den **König dieser Farbe** hält.
   - Verboten: einen König rufen, der **im eigenen Blatt** ist [OÖ23 Allg. 6]; auch „Selbstrufen“ [ÖF21 Rufen].
3. **Drei Könige:** Wer genau drei Könige hält, darf „**den vierten König**“ rufen, ohne die Farbe zu nennen [OÖ23 Allg. 6].
   - Engine: gerufene Farbe = Farbe des fehlenden Königs. Öffentlich wird nur „vierter König“ angezeigt.
   - **[EMPFEHLUNG]** Mit genau drei Königen darf der Spieler auch die Farbe des vierten Königs offen nennen; beides führt zum selben Partner.
4. **Vier Könige:** Ruferspiel unmöglich (3.7).
5. **Verdeckte Partnerschaft:** Nur der Partner weiß, dass er Partner ist. Öffentlich wird die Partnerschaft,
   - sobald der gerufene König gespielt wird, oder
   - sobald dessen Inhaber „König ultimo“ ansagt; nur er darf das (5.3).

   Andere Hinweise als durch Spielweise sind verboten [LTA-GK1 Kap. 10: „Andere Äußerungen sind nicht erlaubt“; OÖT26 6].
   Engine: `partnerSeat` bleibt serverseitig. Das öffentliche Flag `partnerBekannt` wird gesetzt, wenn der König fällt oder KU vom Inhaber angesagt wird.

### 4.2 Talonbehandlung je Spiel

| Spiel | Talon |
|---|---|
| Rufer, Besserrufer, Dreier, Farbendreier | beide Hälften offen aufdecken, eine Hälfte aufnehmen, 3 Karten verlegen |
| Sechserdreier | alle 6 Karten verdeckt aufnehmen, 6 verlegen |
| Solorufer, Solodreier, Farbensolo | Talon bleibt verdeckt, am Ende zu den Gegnern (Solorufer: siehe 4.5 b) |
| Piccolo, Bettel, Piccolo ouvert, Bettel ouvert | Talon bleibt verdeckt, zählt nicht |
| Trischaken | Talon bleibt verdeckt, am Ende an den Gewinner des letzten Stichs |

Belege: [OÖ23 Allg. 7; Farben 1, 3; Trischaken 1; LTA-GK1 Kap. 7, 10, 11].

### 4.3 Offener Talon (Rufer, Besserrufer, Dreier, Farbendreier)

1. Der Spieler deckt beide Hälften **für alle aktiven Spieler** auf [LTA-GK1 Kap. 7; WIKI; KTC Etikette]. Für den Fünften ist der Talon tabu (2.7).
2. Nur bei Rufer oder Besserrufer: Liegt der gerufene König im Talon, gilt 4.5 a.
3. Der Spieler wählt **eine** Hälfte und nimmt ihre 3 Karten auf (15 Karten in der Hand).
4. Er verlegt genau 3 Karten nach 4.6. Die verlegten Karten zählen zu seinen Stichen.
5. Die andere Hälfte gehört den Gegnern. Sie dürfen keine Karten austauschen [LTA-GK1 Kap. 7]. Sie bleibt bis zum Ende des ersten Stichs offen und wird dann umgedreht [OÖT26 3].
6. Danach folgen die Ansagen (5.3) und „Ich liege“.

### 4.4 Sechserdreier

- Der Spieler nimmt **alle sechs Talonkarten verdeckt** auf; der Talon wird niemandem gezeigt [LTA-GK1 Kap. 7, 10]. Er hat 18 Karten und verlegt **sechs** nach 4.6.
- Verlegte Tarock sind auch hier offen zu zeigen [OÖ23 Renonce 2]. Die sechs verlegten Karten zählen zu seinen Stichen [STMK].

### 4.5 Gerufener König im Talon

**a) Rufer und Besserrufer.** Der König ist beim Aufdecken sichtbar. Der Spieler wählt, bevor er eine Hälfte aufnimmt [OÖ23 Allg. 7; ÖF21 Rufen; LTA-GK1 Kap. 10]:

- **„Schleifen“ (aufgeben):** Das Spiel endet sofort.
  - Wortlaut: „Es werden die Punkte von Spiel und Ansage als verloren gewertet“ [OÖ23 Allg. 7]; „er verliert Spiel plus Ansager (3, 5, 7 oder 9 Punkte)“ [ÖF21].
  - Die Gegner können nicht kontrieren [LTA-GK1 Kap. 10: „Er kann das Spiel schleifen und das Spiel samt geplantem Vogerl zahlen. Die Gegner können nicht kontrieren“].
  - Abrechnung in 7.7.
- **Allein weiterspielen:** Der Spieler wählt **frei** eine Hälfte, mit oder ohne König [LTA-GK1 Kap. 10: „egal, ob jene mit dem König oder die andere“].
  - Den gerufenen König liegen zu lassen, ist sanktionslos [OÖ23 Allg. 7]. „Der Verlust des gerufenen Königs wird nicht bewertet, es sei denn er wird ultimo angesagt“ [ÖF21].
  - Er spielt **allein gegen drei**; die Abrechnung erfolgt wie beim Alleinspiel (7.1). Kontras sind möglich [OÖ23 Allg. 7].
  - Beim Besserrufer bleibt die Pflichtansage des ursprünglichen Vogels bestehen [OÖ23 Ansagen 4].
  - **[EMPFEHLUNG]** Nimmt er die Hälfte mit dem König, darf er selbst „König ultimo“ ansagen. [OÖ23] regelt das nicht, erlaubt ist es in [MAIR-AUSL]; es verstößt nicht gegen [OÖ23 Ansagen 1], denn er hält den König selbst.

**b) Solorufer.** Der Talon bleibt verdeckt, niemand weiß von der Lage.
- Der Spieler spielt faktisch allein gegen drei.
- Am Spielende erhält er die **Talonhälfte mit dem König**, die andere geht an die Gegner.
- **Jedes Kontra ist wirkungslos**, auch wenn der Spieler gewinnt [OÖ23 Allg. 7; LTA-F1 Kap. 3].
- Abrechnung als Alleinspieler gegen drei [LTA-GK1 Kap. 10: „so spielt der Spieler allein gegen drei“].
- Engine und Computerspieler dürfen dieses Wissen während des Spiels **nicht** verwenden (10.8).

**Wahrscheinlichkeit:** Der gerufene König liegt mit 6/42 = 1/7 im Talon (42 unbekannte Karten, 6 davon im Talon) [MAIR-GRUND: „bei jedem siebten Besserrufer“].

### 4.6 Verlegen (Ablage)

**Tarockspiele** (Rufer, Besserrufer, Dreier, Sechserdreier):
- **Nie** Könige, **nie** Trullstücke (Sk, XXI, I).
- Tarock (II–XX) darf man nur verlegen, wenn keine verlegbaren Farbkarten mehr vorhanden sind. Verlegte Tarock werden **offen** gezeigt.
- Genau 3 Karten (Sechserdreier: 6).
- Belege: [OÖ23 Renonce 2: „Unerlaubte Ablage (Trullstücke, Könige, falsche Anzahl, verdeckte Tarock)“; LTA-GK1 Kap. 7: „Bei Tarockspielen dürfen Tarock nur verlegt werden, wenn der Spieler keine Farbkarten mehr verlegen kann. Sie müssen aber aufgedeckt werden“; WIKI].

**Farbendreier:**
- **Zuerst Tarock** verlegen, **verdeckt**, keine Trullstücke.
- Farbkarten erst, wenn keine verlegbaren Tarock mehr da sind, dann **immer offen**.
- Nie Könige.
- Belege: [OÖ23 Farben 1; LTA-GK1 Kap. 7, 10; LTA-F1 Kap. 9].
- **[ABWEICHUNG]** [KTC] und [STMK] verlangen zusätzlich mindestens sechs Farbkarten nach dem Verlegen; im OÖ-Profil gilt das nicht [MAIR-CUPS].

**Validierung** (Kartenmenge H nach Aufnahme, zu verlegende Anzahl k, gewählte Ablage D):

```
VERBOTEN = Könige ∪ {Sk, XXI, I}
FT = { c ∈ H | c ist Farbkarte ∧ c ∉ Könige }          // verlegbare Farbkarten
TT = { c ∈ H | c ist Tarock ∧ c ∉ {Sk, XXI, I} }       // verlegbare Tarock

gueltig_Tarockspiel(D)  = |D| == k ∧ D ∩ VERBOTEN == ∅ ∧ ( D ∩ Tarock == ∅  ∨  FT ⊆ D )
                          offenZeigen = D ∩ Tarock
gueltig_Farbendreier(D) = |D| == 3 ∧ D ∩ VERBOTEN == ∅ ∧ ( D ∩ Farbkarten == ∅  ∨  TT ⊆ D )
                          offenZeigen = D ∩ Farbkarten
```

- Eine gültige Ablage existiert immer: Unter 15 (18) Karten sind höchstens 7 verbotene.
- Die UI markiert verlegbare Karten und erklärt Ablehnungen (11.4).

### 4.7 Was offengelegt wird

| Was | Wem | Bis wann |
|---|---|---|
| beide Talonhälften (Rufer, Besserrufer, Dreier, Farbendreier) | vier aktive Spieler | Aufnahme; die liegen gelassene Hälfte bis Ende 1. Stich [OÖT26 3] |
| verlegte Tarock (Tarockspiele einschl. Sechser) | vier aktive Spieler | bleibt als Information vermerkt |
| verlegte Farbkarten (Farbendreier) | vier aktive Spieler | bleibt vermerkt |
| gerufene Farbe bzw. „vierter König“ | alle, auch der Fünfte | ganzes Spiel |
| Karten aller Spieler im Ouvertspiel | alle | ab dem 2. Ausspiel |
| alle Karten, Talon und Ablage | alle | nach Spielende (Rückschau) |

### 4.8 Übergang zu den Ansagen

Nach eventueller Ablage wiederholt der Spieler Spiel, gerufene Farbe und seine Prämien und schließt mit „**Ich liege**“ [OÖT26 3; LTA-GK1 Kap. 6; KTC Etikette].
- „Ich liege“ ist sein letztes Wort. Danach sind keine eigenen Prämienansagen und keine Änderung der Ablage mehr möglich [KTC Etikette; STMK].
- **UI:** Zusammenfassungsbanner „Dreier · Pagat · ich liege“ bzw. „Rufer · Pik-König · ich liege“.

---

## 5. Ansagen (Prämien) und Kontra

### 5.1 Übersicht der Prämien im OÖ-Profil

| Prämie | Bedingung | still | angesagt | in Solospielen | möglich in |
|---|---|---|---|---|---|
| **Trull** | Sk + XXI + I in den Stichen der eigenen Partei | 1 | 2 | doppelt | positive Tarockspiele |
| **Alle Könige** | alle vier Könige in den Stichen der eigenen Partei | 1 | 2 | doppelt | positive Tarockspiele **und** Farbenspiele |
| **König ultimo** | der gerufene König liegt im letzten Stich und die Spielerpartei gewinnt ihn | 1 | 2 | doppelt | nur Ruferspiele |
| **Pagat** (Pagat ultimo) | I gewinnt den 12. Stich | 1 | 2 | doppelt | positive Tarockspiele |
| **Uhu** | II gewinnt den 11. Stich | 2 | 4 | doppelt | positive Tarockspiele |
| **Kakadu** | III gewinnt den 10. Stich | 3 | 6 | doppelt | positive Tarockspiele |
| **Quapil** | IIII gewinnt den 9. Stich | 4 | 8 | doppelt | positive Tarockspiele |
| **Valat** | eine Partei gewinnt alle 12 Stiche | Spielwert × 4 | Spielwert × 8 | **nicht** zusätzlich verdoppelt | alle positiven Spiele |

Belege: [OÖ23 Tarif] (angesagte Werte), [OÖ23 Ansagen 1] („Alle Prämien zählen still die Hälfte, bei den Solospielen doppelt“), [OÖ23 Ansagen 2] (Valat 8-fach/4-fach), [LTA-GK1 Kap. 4, 12], [ÖF21 Ansagen].

- **Solospiele** sind Solorufer, Solodreier und Farbensolo [LTA-GK1 Kap. 9].
- Der Valat wird nicht zusätzlich verdoppelt, weil er sich ohnehin auf den bereits höheren Solo-Spielwert bezieht [LTA-GK1 Kap. 9: „Der Valat bezieht sich ohnedies auf den hier höheren Spielwert (4 bzw. 8-fach)“; STMK ausdrücklich: „Im Solorufer, Solodreier und Farbensolo gelten alle Prämien doppelt (Valat bleibt aber 8-fach/4-fach)“]. Rechnerische Gegenprobe [MAIR-CUPS]: angesagter Valat im Solorufer = 16 Punkte = 2 × 8, nicht 32.
- **In Negativspielen (Piccolo, Bettel, Ouvertspiele, Trischaken) gibt es überhaupt keine Prämien** [OÖ23 Negativ; LTA-GK1 Kap. 11].
- **In Farbenspielen** gibt es nur Alle Könige und Valat; Trull und Vögel werden nicht honoriert [LTA-GK1 Kap. 10, wörtlich für Farben-Solo **und** Farben-Dreier: „An Zusatzprämien sind nur alle Könige und Valat möglich“; LTA-F1 Kap. 8/9: „Trull und Vogerl werden nicht [gewertet]“; ÖF21 Ansagen: „Beim Farbensolo sind nur Vier Könige und Valat möglich“]. Gegenprobe in [MAIR-CUPS], Tabelle „Maximale Punkte pro Spiel“: Farbendreier mit allen stillen Zusätzen 12 = 9 (Spiel) + 3 (stille vier Könige) – für Trull und Vögel ist dort kein Platz.
- **Es gibt keine** Fänge (Mondfang, Sküsfang, Königfang), keinen Kaiserstich, keine Säcke/Absolut, keine Handprämien (Honneurs, Tarockansagen) [OÖ23 Allg. 3; Tarif].

### 5.2 Genaue Definitionen

- **Trull / Alle Könige** sind *Bestandsprämien*: Entscheidend ist, wo die Karten am Ende liegen. Stiche, Ablage und die der Partei zugefallenen Talonkarten zählen gleich (1.8).
- **Vögel** sind *Verlaufsprämien*: Die genannte Karte muss den genannten Stich **selbst stechen** [LTA-GK2 Kap. 1: „Der Ansagende muss das Vogerl selbst in der Karte haben und den Stich auch selbst machen“; MAIR-TURNIER: „Die Vögel müssen selber stechen“].
  - Stichnummern: Pagat = 12., Uhu = 11., Kakadu = 10., Quapil = 9. Stich.
  - Es genügt **nicht**, wenn der Partner den Stich gewinnt.
- **König ultimo:** Der gerufene König wird im letzten Stich gespielt und die Spielerpartei gewinnt den Stich; es genügt, wenn der **Partner** sticht [LTA-GK1 Kap. 4: „vom Spieler oder seinem Partner heimgestochen“; MAIR-TURNIER].
- **Valat:** Eine Partei gewinnt **alle zwölf** Stiche. Auch die Gegenpartei kann einen Valat erzielen oder ansagen (7.4).
- **Kein Kaiserstich:** Fallen Sk, XXI und I in einen Stich, gewinnt der Sküs [OÖ23 Allg. 3].

### 5.3 Wer darf wann was ansagen

**Ablauf:**

1. **Der Spieler** sagt nach dem Verlegen seine Prämien an und schließt mit „Ich liege“ [OÖ23 Allg. 2; LTA-GK1 Kap. 6].
   - **Solodreier:** Alle Ansagen müssen unmittelbar nach der Spielansage erfolgen, bevor „Ich liege“ gesagt wird [OÖ23 Ansagen 3].
   - **Besserrufer:** Der ursprüngliche Vogel (der schon vor der Talonaufnahme im Blatt war) **muss** angesagt werden [OÖ23 Ansagen 4; LTA-GK1 Kap. 10]. Ein weiterer, aus dem Talon gehobener Vogel darf zusätzlich angesagt werden [ÖF21 Rufen; LTA-GK1 Kap. 10].
2. **Reihum** ab dem Spieler nach dem Spieler (gegen den Uhrzeigersinn): „gut“, eigene Ansagen oder Kontra [LTA-GK1 Kap. 6: „der Partner kann Zusatzprämien ansagen, die Gegner können ‚gut‘ sagen oder kontrieren, sie können auch selbst Zusatzprämien ansagen“].
3. **Jeder Spieler darf nur einmal Prämien ansagen** [OÖ23 Ansagen 1].
4. **Ende der Runde:** **[EMPFEHLUNG]** Die Runde endet, wenn nach der letzten Ansage bzw. dem letzten Kontra drei Spieler hintereinander „gut“ gesagt haben. So ausdrücklich [MAIR-TURNIER] und [PAGAT]; [OÖ23 Allg. 2] verlangt nur, dass jede Ansage mit „gut“ zur Kenntnis genommen oder kontriert wird.

**Berechtigungen:**

| Ansage | Wer darf sie machen |
|---|---|
| Vögel (Pagat, Uhu, Kakadu, Quapil) | nur, wer die Karte selbst im Blatt hat. **Nicht für den Partner** [OÖ23 Ansagen 1] |
| König ultimo | nur, wer den gerufenen König im Blatt hat, also der Partner – oder der Spieler, wenn er den König aus dem Talon aufgenommen hat (4.5 a). Nicht für den Partner [OÖ23 Ansagen 1] |
| Trull, Alle Könige | jeder Spieler, auch „auf Verdacht“ ohne die Karten [ÖF21 Ansagen] |
| Valat | jeder Spieler; er sagt ihn für seine Partei an |
| Kontra / Rekontra / Subkontra | siehe 5.6 |

**Folgen falscher Ansagen** (am Tisch; die App verhindert sie, 3.7):
- Besserrufer ohne einen der vier Ansager im Blatt: Renonce, je 3 Punkte an die Mitspieler [OÖ23 Ansagen 4; OÖR25 B.4].
- Falscher Ansager beim Besserrufer (nicht im Blatt oder nur aus dem Talon gehoben) oder falscher König ultimo: Renonce [OÖ23 Ansagen 5].
- Falscher Ansager bei Rufer, Solorufer, Sechserdreier, Dreier, Solodreier: keine Renonce, nur diese Prämie ist verloren [OÖ23 Ansagen 5].

### 5.4 Pflichten aus einer Ansage (wirken als Zugbeschränkung)

**Wichtig – keine harte Sperre.** Eine angesagte Karte zu früh zu spielen ist nach OÖ-Regeln **erlaubt**; es kostet nur die Prämie. [OÖ23 Ansagen 6]: „Wird eine Ansage zum falschen Zeitpunkt gespielt, so gilt diese als verloren. Eine Renonce liegt aber nur dann vor, wenn sich ein Spieler dadurch einen offensichtlichen Vorteil verschafft (z. B. Pagat vorzeitig heimstechen, um Trull und Spiel zu retten).“ Gleichlautend [ÖF21 Ansagen]: „Wird ein angesagter Vogel zum falschen Zeitpunkt gespielt, so ist er verloren und darf nicht zurückgenommen werden, aber es ist keine Renonce, sofern der Spieler nicht gegen die Reihenfolge verstößt.“ Auch [OÖR25 B.2] führt „ein Vogerl wird zum falschen Zeitpunkt gespielt, ohne dass sich der Spieler dadurch einen offensichtlichen Vorteil verschafft“ und „angesagter König wird zu früh gespielt …“ ausdrücklich unter **keine Renonce**.

1. **Obliegenheit, nicht Verbot:** Ein angesagter Vogel bzw. der angesagte König ultimo *sollte* bis zu seinem Stich in der Hand bleiben; wer ihn früher spielt, verliert die Prämie (der Partner trägt den Verlust mit). Der striktere Wortlaut „Beide Ansagen dürfen nicht freiwillig aufgegeben werden“ [MAIR-TURNIER] gehört zum **Tiroler** Profil und wird selbst dort in [MAIR-AUSL] als bloßer Prämienverlust abgerechnet („ist die Prämie verloren … Es handelt sich aber nicht um eine Renonce“). (Korrektur K-1 in Abschnitt 12: Die Fassung vom 11.09.2026 sah hier eine harte Sperre vor.)
2. **Echtes Verbot nur im Vorteilsfall:** Renonce ist ausschließlich, **mit der angesagten Karte vor ihrem Zielstich einen Stich zu gewinnen** (der in [OÖ23 Ansagen 6] genannte Fall „Pagat vorzeitig heimstechen, um Trull und Spiel zu retten“; [ÖF21 Ansagen] ergänzt „mit angesagtem König vorzeitig einstechen“). Die App sperrt diesen Zug **nur dann**, wenn der Stichgewinn schon aus öffentlich bekannten Tatsachen feststeht (z. B. Ausspiel des Pagats, nachdem alle höheren Tarock gefallen sind). Ist es unsicher, bleibt der Zug erlaubt – sonst würde die Ablehnung verdeckte Information verraten (10.8). Gewinnt die Karte dann tatsächlich vorzeitig, ist die Prämie verloren; die am Tisch fällige Renonce bildet die App nicht nach (7.9).
3. **Spielpflicht im richtigen Stich:** Im vorgesehenen Stich ist die angesagte Karte zu spielen, sobald das regelkonform möglich ist [KTC: „muss versuchen, die entsprechende Karte zum vorgeschriebenen Stich zu spielen“; STMK]. **[EMPFEHLUNG]**, in [OÖ23] nicht ausdrücklich; die App erzwingt es (Verzicht wäre ohnehin nur Prämienverlust).
4. **Zwangsweise vorzeitige Abgabe:** Muss wegen Tarockzwang einer von mehreren angesagten Vögeln vorzeitig gespielt werden, ist **zuerst der höhere Vogel** aufzugeben. Ein Verstoß ist Renonce [OÖ23 Ansagen 7; ÖF21: „zuerst Quapil, dann Kakadu, dann Uhu und zuletzt Pagat“]. Das ist eine **harte** Regel und wird erzwungen. Sie gilt nach dem Wortlaut nur für **Vögel**; der angesagte König ultimo ist nicht in die Reihenfolge einbezogen (**[OFFEN] P-12**).
5. **Verhinderung durch Farbzwang:** Kann die angesagte Karte im vorgesehenen Stich wegen Farbzwang nicht gespielt werden, ist die Prämie verloren; danach darf die Karte jederzeit gespielt werden [KTC; STMK]. **[EMPFEHLUNG]**, da [OÖ23] dazu schweigt.
6. **Solidarität:** Der Partner trägt den Verlust mit [ÖF21 Ansagen; OÖ23 Allg. 4].

Umsetzung als Zugregel in 6.5, Fehlertexte in 11.4.

### 5.5 Still oder angesagt

- **Still** heißt: keine Ansage, die Prämie wird einfach erzielt; sie zählt die Hälfte [OÖ23 Ansagen 1].
- **Angesagt** zählt den vollen Tarifwert, kann aber verloren gehen und ist kontrierbar.
- **In Solospielen** zählen alle Prämien doppelt, still wie angesagt [OÖ23 Ansagen 1]. Beispiel: stiller Uhu im Solorufer = 2 × 2 = 4 Punkte [LTA-GK1 Kap. 10].
- **Beim verlorenen Sechserdreier** verdoppelt sich nur das **Spiel**, nicht die Prämien [STMK ausdrücklich: „Ein verlorener Sechserdreier zählt doppelt (nur das Spiel, nicht die Prämien)“; nachgerechnet in [OÖR25 B.4]: Renonce eines Gegenspielers beim Sechser-Dreier mit angesagtem Uhu → je Gegner 8 (Spiel) + 4 (Uhu); PEPI].
- **Beim Valat** zählen nur **angesagte** Prämien, stille nicht – auch beim verlorenen Valat [OÖ23 Ansagen 2; ÖF21 Valat; LTA-GK1 Kap. 12].
- **[OFFEN]** Ob ein misslungener **stiller** Vogel bzw. König ultimo negativ zählt, sagt [OÖ23] nicht. Siehe 9, Punkt P-5; Empfehlung: **nein** (stille Prämien zählen nur, wenn sie gelingen).

### 5.6 Kontra, Rekontra, Subkontra

- **Stufen:** Kontra (× 2), Rekontra (× 4), Subkontra (× 8). Weitere Stufen (Hirsch-, Mordkontra) gibt es nicht [OÖ23 Allg. 5].
- **Wirkung:** „Ein Kontra wirkt sich nur auf das Geld, aber nicht auf die Punktewertung aus.“ [OÖ23 Allg. 5] Die Schrift (Turnierwertung) bleibt unberührt; siehe 7.1 und 8.2.
- **Positive Spiele:** Das Kontra gilt automatisch für alle Mitspieler, also für beide Gegner und beide Partner [OÖ23 Allg. 5].
- **Negativspiele:** Jeder Gegner kontriert für sich [OÖ23 Allg. 5; LTA-GK1 Kap. 8]. Es entstehen bis zu drei getrennte Abrechnungen Spieler–Gegner.
- **Trischaken:** kein Kontra [OÖ23 Allg. 5].
- **Getrennt kontrierbar** sind das Spiel und **jede einzelne** angesagte Prämie; man kann auch „alles“ schießen [LTA-GK1 Kap. 8; LTA-GK2 Kap. 4].
- **Zeitpunkt:** erst, nachdem der Spieler „Ich liege“ gesagt hat [LTA-GK1 Kap. 8]. Ein Kontra vor der Talonaufnahme („Mutterleib“) gibt es nicht [STMK].
- **Wer:** Gegner kontrieren; der Spieler oder sein Partner geht „retour“ (Rekontra); die Gegner können „nochmals spritzen“ (Subkontra) [LTA-GK1 Kap. 8].
- **Solorufer mit gerufenem König im Talon:** Jedes Kontra ist wirkungslos [OÖ23 Allg. 7].
- **Schleifen:** kein Kontra [LTA-GK1 Kap. 10].
- **[EMPFEHLUNG] Kontra unter Partnern zählt nicht.** Weil die Partnerschaft verdeckt ist, kann ein Gegner die Ansage seines eigenen Partners kontrieren. Stellt sich das heraus, zählt das Kontra nicht und wird nicht ausbezahlt [KTC Prämien; STMK Prämien; MAIR-AUSL]. [OÖ23] schweigt dazu.
- **[EMPFEHLUNG] Zeitfenster:** Ein Kontra gegen Spiel oder eine Prämie muss beim ersten eigenen Zug nach dieser Ansage erfolgen; danach nur noch gegen später erfolgte Ansagen [KTC Prämien; STMK Prämien]. Das macht den Automaten eindeutig.

### 5.7 Ansage- und Kontraphase als Zustandsautomat

```
// Ausgangspunkt: Spiel und (bei Ruferspielen) gerufene Farbe stehen fest, Talon ist erledigt.
offen = { SPIEL } ∪ {}                        // kontrierbare Posten
ansagenErlaubt(p) = (positives Spiel) ∧ (p hat noch nicht angesagt)

Schritt 0 (Spieler):
   Solodreier: Ansagen wurden bereits mit dem Gebot gemacht.
   sonst: Spieler sagt 0..n Prämien an (Besserrufer: Pflichtvogel dabei), dann "Ich liege".
   jede Ansage -> Posten in 'offen' mit (typ, partei, stufe=1(angesagt))

Schleife ab dem Spieler nach dem Spieler, zyklisch:
   Aktionen von p:
     a) "gut"
     b) Prämien ansagen  (nur einmal pro Spieler; Vögel/König ultimo nur mit Karte im Blatt;
                          Farbenspiel: nur Alle Könige/Valat; Negativspiel: keine)
     c) Kontra auf einen Posten, der von der Gegenpartei stammt und noch auf Stufe 1 steht
        (Negativspiel: nur eigener Posten Spieler–p)
     d) Rekontra auf einen Posten der eigenen Partei, der auf Stufe 2 steht
     e) Subkontra auf einen Posten der Gegenpartei, der auf Stufe 4 steht
   nach b)–e) wird der Zähler "gut in Folge" zurückgesetzt.
   Ende, wenn drei Spieler hintereinander "gut" gesagt haben.

Nebenbedingungen:
   Farbenspiel: ein Gegner darf entweder kontrieren oder den Valat ansagen, nicht beides [OÖ23 Farben 4].
   Solorufer mit König im Talon: alle Stufen werden am Ende auf 1 zurückgesetzt.
   Partner-Kontras werden am Ende neutralisiert (Stufe zurück auf 1), siehe 5.6.
```

---

## 6. Spielregeln im Stich

### 6.1 Ausspiel und Stichfolge

- **Erster Stich:** In allen **positiven** Spielen einschließlich der Farbenspiele spielt die **Vorhand** aus, gleichgültig wer Spieler ist [OÖ23 Farben 1; LTA-GK1 Kap. 6]. In allen **Negativspielen** (Trischaken, Piccolo, Bettel, Ouvertspiele) spielt der **Spieler** aus [OÖ23 Negativ 1; LTA-GK1 Kap. 6].
- Danach spielt immer der Gewinner des letzten Stichs aus [WIKI].
- Es werden zwölf Stiche zu je vier Karten gespielt.

### 6.2 Grundzwänge (alle Spiele)

1. **Farbzwang (Bedienzwang):** Wer die ausgespielte Farbe hat, muss sie bedienen. Wird Tarock ausgespielt, muss Tarock zugegeben werden [OÖ23 Allg. 3; LTA-GK1 Kap. 8; WIKI].
2. **Tarockzwang:** Wer die ausgespielte Farbe nicht (mehr) hat, **muss Tarock zugeben** [OÖ23 Allg. 3].
3. Wer weder die Farbe noch Tarock hat, gibt eine beliebige Karte zu [LTA-GK1 Kap. 8].
4. **Kein Stichzwang in positiven Spielen** – man darf beliebig unterstehen („laschieren“) [LTA-GK1 Kap. 8]. **Stichzwang nur in Negativspielen** [OÖ23 Allg. 3].
5. Ein **Überstechzwang bei Tarock** besteht in positiven Spielen **nicht**.

### 6.3 Farbenspiele (Farbendreier, Farbensolo)

Zusätzlich zu 6.2 [OÖ23 Farben 1–2; LTA-F1 Kap. 8]:

- **Tarock stechen keine Farbkarte.** Den Stich gewinnt die höchste Karte der ausgespielten Farbe.
- Tarock stechen nur innerhalb von Tarock: Wurde Tarock ausgespielt, gewinnt das höchste Tarock.
- **Ausspielverbot:** Tarock darf erst ausgespielt werden, wenn man **keine Farbkarte mehr** im Blatt hat.
- Farbzwang und Tarockzwang gelten unverändert: Wer eine Farbe nicht bedienen kann, **muss Tarock zugeben** (das den Stich nicht gewinnt); wer auch kein Tarock hat, gibt eine beliebige Farbkarte zu.
- **Kein Stichzwang**; der **Pagat** unterliegt keiner Sonderregel [OÖ23 Farben 1].
- Prämien: nur Alle Könige und Valat (5.1).

### 6.4 Negativspiele

**Gemeinsam** (Piccolo, Bettel, Piccolo ouvert, Bettel ouvert, Trischaken) [OÖ23 Negativ 1–3; Trischaken 1]:

- **Stichzwang:** Man muss die höchste im Stich liegende Karte überstechen, sofern das unter Beachtung des Farbzwangs möglich ist [WIKI; LTA-GK1 Kap. 8].
  - Ist die Farbe bedienbar: mit einer höheren Karte derselben Farbe, wenn vorhanden; sonst beliebige Karte dieser Farbe. Liegt bereits ein Tarock im Stich und war eine Farbe ausgespielt, kann man mit einer Farbkarte nicht überstechen – dann genügt irgendeine Karte der Farbe.
  - Ist die Farbe nicht bedienbar: Tarockzwang; liegt bereits ein Tarock, muss man es überstechen, wenn möglich, sonst genügt ein beliebiges Tarock.
- **Pagat-Regel:** Der Pagat darf nur gespielt (ausgespielt oder zugegeben) werden, wenn er das **letzte Tarock** in der Hand ist [OÖ23 Negativ 2; Trischaken 1].
  - Diese Regel geht dem Stichzwang vor: Könnte man nur mit dem Pagat überstechen, spielt man ein anderes Tarock.
- **Kein Talon**, keine Prämien, Kontra einzeln.
- **Ouvertspiele:** Nach dem ersten Stich und **vor dem zweiten Ausspiel** legen **alle** Spieler ihre Karten offen auf den Tisch. Die drei Gegenspieler dürfen sich über die Spielstrategie beraten [OÖ23 Negativ 3].
  - **[EMPFEHLUNG]** Im LAN ist für die drei Gegner ab diesem Zeitpunkt ein Team-Chat freigeschaltet; der Spieler sieht ihn nicht. Gegen Computerspieler entfällt das.

**Trischaken** zusätzlich [OÖ23 Trischaken 1]:

- „Es besteht Farb- und Stichzwang, aber **es muss nicht austarockiert werden**.“ Erläuterung: Es wird „bunt gemischt“ gespielt; jeder Ausspieler darf frei wählen, ob er Tarock oder eine Farbe ausspielt [LTA-GK1 Kap. 11]. Es gibt keine Pflicht, zuerst die Tarock herauszuziehen.
- Der Pagat darf nur als letztes Tarock gespielt werden.
- Den Talon erhält der Spieler mit dem **letzten Stich**; er zählt zu dessen Punkten.
- Gezählt wird **blattgenau** (1.5).
- Es spielt jeder gegen jeden; kein Kontra, keine Prämien, kein Partner.

### 6.5 Zugbeschränkungen aus Ansagen (nur positive Spiele)

Aus 5.4 folgen Zugregeln. Nur zwei davon sind **harte** Sperren, die restlichen sind Warnungen:

- Eine **reservierte** Karte ist ein angesagter Vogel (Zielstich 9/10/11/12) oder der angesagte König ultimo (Zielstich 12).
- **HART – Stichgewinn vor dem Zielstich:** Ein Zug, mit dem eine reservierte Karte einen Stich **gewinnen** würde, bevor ihr Zielstich erreicht ist, wird abgelehnt (5.4 Punkt 2; einziger in [OÖ23 Ansagen 6] als Renonce benannter Fall). Die Prüfung stützt sich **ausschließlich auf öffentlich bekannte Karten** (gefallene Karten, offen gezeigte Ablage, offen gelegene Talonhälfte, eigenes Blatt): Ausspielen des Pagats bzw. des gerufenen Königs ist gesperrt, wenn keine Karte, die ihn schlagen könnte, mehr im Spiel sein kann; beim Zugeben als letzter Spieler im Stich ist es gesperrt, wenn die reservierte Karte den Stich damit sicher gewinnt. **Im Zweifel ist der Zug erlaubt** – eine Ablehnung, die fremde Handkarten voraussetzt, würde verdeckte Information verraten (10.8).
- **HART – Reihenfolge:** Bleiben nur reservierte Vögel erlaubt (typischer Fall: Tarockzwang, in der Hand nur noch angesagte Vögel), ist der **höchste** Vogel zu spielen [OÖ23 Ansagen 7].
- **HART – im Zielstich:** Im Zielstich muss die reservierte Karte gespielt werden, sobald sie regelkonform gespielt werden darf (5.4 Punkt 3).
- **WARNUNG – sonstiges vorzeitiges Weggeben:** Jeder andere vorzeitige Zug mit einer reservierten Karte ist **erlaubt**. Die App zeigt vorher einmalig „Damit ist {Prämie} verloren – trotzdem spielen?“ und verbucht die Prämie anschließend als verloren. Im Lernmodus erscheint zusätzlich der Hinweistext aus 11.4.
- Kann sie im Zielstich wegen Farbzwang nicht gespielt werden, ist die Prämie verloren und die Reservierung entfällt (5.4 Punkt 5).
- Optionsschalter `harteVogelreservierung` (Standard **aus**) für Runden, die nach [MAIR-TURNIER] streng spielen wollen.

### 6.6 Stichgewinner

```
stichGewinner(stich, art):
   lead = stich[0]
   wenn art == FARBENSPIEL und lead ist Farbkarte:
        kandidaten = Karten der Farbe von lead
   sonst:
        tarocke = Tarock im Stich
        kandidaten = tarocke, wenn vorhanden, sonst Karten der Farbe von lead
   Gewinner = Karte mit der höchsten Stichkraft in kandidaten (Tarock: Zahl, Sk = 22;
              Farbe: K > D > R > B > 1 > 2 > 3 > 4 bzw. K > D > R > B > 10 > 9 > 8 > 7)
```

Es gibt **keinen Kaiserstich** und keine Fangprämien [OÖ23 Allg. 3].

### 6.7 Erlaubte Karten (vollständiger Algorithmus)

```
erlaubteKarten(hand, stich, art, reserviert, stichNr):
   // art ∈ {TAROCKSPIEL, FARBENSPIEL, NEGATIV}
   // reserviert: Karte -> Zielstich (nur TAROCKSPIEL)

   wenn stich leer:                                  // Ausspiel
       wenn art == FARBENSPIEL:
            F = Farbkarten in hand
            L = (F nicht leer) ? F : hand
       sonst:
            L = hand
       wenn art == NEGATIV: L = ohnePagatWennNichtLetztesTarock(L, hand)
   sonst:
       lead = stich[0]
       bedienen = { c ∈ hand | c.farbe == lead.farbe }     // Tarock gilt als eigene "Farbe"
       wenn bedienen nicht leer:
            L = bedienen
            wenn art == NEGATIV:
                 top = aktuell stichführende Karte
                 wenn top.farbe == lead.farbe:
                      hoeher = { c ∈ L | staerker(c, top) }
                      wenn lead ist Tarock: hoeher = ohnePagatWennNichtLetztesTarock(hoeher, hand)
                      wenn hoeher nicht leer: L = hoeher
                 wenn lead ist Tarock: L = ohnePagatWennNichtLetztesTarock(L, hand)
       sonst wenn Tarock in hand:
            L = Tarock in hand                              // Tarockzwang, auch im Farbenspiel
            wenn art == NEGATIV:
                 tarockeImStich = Tarock in stich
                 wenn tarockeImStich nicht leer:
                      hoeher = { c ∈ L | staerker(c, hoechstes(tarockeImStich)) }
                      hoeher = ohnePagatWennNichtLetztesTarock(hoeher, hand)
                      wenn hoeher nicht leer: L = hoeher
                 L = ohnePagatWennNichtLetztesTarock(L, hand)
       sonst:
            L = hand                                        // weder Farbe noch Tarock

   wenn art == TAROCKSPIEL und reserviert nicht leer:       // 6.5
       faellig = { c ∈ L | reserviert[c] == stichNr }
       wenn faellig nicht leer: return faellig            // HART: Spielpflicht im Zielstich
       // HART: eine reservierte Karte darf vor ihrem Zielstich keinen Stich gewinnen
       L2 = { c ∈ L | nicht (reserviert[c] > stichNr und wuerdeStichSicherGewinnen(c, stich, hand)) }
       wenn L2 nicht leer: L = L2            // Zwangslage bleibt erlaubt (sonst gäbe es keinen Zug)
       frei = { c ∈ L | c ∉ reserviert oder reserviert[c] < stichNr }
       wenn frei leer:
            // Zwangsfall: es sind nur noch angesagte Karten spielbar -> OÖ23 Ansagen 7
            voegel = { c ∈ L | c ist Vogel (I..IIII) }
            wenn voegel nicht leer: return { höchster Vogel in voegel }
            return L
       wenn harteVogelreservierung: return frei           // Option, Standard aus
       return L                                           // Standard: vorzeitiges Weggeben erlaubt (Warnung in der UI)
   return L

wuerdeStichSicherGewinnen(c, stich, hand):
   // true NUR, wenn sich der sichere Stichgewinn aus ÖFFENTLICH bekannten Tatsachen ergibt:
   //   c schlägt alle bereits liegenden Karten, UND
   //   keine noch nicht gespielte Karte, die c schlagen könnte, ist überhaupt noch im Spiel
   //   (bekannt sind: gefallene Karten, offen gezeigte Ablage, die allen aktiven Spielern
   //    gezeigte liegen gelassene Talonhälfte, das eigene Blatt).
   // Sonst false. Der Test darf NIE auf fremde Handkarten zugreifen, sonst verrät die
   // Ablehnung eines Zuges verdeckte Information (10.8).

ohnePagatWennNichtLetztesTarock(menge, hand):
   wenn Anzahl Tarock in hand > 1: return menge ohne Pagat
   sonst: return menge
```

Hinweise:
- In positiven Tarockspielen gibt es weder Stichzwang noch Pagat-Beschränkung; die einzige zusätzliche Einschränkung kommt aus Ansagen (6.5).
- `reserviert` enthält nur **angesagte** Vögel und den **angesagten** König ultimo. Für **stille** Prämien gibt es keinerlei Zugbeschränkung.
- In Farbenspielen gilt der Tarockzwang unverändert, das Tarock gewinnt den Stich aber nicht (6.3).
- Die Funktion liefert nie eine leere Menge.

### 6.8 Spielende

- Regulär nach zwölf Stichen.
- **[EMPFEHLUNG] Vorzeitiges Ende** ist nur zulässig, wenn das Ergebnis **aus öffentlich sichtbaren Tatsachen** feststeht, damit keine verdeckte Information verraten wird:
  - **Bettel / Bettel ouvert:** sobald der Spieler einen Stich gemacht hat (verloren).
  - **Piccolo / Piccolo ouvert:** sobald der Spieler den zweiten Stich gemacht hat (verloren). Gewonnen kann er erst nach dem zwölften Stich sein.
  - **Positive Spiele laufen immer bis zum zwölften Stich**, weil Prämien offen sind: „Das Spiel ist somit nicht beendet, wenn die Gegner einen Stich machen“ (angesagter Valat) [OÖ23 Ansagen 2].
  - Grundlage: [WIKI] („bis entweder alle zwölf Stiche gespielt sind oder das angesagte Spielziel bereits gescheitert ist“). Im Lernmodus kann „bis zum Ende ausspielen“ eingeschaltet werden.
- Danach: Zählen (1.5), Prämien prüfen, Abrechnung (7).

---

## 7. Abrechnung

### 7.1 Grundsätze

1. **Zwei Konten.** Jede Abrechnung erzeugt zwei Werte je Spieler:
   - **Schrift** (Turnierwertung): **ohne** Kontras [OÖ23 Allg. 5].
   - **Geld** (Auszahlung): **mit** Kontrafaktor; üblich 1 Punkt = 10 Cent [LTA-GK1 Kap. 8; KTC].
   Beide sind Nullsummen über die vier aktiven Spieler.
2. **Posten.** Abgerechnet werden getrennt: das **Spiel** und **jede** Prämie [LTA-GK1 Kap. 8]. Jeder Posten hat einen Wert `v` (Punkte je Spieler), eine Gewinnerpartei und einen Kontrafaktor `k ∈ {1,2,4,8}` (nur Geld).
3. **Zahlungsweise je Posten:**
   - **Partnerspiel (2 gegen 2):** Jeder Spieler der Gewinnerpartei erhält `+v`, jeder der Verliererpartei `−v` [PAGAT].
   - **Alleinspiel (1 gegen 3):** Der Alleinspieler erhält `±3·v`, jeder Gegner `∓v` [PAGAT; LTA-F1 Kap. 6: „Man spielt alleine, erhält 12 Punkte“ beim Dreier].
   - Für Geld gilt derselbe Betrag mal `k`.
4. **Solidarität der Partner:** Was einer gewinnt oder verliert, gewinnt bzw. verliert auch sein Partner; Spielfehler des Partners muss man mittragen [OÖ23 Allg. 4; PAGAT].
5. **Fünfertisch:** Der aussetzende Geber bekommt 0 Punkte und 0 Geld.
6. **Rundung:** Es gibt keine. Alle Werte sind ganzzahlig, außer bei der Aufteilung von Trischaken-Punkten unter zwei Jungfrauen (7.6).

### 7.2 Spielwertung der positiven Spiele

| Spiel | Wert `v` gewonnen | Wert `v` verloren | Zahlungsweise |
|---|---|---|---|
| Rufer | 1 | 1 | 2 gegen 2 (ohne Partner: 1 gegen 3) |
| Solorufer | 2 | 2 | 2 gegen 2 (ohne Partner: 1 gegen 3) |
| Besserrufer | 1 | 1 | 2 gegen 2 (ohne Partner: 1 gegen 3) |
| Farbendreier | 3 | 3 | 1 gegen 3 |
| Sechserdreier | 4 | **8** | 1 gegen 3 |
| Dreier | 4 | 4 | 1 gegen 3 |
| Farbensolo | 6 | 6 | 1 gegen 3 |
| Solodreier | 8 | 8 | 1 gegen 3 |

- Gewonnen ist das Spiel mit ≥ 35/2 = 107 Dritteln der Spielerpartei (1.6).
- **Nur beim Sechserdreier** zählt der Verlust doppelt [OÖ23 Tarif „Sechserdreier 4“ mit ÖF21 „Beim Verlust des Sechserdreiers zählt das Spiel doppelt“; LTA-GK1 Kap. 10]. Prämien bleiben einfach.
- Beispiele: Dreier gewonnen → Spieler +12, jeder Gegner −4. Sechserdreier verloren → Spieler −24, jeder Gegner +8 [OÖR25 B.4].

### 7.3 Prämienwertung

```
wert(prämie, angesagt, spiel):
    v = tarif(prämie)                 // Trull 2, Alle Könige 2, König ultimo 2,
                                      // Pagat 2, Uhu 4, Kakadu 6, Quapil 8
    wenn nicht angesagt: v = v / 2    // still zählt die Hälfte  [OÖ23 Ansagen 1]
    wenn spiel ∈ {Solorufer, Solodreier, Farbensolo}: v = v * 2
    return v
```

- **Gewinner des Postens:** die Partei, die die Prämie erzielt. Eine **angesagte** Prämie, die nicht erzielt wird, zahlt die ansagende Partei an die Gegenpartei (gleicher Betrag).
- Prämien der **Gegenpartei** sind ebenso möglich: Gewinnen die Gegner still die Trull, erhalten sie sie [LTA-GK1 Kap. 6].
- **Pro Prämienart wird nur ein Posten abgerechnet.** Ist sie angesagt, zählt nur das angesagte Ergebnis; sonst das stille. Dieselbe Prämienart kann nur von einer Partei angesagt werden (Vögel und König ultimo setzen den Kartenbesitz voraus, Trull und Alle Könige sind gegenläufig nicht sinnvoll). **[EMPFEHLUNG]**, siehe 9 Punkt P-6.
- **Stille Prämien** zählen nur, wenn sie gelingen (**[OFFEN]**, siehe 9 Punkt P-5).
- **Beim Valat** zählen nur angesagte Prämien (7.4).
- In Farbenspielen kommen nur Alle Könige und Valat vor (5.1).
- Beispiele:
  - Solorufer, angesagter König ultimo gewonnen: 2 × 2 = 4 je Spieler der Spielerpartei [OÖR25 B.4: „je 4 Punkte für den angesagten König“].
  - Sechserdreier, angesagter Uhu gewonnen: 4 je Gegner, also +12 für den Spieler; stiller Kakadu: 3 je Gegner [OÖR25 B.4].
  - Solorufer, stiller Uhu: 2 × 2 = 4 [OÖR25 B.4].

### 7.4 Valat

- **Angesagter Valat:** Der Posten „Spiel“ wird durch `v = Spielwert × 8` ersetzt; **stiller Valat:** `v = Spielwert × 4` [OÖ23 Ansagen 2].
- Beim **verlorenen angesagten Valat** ist damit auch das Spiel verloren, selbst bei 35/2 oder mehr: Die ansagende Partei zahlt den achtfachen Spielwert [ÖF21 Valat; LTA-GK1 Kap. 12].
- **Beim Sechserdreier** wirkt die Verdoppelung des verlorenen Spiels zusätzlich: verlorener angesagter Valat = 4 × 8 × 2 = 64 je Gegner [ÖF21 Valat: „beim Sechserdreier 8 mal 8 Punkte“; MAIR-CUPS: Valat verloren Sechserdreier −192 für den Spieler].
- **Prämien beim Valat:** Es zählen nur **angesagte** Prämien, stille nicht – bei gelungenem wie verlorenem Valat [OÖ23 Ansagen 2; ÖF21 Valat]. Angesagte Prämien werden dem Spieler gutgeschrieben bzw. gegengerechnet („Da sich der Valat nur auf das Spiel bezieht, gelten andere Ansagen nicht automatisch als verloren“ [OÖ23 Ansagen 2]).
- **Weiterspielen:** Nach einem gescheiterten Valat wird das Spiel zu Ende gespielt, weil die übrigen Ansagen noch offen sind [OÖ23 Ansagen 2].
- **Kontra:** Ein Kontra gegen die Valatansage ist möglich [LTA-GK1 Kap. 12]. „Wird der angesagte Valat geschossen, so bedeutet das: Spiel × 8 × 2, handelt es sich um einen verlorenen Sechser, so nochmals × 2.“ „Wird ein stiller Valat gespielt: Spiel × 4; war das Spiel kontriert, dann zusätzlich × 2.“ [LTA-F1 Kap. 1] Ein Kontra auf das Spiel wirkt also auf den Valatposten weiter.
- **Valat der Gegenpartei:** Gewinnen die drei Gegner alle Stiche, zahlt die Spielerpartei den vier- bzw. achtfachen Spielwert; beim Sechserdreier zusätzlich verdoppelt [LTA-F1 Kap. 1]. Sagen die Gegner den Valat an und erreichen ihn nicht, zahlen sie den achtfachen Spielwert (**[EMPFEHLUNG]**, spiegelbildlich; siehe 9 Punkt V-2).
- **Farbenspiele:** Als Gegenspieler darf man entweder kontrieren oder den Valat ansagen [OÖ23 Farben 4].

### 7.5 Negativspiele

- Werte: Piccolo 2, Bettel 2, Piccolo ouvert 6, Bettel ouvert 7 [OÖ23 Tarif].
- Immer 1 gegen 3: gewonnen +3·v für den Spieler, je −v für die Gegner; verloren umgekehrt.
- **Kontra einzeln:** Jeder Gegner hat seinen eigenen Faktor `k`. Für das Geld wird die Abrechnung Spieler–Gegner einzeln mit `k` multipliziert [OÖ23 Allg. 5].
  - Beispiel: Bettel ouvert (7) verloren, B kontriert (×2), C kontriert und der Spieler geht retour (×4), D sagt nichts.
    Schrift: Spieler −21, B/C/D je +7. Geld: Spieler −(14+28+7) = −49; B +14, C +28, D +7.
- Prämien gibt es nicht; Piccolo verlangt **genau** einen, Bettel **keinen** Stich.

### 7.6 Trischaken

Grundlage: [OÖ23 Trischaken 2–3], ergänzt um [ÖF21 Trischaken] (Dreifach-Gleichstand) und [LTA-GK1 Kap. 11].

- Gezählt wird blattgenau; der Talon gehört zum letzten Stich (1.8).
- **Verlierer** ist, wer die meisten Punkte hat.
- Grundbetrag `b`:
  - normaler Verlierer: `b = 1`;
  - Verlierer mit „dem Spiel“ (≥ 35/2 = 107 Drittel, „Bürgermeister“): `b = 2`;
  - ist der **Trischaker** (die Vorhand) Verlierer oder punktegleich mitbeteiligt, verliert er **allein** und zahlt doppelt: `b = 2`, mit ≥ 35/2 sogar `b = 4`.
- Ausgeschüttet werden `3 · b` Punkte.
- **Jungfrau** (kein Stich): Eine Jungfrau zieht die gesamte Ausschüttung ein; zwei Jungfrauen **teilen** sie [OÖ23 Trischaken 3: „Hat ein Spieler keinen Stich (‚Jungfrau‘), bekommt er den gesamten Gewinn. Haben zwei Spieler keinen Stich, teilen sie sich den Gewinn“].
- **Sonderfall** „35/1 : 34/2 und zwei Jungfrauen“: Der Verlierer zahlt 2 × 2 = 4 [OÖ23 Trischaken 3], also 2 je Jungfrau statt 1,5. Das ist zugleich die allgemeine Auflösung für `b = 1` mit zwei Jungfrauen (3 lässt sich nicht halbieren).
- **[ABWEICHUNG]** [LTA-GK1 Kap. 11] schreibt dagegen: „Haben zwei Spieler keinen Stich, so erhalten sie vom Verlierer je drei Punkte“ (also 6 statt 3/4), und [WIKI] nennt ebenfalls „je 30 Cent … auch wenn er nur 35/1 hat“. Das widerspricht dem ausdrücklichen Wortlaut von [OÖ23 Trischaken 3] („teilen sie sich“) samt dessen Rechenbeispiel. **Festlegung: [OÖ23] gilt** (Primärquelle); Option `jungfrauenVoll` (Standard aus) für Runden nach [LTA-GK1].
- **Punktegleichheit ohne Trischaker:** zwei Gleiche verlieren je 2 Punkte (auch bei 35 : 35) [OÖ23 Trischaken 2]; drei Gleiche verlieren je 1 Punkt [ÖF21 Trischaken].
- Kein Kontra, keine Prämien.

```
trischakenAbrechnung(drittel[4], trischaker T):
    max = größter Wert;  L = { p | drittel[p] == max };  J = { p | drittel[p] == 0 }
    wenn T ∈ L:
         b = (drittel[T] >= 107) ? 4 : 2
         zahle(T, −3b);  empfänger = (J nicht leer) ? J : alle außer T
         verteile 3b gleichmäßig an empfänger
    sonst wenn |L| == 1:
         l = L[0];  b = (drittel[l] >= 107) ? 2 : 1;  gesamt = 3b
         wenn J nicht leer:
              wenn |J| == 2 und b == 1: gesamt = 4      // OÖ-Sonderfall
              zahle(l, −gesamt); verteile gesamt gleichmäßig an J
         sonst:
              zahle(l, −gesamt); jeder andere +b
    sonst wenn |L| == 2:
         beide Verlierer je −2; die beiden anderen erhalten zusammen 4
              (ist genau einer davon Jungfrau, erhält er alle 4; sonst je 2)
    sonst wenn |L| == 3:
         jeder Verlierer −1; der vierte Spieler +3
```

Hinweis: Vier Punktegleiche kann es nicht geben, weil 210 nicht durch 4 teilbar ist.

**Beispiele** (Angaben in Dritteln, Summe immer 210; alle Zeilen nachgerechnet):

| Fall | T | B | C | D | Ergebnis (Schrift) |
|---|---|---|---|---|---|
| Jungfrau D, Verlierer B (30/1) | 60 | 91 | 59 | 0 | T 0, B −3, C 0, **D +3** |
| Trischaker Bürgermeister (36/0) | 108 | 40 | 40 | 22 | **T −12**, B/C/D je +4 |
| zwei Punktegleiche (25/0) | 36 | 75 | 75 | 24 | B −2, C −2, T +2, D +2 |
| 35/1 : 34/2, zwei Jungfrauen | 0 | 106 | 104 | 0 | **B −4**, T +2, D +2, C 0 |
| 35/2 : 34/1, zwei Jungfrauen | 0 | 107 | 103 | 0 | **B −6**, T +3, D +3, C 0 |
| drei Punktegleiche (20/0) | 30 | 60 | 60 | 60 | B/C/D je −1, **T +3** |
| Trischaker punktegleich (25/0) | 75 | 75 | 30 | 30 | **T −6**, B/C/D je +2 |
| Bürgermeister B (36/2) | 50 | 110 | 30 | 20 | **B −6**, T/C/D je +2 |
| 35 : 35, zwei Jungfrauen | 0 | 105 | 105 | 0 | B −2, C −2, T +2, D +2 |

### 7.7 Sonderfälle der Abrechnung

**a) Schleifen (gerufener König im Talon, Rufer oder Besserrufer).**
- Der Spieler zahlt „Spiel und Ansage“ [OÖ23 Allg. 7], konkret „Spiel plus Ansager (3, 5, 7 oder 9 Punkte)“ [ÖF21 Rufen]. Die genannten Beträge sind 1 + 2 (Pagat), 1 + 4 (Uhu), 1 + 6 (Kakadu), 1 + 8 (Quapil); beim einfachen Rufer ist es 1.
- Dass diese Zahlen **je Mitspieler** gemeint sind, zeigt die Parallelstelle im selben Text: Beim Besserrufer ohne Vogel „verliert [er] an alle 3 Spieler Spiel + Pagat (3 Punkte)“ [ÖF21 Rufen], und [OÖR25 B.4] rechnet denselben Fall als „jeder reguläre Spieler 3 Punkte, der Renoncierende 9 Minuspunkte“.
- Weil kein Partner existiert, zahlt er **an alle drei** Mitspieler: `Spieler = −3·(Spielwert + Vogelwert)`, jeder andere `+ (Spielwert + Vogelwert)`.
  - Beispiel Besserrufer mit Uhu: Spieler −15, alle anderen je +5.
  - Beispiel Rufer: Spieler −3, alle anderen je +1.
- Kein Kontra, keine weiteren Prämien [LTA-GK1 Kap. 10].
- **[OFFEN]** Welcher Vogel gilt beim Schleifen, wenn der Spieler mehrere im Blatt hat? Siehe 9 Punkt R-2. **[EMPFEHLUNG]** derjenige, den er als ursprünglichen Ansager benennt; die App verlangt die Benennung vor dem Schleifen und schlägt den niedrigsten vor.

**b) Rufer oder Besserrufer allein weitergespielt** (König lag im Talon): normale Abrechnung, aber **1 gegen 3**; Kontras zählen; Prämien wie üblich.

**c) Solorufer mit gerufenem König im Talon:** 1 gegen 3, der Spieler erhält die Talonhälfte mit dem König; **alle Kontras sind wirkungslos**, gewonnen wie verloren [OÖ23 Allg. 7; LTA-F1 Kap. 3].

**d) Vorzeitiges Spielende** (Bettel/Piccolo, 6.8): normale Wertung des Negativspiels.

### 7.8 Kontra und Geldkonto

| Stufe | Faktor |
|---|---|
| ohne | 1 |
| Kontra („gespritzt“, „Schuss“) | 2 |
| Rekontra („retour“) | 4 |
| Subkontra („nochmals gespritzt“) | 8 |

- Der Faktor wirkt **nur auf das Geldkonto** [OÖ23 Allg. 5]; die Schrift bleibt unverändert.
- Jeder Posten hat seinen eigenen Faktor.
- Positive Spiele: ein Kontra gilt für alle vier Spieler; Negativspiele: je Gegner getrennt.
- Kontras werden nach jedem Spiel sofort ausbezahlt [KTC; STMK].
- **[EMPFEHLUNG]** Die App führt beide Konten und zeigt sie getrennt an („Schrift“ und „Geld“). Als Hausregel-Option kann „Kontra zählt auch für die Wertung“ eingeschaltet werden; Standard ist **aus** (OÖ-konform).

### 7.9 Renonce (nur zur Vollständigkeit)

Die App lässt Regelverstöße nicht zu; im LAN kann nichts Regelwidriges gespielt werden. Die OÖ-Regeln sehen vor [OÖ23 Renonce; OÖR25]:

- **Renonce liegt vor** bei: Verstoß gegen Farb-, Tarock- oder Stichzwang; unerlaubter Ablage (Trullstücke, Könige, falsche Anzahl, verdeckte Tarock, verdeckte Farbkarte beim Farbendreier); Beginn der Lizitation mit falscher Kartenanzahl; Besserrufer ohne Vogel im Blatt; falscher Ansager beim Besserrufer; falscher König ultimo; Verstoß gegen „höherer Vogel zuerst“; spielentscheidendem Vorwerfen oder Reden; Zusammenwerfen der Karten vor Klärung [OÖ23 Renonce 2; Ansagen 4–7; OÖR25 B.2].
- **Folgen:** Das Spiel endet. Alle regulären Spieler erhalten die Punkte, als hätten sie Spiel und alle angesagten Prämien gewonnen; der Renoncespieler bekommt deren Summe als Minus [OÖ23 Renonce 5].
  - Beispiele [OÖR25 B.4]: Renonce des Spielers beim Dreier: Gegner je +4, Spieler −12. Renonce eines Gegners beim Dreier: Spieler +12, reguläre Gegner je +4, Renoncespieler −20. Renonce eines Gegners beim Sechserdreier mit angesagtem Uhu: Spieler +24, reguläre Gegner je +12, Renoncespieler −48.
  - Zwei Renoncespieler teilen den Verlust.
  - Besserrufer ohne Vogel: je 3 Punkte an die Mitspieler, Renoncespieler −9.
  - Trischaken: je 2 Punkte an die regulären Spieler (−6); der Trischaker selbst je 4 (−12); zwei Renoncespieler je −3 [OÖ23 Trischaken 4; OÖR25].
  - Nachweisbare stille Prämien kann der Geschädigte zusätzlich einfordern [OÖ23 Renonce 7].
- **[EMPFEHLUNG]** Die App bildet Renonce nicht nach, erklärt sie aber im Lernmodus (11.5) und zeigt bei verhinderten Zügen, dass dieser Zug am Tisch eine Renonce wäre.

### 7.10 Durchgerechnete Beispiele

Alle Werte in Punkten (Schrift / Geld), Reihenfolge Spieler · Partner · Gegner · Gegner bzw. Spieler · drei Gegner.

| Nr. | Situation | Schrift | Geld |
|---|---|---|---|
| 1 | **Rufer** gewonnen (41/2), stille Trull bei den Gegnern, stille 4 Könige bei der Spielerpartei | Spieler +1, Partner +1, Gegner je −1 | gleich |
| 2 | **Besserrufer**, Pagat angesagt und gemacht, Spiel gewonnen, ein Gegner hat den Pagat kontriert | Spieler +3, Partner +3, Gegner je −3 | Spieler +5, Partner +5, Gegner je −5 |
| 3 | **Piccolo** gewonnen, ein Gegner hat kontriert | Spieler +6, Gegner je −2 | Spieler +8, Kontra-Gegner −4, andere je −2 |
| 4 | **Dreier** gewonnen, stille Trull beim Spieler, stiller Pagat bei den Gegnern | Spieler +12, Gegner je −4 | gleich |
| 5 | **Sechserdreier** verloren, angesagter Uhu gemacht | Spieler −12, Gegner je +4 | gleich |
| 6 | **Sechserdreier** verloren, Kontra auf das Spiel | Spieler −24, Gegner je +8 | Spieler −48, Gegner je +16 |
| 7 | **Solorufer** verloren, angesagter König ultimo gemacht | Spieler +2, Partner +2, Gegner je −2 | gleich |
| 8 | **Solorufer**, gerufener König lag im Talon, Spiel gewonnen, Kontra wirkungslos | Spieler +6, Gegner je −2 | gleich |
| 9 | **Besserrufer**, Spiel gewonnen, angesagter Uhu verloren, Uhu kontriert | Spieler −3, Partner −3, Gegner je +3 | Spieler −7, Partner −7, Gegner je +7 |
| 10 | **Dreier**, stiller Valat, zusätzlich angesagte Trull; stiller Pagat zählt nicht | Spieler +54, Gegner je −18 | gleich |
| 11 | **Dreier**, Valat angesagt und verloren (Spiel mit 40 Punkten gewonnen), angesagter Pagat gemacht | Spieler −90, Gegner je +30 | gleich |
| 12 | **Sechserdreier**, stiller Valat der Gegner, Spiel war kontriert | Spieler −96, Gegner je +32 | Spieler −192, Gegner je +64 |
| 13 | **Rufer**, Valat angesagt und gemacht, kontriert | Spieler +8, Partner +8, Gegner je −8 | Spieler +16, Partner +16, Gegner je −16 |
| 14 | **Farbensolo** gewonnen, stille vier Könige (im Farbensolo doppelt) | Spieler +24, Gegner je −8 | gleich |
| 15 | **Bettel ouvert** verloren, Kontra von B, Kontra + Rekontra gegen C | Spieler −21, Gegner je +7 | Spieler −49; B +14, C +28, D +7 |
| 16 | **Schleifen** eines Besserrufers mit Uhu | Spieler −15, andere je +5 | gleich |

Rechenwege zu den wichtigsten Zeilen:
- Nr. 2: Spiel 1 + Pagat angesagt 2 = 3 je Spieler; Geld 1 + (2 × 2) = 5.
- Nr. 4: Spiel 4 × 3 = 12; stille Trull 1 × 3 = +3; stiller Pagat der Gegner 1 × 3 = −3 → +12.
- Nr. 5: Spiel verloren doppelt 8 × 3 = −24; angesagter Uhu 4 × 3 = +12 → −12.
- Nr. 10: Valat still = 4 × 4 = 16 je Gegner → 48; angesagte Trull 2 × 3 = 6 → 54.
- Nr. 11: Valat angesagt verloren = 8 × 4 = 32 je Gegner → −96; angesagter Pagat 2 × 3 = +6 → −90.
- Nr. 12: 4 (Spiel) × 4 (stiller Valat) × 2 (verlorener Sechser) = 32 je Gegner → −96; Geld zusätzlich × 2 wegen Kontra.
- Nr. 14: Farbensolo 6 × 3 = 18; stille Könige 1 × 2 (Solo) × 3 = 6 → 24.

### 7.11 Abrechnung als Pseudocode

```
struct Posten { Typ typ; Partei gewinner; int wert; int kontra; };

abrechnen(spiel, parteiD, parteiO, drittelD, prämien, valat, kontraSpiel, kontraslos):
    schrift[4] = 0; geld[4] = 0
    b = spielwert(spiel)
    kg = kontraslos ? 1 : kontraSpiel                    // Solorufer mit König im Talon
    wenn valat != null:
        faktor = valat.angesagt ? 8 : 4
        gewinner = valat.gemacht ? valat.partei : gegenpartei(valat.partei)
        v = b * faktor
        wenn spiel == Sechserdreier und gewinner == parteiO: v = v * 2
        buche(v, gewinner, kg)
        prämien = { p ∈ prämien | p.angesagt }           // stille zählen beim Valat nicht
    sonst:
        gewonnen = (drittelD >= 107)
        v = b * ((spiel == Sechserdreier und nicht gewonnen) ? 2 : 1)
        buche(v, gewonnen ? parteiD : parteiO, kg)
    für p in prämien:
        v = wert(p.typ, p.angesagt, spiel)
        buche(v, p.erzielt ? p.partei : gegenpartei(p.partei), kontraslos ? 1 : p.kontra)
    return schrift, geld

buche(v, gewinnerpartei, k):
    wenn |parteiD| == 1:                                  // Alleinspiel
        s = (gewinnerpartei == parteiD) ? +1 : −1
        schrift[spieler] += s*3*v;  geld[spieler] += s*3*v*k
        für g in parteiO: schrift[g] −= s*v; geld[g] −= s*v*k
    sonst:
        für p in gewinnerpartei: schrift[p] += v; geld[p] += v*k
        für p in verliererpartei: schrift[p] −= v; geld[p] −= v*k
```

---

## 8. Partiestruktur (Turnier- und App-Modi)

### 8.1 Der oberösterreichische Turniermodus

- **Drei Durchgänge (Runden)**; je Durchgang **16 Spiele am Vierertisch** [MAIR-CUPS, Tabelle „Cupmodus“, Spalten RTC/HRC/SWC: „Runden 3“, „Spiele pro Runde (4 Spieler) 16“]. 16 Spiele = 4 Radl.
- **Fünfertisch:** Fünfertische sind in allen OÖ-Cups vorgesehen [MAIR-CUPS, Zeile „Fünfertische“]. Die Zahl der Spiele je Durchgang am Fünfertisch ist **in keiner der ausgewerteten Quellen belegt**; die Spalte in [MAIR-CUPS] ist ausdrücklich mit „Spiele pro Runde (4 Spieler)“ überschrieben (RTC/HRC/SWC/TTC/ÖF 16, WTC/STC/KTC 20). **[OFFEN] M-1** **[EMPFEHLUNG]** 4 Radl, also 20 Spiele, damit jeder Spieler viermal gibt und viermal aussetzt; einstellbar.
- Gewertet wird die **Schrift**; Kontras werden sofort in Geld ausbezahlt und nicht geschrieben [OÖ23 Allg. 5; KTC].
- 1 Punkt = 10 Cent ist üblich [LTA-GK1 Kap. 8; KTC; STMK].
- Tagessieger ist, wer die meisten Schriftpunkte hat. Bei Gleichstand entscheidet die beste bzw. zweitbeste Runde [MAIR-CUPS, „Ex-aequo-Kriterien … BR“].
- Cuppunkte nach dem OÖ-Fixpunktesystem (223, 198, 180, … für die besten 50 Ränge) [MAIR-CUPS]; für die App ohne Bedeutung.
- Nicht mehr auflösbare Schreibfehler: Das betroffene Spiel wird für alle vier mit null bewertet [OÖ23 Allg. 9].

### 8.2 Empfohlene Modi der App

| Modus | Inhalt | Wertung |
|---|---|---|
| **Einzelspiel** | 1 Hand | Schrift + Geld |
| **Radl** | 4 Spiele (Vierertisch) bzw. 5 Spiele (Fünfertisch); jeder gibt einmal | Summe |
| **Durchgang (Standard)** | 4 Radl = 16 bzw. 20 Spiele | Summe, Rangliste |
| **Turnier** | 3 Durchgänge = 48 bzw. 60 Spiele, mit Zwischenständen je Durchgang | Schrift, Gleichstand: bester Durchgang |
| **Endlos / Stammtisch** | offen, jederzeit beendbar, laufende Bilanz | Schrift + Geld |

- Die Partie endet **immer am Ende eines Radls**, damit jeder gleich oft Vorhand und Geber war.
- Anzeige je Spieler: Schrift, Geld (in Punkten und optional Euro), Anzahl gespielter Spiele, Bilanz je Spielart (für den Lernmodus interessant).
- Speichern und Fortsetzen muss mitten in der Partie möglich sein (Handy!). Persistiert werden Sitzordnung, Geberposition, Radl-Zähler, beide Konten und der vollständige Spielverlauf des laufenden Spiels.

### 8.3 Optionale Hausregeln (Standard: aus)

Nicht Teil des OÖ-Profils, aber verbreitet; als Schalter sinnvoll [LTA-GK1 Kap. 13; LTA-S „Ergänzungen“]:

- **Doppeltes Radl nach dem Trischaken** oder als Schlussradl („Gstieß a Radl“).
- **Klopfen statt Abheben** (dann werden ganze Blätter zu zwölf Karten gegeben und die Spieler wählen ihr Paket).
- **Zwiccolo** (genau zwei Stiche) und **Farbensechser**.
- **Kontra zählt in die Schrift.**
- **Kaiserstich/Märchenstich** – im OÖ-Profil ausdrücklich nicht [OÖ23 Allg. 3], in Wien üblich [MAIR-CUPS].

Jede eingeschaltete Hausregel muss im Spielprotokoll und beim LAN-Beitritt sichtbar sein, weil sie die Wertung verändert.

### 8.4 Hinweise für LAN und Persistenz

- Der **Server ist maßgeblich**: Er allein kennt Handkarten, Talon, Ablage und die Partnerschaft. Clients erhalten nur ihre Sicht (2.7).
- Jede Aktion wird als Ereignis protokolliert (Gebot, Ruf, Talonwahl, Ablage, Ansage, Kontra, Karte). Aus dem Protokoll lässt sich jede Sicht rekonstruieren; das ist die Grundlage für Wiederaufnahme, Rückschau und Lernmodus.
- Beim Verbindungsabbruch eines Spielers: Ein Computerspieler übernimmt vorübergehend; der Wiedereinstieg ist ohne Informationsverlust möglich, weil der Server die Sicht neu ausliefert.
- Der aussetzende Fünfte bekommt einen eigenen Zuschauerkanal ohne verdeckte Informationen.

---

## 9. Sonderfälle, offene Punkte und regionale Abweichungen

Jede Zeile hat eine ID, damit im Code darauf verwiesen werden kann (z. B. `RuleNote::P5`). **Fett** markiert ist die Festlegung für die App.

### 9.1 Lizitation

| ID | Frage | Quellenlage | Festlegung |
|---|---|---|---|
| **L-1** | Darf die Vorhand beim ersten Sprechen passen? | [OÖ23 Allg. 2] setzt „Vorhand oder Spielansage“ voraus; [KTC] verbietet es ausdrücklich | **Nein.** Die Vorhand muss „Vorhand“ sagen oder ein Spiel ansagen |
| **L-2** | Die Vorhand hat gleich ein Spiel angesagt, alle passen. Darf sie noch wechseln? | [OÖ23 Vorhand 1]: Rufer/Trischaken nur „im Nachhinein“ | **Nein**, das Gebot ist bindend. Rufer und Trischaken gibt es nur nach „Vorhand“ |
| **L-3** | Darf die Vorhand mehrfach halten? | [OÖ23 Vorhand 2] ohne Einschränkung | **Ja**, jedes Mal, wenn ein anderer ein höheres Spiel ansagt |
| **L-4** | Darf die Vorhand den Solodreier halten, obwohl dessen Ansagen „sofort“ erfolgen müssen? | [OÖ23 Vorhand 2] und [Ansagen 3] widersprechen sich nicht ausdrücklich; [MAIR-AUSL] nennt den Solodreier als nicht mehr überbietbar | **Ja, solange die Vorhand noch aktiv ist.** Die Ansagen des überbotenen Bieters verfallen, die haltende Vorhand sagt ihrerseits sofort an. Option `solodreierBeendetLizit` für Runden, die es anders halten |
| **L-5** | Rang von Trischaken und Rufer zueinander | beide nur nach dreimal „weiter“ | ohne Bedeutung; UI reiht Trischaken vor Rufer wie [OÖ23 Tarif] |
| **L-6** | Besserrufer mit hohem Vogel bleibt unter dem Farbendreier | [MAIR-CUPS]: „Besserrufer nicht aufgeteilt – OÖ ja“, [MAIR-GRUND] kritisiert das | **So implementieren.** Der Vogel beeinflusst den Rang nicht |
| **L-7** | Gebot nach eigenem „weiter“ | [LTA-GK1 Kap. 6] | **Nicht möglich**, Passen ist endgültig |
| **L-8** | Zwiccolo, Triccolo, Bei-Spiele, Pagat-/Uhurufer als eigene Gebote | [OÖ23 Tarif] kennt sie nicht; [LTA-F1]: „Es gibt keine sog. Bei-Spiele“ | **Nicht im Profil**; optional als Hausregel (8.3) |

### 9.2 Rufen, Talon, Ablage

| ID | Frage | Quellenlage | Festlegung |
|---|---|---|---|
| **R-1** | Wird der König vor oder nach dem Talon gerufen? | [WIKI] und [KTC] sagen: vorher; [OÖ23] regelt es nicht | **Vorher** (erst rufen, dann Talon aufdecken) |
| **R-2** | Welcher Vogel wird beim Schleifen bezahlt, wenn mehrere im Blatt sind? | [ÖF21]: „Spiel plus Ansager (3, 5, 7 oder 9)“; [LTA-GK1]: „das geplante Vogerl“ | **Der Spieler benennt vor dem Schleifen seinen ursprünglichen Ansager**; die App schlägt den niedrigsten Vogel vor |
| **R-3** | Zahlt der Schleifende an alle drei? | [ÖF21] nennt Beträge je Spieler; ohne Partner bleibt nur 1 gegen 3 | **Ja: −3 × (Spiel + Vogel)** |
| **R-4** | Darf der Spieler, der den gerufenen König aus dem Talon nimmt, König ultimo ansagen? | [MAIR-AUSL] ja; [OÖ23] schweigt; [OÖ23 Ansagen 1] verlangt nur, dass man die Karte selbst hat | **Ja** |
| **R-5** | Abrechnung, wenn der Spieler nach „König im Talon“ allein weiterspielt | [LTA-GK1 Kap. 10]: „spielt allein gegen die anderen drei“ | **1 gegen 3** (dreifacher Spielwert) |
| **R-6** | Solorufer, König im Talon: Wann erfährt der Tisch davon? | [OÖ23 Allg. 7]; [LTA-F1 Kap. 3] | **Erst bei der Abrechnung.** Engine und KI dürfen es vorher nicht verwerten (10.8) |
| **R-7** | Mit drei Königen statt „vierter König“ die Farbe nennen? | nirgends verboten | **Erlaubt**; beides führt zum selben Partner |
| **R-8** | Liegen gelassene Talonhälfte | [OÖT26 3]: nach dem ersten Stich umdrehen | **So umsetzen.** Im Lernmodus als Merkhilfe weiter einsehbar |
| **R-9** | Muss der Spieler beim Verlegen Tarock zeigen? | [OÖ23 Renonce 2] („verdeckte Tarock“ = Renonce) | **Ja**, verlegte Tarock offen; beim Farbendreier verlegte **Farbkarten** offen [OÖ23 Farben 1] |

### 9.3 Prämien und Kontra

| ID | Frage | Quellenlage | Festlegung |
|---|---|---|---|
| **P-1** | Angesagter Vogel kann im Zielstich wegen Farbzwang nicht gespielt werden | [KTC], [STMK]: Prämie verloren, Karte danach frei; [OÖ23] schweigt | **Prämie verloren, Reservierung entfällt** |
| **P-2** | Ist die Aufbewahrungspflicht hart? | **[ABWEICHUNG]** [MAIR-TURNIER] (Tirol): „dürfen nicht freiwillig aufgegeben werden“ – aber [OÖ23 Ansagen 6], [ÖF21 Ansagen] und [OÖR25 B.2] behandeln das vorzeitige Spielen ausdrücklich als bloßen **Prämienverlust** und nur bei „offensichtlichem Vorteil“ als Renonce; auch [MAIR-AUSL] rechnet es als Prämienverlust ab | **Nein.** Vorzeitiges Weggeben ist erlaubt (Warnung, Prämie verloren). **Hart gesperrt** ist nur, mit der angesagten Karte vor ihrem Zielstich einen Stich zu **gewinnen**; hart sind außerdem die Reihenfolge „höherer Vogel zuerst“ im Zwangsfall und die Spielpflicht im Zielstich (5.4, 6.5). Option `harteVogelreservierung` (Standard aus) |
| **P-12** | Gilt „höherer Vogel zuerst“ auch gegenüber dem angesagten König ultimo? | [OÖ23 Ansagen 7] und [ÖF21] nennen nur Vögel | **Nein.** Der König ultimo steht außerhalb der Vogelreihenfolge; muss er zwangsweise weg, ist das reiner Prämienverlust |
| **P-3** | Wer darf Trull und Alle Könige ansagen? | [ÖF21]: „auf Verdacht“ erlaubt | **Jeder Spieler**, auch ohne die Karten |
| **P-4** | Kontra zwischen (verdeckten) Partnern | [KTC], [STMK], [MAIR-AUSL]: zählt nicht; [OÖ23] schweigt | **Zählt nicht** (wird bei der Abrechnung neutralisiert) |
| **P-5** | Zählt ein **misslungener stiller** Vogel bzw. König ultimo negativ? | [PAGAT]: ja, minus; [ÖF21 Rufen]: „Der Verlust des gerufenen Königs wird nicht bewertet, es sei denn er wird ultimo angesagt“; [LTA-F1 Kap. 5] empfiehlt, sich große Vögel für ein stilles Vogerl aufzubehalten – das spricht gegen ein Risiko; [OÖ23] schweigt | **Nein.** Stille Prämien zählen nur, wenn sie gelingen. Option `stillVerlustZaehlt` (Standard aus) für Runden nach [PAGAT] |
| **P-6** | Kann dieselbe Prämienart von beiden Parteien beansprucht werden? | nicht geregelt | **Nein**, je Prämienart genau ein Posten (5.3/7.3) |
| **P-7** | König ultimo in Dreierspielen | es gibt keinen gerufenen König | **Nicht möglich** |
| **P-8** | Dürfen auch die Gegner im Farbenspiel „Alle Könige“ ansagen? | [ÖF21]: „Beim Farbensolo sind nur Vier Könige und Valat möglich“, ohne Partei-Einschränkung | **Ja** |
| **P-9** | Bis wann darf kontriert werden? | [KTC], [STMK]: sofort auf die jeweilige Ansage; [OÖ23 Allg. 2] verlangt Kenntnisnahme mit „gut“ | **Beim ersten eigenen Zug nach der Ansage**; später nur gegen spätere Ansagen |
| **P-10** | Reservierung eines Vogels, dessen Zielstich vorbei ist | – | **Reservierung endet**, die Prämie ist verloren, die Karte frei spielbar |
| **P-11** | Pagatansage im Sechserdreier/Dreier sinnvoll? | [LTA-GK2 Kap. 5] rät ab (5 Kartenpunkte Risiko) | Nur Hinweis für die KI (10.5) |

### 9.4 Valat

| ID | Frage | Quellenlage | Festlegung |
|---|---|---|---|
| **V-1** | Dürfen Gegner kontrieren **und** Valat ansagen? | [OÖ23 Farben 4]: „entweder … oder“ (nur Farbenspiele); [ÖF21]: „zugleich“ erlaubt; [ÖF10]: „nicht zugleich“ | **In Farbenspielen entweder/oder** (geltende Fassung 4/2023), **in allen anderen positiven Spielen beides** |
| **V-2** | Angesagter Valat der Gegenpartei misslingt | nicht ausdrücklich geregelt | **Spiegelbildlich**: Die ansagende Gegenpartei zahlt den achtfachen Spielwert |
| **V-3** | Zählen liegen gebliebene Talonkarten für angesagte Trull/Könige, wenn ein Valat erzielt wurde? | [PAGAT]: Talon fällt der Valatpartei zu; [MAIR-AUSL] und [LTA-GK1 Kap. 7]: Talon gehört immer den Gegnern | **Talon bleibt bei den Gegnern.** Eine angesagte Trull kann also trotz Valat verloren gehen. Option `valatZiehtTalon` (Standard aus) |
| **V-4** | Wirkt ein Kontra auf das Spiel auch auf den Valat? | [LTA-F1 Kap. 1] ausdrücklich ja | **Ja** |
| **V-5** | Angesagter Valat scheitert, die Gegner machen alle übrigen Stiche | – | **Nur der angesagte Posten zählt** (achtfach zu Lasten der Ansager) |
| **V-6** | Valat in Solospielen doppelt? | [LTA-GK1 Kap. 9]: nein, er bezieht sich schon auf den höheren Spielwert | **Nicht zusätzlich verdoppeln** |

### 9.5 Stichregeln

| ID | Frage | Quellenlage | Festlegung |
|---|---|---|---|
| **S-1** | Kaiserstich / Märchenstich | [OÖ23 Allg. 3]: „weder Kaiserstich noch Fänge“ | **Nicht im Profil** (Option für Hausrunden, 8.3) |
| **S-2** | Stichzwang, wenn der Stich schon getarockt ist und man die Farbe hat | [WIKI]: Stichzwang nur „unter Erfüllung des Farbzwangs“ | **Beliebige Karte der ausgespielten Farbe** |
| **S-3** | Pagat-Regel gegen Stichzwang | [OÖ23 Negativ 2] ohne Ausnahme | **Pagat-Regel geht vor**: lieber ein anderes Tarock, auch wenn damit nicht überstochen wird |
| **S-4** | Vorzeitiges Spielende | [WIKI] | **Nur bei gescheitertem Negativspiel** (6.8) |
| **S-5** | Wer deckt bei Ouvert auf? | [OÖ23 Negativ 3]: „alle Karten offen auf den Tisch“ | **Alle vier Spieler**, nach dem ersten Stich, vor dem zweiten Ausspiel; Beratung der drei Gegner erlaubt |
| **S-6** | Farbenspiel: Gilt das Tarock-Ausspielverbot auch für den Spieler? | [OÖ23 Farben 2]: „Tarock darf erst dann ausgespielt werden, wenn man keine Farbkarte mehr im Blatt hat“; [LTA-GK1 Kap. 10]: „Jeder Spieler muss so lange Farbkarten ausspielen, als er noch welche hat“ | **Ja, für alle** |
| **S-8** | Farbenspiel: Wer spielt zum ersten Stich aus? | [OÖ23 Farben 1] eindeutig: „Die Farbenspiele gehören zu den positiven Spielen (Vorhand spielt aus …)“; ebenso [ÖF21 Farbenspiele], [LTA-GK1 Kap. 10]. **[ABWEICHUNG]** [PAGAT] widerspricht sich selbst: erst „In positive contracts (including Farbensolo and Farbendreier) … Forehand leads“, unmittelbar danach „In negative contracts **and Farbensolo**, Declarer leads“ | **Vorhand**, in allen Farbenspielen; die zweite pagat-Aussage ist ein Redaktionsfehler |
| **S-7** | Dürfen eigene Stiche angesehen werden? | [LTA-GK1 Kap. 8] und [MAIR-CUPS]: ja; [ÖF21]: fremde Stiche nein | **Eigene Stiche ja, fremde nur der letzte Stich** (2.8) |

### 9.6 Trischaken

| ID | Frage | Quellenlage | Festlegung |
|---|---|---|---|
| **T-1** | Drei Punktegleiche | [ÖF21]: je 1 Punkt; [OÖ23] nennt nur zwei Gleiche | **Je −1 an den vierten Spieler** |
| **T-2** | Eine Jungfrau bei zwei punktegleichen Verlierern | [OÖ23 Trischaken 3]: „bekommt den gesamten Gewinn“ | **Die Jungfrau erhält alle 4 Punkte** |
| **T-3** | Talonbehandlung | [OÖ23 Trischaken 1]: an den letzten Stich; andere Cups verteilen ihn auf die ersten sechs Stiche [KTC; PAGAT] | **An den Gewinner des letzten Stichs** |
| **T-4** | Fünfertisch | [OÖ23 Allg. 8]: der Geber ist außen vor; [PAGAT] kennt Runden, in denen er mitspielt | **Der Fünfte spielt nicht mit** |
| **T-5** | Kontra | [OÖ23 Allg. 5] | **Keines** |
| **T-6** | Gibt es eine Ausspielpflicht („austarockieren“)? | [OÖ23 Trischaken 1]: „es muss nicht austarockiert werden“; [LTA-GK1 Kap. 11]: „bunt gemischt“ | **Freie Wahl beim Ausspielen** |
| **T-7** | Zwei Jungfrauen: teilen oder je voll? | **[ABWEICHUNG]** [OÖ23 Trischaken 3]: „teilen sie sich den Gewinn“, mit Rechenbeispiel 35/1:34/2 → 2 × 2 = 4; ebenso [KTC]: „Gibt es mehrere ‚Jungfrauen‘, dann werden die zu verteilenden Punkte unter ihnen geteilt“. Dagegen [LTA-GK1 Kap. 11] und [WIKI]: **je** der volle Betrag (also 6 bzw. „je 30 Cent“) | **Teilen nach [OÖ23]** (3 → 2 je Jungfrau, bei „Bürgermeister“ 6 → 3 je Jungfrau). Option `jungfrauenVoll` |

### 9.7 Modus

| ID | Frage | Quellenlage | Festlegung |
|---|---|---|---|
| **M-1** | Spiele je Durchgang am Fünfertisch | in keiner Quelle belegt; die Angabe 16 in [MAIR-CUPS] gilt laut Spaltenkopf nur für Vierertische | **20 (4 Radl)**; einstellbar |
| **M-2** | Zeitlimit je Durchgang | in OÖ nicht veröffentlicht; [KTC]: 100/110 Minuten | **Optional**, Standard aus |
| **M-3** | Muss um Geld gespielt werden? | [OÖ23 Allg. 1]: ja (Turnier) | Für die App ohne Bedeutung; Geldkonto nur als Anzeige |

### 9.8 Abweichungen anderer Regelwerke (für spätere Profile)

**Gegenüber [PAGAT]** (allgemeine Königrufen-Beschreibung):

| Punkt | pagat.com | OÖ 4/2023 |
|---|---|---|
| Tarife | Sechserdreier +2/−4, Bettel 3, Piccolo ouvert 5, Bettel ouvert 6, Solodreier 8 | 4/−8, 2, 6, 7, 8 |
| Zwiccolo, Driccolo, Besserdreier, eigene Vogelrufer | erwähnt | nicht vorhanden |
| Eigenen König rufen | erlaubt („legal to call your own king“) | verboten [OÖ23 Allg. 6] |
| Fänge (Mondfang 1, Sküsfang 2, Königfang 1) | vorhanden | keine [OÖ23 Allg. 3] |
| Kaiserstich | optional | keiner |
| Kontra-Stufen | bis Hirsch-/Mordkontra (×64) | nur Kontra/Rekontra/Subkontra |
| Trischaken | Talonkarte zu jedem der ersten sechs Stiche; zwei Verlierer zahlen zwei Gewinnern | Talon zum letzten Stich; Wertung nach 7.6 |
| Lizitationspriorität | Spieler mit höherer Priorität dürfen gleich hoch bieten | nur die Vorhand darf halten |
| „Marabu“ für IIII | ja | „Quapil“ bzw. „Wildsau“ |
| Stille Prämie misslungen | zählt minus | siehe P-5 |
| Wer darf eine Prämie ansagen | nur, wer Partner des Spielers ist oder dessen Partei bereits bekannt ist („A player may only make a bonus announcement if they are declarer's partner or if it is already known which side they are on“) | jeder Spieler; Trull und Alle Könige sogar „auf Verdacht“ [ÖF21 Ansagen] |
| Farbenspiel, erstes Ausspiel | widersprüchlich (siehe S-8) | immer die Vorhand |

**Gegenüber [WIKI]:** Der Artikel beschreibt zusätzlich Bei-Spiele, Triccolo, halboffene Negativspiele, Handprämien (Köpfe, Honneurs, Tarock, Grand-Tarock, Ohne Trull), Materialprämien (Absolut, Grammopoi), Königfang und Mondfang sowie Kaiserstich-Varianten. Nichts davon gehört zum OÖ-Profil.

**Gegenüber anderen Cups** [MAIR-CUPS, Stand März 2026]:

| Regel | OÖ | Tirol | Wien |
|---|---|---|---|
| Trischaken | 1 | 2 | 2 |
| Bettler | 2 | 4 | 4 |
| Farbendreier | 3 | 5 | 5 |
| Dreier | 4 | 5 | 5 |
| Farbensolo | 6 | 10 | 10 |
| Piccolo ouvert | 6 | 8 | 6 |
| Solodreier | 8 | 10 | 10 |
| Valat | Spiel × 8 | fix 20 | fix 20 |
| Säcke (Absolut) | – | 2 | – |
| Kaiserstich | – | – | ja |
| Vorhand darf halten | ja | – | – |
| Trischaken-Talon | letzter Stich | beides angekreuzt¹ | erste sechs Stiche |
| Zwiccolo | – | – | 2 |

¹ [MAIR-CUPS] markiert für Tirol sowohl „Trischaken: Talon zum Schluss“ als auch „Talon zu den ersten sechs Stichen“; das ist in der Quelle nicht aufgelöst. Für das OÖ-Profil ohne Bedeutung, beim Anlegen eines Tiroler Profils aber zu klären.

Diese Tabelle ist die Vorlage für weitere Regelprofile (`AT-KR-TIROL`, `AT-KR-WIEN`), wenn die App später mehr Profile anbieten soll.

**Private Hausregeln** (z. B. [PEPI], [LTA-GK1 Kap. 13]): Ein Rufer ohne jede Ansage wird nicht gespielt, sondern ausbezahlt; Vorhand weitergeben mit vier Königen; doppelte Runden; Klopfen. Im OÖ-Turnierprofil gilt ausdrücklich: „Nach den Turnierregeln muss auch ein Rufer, bei dem keinerlei Ansage erfolgt, gespielt werden“ [LTA-F1 Kap. 2].

---

## 10. Hinweise für den Computerspieler

### 10.1 Architektur

- Der Regelkern liefert `erlaubteKarten()` (6.7), `stichGewinner()` (6.6) und die Abrechnung (7.11). Die KI arbeitet ausschließlich auf ihrer **Informationsmenge**: eigene Karten, öffentliche Historie, Ansagen, Kontras, offen gezeigte Karten (Talon bis Stichende 1, offen verlegte Tarock), im Ouvert zusätzlich die offenen Blätter.
- Empfohlenes Verfahren:
  1. **Lizitation, Ruf, Talon, Ablage, Ansagen:** Heuristik nach 10.2–10.5 (schnell, erklärbar, ideal für den Lernmodus).
  2. **Kartenspiel:** Determinisierung + Monte-Carlo (PIMC) oder ISMCTS. 20–60 Determinisierungen, je Determinisierung eine schnelle Alpha-Beta- bzw. Rollout-Auswertung; Zeitbudget am Telefon 50–250 ms je Zug.
  3. **Endspiel** (≤ 5 Karten je Hand): exakte Suche je Determinisierung, dann Mehrheitsentscheid.
- **Determinisierung** muss alle bekannten Einschränkungen erfüllen: bekannte Voids (aus Farb-/Tarockzwang), Anzahl noch nicht gespielter Tarock, Kartenbesitz aus Ansagen (Vogel/König beim Ansager), gefallene Karten, liegen gelassene Talonhälfte (in Talonspielen bekannt!), eigene Ablage. Ein einfacher Sampler mit Ablehnung („rejection sampling“) reicht, wenn die Constraints vorher als Zuordnungsmatrix geprüft werden.
- Die Bewertung einer Determinisierung ist der **Punktesaldo aus 7**, nicht nur die Kartenpunkte. So berücksichtigt die KI automatisch Vögel, König ultimo und Valat.

### 10.2 Blattbewertung und Lizitation

Richtwerte, jeweils **vor** dem Talon [LTA-GK1 Kap. 10; LTA-GK2; LTA-F1; MAIR-FIBEL; MAYR-KOMPAKT]:

| Spiel | Anforderung |
|---|---|
| Trischaken | wenige, niedrige Tarock, keine Könige |
| Rufer | mehrere Tarock mit Stecher, dazu Trullstück oder König; zu schwach für einen Sechser, zu stark zum Trischaken |
| Piccolo | ein Planstich (Gstieß, Mond oder kurzer König), sonst kleine Tarock und viele Schlusskarten. Blanker König macht den Stich zu ca. 80 %, einfach besetzt zu ca. zwei Dritteln |
| Bettel | vierfärbig oder tarocklos, keine blanken Figuren, viele Schlusskarten; die gefährlichste Karte kann man zu Beginn ausspielen |
| Solorufer | 5–6 Tarock mit Gstieß oder Mond, ein König, möglichst mehrfärbig und in einer Farbe blank; Ziel ist der König ultimo des Partners |
| Besserrufer | Pagat: 6 Tarock, 1–2 Stecher · Uhu: 7 Tarock, 2–3 Stecher · Kakadu: 8–9 Tarock, ≥ 3 Stecher · Quapil: 9–10 Tarock, ≥ 3 Stecher. Variante „Vierfärber“: 5 Tarock mit Stecher und gute Farbkarte |
| Farbendreier | 6–7 sichere Stiche (der Talon bringt 1–2) |
| Sechserdreier | „10-Punkte-Regel“ (kleines/mittleres Tarock = 1, König = 1, Stecher ab XVI = 2); Gstieß zu sechst oder Mond zu siebent, 1–2 weitere Stecher, Könige, Schmiere zum Verlegen; nie ohne Trullstück, außer mit 9–10 Tarock |
| Dreier | „12-Punkte-Regel“; 6–7 Tarock mit 2–3 Stechern und Trullstück, gute Farbkarte |
| Farbensolo | ca. 8 sichere Stiche bzw. ~30 Punkte im Blatt; drei Könige, Kombination König–Dame, lange geschlossene Farbe |
| Solodreier | „16-Punkte-Regel“; 9–10 Tarock mit 4–5 Stechern, Könige, möglichst keine offene Farbe |
| Ouvertspiele | sehr viele Schlusskarten, in jeder Farbe zwei Karten, höchstens 2–3 sehr kleine Tarock |

Weitere Faktoren:
- **Sitzposition:** „Sei als Erster etwas vorsichtiger, als Letzter etwas mutiger.“ [MAIR-MERK]
- **Talonerwartung:** Erwartungswert 22 · 6/54 = 2,44 Tarock; die Faustregel der Quellen lautet „grobe Annahme: Im Talon liegen 2 Tarock“ [LTA-F1 Kap. 1; LTA-GK1 Kap. 7: „Im Durchschnitt liegen zwei Tarock, oft auch mehr“]. Aus Sicht eines Spielers mit *t* eigenen Tarock ist der Erwartungswert (22 − t) · 6/42. Wurde kein tarockdominiertes Spiel lizitiert, liegen eher mehr [LTA-F1 Kap. 2].
- **Lizitationsinformation:** Bettel deutet auf ungleiche Tarockverteilung, Piccolo auf gleichmäßige [LTA-F1 Kap. 2].
- **Ökonomie:** Ein Alleinspiel bringt den dreifachen Wert. Nach einem gegnerischen Besserrufer ist Kontrieren oft besser als ein eigener Dreier [MAYR-KOMPAKT].
- Der Gewinn-Erwartungswert sollte über Monte-Carlo-Simulation des eigenen Blattes (mit zufälligen Gegnerblättern) kalibriert werden; die Tabellen oben sind die Startwerte.

### 10.3 Königsruf

- Bevorzugt aus einer Farbe mit **besetzter Dame** oder wenigstens Reiter rufen; jedenfalls aus einer kurzen Farbe, nie „aus der Farbe heraus“ (man behält mindestens eine Karte der gerufenen Farbe) [LTA-F1 Kap. 3/4].
- Beim Solorufer von einer **blanken Dame**, damit der Partner den König leichter ansagen kann.
- Mit drei Königen „den vierten König“ rufen: informativ für den Partner („Vorsicht, kein Tarockblatt“) [LTA-F1 Kap. 3].

### 10.4 Talonwahl und Verlegen

- **Besserrufer:** Tarock vor Punkten; sich engfärbig verlegen, die gerufene Farbe behalten [LTA-F1 Kap. 4].
- **Dreier/Sechserdreier:** engfärbig verlegen, offene Farben (Farben ohne König) vermeiden, sonst möglichst viele Punkte verlegen; „Reiz-Dame“ als Trick behalten [LTA-F1 Kap. 5].
- **Farbendreier:** zuerst Tarock verlegen (Pflicht), Farbstiche maximieren.
- **Achtung Information:** Beim offenen Talon wissen die Gegner, was liegen blieb und was der Spieler genommen hat. Die KI muss das in ihrer Inferenz berücksichtigen [LTA-F1 Kap. 6].

### 10.5 Ansagen und Kontra

- **König ultimo** ansagen, wenn man den König mindestens zu dritt hat und 3–4 Tarock mit einem Stecher [LTA-GK2 Kap. 1; LTA-F1 Kap. 3].
- **Vögel** nur bei hoher Gewinnwahrscheinlichkeit ansagen; im Dreier/Sechserdreier den Pagat nur mit sehr starkem Blatt, weil sein Verlust zusätzlich 5 Kartenpunkte kostet [LTA-GK2 Kap. 5; LTA-F1 Kap. 5].
- **Stille Vögel:** Als Gegenspieler lohnt es sich, einen großen Vogel (Uhu/Kakadu/Quapil, nicht den Pagat) für das Endspiel aufzubehalten [LTA-F1 Kap. 5].
- **Kontra-Schwellen** [LTA-GK2 Kap. 1; LTA-F1 Kap. 4/5]:
  - gegen Dreier/Sechserdreier: 6–7 Tarock mit 2 Stechern, möglichst zweifärbig, wenig Schmiere;
  - gegen Besserrufer: 6–7 Tarock mit 1–2 Stechern, möglichst vierfärbig;
  - gegen König ultimo: mindestens 3 Karten der gerufenen Farbe plus Stecher, oder Aussicht auf den letzten Stich.
- Nie den eigenen (vermuteten) Partner kontrieren; die KI schätzt die Partnerwahrscheinlichkeit und kontriert nur über einem Schwellenwert.

### 10.6 Kartenspiel-Heuristiken

**Als Spieler** [MAIR-FIBEL; LTA-F1]:

| Spiel | Eröffnung |
|---|---|
| Rufer, Solorufer | lange Farbe (König zuerst), kurze Könige nicht anspielen |
| Besserrufer | Tarock, bei langer Farbe auch diese; erstes Tarock klein |
| Dreier, Sechserdreier | mittlere Tarock, „nie unter X“, bevorzugt um XV; mit wenig Tarock die lange Farbe |
| Solodreier | hohe Tarock von Beginn an, Sequenzen |
| Farbenspiele | valatsicher beginnen (ein sicherer Farbstich), starke Farbe herunterspielen, Trullstücke der Gegner einsammeln; bei zu wenig Stichen „zurückspielen“ |
| Piccolo, Bettel (auch ouvert) | die gefährlichste Karte zuerst entschärfen; beim Piccolo bleiben zwei Ausspiele |

**Als Partner:** Spiegelprinzip (angezeigte Farbe bzw. Tarock zurückbringen), dem Spieler hoch vorlegen, bei Besserrufer selbst tarockieren, hoch schmieren; „Zweiter Mann so klein er kann, dritter Mann so groß er kann“; ein kleines Tarock für die Spielübergabe aufheben.

**Als Gegner:** Rollen klären – stark ist, wer 6 Tarock mit zwei Stechern hat; schwach mit ≤ 3 Tarock. Der Starke bleibt auf seiner langen Farbe, der Schwache bringt sie zurück oder spielt kurze Farben, legt dem Spieler hohe Tarock vor. Den Spieler „in die Mitte nehmen“. Die gerufene Farbe meidet man; Farben, aus denen der Spieler einen König aus dem Talon genommen hat, ebenfalls.

**Allgemein:** Im Zweifel stechen und schmieren; eine neu gespielte Farbe niedrig stechen („nicht unter X“); Tarock und Vögel mitzählen; „Vogelgier verliert das Spiel“ – Vögel rechtzeitig abgeben, wenn man sie nicht mehr braucht [MAIR-MERK].

**Gegen Piccolo/Bettel:** „Piccolo-Modus“ – austarockieren, den Spieler in die Mitte nehmen, ihm den Pflichtstich verweigern; sobald er seinen Stich hat, „Bettler-Modus“ – ihm einen zweiten Stich anhängen. Der gefährlichste Gegner sitzt rechts vom Spieler [LTA-F1].

**Trischaken:** Zähler loswerden, Mond ausspielen, unter Königen ausspielen, kein Stich ist das Ziel („Jungfrau“).

### 10.7 Inferenz (Partnersuche und Kartenschätzung)

- Partnerindizien: hohes Schmieren in einen Stich, hohes Vorlegen in Tarock, Zurückspielen der angezeigten Farbe, Tarockspiel beim Besserrufer, Ansage „König ultimo“ (sicher) [LTA-GK2 Kap. 1].
- Gegnerindizien: Kontra (Starker Gegner), niedriges Bedienen vorne, Ausspielen der gerufenen Farbe.
- Harte Fakten für die Kartenschätzung: gefallene Karten, Voids, Anzahl noch ausständiger Tarock, liegen gelassene Talonhälfte, offen verlegte Tarock, Ansagen (Ansager hält den Vogel), Lizitation.
- Die KI führt je Gegner eine Wahrscheinlichkeitsverteilung über Karten (einfaches Bayes-Update über die Determinisierungen genügt).

### 10.8 Fairness (Pflicht)

- Die KI darf **nie** verdeckte Informationen verwenden: keine fremden Handkarten, kein ungesehener Talon, kein Wissen über die Partnerschaft, solange sie nicht öffentlich ist.
- Im **Solorufer mit gerufenem König im Talon** darf die KI nicht wissen, dass es keinen Partner gibt; sie muss wie ein Mensch daraus schließen, dass der König nicht auftaucht.
- Der Sampler darf nicht vom echten Blatt „angezogen“ werden. Ein Test dafür: Bei gleicher Informationsmenge und unterschiedlichen echten Verteilungen müssen die Zugwahrscheinlichkeiten identisch sein.
- Schwierigkeitsgrade werden über Suchtiefe, Zeitbudget und absichtliche Fehlerquote gesteuert, **nicht** über zusätzliche Information.

### 10.9 Schwierigkeitsgrade

| Stufe | Verfahren |
|---|---|
| Anfänger | reine Heuristik, keine Inferenz, 10–15 % zufällige Zweitwahl, keine stillen Vögel, kein Kontra außer bei sehr starkem Blatt |
| Fortgeschritten | Heuristik + Inferenz + 10–20 Determinisierungen im Endspiel |
| Profi | ISMCTS/PIMC mit vollem Zeitbudget, exaktes Endspiel ab 5 Karten, Ansage- und Kontralogik nach Erwartungswert |

### 10.10 Tests

- Deterministische Seeds; die drei Übungsspiele aus 11.7 als Regressionstests (Karten, erlaubte Züge, Stichgewinner, Abrechnung).
- Zufallstest: 10 000 Spiele Selbstspiel; Prüfungen: Summe aller Konten = 0, keine illegale Karte, jede Hand endet nach 12 Stichen (oder korrekt vorzeitig), Wertungsgrenze 107 Drittel korrekt, Prämienlogik.
- Statistiktest: Über viele Spiele soll kein Sitz systematisch im Vorteil sein (außer dem echten Vorhandvorteil).

---

## 11. Inhalte für den Lernmodus

### 11.1 Konzept

Der Lernmodus ist kein eigener Spielmodus, sondern eine Schicht über dem normalen Spiel:

1. **Erklärstufen:** *Neuling* (alles wird erklärt, Züge werden vorgeschlagen), *Lernend* (Erklärung auf Anforderung, Warnung vor groben Fehlern), *Aus*.
2. **Warum-Knopf:** Jederzeit abrufbar: „Warum darf ich diese Karte nicht spielen?“, „Warum ist das ein guter Zug?“, „Wie stehe ich gerade?“ (Punktezähler live, Tarockzähler, gefallene Vögel).
3. **Sofortige, freundliche Rückmeldung:** Unzulässige Aktionen werden nicht bestraft, sondern erklärt (11.4). Die Karte bleibt in der Hand, die betroffenen erlaubten Karten werden hervorgehoben.
4. **Nachbesprechung:** Nach jedem Spiel eine kurze Bilanz: Wer hat wie viele Punkte, welche Prämien sind gefallen, welcher Zug war entscheidend.
5. **Übungsspiele** mit festen Blättern (11.7) und Schritt-für-Schritt-Erklärung.
6. **Nachschlagen:** Glossar (11.6), Tarifkarte, Regelkapitel, jeweils aus dem Spiel heraus erreichbar.

### 11.2 Die Phasen in einfachem Deutsch

**Die Karten.** Tarock spielt man mit 54 Karten. 32 davon sind Farbkarten in Herz, Karo, Pik und Treff: König, Dame, Reiter, Bube und vier kleine Karten. Aufpassen: Der **Reiter** (das Pferd) ist stärker als der Bube und schwächer als die Dame – Anfänger verwechseln ihn gern mit dem König. Bei **Herz und Karo** sind die kleinen Karten As, 2, 3, 4, und das **As ist die stärkste** davon; bei **Pik und Treff** sind es 10, 9, 8, 7. Die restlichen 22 Karten sind die **Tarock**, durchnummeriert von I bis XXI, dazu der **Sküs** ohne Zahl. Jedes Tarock sticht jede Farbkarte. Drei Tarock haben eigene Namen und sind besonders wertvoll: **Sküs**, **Mond** (XXI) und **Pagat** (I); zusammen heißen sie **Trull**. Die vier kleinsten Tarock heißen **Vögel**: Pagat (I), Uhu (II), Kakadu (III), Quapil (IIII).

**Punkte.** Nicht jede Karte zählt gleich. Könige, Sküs, Mond und Pagat zählen 5, Damen 4, Reiter 3, Buben 2 und alle anderen Karten 1. Gezählt wird immer in Dreierpaketen, und von jedem Paket zieht man 2 ab. Im ganzen Spiel stecken 70 Punkte. Wer ein normales Spiel gewinnen will, braucht davon **mehr als die Hälfte: 35 Punkte und 2 Blatt**. 35 Punkte und 1 Blatt sind zu wenig – das ist der häufigste Schock für Neulinge.

**Geben.** Gespielt wird gegen den Uhrzeigersinn, also nach rechts. Der Geber mischt, der linke Nachbar hebt ab. Dann bekommt jeder sechs Karten, sechs Karten kommen als **Talon** in die Mitte (zwei Päckchen zu drei), und jeder bekommt nochmals sechs. Jeder hat zwölf Karten. Der Spieler rechts vom Geber ist die **Vorhand**; sie spricht zuerst und spielt meistens die erste Karte aus.

**Lizitation (das Ansagen des Spiels).** Jetzt wird versteigert, wer welches Spiel spielen darf. Die Vorhand sagt „Vorhand“ – das heißt nur: „Ich will mir alles offenhalten.“ Dann sagt reihum jeder entweder ein Spiel an oder „weiter“. Wer einmal „weiter“ gesagt hat, ist draußen. Ein neues Gebot muss immer **höher** sein als das vorige. Die Vorhand hat zwei Vorrechte: Sie darf ein angesagtes Spiel **halten** (also selbst spielen, ohne zu überbieten), und wenn alle anderen „weiter“ sagen, darf sie jedes Spiel wählen – auch den billigen **Rufer** oder das **Trischaken**. Den **Sechserdreier** muss sie dagegen sofort ansagen, sonst ist er weg.

**Rufen (den Partner suchen).** Bei den Ruferspielen sucht der Spieler einen Partner: Er ruft einen **König**, den er selbst nicht hat. Wer diesen König hat, ist sein Partner – sagt es aber nicht! Beide erkennen einander erst am Spiel: Der Partner schmiert dem Spieler hohe Karten in den Stich oder legt hoch vor. Wer drei Könige hat, darf „**den vierten König**“ rufen, ohne die Farbe zu verraten.

**Talon und Verlegen.** Bei den meisten Spielen deckt der Spieler den Talon auf. Er nimmt eine Hälfte (drei Karten) und legt dafür drei Karten wieder weg; sie zählen am Ende zu seinen Stichen. Die andere Hälfte gehört den Gegnern. Weglegen darf man **keine Könige und keine Trullstücke**, Tarock nur, wenn es gar nicht anders geht – und dann offen für alle sichtbar. Beim **Sechserdreier** bekommt der Spieler alle sechs Karten verdeckt und legt sechs weg. Bei den **Solospielen** bleibt der Talon liegen und gehört am Ende den Gegnern.

**Ansagen und Kontra.** Jetzt sagt der Spieler seine **Prämien** an, zum Beispiel „Pagat“ (ich mache den letzten Stich mit dem Pagat) oder „Trull“. Dann sagt er „**Ich liege**“. Danach sind die anderen dran: Sie sagen „gut“, machen eigene Ansagen oder **kontrieren** („Schuss“), wenn sie nicht glauben, dass es gelingt. Kontra verdoppelt den Einsatz, Rekontra vervierfacht ihn, Subkontra verachtfacht ihn. Wichtig: **Kontra zählt nur beim Geld, nicht in der Turnierwertung.** Wer eine Prämie nicht ansagt, kann sie trotzdem **still** erzielen – dann zählt sie die Hälfte.

**Spielen.** Die Vorhand spielt aus (bei Negativspielen der Spieler). Reihum legt jeder eine Karte:
- **Farbzwang:** Man muss die ausgespielte Farbe bedienen, wenn man sie hat.
- **Tarockzwang:** Hat man die Farbe nicht, **muss** man Tarock zugeben.
- Hat man weder noch, legt man eine beliebige Karte ab.
- Den Stich gewinnt das höchste Tarock, sonst die höchste Karte der ausgespielten Farbe. Wer den Stich gewinnt, spielt zum nächsten aus.

**Zählen und Abrechnen.** Am Ende zählt jede Partei ihre Stiche (samt Ablage bzw. Talonhälfte). Ab 35 Punkten und 2 Blatt ist ein positives Spiel gewonnen. Dazu kommen die Prämien: Jede wird **getrennt** verrechnet. Man kann also das Spiel gewinnen und trotzdem Minus machen, weil ein angesagter Vogel misslungen ist.

**Zu fünft.** Sitzt man zu fünft am Tisch, gibt der Geber nur den anderen vier und **setzt aus**. Er ist reiner Zuschauer, darf nichts sagen und den Talon nicht anschauen. Nach jedem Spiel rückt alles einen Platz weiter, also setzt jeder einmal pro Runde aus.

### 11.3 Warum es jedes Spiel und jede Prämie gibt

**Die Spiele:**

| Spiel | Wozu es da ist | Wann man es wählt |
|---|---|---|
| **Trischaken** | Strafspiel für „Staudenhocker“: Wer nichts ansagt, riskiert, dass die Vorhand alle zum Punktevermeiden zwingt. Es sorgt dafür, dass mutig lizitiert wird | Vorhand mit wenigen, kleinen Tarock und ohne Könige, wenn sonst niemand etwas will |
| **Rufer** | Das Grundspiel: billig, aber mit Partner. Es gibt der Vorhand auch mit mittlerem Blatt eine Aufgabe | einige Tarock mit Stecher, dazu ein Trullstück oder König: zu schwach für mehr, zu stark fürs Trischaken |
| **Piccolo / Bettel** | Chance für schlechte Blätter: Wer fast nichts sticht, kann daraus ein Spiel machen | Piccolo: genau ein sicherer Stich (Sküs oder kurzer König). Bettel: keine gefährlichen Karten, viele Schlusskarten |
| **Solorufer** | Belohnt ein Blatt mit hohen Tarock ohne Talonhilfe; hier zählt jede Prämie doppelt, vor allem der König ultimo | 5–6 Tarock mit Sküs oder Mond, ein König, kurze Farben |
| **Besserrufer** | Das eigentliche Tarockspiel mit Partner: Man verpflichtet sich zu einem Vogel und bekommt dafür einen hohen Zusatzwert | viele Tarock (Pagat ab 6, Uhu ab 7, Kakadu ab 8–9) mit Stechern |
| **Farbendreier / Farbensolo** | Geben Blättern mit vielen Königen und Figuren, aber schlechten Tarock eine Chance, weil Tarock hier nicht stechen | 6–7 (Farbendreier) bzw. 8 sichere Farbstiche |
| **Sechserdreier** | Das Wagnis der Vorhand: alle sechs Talonkarten, dafür doppelter Verlust | 6–7 Tarock mit Trullstück und Stechern, Schmiere zum Verlegen |
| **Dreier** | Das klassische Alleinspiel: dreifache Bezahlung | wie Sechserdreier, aber etwas stärker, weil nur drei Talonkarten kommen |
| **Piccolo / Bettel ouvert** | Die Königsdisziplin der Negativspiele: mit offenen Karten gegen drei beratende Gegner | sehr viele Schlusskarten, in jeder Farbe zwei Karten |
| **Solodreier** | Das höchste Spiel für ein Traumblatt; alle Prämien doppelt | 9–10 Tarock mit 4–5 Stechern und Königen |

**Die Prämien:**

| Prämie | Wozu | Wann ansagen |
|---|---|---|
| **Pagat ultimo** | Der Reiz, die kleinste Karte bis zum Schluss durchzubringen. Älteste Zusatzprämie des Tarocks | wenn man genug hohe Tarock hat, um alle anderen leerzuspielen |
| **Uhu, Kakadu, Quapil** | Dasselbe für den vorletzten, drittletzten, viertletzten Stich; je höher der Vogel, desto mehr Tarockübermacht ist nötig | nur mit klarer Tarockhoheit, sonst still versuchen |
| **König ultimo** | Belohnt das Zusammenspiel: Der Partner bringt den gerufenen König im letzten Stich heim – und gibt sich dadurch zu erkennen | König mindestens zu dritt und 3–4 Tarock mit Stecher |
| **Trull** | Belohnt den Besitz der drei teuersten Tarock | wenn man zwei Trullstücke hat und das dritte fangen kann; auf Verdacht auch als Signal an den Partner |
| **Alle Könige** | Belohnt Königsblätter, die sonst leicht abgestochen werden | mit drei Königen und Tarockschutz |
| **Valat** | Der Jackpot: alle zwölf Stiche | nur mit nahezu geschlossener Tarockfolge; sonst still probieren |
| **Kontra** | Hält die Spannung: Die Gegner können jede Ansage verdoppeln | sobald eine echte Chance besteht – „Ein Kontra belebt das Spiel“ |

### 11.4 Fehlermeldungen im Wortlaut

Die App verhindert unzulässige Aktionen; im Lernmodus erscheint dazu der folgende Text (Platzhalter in geschweiften Klammern). Jeder Text nennt zuerst die Regel, dann die Folge, dann die Alternative. Ein „Mehr dazu“ verweist auf das Regelkapitel.

**Beim Kartenspielen**

| Code | Situation | Text |
|---|---|---|
| `E_NICHT_AM_ZUG` | fremder Zug | „Du bist noch nicht dran. Zuerst spielt {Name}.“ |
| `E_FARBZWANG` | Farbe ausgespielt, Spieler hat sie noch | „Es gilt **Farbzwang**: {Farbe} wurde ausgespielt, und du hast noch {Farbe} auf der Hand ({Karten}). Du musst eine davon spielen. Die {gewählte Karte} darfst du nur spielen, wenn du keine {Farbe} mehr hast.“ |
| `E_FARBZWANG_TAROCK` | Tarock ausgespielt, Spieler hat Tarock | „Es wurde **Tarock** ausgespielt. Tarock ist wie eine eigene Farbe: Solange du Tarock hast, musst du Tarock zugeben. Erlaubt sind: {Karten}.“ |
| `E_TAROCKZWANG` | keine Farbe mehr, aber Tarock vorhanden | „Du hast kein {Farbe} mehr – dann gilt der **Tarockzwang**: Du musst ein Tarock zugeben, auch wenn du damit nichts gewinnst. Erlaubt sind: {Tarock}. Erst wenn du auch kein Tarock mehr hast, darfst du eine beliebige Karte abwerfen.“ |
| `E_STICHZWANG_FARBE` | Negativspiel, könnte in der Farbe überstechen | „Im {Spiel} gilt **Stichzwang**: Du musst die höchste Karte im Stich ({Karte}) überstechen, wenn du es in der ausgespielten Farbe kannst. Möglich wäre: {Karten}.“ |
| `E_STICHZWANG_TAROCK` | Negativspiel, müsste übertrumpfen | „Im {Spiel} gilt **Stichzwang** auch beim Tarock: Du musst das höchste Tarock im Stich ({Karte}) überstechen, wenn du kannst. Möglich wäre: {Karten}.“ |
| `E_STICHZWANG_TRUMPFEN` | Negativspiel, void, muss Tarock spielen | „Du kannst {Farbe} nicht bedienen. Im {Spiel} musst du dann mit Tarock stechen, solange du eines hast. Erlaubt sind: {Tarock}.“ |
| `E_PAGAT_NEGATIV` | Pagat gespielt, weitere Tarock vorhanden | „Der **Pagat darf im {Spiel} erst als letztes Tarock** gespielt werden. Du hast noch {n} andere Tarock ({Karten}). Spiel eines davon – der Pagat kommt erst, wenn er dein einziges Tarock ist.“ |
| `E_FARBENSPIEL_TAROCK_AUSSPIEL` | Farbenspiel, Tarock ausgespielt trotz Farbkarten | „Im **Farbenspiel** sticht Tarock keine Farbe, und du darfst Tarock erst ausspielen, wenn du **keine Farbkarte mehr** hast. Du hast noch {Karten}.“ |
| `W_VOGEL_RESERVIERT` (**Warnung**, kein Verbot) | angesagter Vogel vor seinem Stich | „Du hast **{Vogel}** angesagt. Wenn du ihn jetzt weggibst, ist die Prämie verloren – erlaubt ist es aber. Behalte ihn bis zum {n}-letzten Stich, oder spiel eine andere Karte: {Karten}. · *Trotzdem spielen*“ |
| `E_VOGEL_STICHT_ZU_FRUEH` (**Verbot**) | angesagter Vogel würde einen Stich vor seinem Zielstich sicher gewinnen | „Mit dem angesagten **{Vogel}** darfst du **keinen früheren Stich gewinnen** – das ist der einzige Fall, den die Regeln ausdrücklich verbieten („Pagat vorzeitig heimstechen, um Trull und Spiel zu retten“). Spiel eine andere Karte: {Karten}.“ |
| `E_VOGEL_FAELLIG` | Zielstich, Vogel spielbar, andere Karte gewählt | „Jetzt ist der {n}-letzte Stich – der Stich für deinen angesagten **{Vogel}**. Du darfst ihn regelkonform spielen, also musst du ihn jetzt auch spielen.“ |
| `E_HOEHERER_VOGEL` | mehrere angesagte Vögel, Zwangsabgabe | „Du musst jetzt einen deiner angesagten Vögel vorzeitig hergeben. Die Regel lautet: **zuerst der höhere Vogel**. Spiel also {Vogel} und behalte {Vogel2}.“ |
| `W_KOENIG_ULTIMO_RESERVIERT` (**Warnung**) | angesagter König vor dem letzten Stich | „Du hast **König ultimo** angesagt. Gibst du den {König} jetzt her, ist die Prämie weg – verboten ist es nicht. · *Trotzdem spielen*“ |
| `E_KOENIG_ULTIMO_STICHT_ZU_FRUEH` (**Verbot**) | angesagter König würde vorzeitig einstechen | „Mit dem angesagten {König} darfst du **nicht vorzeitig einstechen**. Spiel eine andere Karte: {Karten}.“ |
| `E_KOENIG_ULTIMO_FAELLIG` | letzter Stich, König spielbar | „Letzter Stich: Jetzt muss der angesagte {König} auf den Tisch.“ |

**Beim Verlegen (Talon)**

| Code | Text |
|---|---|
| `E_ABLAGE_KOENIG` | „**Könige darf man nie verlegen.** Der {König} bleibt in der Hand. Verlege stattdessen kleine Farbkarten – am besten aus einer Farbe, die du dann ganz loswirst.“ |
| `E_ABLAGE_TRULL` | „**Trullstücke (Sküs, Mond, Pagat) darf man nie verlegen.** Sie zählen je 5 Punkte und gehören zur Trull.“ |
| `E_ABLAGE_TAROCK_NOCH_FARBE` | „**Tarock darfst du nur verlegen, wenn du keine Farbkarten mehr verlegen kannst.** Du hast noch {Karten}. Und wenn es einmal nötig ist, musst du das Tarock offen hinlegen.“ |
| `E_ABLAGE_FARBE_NOCH_TAROCK` | „Im **Farbendreier** ist es umgekehrt: Zuerst müssen Tarock weg (verdeckt), Farbkarten erst, wenn du keine verlegbaren Tarock mehr hast – und dann offen. Du hast noch {Tarock}.“ |
| `E_ABLAGE_ANZAHL` | „Du musst genau {k} Karten verlegen, im Moment sind es {m}.“ |
| `I_ABLAGE_OFFEN` (Hinweis) | „Dieses Tarock wird beim Verlegen **offen** hingelegt – alle sehen es. Das ist Vorschrift.“ |

**Beim Lizitieren**

| Code | Text |
|---|---|
| `E_GEBOT_ZU_NIEDRIG` | „{Spiel} steht in der Reihenfolge nicht über {aktuelles Spiel}. Du musst höher bieten (ab {nächstes mögliches Spiel}) oder ‚weiter‘ sagen.“ |
| `E_VORHANDSPIEL_NUR_HINTENNACH` | „**Rufer und Trischaken** sind Vorhandspiele: Sie gehen nur, wenn du Vorhand bist **und** alle anderen ‚weiter‘ gesagt haben.“ |
| `E_SECHSER_NUR_VORNEWEG` | „Den **Sechserdreier** kann die Vorhand nur ganz zu Beginn ansagen, also statt ‚Vorhand‘. Jetzt ist er nicht mehr möglich.“ |
| `E_SCHON_GEPASST` | „Du hast schon ‚weiter‘ gesagt. Wer einmal gepasst hat, ist aus der Lizitation draußen.“ |
| `E_VORHAND_MUSS_SPRECHEN` | „Als Vorhand kannst du nicht passen. Sag ‚Vorhand‘, wenn du dir alles offenhalten willst, oder sag gleich ein Spiel an.“ |
| `E_HALTEN_NUR_VORHAND` | „**Halten** darf nur die Vorhand. Du musst höher bieten oder ‚weiter‘ sagen.“ |
| `E_RUFER_VIER_KOENIGE` | „Du hast **alle vier Könige**. Dann kannst du keinen König rufen und darfst kein Ruferspiel ansagen. Möglich sind Alleinspiele oder Negativspiele.“ |
| `E_BESSERRUFER_OHNE_VOGEL` | „Für den **Besserrufer** brauchst du schon jetzt einen Vogel im Blatt: Pagat (I), Uhu (II), Kakadu (III) oder Quapil (IIII). Du hast keinen davon.“ |

**Beim Rufen**

| Code | Text |
|---|---|
| `E_EIGENER_KOENIG` | „Du darfst **keinen König rufen, den du selbst hast**. Ruf eine Farbe, in der dir der König fehlt – am besten eine, in der du die Dame hast.“ |
| `E_VIERTER_KOENIG_NUR_MIT_DREI` | „‚Den vierten König‘ darfst du nur rufen, wenn du **genau drei Könige** in der Hand hast.“ |

**Bei Ansagen und Kontra**

| Code | Text |
|---|---|
| `E_VOGEL_NICHT_IM_BLATT` | „**{Vogel}** darfst du nur ansagen, wenn du die Karte selbst hast. Für den Partner darf man Vögel und König ultimo nicht ansagen.“ |
| `E_KU_OHNE_KOENIG` | „**König ultimo** darf nur ansagen, wer den gerufenen {König} selbst in der Hand hat.“ |
| `E_ANSAGE_NUR_EINMAL` | „Jeder Spieler darf **nur einmal** Prämien ansagen. Deine Ansage war {Ansagen}.“ |
| `E_BESSERRUFER_VOGEL_PFLICHT` | „Beim **Besserrufer musst du deinen Vogel ansagen** – und zwar den, den du schon vor dem Talon hattest ({Vogel}). Ein Vogel aus dem Talon darf nur zusätzlich angesagt werden.“ |
| `E_NEGATIV_KEINE_ANSAGE` | „Im {Spiel} gibt es **keine Prämien**. Es geht nur um {Stichziel}.“ |
| `E_FARBENSPIEL_ANSAGE` | „Im Farbenspiel sind nur **Alle Könige** und **Valat** möglich. Trull und Vögel werden hier nicht gewertet.“ |
| `E_SOLODREIER_ANSAGE_ZU_SPAET` | „Beim **Solodreier** müssen alle Ansagen sofort mit dem Gebot kommen, weil niemand mehr überbieten kann. Nach ‚Ich liege‘ geht nichts mehr.“ |
| `E_KONTRA_VOR_ICH_LIEGE` | „Kontrieren kannst du erst, wenn der Spieler **‚Ich liege‘** gesagt hat.“ |
| `E_KONTRA_EIGENE_ANSAGE` | „Die eigene Ansage kann man nicht kontrieren. Gegen ein Kontra der Gegner kannst du ‚Rekontra‘ sagen.“ |
| `E_KONTRA_STUFE` | „Höher als **Subkontra** geht es nicht: Kontra (×2), Rekontra (×4), Subkontra (×8).“ |
| `E_KONTRA_ZU_SPAET` | „Diese Ansage hättest du sofort kontrieren müssen. Jetzt kannst du nur noch Ansagen kontrieren, die nach dir gemacht wurden.“ |
| `E_TRISCHAKEN_KEIN_KONTRA` | „Beim **Trischaken** gibt es kein Kontra – hier spielt jeder für sich.“ |
| `E_FARBENSPIEL_KONTRA_ODER_VALAT` | „Im Farbenspiel darfst du als Gegenspieler **entweder** kontrieren **oder** den Valat ansagen, nicht beides.“ |

### 11.5 Typische Anfängerfehler

1. **Reiter für den König halten.** Der Reiter sitzt auf einem Pferd, der König trägt Krone und Zepter.
2. **Rote Kleine falsch einschätzen.** In Herz und Karo ist das **As die höchste** kleine Karte und die **4 die niedrigste**; in Pik und Treff ist die 10 die höchste und die 7 die niedrigste.
3. **35 Punkte für genug halten.** Es braucht **35 und 2 Blatt**. Bei 35/1 ist das Spiel verloren.
4. **Den eigenen König rufen** oder mit vier Königen ein Ruferspiel ansagen. Beides ist verboten.
5. **Könige oder Trullstücke verlegen** wollen – nie erlaubt; Tarock nur im Notfall und dann offen.
6. **Tarockzwang vergessen:** Wer die Farbe nicht hat, muss Tarock zugeben, auch wenn er den Stich nicht gewinnt. Wer stattdessen eine andere Farbe abwirft, macht am Tisch eine Renonce.
7. **Im Negativspiel den Pagat zu früh spielen.** Er darf erst weg, wenn er das letzte Tarock ist.
8. **Im Farbenspiel Tarock ausspielen,** obwohl noch Farbkarten in der Hand sind.
9. **Angesagten Vogel zu früh hergeben.** Verboten ist das nicht – aber die Prämie ist dann weg. Müssen mehrere weg, zuerst der höhere. Wirklich verboten (Renonce) ist nur, mit dem angesagten Vogel vorzeitig einen Stich zu **gewinnen**.
10. **Vogelgier.** Ein stiller Vogel ist schön, kostet aber oft das Spiel: „Das Taktieren mit Vogerln kann dir das Spiel kosten.“
11. **Nicht schmieren.** Hohe Figuren gehören in den sicheren Stich des Partners, nicht ins eigene Blatt gehortet.
12. **Tarock horten.** Wer nie sticht, bleibt am Ende mit wertlosen hohen Tarock sitzen.
13. **Kurze Könige zu früh selbst anspielen** – als Alleinspieler beinahe immer ein Fehler.
14. **Nicht mitzählen.** Wenigstens die gefallenen Tarock und die Vögel muss man sich merken; die liegen gelassene Talonhälfte mitzählen!
15. **Kontra persönlich nehmen.** Ein Kontra ist Teil des Spiels und zählt bei uns nur fürs Geld.
16. **Den Partner suchen, indem man redet.** Erlaubt sind nur Signale durch das Spiel selbst.
17. **Vergessen, dass die liegen gelassene Talonhälfte den Gegnern gehört** – oft entscheidet sie das Spiel.
18. **Am Fünfertisch mitreden.** Der Aussetzer ist Zuschauer und darf den Talon nicht sehen.
19. **Beim Trischaken Stiche machen.** Ziel ist, möglichst wenige Punkte zu bekommen; ideal ist die Jungfrau (kein Stich).
20. **Zu vorsichtig lizitieren.** „Wer nichts wagt, gewinnt auch nichts“ – und wer gar nie ansagt, bekommt das Trischaken serviert.

### 11.6 Glossar

| Begriff | Bedeutung |
|---|---|
| **Ablage** | Die Karten, die der Spieler nach der Talonaufnahme verlegt. Sie zählen zu seinen Stichen. |
| **Absolut** | Prämie für besonders viele Punkte (45/2). **Im OÖ-Profil nicht gespielt.** |
| **Angesagt** | Eine Prämie, die vorher angekündigt wurde. Zählt doppelt so viel wie „still“, kann aber verloren gehen und kontriert werden. |
| **Ansager** | OÖ-Ausdruck für die Vögel (I, II, III, IIII), daher „A-Rufer“. |
| **Austarockieren** | Tarock so lange spielen, bis die anderen keine mehr haben. Beim Trischaken ausdrücklich nicht verlangt. |
| **Besserrufer (A-Rufer)** | Ruferspiel mit Pflichtansage eines Vogels. |
| **Bettel (Bettler)** | Negativspiel: keinen einzigen Stich machen. |
| **Blatt** | 1. die zwölf Karten eines Spielers, 2. eine Karte mit dem kleinsten Wert (⅓ Punkt). |
| **Bürgermeister** | Beim Trischaken: wer mindestens 35 Punkte und 2 Blatt bekommen hat; er zahlt doppelt. |
| **Cavall** | anderer Name für den Reiter. |
| **Dreier** | Alleinspiel gegen drei mit drei Talonkarten. |
| **Fahren** | anderer Name für Trischaken. |
| **Fänge** | Prämien für das Abstechen von Mond, Sküs oder gerufenem König. **Im OÖ-Profil nicht gespielt.** |
| **Farbzwang (Bedienzwang)** | Man muss die ausgespielte Farbe bedienen, wenn man sie hat. |
| **Farbenspiel** | Farbendreier oder Farbensolo: Tarock stechen keine Farbe. |
| **Gabel** | Zwei Karten einer Farbe, die eine gegnerische Karte „einklemmen“ (z. B. König und Reiter gegen die Dame). |
| **Geber** | Wer mischt und austeilt. Am Fünfertisch setzt er aus. |
| **Gstieß** | anderer Name für den Sküs. |
| **Hausmeister** | Das höchste Tarock, das noch im Spiel ist. |
| **Ich liege** | Formel, mit der der Spieler seine Ansagen beendet und das Spiel eröffnet. |
| **Jungfrau** | Beim Trischaken: ein Spieler ohne Stich; er kassiert. |
| **Kaiserstich (Märchenstich)** | Sonderregel, nach der der Pagat sticht, wenn die ganze Trull in einem Stich fällt. **Im OÖ-Profil nicht gespielt.** |
| **Kiebitz** | Zuschauer; am Fünfertisch der aussetzende Geber. Er hat zu schweigen. |
| **König ultimo** | Prämie: Der gerufene König liegt im letzten Stich, und die Spielerpartei gewinnt ihn. |
| **Kontra (Schuss, gespritzt)** | Verdoppelt den Wert eines Postens; Rekontra („retour“) vervierfacht, Subkontra verachtfacht. Zählt nur fürs Geld. |
| **Lizitation** | Die Versteigerung des Spiels. |
| **Mond** | Tarock XXI, zweithöchstes Tarock, Trullstück, 5 Punkte. |
| **Negativspiel** | Trischaken, Piccolo, Bettel und die Ouvertspiele. |
| **Ouvert** | „offen“: Nach dem ersten Stich legen alle ihre Karten offen hin. |
| **Pagat (Spatz)** | Tarock I, kleinstes Tarock, Trullstück, 5 Punkte. |
| **Pagat ultimo** | Prämie: mit dem Pagat den letzten Stich machen. |
| **Piccolo** | Negativspiel: genau einen Stich machen. |
| **Prämie (Ansage, Zusatzprämie)** | Zusatzziel neben dem Spiel: Trull, Alle Könige, König ultimo, Vögel, Valat. |
| **Quapil (Wildsau, Marabu)** | Tarock IIII; Prämie für den viertletzten Stich. |
| **Radl** | Eine Runde, in der jeder einmal gibt: 4 Spiele zu viert, 5 Spiele zu fünft. |
| **Reiter** | Figur zwischen Dame und Bube (Pferd im Bild), 3 Punkte. |
| **Renonce** | Regelverstoß mit Strafwertung. In der App ausgeschlossen, weil unzulässige Züge gar nicht möglich sind. |
| **Rufer** | Das billigste Partnerspiel der Vorhand. |
| **Schleifen** | Aufgeben eines Ruferspiels, wenn der gerufene König im Talon liegt. |
| **Schmieren** | Dem Partner hohe Punktekarten in den Stich legen. |
| **Sechserdreier** | Alleinspiel der Vorhand mit allen sechs Talonkarten; verloren zählt es doppelt. |
| **Skartin (Skartindel, Glatze)** | Die kleinen Farbkarten (1–4 bzw. 7–10). |
| **Sküs** | Höchstes Tarock, ohne Zahl, Trullstück, 5 Punkte. |
| **Solospiel** | Spiel ohne Talonaufnahme (Solorufer, Solodreier, Farbensolo). Alle Prämien zählen doppelt. |
| **Spieler (Spielersteher)** | Wer die Lizitation gewonnen hat. |
| **Stecher** | Umgangssprachlich die hohen Tarock (etwa ab XVI). |
| **Stichzwang** | Pflicht, die höchste liegende Karte zu überstechen – nur in Negativspielen. |
| **Still (heimlich)** | Eine Prämie ohne Ansage erzielen; sie zählt die Hälfte. |
| **Talon** | Die sechs Karten in der Tischmitte, in zwei Hälften zu drei. |
| **Tarock** | 1. die 22 Trümpfe, 2. das Spiel selbst. |
| **Tarockzwang** | Pflicht, Tarock zuzugeben, wenn man die ausgespielte Farbe nicht hat. |
| **Trischaken** | Vorhandspiel, bei dem jeder gegen jeden möglichst wenige Punkte machen will. |
| **Trull** | Sküs, Mond und Pagat zusammen; auch die Prämie dafür. |
| **Uhu** | Tarock II; Prämie für den vorletzten Stich. |
| **Ultimo** | „zum Schluss“ – bezieht sich auf den letzten Stich. |
| **Valat** | Alle zwölf Stiche einer Partei. |
| **Verlegen** | Karten nach der Talonaufnahme ablegen. |
| **Vogel (Vogerl)** | Pagat, Uhu, Kakadu, Quapil und die zugehörigen Prämien. |
| **Vorhand** | Der Spieler rechts vom Geber; spricht zuerst und spielt in positiven Spielen aus. |
| **Vorhandspiel** | Spiel, das nur die Vorhand ansagen darf: Trischaken, Rufer, Sechserdreier. |
| **Weiter (gut)** | Passen in der Lizitation bzw. Kenntnisnahme einer Ansage. |
| **Zähler** | Karte mit mehr als 1 Punkt (König, Trullstück, Dame, Reiter, Bube). |

### 11.7 Drei Übungsspiele mit festen Blättern

Alle drei Spiele sind vollständig durchgerechnet und regelgeprüft. Die Prüfung wurde am 12.09.2026 maschinell wiederholt: In jedem Spiel bilden die 4 × 12 Handkarten plus Talon genau die 54 Karten ohne Dubletten, jeder einzelne Zug erfüllt Farb-, Tarock- und (im Piccolo) Stichzwang samt Pagatregel, jeder Stichgewinner stimmt, und die Kartenpunkte summieren sich auf 210 Drittel. Sie eignen sich als Skript für den Lernmodus **und** als Regressionstest der Engine (10.10).

Spielrichtung immer gegen den Uhrzeigersinn: **Du → Berta → Carl → Dora → Du**.

---

#### Übungsspiel 1: „Mein erster Rufer“

**Lernziele:** Lizitation der Vorhand, König rufen, Talon und Verlegen, Farb- und Tarockzwang, Partner erkennen, Zählen, stille Prämien.

**Tisch:** Geberin ist **Dora**, also ist **Du** die Vorhand.

| Spieler | Tarock | ♥ | ♦ | ♠ | ♣ |
|---|---|---|---|---|---|
| **Du** (Vorhand) | XX, XVII, XI, VIII, V | K, B | 3 | D, 9 | K, 8 |
| Berta | Sk, XVI, XIII, IX, III | R, 2 | D, B | B | D, R |
| Carl | XIX, XV, XII, IIII, II | 1, 3 | K, 2 | K, 10 | 10 |
| Dora (Geberin) | XXI, XIV, X, VII, I | 4 | 1 | R, 8, 7 | B, 7 |
| **Talon** | Hälfte A: XVIII, VI, ♦4 · Hälfte B: ♥D, ♦R, ♣9 | | | | |

**1. Lizitation**
- Du: „**Vorhand**“. – *Dein Blatt: fünf Tarock, kein Trullstück, zwei Könige. Für einen Sechserdreier zu schwach, fürs Trischaken zu stark. Mit „Vorhand“ hältst du dir alles offen.*
- Berta: „weiter“ · Carl: „weiter“ · Dora: „weiter“.
- Du: „**Rufer**“. – *Jetzt darfst du jedes Spiel außer dem Sechserdreier wählen. Der Rufer ist das passende: billig, aber mit Partner.*

**2. König rufen:** „**Der Pik-König**“. – *Du hast in Pik die Dame und die 9. Man ruft am liebsten aus einer Farbe mit besetzter Dame: Wenn der Partner den König spielt, macht deine Dame später vielleicht auch noch einen Stich. Der Herz- und der Treff-König liegen in deiner Hand – die darfst du nicht rufen.*

**3. Talon:** Beide Hälften werden aufgedeckt.
- Hälfte A (XVIII, VI, ♦4) bringt **zwei Tarock**, darunter einen Stecher.
- Hälfte B (♥D, ♦R, ♣9) bringt **6 Punkte**, aber kein Tarock.
- **Du nimmst A.** – *Mit fünf Tarock bist du dünn besetzt; Tarock sind hier mehr wert als sechs Punkte. Hälfte B zählt am Ende zu den Gegnern – merk dir das beim Zählen!*

**4. Verlegen:** ♦3, ♦4 und ♥B.
- *Mit den beiden Karo bist du in dieser Farbe blank und kannst Karo künftig stechen. Der Herz-Bube bringt zwei Punkte sicher in deine Stiche.*
- Probiert der Lernende ♣K: `E_ABLAGE_KOENIG`. Probiert er XVIII: `E_ABLAGE_TAROCK_NOCH_FARBE`.

**5. Ansagen:** Du: „**Ich liege**“ (keine Prämie – du hast kein Trullstück und keinen Vogel). Berta, Carl, Dora: „gut“.

**6. Der Spielverlauf** (du spielst als Vorhand aus):

| Stich | Du | Berta | Carl | Dora | Stich geht an | Wert |
|---|---|---|---|---|---|---|
| 1 | **♣K** | ♣R | ♣10 | ♣7 | **Du** | 7/1 |
| 2 | **XI** | III | XIX | XXI | Dora | 5/1 |
| 3 | V | ♦B | ♦K | **♦1** | **Du** | 6/1 |
| 4 | **♠9** | ♠B | ♠K | ♠7 | Carl | 6/1 |
| 5 | XVIII | Sk | **XII** | VII | Berta | 5/1 |
| 6 | XVII | **♦D** | ♦2 | X | **Du** | 4/1 |
| 7 | **XX** | IX | II | XIV | **Du** | 1/1 |
| 8 | **♣8** | ♣D | XV | ♣B | Carl | 5/1 |
| 9 | ♥K | ♥2 | **♥3** | ♥4 | **Du** | 5/1 |
| 10 | **VIII** | XIII | IIII | I | Berta | 5/1 |
| 11 | VI | **♥R** | ♥1 | ♠8 | **Du** | 3/1 |
| 12 | **♠D** | XVI | ♠10 | ♠R | Berta | 6/1 |

(Die fett gesetzte Karte ist jeweils die ausgespielte.)

**Erklärungen Stich für Stich**
1. *Du spielst den Treff-König. In Treff hast du nur zwei Karten; je länger du wartest, desto eher sticht ihn jemand ab. Alle bedienen – 8 Punkte für dich.*
2. *Ein mittleres Tarock ist die Standarderöffnung („nie unter X“). Berta gibt klein zu, Carl legt mit XIX hoch vor – ein erstes Partnersignal. Dora sticht mit dem Mond.*
3. *Dora spielt Karo. Du hast keines mehr: **Tarockzwang**, du musst ein Tarock zugeben und nimmst das kleine V. Carl schmiert dir den Karo-König – jetzt weißt du: Carl ist dein Partner.* (Versucht der Lernende hier ♥K: `E_TAROCKZWANG`.)
4. *Du spielst in die gerufene Farbe. Carl macht den Stich mit dem gerufenen Pik-König – die Partnerschaft ist jetzt offen.*
5. *Carl bringt Tarock zurück (Spiegelprinzip). Du stichst hoch mit XVIII und lockst damit den Sküs heraus. Ab jetzt ist dein XXer das höchste Tarock im Spiel („Hausmeister“).*
6. *Berta spielt die Karo-Dame, Dora sticht mit X – du überstichst mit XVII und holst dir die Dame.*
7. *Mit dem Hausmeister ziehst du die letzten hohen Tarock heraus.*
8. *Nicht alles gelingt: Carl sticht die Treff-Dame ein. Das ist nicht schlimm, er ist dein Partner.*
9. *Carl spielt klein Herz, dein blanker Herz-König macht den Stich.*
10. *Dora muss den Pagat zugeben – es ist ihr letztes Tarock. Er fällt nicht im letzten Stich, also gibt es dafür keine Prämie, aber 5 Kartenpunkte für Berta.*
11. *Dora hat weder Herz noch Tarock und wirft ab; du stichst mit dem kleinen VI.*
12. *Letzter Stich: Berta hat kein Pik mehr und sticht deine Dame ein.*

**7. Zählen**
- Deine Partei (Du + Carl): 8 Stiche und die drei verlegten Karten = 35 Karten → **41 Punkte 2 Blatt** (Rechenweg in 1.7, Beispiel B).
- Gegner (Berta + Dora): 4 Stiche und die Talonhälfte B = 19 Karten → **28 Punkte 1 Blatt**.
- 41/2 ≥ 35/2 → **Spiel gewonnen.**

**8. Prämien**
- **Trull:** Sküs (Stich 5) und Pagat (Stich 10) liegen bei Berta, der Mond (Stich 2) bei Dora → die **Gegner haben still die Trull**: 1 Punkt für sie.
- **Alle Könige:** ♣K (1), ♦K (3), ♠K (4) und ♥K (9) – **alle vier in euren Stichen**: 1 Punkt für euch.
- König ultimo, Vögel: nichts, der Pagat fiel im 10. Stich.

**9. Abrechnung**

| Posten | Du | Berta | Carl | Dora |
|---|---|---|---|---|
| Rufer gewonnen (1) | +1 | −1 | +1 | −1 |
| stille Trull (Gegner, 1) | −1 | +1 | −1 | +1 |
| stille Vier Könige (1) | +1 | −1 | +1 | −1 |
| **Summe (Schrift und Geld)** | **+1** | **−1** | **+1** | **−1** |

**Lehre:** Das Spiel zu gewinnen ist nur die halbe Miete. Die Trull rutschte zu den Gegnern, weil du die drei Trullstücke nie eingefangen hast – ohne die vier Könige wäre das Spiel unterm Strich null gewesen.

---

#### Übungsspiel 2: „Piccolo – genau ein Stich“

**Lernziele:** Bieten aus der Hinterhand, Negativspiel, Stichzwang, Pagat-Regel, Schlusskarten, einzelnes Kontra.

**Tisch:** Geber ist **Du**, Vorhand ist **Berta**.

| Spieler | Tarock | ♥ | ♦ | ♠ | ♣ |
|---|---|---|---|---|---|
| **Du** (Geber) | Sk, XIX, IIII, I | 4, 2 | 4, 3 | 8, 7 | 9, 7 |
| Berta (Vorhand) | XVIII, XIV, X, VI | K, D, R | 2 | D, 10 | K, 8 |
| Carl | XX, XVII, XIII, IX, VII, V | 1, 3 | D, B | R | B |
| Dora | XXI, XVI, XV, XII, VIII, II | B | R, 1 | B, 9 | 10 |
| **Talon** | III, XI, ♦K, ♠K, ♣D, ♣R (bleibt unberührt liegen) | | | | |

**1. Lizitation**
- Berta: „Vorhand“ · Carl: „weiter“ · Dora: „weiter“.
- Du: „**Piccolo**“. – *Dein Blatt ist für ein positives Spiel wertlos: vier Tarock, sonst lauter kleine Farbkarten. Aber du hast in jeder Farbe die **Schlusskarte** (♥4, ♦4, ♠7, ♣7) und mit dem **Sküs** einen absolut sicheren Stich. Genau ein Stich – das ist der Piccolo.*
- Berta: „weiter“ → **du spielst Piccolo**, und du spielst auch aus (Negativspiel!).

**2. Ansagen:** „Ich liege.“ Berta „gut“, Carl „gut“, **Dora: „Kontra“** (sie hat sechs Tarock und glaubt, dir einen zweiten Stich anhängen zu können), Du „gut“, Berta „gut“, Carl „gut“.
- *Im Negativspiel kontriert jeder für sich: Nur zwischen Dora und dir verdoppelt sich das Geld.*

**3. Der Spielverlauf**

| Stich | Ausspiel | 2. Karte | 3. Karte | 4. Karte | Stich geht an |
|---|---|---|---|---|---|
| 1 | **Du XIX** | Berta VI | Carl XX | Dora XXI | Dora |
| 2 | **Dora XV** | Du Sk | Berta X | Carl V | **Du** |
| 3 | **Du IIII** | Berta XIV | Carl XVII | Dora II | Carl |
| 4 | **Carl ♣B** | Dora ♣10 | Du ♣9 | Berta ♣K | Berta |
| 5 | **Berta ♣8** | Carl VII | Dora VIII | Du ♣7 | Dora |
| 6 | **Dora ♠9** | Du ♠8 | Berta ♠10 | Carl ♠R | Carl |
| 7 | **Carl ♦B** | Dora ♦R | Du ♦3 | Berta ♦2 | Dora |
| 8 | **Dora ♦1** | Du ♦4 | Berta XVIII | Carl ♦D | Berta |
| 9 | **Berta ♠D** | Carl IX | Dora ♠B | Du ♠7 | Carl |
| 10 | **Carl ♥1** | Dora ♥B | Du ♥2 | Berta ♥R | Berta |
| 11 | **Berta ♥D** | Carl ♥3 | Dora XII | Du ♥4 | Dora |
| 12 | **Dora XVI** | Du I | Berta ♥K | Carl XIII | Dora |

**Erklärungen**
1. *Deine gefährlichste Karte zuerst: der XIXer. Wegen des **Stichzwangs** muss Carl mit dem XXer überstechen und Dora mit dem Mond – die Karte ist weg, ohne dass du einen Stich bekommst.* (Versucht der Lernende, den Pagat auszuspielen: `E_PAGAT_NEGATIV`.)
2. *Dora spielt XV. Du musst überstechen, wenn du kannst – möglich ist nur der Sküs. Dein Pflichtstich ist gemacht. Ab jetzt gilt: **kein Stich mehr**.* (Versuch mit IIII: `E_STICHZWANG_TAROCK`.)
3. *Du spielst zum zweiten und letzten Mal aus. Der IIIIer muss weg, solange andere noch Tarock haben. Danach ist der Pagat dein letztes Tarock und darf gespielt werden.*
4.–9. *Jetzt gilt nur noch: immer die **höhere** von zwei Karten einer Farbe abgeben und die Schlusskarte behalten. Deshalb ♣9 vor ♣7, ♠8 vor ♠7, ♦3 vor ♦4 (in Rot ist die 3 höher als die 4!). In diesem Blatt geht beides gut, weil vor dir jeweils schon eine höhere Karte liegt – aber wer die Schlusskarte verschenkt, sitzt im nächsten Durchgang in der Falle.*
10.–11. *Dieselbe Regel bei Herz: erst ♥2, dann ♥4. Warum es hier gefahrlos ist: Im 11. Stich sticht Dora mit XII ein, und eine Farbkarte kann ein Tarock nie überstechen – du darfst also jedes Herz zugeben. **Gefährlich wäre es, wenn du in einem Stich vor einem Gegner an der Reihe wärst**: Dann zwingt dich der Stichzwang, die höchste liegende Herzkarte zu überstechen, sofern du eine höhere Herzkarte hast. Genau dafür hebt man sich die Schlusskarte (♥4) bis zuletzt auf.*
12. *Der Pagat ist dein letztes Tarock und darf zugegeben werden. Er sticht nichts. Ergebnis: **genau ein Stich – Piccolo gewonnen.***

**4. Abrechnung**

| Konto | Du | Berta | Carl | Dora |
|---|---|---|---|---|
| Schrift (Piccolo 2, allein gegen drei) | **+6** | −2 | −2 | −2 |
| Geld (Doras Kontra verdoppelt nur ihren Anteil) | **+8** | −2 | −2 | −4 |

**Lehre:** Im Negativspiel ist die Reihenfolge der Abwürfe alles. Und: Kontra ändert die Turnierwertung nicht, nur das Geld.

---

#### Übungsspiel 3: „Besserrufer mit Pagat ultimo“

**Lernziele:** Bieten mit Vogel, Pflichtansage, Kontra, Reservierung der angesagten Karte, Tarock zählen, den Vogel heimspielen.

**Tisch:** Geberin ist **Berta**, Vorhand ist **Carl**. Reihenfolge: Carl → Dora → Du → Berta.

| Spieler | Tarock | ♥ | ♦ | ♠ | ♣ |
|---|---|---|---|---|---|
| Carl (Vorhand) | Sk, XX, XV, XII, VIII, III | D, 4 | R, 3 | B | 9 |
| Dora | XIV, VI, II | B, 3, 2 | D, 2 | R, 10, 9 | K |
| **Du** | XXI, XVIII, XVI, XIII, IX, VII, **I** | K | 4 | D, 8 | 10 |
| Berta (Geberin) | XVII, XI, X, IIII | 1 | B | K, 7 | R, B, 8, 7 |
| **Talon** | Hälfte A: XIX, V, ♦1 · Hälfte B: ♦K, ♣D, ♥R | | | | |

**1. Lizitation**
- Carl: „Vorhand“ · Dora: „weiter“.
- Du: „**Besserrufer**“. – *Sieben Tarock mit Mond, XVIII und XVI – dazu der **Pagat**. Das ist das klassische Blatt für einen Pagatrufer (Richtwert: ab sechs Tarock mit ein bis zwei Stechern). Ohne Vogel im Blatt dürftest du gar nicht Besserrufer sagen.*
- Berta: „weiter“ · Carl: „weiter“. – *Carl hätte mit sechs Tarock überbieten können, verzichtet aber: „Kontrieren ist einfacher.“*

**2. König rufen:** „**Der Pik-König**“ (du hast ♠D und ♠8). → Berta ist deine Partnerin, sagt aber nichts.

**3. Talon:** Hälfte A (XIX, V, ♦1) oder B (♦K, ♣D, ♥R, zusammen 10 Punkte).
- **Du nimmst A.** – *Beim Besserrufer zählt Tarock mehr als Punkte: Der XIXer ist ein zusätzlicher Stecher, mit dem du die Gegner leerspielst. Die 10 Punkte aus Hälfte B gehen an die Gegner – das ist der Preis.*

**4. Verlegen:** ♦4, ♦1 und ♣10.
- *Damit bist du in Karo und Treff blank („engfärbig“) und kannst diese Farben stechen. Die gerufene Farbe Pik behältst du – dort macht dein Partner mit dem König einen Stich.*

**5. Ansagen:** „**Besserrufer mit Pagat, der Pik-König hilft mir, ich liege.**“
- *Die Ansage des Vogels ist beim Besserrufer **Pflicht** – und zwar genau des Vogels, den du schon vor dem Talon hattest.* (Wer sie weglässt: `E_BESSERRUFER_VOGEL_PFLICHT`.)
- Berta: „gut“ – *Sie hat den gerufenen König nur zu zweit und vier kleine Tarock; für einen König ultimo zu wenig (Richtwert: König mindestens zu dritt und 3–4 Tarock mit Stecher).*
- **Carl: „Kontra auf den Pagat“** – *sechs Tarock mit Sküs und XXer, vierfärbig: ein klassisches Vogel-Kontra.*
- Dora „gut“, Du „gut“, Berta „gut“ → Spiel beginnt. Carl spielt als Vorhand aus.

**6. Der Spielverlauf**

| Stich | Ausspiel | 2. Karte | 3. Karte | 4. Karte | Stich geht an |
|---|---|---|---|---|---|
| 1 | **Carl ♦R** | Dora ♦2 | Du V | Berta ♦B | **Du** |
| 2 | **Du VII** | Berta XVII | Carl XX | Dora VI | Carl |
| 3 | **Carl ♣9** | Dora ♣K | Du IX | Berta ♣R | **Du** |
| 4 | **Du XIII** | Berta XI | Carl XV | Dora II | Carl |
| 5 | **Carl ♥4** | Dora ♥3 | Du ♥K | Berta ♥1 | **Du** |
| 6 | **Du XVIII** | Berta IIII | Carl III | Dora XIV | **Du** |
| 7 | **Du XIX** | Berta X | Carl VIII | Dora ♥2 | **Du** |
| 8 | **Du XVI** | Berta ♣7 | Carl XII | Dora ♠9 | **Du** |
| 9 | **Du ♠8** | Berta ♠K | Carl ♠B | Dora ♠10 | Berta |
| 10 | **Berta ♠7** | Carl Sk | Dora ♠R | Du ♠D | Carl |
| 11 | **Carl ♦3** | Dora ♦D | Du XXI | Berta ♣8 | **Du** |
| 12 | **Du I** | Berta ♣B | Carl ♥D | Dora ♥B | **Du** |

**Erklärungen**
1. *Carl spielt Karo an. Du hast keines mehr (verlegt): **Tarockzwang**. Das kleine V genügt, denn Berta bedient Karo.*
2. *Als Besserrufer tarockierst du – und zwar zuerst klein. Berta legt mit XVII hoch vor: „Ich bin deine Partnerin.“ Carl muss den XXer opfern.*
3. *Wieder Tarockzwang, wieder ein Stich für dich – samt Treff-König der Gegner.*
4. *Weiter Tarock: Jeder gezogene gegnerische Tarock bringt dich näher an den Pagat-Stich.*
5. *Dein blanker Herz-König geht durch.*
6.–8. *Drei Stiche hintereinander mit deinen Stechern. **Mitzählen:** Nach Stich 8 sind alle Tarock außer deinem Mond, deinem Pagat und Carls Sküs gefallen.*
   - *Wichtig ist jetzt, **nicht** den Mond auszuspielen: Carl müsste mit dem Sküs stechen und käme ans Ausspielen. Er würde Karo oder Herz spielen – beides hast du nicht mehr, du müsstest mit dem **Pagat** stechen, und der Pagat wäre verloren.*
9. *Stattdessen spielst du deine Pik-Acht in die gerufene Farbe. Berta macht den Stich mit dem gerufenen König und kommt ans Ausspielen.*
10. *Berta spielt Pik nach. Carl hat kein Pik mehr und **muss** mit dem Sküs stechen (Tarockzwang) – sein letztes Tarock ist weg. Deine Pik-Dame ist der Preis dafür; das Spiel ist dir sicher.*
11. *Carl spielt Karo, du bist blank. Tarockzwang – aber **nicht mit dem Pagat**: Der ist angesagt und für den letzten Stich gedacht. Du stichst mit dem Mond.* (Versuch mit I: `W_VOGEL_RESERVIERT` – erlaubt wäre es, aber der angesagte Pagat wäre verloren; da hier außer deinem Mond und Carls schon gefallenem Sküs kein höheres Tarock mehr im Spiel ist, würde der Pagat den Stich sogar gewinnen: `E_VOGEL_STICHT_ZU_FRUEH`.)
12. *Kein Gegner hat mehr ein Tarock. Der Pagat macht den letzten Stich: **Pagat ultimo gelungen** – trotz Kontra.*

**7. Ergebnis und Abrechnung**
- Kartenpunkte: Du + Berta **47/0**, Carl + Dora **23/0** (darin Hälfte B mit ♦K, ♣D, ♥R) → Spiel gewonnen.
- Pagat angesagt und gemacht, von Carl kontriert.
- Trull: Der Sküs liegt bei Carl → keine Trull. Vier Könige: Der Karo-König lag im Talon und zählt zu den Gegnern → nein.

| Posten | Du | Berta | Carl | Dora |
|---|---|---|---|---|
| Besserrufer gewonnen (1) | +1 | +1 | −1 | −1 |
| Pagat angesagt (2) | +2 | +2 | −2 | −2 |
| **Schrift** | **+3** | **+3** | **−3** | **−3** |
| **Geld** (Kontra verdoppelt nur den Pagat) | **+5** | **+5** | **−5** | **−5** |

**Lehre:** Ein Vogel wird nicht im letzten Stich gewonnen, sondern in den Stichen 8 bis 11 – durch Tarockzählen und die richtige Reihenfolge. Hätte Carl statt des Sküs ein kleines Tarock behalten, wäre der Pagat kaum heimgekommen.

---

## 12. Prüfprotokoll (unabhängige Quellenprüfung, 12.09.2026)

### 12.1 Was geprüft wurde

Alle Regeln, Werte und Abläufe dieses Dokuments wurden gegen die **Volltexte** der in Anhang A genannten Primärquellen geprüft, nicht gegen Zusammenfassungen:

| Quelle | Form der Prüfung |
|---|---|
| [OÖ23] | PDF von haudum.at geladen, Volltext extrahiert, **jeder** `[OÖ23 …]`-Verweis Satz für Satz abgeglichen. Metadaten: Autor „KIENAST Franz“, erstellt 13.09.2023 |
| [OÖR25], [OÖT26] | PDFs der Linzer Tarock Akademie, Volltext (7 bzw. 1 Seite) |
| [LTA-GK1], [LTA-GK2], [LTA-F1] | PDFs, Volltext |
| [ÖF21] | Turnierregelseite tarockoesterreich.jimdofree.com, Volltext |
| [PAGAT], [WIKI] | Volltext |
| [MAIR-CUPS], [MAIR-AUSL], [MAIR-TURNIER] | PDFs, Volltext |
| [KTC], [STMK] | PDFs, Volltext |
| [HRC] | Seite abgerufen: nennt weiterhin Regeln **4/2023**, Renonceregeln **11/2025**, Taroquette **4/2026** |
| Übungsspiele 11.7 | maschinell nachgespielt (Kartenvollständigkeit, Zuglegalität, Stichgewinner, Punkte) |
| Abrechnungsbeispiele 7.6, 7.10 | jede Zeile nachgerechnet |

### 12.2 Korrekturen gegenüber der Fassung vom 11.09.2026

| Nr. | Stelle | Was falsch/unvollständig war | Korrektur |
|---|---|---|---|
| K-1 | 5.4, 6.5, 6.7, 9.3 **P-2**, 11.4, 11.5 | **Sachlicher Regelfehler.** Die Aufbewahrungspflicht für angesagte Vögel und den angesagten König ultimo war als **harte Sperre** umgesetzt. [OÖ23 Ansagen 6] („Wird eine Ansage zum falschen Zeitpunkt gespielt, so gilt diese als verloren. Eine Renonce liegt aber nur dann vor, wenn sich ein Spieler dadurch einen offensichtlichen Vorteil verschafft“), [ÖF21 Ansagen] und [OÖR25 B.2] behandeln das vorzeitige Spielen ausdrücklich als bloßen Prämienverlust; auch die strengere Tiroler Quelle rechnet es in [MAIR-AUSL] so ab. Die Sperre nahm dem Spieler eine regelkonforme Option | Reservierung ist jetzt **weich** (Warnung, Prämie verloren). Hart bleiben: vorzeitiger **Stichgewinn** mit der angesagten Karte, die Reihenfolge „höherer Vogel zuerst“ im Zwangsfall und die Spielpflicht im Zielstich. Neue Option `harteVogelreservierung` (Standard aus) |
| K-2 | 6.5, 6.7 | Die neue Sperre darf keine verdeckte Information verraten | `wuerdeStichSicherGewinnen()` darf **nur** öffentlich bekannte Karten auswerten; im Zweifel ist der Zug erlaubt (Verweis auf 10.8) |
| K-3 | 7.6, 9.6 **T-7** | Die Regel „zwei Jungfrauen teilen“ war ohne Hinweis darauf übernommen, dass [LTA-GK1 Kap. 11] („je drei Punkte“) und [WIKI] („je 30 Cent“) das **anders** sehen | [ABWEICHUNG] ergänzt, [OÖ23] als Primärquelle bestätigt, [KTC] als weitere Stütze ergänzt, Option `jungfrauenVoll` |
| K-4 | 8.1, 9.7 **M-1** | **Falsches Zitat.** „Im Tiroler Cup sind es 20 [Spiele je Durchgang am Fünfertisch]“ steht so nicht in [MAIR-CUPS]; dort ist die Spalte mit „Spiele pro Runde (**4 Spieler**)“ überschrieben und nennt für Tirol 16 | Zitat entfernt; die Zahl für Fünfertische ist in **keiner** Quelle belegt, die Empfehlung 20 bleibt als solche gekennzeichnet |
| K-5 | 2.6 | „Radl … 5 Spiele am Fünfertisch [WIKI]“ – [WIKI] sagt nur „Vier Spiele bilden eine Runde oder ein Radl“ | Als **[EMPFEHLUNG]** gekennzeichnet |
| K-6 | 3.4, 3.8 | **Falsches Zitat.** [OÖ23 Allg. 2] kennt als Passwort nur **„gut“**, nicht „weiter“ | Richtiggestellt; „weiter“ mit [LTA-GK1 Kap. 6], [WIKI], [KTC] belegt |
| K-7 | 1.5 | „Eine übrige Karte zählt ⅔ weniger … [WIKI; PAGAT]“ – [PAGAT] sagt für **eine wie zwei** Restkarten „one point less“ (gerundete Tischnäherung) | Zuordnung richtiggestellt, Näherung erläutert |
| K-8 | 1.6 | „In Teilen der Steiermark genügen 35/1“ ohne Hinweis, dass das Cup-Regelwerk [STMK] selbst 35/2 verlangt | Ergänzt |
| K-9 | 2.7 | [OÖT26 7] für „Mimik/Gestik“ – das steht in Punkt **6**; Punkt 7 betrifft Kiebitze und den Fünften | Beide Fundstellen richtig zitiert |
| K-10 | 9.5 **S-8** (neu) | [PAGAT] widerspricht sich, wer im Farbensolo ausspielt; das war nicht vermerkt | Neue Zeile S-8 mit Beleg aus [OÖ23 Farben 1] |
| K-11 | 9.3 **P-12** (neu) | Offen war, ob die Regel „höherer Vogel zuerst“ den angesagten König ultimo einschließt | [OÖ23 Ansagen 7] und [ÖF21] nennen nur Vögel → König bleibt außerhalb der Reihenfolge |
| K-12 | 9.8 | Der pagat-Vergleich nannte die Einschränkung „Ansage nur bei bekannter Partei“ nicht; die Tirol-Zeile „Trischaken-Talon“ gab [MAIR-CUPS] verkürzt wieder (dort sind **beide** Varianten angekreuzt) | Beides ergänzt bzw. mit Fußnote versehen |
| K-13 | 11.7, Übungsspiel 2, Erklärung zu Stich 10/11 | **Sachlich falsche Begründung.** „Hättest du die ♥2 behalten, hättest du im 11. Stich über Carls ♥3 stechen müssen“ – im 11. Stich liegt bereits Doras XII; eine Farbkarte kann ein Tarock nie überstechen, beide Herz wären erlaubt | Begründung ersetzt; das richtige Prinzip (Schlusskarte aufheben, weil man in einem Stich vor einem Gegner sitzen kann) erklärt |
| K-14 | 11.7, Übungsspiel 3, Stich 11 | Fehlercode `E_VOGEL_RESERVIERT` als Verbot dargestellt | Auf die neue Logik umgestellt (`W_VOGEL_RESERVIERT` bzw. `E_VOGEL_STICHT_ZU_FRUEH`, weil hier alle höheren Tarock öffentlich gefallen sind) |
| K-15 | 0.1, 1.8, 2.6, 5.1, 5.4, 7.7, 9.6 u. a. | Mehrere Belege waren nur mit Kürzel angegeben | Wörtliche Belegstellen eingesetzt, damit sie ohne Rückgriff auf die Quelle nachprüfbar sind |

### 12.3 Bestätigt (keine Änderung nötig)

- **Tarif und Lizitationsreihenfolge** (3.1, 7.2, 7.5): identisch mit der Kopftabelle von [OÖ23] – Trischaken 1, Rufer 1, Piccolo 2, Bettel 2, Solorufer 2, A-Rufer 1+, Farbendreier 3, Sechserdreier 4, Dreier 4, Farbensolo 6, Piccolo ouvert 6, Bettel ouvert 7, Solodreier 8; Prämien König Ultimo 2, Trull 2, Alle Könige 2, Pagat 2, Uhu 4, Kakadu 6, Quapil 8, Valat 8-fach.
- **Sechserdreier: Verlust doppelt.** In [OÖ23] selbst nicht ausgesprochen, aber durch drei unabhängige Quellen gedeckt: [ÖF21 Vorhand] „Beim Verlust des Sechserdreiers zählt das Spiel doppelt“, [LTA-GK1 Kap. 10] „bei Verlust zahlt der Spieler doppelt“, [STMK] „Ein verlorener Sechserdreier zählt doppelt (nur das Spiel, nicht die Prämien)“ – und rechnerisch durch [OÖR25 B.4]: „Renonce eines Gegenspielers beim Sechser-Dreier: Der Spieler erhält 3 × 4 Punkte …, die regulären Gegner erhalten je 8 Punkte.“
- **Gewinnschwelle 35/2 = 107 Drittel** [OÖ23 Allg. 3 „36 Punkte (= 35 Punkte und 2 Blatt)“].
- **Still = die Hälfte, in Solospielen doppelt** [OÖ23 Ansagen 1]; nachgerechnet an [OÖR25 B.4] („stiller Kakadu 3 Punkte“, „stiller Uhu im Solorufer 4 Punkte“, „angesagter König im Solorufer 4 Punkte“).
- **Valat wird in Solospielen nicht zusätzlich verdoppelt** (9.4 V-6): [LTA-GK1 Kap. 9], ausdrücklich [STMK] „Valat bleibt aber 8-fach/4-fach“, rechnerisch [MAIR-CUPS] (angesagter Valat im Solorufer = 16 = 2 × 8).
- **Beim Valat zählen nur angesagte Prämien** [OÖ23 Ansagen 2; ÖF21 Valat; LTA-GK1 Kap. 12; MAIR-CUPS Fußnote „Kriterium angesagt … OÖ ja“].
- **Verlorener angesagter Valat beim Sechserdreier = 64 je Gegner** [ÖF21 „beim Sechserdreier 8 mal 8 Punkte“; MAIR-CUPS „Valat verloren, Sechserdreier −192“].
- **Kontra wirkt nur aufs Geld** [OÖ23 Allg. 5], wirkt auch auf den Valatposten [LTA-F1 Kap. 1].
- **In Farbenspielen nur Alle Könige und Valat** [LTA-GK1 Kap. 10 für beide Farbenspiele; LTA-F1; ÖF21]; rechnerisch bestätigt durch [MAIR-CUPS].
- **Gerufener König im Talon** (4.5, 7.7): Wortlaut von [OÖ23 Allg. 7] vollständig abgedeckt, einschließlich „Kontra beim Solorufer wirkungslos“ [zusätzlich LTA-F1 Kap. 3: „War das Spiel geschossen, so zählt dieser Schuss nicht. Das gilt auch für den Fall, dass der Spieler gewonnen hat“].
- **Schleifen zahlt 3 × (Spiel + Vogel)** (9.2 R-3): gestützt auf die Parallelstellen [ÖF21 Rufen] („an alle 3 Spieler Spiel + Pagat (3 Punkte)“) und [OÖR25 B.4] („je drei Punkte an die Mitspieler … 9 Minuspunkte“).
- **Verlegen** (4.6): [OÖ23 Renonce 2] und [OÖ23 Farben 1] wörtlich; [LTA-GK1 Kap. 7] und [PAGAT] deckungsgleich.
- **Zugregeln** (6.2, 6.3, 6.4): [OÖ23 Allg. 3; Farben 1–2; Negativ 1–2; Trischaken 1] wörtlich; [LTA-GK1 Kap. 8] als Erläuterung.
- **Renonce-Werte** (7.9): jede Zahl gegen die Rechenbeispiele in [OÖR25 B.4] geprüft.
- **Trischaken-Wertung** (7.6): Grundbeträge, Bürgermeister, Trischaker-Verdopplung, zwei bzw. drei Punktegleiche und der Sonderfall 35/1:34/2 stimmen mit [OÖ23 Trischaken 2–3] und [ÖF21 Trischaken] überein; alle neun Beispielzeilen nachgerechnet (Summe stets 210 Drittel).
- **Abrechnungsbeispiele 7.10**: alle 16 Zeilen nachgerechnet, keine Abweichung.
- **Übungsspiele 11.7**: alle drei Blätter sind vollständig und dublettenfrei; alle 36 Stiche sind regelkonform; die Endstände 41/2 : 28/1 (Spiel 1) und 47/0 : 23/0 (Spiel 3) sowie „genau ein Stich“ (Spiel 2) stimmen exakt.
- **Regelprofil aktuell:** [HRC] nennt am 12.09.2026 weiterhin 4/2023 (+ Renonceregeln 11/2025, Taroquette 4/2026).

### 12.4 Was unsicher bleibt

1. **Spiele je Durchgang am Fünfertisch** (M-1) – in keiner Quelle belegt. Die App braucht hier eine Einstellung.
2. **Misslungene stille Prämie** (P-5) – [OÖ23] schweigt. Für den König ultimo spricht [ÖF21 Rufen] klar gegen ein Minus („Der Verlust des gerufenen Königs wird nicht bewertet, es sei denn er wird ultimo angesagt“); für die Vögel fehlt eine ausdrückliche Stelle, [PAGAT] handhabt es umgekehrt. Empfehlung „kein Minus“ bleibt, Option `stillVerlustZaehlt`.
3. **Talon beim Valat** (V-3) – [MAIR-AUSL] („Talon gehört den Gegnern, auch wenn diese keinen Stich haben“) ist eine Tiroler Auslegung; [OÖ23] sagt dazu nichts. Empfehlung unverändert, Option `valatZiehtTalon`.
4. **Vorhand hält einen Solodreier** (L-4) – [OÖ23 Vorhand 2] und [Ansagen 3] lassen beides zu; [MAIR-AUSL] begründet die Sofort-Ansage nur damit, dass der Solodreier „nicht mehr überboten werden kann“, was das Halten nicht ausdrücklich ausschließt.
5. **Kontra-Zeitfenster** (P-9) – nur durch [KTC] und [STMK] belegt, nicht durch [OÖ23].
6. **Kontra unter verdeckten Partnern** (P-4) – [OÖ23] schweigt; [KTC], [STMK] und [MAIR-AUSL] stimmen überein, deshalb übernommen.
7. **Angesagter Valat der Gegenpartei misslingt** (V-2) – nirgends geregelt; spiegelbildliche Festlegung.
8. **Zwei Jungfrauen beim Trischaken** (T-7) – die Quellen widersprechen sich offen (siehe K-3).
9. **Talon-Zuordnung beim Solodreier** – dass der ganze Talon den Gegnern zufällt, sagt [OÖ23 Farben 3] ausdrücklich nur für das Farbensolo; für Solorufer folgt es mittelbar aus [OÖ23 Allg. 7], für den Solodreier aus [LTA-GK1 Kap. 9/10] und [MAIR-AUSL]. Inhaltlich unstrittig, im OÖ-Regeltext aber nicht ausformuliert.
10. **Reihenfolge der Restkarten in [MAIR-CUPS]**: In der Spalte „stiller Valat + stille Zusätze“ addiert Mair bei den Farbenspielen stille vier Könige hinzu, obwohl nach [OÖ23 Ansagen 2] beim Valat nur angesagte Prämien zählen. Das ist vermutlich ein Fehler seiner Vergleichstabelle; dieses Dokument folgt [OÖ23].

---

## Anhang A: Quellen

| Kürzel | Quelle |
|---|---|
| **[OÖ23]** | *Version 4/2023 Oberösterreichische Tarockregeln – Königrufen.* PDF, Raiffeisen Tarockcup Austria: https://www.haudum.at/userupload/editorupload/files/tarockcup/dokumente/Spielregeln_Tarockcup.pdf (abgerufen 11.09.2026; PDF-Erstellung 13.09.2023). Inhaltsgleich: Hausruckviertler Tarockcup, „OÖ-Tarockregeln_4_2023_HRC.pdf“, und Linzer Tarock Akademie, „OÖ-Tarockregeln_2023.pdf“ |
| **[HRC]** | Hausruckviertler Tarockcup, Seite „OÖ-Tarockregeln 4/2023“: https://hausruckcup1.jimdofree.com/organisation/turnierregeln-4-2023/ (abgerufen 11.09.2026) – nennt die drei OÖ-Cups und verlinkt [OÖR25], [OÖT26] |
| **[OÖR25]** | *Die oberösterreichischen Renonce-Regeln und ihre verbindliche Auslegung, Stand 11/2025.* https://www.linzer-tarock-akademie.at/fileadmin/user_upload/skriptum/O%C3%96-Renonce_2025pdf.pdf |
| **[OÖT26]** | *Taroquette Oberösterreich, Stand 04/2026.* https://www.linzer-tarock-akademie.at/fileadmin/user_upload/skriptum/O%C3%96_Taroquette_2026.pdf |
| **[LTA-GK1]** | Linzer Tarock Akademie: *Königrufen – Grundkurs 1*, Stand 01/25. https://www.linzer-tarock-akademie.at/fileadmin/user_upload/skriptum/a_Skriptum_GK1_2025_1.pdf |
| **[LTA-GK2]** | dieselbe: *Königrufen – Grundkurs 2*, 07/23. https://www.linzer-tarock-akademie.at/fileadmin/user_upload/skriptum/Skriptum_GK2_-_2023.pdf |
| **[LTA-F1]** | dieselbe: *Königrufen – Fortgeschrittene 1*, 09/23. https://www.linzer-tarock-akademie.at/fileadmin/user_upload/skriptum/Skriptum_F1_2023.pdf |
| **[LTA-S]** | dieselbe: *Königrufen – Spielstrategien*, 01/26. https://www.linzer-tarock-akademie.at/fileadmin/user_upload/skriptum/Skriptum_3_Strategien_2026.pdf |
| **[ÖF21]** | *Die Turnierregeln beim Tarock-Österreich-Finale (Juni 2021)*, Franz Kienast. https://tarockoesterreich.jimdofree.com/tarock-%C3%B6sterreich-finale/%C3%B6f-turnierregeln-2021/ |
| **[ÖF10]** | dieselben, Fassung 2010 (Archivkopie). https://web.archive.org/web/20101213015041/http://tarockoesterreich.jimdo.com/tarock-%C3%B6sterreich-finale/turnierregeln/ |
| **[PAGAT]** | John McLeod: *Königrufen*, pagat.com, letzte Änderung 01.09.2026. https://www.pagat.com/tarot/koenig.html |
| **[PAGAT-SK]** | Matthew Macfadyen: *Königrufen – remarks on skilful play*. https://www.pagat.com/tarot/krufsk.html |
| **[WIKI]** | Wikipedia (de): *Königrufen*, abgerufen 11.09.2026. https://de.wikipedia.org/wiki/K%C3%B6nigrufen |
| **[MAIR-CUPS]** | Markus Mair: *Unterschiede Cupregeln*, März 2026. http://www.tarock.tirol/tarock-regeln/tarock-regelvergleich-cups.pdf |
| **[MAIR-GRUND]** | derselbe: *Regelvergleich – Grundsätzliche Überlegungen*, 2024. http://www.tarock.tirol/tarock-regeln/tarock-regelvergleich-grundsaetzliches.pdf |
| **[MAIR-LIZ]** | derselbe: *Vergleich Lizitationstabellen Cups*. http://www.tarock.tirol/tarock-regeln/tarock-vergleich-lizitationstabellen.pdf |
| **[MAIR-AUSL]** | derselbe: *Regelauslegung / Erläuterungen*, September 2026 (Tiroler Auslegung). http://www.tarock.tirol/tarock-regeln/tarock-regeln-auslegung.pdf |
| **[MAIR-TURNIER]** | derselbe: *Turnierregeln TAROCK – Königrufen*, April 2026 (Tirol). http://www.tarock.tirol/tarock-regeln/tarock-regeln-turnier.pdf |
| **[MAIR-KURS]** | derselbe: *TAROCK Königrufen – Kursregeln*, März 2025. http://www.tarock.tirol/tarock-regeln/tarock-regeln-kurs.pdf |
| **[MAIR-FIBEL]** | derselbe: *Tarockfibel*, 2025. http://www.tarock.tirol/tarock-regeln/tarock-fibel.pdf |
| **[MAIR-MERK]** | derselbe: *Tarockmerksätze*, März 2025. http://www.tarock.tirol/tarock-regeln/tarock-merksaetze.pdf |
| **[MAYR-KOMPAKT]** | Wolfgang Mayr: *Königrufen kompakt – Gebote und Binsenweisheiten*. http://www.tarock.tirol/tarock-regeln/others/mayr-koenigrufen-kompakt.pdf |
| **[KTC]** | *Tarife und Regeln des Kärntner Tarockcups 2023/24*. https://xn--krntner-tarockcup-qqb.at/wp-content/uploads/2023/03/KTC-Regelwerk-2023-24.pdf |
| **[STMK]** | *Die Regeln des Steirischen Tarockcups 2017/2018*. https://tarocksteiermark.wordpress.com/wp-content/uploads/2017/05/regeln-steirischer-tarock-cup_2017_2018.pdf |
| **[PEPI]** | Pepi Tichler: *Spielregeln Königrufen* (private, an OÖ angelehnte Runde). https://www.pepitichler.at/spielregeln/ |

Weitere Literatur (nicht ausgewertet, für spätere Vertiefung): Wolfgang Mayr / Robert Sedlaczek, *Die Strategie des Tarockspiels*, 4. Auflage 2014; Martin Vácha, *Handbuch Tarock*, 2015; Dummett/McLeod, *A History of Games Played with the Tarot Pack*, 2004.

## Anhang B: Offene Punkte auf einen Blick

Diese Punkte sind in den Quellen nicht oder nicht eindeutig geregelt. Für jeden gibt es oben eine Festlegung; jede sollte im Code als benannte Option sichtbar sein.

| ID | Kurz | Festlegung | Option |
|---|---|---|---|
| L-1 | Vorhand darf nicht passen | so umsetzen | – |
| L-4 | Vorhand hält Solodreier | erlaubt | `solodreierBeendetLizit` |
| R-2 | Welcher Vogel beim Schleifen | Spieler benennt, Vorschlag niedrigster | – |
| R-3 | Schleifen zahlt 3× | ja | – |
| R-4 | König ultimo nach Talonaufnahme des gerufenen Königs | erlaubt | – |
| R-5 | Alleinspiel nach „König im Talon“ | 1 gegen 3 | – |
| P-1 | Vogel im Zielstich nicht spielbar | Prämie verloren, Karte frei | – |
| **P-2** | **Aufbewahrungspflicht für angesagte Karten** | **keine harte Sperre; nur der vorzeitige Stichgewinn ist verboten** | `harteVogelreservierung` |
| P-4 | Kontra unter Partnern | zählt nicht | – |
| **P-5** | **Misslungene stille Prämie** | **kein Minus** | `stillVerlustZaehlt` |
| P-6 | Prämienart nur einmal | ein Posten | – |
| P-9 | Kontra-Zeitfenster | sofort | – |
| P-12 | König ultimo in der Vogelreihenfolge | nicht einbezogen | – |
| V-1 | Kontra und Valat zugleich | nur außerhalb der Farbenspiele | – |
| V-2 | Gegner-Valat misslungen | 8-facher Spielwert von den Ansagern | – |
| **V-3** | **Talon beim Valat** | **bleibt bei den Gegnern** | `valatZiehtTalon` |
| S-7 | Eigene Stiche ansehen | erlaubt | `sticheAnsehen` |
| S-8 | Farbenspiel, erstes Ausspiel | Vorhand (pagat widersprüchlich) | – |
| T-1 | Drei Punktegleiche beim Trischaken | je −1 | – |
| T-2 | Jungfrau bei zwei Verlierern | erhält alles | – |
| **T-7** | **Zwei Jungfrauen beim Trischaken** | **teilen den Gewinn ([OÖ23])** | `jungfrauenVoll` |
| M-1 | Spiele je Durchgang am Fünfertisch | 20 | einstellbar |
| 2.6 | Radl am Fünfertisch = 5 Spiele | so umsetzen (nicht belegt) | – |
