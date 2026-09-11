# Ungarisches Tarokk – „Illusztrált tarokk" (XX‑hívás)
## Implementierungsspezifikation für harbour‑tarock (Regelprofil `HU-ILLU-ITVB-2019`)

Stand: 12.09.2026 · Zielplattformen: Sailfish OS (Qt 5.6 / QtQuick 2.6 / Silica), Android (Qt 6.11) · Engine: C++17

---

## 0. Vorbemerkungen, Quellenlage, Regelprofile

### 0.1 Was hier spezifiziert wird

Spezifiziert wird **Illusztrált tarokk** (auch *Palatinusz tarokk*, „12‑figurás tarokk"), also die heute in
ungarischen Vereinen und auf den Landeskupas gespielte Ausbaustufe des Grundspiels **Paskievics tarokk**
(= *húszashívásos tarokk*, „XX‑Ruf‑Tarokk", 6 Figuren). Illusztrált = Paskievics **plus** sechs weitere, nur
angesagt gültige Figuren (centrum, kismadár, nagymadár, pagát uhu, király uhu, király ultimó); die
Turnierregeln des ITVB ergänzen zusätzlich sas ultimó und sas uhu.

Die Abweichungen des Grundspiels Paskievics sind in §12 gesammelt („Paskievics‑Profil").

### 0.2 Quellen (vollständige URLs in §13)

| Kürzel | Quelle | Rolle in dieser Spezifikation |
|---|---|---|
| **[ITVB]** | *Országos Nyílt Illusztrált Tarokk Kupák versenyszabályzata / Etikai kódex*, Version 4.0, gültig ab 01.01.2019, hrsg. vom **Illusztrált Tarokk Versenyrendező Bizottság (ITVB)**, veröffentlicht auf tarokk.hu (Tarokk Regiszter) | **Primäre Normquelle** für den App‑Default. Regelt genau die Punkte, die „die einzelnen Tarokk‑Kreise unterschiedlich spielen" (ITVB 1.3) |
| **[BTE]** | *Budapesti Tarokk Egyesület Játékszabályzata*, gültig ab 01.01.2016 | Vereins‑Regelwerk, in allen relevanten Punkten deckungsgleich mit [ITVB] |
| **[TA48]** | *Tarokkakadémia II. könyv – Az illusztrált tarokkjáték szabályainak részletes ismertetése és magyarázata*, Fazekas B. / Kovács E. / Szigetvári Z., Centrum, Budapest 1948 (Scan auf tarokk.hu) | **Hintergrundnorm**: [ITVB 1.3] erklärt ausdrücklich die Ausgabe 1948 der *Tarokkakadémia* zur subsidiären Regelquelle („Mögöttes szabályként a Tarokkakadémia 1948‑as kiadása szolgál"). Enthält u. a. die klassische „Számítási táblázat" (Wertungstabelle). **⚠ Das Digitalisat ist ein passwortgeschützter Bild‑Scan ohne Textebene; die Seitenzitate dieser Spezifikation konnten nicht maschinell verifiziert werden** (Prüfprotokoll §15). Alle TA48‑gestützten Aussagen sind deshalb entweder durch eine zweite Quelle gedeckt oder als unbestätigt markiert |
| **[MKVM]** | *Az illusztrált tarokk játékszabályai*, mkvm.hu. **Achtung:** MKVM = **Magyar Kereskedelmi és Vendéglátóipari Múzeum** (Museum, heute Teil des Ungarischen Nationalmuseums) – **kein** Spielerverband. Der Text ist laut Fußnote von **kartya-jatek.hu** (Sammlung Jánoska Antal) übernommen | Moderne, gut lesbare Gesamtdarstellung inkl. Wertungstabelle; vom Nutzer als Leitquelle benannt |
| **[PAG-I]** | pagat.com, *Illustrated Hungarian Tarokk* (J. McLeod, Stand 19.04.2022) | Detaillierteste Darstellung in westlicher Sprache; Grundlage der Wertungs‑Multiplikatortabelle in §7 |
| **[PAG-P]** | pagat.com, *Hungarian Tarokk* (Paskievics/XX‑hívás, Stand 01.09.2026) | Grundregeln, Varianten, Taktik |
| **[PAG-EX]** | pagat.com, *Illustrated Hungarian Tarokk: Example Deals* (Révész Gábor) | Beispielabrechnungen zur Verifikation |
| **[SURVEY]** | Zsigri Gyula, Umfrage unter 11 ungarischen Spielerkreisen (1997), auf pagat.com | Belegt Mehrheitsverhältnisse bei strittigen Regeln |
| **[KEGL]** | Kégl Lóránt, *Tarokk page for advanced players* + *Standardization of the Rules of Paskievics Tarokk* + *tarokk_basic.pdf* | Moderne Standardisierungs‑ und Strategiequelle (Paskievics‑Schule); Grundlage vieler KI‑Heuristiken |
| **[PVSZ18]** | *Paskievics tarokkversenyeken alkalmazandó versenyszabályok*, gültig ab 01.03.2018 (tarokk.hu) | Paskievics‑Turnierregeln; wichtig für §12 |
| **[HUWIKI]** | hu.wikipedia.org, *Tarokk* | Gegenprobe; bei Grundregeln zuverlässig, bei zwei Figurenwerten Ausreißer (siehe §9.22) |
| **[UZO]** | Uzonyi Ákos, Android‑App „Tarokk" (`com.tisza.tarock`), Quellcode GitHub `AkosUzonyi/Tarock`, **GPL‑3.0** | Nur als *Implementierungs‑Gegenprobe* gelesen. **Kein Code übernehmen** (GPL‑3.0 ist mit einer permissiven Lizenzierung unserer App unvereinbar) |

Zitierweise im Folgenden: `[ITVB 5.3]`, `[PAG-I §Bonuses]`, `[TA48 S. 61]` usw.

### 0.3 Regelprofile in der App

Die Engine implementiert das Regelwerk als **Profil** (Datenstruktur, kein `#ifdef`). Auslieferung:

| Profil‑ID | Beschreibung | Status |
|---|---|---|
| `HU-ILLU-ITVB-2019` | Illusztrált nach [ITVB] 4.0 + [TA48] als Hintergrundnorm | **Default** |
| `HU-ILLU-PAGAT` | Illusztrált nach [PAG-I] (u. a. XX drückbar, pagát uhu 20, Ultimó→Uhu‑Hochstufung erlaubt) | Option |
| `HU-PASK-ITVB` | Paskievics (6 Figuren) nach [PVSZ18]/[KEGL] | Option (§12) |

Jeder in §9 aufgelistete strittige Punkt ist ein **einzelnes Profil‑Flag**, damit Spielkreise ihre Hausregeln
abbilden können. Jede Regel dieses Dokuments, die profilabhängig ist, trägt die Markierung **⚙ Flag:
`flag_name`**.

### 0.4 Sprachliche Konventionen

Ungarische Fachbegriffe bleiben im Original (siehe Glossar §11.7). Kartenkürzel in dieser Spezifikation:

* Tarock: `I … XXI` (römisch, IIII statt IV wie auf den Karten) und `Skíz`.
* Farben: `♥` kőr (Herz), `♦` káró (Karo), `♣` treff (Kreuz), `♠` pikk (Pik).
* Ränge: `K` király (König), `D` dáma (Dame), `L` lovas (Reiter/Cavall), `B` bubi/botos (Bube),
  `A` ász (Ass, nur rot), `10` tízes (Zehn, nur schwarz).
* Beispiel: `♥K` = kőr király, `♠10` = pikk tízes.

---

## 1. Karten, Rangfolgen, Kartenwerte, Zählung

### 1.1 Das Blatt: 42 Karten

Gespielt wird mit dem 42‑Blatt‑Tarokk, das aus dem handelsüblichen 54‑Blatt‑Tarokkblatt (Industrie und Glück)
hergestellt wird: **die 2, 3, 4 der roten Farben und die 7, 8, 9 der schwarzen Farben werden entfernt**
[PAG-P §Cards], [MKVM], [HUWIKI]. Es bleiben:

* **22 Tarock** (ungarisch *tarokk*, nie *adu*): `Skíz`, `XXI` … `I`
* **20 Farbkarten**: je 5 in ♥, ♦, ♣, ♠

```
Deck = 22 Tarock + 4 Farben × 5 Karten = 42 Karten
```

### 1.2 Tarockordnung (Trumpffolge)

Absteigend:

```
Skíz > XXI > XX > XIX > XVIII > XVII > XVI > XV > XIV > XIII > XII > XI > X > IX > VIII > VII > VI > V > IIII > III > II > I
```

* Der **Skíz** trägt keine Zahl (Narrenbild) und ist der **höchste Tarock** – er ist **kein** „Excuse"/„Narr"
  wie im französischen Tarot: er sticht normal, kann von keiner Karte
  gestochen werden, unterliegt dem Tarockzwang und hat keinerlei Sonderrechte [MKVM], [HUWIKI §Változatok].
* `I` heißt **pagát**, `II` heißt **sas** („Adler", nur in Figuren relevant), `XXI` hat keinen eigenen
  Namen (österreichisch „Mond"), `XX` heißt *húszas* und ist die normale Rufkarte.
* **honőrök** (Honneurs) = `I` (pagát), `XXI`, `Skíz`.
  **nagyhonőrök** (große Honneurs) = `XXI` und `Skíz` [PAG-I §Players and equipment], [MKVM].

### 1.3 Farbreihenfolgen – rot und schwarz sind unterschiedlich!

| Farbe | Rangfolge hoch → niedrig |
|---|---|
| **Rot** (♥ kőr, ♦ káró) | király `K` > dáma `D` > lovas `L` > bubi/botos `B` > **ász `A`** |
| **Schwarz** (♣ treff, ♠ pikk) | király `K` > dáma `D` > lovas `L` > bubi/botos `B` > **tízes `10`** |

Grund: Im vollen 54‑Blatt laufen die Zahlkarten in Rot *absteigend* (A, 2, 3, 4 – das Ass ist die **höchste**
Zahlkarte) und in Schwarz *aufsteigend* (10, 9, 8, 7 – die Zehn ist die **höchste** Zahlkarte). Nach dem
Entfernen der drei jeweils niedrigsten Zahlkarten bleibt rot das Ass, schwarz die Zehn übrig – beide sind in
ihrer Farbe die **niedrigste** Karte [PAG-P §Cards], [HUWIKI §Változatok].

**⚙ Flag `red_low_card = ace | four`:** Manche Kreise benutzen statt der roten Asse die roten Vierer (rein
kosmetisch, da beide die niedrigste rote Karte sind) [PAG-P §Variations]. Default: Ass.

### 1.4 Kartenwerte

| Karte | Wert | Anzahl | Summe |
|---|---|---|---|
| honőr: `Skíz`, `XXI`, `I` | **5** | 3 | 15 |
| Tarock `II`–`XX` | **1** | 19 | 19 |
| király (K) | **5** | 4 | 20 |
| dáma (D) | **4** | 4 | 16 |
| lovas (L) | **3** | 4 | 12 |
| bubi (B) | **2** | 4 | 8 |
| ász / tízes | **1** | 4 | 4 |
| | | **42** | **94** |

Tarock zusammen = 34, Farbkarten zusammen = 60 (15 je Farbe) [MKVM], [PAG-P].

### 1.5 Zählmethode (exakt)

Ungarisches Tarokk zählt **durch einfache Addition** – es gibt **keine** österreichische „Dreierzählung"
(Wert − ⅔ pro Stich), **keine** Halbierung und **keine** Abzüge pro Stich.

Abrechnungsverfahren am Ende der 9 Stiche:

1. Die Spielmacher‑Partei (felvevő + partner) legt ihre gewonnenen Stiche zusammen **und addiert den eigenen
   Skart des Spielmachers** (die Karten, die der Spielmacher gedrückt hat).
2. Die Gegenpartei legt ihre gewonnenen Stiche zusammen **und addiert den Skart der drei Nicht‑Spielmacher**
   (also auch den des Spielmacher‑Partners! [PAG-P §Discarding], [MKVM]).
3. Die Kartenwerte werden aufaddiert; die beiden Summen ergeben stets **94**.
4. **Ausnahme:** Wurde die Rufkarte gedrückt (§4.5), bekommt der allein spielende Spielmacher **alle sechs
   Skartkarten** [ITVB 5.8], [MKVM], [KEGL §Compulsory announcements] – siehe §9.3.

Implementierung: `int pointsOf(Team t)` summiert `cardValue` über `tricksWon(t) ∪ skart(t)`. Als Invariante
gilt `pointsOf(D) + pointsOf(O) == 94` (Assertion!).

### 1.6 Durchgerechnetes Zählbeispiel

(Es ist das Übungspartie 1 aus §11.8.1; die Stiche stehen dort vollständig.)

Stiche der Spielmacher‑Partei (S + N):

| Stich | Karten | Werte | Summe |
|---|---|---|---|
| 3 | ♠D, ♠B, V, XIV | 4 + 2 + 1 + 1 | 8 |
| 4 | XVI, II, XI, VIII | 1 + 1 + 1 + 1 | 4 |
| 5 | XIX, XVIII, XVII, III | 1 + 1 + 1 + 1 | 4 |
| 6 | ♥L, ♥D, XX, ♥B | 3 + 4 + 1 + 2 | 10 |
| 7 | ♠L, XV, XXI, ♠K | 3 + 1 + 5 + 5 | 14 |
| 9 | IX, ♦K, ♣K, Skíz | 1 + 5 + 5 + 5 | 16 |
| | **Stiche gesamt** | | **56** |
| Skart des Spielmachers | ♣L, ♦A, ♠10 | 3 + 1 + 1 | 5 |
| | **Spielmacher‑Partei** | | **61** |

Gegenpartei: Stich 1 (VI, IIII, VII, XII = 4) + Stich 2 (♣D, X, XIII, ♣B = 8) + Stich 8 (♥K, I, ♦D, ♥A = 15)
= 27, plus Skart der drei anderen (♦B 2 + ♣10 1 + ♦L 3 = 6) = **33**.

Probe: 61 + 33 = 94 ✔

### 1.7 Gewinnschwellen

| Ereignis | Bedingung | Quelle |
|---|---|---|
| **parti** (Grundspiel) gewonnen von der Spielmacher‑Partei | ≥ **48** Kartenpunkte | [ITVB 7.11], [MKVM], [PAG-I] |
| parti gewonnen von der Gegenpartei | ≥ **47** Punkte (bei 47:47 gewinnt **die Gegenpartei**) | [ITVB 7.11] – „unabhängig davon, ob sie kontriert hat" |
| **duplajáték** (Doppelspiel) | ≥ **71** Punkte (Gegner ≤ 23) | [ITVB 6], [PAG-I] |
| **volát** | **alle 9 Stiche** | [ITVB 6], [MKVM] |

Merksatz aus Ungarn: „Nem engedünk a 48‑ból!" („Von den 48 lassen wir nicht ab!") – Anspielung auf 1848
[HUWIKI §Története]. (Der Satz steht **nicht** bei [PAG-P]; die dortige Einleitung nennt nur die
Paskievics‑Legende.)

**⚙ Flag `tie_rule = defenders_win | last_kontra_loses`** – siehe §9.10.

---

## 2. Sitzordnung, Geben, Talon; 4 gegen 5 Spieler

### 2.1 Spielrichtung

**Alles läuft gegen den Uhrzeigersinn** („minden mozzanata jobbra tartással történik" [MKVM]): Der *nächste*
Spieler ist stets der **rechte Nachbar**. Das gilt für Austeilen, Lizit, Ansagerunde, Kartenspiel und für das
Weiterwandern des Gebens [PAG-P §Outline].

Interne Sitzindizes 0..3 (bzw. 0..4); `next(seat) = (seat + 1) mod n` entspricht „rechter Nachbar".

### 2.2 Platzwahl (Ceremonie, optional in der App)

Traditionell: Man legt an die Tischseiten je eine Karte (ein Tarock + je eine Farbkarte verschiedener Farben,
bei 5 Spielern 4 Farbkarten), mischt einen gleichartigen zweiten Satz und lässt jeden ziehen; wer den Tarock
zieht, setzt sich auf den Tarock‑Platz, **der Spieler links vom Tarock‑Zieher gibt zuerst** [PAG-I §Choice of
seats], [MKVM §A játék kezdete], [KEGL]. In der App: Zufallspermutation der Sitze + Startgeber; als
Wiederholungsrecht kann [KEGL]s Regel angeboten werden („wer zweimal einen XXI‑Fang erlitten hat, darf neu
auslosen").

### 2.3 Mischen, Abheben, Geben

1. Der Geber (*osztó*) mischt gründlich.
2. **Der Spieler links vom Geber hebt ab** (Pflicht) [MKVM], [PAG-P §Choice of seats].
3. Der Geber legt **zuerst 6 Karten verdeckt in die Mitte = talon**.
4. Danach, beginnend beim Spieler **rechts vom Geber** und gegen den Uhrzeigersinn: **je 5 Karten**, dann
   **je 4 Karten**. Jeder aktive Spieler hat 9 Karten [MKVM], [PAG-I §Choice of seats].
5. Der Talon behält seine Reihenfolge; die oberste Karte bleibt oben ([ITVB 3.2]: „a megkevert kártyák első 6
   lapja a talon, azonos sorrendben kerül kiosztásra").

Reihenfolge der Talonverteilung siehe §5.2.

**⚙ Flag `cut_variants`** (nur Turnierdetail, für die App irrelevant, aber dokumentiert): [ITVB 3.2] kennt
drei Abhebe‑Varianten (mehrere Karten abheben → Normalgabe; auf das Paket klopfen → Karten werden zu je 9
ausgelegt und in umgekehrter Spielreihenfolge gewählt; eine Karte abheben → einzeln geben). Die App gibt
immer normal.

### 2.4 Vier Spieler

Alle vier spielen jede Runde. Das Geben wandert nach jeder Partie **nach rechts** [PAG-P].

Rollen pro Partie:
* **osztó** (Geber),
* **induló** (Vorhand) = Spieler **rechts vom Geber**: spricht als **Erster im Lizit** und **spielt zum ersten
  Stich aus – unabhängig davon, wer Spielmacher ist** [MKVM], [PAG-I §The play].

### 2.5 Fünf Spieler („Fünfertisch")

* **Der jeweilige Geber setzt aus** und ist reiner Zuschauer; gespielt wird immer zu viert
  [PAG-I §Players and equipment], [MKVM]: „a mindenkori osztó kimarad a játékból".
* Der Geber **gewinnt und verliert nichts**; sämtliche Zahlungen laufen nur zwischen den vier aktiven
  Spielern [PAG-I §The scoring].
* Das Geben wandert nach rechts → jeder setzt jede 5. Partie aus.
* **Der aussetzende Geber darf keinerlei verdeckte Information sehen**: [ITVB 5.1] „Ötös asztal esetén az
  osztó a talont középre teszi, és ezután **nem nézheti meg**! Minden játékos maga veszi fel a saját
  talonját." Ebenso [KEGL]: der Geber bekommt keine Sonderaufgaben; gedrückte Tarocke meldet der Drückende
  selbst.
  Wortgleich [PVSZ18 6.6]: „Az osztó (akár négyes akár ötös asztal esetén) a talon tartalmáért **nem felel**.
  Ötös asztal esetén az osztó a talont középre teszi, **nem nézheti meg, nem nyúlhat hozzá!**"
  * **Abweichung [PAG-I], [PAG-P]:** Dort verteilt der Geber bei fünf Spielern den Talon, *prüft* die Skarts
    der drei Nicht‑Spielmacher und meldet „tiszta" bzw. die Zahl der gedrückten Tarocke (so auch
    [PAG-EX] Nr. 12 im Kommentar). **Für unsere App verboten**
    (Nutzervorgabe „Aussetzer darf keine verdeckten Karten sehen") – wir folgen [ITVB]/[PVSZ18]/[KEGL].
    **⚙ Flag `dealer_checks_skart = false`.**
* Zuschauer‑UI: Der Aussetzer sieht Tisch, Lizit, Ansagen, gespielte Karten, offen gezeigte Tarocke des
  Spielmachers und die Abrechnung – **nicht** Handkarten, Talon, Skarts, nicht die gerufene Karte, solange
  sie geheim ist.
* **Nicht implementieren** (historische Kuriosa, [PAG-P §Variations] nennt sie selbst „rather pointless"):
  Zahlung an den Geber für Honneurs aus dem Talon (3/2/1) sowie „sír‑nevet" (Geber gewinnt/verliert mit den
  Gegenspielern). **⚙ Flags `dealer_honour_fee = off`, `dealer_weeps = off`.**

### 2.6 Netzwerk‑/Sichtbarkeitsregeln (LAN, Server ist autoritativ)

Geheime Informationen, die der Server nie an unberechtigte Clients sendet: Handkarten, Talonverteilung,
Skarts (bis zur Abrechnung), Identität des Partners (bis sie sich aus Lizit, Ansagen oder Spielverlauf
ergibt), sowie – bei fünf Spielern – alles Verdeckte gegenüber dem Geber.

Öffentlich sind: alle Gebote, alle Ansagen/Kontras, die **Anzahl** der von jedem Spieler gedrückten Tarocke,
die **konkreten** vom Spielmacher gedrückten Tarocke (offen bis zum ersten Ausspiel, §5.6), alle gespielten
Karten, Stichgewinner, Abrechnung.

---

## 3. Das Lizit (licitálás) – Verträge, Rechte, Ablauf

### 3.1 Die vier Verträge

Das Gebot bestimmt, **wie viele Talonkarten der Spielmacher bekommt** – je weniger, desto höher der Wert:

| Gebot (ungarisch) | deutsch | Talonkarten für den Spielmacher | **Grundwert `g`** |
|---|---|---|---|
| **három** | „Drei" | 3 | **1** |
| **kettő** | „Zwei" | 2 | **2** |
| **egy** | „Eins" | 1 | **3** |
| **szóló** | „Solo" | 0 | **4** |

Quellen: [ITVB 6] (Tabelle „3‑s parti 1 / 2‑s parti 2 / 1‑s parti 3 / Szóló parti 4"), [MKVM], [PAG-I
§The scoring]; ebenso [BTE 5] und [PVSZ18 4.5/4.6] (dort als Multiplikatoren), sowie – unbestätigt –
[TA48 S. 61].

Es gibt **keine** weiteren Spielarten (kein Farbsolo, kein Negativspiel, kein „Klop"/„Trischaken"). Höhere
Ziele (≥ 71 Punkte, alle Stiche) werden **nicht** im Lizit, sondern als **Ansagen** (duplajáték, volát)
in der Ansagerunde übernommen (§5).

### 3.2 Wer darf lizitieren

* **Nur wer mindestens einen honőr (Skíz, XXI oder pagát) auf der Hand hat**, darf ein Gebot abgeben; wer
  keinen hat, **muss passen** [MKVM „Ebben részt vehet az a játékos, akinek legalább egy honőr … jutott"],
  [PAG-P §The bidding], [PAG-I §The bidding].
  *Hinweis:* [ITVB] **formuliert diese Grundregel nicht** – sie gehört zu den Regeln, die [ITVB 1.3]
  ausdrücklich der Hintergrundnorm [TA48] überlässt. [ITVB 4.1] („A honőrök egyenrangúak" = die honőrök
  sind gleichrangig) sagt nur, dass **jeder** honőr zum Bieten genügt und es unter ihnen keine Rangfolge
  gibt.
* **Einzige Ausnahme – `próbahárom` („Probe‑Drei"):** Haben die ersten drei Spieler gepasst, darf der
  **vierte** Spieler auch ohne honőr bieten und auf einen honőr aus dem Talon spekulieren
  [PAG-I], [MKVM], [PAG-P §The bidding]. Erhält er keinen, wird die Partie annulliert; ob er dafür zahlt,
  ist quellenabhängig (§4.6, §9.15): [PAG-P]/[PAG-I] Spielwert an jeden, [PVSZ18 8.2] 3 Punkte an jeden,
  [MKVM] **straffrei** („Amennyiben nem kapott honőrt a talonból, új osztás következik").
* Niemand ist verpflichtet zu bieten (außer in den Halte‑Situationen §3.4).

### 3.3 Gebotsmechanik

* Reihenfolge: **induló** (rechts vom Geber) beginnt, dann gegen den Uhrzeigersinn, so viele Runden wie nötig.
* Jedes Gebot muss **höher** sein als das laufende (három → kettő → egy → szóló), **außer** beim Halten
  (§3.4).
* **passz** scheidet endgültig aus dem Lizit aus [PAG-P §The bidding].
* Das Lizit endet, wenn (a) alle außer einem gepasst haben, oder (b) nach einem gehaltenen `szóló`
  kein höheres Gebot mehr möglich ist. Letzter Bieter = **felvevő** (Spielmacher).
* Passen alle vier: §4.6.

### 3.4 Halten („tartom") – die sitzabhängigen Rechte

**Default‑Regel („neue Regel", heute Mehrheit, [SURVEY] 9 : 2, [ITVB]-konform, [PAG-P §The bidding]):**

> „Tartom" darf sagen, wer **in diesem Lizit schon geboten hat**, wessen Gebot **überboten** wurde und dessen
> Gebot **noch nicht von jemand anderem gehalten** wurde. Ein Gebot kann nur **einmal** gehalten werden: Nach
> einem „tartom" muss der nächste Sprecher höher bieten oder passen.

**⚙ Flag `hold_rule = new | old`.** Alte Regel ([PAG-P §Variations], Bücher, Minderheit): Halten darf nur,
wer **früher an der Reihe war** als der Spieler, dessen Gebot er hält (A hält alle, D nie).

Beispiel (A = induló, Spielfolge A, B, C, D):

```
A: három   B: kettő   C: passz   D: egy
A: tartom  B: passz              D: szóló
A: tartom  →  Ende. A ist felvevő mit szóló (0 Talonkarten).
```
[PAG-P §The bidding, Beispiel 2] – dort mit dem Zusatz: Hätte **B** in der zweiten Runde noch bieten wollen,
hätte er **szóló** sagen müssen, denn D's `egy` war bereits gehalten.

### 3.5 Konventionelle Gebote: invit (Sprung) und engedés (Zulassen)

Diese „Konventionen" sind in Ungarn **verbindliche Regeln**, keine bloßen Absprachen: Wer sie verletzt,
begeht einen renonsz [PAG-P §Conventions], [ITVB 9.1], [PVSZ18 5.2.7].

Zweck: Zwei starke Blätter sollen zueinander finden, auch wenn keines den XX hat.

| Aktion | Bedeutung (Versprechen) | Quelle |
|---|---|---|
| **Ein Sprung** über das minimal mögliche Gebot (Halten zählt als „minimal möglich") | Der Bieter hat den **XIX** | [ITVB 4.10], [MKVM], [PAG-I §The bidding] |
| **Zwei Sprünge** | Der Bieter hat den **XVIII** | [ITVB 4.10] |
| **engedés** („Zulassen"): A sagt `három`, B sagt `kettő`, die anderen passen, **A passt** | A hat den **XX** | [ITVB 4.10], [PAG-I], [MKVM] |

**Zusätzliche Bedingungen im Default‑Profil [ITVB 4.3, 4.7]:**
* **Invit/engedés nur mit großem honőr** (Skíz oder XXI) **und mindestens fünf Tarock**.
* **[ITVB 4.6] Ein Invit kann nicht zurückgenommen werden: Der Invitgeber darf danach nicht mehr bieten**
  (nur noch passen).
* **[ITVB 4.9] „Viszontinvit nincs"**: Das *Annehmen* eines Invits durch einen Sprung ist **kein** neuer
  Invit. [PAG-I] formuliert dasselbe als Erlaubnis: „When there has already been a cue bid during the
  auction, a **second jump has no special meaning and can be made with any hand containing an honour**."
  Ein zweiter Sprung ist also erlaubt, aber bedeutungslos.
* **[ITVB 4.8]** Man darf das Lizit mit einem Invit eröffnen. Ein `szóló` als allererstes Gebot ist **kein**
  Invit (niemand könnte es annehmen). Ebenso [BTE 3.7], [PVSZ18 5.2.3].
* Nach drei Pässen ist ein Sprung des vierten Spielers **kein** Invit [PAG-I §The bidding] – weder der
  einfache (`kettő`) noch der doppelte (`egy`).
* **Der pagát genügt nicht:** [PAG-I] „in Illustrated Tarokk you **cannot cue bid when your only honour is
  the pagát**"; ebenso [ITVB 4.3]/[BTE 3.3] („csak nagy honőrrel"), [MKVM] („Pagáttal invitet adni nem
  lehet, **fogadni viszont igen**" – annehmen darf man den Invit mit dem pagát sehr wohl).
  Gegenposition nur im Paskievics‑Profil: [PVSZ18 5.2.1] erlaubt den Invit mit jedem honőr, [KEGL] hält das
  Verbot für „a bad habit" (§12).
* **Den Invit anzunehmen ist nicht Pflicht** [MKVM „Az invitet elfogadni nem kötelező"].

**Annahme:** „Angenommen" ist ein Invit, wenn nach ihm noch jemand bietet oder hält. Dann **muss der Gewinner
des Lizits die Invitkarte rufen** [ITVB 4.4], auch wenn er selbst gar nicht der Annehmende war
[PAG-P §Cue bids]. Bietet nach dem Invit niemand mehr, **gelten für den Invitgeber die normalen Regeln**
([ITVB 4.4]) – er ist Spielmacher und ruft normal den XX (§4.5).

**Halte‑Pflicht der Vorhand‑Situation:** Wer `három` gesagt hat und nur mit `kettő` überboten wurde (die
beiden anderen haben gepasst), **darf nicht einfach passen** – Passen wäre engedés und verspricht XX + großen
honőr + 5 Tarock. Ohne diese Karten **muss** er `tartom` sagen oder ein (gedecktes) Sprunggebot machen
[PAG-I §The bidding], [MKVM] („A licitben elsőként megszólalónak kötelező megtartani a kettes játékot, az
egyest és szólót azonban nem").

**⚙ Flag `invit_requirements = itvb | pagat`:** [PAG-I] verlangt XIX/XVIII **plus großen honőr** (kein
Mindest‑Tarockzähler) und erlaubt dem Invitgeber, weiterzubieten. [PVSZ18 5.2.1/5.2.2] erlaubt im
Paskievics‑Profil sogar den **Invit mit dem pagát**, dann ist `pagát ultimó` Pflichtansage (§12).

### 3.6 Beispiele für Lizitfolgen (alle aus den Quellen verifiziert)

```
(1) A: három   B: kettő   C: passz   D: passz
    A: passz                           → engedés: A zeigt XX (+ nagyhonőr, ≥5 Tarock).
      B ist felvevő mit kettő und MUSS den XX rufen.               [PAG-I, ITVB 4.10]

(2) A: három   B: passz   C: passz   D: egy      (Sprung: D zeigt XIX)
    A: szóló   (kein Invit – es gab schon einen)  D: passz
      A ist felvevő mit szóló und MUSS D's XIX rufen.               [PAG-P §Examples]

(3) A: passz   B: passz   C: három   D: kettő   (A und B sind draußen)
    C muss jetzt halten, springen oder engedés geben (Passen = XX-Versprechen!).
    C: tartom   D: passz  → C ist felvevő mit KETTŐ (2 Talonkarten) – „tartom" gleicht
                            das laufende Gebot an, es bleibt nicht `három`.

(4) A: passz   B: három   C: passz   D: szóló   (zwei Sprünge: D zeigt XVIII)
    B: tartom  → B ist felvevő mit szóló und MUSS D's XVIII rufen.  [MKVM Beispieltabelle]

(5) A: kettő (Sprung ⇒ XIX)  B: egy  C: passz  D: passz  A: passz
      B ist felvevő mit egy und MUSS A's XIX rufen.                 [PAG-P]
```

### 3.7 Sonderrecht des einzigen Bieters („Erhöhen")

**[ITVB 4.2]:** Hat **nur ein Spieler** geboten, darf er vor der Talonaufnahme frei wählen, **wie viele
Karten er nimmt (3, 2, 1 oder 0)**, und dadurch den Spielwert erhöhen. **Ausnahme:** Er hat als Eröffner
einen Invit gegeben, der nicht angenommen wurde – dann bleibt sein Gebot stehen.

**⚙ Flag `sole_bidder_may_raise = true`** (Default nach [ITVB]/[BTE]). [KEGL] und [PAG-P §Variations]
(Minderheit) verbieten dies ausdrücklich. Für `próbahárom` (Gebot ohne honőr): **kein** Erhöhungsrecht.

### 3.8 Lizit als Zustandsautomat (Implementierung)

```cpp
struct BidState {
  Seat  current;                 // wer spricht
  int   highest;                 // 3,2,1,0 als "Talonkarten" (kleiner = höher)
  Seat  highestBidder;
  bool  highestHeld;             // wurde das Höchstgebot schon gehalten?
  bool  hasBid[4];               // hat schon geboten
  bool  out[4];                  // hat gepasst
  Invit invit;                   // NONE | XIX | XVIII | XX(engedés), + Seat inviter
  bool  inviterLocked;           // ITVB 4.6: Invitgeber darf nicht mehr bieten
};
```
`legalBids(seat)` liefert: `PASS` (außer in der Halte‑Pflicht §3.5, wenn der Spieler nicht engedés‑berechtigt
ist), das Minimalgebot (bzw. `HOLD`, falls zulässig), und Sprunggebote nur, wenn der Spieler die
Invit‑Voraussetzungen erfüllt (bzw. der Sprung nach drei Pässen bedeutungsfrei ist). Der Server prüft diese
Bedingungen serverseitig (Cheat‑Schutz), die UI blendet illegale Gebote aus (Meldungen §11.6).

---

## 4. Talon, Skart (Drücken), Rufkarte, Einwurf

### 4.1 Verteilung des Talons

Der Spielmacher bekommt seine Karten **vom oberen Ende des Talons**, anschließend die übrigen Spieler
gegen den Uhrzeigersinn (beginnend rechts vom Spielmacher) [PAG-P §Distributing the talon]:

| Gebot | felvevő | 2. Spieler | 3. Spieler | 4. Spieler |
|---|---|---|---|---|
| **három** | 3 | 1 | 1 | 1 |
| **kettő** | 2 | 2 | 1 | 1 |
| **egy** | 1 | 2 | 2 | 1 |
| **szóló** | 0 | 2 | 2 | 2 |

(identisch in [MKVM], [PAG-I], [PAG-P])

„2. Spieler" = der rechte Nachbar des Spielmachers usw. Niemand zeigt seine Talonkarten. Bei vier Spielern
nimmt sie jeder selbst; bei fünf Spielern ebenfalls ([ITVB 5.1], [PVSZ18 6.6]: „Minden játékos maga veszi
fel a saját talonját"; der Geber fasst den Talon nicht an, §2.5 – ⚠ bei [PAG-I]/[PAG-P] verteilt sie der
Geber).

Die Tabelle ist in [MKVM §Lapcsere], [PAG-I §Drawing from the talon] und [PAG-P] identisch; die Restkarten
werden „so gleichmäßig wie möglich" verteilt, wobei die dem Spielmacher **rechts** Nächstsitzenden die
Zusatzkarte bekommen [PAG-P].

### 4.2 Einwurf / Annullieren der Partie („bedobás")

**Zeitfenster [ITVB 8.2], [BTE 7.1]: nach der Talonaufnahme, vor dem Drücken.** (Vier Könige dürfen auch
schon vor der Talonaufnahme eingeworfen werden.)

Einwurfberechtigte Blätter:

1. alle **vier Könige** auf der Hand,
2. **kein einziger Tarock**,
3. **nur der XXI** (kein weiterer Tarock),
4. **nur der pagát**,
5. **nur XXI und pagát** (kein weiterer Tarock).

Folge: Keine Wertung, dieselbe Person gibt neu (§4.6).

Dieselben fünf Fälle bei [BTE 7.1], [MKVM §Lapcsere], [PAG-I], [PAG-P]; [PVSZ18 6.2] nennt sie ohne den
Zusatz „nach der Talonaufnahme" (dort nur „csak fektetés előtt").

**⚙ Flag `annul_holdings`** (Mehrheitsbild [SURVEY IV.1]: einzelner XXI 11/11, vier Könige 10/11, kein
Tarock 10/11, einzelner pagát 9/11, XXI+pagát 6/11). [KEGL] empfiehlt nur die Fälle 1–3 („no trumps,
singleton XXI, four kings") und hält den Einwurf mit blankem pagát bzw. XXI+pagát für unbegründet.
[PAG-I] erlaubt den Einwurf **unmittelbar nach dem Talontausch** („once the round of announcements is
underway it is too late") und verbietet ihn für die Fälle 2–5, wenn der Spieler einen Tarock gedrückt hat;
vier Könige bleiben auch dann einwurfberechtigt.
**⚙ Flag `annul_timing = before_discard | after_discard`** (Default: `before_discard`, [ITVB]/[BTE]).

**⚙ Flag `annul_after_tarokk_discard = false`** (Default). Quellenlage: [SURVEY IV.2] 9 : 2 **dagegen**,
dass man mit blankem XXI einwerfen darf, wenn man Tarock gedrückt hat; ebenso [PAG-I], [KEGL] („the game
cannot be annulled"). ⚠ **[MKVM] empfiehlt ausdrücklich das Gegenteil**: „Előzetes megállapodás kérdése,
hogy ez az állapot tarokkfektetéssel is elérhető legyen. **Javasoljuk e gyakorlat bevezetését**" – also
Vorabvereinbarung, mit Empfehlung, es zu erlauben. Da [ITVB] dazu schweigt, bleibt der Default beim
Mehrheitsbild.

### 4.3 Drücken (skartolás / fektetés)

Jeder Spieler legt **genau so viele Karten verdeckt ab, wie er aus dem Talon bekommen hat**, und hat danach
wieder 9 Karten.

**Verboten zu drücken** [ITVB 5.3], [BTE 4.2], [PVSZ18 6.1], [MKVM]:
* **honőr** (Skíz, XXI, pagát),
* **jeder König**,
* **der XX** („a skartba nem lehet honőrt, királyt és a **XX tarokkot** letenni" [MKVM]),
* **die Invitkarte** (XIX bzw. XVIII) in einem Invitspiel – sie muss gerufen werden können.

Alles andere, **auch Tarock**, darf gedrückt werden.

**⚙ Flag `xx_discardable = false`** (Default nach [ITVB]/[BTE]/[PVSZ18]/[MKVM]/[KEGL]). [PAG-I]/[PAG-P]
erlauben das Drücken des XX (**außer** im engedés‑Spiel, dort ist er wie die Invitkarte gesperrt);
[SURVEY III] 6 : 5 dagegen. Siehe §9.1.

**Korrekturfenster (Implementierungshinweis):** [PVSZ18 6.7] „A fektetett lap az első bemondás előtt
**módosítható**, utána viszont **meg sem nézhető**" – der Skart darf bis zur ersten Ansage noch geändert
werden, danach nicht einmal mehr angesehen. Die App bildet das als Undo‑Fenster bis zum Beginn der
Ansagerunde ab (§5.1) und sperrt danach jede Skart‑Ansicht (außer der offen liegenden Tarocke des
Spielmachers, §4.4).

### 4.4 Ablage der Skarts, Meldepflichten

* **Der Skart des Spielmachers** bleibt verdeckt vor ihm liegen und zählt am Ende **zu seinen Stichen**.
* Die Skarts der drei anderen kommen auf **einen** Stapel und zählen **für die Gegenpartei** – auch der
  Skart des Spielmacher‑Partners [PAG-P §Discarding], [PAG-I], [MKVM], [HUWIKI].
  *Ablageort:* [MKVM]/[HUWIKI] „az osztó **jobb kezéhez**" (an die rechte Hand des Gebers; [HUWIKI] nennt
  den Stapel scherzhaft *szegények lapja*, „Karten der Armen"); [ITVB 5.2] „A skart helye **az osztó és az
  induló között** van" und schreibt zusätzlich die Ablegereihenfolge vor (erst der rechte Nachbar des
  Spielmachers, dann der ihm Gegenübersitzende quer darüber, dann der linke Nachbar quer darüber – damit
  ein Falschdrückender nachträglich identifizierbar ist); [PAG-I] legt ihn bei fünf Spielern vor den Geber.
  **Für die App ist das reine Darstellung** – regelrelevant ist nur die Zuordnung zur Gegenpartei.
* **Tarockmeldung:** Wer Tarock gedrückt hat, **muss** vor Beginn der Ansagen die **Anzahl** melden
  („egy tarokk fekszik", „két tarokk fekszik") [ITVB 5.4], [MKVM], [KEGL]. Wer nichts sagt, hat keine
  Tarocke gedrückt („tiszta").
* **Der Spielmacher muss seine gedrückten Tarocke offen zeigen** [ITVB 5.5], [MKVM]; sie bleiben bis zum
  ersten Ausspiel offen liegen [PAG-P §Discarding]. Alle wissen also *welche* Tarocke er gedrückt hat.

In der App: Nach dem Drücken zeigt der Server allen die Meldungen `tarocksDiscarded[seat] : int` und die
konkreten Tarockkarten des Spielmachers.

### 4.5 Partnerruf (Rufkarte)

Der Spielmacher eröffnet die Ansagerunde und **muss einen Tarock rufen**; dessen Besitzer ist sein Partner
(*segítő*), darf sich aber **nicht zu erkennen geben** [PAG-I §Calling a partner], [MKVM].

Regelkaskade (Default‑Profil):

1. **Normalfall: Der Spielmacher ruft den `XX`** [MKVM], [ITVB].
2. **Hat er den XX selbst**, ruft er den **höchsten Tarock, den er nicht hat** (XIX, sonst XVIII, …)
   [MKVM] – oder er ruft **den XX selbst** und spielt damit bewusst **allein gegen drei**
   [ITVB 5.6: „A felvevő önmagát minden esetben csak XX‑assal hívhatja meg"], [PAG-I].
   *Eine Karte aus dem eigenen Skart darf nicht gerufen werden* [KEGL §Some important rules].
3. **Invitspiel:** Er **muss** die Invitkarte (XIX/XVIII) rufen; im engedés‑Spiel den XX [ITVB 4.4],
   [PAG-I]. Das gilt auch dann, wenn Tarocke gedrückt wurden.
4. **Hat ein *anderer* Spieler Tarock gedrückt und hält der Spielmacher den XX**, darf er **jeden beliebigen
   Tarock** rufen [ITVB 5.7], [BTE 4.6], [PVSZ18 6.5] („Tarokk fektetése esetén, ha a felvevőnél van a
   XX‑as tarokk, **bármilyen tarokkot hívhat**, kivéve, ha csak a felvevő fektetett tarokkot").
   ⚠ **Präzisierung:** Die Einschränkung „**außer einem honőr**" steht **nicht** im ungarischen Text von
   [ITVB]/[BTE]/[PVSZ18], sondern nur bei [PAG-I §Calling a partner] („free to call any tarokk other than
   an honour") und [PAG-EX] Nr. 12. Sie ist gleichwohl sinnvoll (ein honőr darf ohnehin nicht gedrückt
   werden, ein honőr‑Ruf wäre also nie ein „Ruf ins Leere") und bleibt **App‑Default**.
   **⚙ Flag `free_call_excludes_honours = true`.**
   *Abweichung:* [PAG-I] gibt dieses Recht auch ohne XX‑Besitz (dort ist der XX drückbar) – siehe §9.2.
   [PAG-P §Variations] merkt an, dass manche Kreise, die den XX nicht drücken lassen, das freie Rufrecht
   trotzdem beibehalten.

**Ruf im Skart („hivatalból kontra"):** Wurde die gerufene Karte gedrückt, **muss der Drückende sofort
– „a felvevő licitjének passzal való lezárása után **sorrendtől függetlenül azonnal**", also unabhängig von
der Zugreihenfolge – „Hivatalból kontra" ansagen** (Kontra von Amts wegen auf das Grundspiel)
[ITVB 5.8], [BTE 4.7], [PVSZ18 6.4], [MKVM], [PAG-I], [KEGL]. Folgen:
* Der Spielmacher spielt **allein gegen drei**,
* **alle sechs Skartkarten zählen für ihn** [ITVB 5.8], [MKVM],
* das Grundspiel ist kontriert (×2).

### 4.6 Neugabe‑Fälle

| Fall | Folge |
|---|---|
| Alle vier passen | Karten zurück, **derselbe Geber gibt neu** [PAG-P], [MKVM] |
| Einwurf nach §4.2 | dito |
| `próbahárom` ohne honőr aus dem Talon | Partie annulliert; **der Bieter zahlt jedem der drei anderen den Wert des Grundspiels** (bei `három` also 1) [PAG-I], [PAG-P §The bidding] — ⚠ [PVSZ18 8.2]: Wert eines `egy`‑Spiels = **3 Punkte** an jeden; ⚠ [KEGL]: 2 Einheiten je Spieler (bzw. in die ultimó‑Kasse); ⚠ [MKVM]: **straffrei**, nur Neugabe |
| Renonsz (in der App unmöglich – die Engine lässt nur legale Aktionen zu) | §6.7 |

**⚙ Flag `double_round_after_annul = none | stacking | multiplying | ultimo_pot`** – Details §9.13/§9.14.
Default `none` (keine Verdopplungsrunden), weil [ITVB], [BTE] und [PVSZ18] sie nicht kennen (im
Turnierbetrieb mit fester Partienzahl wären sie auch systemfremd) und [KEGL] sie ablehnt („not worthy of
serious consideration").

⚠ **Korrektur der Quellenlage:** Bei [PAG-I] und [PAG-P] ist die Verdopplungsrunde die **Hauptregel**, nicht
eine Variante: „If all four players pass the cards are thrown in, the same player deals again, and **for the
next round (four deals if there are four players; five deals if there are five players) all the scores are
doubled**. If during the doubled round another hand is passed out, another doubled round is started, leading
to some deals in which the scores are **quadrupled** while the two rounds overlap." Dasselbe gilt nach einem
Einwurf. Erst unter §Variations heißt es „Some people do **not** play with the doubling of scores after a
hand is passed out or annulled" (sowie zwei weitere Spielarten: Zusatzrunde erst *nach* der laufenden
Verdopplungsrunde ohne Vervierfachung; bzw. eine volle Vervierfachungsrunde ohne weitere Steigerung).
Die App bildet alle vier Spielarten über das Flag ab, bleibt aber bei `none` als Default.
[KEGL] ersetzt die Verdopplung durch die **ultimó‑Kasse**: Bei Annullierung zahlt **jeder Spieler 2
Einheiten** ein (bei misslungenem `próbahárom` der Bieter allein den ganzen Betrag), die Kasse geht an ein
erfülltes angesagtes pagát ultimó und wird bei einem verfehlten verdoppelt; Deckel 24 Einheiten je Spieler
(25 zu fünft).

---

## 5. Ansagen, Prämien (figurák) und Kontra

### 5.1 Ablauf der Ansagerunde

Nach dem Drücken folgt die Ansagerunde (auch „zweites Lizit"), **beginnend beim Spielmacher**, dann gegen den
Uhrzeigersinn, so viele Umläufe wie nötig [PAG-I §Procedure], [MKVM].

An seinem Zug darf ein Spieler beliebig viele der folgenden Dinge sagen und beendet seinen Zug **immer mit
„passz"** (bzw. „mehet"):

1. **tarokkszám** – Ansage von 8 oder 9 Tarock (§5.11),
2. **Partnerruf** (nur Spielmacher, nur im ersten Zug, §4.5),
3. **Figuren** (Prämien) ansagen (§5.3),
4. **kontra / rekontra / szubkontra / hirskontra** auf Spiel oder Figuren der Gegenpartei (§5.12).

Die Ansagerunde endet, wenn **drei Spieler nacheinander nichts außer „passz" sagen** [PAG-I §Procedure],
[MKVM]. Eine tarokkszám‑Ansage **hält die Runde am Leben** („licitfenntartó hatályú", [ITVB 7.1], [KEGL]).

**[ITVB 7.4]:** „Amíg a licitáló játékos PASSZ‑t nem mond, a figurákat és bemondásokat bármilyen sorrendben
teheti, **akár meg is ismételheti**, de a már kimondott figura **vissza nem szívható!**" – Solange ein
Spieler an seinem Zug noch nicht „passz" gesagt hat, darf er Figuren in beliebiger Reihenfolge ansagen;
**eine ausgesprochene Figur kann nicht zurückgenommen werden.** (Zum scheinbaren Widerspruch zwischen dem
„akár meg is ismételheti" und dem Wiederholungsverbot in [ITVB 7.14] siehe §9.29.)
Reihenfolgeempfehlung des klassischen Textes [TA48 S. 30, unbestätigt]: tarokkszám → Rufkarte → Figuren –
deckungsgleich mit [PAG-I §Procedure]: „The declarer speaks first and **may begin by declaring tarokks**.
The declarer **must call a partner**, may then go on to **announce bonuses**, and must end by saying pass."

**Etikette (nicht regelrelevant, aber für die KI‑Sprechblasen):** Ansagen „in einem Atemzug" ohne Pausen
[KEGL §Proper behaviour].

### 5.2 Wem „gehört" eine Ansage? (Parteizuordnung)

Da Partner anfangs unbekannt sind, braucht jede Ansage eine **öffentlich bestimmbare Partei**, sonst könnte
man sie nicht kontrieren.

**Präzisierung des Begriffs „zuletzt Sprechender":** Gemeint ist **nicht** der zuletzt am Zug gewesene
Spieler, sondern der **zuletzt inhaltlich Sprechende** – also derjenige, der zuletzt eine Figur angesagt
oder ein kontra/rekontra/… gesagt hat (im Default‑Profil zusätzlich: eine tarokkszám angesagt hat). Ein
bloßes „passz" zählt **nicht**. Belege: [PAG-I §Procedure] „assumed to be the partner of the player who
**most recently announced a bonus or said kontra** (or rekontra, etc.)"; [MKVM] „az alapjáték vagy bármely
bemondás **kontrázása után** következő minden bemondás a kontrázó játékos partnerségét jelzi"; [PAG-P
§Variations] formuliert es für die tarokkszám als „the most recent speaker in the round of announcements
**who did not just say pass**".

**Default (ITVB):**
* **[ITVB 7.2]** Vor dem eindeutigen Bekanntwerden der Partner **gilt jede Figurenansage als Ansage der
  Partei des zuletzt Sprechenden** („minden figura bemondása az utolsó megszólalónak szól") –
  **Ausnahmen:** `centrum` sowie `kismadár`/`nagymadár`, wenn **der Spielmacher** trull angesagt hat.
  ⚠ [BTE 6.2] kennt **nur die centrum‑Ausnahme**, nicht die madár‑Ausnahme; [PAG-I] kennt gar keine
  Ausnahme. **⚙ Flag `attribution_exceptions = itvb | bte | none`** (Default `itvb`).
  Wem die ausgenommenen Ansagen dann gehören, sagt [ITVB] nicht – App‑Entscheidung siehe §9.27.
* **[ITVB 7.1]**, **[BTE 6.1]**, **[PVSZ18 4.1.2]** Dasselbe gilt für die **tarokkszám**‑Ansage
  ([PVSZ18 4.1.2] stellt den Verstoß ausdrücklich als RENONSZ unter Strafe).
* Wer also etwas ansagen will, aber **gegen** den zuletzt Sprechenden spielt, **muss sich zuerst durch ein
  kontra/rekontra zu erkennen geben** [PAG-I §Procedure], [MKVM].
* Ist die Parteizugehörigkeit bereits öffentlich bekannt (Invit/engedés, gespielte Rufkarte, frühere
  Kontras), entfällt die Pflicht.

Hat noch niemand etwas gesagt, gilt ein Ansager als **Partner des Spielmachers** [PAG-I].

*Alternative Systematik [KEGL]* (nicht implementiert, aber gleichwertig und für den Lernmodus lehrreich):
statt der „letzter Sprecher"-Regel drei Erlaubnisregeln – (1) der Spielmacher darf immer ansagen, (2) ein
Gegner darf eine neue Figur erst ansagen, **nachdem** er das Spiel kontriert (bzw. das rekontrierte Spiel
szubkontriert) hat, (3) der Partner des Spielmachers darf nur ansagen, solange das Spiel nicht kontriert
ist bzw. nachdem es rekontriert wurde.

**⚙ Flag `tarokkszam_identifies = true`** (Default ITVB/MKVM). [PAG-I]/[KEGL] behandeln die tarokkszám als
neutral (sie identifiziert **nicht**). Siehe §9.5.

**Implementierung:** Die Engine kennt die wahren Parteien. Sie berechnet zu jedem Zeitpunkt
`presumedTeam(speaker)` nach obiger Regel und lässt eine Ansage nur zu, wenn
`presumedTeam == realTeam(speaker)` **oder** der Spieler im selben Zug **vorher** ein kontra/rekontra
gesagt hat, das seine Partei offenlegt. Die UI bietet in diesem Fall den kombinierten Zug an
(„Kontra Spiel + Ansage") und erklärt ihn (§11.6, Meldung A7).

---

### 5.3 Wertetabelle (Default‑Profil `HU-ILLU-ITVB-2019`)

`g` = Grundwert des Vertrags (§3.1: három 1, kettő 2, egy 3, szóló 4).

| # | Figur | Bedingung | angesagt | still (*csendben*) | Quelle |
|---|---|---|---|---|---|
| 1 | **parti** (Grundspiel) | ≥ 48 Kartenpunkte | `g` | – | [ITVB 6] |
| 2 | **tulétroá** / trull / tuli | alle **drei honőr** (Skíz, XXI, I) in den eigenen Stichen | **2** | **1** | [ITVB 6], [MKVM], [PAG-I] |
| 3 | **négykirály** | alle **vier Könige** in den eigenen Stichen | **2** | **1** | dito |
| 4 | **duplajáték** („dupla") | ≥ **71** Kartenpunkte | **4 × g** | **2 × g** | dito |
| 5 | **volát** | alle **9 Stiche** | **6 × g** | **3 × g** | dito |
| 6 | **pagát ultimó** | letzter (9.) Stich wird **mit dem pagát gewonnen** | **10** | **5** | dito |
| 7 | **sas ultimó** | 9. Stich mit dem **II** gewonnen | **10** | **5** | [ITVB 6], [MKVM] |
| 8 | **XXI‑fogás** („huszonegyfogás") | der **Skíz sticht den XXI eines Gegners** | **42** | **21** | [ITVB 6], [MKVM], [PAG-I] |
| 9 | **centrum** | die **ersten 5 Stiche**, der 5. **mit dem XX** gewonnen | **10** | – | [ITVB 6], [MKVM], [PAG-I] |
| 10 | **kismadár** („kleiner Vogel") | die **ersten 6 Stiche**, der 6. **mit dem XXI** | **10** | – | dito |
| 11 | **nagymadár** („großer Vogel") | die **ersten 7 Stiche**, der 7. **mit dem Skíz** | **10** | – | dito |
| 12 | **pagát uhu** („Uhu") | der **8. Stich** wird **mit dem pagát** gewonnen | **15** | – | [ITVB 6], [BTE 5], [MKVM], [HUWIKI] (⚠ [PAG-I]: **20**, dort mit Hinweis „many players prefer to score only 15") |
| 13 | **sas uhu** | 8. Stich mit dem **II** | **15** | – | [ITVB 6], [MKVM] |
| 14 | **király ultimó** | 9. Stich mit einem **benannten König** (Farbe muss mit angesagt werden) | **15** | – | [ITVB 6], [BTE 5], [MKVM], [PAG-I], [HUWIKI] |
| 15 | **király uhu** | 8. Stich mit einem **benannten König** | **20** | – | [ITVB 6], [BTE 5], [MKVM], [PAG-I] (⚠ [HUWIKI]: 15) |
| 16 | **8 tarokk** (tarokkszám) | genau 8 Tarock nach dem Drücken | **1** | (1, nur vom Partner) | §5.11 |
| 17 | **9 tarokk** | genau 9 Tarock | **2** | (2, nur vom Partner) | §5.11 |
| — | **pagátfogás** (optional) | der pagát der **Gegenpartei** landet in einem Stich der eigenen Partei | 8 | 4 | [MKVM] „néhány társaságban", [PAG-I §Variations] – **⚙ Flag `pagatfogas = off`** |

**Still erfüllbar** (ohne Ansage, halber Wert) sind ausschließlich: **tulétroá, négykirály, duplajáték,
volát, pagát ultimó, sas ultimó, XXI‑fogás** [ITVB 6 Spalte „Csendben teljesíthető"], [BTE 5], [MKVM
§Csendes teljesítések]. Alle Illusztrált‑Figuren (centrum, madarak, uhuk, király ultimó) zählen
**nur angesagt** [ITVB 6], [BTE 5], [MKVM], [PAG-I], [HUWIKI].
Normbasis für den halben Wert: **[ITVB 7.12]** / **[BTE 6.11]** „Csendes figurát, mind a felvevők, mind az
ellenfelek teljesíthetnek. Csendes figurák teljesítése esetén **a pontok 50 %-a** jár." – stille Figuren
können also von **beiden** Parteien erfüllt werden.

**⚙ Flag `uhu_value = 15 | 20`** (betrifft **pagát uhu und sas uhu gemeinsam**; Default 15 nach [ITVB 6],
[BTE 5], [MKVM], [HUWIKI]). [PAG-I] nennt für **beide** 20 und merkt ausdrücklich an: „Many players prefer
to score only **15** points rather than 20 for announced **pagátuhu and sasuhu**, since like the
corresponding ultimó bonuses they are easier to make with a trump than with a king." (Der bisherige
Flagname `pagat_uhu_value` erfasste den sas uhu nicht – korrigiert.)
Der **király uhu** bleibt in allen ungarischen Verbandsquellen bei **20** ([ITVB 6], [BTE 5], [MKVM],
[PAG-I]); nur [HUWIKI] nennt 15 (§9.22).
**⚙ Flag `sas_figures = on|off`** (Default on: [ITVB 6], [BTE 5] und [MKVM] führen sas ultimó/sas uhu
regulär; [PAG-I] kennt sie nur als Variante „borrowed from High Tarokk", [PVSZ18] gar nicht).

### 5.4 Prinzipien der Figurenwertung

* Figuren gehören **immer der Partei**, nicht der Person – Partner gewinnen/verlieren gemeinsam (nur den Hut
  trägt der XXI‑Verlierer persönlich, §5.10) [PAG-P §Bonuses].
* **Angesagt und verfehlt = Zahlung in gleicher Höhe an die Gegenpartei** (Vorzeichenumkehr).
* **Still erfüllt = halber Wert**; still *verfehlt* gibt es nur beim (silent) pagát/sas ultimó (§5.8).
* Figuren sind voneinander unabhängig – **Ausnahme:** parti / duplajáték / volát greifen ineinander (§7.2).
* Dieselbe Figur kann **beiden** Parteien Punkte bringen: Sagt Partei A „négykirály" an und Partei B holt
  alle vier Könige, bekommt B 2 (verfehlte Ansage) + 1 (eigene stille Figur) = 3 [PAG-P §The scoring].
* **Stille tulétroá und stille négykirály werden nicht gezahlt, wenn dieselbe Partei volát macht**
  [PAG-I §The scoring: „A team which wins all the tricks scores nothing for silent trull or silent four
  kings. **Announced** trull or four kings are scored as usual."], [SURVEY VI: 9 : 2].
  ⚠ **Abweichende Formulierungen:** [MKVM] und [HUWIKI] beziehen die Ausnahme ausdrücklich nur auf den
  **stillen** volát („Csendes volátnál a csendes tulétroá, négy király **és duplajáték** teljesítéséért nem
  jár pont") und nehmen die stille dupla mit hinein; [KEGL] ebenso („If volat is made, no score is awarded
  for silent trull, silent four kings or silent high game"). Praktisch deckungsgleich, denn die stille
  dupla wird durch den volát ohnehin verdrängt (§7.2). ✅ **App folgt [PAG-I]**: Die Ausnahme gilt bei
  **jedem** erzielten volát, angesagt wie still. **⚙ Flag `volat_kills_silent_bonuses = any | silent_only`.**

### 5.5 Wer darf was ansagen – Beschränkungen (Default‑Profil)

| Regel | verbindlich? | Quelle |
|---|---|---|
| **tulétroá nur mit großem honőr in der Hand** – Ausnahme: als Partner des Spielmachers, wenn dieser ohne Aufforderung centrum/kismadár/nagymadár angesagt hat | **Regel** (renonsz) | [ITVB 7.6], [BTE 6.5], [PVSZ18 4.2.3], [KEGL], [MKVM] |
| **Spielmacher, erste Runde:** tulétroá bedeutet **beide große honőr** – bei **drei Bietern** bedeutet es „Skíz" | **Regel** (renonsz) | [ITVB 7.7], [BTE 6.6], [PVSZ18 4.2.1], [MKVM] |
| **Invit-/engedés‑Spiel, erste Runde:** tulétroá des Spielmachers zeigt einen großen honőr. Sagt der Spielmacher (bei zwei Bietern) keinen tulétroá, darf der Invitgeber in der ersten Runde nur mit **zwei honőr oder Skíz** tulétroá sagen – oder wenn der Spielmacher centrum/kismadár/nagymadár angesagt hat | **Regel** (renonsz) | [ITVB 7.8], [PVSZ18 4.2.2]; [BTE 6.7] **ohne** den centrum/madár‑Zusatz |
| **tulétroá in der zweiten Runde** bedeutet mindestens **einen** großen honőr | **Regel** (Paskievics) | [PVSZ18 4.2.4]; [MKVM] sinngemäß |
| **Nach volát** dürfen tulétroá, négykirály und duplajáték **nicht mehr** angesagt werden | **Regel** | [ITVB 7.5], [BTE 6.4], [PVSZ18 7.1], [KEGL] — ⚠ [PAG-I] verbietet nur die **duplajáték** nach volát |
| **dupla und volát dürfen zusammen (auch im selben Zug) angesagt werden** | **Regel** | [ITVB 7.5], [BTE 6.4], [PVSZ18 7.2] — ⚠ **Mehrheit dagegen:** [PAG-I] „You cannot announce both double game and volát at the same turn to speak", [KEGL] „illegal … on the same turn", [SURVEY V] 7 (nicht im selben Zug) : 3 (im selben Zug) : 1 (gar nicht). ⚙ Flag `dupla_volat_same_turn` |
| **Ein angesagtes ultimó darf nicht zu einem uhu „hochgestuft" werden** | **Regel** | [ITVB 7.10], [BTE 6.9] — ⚠ [PAG-I] erlaubt dem *Partner* des Ansagers (nicht dem Ansager selbst) die Hochstufung; das uhu **ersetzt** dann das ultimó; ⚙ Flag `ulti_to_uhu` |
| **Umgekehrt: nach einem angesagten uhu darf dieselbe Partei für dieselbe Karte kein ultimó mehr ansagen** („Herabstufen" verboten) | **Regel** | [PAG-I §Procedure]; [ITVB] schweigt dazu – App übernimmt die pagat‑Regel, da sie sonst die uhu‑Bindung aushebeln würde. ⚙ Flag `uhu_to_ulti = forbidden` |
| **Ein Paar darf pro Stich nur eine Figur ansagen** (kein pagát ulti + sas ulti + király ulti auf den 9. Stich, keine zwei király ultimó) | **Regel** | [ITVB 7.13], [BTE 6.12] |
| **Wiederholung einer schon angesagten eigenen Figur = renonsz** | **Regel** | [ITVB 7.14] — ⚠ steht in Spannung zu [ITVB 7.4] („a figurákat … **akár meg is ismételheti**"); siehe §9.29 |
| **duplajáték bedeutet nicht den Besitz einer bestimmten Karte** (nur Blattstärke) | **Regel** | [ITVB 7.5], [BTE 6.4] |
| **centrum/kismadár/nagymadár** sind beliebig kombinierbar (verschiedene Stiche) und auch mit uhu/ultimó kombinierbar | **Regel** | [PAG-I §Procedure] |
| **Erste Runde, nach tulétroá: négykirály zeigt den höchsten noch unbekannten Tarock** (im Invitspiel den XX) | **NUR „erős ajánlás"** (starke Empfehlung) | [ITVB 7.9], [BTE 6.8]; [MKVM] ebenso als Konvention. **[ITVB 9.1] nimmt „erős ajánlás"-Regeln ausdrücklich vom renonsz aus** |
| **Ein Spielmacher mit ≥ 5 Tarock *soll* in der ersten Runde tulétroá sagen** | **NUR „erős ajánlás"** | Zusatzsätze in [ITVB 7.6/7.7/7.8], [BTE 6.5/6.6/6.7] |

⚠ **Korrektur gegenüber der Vorfassung:** [ITVB] unterscheidet zwischen echten Regeln und *erős ajánlás*
(„starke Empfehlung"). **[ITVB 9.1]:** „A tarokkozás szabályainak és jelen szabályzat utasításainak
megszegése renonsz, **kivéve az erős ajánlás minősítésűeket**." Die négykirály‑Bedeutung [ITVB 7.9] und die
„≥ 5 Tarock ⇒ tulétroá"-Sätze sind als *erős ajánlás* gekennzeichnet und dürfen deshalb **nicht** hart
erzwungen werden.

**⚙ Flag `enforce_trull_conventions = true`**: Erzwungen werden nur die als **Regel** markierten
Bedeutungsregeln ([ITVB 7.6 Satz 1], 7.7, 7.8). **[ITVB 7.9] wird nie erzwungen**, sondern nur als
KI‑Konvention und Lernhinweis verwendet (Meldung `A_FOURKINGS_HINT`, kein Verbot). Im Profil
`HU-ILLU-PAGAT` sind auch 7.6–7.8 nur Empfehlungen (die KI hält sich trotzdem daran).

### 5.6 centrum, kismadár, nagymadár (nur angesagt)

* **centrum**: Die ansagende Partei gewinnt die Stiche 1–5, **und der 5. Stich wird vom `XX` gestochen**
  (der XX selbst muss der höchste Tarock im Stich sein!) [ITVB 6], [BTE 5], [MKVM], [PAG-I].
* **kismadár**: Stiche 1–6, der 6. mit dem `XXI`.
* **nagymadár**: Stiche 1–7, der 7. mit dem `Skíz`.
* Verlorener Stich vorher ⇒ Figur verfehlt.
* **Achtung [ITVB 8.5] (Beispiel im Regeltext), [BTE 7.4]:** Fällt centrum nur deshalb, weil der Ansager
  den XX **zwangsweise** vor dem 5. Stich abspielen musste (Reihenfolgeregel §6.4.3), **können kismadár und
  nagymadár trotzdem noch gelingen** – Originalbeispiel: „6 tarokkos XX‑as, XXI‑es, Skíz, centrum, kismadár,
  nagymadár bemondásnál az első 3 ütésben nem tud hívni, és a színét sem hívják ki, ezért a 4. ütésben
  köteles a XX‑ast rakni. **Utána hívhat színt és teljesülhet a kismadár, nagymadár mondás.**"
  Fällt centrum dagegen, weil ein Stich **verloren** ging, sind kismadár/nagymadár automatisch mit verloren
  (sie verlangen ja ebenfalls alle vorangehenden Stiche) [PAG-I §Rájátszás].

### 5.7 uhu und ultimó (gebundene Karten)

* **ultimó** = **9. Stich** mit einer bestimmten Karte gewinnen; **uhu** = **8. Stich**.
* Karten: `pagát` (I), `sas` (II), oder ein **benannter König** (Farbe muss angesagt werden, z. B.
  „treffkirály‑ultimó").
* Die benannte Karte muss den Stich **selbst gewinnen**; gewinnt ihn der Partner, ist die Figur verfehlt
  („irrespective of whether the trick is won by an opponent or the partner of the player with the pagát"
  [PAG-I §Bonuses]), ebenso [MKVM] („Ha bármelyik játékosnak tarokkja marad a kilencedik ütésre … a
  királyultimó elbukott").
* Eine angesagte Figur dieser Art ist verfehlt, wenn die Karte **vorher** (erzwungen) abgegeben werden muss
  oder wenn die ansagende Partei die Karte gar nicht besitzt [PAG-I].
* **Der `király ultimó`/`király uhu` gelingt praktisch nur, wenn der Königsbesitzer selbst zum Zielstich
  ausspielt** (ein König kann nur den Stich seiner eigenen Farbe gewinnen, und nur, wenn niemand
  tarockieren kann) – daher der taktische Hinweis in §10.5 und §11.3.
* Verpflichtungen im Spiel (**rájátszás / lekötött lap**) siehe §6.4 – das ist die wichtigste
  Spielbeschränkung im Illusztrált.

### 5.8 pagát ultimó / sas ultimó – auch „still" möglich

* **Still erfüllt** (pagát gewinnt den 9. Stich ohne Ansage): **5 Punkte**.
* **Still verfehlt:** Wird der pagát zum 9. Stich gespielt und **gestochen**, zahlt die Partei des pagát
  **5 Punkte** an die andere – auch dann, wenn der Stich vom *eigenen Partner* gewonnen wurde
  [PAG-I §Bonuses], [MKVM]. (Dasselbe für den sas, falls `sas_figures = on` [PAG-I §Variations].)
* **[ITVB 8.6]/[BTE 7.5]:** „Ha uhu mondás esetén a bemondó a lekötött lappal ultit játszik, csendes ultinak
  számít, ha elfogják, akkor azt bukott csendes ultiként kell elszámolni." – Hat eine Partei **uhu**
  angesagt und spielt die gebundene Karte dennoch erst im 9. Stich, gilt das als **stilles ultimó** (bzw.
  als **verfehltes** stilles ultimó, wenn die Karte gestochen wird). Das angesagte uhu ist dabei natürlich
  verloren.
* **[ITVB 8.7]/[BTE 7.6]:** Erfüllt die ansagende Partei mit einer *anderen* Karte im selben Stich eine
  weitere Figur, zählt diese als **stille** Figur (und bei Misslingen als verfehlte stille Figur).

**Implementierungsregel:** Die stille (pagát-/sas‑)ultimó‑Wertung wird für die Partei ausgewertet, die die
Karte zum 9. Stich gespielt hat, **sofern diese Partei für diese Karte kein ultimó angesagt hat**.

### 5.9 tulétroá und négykirály

* **tulétroá** (fr. *tous les trois*): Alle drei honőr landen in den Stichen der Partei. Da honőr und Könige
  nicht gedrückt werden dürfen, sind alle sieben Karten immer im Spiel.
* **négykirály**: Alle vier Könige in den Stichen der Partei.
* Beide: angesagt 2, still 1; **mit volát derselben Partei entfallen die stillen Wertungen** (§5.4).

### 5.10 XXI‑fogás (der „Fang")

* Bedingung: In **einem** Stich liegen `Skíz` und `XXI`, **und sie gehören verschiedenen Parteien**; der Skíz
  gewinnt den Stich. Liegen beide in der Hand derselben Partei, ist es **kein** Fang und es gibt nichts
  [PAG-I §Bonuses], [MKVM].
* Werte: **21 still / 42 angesagt** (im Default‑Profil).
* **polgármester („Bürgermeister"):** Wer seinen XXI verliert, ist bis zum nächsten Fang „Bürgermeister" und
  trägt traditionell einen albernen Hut [PAG-P §The mayor's hat]. In der App: Spielerstatus‑Abzeichen 🎩,
  Statistik „Fänge/erlittene Fänge" (§8.3).
* Angesagt und misslungen (XXI entwischt oder lag gar nicht beim Gegner): **−42** für die Ansager.
* **⚙ Flag `xxi_catch_values = 21/42 | 30/60`** – einige neuere Bücher und [HUWIKI] nennen 30/60, in der
  Praxis dominieren 21/42 [PAG-P §Variations in the scoring].

### 5.11 tarokkszám (8 oder 9 Tarock)

* Jeder Spieler **darf** in der Ansagerunde ansagen, dass er **genau 8** (1 Punkt) bzw. **genau 9** (2 Punkte)
  Tarock hat. „8 Tarock" mit 9 Tarock auf der Hand anzusagen ist **illegal** [PAG-I §Declaring tarokks].
* **Pflicht** ist die Ansage für jeden, der **pagát ultimó, pagát uhu, sas ultimó oder sas uhu ansagt oder
  kontriert** und 8/9 Tarock hält [ITVB 7.3], [BTE 6.3], [MKVM]. **Ausnahme [ITVB 7.3]/[BTE 6.3]:** wenn
  nicht der Ansager, sondern sein **Partner** die betreffende Karte (pagát bzw. sas) besitzt.
  ([PVSZ18 4.1.3] kennt im Paskievics‑Profil nur die Pflicht beim **pagát ultimó** und dessen Kontra.)
  *Abweichung:* [PAG-I] dehnt die Pflicht auf **pagát *oder* király ultimó bzw. uhu** aus („except when
  announcing or saying kontra to pagát **or king** ultimo or pagát **or king** uhu"), kennt aber die
  Partner‑Ausnahme nicht. **⚙ Flag `tarokkszam_required_for_king_ulti = false`** (Default = ITVB/BTE).
* **Zahlung:** Der Ansager erhält **von jedem der drei anderen aktiven Spieler** den Betrag (also auch vom
  eigenen Partner): „Payment for declared tarokks is made by **all other active players, including the
  partner** of the player who holds the tarokks. If the tarokks are not declared, payment can be claimed at
  the end of the play from the partner of the holder **only**." [PAG-I §Declaring tarokks + Scoring‑Tabelle].
  Die Zahlung ist **unabhängig vom Ausgang der Partie** ([MKVM]: „A tarokkszám díját a játékcélok
  teljesítésétől függetlenül megkapja a játékos, ha ezt a licitálásnál bemondta").
  **⚙ Flag `tarokkszam_payment = all_three | team_item`** – [PVSZ18 4.1] wertet die tarokkszám als
  **Parteiprämie** („az azt bemondó játékosnak **és a partnerének** jár"), siehe §9.4.
* Die tarokkszám ist **nicht kontrierbar**. Das ist eine **Auslegung**, kein Zitat: [ITVB 6] listet die
  tarokkszám in derselben Tabelle wie die Figuren, die Kontra‑Zeile spricht aber ausdrücklich nur von der
  „**bemondott figura**"; [PAG-I] führt die Deklaration durchgängig getrennt von den „bonuses". Siehe §9.30.
* **[ITVB 7.1]/[BTE 6.1]/[PVSZ18 4.1.1]:** Die tarokkszám‑Ansage ist **licitfenntartó hatályú** – sie hält
  die Ansagerunde am Leben, alle drei anderen Spieler kommen noch einmal an die Reihe ([KEGL] explizit).

### 5.12 kontra, rekontra, szubkontra, hirskontra

* Jede **angesagte** Figur und das **Grundspiel** können von **jedem Mitglied der Gegenpartei einzeln**
  kontriert werden; die Ansagerpartei kann **rekontrieren**, dann wieder die Gegenpartei („szubkontra"),
  dann „hirskontra" [MKVM], [ITVB 6].
* Jede Stufe **verdoppelt**: kontra ×2, rekontra ×4, szubkontra ×8, hirskontra ×16
  [ITVB 6 „Bemondott figura rekontra, szubkontra, **hirskontráig** mehet"], [BTE 5], [PVSZ18 4.8].
  ⚠ **[MKVM] kennt nur drei Stufen** („A kontra kétszeres, a rekontra négyszeres, a **szubkontra
  nyolcszoros** díjazást jelent"), [PAG-I] nennt eine fünfte („**mordkontra**", ×32).
  **⚙ Flag `max_kontra_level = 3 (szubkontra) | 4 (hirskontra, Default) | 5 (mordkontra)`.**
* Kontras sind **einzeln** zu benennen („kontra játék", „kontra pagátulti") und wirken unabhängig voneinander
  [PAG-I §Kontra].
* **Stille Figuren können nicht kontriert werden** (sie sind nicht angesagt).
* Das **Grundspiel** gehört stets der Spielmacher‑Partei; nur die Gegenpartei kann „kontra játék" sagen.
* **Hivatalból kontra** (§4.5) ist ein echtes Kontra des Grundspiels.
* **[ITVB 7.11]/[BTE 6.10]/[PVSZ18 9]/[KEGL]:** Ein Kontra ändert **nichts** an der 48/47‑Schwelle
  („A játékot az ellenpár **függetlenül attól, hogy kontrázott-e vagy sem**, akkor nyeri, ha az ütéseinek
  pontértéke legalább 47").
* ⚠ **Sonderfall 47:47 mit Kontra:** [KEGL] fordert zusätzlich, dass bei exakt 47:47 die Zahlung **nicht**
  verdoppelt wird („If the game ends with 47 points on both sides, payment is not doubled. Such a rule would
  be ludicrous."). [ITVB]/[BTE] sagen dazu nichts; der Wortlaut („akkor nyeri") spricht für eine normale,
  also kontrierte Wertung. ✅ **App‑Default: normal kontriert werten**
  (**⚙ Flag `tie_kontra_not_doubled = false`**).

---

## 6. Das Spiel (lejátszás)

### 6.1 Ausspiel und Stichfolge

* **Zum ersten Stich spielt der `induló` aus** – der Spieler **rechts vom Geber**, unabhängig davon, wer
  Spielmacher ist [MKVM], [PAG-I §The play]. Das ist eine der wichtigsten taktischen Eigenheiten des Spiels
  ([MKVM]: „**Függetlenül attól, hogy ki lett a felvevő**, az első ütéshez az osztótól jobbra ülő játékos
  hív ki lapot"; [PAG-I]: „The player to dealer's right leads to the first trick").
* Danach spielt der Gewinner des letzten Stichs aus. Neun Stiche, dann Abrechnung.

### 6.2 Bedienzwang

In dieser Reihenfolge:

1. **Farbzwang:** Es muss die ausgespielte Farbe bedient werden, wenn man sie hat – **auch wenn man mit einem
   Tarock stechen könnte** [MKVM], [HUWIKI]: „Pl.: kőr dámára kötelező rátennünk a kőr lovast, hiába tudnánk
   megütni tarokkal".
2. **Tarockzwang:** Wer die ausgespielte Farbe nicht hat, **muss Tarock spielen**, wenn er einen hat
   [MKVM], [PAG-I §The play].
3. Wurde ein **Tarock ausgespielt**, müssen alle Tarock zugeben, sofern sie einen haben.
4. Wer weder Farbe noch Tarock hat, spielt eine **beliebige** Karte.

### 6.3 Kein Überstichzwang

**Es gibt keinerlei Pflicht, zu übertrumpfen oder überhaupt zu stechen** („A tarokkban NINCS kötelező
felülütési szabály (über)" [HUWIKI], [MKVM]). Man darf beliebig tief untertarockieren – das ist ein
zentrales taktisches Mittel (z. B. der Skíz‑Besitzer legt hohe, aber nicht stechende Tarocke – siehe §10.7).

### 6.4 Gebundene Karten (lekötött lap) und Rájátszás – die Ansage‑Spielpflichten

Sobald eine Partei eine Figur angesagt hat, die eine **bestimmte Karte zu einem bestimmten Stich** verlangt
(centrum → `XX`/Stich 5, kismadár → `XXI`/6, nagymadár → `Skíz`/7, uhu → Karte/Stich 8, ultimó → Karte/Stich
9), gilt für die ansagende Partei [ITVB 8.5], [BTE 7.4], [PAG-I §Rájátszás]:

1. **Sperre:** Die gebundene Karte darf **vor ihrem Stich nicht gespielt werden**, solange eine andere Karte
   erlaubt ist (Bedien‑ und Tarockzwang gehen vor).
2. **Pflicht:** **Im gebundenen Stich muss sie gespielt werden**, wenn sie nach §6.2 erlaubt ist – auch wenn
   man den Stich damit sicher verliert.
   *Beispiel [PAG-I]:* pagát uhu angesagt, letzte zwei Karten pagát + höherer Tarock, ein Gegner spielt zum
   8. Stich den `II` aus → **man muss den pagát legen und darf nicht stechen.**
3. **Reihenfolge:** Hat eine Partei mehrere gebundene Karten und wird zum vorzeitigen Abspielen gezwungen,
   ist **die Karte mit dem früheren Zielstich zuerst** abzuspielen – auch wenn dadurch eine Figur fällt
   [ITVB 8.5], [PAG-I].
   *Beispiel [ITVB 8.5]:* 6 Tarock inkl. XX, XXI, Skíz mit centrum + kismadár + nagymadár; in den Stichen
   1–3 kommt der Spieler nicht ans Ausspiel und seine Farbe wird nicht gespielt, im 4. Stich muss er den
   **XX** legen (centrum fällt) – kismadár/nagymadár können weiterleben.
4. **Freigabe:** Ist die Figur bereits gefallen, ist die Karte **frei** – „A lekötött lap a figura bukása
   után szabadon kijátszható" [ITVB 8.5], [BTE 7.4]. [PAG-I] formuliert die Freigabe ausdrücklich nur für
   **centrum/kismadár/nagymadár** („If you lose a trick before the relevant trick, then **after** you have
   lost the trick you are released from your obligations"). Praktisch fallen beide Lesarten zusammen, denn
   ein angesagtes **uhu/ultimó** kann vor seinem Zielstich nur dadurch fallen, dass die gebundene Karte
   selbst erzwungen abgespielt wird – dann gibt es nichts mehr freizugeben. ✅ App folgt [ITVB]/[BTE].
   *Original‑Beispiel [PAG-I]:* Ansagen trull + négykirály + centrum + kismadár + nagymadár + dupla + volát;
   im 1. Stich trumpft ein Gegner mit dem XV – wegen des kismadár darf man **nicht** mit dem XXI überstechen,
   muss also centrum, beide madár, négykirály und volát verlieren; **danach** sind XX, XXI und Skíz frei.
5. Auch das **angesagte pagát ultimó** bindet den pagát bis zum 9. Stich: „Selbst wenn klar ist, dass der
   pagát nicht mehr gewinnen kann, darf man ihn nicht vorzeitig retten" [PAG-P §The play], [ITVB 8.5].
6. **[ITVB 8.4]/[BTE 7.3]:** Eine Figur gilt in dem Moment als gefallen, in dem der **vierte** Spieler seine
   Karte gelegt hat (maßgeblich für Nr. 4 und für die Sperre in Nr. 1).
7. **Die Bindung gilt für die *ganze Partei*, nicht nur für den Ansager** („A bemondó pár/személy(ek)" in
   allen ITVB‑Figurendefinitionen; [PAG-I]: „If **your team** has announced …"). Hält der Partner die
   gebundene Karte, ist **er** gebunden – auch wenn er sie erst nach der Ansage als solche erkennt.

**Referenzimplementierung `legalMoves`:**

```cpp
CardSet legalMoves(const Hand& h, const Trick& t, const Bindings& b, int trickNo, Team team) {
    CardSet base = followRule(h, t);                     // §6.2
    auto mine = b.activeFor(team, /*holder=*/h);         // nur Bindungen an Karten in dieser Hand
    if (mine.empty()) return base;
    // (a) Ist eine Karte für genau diesen Stich gebunden und erlaubt -> Pflicht
    for (auto& [card, target] : mine)
        if (target == trickNo && base.contains(card)) return CardSet{card};
    // (b) sonst: gebundene Karten herausnehmen
    CardSet free = base - keys(mine);
    if (!free.empty()) return free;
    // (c) nur gebundene Karten legal -> die mit dem kleinsten Zielstich
    return CardSet{ argmin(mine, by_target_trick) };
}
```

### 6.5 Stichgewinn

Es gewinnt **der höchste Tarock im Stich**, sonst die **höchste Karte der ausgespielten Farbe** [MKVM],
[PAG-I]. Es gibt **keine** Sonderstiche:

* **kein Kaiserstich / „Pagat‑Ultimo‑Stich"** wie im österreichischen Tarock (Skíz + XXI + pagát in einem
  Stich ⇒ der pagát gewinnt) – diese Regel existiert im ungarischen Tarokk **nicht**; keine der Quellen
  ([ITVB], [BTE], [PVSZ18], [MKVM], [PAG-I], [PAG-P], [KEGL], [HUWIKI]) kennt sie.
* Der Skíz ist **keine** Ausweichkarte („Excuse“/Narr) – er sticht und wird gestochen wie jeder andere Tarock.
* Keine „Vogel"-Pflichtspiele, keine Farbspiele, keine Trümpfe außer den Tarocken.

### 6.6 Stichhaltung und Aufdeckung

* Solange die Partnerschaft unbekannt ist, **hält jeder Spieler seine Stiche einzeln**, und der Skart der
  Gegenpartei bleibt separat [PAG-I §The play]. Erst wenn die Rufkarte fällt (oder die Partnerschaft
  anderweitig bewiesen ist), werden die Stapel zusammengelegt.
  In der App: Die Engine weiß es ohnehin; die UI zeigt Stichstapel erst dann zusammengefasst, wenn die
  Partnerschaft öffentlich ist (sonst verrät die UI Information!).
* **[ITVB 8.3]:** Der **letzte Stich** darf bis zum nächsten Ausspiel noch einmal angesehen werden. In der
  App: „letzten Stich zeigen"-Button bis zur nächsten Karte.

---

### 6.7 Renonsz (Regelverstöße) – und warum die App sie verhindert

Die ungarischen Regelwerke widmen dem *renonsz* (Regelverstoß) viel Raum, weil am realen Tisch niemand die
Legalität erzwingt: Lizit ohne honőr, unerlaubtes Drücken, falsche Invit‑Bedeutung, zu frühes Ausspielen einer
gebundenen Karte, Nichtbedienen usw. Strafen:
* **[ITVB 9.8/9.9]:** der Verursacher zahlt den drei anderen die bis dahin angesagten **und** erfüllten
  Figuren (samt Kontras), **mindestens 5, höchstens 20** Punkte; bei zwei Schuldigen ([ITVB 9.5]: auch wer
  einen erkannten renonsz nicht sofort anzeigt, begeht einen) zahlt jeder das Doppelte, min. 10 / max. 40.
* **[BTE 8.6/8.7]:** identisch, 5–20; zusätzlich [BTE 8.3]: die **Mehrheit der unbeteiligten Spieler**
  entscheidet, ob weitergespielt, straffrei neu gegeben oder der renonsz verrechnet wird.
* **[PVSZ18 10.7]** (Paskievics): min. **5**, max. **10** an jeden der drei; bei zwei Schuldigen 10–20.
* **[MKVM]:** Wert der Ansagen und der bereits still erzielten Leistungen, „de **legalább 10 egységet**"
  an jeden der drei Mitspieler; die Partie endet und gilt als gespielt.
* **[ITVB 9.2]/[BTE 8.2]/[PVSZ18]:** renonsz beim Mischen/Abheben/Geben ⇒ **keine Strafe, Neugabe**.
* **[KEGL §Reneging]:** Zahlung in die ultimó‑Kasse.

**Für harbour‑tarock gilt:** Die Engine lässt ausschließlich legale Aktionen zu (Server ist autoritativ),
Regelverstöße können also nicht entstehen; der Lernmodus erklärt stattdessen, *warum* eine Aktion gesperrt
ist (§11.6). Die renonsz‑Tarife werden nur dokumentiert (für einen späteren „Offline‑Schiedsrichter"‑Modus
und zum Verständnis der Turnierregeln).

---

## 7. Abrechnung (elszámolás)

### 7.1 Zahlungsmodell

Alle Wertungen sind **Nullsummen** pro Spieler. Grundmuster [PAG-I §The scoring]:

* **2 gegen 2:** Jeder Verlierer zahlt **einem** Gewinner den Nettobetrag ⇒ **jeder Gewinner +V, jeder
  Verlierer −V**.
* **1 gegen 3** (Spielmacher hat sich selbst gerufen oder die gerufene Karte lag im Skart): Der Alleinspieler
  zahlt/erhält **von jedem** der drei anderen ⇒ **Alleinspieler ±3V, jeder Gegner ∓V**.
* **5 Spieler:** Der Geber zahlt und erhält **nichts**.
* **tarokkszám** (Default): Der Ansager erhält **von jedem** der drei anderen den Betrag (§5.11).

```cpp
// V = Summe aller Partie-Items aus Sicht der Spielmacherpartei (positiv = D gewinnt)
for (Seat s : activeSeats) {
    if (isDeclarerTeam(s)) score[s] += (teamSize(D)==1 ? 3*V : V);
    else                   score[s] -= (teamSize(D)==1 ? V   : V);
}
// + tarokkszám-Transfers
```

### 7.2 parti / duplajáték / volát – das Zusammenspiel (exakter Algorithmus)

Die folgende Berechnung ist [PAG-I §The scoring] entnommen und reproduziert dessen Multiplikatortabelle
exakt (§7.3 – als Unit‑Test verwenden!).

```python
# Eingaben: g (Grundwert 1/2/3/4), pD (Kartenpunkte der Spielmacherpartei), tricksD (Stiche von D),
#           annD[T], annV[T]  = hat Partei T dupla/volát angesagt?
#           kG, kD[T], kV[T]  = Kontrastufen (0 = kein Kontra, 1 = kontra, 2 = rekontra, …)
other        = {'D':'O','O':'D'}
winnerGame   = 'D' if pD >= 48 else 'O'                       # 47:47 -> O  (§1.7)
duplaMadeBy  = 'D' if pD >= 71 else ('O' if 94-pD >= 71 else None)
volatMadeBy  = 'D' if tricksD == 9 else ('O' if tricksD == 0 else None)

# 1) angesagte dupla / volát  – immer gewertet, Vorzeichen je nach Erfüllung
for T in ('D','O'):
    if annD[T]: add(4*g * 2**kD[T], winner = T if duplaMadeBy == T else other[T])
    if annV[T]: add(6*g * 2**kV[T], winner = T if volatMadeBy == T else other[T])

# 2) stille dupla / volát  (volát verdrängt die stille dupla derselben Partei)
silent = None
if   volatMadeBy and not annV[volatMadeBy]:
    silent = (volatMadeBy, 3*g)                               # auch neben angesagter dupla zulässig
elif duplaMadeBy and not annD[duplaMadeBy] and not annV[duplaMadeBy]:
    silent = (duplaMadeBy, 2*g)

# 3) Grundspiel (parti)
if kG > 0:
    scoreGame = True                                          # kontriertes Spiel zählt IMMER
else:
    scoreGame = (not (silent and silent[0] == winnerGame)      # stille dupla/volát ersetzt die parti
                 and not (annD[winnerGame] or annV[winnerGame]))
if scoreGame: add(g * 2**kG, winner = winnerGame)
if silent:    add(silent[1], winner = silent[0])
```

*Dieser Algorithmus wurde gegen alle 48 Felder der Tabelle in §7.3 getestet (Referenzimplementierung + Unit
Test).*

Erläuterung der Sonderfälle in Worten [PAG-I]:
* Ohne Ansage und ohne Kontra wird **nur eine** der drei Wertungen parti / dupla / volát gezahlt (die
  jeweils erreichte, höchste).
* Wer **dupla ansagt**, kann das Grundspiel **nicht** schreiben, wohl aber zusätzlich eine **stille volát**.
  Verliert er dupla **und** Spiel, schreibt die Gegenpartei zusätzlich das Grundspiel (zusammen 5 × g).
* Wer **dupla und volát ansagt**, schreibt beide getrennt; das Grundspiel wird nur gewertet, wenn die
  Ansager das Spiel verlieren.
* Wer **nur volát ansagt**, kann weder parti noch dupla schreiben; die Gegenpartei aber schon.
* **Ein kontriertes Grundspiel wird immer gewertet**, zusätzlich zu einer stillen dupla **oder** volát (nicht
  beiden).

### 7.3 Multiplikatortabelle (Verifikationstabelle für Unit‑Tests)

Vielfaches des Grundwerts `g`, aus Sicht der **Spielmacherpartei** [PAG-I §The scoring]:

| Ansagen | kein Stich | ≤ 23 | 24–47 | 48–70 | ≥ 71 | alle Stiche |
|---|---|---|---|---|---|---|
| (keine) | −3 | −2 | −1 | **+1** | +2 | +3 |
| dupla (von D) | −7 | −6 | −5 | −4 | **+4** | +7 |
| volát (von D) | −9 | −8 | −7 | −6 | −6 | **+6** |
| dupla + volát (D) | −13 | −12 | −11 | −10 | −2 | **+10** |
| kontra auf das Spiel | −5 | −4 | −2 | **+2** | +4 | +5 |
| kontra Spiel; Gegner sagen dupla an | −9 | −6 | +2 | +6 | +8 | +9 |
| dupla (D); Gegner kontrieren die dupla | −11 | −10 | −9 | −8 | **+8** | +11 |
| dupla (D); Gegner kontrieren dupla **und** Spiel | −13 | −12 | −10 | −6 | **+10** | +13 |

### 7.4 Sonstige Figuren

Alle Figuren aus §5.3 Nr. 2, 3, 6–15 werden **zusätzlich** und **unabhängig vom Grundwert `g`** gezahlt
(also z. B. pagát ultimó = 10, egal ob három oder szóló) [PAG-I §The scoring]:

```
für jede angesagte Figur f einer Partei T:  ± wert(f) * 2^kontraStufe(f)
für jede still erfüllte Figur f (nur Nr. 2,3,6,7,8):  + wert_still(f)     (ohne Kontra-Faktor)
für jede still verfehlte Figur (nur pagát/sas ultimó): − wert_still(f)
```

### 7.5 Durchgerechnete Beispiele

**(a) Übungspartie 1 (§11.8.1):** Vertrag `három` (g = 1), keine Ansagen, D hat 61 Punkte.
→ Tabelle Zeile 1, Spalte 48–70: **+1 × g = +1**.
Ergebnis: S +1, N +1, E −1, W −1.

**(b) Übungspartie 2 (§11.8.2):** Vertrag `kettő` (g = 2), tulétroá angesagt (D), kontriert (O), rekontriert
(D) ⇒ Faktor 4; D hat 68 Punkte und alle drei honőr.
→ parti +1 × 2 = **+2**; tulétroá 2 × 4 = **+8**. Summe **+10** je Spieler der Spielmacherpartei.
Ergebnis: S +10, W +10, E −10, N −10.

**(c) Übungspartie 3 (§11.8.3):** Vertrag `kettő` (g = 2); S sagt „nyolc tarokk" + „pagát ultimó" an;
D erzielt 70 Punkte, der pagát gewinnt den 9. Stich, der Skíz fängt den XXI, alle drei honőr bei D.
→ parti +2, pagát ultimó +10, stiller XXI‑fogás +21, stille tulétroá +1 ⇒ **+34** je D‑Spieler.
Zusätzlich tarokkszám: S erhält 1 von jedem der drei anderen.
Ergebnis: **S +37, N +33, E −35, W −35** (Summe 0 ✔).

**(d) Beispiel aus [PAG-EX] Nr. 1 (Verifikation der Tabelle):** Vertrag `kettő` (engedés; B ist felvevő,
A sein Partner); die **Spielmacherpartei** sagt volát an, die Gegenpartei kontriert; sie holt 73 Punkte,
aber nicht alle Stiche.
→ Zeile „volát", Spalte ≥ 71: −6 × g = −12, mit Kontra ×2 = **−24**; dazu tulétroá angesagt +2 und pagát uhu
+20 ⇒ Endstand **−2** je D‑Spieler. (Exakt so bei [PAG-EX] Nr. 1: „Two with announced volát lost with
kontra: −24 / Trull: 2 / Pagátuhu: 20 / Total: −2".)
⚠ **Achtung:** Die +20 sind der **pagat‑Wert** des pagát uhu. Mit dem App‑Default (15, §5.3) lautet das
Ergebnis **−7**. Zusätzlich zu verbuchen wäre die dort angesagte „kilenc tarokk" (2 von jedem der drei
anderen an B) – [PAG-EX] führt die Deklarationen in der Ergebniszeile nicht mit.

**(e) Alleinspiel [PAG-EX] Nr. 12:** Vertrag `kettő`, der gerufene XX lag im Skart, C spielt allein;
rekontriertes Grundspiel +8, kontrierte dupla −16, tulétroá +2 ⇒ −6 **pro Gegner**, der Alleinspieler also
**−18**, jeder Gegner **+6**. (Ebenfalls angesagt, aber nicht in der Ergebniszeile: „nyolc tarokk".)
**Nachgerechnet:** [PAG-EX] gibt „Value of cards taken by declarer's team: **56** points" an. C's Stiche
(1, 3, 5, 8, 9) enthalten 8 + 10 + 9 + 10 + 15 = **52** Punkte, sein eigener Skart (♣10, ♥lovas) **4**.
56 = 52 + 4 ⇒ **[PAG-EX] rechnet die Skarts der drei Gegner weiterhin den Gegnern zu** und widerspricht
damit [ITVB 5.8]/[MKVM] (dort bekäme C alle sechs Skartkarten, also 52 + 14 = 66). Am Ergebnis dieser Partie
ändert das nichts (auch 66 < 71), aber die Regel ist real strittig – §9.3.

### 7.6 Reihenfolge der Verbuchung (UI)

Die Abrechnungsanzeige listet je Zeile: Figur · Partei · angesagt/still · Kontrastufe · Grundwert ·
Faktor · Betrag; darunter die Zeile „tarokkszám" und die Summe pro Spieler. Für die Lernhilfe zusätzlich
ein „Warum?"-Link je Zeile (§11).

---

## 8. Matchstruktur (Turnier‑ und App‑Modus)

### 8.1 Vorgaben aus dem ungarischen Turnierbetrieb

* **Vierertisch: 16 Partien pro Runde; Fünfertisch: 20 Partien pro Runde** [ITVB 2.7] – im Original
  „**legfeljebb** … lehet játszani", also eine **Obergrenze** innerhalb des Zeitlimits, keine Sollzahl;
  Zeitlimit 90 min (1. Runde) bzw. 80 min (Runden 2–4) [ITVB 2.6]; Turnier = **4 Runden** [ITVB 1.2].
  [PVSZ18 2.2/2.3/2.4]: 16 bzw. 20 Partien, 80 min in **allen** Runden.
* Tischplatzierungspunkte [ITVB 2.1/2.2]: Vierertisch 10‑8‑6‑4; Fünfertisch 10‑8‑6‑4‑**4** (1. Runde) bzw.
  10‑8‑6‑4‑**2** (Runden 2–4); Gleichstand ⇒ arithmetisches Mittel [ITVB 2.3].
  ⚠ Abweichend: [BTE 2.2] immer 10‑8‑6‑4‑2; [PVSZ18 3.2] immer 10‑8‑6‑4‑4.
* Zusätzliche **XXI‑Fang‑Wertung** [ITVB 2.5] – eine reine **Turnierstatistik neben** der Geldwertung:
  „Fogta" (gefangen) +10, „Bukta" (XXI verloren) −10, „Hajtotta" („getrieben" – nach üblicher Lesart der
  Partner des Fängers) +5, „Szenvedte" („erlitten" – der Partner des Opfers) −5; bei **angesagtem** Fang und
  bei „Fang von hinten" jeweils **×5**. Nach Ablauf der Spielzeit darf nicht mehr abgehoben und keine neue
  Partie gegeben werden [ITVB 2.6].

### 8.2 Empfehlung für die App

| Modus | Aufbau |
|---|---|
| **Schnellspiel** | 4 Partien (4 Spieler) bzw. 5 Partien (5 Spieler) = **eine volle Geberrunde**; danach Abschlusstabelle |
| **Standard‑Match (Default)** | **4 volle Geberrunden** = 16 Partien (4 Spieler) / 20 Partien (5 Spieler), exakt wie [ITVB 2.7] |
| **Turnier** | 4 Runden à 16/20 Partien, Tischpunkte 10‑8‑6‑4(‑2), optional Zeitlimit |
| **Traditionell** | „A skíz oszt, nem oszt": Ein Spieler kündigt das Ende an; wer in der nächsten Partie den Skíz bekommt, ist „der Skíz"; gespielt wird bis er das zweite Mal geben würde – 4–7 (4 Spieler) bzw. 5–9 (5 Spieler) weitere Partien [PAG-P §Ending the session] |

Match‑Invarianten: Jede Partie ist eine Nullsumme; das Match‑Ergebnis ist die Summe. Jeder Spieler gibt gleich
oft (also auch: setzt bei fünf Spielern gleich oft aus) – die App erzwingt deshalb **volle Geberrunden**.

### 8.3 Statistiken (Wiederspielwert, sehr „ungarisch")

* **polgármester**‑Abzeichen 🎩 für den letzten XXI‑Verlierer (bleibt bis zum nächsten Fang).
* XXI‑Fang‑Bilanz nach [ITVB 2.5].
* pagát‑ultimó‑Quote, durchschnittliche Kartenpunkte, Anteil gewonnener Spielmacher‑Partien.
* Optional **ultimó‑Kasse** (§9.14) als Anzeige.

---

## 9. Grenzfälle, regionale Abweichungen, Empfehlungen

Legende: ✅ = Empfehlung/Default der App, ⚠ = abweichende Quelle.

| # | Streitpunkt | Quellenlage | ✅ Empfehlung (Flag) |
|---|---|---|---|
| 1 | **Darf der XX gedrückt werden?** | ✅ nein: [ITVB 5.3], [BTE 4.2], [PVSZ18 6.1], [MKVM], [KEGL]; ⚠ ja: [PAG-I], [PAG-P] (nicht im engedés‑Spiel); [SURVEY III] 6 : 5 dagegen | **nein** (`xx_discardable=false`) |
| 2 | **Freier Ruf nach Tarockdrücken** | ✅ nur wenn der Spielmacher selbst den XX hat [ITVB 5.7], [BTE 4.6], [PVSZ18 6.5]; ⚠ generell [PAG-I]; [KEGL]: ganz abschaffen, da XX nicht drückbar; [PAG-P §Variations]: manche behalten es trotz XX‑Verbot bei. **Der Zusatz „außer honőr" steht nur bei [PAG-I], nicht im ungarischen Normtext** (§4.5) | **ITVB‑Fassung** (`free_call_requires_xx=true`, `free_call_excludes_honours=true`) |
| 3 | **Rufkarte lag im Skart: Wem gehört der Skart?** | ✅ alle 6 Karten dem Alleinspieler [ITVB 5.8], [BTE 4.7], [PVSZ18 6.4], [MKVM], [KEGL]; ⚠ [PAG-EX] Nr. 12 rechnet die Gegner‑Skarts weiterhin den Gegnern zu (nachgerechnet, §7.5e) | **alle 6 dem Spielmacher** (`skart_to_lone_declarer=true`), im Lernmodus erklärt |
| 4 | **tarokkszám: wer zahlt?** | ✅ alle drei anderen [PAG-I §Declaring tarokks]; ⚠ Parteiprämie [PVSZ18 4.1]; [ITVB]/[BTE]/[MKVM] schweigen zur Zahlungsrichtung | **alle drei** (`tarokkszam_payment=all_three`) |
| 5 | **Identifiziert die tarokkszám die Partei?** | ✅ ja, sie „gilt dem zuletzt Sprechenden" [ITVB 7.1], [MKVM]; ⚠ nein [PAG-I], [KEGL] | **ja** (`tarokkszam_identifies=true`) |
| 6 | **ultimó → uhu hochstufen** | ✅ verboten [ITVB 7.10], [BTE 6.9]; ⚠ erlaubt für den **Partner** des Ansagers [PAG-I] (das uhu ersetzt dann das ultimó) | **verboten** (`ulti_to_uhu=false`) |
| 6b | **uhu → ultimó „herabstufen"** | nur [PAG-I] regelt es: **verboten**; [ITVB]/[BTE] schweigen | **verboten** (`uhu_to_ulti=forbidden`) – sonst ließe sich die uhu‑Bindung umgehen |
| 7 | **pagát uhu / sas uhu Wert** | ✅ 15 [ITVB 6], [BTE 5], [MKVM], [HUWIKI]; ⚠ 20 [PAG-I] („many players prefer 15") | **15** (`uhu_value=15`, gilt für beide uhu mit Tarock) |
| 8 | **sas ultimó / sas uhu** | ✅ regulär [ITVB 6], [MKVM]; ⚠ Variante [PAG-I] | **an** (`sas_figures=on`) |
| 9 | **pagátfogás** (8/4) | „in einigen Gesellschaften" [MKVM], [PAG-I §Variations] | **aus** (`pagatfogas=off`) |
| 10 | **47:47 nach Kontra** | ✅ Gegenpartei gewinnt immer mit 47 [ITVB 7.11], [BTE 6.10], [PVSZ18 9], [KEGL], [HUWIKI]; ⚠ „Pontegyenlőségnél a kontra utolsó fokozatát bemondója elveszíti a partit" (bei Gleichstand verliert, wer die letzte Kontrastufe sagte) [MKVM] – dort allerdings unmittelbar gefolgt von „A tarokk klasszikus szabálya **minden esetben** a felvevő párt kötelezi 48 pont elérésére", was der eigenen Aussage widerspricht; ⚠ Kovács‑Bücher: nach Kontra brauchen die Gegner 48 [PAG-P §Variations] | **ITVB** (`tie_rule=defenders_win`) |
| 11 | **parti/dupla/volát‑Verrechnung** | ✅ [PAG-I]‑Algorithmus (§7.2, gegen alle 48 Tabellenfelder geprüft); ⚠ [PVSZ18 4.5/4.6]: bei angesagter dupla/volát zählt die parti **für die Ansager** „semmilyen körülmény esetén sem" – die Gegner schreiben sie aber sehr wohl, wenn sie sie machen; ⚠ Variante: Kontra multipliziert auch stille dupla/volát [PAG-P §Variations]; ⚠ Variante: Gegner können stille dupla/volát nur schreiben, wenn sie kontriert haben [PAG-P §Variations] | **[PAG-I]‑Algorithmus** (`game_dupla_volat=pagat`) |
| 12 | **stille négykirály neben stiller dupla** | ✅ zulässig [PAG-I]; ⚠ [KEGL]: entfällt („If four kings and high game are both made silently, no score is awarded for four kings"); ⚠ [PAG-P §Variations] nennt beides als verbreitet | **zulässig** (`silent_kings_with_silent_dupla=true`) |
| 13 | **Einwurf‑Blätter und ‑Zeitpunkt** | ✅ 5 Fälle, nach Talon vor Skart [ITVB 8.2], [BTE 7.1], [MKVM], [PVSZ18 6.2]; ⚠ [PAG-I]: unmittelbar nach dem Talontausch (= nach dem Skart), gesperrt für die Fälle 2–5 nach Tarockdrücken; ⚠ [KEGL]: nur 3 Fälle | **ITVB** (`annul_holdings=itvb`, `annul_timing=before_discard`) |
| 13b | **Einwurf, nachdem man Tarock gedrückt hat** | ✅ nein [SURVEY IV.2] 9 : 2, [PAG-I], [KEGL]; ⚠ **[MKVM] empfiehlt, es zuzulassen** (Vorabvereinbarung) | **nein** (`annul_after_tarokk_discard=false`) |
| 14 | **Folgen von Einwurf / vier Pässen** | ✅ nur Neugabe ([ITVB]/[BTE]/[PVSZ18] kennen nichts anderes); ⚠ **Verdopplungsrunde ist bei [PAG-I]/[PAG-P] die Hauptregel** (4 bzw. 5 Partien, überlappend bis ×4), Weglassen dort nur Variante; ⚠ ultimó‑Kasse [KEGL] | **nur Neugabe** (`double_round_after_annul=none`), Kasse/Verdopplung als Option |
| 15 | **Strafe für misslungene `próbahárom`** | 1 × Spielwert an jeden [PAG-I], [PAG-P]; ⚠ 3 Punkte (= `egy`‑Wert) [PVSZ18 8.2]; ⚠ 2 Einheiten bzw. in die Kasse [KEGL], BGA; ⚠ **straffrei** [MKVM] | **Spielwert (1 bei `három`)** (`proba_penalty=game_value`) |
| 16 | **Darf der einzige Bieter erhöhen?** | ✅ ja [ITVB 4.2], [BTE 3.2], [PVSZ18 5.1.2]; ⚠ nein [KEGL], [PAG-P §Variations] (dort umgekehrt als Variante beschrieben: „some groups allow the bidder to increase") | **ja** (`sole_bidder_may_raise=true`) |
| 17 | **Invit‑Voraussetzungen** | ✅ großer honőr + ≥ 5 Tarock, kein Weiterbieten [ITVB 4.3/4.6/4.7], [BTE 3.3/3.5/3.6]; ⚠ [PAG-I]: XIX/XVIII + großer honőr (kein Tarockminimum), pagát genügt **nicht**; ⚠ [PVSZ18 5.2]: Invit mit jedem honőr, mit pagát Pflicht‑Ultimó; [MKVM]: „Pagáttal invitet adni nem lehet, fogadni viszont igen" | **ITVB** (`invit_requirements=itvb`) |
| 18 | **Halteregel** | ✅ neue Regel [SURVEY I] 9 : 2, [PAG-P], [ITVB]‑kompatibel; ⚠ alte Regel (halten darf nur, wer früher an der Reihe war) [PAG-P §Variations], [KEGL], „most of the books" | **neu** (`hold_rule=new`) |
| 19 | **Bedeutung `A:3 – B:2 – C:szóló`** | ✅ XIX (ein Sprung über das Mindestgebot `egy`) [ITVB 4.10], [BTE 3.9], [PVSZ18 5.2.5], [MKVM]; ⚠ XVIII [KEGL] („we recommend the first approach"), [PAG-P §Variations] „the original rule … most of the books still give this rule"; [SURVEY II.b] 9 : 2 für XIX | **XIX** (`jump_meaning=new`) |
| 20 | **Trull‑Bedeutungsregeln erzwingen?** | [ITVB 7.6 S.1/7.7/7.8], [BTE 6.5–6.7], [PVSZ18 4.2] machen sie zu Regeln (renonsz); **[ITVB 7.9] und die „≥ 5 Tarock"-Sätze sind dagegen nur *erős ajánlás* und laut [ITVB 9.1] ausdrücklich kein renonsz**; [KEGL] lehnt jede Trull‑Pflicht ab | **nur die echten Regeln erzwingen** (`enforce_trull_conventions=true`), abschaltbar; 7.9 nie erzwingen |
| 21 | **„három + trull, sonst nichts" nicht ausspielen** | ⚠ [KEGL], [PAG-I §Variations]: Partie wird nicht gespielt, Gegner zahlen 3 an die Spielmacherpartei (die Gegner können das durch eine eigene Ansage – z. B. 8/9 tarokk – oder ein Kontra verhindern); ✅ [ITVB 8.1]: **jede Partie wird gespielt** | **ausspielen** (`skip_trull_three=false`) |
| 22 | **[HUWIKI]‑Werte** (XXI‑fogás **60**, király uhu **15**, Illusztrált‑Figuren „nur angesagt") | XXI‑fogás 60 und király uhu 15 weichen von [ITVB]/[BTE]/[MKVM]/[PAG-I] ab; „nur angesagt" ist für die sechs Illusztrált‑Figuren dagegen **korrekt** | Werte **ignorieren**, Aussage zu „nur angesagt" übernehmen |
| 23 | **5 Spieler: Geber prüft Skarts / verteilt den Talon** | ⚠ [PAG-I]/[PAG-P]/[PAG-EX] ja; ✅ [ITVB 5.1], [PVSZ18 6.6] („nem nézheti meg, **nem nyúlhat hozzá**"), [KEGL] nein | **nein** (Nutzervorgabe + ITVB/PVSZ18) |
| 24 | **Kontra‑Stufen** | szubkontra als höchste (×8) [MKVM]; hirskontra (×16) [ITVB 6], [BTE 5], [PVSZ18 4.8]; mordkontra (×32) [PAG-I] | **hirskontra** (`max_kontra_level=4`) |
| 25 | **Alleinspiel: Grundwert** | Quellen schweigen; [UZO] rechnet Alleinspiele intern wie `szóló` (g = 4) | **Grundwert bleibt das Gebot**; [UZO]‑Praxis nur als Option dokumentieren (`lone_is_solo_value=false`) |
| 26 | **Erzwungene stille Ultimó‑Wertung nach gefallenem uhu** | [ITVB 8.6], [BTE 7.5] (wörtlich identisch) | umsetzen (§5.8) |
| 27 | **centrum‑Zuordnung, wenn der XX bei der Gegenpartei liegt** (nur in Invitspielen möglich) | [ITVB 7.2] nimmt centrum von der „letzter Sprecher"-Regel aus, sagt aber nicht, wem es dann gehört | **App‑Entscheidung:** centrum gilt als Ansage der Spielmacher‑Partei; ein Gegner, der centrum ansagen will, muss sich vorher durch Kontra offenbaren (dokumentiert im Lernmodus) |
| 28 | **Konventionen des Buches 1948 als Pflicht** (z. B. „bei centrum muss mit Tarock ausgespielt werden", „Skíz mit 7 Tarock muss bis szóló bieten") [TA48] | historisch als „kötelező szabály" formuliert, heute Taktik. **⚠ Nicht am Original verifizierbar** (§13, Prüfprotokoll) | **nicht erzwingen**; als KI‑Heuristik und Lerntipp verwenden |
| 29 | **[ITVB 7.4] vs. [ITVB 7.14]** – „a figurákat … akár meg is **ismételheti**" (7.4) gegen „A pár által bemondott figura **ismételt bemondása renonsz**" (7.14) | Interner Widerspruch des ITVB‑Textes; [BTE] enthält keine der beiden Klauseln | **App‑Entscheidung:** 7.4 regelt nur die **Reihenfolgefreiheit innerhalb eines Zuges** (man darf im selben Zug mehrere Figuren in beliebiger Ordnung nennen und eine gerade gesagte wiederholen/bekräftigen); 7.14 verbietet die **erneute Ansage einer bereits gültig angesagten** Figur. Die Engine verbietet die Wiederholung (`A_REPEAT`) |
| 30 | **Ist die tarokkszám kontrierbar?** | Keine Quelle sagt es ausdrücklich. [ITVB 6]/[BTE 5] beziehen das Kontra auf die „bemondott **figura**"; [PAG-I] trennt „declarations" durchgehend von „bonuses"; [PAG-P §Variations] kennt Kreise, die die Deklaration „like a bonus announcement" behandeln – dort ginge es um die **Parteizuordnung**, nicht um Kontra | **nicht kontrierbar** (`tarokkszam_kontrable=false`) |
| 31 | **Zweiter Sprung im selben Lizit** | [ITVB 4.9]/[BTE 3.8]/[PVSZ18 5.2.4] „Viszontinvit nincs" (kein Gegen‑Invit); [PAG-I] präzisiert: ein zweiter Sprung ist **erlaubt**, aber bedeutungslos, und darf mit jedem Blatt mit honőr gemacht werden | **erlaubt, bedeutungsfrei** (`second_jump = allowed_meaningless`) |
| 32 | **Weitere Konventionsvarianten** (nicht implementiert, nur dokumentiert) | [PAG-P §Variations]: erstes Gebot nie konventionell; Invit erlischt, wenn der Invitgeber später selbst hält/überbietet; Invit nur für Spieler, die schon geboten haben; **József Pais**: einfacher Sprung in der **zweiten** Runde = XVII‑Invit; gar keine Konventionen | **nicht anbieten** (Profilraum sonst zu groß) |

---

---

## 10. Hinweise für den heuristischen Computergegner

Ungarisches Tarokk ist ein Spiel mit **verdecktem Partner, Pflichtkonventionen und hohen Prämien**. Eine KI,
die nur Kartenpunkte maximiert, spielt erkennbar falsch: Die Prioritätenliste der ungarischen Schule lautet
[PAG-P §Play]:

```
1. XXI fangen bzw. den eigenen XXI retten        (21/42 Punkte!)
2. pagát ultimó machen bzw. verhindern           (5/10)
3. parti bzw. duplajáték machen                  (g … 4g)
4. pagát fangen/retten (→ tulétroá)              (1/2)
5. Könige einbringen (→ négykirály)              (1/2)
```
Diese Reihenfolge ändert sich, sobald Figuren angesagt (und ggf. kontriert) wurden – dann zählt zuerst, was
am teuersten ist.

### 10.1 Informationsmodell (Inferenz)

Die KI führt pro Gegner eine **Constraint-Menge**; daraus wird determinisiert (§10.9):

| Beobachtung | Schlussfolgerung |
|---|---|
| Spieler hat geboten | hat **mindestens einen honőr** (§3.2) – Ausnahme `próbahárom` in vierter Position |
| Spieler hat gepasst, obwohl er halten musste (§3.5) | hat XX + großen honőr + ≥ 5 Tarock (engedés) |
| Sprunggebot | hat XIX bzw. XVIII + großen honőr + ≥ 5 Tarock |
| Spielmacher ruft XX | Spielmacher hat **nicht** den XX |
| Spielmacher ruft XIX (ohne Invit) | Spielmacher hat den XX |
| „x tarokk fekszik" | so viele Tarocke sind aus dem Spiel; die des Spielmachers sind sogar **namentlich** bekannt (offen gezeigt) |
| tulétroá des Spielmachers in Runde 1 | beide große honőr (bei drei Bietern: Skíz) [ITVB 7.7] |
| tulétroá des Partners | mindestens ein großer honőr, meist ≥ 5 Tarock |
| négykirály nach tulétroá | „höchster unbekannter Tarock" (bei XX‑Ruf also XIX; hat der Spielmacher ohne Invit den XIX gerufen, also der XVIII; im Invitspiel der XX) – [ITVB 7.9] (nur *erős ajánlás*), [BTE 6.8], [MKVM], [PAG-I §Announcing Four Kings]. Zeigt zusätzlich meist ≥ 5 Tarock |
| duplajáték | ≥ 6 Tarock, inkl. des **zweithöchsten** unbekannten Tarocks (bei XX‑Ruf also der XVIII; ist der XVIII gerufen, der XIX) [PAG-I §Announcing Double Game] |
| Ansage ohne vorheriges Kontra | Ansager ist Partner des zuletzt Sprechenden (§5.2) |
| 8/9 tarokk | exakte Tarockzahl; Pflichtansage bei pagát-/sas‑Figuren (§5.11) |
| Spieler bedient eine Farbe nicht | Farbe leer (harte Constraint) |
| Spieler spielt hohen, aber nicht stechenden Tarock | klassisches **Fangsignal**: „Ich habe den Skíz und will nicht in Vorhand kommen" [PAG-P §Catching the XXI] |
| Spieler spielt sehr kleinen Tarock, obwohl größerer verloren gehen könnte | Signal „kein Fang geplant" |
| [KEGL] Fangsignal beim Tarockspiel | **Wenn ein hoher Tarock ausgespielt wurde:** Zugabe ≥ X = „Tarock spielen!"; ≤ IX = „Farbe spielen!". Fortsetzung: fallende Reihe = „Tarock", steigende Reihe = „Farbe" [KEGL, *tarokk_basic.pdf* §Playing] |

Zusätzlich zählt die KI **Tarocke mit** (22 gesamt), verfolgt honőr, `II`, `XVI–XX` und alle Könige –
[MKVM]‑Tipp: „A lejátszásnál számolni kell a már ütésbe került tarokkokat."

### 10.2 Blattbewertung

```
tarockScore  = Σ über Tarock: w(rang)        w(Skíz,XXI)=4, w(XX,XIX,XVIII)=3, w(X..XVII)=1.5, w(<X)=1
suitPenalty  = 2 * (Anzahl verschiedener Farben)            // wenige Farben = stark
kingBonus    = +1 je König MIT Beikarte, −2 je blanker König
honourBonus  = Skíz +4, XXI +3 (−2 wenn < 5 Tarock: „schwacher XXI"), pagát +1 (+3 ab 7 Tarock)
```
Richtwerte [PAG-P §Assessing the strength]: Durchschnitt = **5 Tarock**; stark = 6–7 Tarock mit 2–3 hohen
(≥ XVIII) plus Mitteltarock; „7–8 kleine Tarocke" sind schwächer, als sie aussehen.

### 10.3 Lizit‑Heuristik (aus [KEGL] *tarokk_basic.pdf* und [PAG-P §Bidding])

* **Mit Skíz praktisch immer bieten** (man will die Partie spielen, um den XXI fangen zu können).
* Mit XXI oder pagát bieten, außer das Blatt ist deutlich unterdurchschnittlich.
* **Schwacher XXI (≤ 5 Tarock) sollte trotzdem bieten** – „Totstellen" funktioniert gegen gute Spieler
  nicht; zwei Talonkarten erzeugen eine leere Farbe als Fluchtweg.
* **XX + pagát ohne großen honőr:** passen und darauf warten, gerufen zu werden (Ausnahme: zwei andere haben
  schon geboten).
* **Wenn der linke Nachbar `három` sagt, mit honőr automatisch `kettő` bieten** – man wird entweder zum
  Partner eines starken engedés‑Blattes oder bekommt eine Talonkarte mehr.
* **Invit** nur mit XIX/XVIII + großem honőr + ≥ 6 Tarock (5 ist das Regelminimum), guter Farbe (König +
  Beikarte).
* **Fangkonstellation** (Skíz sitzt *nach* dem XXI, keiner der beiden hat den XX): Mit Skíz und ≥ 4 Tarock
  auf `egy` gehen; mit ≥ 7 Tarock `szóló`. Mit XXI umgekehrt: `egy` halten mit ≤ 6 Tarock, `szóló` halten mit
  ≤ 4–5.
* In vierter Position nach drei Pässen: `próbahárom` nur mit sehr gutem Blatt (das Risiko ist der Strafbetrag
  §9.15); alternativ passen und die Partie einwerfen lassen.

### 10.4 Skart‑Heuristik

Prioritäten [KEGL §Exchange]:
1. **Farbenzahl minimieren** (freie Hand für Tarockführung),
2. **Punktwert des eigenen Skarts maximieren** – aber: Wer den XX hält und gerufen werden wird, drückt
   **billig** (sein Skart zählt für die Gegenpartei!) [PAG-P §Discarding],
3. Sprichwörtliche Feinheiten: „Der Partner des Spielmachers meidet ein blankes Káró", „Der pagát flieht auf
   Pikk", „Schwarze Farben gegen das Ultimó".

Gegenregeln (Farben behalten):
* Wer als XXI‑Besitzer **Fluchtfarben** braucht (3 verschiedene Farben, notfalls sogar Tarock drücken),
* wer mit dem **Skíz auf Fang** geht (2 Farben behalten, um nicht in Vorhand zu geraten),
* Verteidiger mit 6–7 Tarock, die ein Ultimó vermuten,
* Vorhand‑Verteidiger: eine **königlose Farbe** behalten (ideales Ausspiel).

### 10.5 Ansage‑Heuristik

* **tulétroá:** als Spielmacher mit beiden großen honőr und ≥ 6 Tarock (bzw. 5 hohe + lange Farbe); als
  Partner mit großem honőr und ≥ 5 Tarock; nach „8/9 tarokk" des Spielmachers schon mit 4 bzw. 3 Tarock
  [KEGL].
* **négykirály** ist in erster Linie **Aufforderung** („biztatás"), kein Selbstzweck; zeigt den höchsten noch
  unbekannten Tarock und meist ≥ 5 Tarock.
* **duplajáték:** ≥ 6 Tarock inkl. zweithöchstem unbekannten Tarock; bei `egy`/`szóló` vorsichtig sein – die
  Gegner haben 15–20 Punkte im Skart.
* **pagát ultimó:** ≥ 7 Tarock, mehrere hohe, plus positives Signal des Partners; Pflicht: tarokkszám ansagen.
* **XXI‑fogás** ansagen fast nur, wenn Partner vorher „négykirály/dupla" als Aufforderung gezeigt hat und
  die Sitzordnung stimmt.
* **centrum** braucht i. d. R. die fünf höchsten Tarock in der Partei **und** das Ausspiel; kismadár/nagymadár
  entsprechend eine Karte mehr; sie lohnen fast nur nach tulétroá + négykirály‑Dialog.
* **király ultimó:** starke Hand (6–7 Tarock) + der König sollte eine **Beikarte** haben (sonst kann die
  Gegenseite die Farbe anspielen); notfalls im Skart eine Farbe bewusst auf „König + 1" reduzieren.
* Nie dupla + volát unüberlegt: Eine kontrierte, verfehlte dupla bei `szóló` kostet 4 × 4 × 2 = 32.

### 10.6 Kontra‑Heuristik

* „kontra játék" als Verteidiger mit ≥ 6 Tarock inkl. Skíz/XXI und wenn die eigene Seite Kartenpunkte hat.
* „kontra tulétroá" lohnt vor allem mit dem pagát + langen Tarocken (man kann ihn retten).
* „kontra négykirály" ist der häufigste Konter: Die Ansage ist oft nur Aufforderung.
* „kontra pagátulti/uhu" nur mit mindestens einem sicheren Tarockstich am Ende – und **Pflicht**, dabei 8/9
  tarokk anzusagen, falls vorhanden.
* Als Ansager rekontrieren, wenn die eigene Annahme durch die Ansagen bestätigt wurde (z. B. beide große
  honőr in der Partei).

### 10.7 Spiel‑Heuristik

**Fangen des XXI** [PAG-P §Catching the XXI]
* Der Skíz‑Besitzer will **nicht in Vorhand** kommen: möglichst hohe, aber nicht stechende Tarocke legen.
* Ideales Fangblatt: Skíz + 5–6 kleine Tarocke, Rest in verschiedenen Farben (keine Könige).
* Muss er ausspielen, dann eine Farbe **ohne König**, in der Hoffnung, dass der XXI bedienen muss.
* Der Partner des Skíz spielt hohe Tarocke, wenn der Skíz **nach** dem XXI sitzt.

**Retten des XXI**
* Der XXI‑Besitzer sucht eine „Fluchtfarbe" (Farbe ohne König, in der der Skíz bedienen muss) oder wartet,
  bis der Skíz gezwungen ist zu stechen.
* Der Partner spielt Tarock, wenn der XXI längere Tarocke hat; sonst Farben.

**pagát ultimó** [PAG-P §Making pagátultimó]
* Der pagát‑Besitzer (längere Tarockhand) beginnt mit **mittleren/kleinen** Tarocken, lässt den Partner
  stechen und zurückspielen; die hohen Tarocke braucht er am Ende, um die letzten fremden Tarocke zu ziehen.
* Der Partner hält bewusst einen **kleinen** Tarock bis zum Schluss zurück.
* Als Verteidiger gegen ein Ultimó: Farben spielen, die den pagát‑Partner zum Stechen zwingen, und den
  eigenen letzten Tarock für den 9. Stich aufheben.

**parti / duplajáték**
* „kurta üt" – der Partner mit den **kürzeren** Tarocken nimmt die frühen Stiche.
* Gegner ihre Stiche früh und billig (tarockreich) machen lassen.
* Könige nicht billig hergeben: Wenn links ein kleiner Farbwert kommt, mit **hohem** Tarock stechen.

### 10.8 Prüfbare Spielregeln für die KI (Hard Constraints)

Die KI darf nie: eine gebundene Karte zu früh spielen (§6.4), eine Ansage machen, die die Partei falsch
identifiziert (§5.2), eine Konvention verletzen, die als Regel gilt (§3.5, §5.5) – der Engine‑Validator
weist das ohnehin zurück, aber die KI soll solche Züge gar nicht erst erzeugen.

### 10.9 Suchverfahren

Empfehlung: **PIMC/ISMCTS mit Determinisierung**.

1. **Sampling:** Erzeuge N (Default 60–200, je nach Gerät) Weltzustände, die alle bekannten Constraints
   erfüllen (§10.1) – Ziehen mit Rejection‑Sampling, Fallback: gewichtetes Zuweisen.
2. **Rollouts:** In jedem Sample vollständige Partie mit schnellen Heuristik‑Policies (§10.7) ausspielen,
   Bewertung = **Punkte der Abrechnung** (nicht Kartenpunkte!), damit Prämien korrekt gewichtet werden.
3. **Zugwahl:** Mittelwert über die Samples, Tie‑Break durch Heuristikpriorität.
4. **Ansagen/Lizit:** Regelbasiert (§10.3/§10.5) – die Suche lohnt dort erst in einer späteren Version.

**Performance‑Budget:** Sailfish‑Zielgeräte (Xperia 10 III o. ä.) – maximal ~150 ms pro Zug im Level
„Normal", ~600 ms in „Stark"; Determinisierung und Rollouts in einem Worker‑Thread, Ergebnis per Queued
Connection an die QML‑Ebene.

### 10.10 Schwierigkeitsstufen

| Stufe | Verhalten |
|---|---|
| **Kezdő (Anfänger)** | Nur Heuristik, keine Suche; ignoriert Fangsignale; sagt nur tulétroá/négykirály; kontriert selten |
| **Haladó (Fortgeschritten)** | Heuristik + 40 Samples; nutzt Konventionen und Inferenz; sagt Ultimó/centrum, wenn die Bedingungen klar erfüllt sind |
| **Mester (Meister)** | 150+ Samples, Fangsignale aktiv (sendet und liest sie), Kontra‑Ökonomie, Skart optimiert auf Figurenchancen |

Für den Lernmodus zusätzlich: **„Gläserner Gegner"** – die KI erklärt ihre Entscheidungen mit genau den
Konventionsregeln aus §5.5/§10.1 („Ich sage négykirály, weil ich den XIX habe und dich zu centrum einlade").

---

## 11. Inhalte für den Lernmodus („Alles erklärt")

Der Lernmodus ist kein Anhang, sondern ein eigener Spielmodus: Er erklärt **jede Phase**, begründet **jede
Regel**, zeigt bei jeder gesperrten Aktion **warum** sie gesperrt ist, und enthält drei fest vorgegebene
Übungspartien. Alle Texte unten sind als **finale deutsche UI‑Texte** formuliert (Platzhalter in `{}`),
ungarische Fachbegriffe bleiben stehen und sind antippbar → Glossar (§11.7).

### 11.1 Aufbau

| Modul | Inhalt | Abschluss |
|---|---|---|
| **L0 – Die Karten** | 42 Karten, Tarockreihe, rote/schwarze Farbreihenfolge, Kartenwerte, 94 Punkte, 48 zum Sieg | Mini‑Quiz: „Welche Karte ist höher?", „Wie viele Punkte sind das?" |
| **L1 – Ein Stich** | Farbzwang, Tarockzwang, kein Überstichzwang, Stichgewinn | 5 gestellte Stiche spielen |
| **L2 – Geben & Talon** | 6 zum Talon, 5+4 an jeden, Talonverteilung nach Gebot, Drücken, Verbote | Drück‑Übung mit Erklärung |
| **L3 – Lizit** | három/kettő/egy/szóló, honőr‑Pflicht, tartom, invit, engedés | Lizit‑Quiz mit 6 Situationen |
| **L4 – Partner & Ansagen** | XX rufen, die Ansagerunde, tulétroá/négykirály als Dialog, Kontra | „Wer ist mein Partner?"-Übung |
| **L5 – Figuren** | Alle 15 Figuren, still vs. angesagt, gebundene Karten | Übungspartie 1 (§11.8.1) |
| **L6 – Der Fang** | XXI‑fogás, Sitzordnung, Signale, polgármester | Übungspartie 3 (§11.8.3) |
| **L7 – Abrechnung** | Punktezählung, Multiplikatoren, dupla/volát‑Logik | Abrechnungs‑Quiz |

Zusätzlich global: **Regel‑Lexikon** (durchsuchbar, = §1–§9 in Kurzform), **Glossar** (§11.7), **„Warum
war das verboten?"‑Historie** der letzten 10 gesperrten Aktionen.

### 11.2 Die Phasen in einfacher Sprache

**1. Geben.** „Der Geber mischt, der linke Nachbar hebt ab. Zuerst kommen **6 Karten verdeckt in die Mitte**
– das ist der **talon**. Dann bekommt jeder erst 5, dann 4 Karten, also 9. Ausgeteilt und gespielt wird
**nach rechts**. Wer rechts vom Geber sitzt, heißt **induló** (Vorhand): Er spricht im Lizit zuerst und
spielt später die erste Karte aus – egal, wer das Spiel gewinnt. Bei fünf Spielern setzt der Geber aus und
schaut nur zu; er sieht keine verdeckten Karten."

**2. Lizit.** „Jetzt wird versteigert, wer **felvevő** (Spielmacher) sein darf. Es geht nicht darum, ein Ziel
zu versprechen, sondern darum, **wie wenige Karten du aus dem Talon nimmst**: `három` = 3 Karten (billigstes
Spiel, Wert 1), `kettő` = 2 (Wert 2), `egy` = 1 (Wert 3), `szóló` = 0 (Wert 4). **Mitbieten darf nur, wer
einen honőr hat** – also Skíz, XXI oder pagát. Wer schon geboten hat und überboten wurde, darf mit
**`tartom`** („ich halte") gleichziehen. Vorsicht: Ein **Sprung** über ein Gebot ist kein Angeberzug, sondern
ein **Versprechen** (invit): ein Sprung = „Ich habe den XIX", zwei Sprünge = „Ich habe den XVIII". Und wenn
du `három` gesagt hast, jemand `kettő` bietet und die beiden anderen passen, dann heißt **dein Passen**:
„Ich habe den XX!" (**engedés**). Diese Versprechen musst du halten – sie sind Regeln, keine Höflichkeit."

**3. Talon & Drücken.** „Der Spielmacher nimmt so viele Talonkarten, wie sein Gebot sagt; die restlichen
Talonkarten bekommen die anderen. Dann legt jeder genau so viele Karten wieder weg (**skart**). **Nie
drücken darfst du: Skíz, XXI, pagát, jeden König und den XX** (und in einem Invitspiel die versprochene
Karte). Tarock darfst du drücken, musst es aber **ansagen** – und als Spielmacher sogar offen zeigen. Dein
Skart zählt am Ende zu deinen Stichen, wenn du Spielmacher bist; die Skarts der anderen drei zählen für die
Gegenpartei – auch der deines Partners!"

**4. Rufen.** „Der Spielmacher ruft einen Tarock – fast immer den **XX**. Wer ihn hat, ist sein Partner,
**sagt aber nichts**. Oft merkt man erst mitten im Spiel, wer zusammengehört. Hat der Spielmacher den XX
selbst, ruft er den höchsten Tarock, den er **nicht** hat (XIX, sonst XVIII …) – oder er ruft seinen eigenen
XX und spielt **allein gegen drei**."

**5. Ansagerunde.** „Jetzt sagt man, was man zusätzlich schaffen will (**figurák**). Man kann fast alles auch
**still** („csendben") erfüllen und bekommt dann den **halben** Preis – angesagt gibt es den vollen, aber man
zahlt ihn auch, wenn es schiefgeht. Die Gegner können **kontra** sagen (doppelter Preis), die Ansager
**rekontra** (vierfach) usw. Wichtig: Weil der Partner geheim ist, gilt die Regel: **Wer etwas ansagt, gilt
als Partner des zuletzt Sprechenden.** Willst du etwas ansagen, obwohl du gegen den zuletzt Sprechenden
spielst, musst du dich vorher mit einem Kontra zu erkennen geben."

**6. Spiel.** „Neun Stiche. **Farbe muss bedient werden**, sonst **muss Tarock gespielt** werden; hast du
beides nicht, ist alles erlaubt. **Stechen musst du nie** – du darfst auch einen kleinen Tarock „unterlegen".
Es gewinnt der höchste Tarock, sonst die höchste Karte der ausgespielten Farbe. Wenn deine Partei eine Figur
angesagt hat, die eine bestimmte Karte für einen bestimmten Stich verlangt (z. B. pagát ultimó), ist diese
Karte **gebunden**: Du darfst sie vorher nicht spielen und musst sie im Zielstich spielen."

**7. Abrechnung.** „Jede Partei zählt ihre Kartenpunkte (plus den passenden skart). Zusammen sind es immer
**94**. Die Spielmacher‑Partei gewinnt mit **48**; bei 47:47 gewinnen die Gegner. Dazu kommen die Figuren.
Alles wird **pro Spieler** verrechnet: In einer 2‑gegen‑2‑Partie zahlt jeder Verlierer einem Gewinner den
Betrag; wer allein gegen drei spielt, kassiert oder zahlt **dreimal**."

### 11.3 Wozu es die einzelnen Verträge und Figuren gibt – und wann man sie wählt

| Element | Wozu es existiert | Wann sagst du es an? |
|---|---|---|
| **három** | Standardvertrag: Du willst vor allem **drei Talonkarten**, um dein Blatt zu reparieren und ein voides Farbbild zu bauen | Immer, wenn du einen honőr hast und dein Blatt durchschnittlich ist |
| **kettő** | Ein Zwischengebot – oft nur, **um den engedés zu provozieren**, wenn links `három` gesagt wurde | Mit honőr, wenn dein linker Nachbar `három` gesagt hat |
| **egy / szóló** | Höherer Spielwert; **du verzichtest auf Talonkarten**, damit die Gegner sie nicht bekommen, oder um die Fangsituation zu steuern | Nur mit starkem Blatt (≥ 7 Tarock), oder in Fangkonstellationen (§10.3) |
| **tulétroá** (2/1) | Die drei honőr sind die „Kronjuwelen". Die Ansage ist vor allem ein **Informationssignal**: „Ich habe beide großen honőr" | Als Spielmacher mit Skíz **und** XXI; als Partner mit einem großen honőr und ≥ 5 Tarock |
| **négykirály** (2/1) | Könige kann man nicht drücken und sie werden gern gestochen – deshalb ist die Ansage vor allem **Aufforderung** an den Partner („ich habe den nächsthöheren Tarock, sag mehr an!") | Nach tulétroá, wenn du den höchsten noch unbekannten Tarock hast (meist XIX) und ≥ 5 Tarock |
| **duplajáték** (4g/2g) | Belohnt ein **Übergewicht** von ¾ der Punkte; zugleich Signal „mein Blatt ist sehr stark" | Mit ≥ 6 Tarock inkl. des zweithöchsten unbekannten Tarocks; bei `egy`/`szóló` vorsichtig (Gegnerskart!) |
| **volát** (6g/3g) | Der Traum: alle neun Stiche | Nur nach klarer Aufforderung des Partners und mit lückenloser Tarockkontrolle |
| **pagát ultimó** (10/5) | Der kleinste Tarock gewinnt den letzten Stich – das klassische Kunststück | ≥ 7 Tarock inkl. hoher; Pflicht: `8/9 tarokk` mit ansagen, wenn du sie hast |
| **sas ultimó / sas uhu** | Dieselbe Idee mit dem `II` | Selten; nur mit sehr langer Tarockhand |
| **XXI‑fogás** (42/21) | Die teuerste Einzelprämie: Der Skíz schnappt den zweithöchsten Tarock. Sie ist der **Grund**, warum in Ungarn so viel „Katz und Maus" gespielt wird | Ansagen fast nur, wenn der Partner per négykirály/dupla dazu eingeladen hat und die Sitzordnung passt (Skíz **nach** dem XXI) |
| **centrum / kismadár / nagymadár** (je 10) | Belohnen perfekte Kontrolle der ersten Stiche mit XX / XXI / Skíz | Nur nach dem Ansage‑Dialog (tulétroá + négykirály) und meist nur mit dem Ausspiel |
| **pagát uhu (15) / király uhu (20)** | Vorletzter Stich mit pagát bzw. einem bestimmten König | uhu ist **schwerer** als ultimó – der 8. Stich ist noch voller Tarock; nur mit Kontrolle |
| **király ultimó** (15) | Letzter Stich mit einem **benannten König** – der König ist keine Stechkarte, deshalb so wertvoll | Am besten mit König **plus Beikarte** in derselben Farbe, damit die Gegner die Farbe nicht totspielen können |
| **8/9 tarokk** (1/2) | Kein Kunststück, sondern eine **Meldung**: „Ich habe sehr viele Tarock." Sie macht Ultimós erst planbar | Immer, wenn es dem Partner hilft – **Pflicht** bei pagát-/sas‑Ansagen und deren Kontra |
| **kontra** | Macht Ansagen teuer und **beweist zugleich, auf welcher Seite du stehst** | Mit Karten, die die Ansage konkret verhindern – nicht aus Prinzip |

### 11.4 Die Ansage‑Konventionen als „Gespräch" (Lerntext)

„Die Ansagen sind eine **Sprache**. Zwei Beispiele:

```
Spielmacher:  „Ich rufe den XX – tulétroá."      → Ich habe Skíz UND XXI.
Partner (XX): „négykirály."                      → Ich habe den XIX und ein starkes Blatt: Sag mehr an!
Spielmacher:  „centrum."                         → Dann nehme ich die ersten fünf Stiche, den fünften mit dem XX.
```
```
Spielmacher:  „Ich rufe den XX."  (kein tulétroá) → Mir fehlt mindestens ein großer honőr.
Gegner:       „kontra játék."                     → Ich glaube nicht an 48 Punkte – und ich bin Gegner.
Partner:      „tulétroá."  (ohne rekontra)        → Ich habe einen großen honőr … aber ich bin NICHT der XX!
```
Wer diese Sätze ohne die versprochenen Karten sagt, begeht am realen Tisch einen **renonsz** – in der App
sperrt das Spiel solche Ansagen (§5.5)."

### 11.5 Typische Anfängerfehler (mit Gegenmittel)

1. **Mit Tarock stechen, obwohl man die Farbe noch hat.** → Farbzwang geht immer vor (§6.2).
2. **Den XX oder einen König drücken wollen.** → Verboten (§4.3); Könige sind Ballast, aber man muss sie
   ausspielen oder vom Partner stechen lassen.
3. **Ohne honőr bieten** (bzw. den Sinn des Lizits missverstehen). → Das Gebot sagt nur, **wie viele
   Talonkarten** du nimmst.
4. **Passen, wenn man halten müsste.** → Passen nach `három`–`kettő` ist ein **Versprechen** (XX + großer
   honőr + 5 Tarock).
5. **Sprunggebot als „ich bin stark".** → Ein Sprung ist ein **invit** und verspricht XIX bzw. XVIII.
6. **tulétroá ohne großen honőr.** → Im Default‑Profil gesperrt (§5.5).
7. **Ansagen, ohne sich zu erkennen zu geben.** → Wer gegen den zuletzt Sprechenden spielt, muss erst
   kontrieren (§5.2).
8. **Den pagát „retten" wollen, obwohl pagát ultimó angesagt ist.** → Gebundene Karte (§6.4).
9. **Alle hohen Tarocke zuerst spielen.** → Dann fallen die letzten (teuren) Stiche an die Gegner; besser
   „kurta üt": der kurze Partner sticht früh.
10. **Den eigenen XXI leichtfertig in einen Stich legen, in dem der Skíz noch sitzen kann** – 21 bzw. 42
    Punkte Verlust und der Hut des **polgármester**.
11. **Vergessen, dass 47:47 die Gegner gewinnen** – die Spielmacher brauchen **48**.
12. **Bei volát zusätzlich stille tulétroá/négykirály erwarten** – die entfallen (§5.4). Deshalb ist es bei
    `három` oft besser, einen Stich abzugeben: dupla (2) + tulétroá (1) + négykirály (1) = 4 > volát (3).
13. **Den letzten Stich unterschätzen:** Er entscheidet über ultimó‑Prämien und enthält oft die Könige.
14. **Bei fünf Spielern als Geber mitreden.** → Der Geber ist reiner Zuschauer.

### 11.6 „Warum ist das nicht erlaubt?" – vollständige Meldungstexte

Die Engine liefert zu **jeder** abgelehnten Aktion einen `RuleViolation{ id, params }`; die UI zeigt den Text
unten (Kurzform als Toast, Langform im „Warum?"-Dialog mit Regelverweis). Die Engine verhindert illegale Züge
schon in der Auswahl – die Meldung erscheint, wenn der Spieler eine gesperrte Karte antippt.

#### 11.6.1 Kartenspiel

| ID | Auslöser | Kurztext | Langtext (Lernmodus) |
|---|---|---|---|
| `P_FOLLOW_SUIT` | Spieler hat die ausgespielte Farbe noch | **Farbzwang** | „Es wurde {Farbe} ausgespielt und du hast noch {Farbe} auf der Hand. **Színre színt kell adni** – Farbe muss bedient werden, auch wenn du mit einem Tarock stechen könntest. (Regel §6.2)" |
| `P_MUST_TAROCK` | Spieler ist in der ausgespielten Farbe leer, hat aber Tarock | **Tarockzwang** | „Du hast kein {Farbe} mehr. Dann **musst du einen Tarock spielen** – auch dann, wenn dein Tarock den Stich nicht gewinnt. (Regel §6.2)" |
| `P_TAROCK_ON_TAROCK` | Tarock ausgespielt, Spieler hat Tarock | **Tarockzwang** | „Es wurde ein Tarock ausgespielt. Solange du Tarock hast, musst du Tarock zugeben. (Regel §6.2)" |
| `P_BOUND_TOO_EARLY` | gebundene Karte, Zielstich noch nicht erreicht, andere Karte erlaubt | **{Karte} ist gebunden** | „Deine Partei hat **{Figur}** angesagt. Der/die **{Karte}** ist damit für den **{n}. Stich gebunden** (lekötött lap) und darf vorher nicht gespielt werden, solange du eine andere erlaubte Karte hast. (Regel §6.4)" |
| `P_BOUND_MUST_PLAY` | Zielstich erreicht, gebundene Karte erlaubt, andere Karte gewählt | **Jetzt muss {Karte} kommen** | „Ihr habt **{Figur}** angesagt – im **{n}. Stich** muss die versprochene Karte **{Karte}** gespielt werden (rájátszás), auch wenn ihr den Stich damit verliert. (Regel §6.4)" |
| `P_BOUND_ORDER` | mehrere gebundene Karten, nur gebundene Karten legal | **Reihenfolge der Zusagen** | „Du hast nur noch gebundene Karten. Dann muss zuerst die Karte für den **früheren** Stich heraus: **{Karte}** (für Stich {n}) – auch wenn dadurch **{Figur}** verloren geht. (Regel §6.4)" |
| `P_PAGAT_ULTI_HOLD` | angesagtes pagát/sas ultimó, Spieler will die Karte früher spielen | **pagát bleibt liegen** | „Ihr habt **pagát ultimó** angesagt: Der pagát muss bis zum **9. Stich** aufgehoben werden. Auch wenn du siehst, dass er nicht mehr gewinnen kann, darfst du ihn nicht früher abwerfen. (Regel §6.4)" |
| `P_NOT_YOUR_TURN` | Spieler nicht am Zug | **Nicht am Zug** | „Zuerst spielt {Spieler}. Gespielt wird immer **nach rechts** (gegen den Uhrzeigersinn). (Regel §2.1)" |
| `P_NOT_YOUR_CARD` | Karte gehört nicht zur Hand | **Fremde Karte** | „Diese Karte liegt nicht in deiner Hand." |
| `P_LEAD_ANY` (Hinweis, keine Sperre) | Spieler spielt aus | **Ausspiel frei** | „Beim Ausspiel darfst du **jede** Karte spielen – es gibt keine Vorgaben (Ausnahme: gebundene Karten)." |
| `P_NO_OVERTRUMP` (Hinweis) | Spieler legt kleineren Tarock unter | **Kein Überstichzwang** | „Das ist erlaubt: Im Tarokk gibt es **keinen Überstichzwang** (nincs felülütési kényszer) – du darfst untertarockieren, um deine hohen Tarocke zu sparen oder Signale zu geben. (Regel §6.3)" |

#### 11.6.2 Drücken (skart)

| ID | Auslöser | Kurztext | Langtext |
|---|---|---|---|
| `S_HONOUR` | Skíz, XXI oder pagát gewählt | **honőr bleibt im Blatt** | „**Skíz, XXI und pagát (honőr) dürfen nie gedrückt werden.** Sie sind die Karten, um die sich das ganze Spiel dreht. (Regel §4.3)" |
| `S_KING` | König gewählt | **Könige bleiben im Blatt** | „Könige dürfen **nie** gedrückt werden – auch wenn sie oft gestochen werden. Genau deshalb ist ein blanker König eher eine Last als ein Vorteil. (Regel §4.3)" |
| `S_XX` | XX gewählt (Profil ITVB) | **Der XX bleibt im Blatt** | „Der **XX** ist die Rufkarte des Spiels und darf nicht gedrückt werden (Profil ITVB/BTE). In manchen Kreisen ist es erlaubt – dann kann es passieren, dass der Spielmacher plötzlich allein gegen drei spielt. (Regel §4.3, Variante §9.1)" |
| `S_INVIT_CARD` | Invitkarte (XIX/XVIII) gewählt | **Versprochene Karte** | „Du hast mit deinem Sprunggebot den **{Karte}** versprochen. Diese Karte muss im Blatt bleiben, damit der Spielmacher sie rufen kann. (Regel §4.3)" |
| `S_COUNT` | falsche Anzahl | **{n} Karten drücken** | „Du musst genau **{n}** Karten ablegen – so viele, wie du aus dem talon bekommen hast." |
| `S_TAROCK_INFO` (Hinweis) | Tarock gewählt | **Tarock drücken ist erlaubt** | „Tarock darfst du drücken. Aber: Die **Anzahl** musst du ansagen, und als Spielmacher musst du die gedrückten Tarocke **offen zeigen**. (Regel §4.4)" |
| `S_DECLARER_SKART_INFO` (Hinweis) | Spielmacher drückt | **Dein Skart zählt für dich** | „Als Spielmacher zählt dein Skart zu **deinen** Stichen – drücke also möglichst **wertvolle** Karten (dáma, lovas). Alle anderen sollten billig drücken: Ihr Skart zählt für die **Gegenpartei**. (Regel §1.5)" |

#### 11.6.3 Lizit

| ID | Auslöser | Kurztext | Langtext |
|---|---|---|---|
| `B_NO_HONOUR` | Gebot ohne honőr | **Kein honőr** | „Mitbieten darf nur, wer **Skíz, XXI oder pagát** hat. Du musst passen. (Ausnahme: Wenn die ersten drei passen, darf der vierte Spieler auch ohne honőr ein `próbahárom` wagen – dann muss er aber einen honőr aus dem talon bekommen, sonst zahlt er Strafe.) (Regel §3.2)" |
| `B_HOLD_NOT_ALLOWED` | `tartom` ohne Vorgebot / Gebot schon gehalten | **`tartom` nicht möglich** | „`Tartom` darf nur sagen, wer in diesem Lizit **schon geboten hat** und dessen Gebot überboten wurde – und nur, wenn das aktuelle Gebot noch **nicht** gehalten wurde. (Regel §3.4)" |
| `B_JUMP_WITHOUT_CARD` | Sprunggebot ohne Invitkarte/Voraussetzungen | **Sprung = Versprechen** | „Ein Sprung über das nächstmögliche Gebot ist ein **invit**: ein Sprung verspricht den **XIX**, zwei Sprünge den **XVIII** – jeweils zusammen mit einem großen honőr (Skíz oder XXI) und mindestens fünf Tarock. Diese Karten hast du nicht. (Regel §3.5)" |
| `B_PASS_IS_ENGEDES` | Passen wäre engedés, Karten fehlen | **Passen hieße: „Ich habe den XX"** | „Wenn du nach deinem `három` nur mit `kettő` überboten wurdest und die anderen gepasst haben, bedeutet dein Passen **engedés**: Du versprichst den **XX**, einen großen honőr und fünf Tarock. Ohne diese Karten musst du `tartom` sagen (oder ein Sprunggebot machen). (Regel §3.5)" |
| `B_INVITER_LOCKED` | Invitgeber will weiterbieten | **Invit ist bindend** | „Ein invit kann nicht zurückgenommen werden – nach deinem Invit darfst du nur noch passen. (Regel §3.5, [ITVB 4.6])" |
| `B_TOO_LOW` | Gebot nicht höher | **Zu niedrig** | „Jedes Gebot muss höher sein als das laufende: `három` → `kettő` → `egy` → `szóló`. Gleichziehen geht nur mit `tartom`." |
| `B_ALREADY_PASSED` | Spieler hat gepasst | **Schon ausgestiegen** | „Wer einmal gepasst hat, ist aus dem Lizit draußen." |

#### 11.6.4 Ansagen

| ID | Auslöser | Kurztext | Langtext |
|---|---|---|---|
| `A_TRULL_NO_HONOUR` | tulétroá ohne großen honőr | **tulétroá braucht einen großen honőr** | „**tulétroá** darf nur ansagen, wer **Skíz oder XXI** hält (Ausnahme: als Partner, wenn der Spielmacher von sich aus centrum/kismadár/nagymadár angesagt hat). Die Ansage ist ein Signal und muss stimmen. (Regel §5.5)" |
| `A_TRULL_DECLARER_R1` | Spielmacher, 1. Runde, nicht beide großen honőr | **Das hieße: beide großen honőr** | „In der **ersten Ansagerunde** bedeutet dein tulétroá als Spielmacher: **Skíz UND XXI** (bei drei Bietern: den Skíz). (Regel §5.5, [ITVB 7.7])" |
| `A_AFTER_VOLAT` | tulétroá/négykirály/dupla nach eigenem volát | **Nach volát nicht mehr** | „Nach einer volát‑Ansage sind tulétroá, négykirály und duplajáték nicht mehr ansagbar – volát schließt sie ein. (Regel §5.5)" |
| `A_ULTI_TO_UHU` | uhu auf dieselbe Karte nach eigenem ultimó | **Kein Hochstufen** | „Ein bereits angesagtes **ultimó** darf nicht zum **uhu** hochgestuft werden. (Regel §5.5, [ITVB 7.10]; in der pagat‑Variante darf der Partner das.)" |
| `A_SAME_TRICK` | zweite Figur derselben Partei für denselben Stich | **Nur eine Figur pro Stich** | „Eure Partei hat für den **{n}. Stich** schon **{Figur}** angesagt. Pro Stich ist nur eine Figur erlaubt (kein pagát ulti + király ulti auf denselben Stich). (Regel §5.5, [ITVB 7.13])" |
| `A_REPEAT` | Figur schon angesagt | **Schon angesagt** | „Diese Figur hat eure Partei bereits angesagt." |
| `A_TAROKKSZAM_REQUIRED` | pagát/sas ultimó/uhu angesagt oder kontriert, 8/9 Tarock nicht gemeldet | **Tarockzahl ist Pflicht** | „Wer **pagát-/sas‑ultimó oder ‑uhu** ansagt oder kontriert und **8 oder 9 Tarock** hält, **muss** die Tarockzahl ansagen. (Regel §5.11, [ITVB 7.3])" |
| `A_TAROKKSZAM_WRONG` | Zahl stimmt nicht | **Falsche Tarockzahl** | „Du hast {n} Tarock. „8 tarokk" mit neun Tarock anzusagen ist verboten." |
| `A_IDENTIFY_FIRST` | Ansage, obwohl `presumedTeam ≠ realTeam` | **Zuerst Farbe bekennen** | „Solange die Partner geheim sind, gilt: **Wer ansagt, gilt als Partner des zuletzt Sprechenden.** Du spielst aber gegen {Spieler}. Sage deshalb zuerst **{kontra/rekontra} {Sache}** – damit ist klar, auf welcher Seite du stehst. (Regel §5.2)" |
| `A_KONTRA_OWN_TEAM` | Kontra gegen eigene Ansage | **Nicht die eigene Partei** | „Kontra gibt es nur gegen Ansagen der **Gegenpartei**; die eigene Ansage kann man nur **rekontrieren**, wenn sie kontriert wurde. (Regel §5.12)" |
| `A_REKONTRA_WRONG_SIDE` | Rekontra durch Gegner | **Rekontra nur für die Ansager** | „Rekontrieren darf nur die Partei, die die Figur angesagt hat (danach kommt szubkontra der Gegner, dann hirskontra). (Regel §5.12)" |
| `A_CALL_MUST_BE_XX` | falscher Ruf | **Rufpflicht** | „Du musst den **{Karte}** rufen: {Grund}. (Regel §4.5)" – Gründe: „Du hast den XX nicht" / „Der invit verpflichtet dich zur {XIX/XVIII}" / „Beim engedés wird der XX gerufen" |
| `A_CALL_OWN_SKART` | Ruf einer selbst gedrückten Karte | **Nicht aus dem eigenen Skart** | „Eine Karte, die du selbst gedrückt hast, darfst du nicht rufen – sonst würdest du absichtlich allein spielen; das geht nur über den XX. (Regel §4.5)" |
| `A_KONTRA_NEEDS_TAROKKSZAM` | Kontra auf pagát-/sas‑ultimó oder ‑uhu mit 8/9 Tarock | s. `A_TAROKKSZAM_REQUIRED` | dito |
| `A_UHU_TO_ULTI` | ultimó auf eine Karte, für die die eigene Partei schon uhu angesagt hat | **Kein Herabstufen** | „Eure Partei hat für die **{Karte}** bereits **{uhu}** angesagt. Ein uhu darf nachträglich nicht zu einem ultimó verkleinert werden – sonst könntet ihr euch der Bindung an den 8. Stich entziehen. (Regel §5.5, [PAG-I])" |
| `A_FOURKINGS_HINT` (**Hinweis, keine Sperre**) | négykirály in der ersten Runde nach tulétroá, ohne den höchsten unbekannten Tarock | **négykirály ist eine Einladung** | „Nach einem tulétroá bedeutet **négykirály** üblicherweise: ‚Ich habe den höchsten Tarock, dessen Platz noch niemand kennt' (bei XX‑Ruf also den **XIX**, im Invitspiel den **XX**) – plus meist mindestens fünf Tarock. Das ist in [ITVB 7.9] nur eine **erős ajánlás** (starke Empfehlung) und **kein** renonsz; die App lässt die Ansage also zu, dein Partner wird sie aber so verstehen. (Regel §5.5)" |

#### 11.6.5 Sonstige Hinweise (nur informierend)

| ID | Text |
|---|---|
| `I_SILENT_FIGURE` | „Das war eine **stille Figur** (csendben): Nicht angesagt, aber erfüllt – es gibt den **halben** Preis." |
| `I_SILENT_ULTI_LOST` | „Der pagát wurde im letzten Stich gestochen: Das ist ein **verlorenes stilles pagát ultimó** – eure Partei zahlt 5, auch ohne Ansage. (Regel §5.8)" |
| `I_XXI_CATCH` | „**XXI‑fogás!** Der Skíz hat den XXI eines Gegners gefangen: {Wert} Punkte. {Spieler} ist jetzt **polgármester** 🎩." |
| `I_VOLAT_SILENT_RULE` | „Achtung: Wer **volát** macht, bekommt **keine** stille tulétroá und keine stille négykirály. Manchmal lohnt es sich, einen Stich abzugeben!" |
| `I_47_47` | „47:47 – bei Gleichstand gewinnt die **Gegenpartei**; die Spielmacher brauchen 48." |
| `I_HIVATALBOL_KONTRA` | „**Hivatalból kontra**: Die gerufene Karte liegt im Skart. {Spieler} muss das ansagen, der Spielmacher spielt **allein gegen drei** – und bekommt dafür alle sechs Skartkarten. (Regel §4.5)" |

### 11.7 Glossar (ungarisch → deutsch, mit österreichischen Entsprechungen)

| Begriff | Bedeutung |
|---|---|
| **adu** | „Trumpf" – im Tarokk **nicht** gebräuchlich; man sagt immer *tarokk* |
| **ász** | Ass (nur rote Farben), niedrigste Karte der Farbe, 1 Punkt |
| **bedobás** | Einwurf/Annullieren der Partie mit einem der Blätter aus §4.2 |
| **bemondás** | Ansage (Figur, Kontra, Tarockzahl) |
| **biztatás** | „Ermunterung": eine billige Ansage (meist négykirály/dupla), die den Partner zu einer teuren Ansage einlädt |
| **botos / bubi** | Bube (2 Punkte) |
| **centrum** | Figur: die ersten 5 Stiche, der 5. mit dem XX |
| **csendben / csendes** | „still" – Figur ohne Ansage erfüllt, halber Preis |
| **dáma** | Dame (4 Punkte) |
| **duplajáték / dupla** | Figur: mindestens 71 Kartenpunkte |
| **ellenfél / ellenpár / védők** | Gegenspieler/Gegenpartei |
| **engedés** (auch *adott játék*) | „Zulassen": Der `három`-Bieter passt und verspricht damit XX + großen honőr (+ 5 Tarock) |
| **fektetés** | Das Ablegen der Karten beim Drücken (Synonym zu *skartolás*) |
| **felvevő** | Spielmacher (wer das Lizit gewonnen hat) |
| **figura** | Prämie/Bonus (tulétroá, ultimó, …) |
| **hajtotta** | „getrieben": Der Partner des Fängers hat den XXI ins Messer getrieben (Turnierstatistik) |
| **hirskontra** | vierte Kontrastufe (×16) |
| **hivatalból kontra** | „Kontra von Amts wegen": Pflichtansage dessen, der die gerufene Karte gedrückt hat |
| **honőr** | Skíz, XXI, pagát (je 5 Punkte); **nagyhonőr** = Skíz und XXI |
| **húszas** | der XX (die übliche Rufkarte) |
| **induló** | Vorhand: rechter Nachbar des Gebers; eröffnet Lizit und Spiel |
| **invit** | Sprunggebot mit Kartenversprechen (XIX bzw. XVIII) |
| **káró / kőr / pikk / treff** | Karo ♦ / Herz ♥ / Pik ♠ / Kreuz ♣ |
| **király** | König (5 Punkte) |
| **királyultimó / királyuhu** | Figur: 9. bzw. 8. Stich mit einem benannten König |
| **kismadár** | „kleiner Vogel": erste 6 Stiche, der 6. mit dem XXI |
| **kontra / rekontra / szubkontra** | Verdopplungsstufen (×2 / ×4 / ×8) |
| **kurta üt** | „der Kurze sticht": Der Partner mit den wenigeren Tarocken nimmt die frühen Stiche |
| **lekötött lap** | „gebundene Karte": Karte, die durch eine Ansage an einen bestimmten Stich gebunden ist |
| **lovas** | Reiter/Cavall (3 Punkte) |
| **magas tarokk** | „hohes Tarokk", noch größere Ausbaustufe mit ~22 Figuren (nicht Teil dieser Spezifikation) |
| **mehet** | „los geht's" – Synonym für *passz* am Ende eines Ansagezugs |
| **nagymadár** | „großer Vogel": erste 7 Stiche, der 7. mit dem Skíz |
| **négykirály** | Figur: alle vier Könige |
| **osztó** | Geber |
| **pagát** | Tarock I (5 Punkte); österr. *Pagat* |
| **pagátfogás** | optionale Figur: den gegnerischen pagát fangen |
| **parti** | die Partie / das Grundspiel (≥ 48 Punkte) |
| **passz** | passen |
| **polgármester** | „Bürgermeister": Wer seinen XXI an den Skíz verloren hat, trägt (traditionell mit Hut) diesen Titel bis zum nächsten Fang |
| **próbahárom** | „Probe‑Drei": Gebot des vierten Spielers ohne honőr nach drei Pässen |
| **rájátszás** | Die Spielpflicht, die eine Ansage mit gebundener Karte erzeugt |
| **renonsz** | Regelverstoß (am realen Tisch mit Strafzahlung) |
| **robber** | Turnierrunde (16 bzw. 20 Partien) |
| **sas** | „Adler" = Tarock II |
| **segítő** | „Helfer" = Partner des Spielmachers |
| **skart / skartolás** | Die gedrückten Karten / das Drücken |
| **skíz** | höchster Tarock (Narrenbild, ohne Zahl); österr. *Sküs* |
| **szóló** | Vertrag ohne Talonkarten (Wert 4) |
| **színtelenítés** | „Entfärben": Farben beim Drücken komplett abwerfen |
| **talon** | die sechs verdeckten Karten in der Tischmitte |
| **tarokkszám** | Ansage von 8 oder 9 Tarock |
| **tartom** | „ich halte" (Gleichziehen im Lizit) |
| **tízes** | Zehn (nur schwarze Farben), niedrigste Karte der Farbe |
| **tulétroá / tuli / trull** | Figur: alle drei honőr (von frz. *tous les trois*) |
| **uhu** | Figur mit dem **8.** Stich (pagát uhu, sas uhu, király uhu) |
| **ultimó / ulti** | Figur mit dem **9.** (letzten) Stich |
| **volát** | alle neun Stiche |
| **XXI‑fogás / huszonegyfogás** | „XXI‑Fang": Der Skíz sticht den XXI eines Gegners |

Österreichische Entsprechungen für Umsteiger: *Sküs* = skíz, *Mond* = XXI, *Pagat* = pagát, *Trull* =
tulétroá, *Talon* = talon, *Vorhand* = induló, *Königrufen* ≈ húszashívás (dort wird jedoch ein **König**
gerufen, hier ein **Tarock**).

### 11.8 Drei Übungspartien (feste Kartenverteilungen)

Konventionen: Spieler **Süd (= du)**, **Ost**, **Nord**, **West**; gespielt wird **nach rechts**, die
Reihenfolge ist also `Süd → Ost → Nord → West`. Alle drei Partien sind mit der Regel‑Engine durchgerechnet
(Legalität jeder Karte, Stichgewinner, Punkte, Abrechnung).

---

#### 11.8.1 Übungspartie 1 – „Die erste Partie: Lizit, Ruf, Farb‑ und Tarockzwang"

**Lernziele:** Lizit mit honőr · Drücken · XX rufen · Farbzwang/Tarockzwang · Partner erkennen ·
Kartenpunkte zählen · warum der pagát entwischt.

**Geber:** West → **Vorhand (induló) ist Süd = du.**

**Blätter vor dem Talon**

| | Tarock | Farbe |
|---|---|---|
| **Süd (du)** | Skíz, XXI, XIX, XVI, XIV, X, VI | ♥K, ♣L |
| **Ost** | XVIII, XIII, IX, IIII, I | ♥D, ♠K, ♠D, ♦B |
| **Nord** | XX, XVII, XI, VII | ♦K, ♦D, ♠B, ♠L, ♣10 |
| **West** | XV, XII, VIII, V, III | ♣K, ♣D, ♥A, ♥B |

**Talon (von oben):** ♥L, ♦A, ♠10, II, ♣B, ♦L

**Schritt 1 – Lizit.**
Du sagst **`három`**. *Erklärung:* „Du hast zwei honőr (Skíz und XXI) – du **darfst** bieten. Mit sieben
Tarock ist dein Blatt stark; `három` holt dir drei Talonkarten. (Als einziger Bieter dürftest du später sogar
noch auf `kettő`/`egy`/`szóló` erhöhen – für die erste Partie bleiben wir bei `három`.)"
Ost, Nord, West passen. *Erklärung:* „Ost hat zwar den pagát (also einen honőr und damit das Recht zu
bieten), aber nur ein mittelmäßiges Blatt und passt. **Nord und West haben gar keinen honőr** – Nord hält
zwar den begehrten XX, aber weder Skíz noch XXI noch pagát, und West hat nur Mitteltarocke. **Beide müssen
passen.** Nord ist das sogar recht: Mit dem XX wird er ohnehin fast sicher als Partner **gerufen**."

**Schritt 2 – Talon.** Du bekommst **♥L, ♦A, ♠10**; Ost bekommt II, Nord ♣B, West ♦L.

**Schritt 3 – Drücken.** Du drückst **♣L, ♦A, ♠10**.
*Erklärung:* „Du wirfst drei Farben komplett weg (*színtelenítés*) – dann kannst du in Treff, Karo und Pik
**mit Tarock stechen**. ♥K behältst du mit zwei Beikarten (♥L, ♥B), so ist der König geschützt. Dein Skart
zählt am Ende **zu deinen Stichen** (5 Punkte)." Wenn du den ♥K oder den XXI antippst: Meldung `S_KING` bzw.
`S_HONOUR`.

**Schritt 4 – Ansagen.** Du sagst: **„Hívom a húszast – passz"** („Ich rufe den XX").
*Erklärung:* „Du hast den XX nicht, also **musst** du ihn rufen. Wer ihn hat, ist dein Partner – er verrät
sich nicht. Du könntest jetzt `tulétroá` ansagen (du hast **beide** großen honőr!). Wir lassen es weg und
schauen am Ende, ob es geklappt hätte."
Ost, Nord, West sagen „passz" → Spiel beginnt.

**Schritt 5 – Die neun Stiche** (dein Zug ist jeweils fett):

| # | Ausspiel | Süd | Ost | Nord | West | Stich an (Punkte) | Erklärung |
|---|---|---|---|---|---|---|---|
| 1 | Süd | **VI** | IIII | VII | XII | West (4) | „Du ziehst Tarock, aber **klein** – so verschwendest du keine hohen Tarocke. West sticht mit XII." |
| 2 | West | **X** | XIII | ♣B | ♣D | Ost (8) | „West spielt ♣D aus. Du hast **kein Treff mehr** → **Tarockzwang**: Du musst Tarock spielen, nimm den kleinen X. Ost übertarockiert mit XIII." |
| 3 | Ost | **XIV** | ♠D | ♠B | V | **Süd (8)** | „Ost spielt ♠D. Nord bedient mit ♠B, West hat kein Pik und muss tarockieren (V), du auch – dein XIV gewinnt." |
| 4 | Süd | **XVI** | II | XI | VIII | **Süd (4)** | „Du ziehst weiter Tarock und behältst die Führung." |
| 5 | Süd | **XIX** | XVIII | XVII | III | **Süd (4)** | „Mit dem XIX lockst du den XVIII heraus – jetzt sind die großen gegnerischen Tarocke weg." |
| 6 | Süd | **♥L** | ♥D | **XX** | ♥B | Nord (10) | „Du spielst Herz. Ost muss bedienen, **Nord hat kein Herz und sticht mit dem XX** – damit ist klar: **Nord ist dein Partner!**" |
| 7 | Nord | XXI | ♠K | ♠L | XV | **Süd (14)** | „Nord spielt ♠L, West sticht mit XV; du hast kein Pik → Tarockzwang. Der **XXI ist hier ungefährlich**, weil du den Skíz selbst hast – niemand kann ihn fangen." |
| 8 | Süd | **♥K** | **I (pagát)** | ♦D | ♥A | Ost (15) | „Ost hat kein Herz mehr und **muss Tarock spielen**; er hat noch IX und den pagát. Er wählt den **pagát** – und das ist richtig: Nord und West sind tarockfrei, und du selbst hast zwar noch den Skíz, aber du hast bereits ♥K zu diesem Stich ausgespielt und kannst nicht noch einmal legen. Also **gewinnt der pagát den Stich und entwischt**. (Mit IX hätte Ost den Stich zwar auch gewonnen, müsste dann aber den pagát zum 9. Stich ausspielen, wo dein Skíz ihn schluckt – **verlorenes stilles pagát ultimó, −5** für Ost und West.) Genau deshalb hätte `tulétroá` hier **verloren**!" |
| 9 | Ost | **Skíz** | IX | ♦K | ♣K | **Süd (16)** | „Der letzte Stich bringt zwei Könige – der Skíz holt ihn." |

**Schritt 6 – Abrechnung.** Deine Partei: 56 Stichpunkte + 5 Skart = **61**; Gegner 27 + 6 = **33**
(Summe 94 ✔). 61 ≥ 48 → **parti gewonnen**, Wert `három` = **1**.
**Ergebnis: Süd +1, Nord +1, Ost −1, West −1.**
*Schlussbemerkung:* „Hättest du `tulétroá` angesagt, hättest du 2 Punkte **verloren** (der pagát entwischte).
Hättest du im 8. Stich statt ♥K den Skíz gespielt, hätte Ost den pagát für den **letzten** Stich aufgehoben –
und dafür hättest du sogar 5 Punkte für ein **stilles pagát ultimó** der Gegner bezahlt."

---

#### 11.8.2 Übungspartie 2 – „engedés, tulétroá, kontra und rekontra"

**Lernziele:** Die Halte‑Pflicht und das **engedés** · Rufpflicht im engedés‑Spiel · tulétroá als Signal ·
kontra/rekontra · den pagát fangen.

**Geber:** Nord → **Vorhand ist West**; Reihenfolge `West → Süd → Ost → Nord`.

**Blätter vor dem Talon**

| | Tarock | Farbe |
|---|---|---|
| **West** | Skíz, XX, XV, XIV, XII, VIII | ♠K, ♣B, ♥A |
| **Süd (du)** | XXI, XVII, XI, VI | ♥K, ♥B, ♦D, ♣D, ♠L |
| **Ost** | XIII, IX, V, I | ♠D, ♠10, ♣10, ♦B, ♦A |
| **Nord** | XVIII, X, IIII, III, II | ♦K, ♦L, ♥D, ♣K |

**Talon (von oben):** XIX, ♥L, XVI, ♠B, ♣L, VII

**Schritt 1 – Lizit.**
West: `három`. **Du: `kettő`.** *Erklärung:* „Dein linker Nachbar hat `három` gesagt und du hast einen honőr
(XXI). Dann bietet man fast immer `kettő`: Entweder West **lässt dir das Spiel** (engedés) – dann hast du
einen Partner mit XX und großem honőr –, oder er bietet weiter, und du bekommst eine Talonkarte mehr."
Ost: `passz`, Nord: `passz` (kein honőr), **West: `passz`**.
*Erklärung:* „Achtung, das ist kein normales Passen! Nach `három`–`kettő` mit zwei Pässen heißt Wests Passen
**engedés**: Er verspricht den **XX**, einen großen honőr und mindestens fünf Tarock. **Du bist Spielmacher
mit `kettő` und musst den XX rufen.**"

**Schritt 2 – Talon.** Du: XIX, ♥L · Ost: XVI, ♠B · Nord: ♣L · West: VII.

**Schritt 3 – Drücken.** Du drückst **♣D, ♠L** (7 Punkte in deinen eigenen Skart);
Ost drückt ♦B, ♦A; Nord ♦L; West ♥A. Niemand drückt Tarock → keine Tarockmeldung.

**Schritt 4 – Ansagen.**
* **Du: „Hívom a húszast – tulétroá – passz."** *Erklärung:* „Du **musst** den XX rufen (engedés). Und du
  darfst `tulétroá` ansagen, weil du mit dem XXI einen **großen honőr** hast – im engedés‑Spiel heißt diese
  Ansage genau das: ‚Ich habe einen großen honőr.' Dein Partner weiß jetzt, dass ihr beide großen honőr habt
  – der pagát muss also gefangen werden."
* **Ost: „Kontra tulétroá – passz."** *Erklärung:* „Ost hat den pagát und hofft, ihn zu retten. Mit dem
  Kontra verrät er zugleich, dass er **gegen** dich spielt."
* Nord: „passz."
* **West: „Rekontra tulétroá – passz."** *Erklärung:* „Dein Partner hat Skíz **und** XX und sieben Tarock –
  er traut sich den Fang zu. Jetzt steht der tulétroá bei **2 × 4 = 8 Punkten**."
* Du, Ost, Nord: „passz" → Spiel.

**Schritt 5 – Die neun Stiche**

| # | Ausspiel | West | Süd | Ost | Nord | Stich an | Erklärung |
|---|---|---|---|---|---|---|---|
| 1 | West | VII | **VI** | V | X | Nord (4) | „Dein Partner zieht klein Tarock an; Nord nimmt mit X." |
| 2 | Nord | VIII | **♦D** | IX | ♦K | Ost (11) | „Nord spielt seinen **blanken ♦K** – ein einzelner König ist immer eine Last. West hat kein Karo und tarockiert, du musst bedienen, Ost übertarockiert." |
| 3 | Ost | ♠K | **XI** | ♠10 | II | **Süd (8)** | „Ost spielt klein Pik. Nord hat kein Pik (II), **West muss seinen ♠K zugeben** – auch Könige unterliegen dem Farbzwang. Dein XI holt den Stich." |
| 4 | Süd | XIV | **♥B** | XIII | ♥D | **West (8)** | „Du spielst eine kleine Herz: Ost hat kein Herz und muss tarockieren, dein Partner übertarockiert." |
| 5 | West | XII | **XVII** | XVI | XVIII | Nord (4) | „Hier nimmt Nord mit dem XVIII – auch Gegner gewinnen Stiche." |
| 6 | Nord | ♣B | **XIX** | ♣10 | ♣L | **Süd (7)** | „Du hast kein Treff → Tarockzwang; mit XIX sicherst du den Stich." |
| 7 | Süd | XV | **♥L** | **I (pagát)** | III | **West (10)** | „Jetzt der entscheidende Stich: Du spielst Herz, **Ost hat weder Herz noch einen anderen Tarock – er muss den pagát legen**. Nord versucht mit III zu retten, aber West sticht mit XV. **tulétroá ist erfüllt!**" |
| 8 | West | Skíz | **XXI** | ♠B | IIII | **West (13)** | „West zieht den Skíz; du musst deinen **XXI** zugeben (Tarockzwang) – das ist **kein** XXI‑Fang, weil ihr Partner seid." |
| 9 | West | XX | **♥K** | ♠D | ♣K | **West (15)** | „Der letzte Stich geht an euch." |

**Schritt 6 – Abrechnung.** Eure Partei: 61 + 7 Skart = **68**, Gegner: 19 + 7 = **26** (94 ✔).
* parti (`kettő`) = **+2**
* tulétroá angesagt, kontriert, rekontriert = 2 × 4 = **+8**
**Ergebnis: Süd +10, West +10, Ost −10, Nord −10.**

---

#### 11.8.3 Übungspartie 3 – „Tarockzahl, pagát ultimó, gebundene Karte und der XXI‑Fang"

**Lernziele:** Ruf des XIX, wenn man den XX selbst hat · Pflichtansage der Tarockzahl · pagát ultimó und die
**gebundene Karte** · der **XXI‑fogás** · stille Figuren.

**Geber:** West → **Vorhand ist Süd = du.**

**Blätter vor dem Talon**

| | Tarock | Farbe |
|---|---|---|
| **Süd (du)** | XX, XVIII, XVII, XV, V, I | ♠K, ♠B, ♣10 |
| **Ost** | XXI, XII, VIII, III | ♥K, ♣L, ♠D, ♠L, ♦A |
| **Nord** | Skíz, XIX, XIV, IIII | ♥A, ♥B, ♥D, ♦K, ♣K |
| **West** | XI, X, VI, II | ♦D, ♦B, ♣B, ♣D, ♠10 |

**Talon (von oben):** IX, XIII, XVI, ♥L, VII, ♦L

**Schritt 1 – Lizit.** Du: `három`. Ost: `passz`. Nord: `kettő`. West: `passz`. **Du: `tartom`.** Nord:
`passz`.
*Erklärungen:* „Du hast den pagát (ein honőr) und sechs Tarock – du darfst und solltest bieten. · Ost hat den
**XXI mit nur vier Tarock** – ein ‚schwacher XXI'. Er passt in der Hoffnung, nicht aufzufallen; die
ungarische Schule sagt: **Das ist ein Fehler** – gleich sehen wir, warum. · Nord bietet mit dem Skíz `kettő`.
· Du hältst (`tartom`). **Passen dürftest du hier nicht als engedés** – dafür bräuchtest du einen **großen**
honőr; du hast nur den pagát."

**Schritt 2 – Talon.** Du: IX, XIII (jetzt **acht Tarock**!) · Ost: XVI, ♥L · Nord: VII · West: ♦L.

**Schritt 3 – Drücken.** Du drückst **♠B, ♣10**.
*Erklärung:* „Du behältst ♠K (Könige darf man nicht drücken) und wirfst den Rest der Farben weg – mit acht
Tarock willst du **jede** Farbe stechen können."

**Schritt 4 – Ansagen.**
**Du: „Nyolc tarokk – hívom a tizenkilencest – pagátultimó – passz."**
*Erklärungen:*
* „**nyolc tarokk**: Du hast genau acht Tarock. Diese Ansage ist hier **Pflicht**, weil du gleich
  `pagát ultimó` ansagst (§5.11). Sie bringt dir 1 Punkt von **jedem** Mitspieler."
* „**Ruf:** Du hast den XX selbst – dann rufst du den **höchsten Tarock, der dir fehlt**, also den **XIX**.
  (Du dürftest auch deinen eigenen XX rufen – dann spielst du **allein gegen drei**.)"
* „**pagátultimó:** Du versprichst, den **letzten Stich mit dem pagát** zu gewinnen. Ab jetzt ist der pagát
  eine **gebundene Karte**: Du darfst ihn nicht spielen, solange dir eine andere Karte erlaubt ist."
Ost, Nord, West: „passz".

**Schritt 5 – Die neun Stiche**

| # | Ausspiel | Süd | Ost | Nord | West | Stich an | Erklärung |
|---|---|---|---|---|---|---|---|
| 1 | Süd | **♠K** | ♠D | IIII | ♠10 | Nord (11) | „Den blanken König gleich spielen, solange die anderen noch Pik bedienen müssen. Nord hat kein Pik und sticht mit seinem kleinsten Tarock – dass er (als Besitzer des XIX) dein Partner ist, weißt du in diesem Moment noch nicht." |
| 2 | Nord | **V** | III | VII | X | West (4) | „Tarock wurde angespielt: Du **musst** Tarock zugeben. Tippst du den pagát an, kommt die Meldung: *Der pagát ist bis zum 9. Stich gebunden.*" |
| 3 | West | **XIII** | XVI | ♦K | ♦D | Ost (11) | „West spielt Karo; du hast keins → Tarockzwang; Ost übertarockiert mit XVI, Nord muss seinen ♦K zugeben." |
| 4 | Ost | **IX** | VIII | XIV | VI | Nord (4) | „Ost zieht Tarock – gut für dich: Jeder Tarockstich bringt dein Ultimó näher." |
| 5 | Nord | **XV** | XII | XIX | XI | Nord (4) | „Jetzt ist Ost bis auf den **XXI** tarockleer und West bis auf den **II**." |
| 6 | Nord | **XVII** | ♣L | ♣K | ♣B | **Süd (11)** | „Nord spielt seinen ♣K; du hast kein Treff → Tarockzwang und nimmst den Stich." |
| 7 | Süd | **XVIII** | **XXI** | **Skíz** | II | Nord (12) | „**Der Fang!** Du spielst Tarock; Ost hat nur noch den **XXI** und muss ihn legen; dein Partner sitzt **nach** ihm und sticht mit dem **Skíz**: **XXI‑fogás, 21 Punkte still** – Ost ist **polgármester** 🎩." |
| 8 | Nord | **XX** | ♥L | ♥D | ♦B | **Süd (10)** | „Nord spielt Herz; du hast keins → du musst Tarock spielen. Der pagát ist gebunden, also kommt der **XX**." |
| 9 | Süd | **I (pagát)** | ♥K | ♥B | ♦L | **Süd (15)** | „Niemand hat mehr einen Tarock – **der pagát gewinnt den letzten Stich: pagát ultimó erfüllt!**" |

**Schritt 6 – Abrechnung.** Eure Partei: 67 + 3 Skart = **70**; Gegner: 15 + 9 = **24** (94 ✔).

| Posten | Rechnung | Betrag |
|---|---|---|
| parti (`kettő`, 70 ≥ 48) | 1 × g = 1 × 2 | **+2** |
| pagát ultimó, angesagt | | **+10** |
| XXI‑fogás, still | 42 / 2 | **+21** |
| tulétroá, still (Skíz + XXI + pagát alle in euren Stichen) | 2 / 2 | **+1** |
| **Summe je Spieler der Spielmacher‑Partei** | | **+34** |
| tarokkszám „nyolc tarokk" | 1 von jedem der drei anderen an Süd | Süd +3, andere je −1 |

**Ergebnis: Süd +37, Nord +33, Ost −35, West −35** (Summe 0 ✔).

*Schlussbemerkungen für den Lernmodus:*
* „**négykirály** gab es nicht: Der ♦K landete im 3. Stich bei den Gegnern."
* „Hättest du den XXI‑Fang **angesagt**, wären es 42 statt 21 Punkte gewesen – aber wenn Ost den XXI
  gerettet hätte, hättest du 42 **bezahlt**. Deshalb sagt man ihn nur nach klarer Einladung des Partners an."
* „Ost hätte mit dem schwachen XXI besser selbst geboten: Mit zwei Talonkarten hätte er sich eine leere Farbe
  bauen können, auf der der XXI entkommt."

---

## 12. Profil „Paskievics" (XX‑hívás, 6 Figuren) – Unterschiede

Paskievics ist das **Grundspiel**; Illusztrált ist Paskievics + 6 (bzw. mit sas + 8) Figuren. Alles aus §1–§4
und §6–§8 gilt unverändert. Unterschiede:

| Punkt | Paskievics | Illusztrált (Default) |
|---|---|---|
| **Figurenliste** | nur **tulétroá, négykirály, duplajáték, volát, pagátultimó, XXI‑fogás** [PAG-P §Bonuses], [PVSZ18 4] | + centrum, kismadár, nagymadár, pagát uhu, király uhu, király ultimó (+ sas ultimó/uhu) |
| **gebundene Karten** | nur der pagát bei angesagtem pagát ultimó | alle Figuren aus §5.7/§5.6 |
| **Invit** | **mit jedem honőr** erlaubt; **mit dem pagát ist `pagát ultimó` Pflichtansage** [PVSZ18 5.2.1/5.2.2], [KEGL] | nur mit großem honőr + ≥ 5 Tarock [ITVB 4.3/4.7] |
| **engedés** | „A XX‑as pagátos a játékot nem engedheti" – wer XX **und** pagát hält, darf nicht zulassen [PVSZ18 5.1.3/5.2.8] | keine entsprechende Sonderregel |
| **tarokkszám** | Parteiprämie: „mely az azt bemondó játékosnak **és a partnerének** jár" [PVSZ18 4.1]; Pflicht **nur** bei pagát ultimó und dessen Kontra [PVSZ18 4.1.3] | Zahlung von allen drei (Default, §5.11); Pflicht auch bei pagát uhu, sas ulti/uhu [ITVB 7.3] |
| **tulétroá, 2. Runde** | bedeutet mindestens **einen** großen honőr [PVSZ18 4.2.4] | [ITVB] regelt nur die 1. Runde |
| **dupla/volát vs. parti** | „bemondása esetén a parti semmilyen körülmény esetén sem számítandó" – bei angesagter dupla/volát wird die parti **für die ansagende Partei** gar nicht gewertet; **die Gegenpartei schreibt sie aber sehr wohl**: „Elvesztése esetén, ha az ellenfél partit is csinál, úgy számára **mindkettő** pontot jelent" [PVSZ18 4.5/4.6] | Algorithmus §7.2 (kontriertes Spiel zählt immer) |
| **próbahárom‑Strafe** | Wert eines `egy`‑Spiels = **3 Punkte** an jeden der drei Mitspieler [PVSZ18 8.2] | Spielwert (Default 1 bei `három`), §9.15 |
| **Einwurf** | dieselben Blätter (blanker pagát/XXI, beide, vier Könige, kein Tarock), aber Zeitfenster nur „**fektetés előtt**" ohne den ITVB‑Zusatz „talon felvétel után" [PVSZ18 6.2] | [ITVB 8.2]: „csak talon felvétel után, fektetés előtt" |
| **Skart‑Korrektur** | „A fektetett lap **az első bemondás előtt módosítható**, utána viszont meg sem nézhető" [PVSZ18 6.7] | in §4.3 als App‑Regel übernommen |
| **renonsz** | 5–10 Punkte je Mitspieler (bei zwei Schuldigen 10–20) [PVSZ18 10.7] | 5–20 (bzw. 10–40) [ITVB 9.9] |
| **Spielkultur** | „Hauptaufgabe ist der XXI‑Fang" [kartya-jatek.hu]; [KEGL] hält Paskievics für die **anspruchsvollere** Variante | mehr Ansagen, mehr Varianz |
| **Sonderregel** [KEGL] | Bei `három`, wenn nur der Spielmacher tulétroá ansagt und sonst nichts kommt, wird **nicht gespielt**; die Gegner zahlen 3 | [ITVB 8.1]: **jede** Partie wird gespielt |

Die App bietet Paskievics als eigenes Profil an (`HU-PASK-ITVB`); im Lernmodus ist es der empfohlene
**Einstiegsmodus**: [PAG-I §Introduction] empfiehlt, zuerst Paskievics zu lernen. [KEGL] hält die verbreitete
Formulierung „erst Paskievics, dann das ‾richtige‹ (illusztrált) Spiel" für grundfalsch – nicht weil
Paskievics zu einfach wäre, sondern weil er Illusztrált für die **schwächere**, glücksabhängigere Variante
hält. Für die App heißt das: Paskievics ist ein vollwertiges Profil, kein „Kinderspiel".

---

## 13. Quellenverzeichnis (Abruf: 11.–12.09.2026)

**Ungarische Primär‑/Verbandsquellen**
1. **[ITVB]** Országos Nyílt Illusztrált Tarokk Kupák versenyszabályzata / Etikai kódex, Verzió 4.0, hatályos
   2019.01.01. – <https://tarokk.hu/T_tajekoztatok/Illusztralt_versenyszabalyzat.pdf>
   (ältere Fassungen: `…_2016.pdf`, `…_2017_1_modositas.pdf`, `…_2018_2_modositas.pdf`,
   `…_2019_3_modositas.pdf`)
2. **[BTE]** Budapesti Tarokk Egyesület Játékszabályzata, Etikai Kódex, hatályos 2016.01.01. –
   <https://tarokk.hu/T_dokumentumok/BTE%20J%C3%A1t%C3%A9kszab%C3%A1lyzat.pdf>
3. **[PVSZ18]** Paskievics tarokkversenyeken alkalmazandó versenyszabályok, hatályos 2018.03.01. –
   <https://tarokk.hu/T_tajekoztatok/Pasvievics_versenyszabalyzat_2018_mod2c.pdf>
4. **[TA48]** Fazekas Béla – Kovács Endre – Szigetvári Zoltán: *Tarokkakadémia* II. könyv, „Az illusztrált
   tarokkjáték szabályainak részletes ismertetése és magyarázata", Centrum, Budapest 1948 (Digitalisat) –
   <https://tarokk.hu/T_dokumentumok/TAROKK_akademia.pdf>
   (zitierte Stellen: S. 12–15 Figurenübersicht, S. 16–23 Lizit, S. 24–29 Skart, S. 29–30 Ansagereihenfolge,
   S. 50–53 uhu/király ultimó, S. 54–56 XXI‑Fang, S. 57–60 Ausspiel, **S. 61 Számítási táblázat**,
   S. 62–64 renonsz)
   **⚠ Verifikationsstatus:** Die Datei (77 Seiten, ~10 MB) ist mit einem Owner‑Passwort geschützt und
   enthält **nur Bilddaten, keine Textebene** – Seitenzitate ließen sich nicht gegenprüfen. Für die
   Spezifikation ist das unkritisch, weil jede materielle TA48‑Aussage zusätzlich durch [ITVB], [BTE],
   [MKVM] oder [PAG-I] gedeckt ist; die verbliebenen reinen TA48‑Zitate sind im Text als *unbestätigt*
   markiert.
5. **[TAOR]** Kovács Endre – Szigetvári Zoltán: *Tarokk‑őr* (1940) – <https://tarokk.hu/T_dokumentumok/TAROKK_or.pdf>
   (nicht im Detail ausgewertet; Paskievics‑Klassiker)
6. **[MKVM]** „Az illusztrált tarokk játékszabályai", Magyar Kereskedelmi és Vendéglátóipari Múzeum –
   <https://mkvm.hu/az-illusztralt-tarokk-jatekszabalyai/> · Textquelle laut Fußnote:
   <https://www.kartya-jatek.hu/illusztralt_tarokk/> und <https://www.kartya-jatek.hu/tarokk_valtozatok/>
7. **[HUWIKI]** „Tarokk", ungarische Wikipedia – <https://hu.wikipedia.org/wiki/Tarokk>
8. Tarokk Regiszter (Turnierportal, Ergebnisse, ITVB‑Protokolle) – <https://tarokk.hu/> ,
   <https://tarokk.hu/T_itvb.php>

**Internationale Darstellungen**
9. **[PAG-I]** John McLeod: *Illustrated Hungarian Tarokk* – <https://www.pagat.com/tarot/illusztr.html>
   (ungarische Fassung: <https://www.pagat.com/tarot/illu_hu.html>)
10. **[PAG-P]** John McLeod: *Hungarian Tarokk* (Paskievics / XX‑hívás) –
    <https://www.pagat.com/tarot/xx-hivas.html>
11. **[PAG-EX]** *Illustrated Hungarian Tarokk: Example Deals* (Révész Gábor) –
    <https://www.pagat.com/tarot/ihtex.html> (Einzelblätter `t01e.html` … `t13e.html`)
12. **[SURVEY]** Zsigri Gyula: *Tarokk Survey* (1997) – <https://www.pagat.com/tarot/tarokk-survey.html>
13. **[KEGL]** Kégl Lóránt: *Tarokk page for advanced players* – <https://tarokk.lorantkegl.eu/tarokk_en.htm> ·
    *Standardization of the Rules of Paskievics Tarokk* – <https://tarokk.lorantkegl.eu/tarokkstandard_en.htm> ·
    Konventionsübersicht (PDF) – <https://lorantkegl.eu/tarokk_basic.pdf>
14. **[UZO]** Uzonyi Ákos: *Tarokk* (Android, `com.tisza.tarock`), Quellcode **GPL‑3.0** –
    <https://github.com/AkosUzonyi/Tarock> · Play Store:
    <https://play.google.com/store/apps/details?id=com.tisza.tarock>
    **→ nur als Gegenprobe gelesen, kein Code‑Übernahme (Lizenzinkompatibilität).**
15. Board Game Arena, *Hungarian Tarokk* (Implementierung nach [KEGL]) –
    <https://boardgamearena.com/gamepanel?game=hungariantarokk> (Regelabweichungen sind in [PAG-P §Other
    Hungarian Tarokk WWW pages] aufgelistet)

**Ergänzend (nicht regelbildend)**
16. en.wikipedia.org, *Hungarian Tarokk* – <https://en.wikipedia.org/wiki/Hungarian_Tarokk>
17. „MIÉRT RONTANÁNK EL A JÁTÉKOT?" (ITVB‑Diskussionsbeitrag zu Kontra‑Sitten) –
    <https://tarokk.hu/T_tajekoztatok/MI%C3%89RT_RONTAN%C3%81NK_EL_A_J%C3%81T%C3%89KOT.pdf>

**Nicht verwendet / bewusst verworfen:** ältere Bücher mit abweichenden Tarifen (Kovács: négykirály 2/4,
pagátulti 6/12, XXI‑fogás 30/60 – vgl. [PAG-P §Variations in the scoring]); die Werte der ungarischen
Wikipedia (XXI‑fogás 60, király uhu 15); *Magas tarokk* und *Royal tarokk* (eigene Spiele).

---

## 14. Offene Punkte / Entscheidungen, die noch zu treffen sind

1. **Neuere ITVB‑Fassung?** Auf tarokk.hu ist Version 4.0 (2019) die letzte veröffentlichte; ob die ITVB seit
   2020 gedruckte Änderungen beschlossen hat, ließ sich online nicht belegen (Protokolle bis 2021 vorhanden,
   ohne Regeländerung). → Vor Release ggf. bei ITVB/BTE nachfragen.
2. **tarokkszám‑Zahlung:** „von allen drei" ([PAG-I], App‑Default) **vs.** Parteiprämie ([PVSZ18 4.1]);
   [ITVB], [BTE] und [MKVM] schweigen zur Zahlungsrichtung.
   Betrifft nur 2‑gegen‑2‑Partien; Entscheidung des Nutzers erwünscht (Flag ist implementiert).
3. **pagát uhu / sas uhu 15 oder 20** (`uhu_value`) – Default 15 ([ITVB]/[BTE]/[MKVM]/[HUWIKI]); wer nach
   pagat.com spielt, stellt 20 ein. Der **király uhu** bleibt in beiden Fällen bei 20.
4. **Skart bei „hivatalból kontra":** [ITVB]/[MKVM] geben alle sechs Karten dem Alleinspieler, das
   pagat‑Beispieldeal rechnet anders. Default = ITVB; bestätigen.
5. **Konventionszwang** (tulétroá‑Bedeutungen, Invit‑Voraussetzungen): im Turnierprofil bindend, im
   Freundesspiel evtl. lästig → Default „an", Umschalter im Optionsmenü. Nutzerwunsch?
6. **Verdopplungsrunden / ultimó‑Kasse** nach Einwurf: Default „keine Folgen". Soll die „ultimó‑Kasse"
   ([KEGL]) als Option implementiert werden (sie verändert die optimale Strategie)?
7. **sas ultimó/uhu** standardmäßig an (ITVB/MKVM) – oder aus, um Illusztrált „klassisch" (12 Figuren) zu
   halten?
8. **Kartenbilder:** Für das 42‑Blatt werden aus dem 54‑Blatt‑Satz die 2/3/4 rot und 7/8/9 schwarz
   ausgeblendet – die Lizenz‑/Artwork‑Frage ist im Recherchebericht (`tarock-research.json`) behandelt und
   hier nicht Gegenstand.
9. **Ungarische Lokalisierung:** Die Engine sollte Ansagen als Enum führen und sowohl deutsche als auch
   ungarische Bezeichner rendern („Hívom a húszast" / „Ich rufe den XX") – die Terminologie oben ist dafür
   bereits vollständig.
10. **Turnierbetrieb:** Soll der Turniermodus die ITVB‑Tischpunkte und die XXI‑Fang‑Statistik exportieren
    (CSV/Tarokk‑Regiszter‑kompatibel)?
11. **Wertung der tarokkszám bei fünf Spielern:** [PAG-I] sagt „paid by each of the other **three active**
    players" – der aussetzende Geber zahlt also nicht. Die App setzt das so um; im Profil `team_item`
    entfällt die Frage ohnehin.
12. **`uhu_value` gilt für pagát uhu *und* sas uhu** (Flagname korrigiert, §5.3). Der **király uhu** bleibt
    in jedem Fall bei 20.

---

## 15. Prüfprotokoll (unabhängige Regelprüfung)

Geprüft am **12.09.2026** durch Abruf und Volltextauswertung der Primärquellen. Die Prüfung umfasste jede
Zahl, jedes Zitat, jede Quellenzuordnung und alle drei Übungspartien (letztere Karte für Karte nachgespielt).

### 15.1 Was tatsächlich geprüft wurde

| Quelle | Zustand | Ergebnis |
|---|---|---|
| **[ITVB]** `Illusztralt_versenyszabalyzat.pdf` (13 S., Version 4.0, gültig ab 01.01.2019) | Textebene vorhanden, vollständig gelesen | alle Ziffern‑Zitate §§1–9 verifiziert |
| **[BTE]** `BTE Játékszabályzat.pdf` (11 S., ab 01.01.2016) | vollständig gelesen | verifiziert; Abweichungen zu ITVB dokumentiert (§5.2, §5.5, §8.1) |
| **[PVSZ18]** `Pasvievics_versenyszabalyzat_2018_mod2c.pdf` (4 S.) | vollständig gelesen | verifiziert; §12 präzisiert |
| **[MKVM]** mkvm.hu | vollständig gelesen | verifiziert; Wertetabelle, Talonverteilung, Lizit‑Beispiele |
| **[PAG-I]**, **[PAG-P]**, **[PAG-EX] 1 + 12**, **[SURVEY]** | vollständig gelesen | verifiziert; Multiplikatortabelle Feld für Feld |
| **[KEGL]** `tarokk_en.htm`, `tarokkstandard_en.htm`, `tarokk_basic.pdf` | vollständig gelesen | verifiziert |
| **[HUWIKI]** | vollständig gelesen | verifiziert (inkl. der abweichenden Werte) |
| **[TA48]** `TAROKK_akademia.pdf` | **passwortgeschützter Bild‑Scan, 77 Seiten, keine Textebene** | **nicht prüfbar** – siehe §15.4 |

Der Algorithmus aus §7.2 wurde gegen **alle 48 Felder** der Multiplikatortabelle §7.3 und gegen die
Ergebniszeilen von [PAG-EX] 1 und 12 nachgerechnet: **vollständig konsistent**, keine Korrektur nötig.
Die drei Übungspartien §11.8 wurden auf Kartenvollständigkeit (42 verschiedene Karten), Legalität jeder
einzelnen Karte (Farb-/Tarockzwang, Bindungen), Stichgewinner, Kartenpunkte und Abrechnung nachgerechnet:
**alle Zahlen stimmen**; zwei *Erklärungstexte* waren falsch (§15.2 Nr. 22/23).

### 15.2 Korrigierte Fehler

| # | Stelle | Was falsch war | Korrektur |
|---|---|---|---|
| 1 | §5.5, §9.20, §10.1 | [ITVB 7.9] (négykirály = höchster unbekannter Tarock) und die „≥ 5 Tarock ⇒ tulétroá"-Sätze wurden als **Regeln** behandelt und vom Flag `enforce_trull_conventions` erzwungen | **[ITVB 9.1]**: „…megszegése renonsz, **kivéve az erős ajánlás minősítésűeket**". 7.9 ist ausdrücklich *erős ajánlás* ⇒ nie erzwingen; §5.5 trennt jetzt Regel/Empfehlung spaltenweise, neue Hinweismeldung `A_FOURKINGS_HINT` |
| 2 | §4.6 | „[PAG-P] beschreibt als **Mehrheitsvariante** eine Verdopplungsrunde" | Falsch herum: bei [PAG-I]/[PAG-P] ist die Verdopplungsrunde die **Hauptregel** (inkl. überlappender Vervierfachung); das Weglassen steht unter §Variations. Absatz neu geschrieben, drei Spielarten + [KEGL]s ultimó‑Kasse dokumentiert |
| 3 | §4.5 Nr. 4 | „jeden beliebigen Tarock **außer einem honőr** rufen [ITVB 5.7]" | [ITVB 5.7]/[BTE 4.6]/[PVSZ18 6.5] sagen nur „**bármilyen tarokkot**". Die honőr‑Ausnahme stammt allein von [PAG-I]/[PAG-EX 12]. Attribution korrigiert, eigenes Flag `free_call_excludes_honours` |
| 4 | §5.12 | vier Kontrastufen mit [MKVM] belegt | [MKVM] kennt nur **drei** (kontra/rekontra/szubkontra, ×2/×4/×8); hirskontra kommt aus [ITVB 6]/[BTE 5]/[PVSZ18 4.8], mordkontra aus [PAG-I]. Flag um Stufe 3 erweitert |
| 5 | §5.3 | Flag hieß `pagat_uhu_value` und erfasste den **sas uhu** nicht | umbenannt in `uhu_value`; [PAG-I] setzt *beide* Tarock‑uhu auf 20 und nennt 15 als verbreitete Alternative |
| 6 | §1.7 | „Nem engedünk a 48‑ból!" mit [PAG-P §Introduction] belegt | Der Satz steht dort **nicht**; Beleg ist [HUWIKI §Története] |
| 7 | §3.2 | Honőr‑Pflicht im Lizit mit „[ITVB 4.1 „A honőrök egyenrangúak"]" belegt | [ITVB 4.1] sagt nur, dass die honőrök **gleichrangig** sind; die Grundregel selbst steht in [MKVM]/[PAG-P]/[PAG-I] und gehört zur Hintergrundnorm ([ITVB 1.3]) |
| 8 | §4.6 | Strafe für misslungene `próbahárom` nur mit [PAG-I] | vier verschiedene Tarife ergänzt: [PAG-I] Spielwert, [PVSZ18 8.2] 3 Punkte, [KEGL] 2 Einheiten, **[MKVM] straffrei** |
| 9 | §4.4 | Ablageort des Skarts mit [ITVB 5.2] belegt | [ITVB 5.2] sagt „**az osztó és az induló között**" und regelt die Ablege*reihenfolge*; „rechte Hand des Gebers" steht bei [MKVM]/[HUWIKI], „vor dem Geber bei fünf Spielern" bei [PAG-I] |
| 10 | §5.2 | „Partei des zuletzt **Sprechenden**" ohne Definition | präzisiert: der zuletzt eine **Figur/ein Kontra** (bzw. im Default‑Profil eine tarokkszám) Ansagende; ein bloßes „passz" zählt nicht ([PAG-I], [MKVM], [PAG-P]) |
| 11 | §5.2 | Ausnahmenliste [ITVB 7.2] ohne Hinweis auf [BTE] | [BTE 6.2] kennt **nur** die centrum‑Ausnahme; neues Flag `attribution_exceptions` |
| 12 | §5.5 | Verbot „nach volát kein tulétroá/négykirály/dupla" auch [PAG-I] zugeschrieben | [PAG-I] verbietet nach volát **nur die duplajáték** |
| 13 | §5.5 | „ultimó → uhu" geregelt, die Gegenrichtung fehlte | **[PAG-I]**: nach einem angesagten uhu ist ein ultimó auf dieselbe Karte verboten. Neue Zeile + Meldung `A_UHU_TO_ULTI` + §9.6b |
| 14 | §5.5 | „dupla + volát im selben Zug" ohne Mehrheitsbild | ergänzt: [PAG-I] und [KEGL] verbieten es, **[SURVEY V] 7 : 3 : 1** dagegen |
| 15 | §5.4 | „stille tulétroá/négykirály entfallen bei volát" ohne Hinweis auf die abweichenden Formulierungen | [PAG-I] „wins all the tricks" (also auch angesagter volát), [MKVM]/[HUWIKI]/[KEGL] „**csendes** volát" und zusätzlich stille dupla. Flag `volat_kills_silent_bonuses` |
| 16 | §5.6 | Freigabe‑Sonderfall (centrum fällt durch Zwangsabwurf, madarak leben weiter) mit „[TA48 S. 58]" belegt | Das ist das **Originalbeispiel in [ITVB 8.5]/[BTE 7.4]**; ungarischer Wortlaut ergänzt |
| 17 | §6.4 | Nr. 4 („Freigabe") und Nr. 5 (pagát bleibt gebunden) wirkten widersprüchlich | aufgelöst: [ITVB 8.5]/[BTE 7.4] geben jede Karte nach dem Fallen der Figur frei, [PAG-I] formuliert die Freigabe nur für centrum/madarak; praktisch deckungsgleich. Neue Nr. 7: Bindung gilt für die **Partei** |
| 18 | §6.7 | renonsz‑Tarife unvollständig/teils aus [TA48] | vollständige Tarife aus [ITVB 9.2/9.5/9.8/9.9], [BTE 8.2/8.3/8.6/8.7], [PVSZ18 10.7], [MKVM] |
| 19 | §7.5 (d) | „−2 je D‑Spieler" ohne Hinweis, dass das den pagat‑Wert 20 für pagát uhu voraussetzt | ergänzt: mit App‑Default 15 lautet das Ergebnis **−7**; außerdem D/O als Parteikürzel klargestellt (in [PAG-EX] 1 ist **B** Spielmacher, **A** sagt volát an) |
| 20 | §7.5 (e) / §9.3 | Behauptung über [PAG-EX] 12 war unbelegt | **nachgerechnet**: 56 = 52 (Stiche 1, 3, 5, 8, 9) + 4 (eigener Skart) ⇒ [PAG-EX] gibt dem Alleinspieler **nicht** alle sechs Skartkarten. Behauptung bestätigt und belegt |
| 21 | §8.1 | „16/20 Partien pro Runde" als Sollzahl | [ITVB 2.7] schreibt „**legfeljebb**" (höchstens) innerhalb des Zeitlimits; abweichende Tischpunkte in [BTE 2.2]/[PVSZ18 3.2] ergänzt |
| 22 | §11.8.1 Schritt 1 | „**Nord hat XX und XXI**, aber nur vier Tarock – er wartet lieber darauf, gerufen zu werden" | **Sachfehler:** Nord hält XX, XVII, XI, VII – also **keinen einzigen honőr**; den XXI hat Süd. Nord **muss** passen (§3.2). Text neu geschrieben |
| 23 | §11.8.1 Stich 8 | „Ost … **sein einziger Tarock ist der pagát**. Weil **niemand mehr Tarock hat**, gewinnt der pagát" | **Zwei Sachfehler:** Ost hält noch IX **und** I; Süd hält noch den Skíz (kann aber nicht mehr legen, weil er ♥K ausgespielt hat). Erklärung neu geschrieben – samt Begründung, warum Ost den pagát **wählt** (mit IX müsste er den pagát zum 9. Stich vorlegen und verlöre 5 für ein misslungenes stilles pagát ultimó) |
| 24 | §9.22 | „[HUWIKI]‑Werte … sind **nicht** repräsentativ" pauschal | differenziert: XXI‑fogás 60 und király uhu 15 sind Ausreißer, die Aussage „die sechs Illusztrált‑Figuren zählen nur angesagt" ist dagegen **korrekt** |
| 25 | §4.2 | Einwurf nach Tarockdrücken nicht behandelt | neues Flag `annul_after_tarokk_discard`; [SURVEY IV.2] 9 : 2 dagegen, [PAG-I]/[KEGL] dagegen, ⚠ **[MKVM] empfiehlt ausdrücklich dafür** |
| 26 | §4.3 | Korrekturfenster für den Skart fehlte | **[PVSZ18 6.7]**: bis zur ersten Ansage änderbar, danach nicht einmal ansehbar ⇒ Undo‑Fenster der App |
| 27 | §3.5 | Invit‑Regeln unvollständig | ergänzt: pagát genügt nicht ([PAG-I], [ITVB 4.3], [MKVM]), zweiter Sprung erlaubt aber bedeutungsfrei ([PAG-I]), Annahme nicht verpflichtend ([MKVM]) |
| 28 | §5.11 | „nicht kontrierbar" wie ein Zitat formuliert | als **Auslegung** gekennzeichnet, Begründung und neuer §9.30 |
| 29 | §5.11 | Pflichtumfang der tarokkszám | [PAG-I] fordert sie auch bei **király** ulti/uhu und kennt die Partner‑Ausnahme nicht; [PVSZ18 4.1.3] nur beim pagát ultimó |
| 30 | §2.5, §4.1 | 5‑Spieler‑Regel nur mit [ITVB 5.1]/[KEGL] belegt | **[PVSZ18 6.6]** ergänzt („nem nézheti meg, **nem nyúlhat hozzá**") – die Nutzervorgabe deckt sich mit zwei Verbandsregelwerken |

Neu aufgenommene Streitpunkte in §9: **6b** (uhu → ultimó), **13b** (Einwurf nach Tarockdrücken),
**29** (ITVB 7.4 vs. 7.14), **30** (Kontra auf tarokkszám), **31** (zweiter Sprung), **32** (weitere
Konventionsvarianten).

### 15.3 Ausdrücklich geprüft und **unverändert bestätigt**

* **§1** Deck, Rangfolgen (rot/schwarz), Kartenwerte, 94 Punkte, Additionszählung, Skartzuordnung,
  48/47/71/volát – [MKVM], [PAG-P], [ITVB 7.11], [HUWIKI] wörtlich übereinstimmend.
* **§1.6** Zählbeispiel: nachgerechnet, 61 + 33 = 94 ✔.
* **§2** Geberichtung („jobbra tartás"), 6er‑Talon dann 5 + 4, Abheben links, induló = rechts vom Geber,
  Platzwahl‑Zeremonie, Geberrotation nach rechts, Fünfertisch‑Aussetzen.
* **§3.1/3.3/3.4** Vertragswerte 1/2/3/4, Talonkartenzahl als Gebotsinhalt, „tartom" nur einmal, Passen
  endgültig; das Beispiel in §3.4 ist wörtlich [PAG-P §The bidding, Beispiel 2].
* **§3.6** alle fünf Lizitfolgen – insbesondere, dass ein Sprung immer relativ zum **minimal möglichen**
  Gebot gezählt wird (bestätigt durch [MKVM]s Beispieltabellen und [SURVEY II.b]).
* **§4.1** Talonverteilungstabelle – identisch in [MKVM], [PAG-I], [PAG-P].
* **§4.2** die fünf Einwurfblätter und das Zeitfenster [ITVB 8.2]/[BTE 7.1]; [SURVEY IV.1]‑Zahlen
  (11/10/10/9/6 von 11) stimmen exakt.
* **§5.3** die komplette Wertetabelle gegen [ITVB 6] **und** [BTE 5] **und** [MKVM] – inkl.
  „Csendben teljesíthető"-Spalte; pagátfogás 8/4.
* **§7.2/7.3** Algorithmus und Multiplikatortabelle (alle 48 Felder nachgerechnet).
* **§8.1** Turnierparameter, XXI‑Fang‑Wertung (10/−10/5/−5, ×5 bei Ansage und „Fang von hinten").
* **§9** Nrn. 1, 5, 8, 9, 10, 11, 12, 16, 18, 19, 21, 23, 24, 27 – Quellenlage in jedem Punkt zutreffend.
* **§10** Prioritätenliste („Catch or save the XXI …") wörtlich [PAG-P §Advice on tactics]; Blattstärke
  („average hand has 5 tarokks") wörtlich; Skart‑Prioritäten und die drei Sprichwörter wörtlich
  [KEGL *tarokk_basic.pdf* §Exchange]; Fangkonstellations‑Gebote (Skíz ≥ 4 ⇒ `egy`, ≥ 7 ⇒ `szóló`; XXI
  hält `egy` mit ≤ 6, `szóló` mit ≤ 4–5) wörtlich [KEGL]; das X/IX‑Fangsignal steht in
  [KEGL *tarokk_basic.pdf*] (Kontext präzisiert: nur nach dem Ausspiel eines **hohen** Tarocks).
* **§11.8.2** und **§11.8.3** vollständig nachgespielt: Kartenverteilung, jede erzwungene Karte, alle
  Stichgewinner, 68 : 26 bzw. 70 : 24, Abrechnungen +10/+10/−10/−10 bzw. +37/+33/−35/−35 (Summe 0) – **korrekt**.
* **§12** alle [PVSZ18]‑Zitate; **§2.2** [KEGL]s Wiederauslosungsrecht nach zwei erlittenen XXI‑Fängen.

### 15.4 Was unsicher bleibt

1. **[TA48] ist nicht verifizierbar.** Das Digitalisat ist passwortgeschützt und enthält nur Bilddaten.
   Alle im Text verbliebenen TA48‑Belege sind als *unbestätigt* markiert; keine Regel dieser Spezifikation
   hängt noch allein an [TA48].
2. **Wem gehören centrum / kismadár / nagymadár in der Ausnahme des [ITVB 7.2]?** Der Normtext nimmt sie von
   der „letzter Sprecher"-Regel aus, sagt aber nicht, welcher Partei sie stattdessen zugerechnet werden.
   Die App‑Entscheidung (§9.27: Spielmacher‑Partei) bleibt eine **Konstruktion**, keine Quellenaussage.
3. **[ITVB 7.4] gegen [ITVB 7.14]** – interner Widerspruch des Regelwerks (§9.29); die gewählte Lesart ist
   plausibel, aber nicht belegt.
4. **Ist die tarokkszám kontrierbar?** Keine Quelle sagt es ausdrücklich (§9.30).
5. **Grundwert beim Alleinspiel** (§9.25) – keine Quelle regelt, ob der Spielwert bei „hivatalból kontra"
   das Gebot bleibt. Bleibt App‑Entscheidung.
6. **[MKVM] widerspricht sich beim Gleichstand** (§9.10): „bei Gleichstand verliert, wer die letzte
   Kontrastufe sagte" direkt gefolgt von „die klassische Regel verpflichtet **in jedem Fall** die
   Spielmacherpartei auf 48". Die App folgt [ITVB 7.11].
7. **47:47 mit Kontra – verdoppelte Zahlung?** [KEGL] verneint, [ITVB]/[BTE] schweigen (§5.12,
   `tie_kontra_not_doubled`).
8. **[MKVM] als „Leitquelle"**: Der Text stammt laut eigener Fußnote von kartya-jatek.hu (Sammlung Jánoska
   Antal) und ist **kein Verbandsregelwerk**; er wurde als gut lesbare, aber nachrangige Quelle behandelt.
   Er ist in einem Punkt strenger als [ITVB] (XX nicht drückbar wird ausdrücklich genannt) und in einem
   Punkt großzügiger ([MKVM] empfiehlt den Einwurf auch nach Tarockdrücken).
9. **Aktualität von [ITVB] 4.0**: bestätigt als jüngste veröffentlichte Fassung (Dokumenthistorie im PDF
   endet mit „4.0 – 2019.01.01. –"); ob seither Beschlüsse ergingen, ist online weiterhin nicht belegbar
   (§14.1).
