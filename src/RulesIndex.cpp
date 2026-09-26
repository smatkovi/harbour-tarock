/*
    Copyright (C) 2026 smatkovi

    This file is part of harbour-tarock.

    harbour-tarock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    harbour-tarock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with harbour-tarock. If not, see <https://www.gnu.org/licenses/>.

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "RulesIndex.h"

#include "ReasonText.h"

#include <QLatin1String>

using tarock::BonusDef;
using tarock::BonusId;
using tarock::ContractDef;
using tarock::ContractId;
using tarock::Phase;
using tarock::ProfileId;
using tarock::RuleProfile;

namespace {

// --- the explanation panel ----------------------------------------------------------
//
// One row per {profile, phase, contract}. ContractId::None means "whatever is
// being played"; a row with a real contract wins over it, so the talon phase
// of the Sechserdreier reads differently from the one of the Rufer (§7.2).
struct PhaseText {
    ProfileId profile;
    Phase phase;
    ContractId contract;
    const char* title;         // QT_TR_NOOP
    const char* whatHappens;   // koenigrufen.md §11.2, two sentences
    const char* whatToDo;      // koenigrufen.md §11.3, the decision
    const char* anchor;
};

const PhaseText kPhaseTexts[] = {
    {ProfileId::AtKrOoe2023, Phase::Deal, ContractId::None, QT_TR_NOOP("Geben"),
     QT_TR_NOOP("Der Geber mischt, der linke Nachbar hebt ab. Jeder bekommt zweimal sechs "
                "Karten, sechs Karten kommen als Talon in die Mitte."),
     QT_TR_NOOP("Schau dir dein Blatt an: Wie viele Tarock hast du, welche Stecher, welche "
                "Könige? Danach spricht die Vorhand zuerst."),
     "2.4"},
    {ProfileId::AtKrOoe2023, Phase::Bidding, ContractId::None, QT_TR_NOOP("Lizitation"),
     QT_TR_NOOP("Jetzt wird versteigert, wer welches Spiel spielen darf. Wer einmal „weiter“ "
                "gesagt hat, ist draußen, und jedes neue Gebot muss höher sein als das vorige."),
     QT_TR_NOOP("Welches Spiel traust du dir zu? Als Vorhand hältst du dir mit „Vorhand“ alles "
                "offen; den Sechserdreier musst du sofort ansagen, sonst ist er weg."),
     "3.1"},
    {ProfileId::AtKrOoe2023, Phase::Call, ContractId::None, QT_TR_NOOP("König rufen"),
     QT_TR_NOOP("Bei den Ruferspielen sucht der Spieler einen Partner: Er ruft einen König, "
                "den er selbst nicht hat. Wer diesen König hält, ist sein Partner – sagt es "
                "aber nicht."),
     QT_TR_NOOP("Ruf am liebsten aus einer Farbe, in der du die Dame hast. Wer drei Könige "
                "hält, darf „den vierten König“ rufen, ohne die Farbe zu verraten."),
     "4.1"},
    {ProfileId::AtKrOoe2023, Phase::Talon, ContractId::None, QT_TR_NOOP("Talon"),
     QT_TR_NOOP("Der Talon liegt in zwei Hälften zu drei Karten offen in der Mitte. Der "
                "Spieler nimmt eine Hälfte; die andere zählt am Ende zu den Gegnern."),
     QT_TR_NOOP("Vergleich die Hälften: Tarock sind meist mehr wert als Punkte. Und merk dir, "
                "dass die liegen gelassene Hälfte den Gegnern gehört."),
     "4.2"},
    {ProfileId::AtKrOoe2023, Phase::Talon, ContractId::Sechserdreier, QT_TR_NOOP("Talon"),
     QT_TR_NOOP("Beim Sechserdreier bekommt der Spieler alle sechs Talonkarten verdeckt und "
                "legt dafür sechs Karten weg."),
     QT_TR_NOOP("Nimm den ganzen Talon – hier gibt es nichts zu wählen. Dafür zählt das Spiel "
                "verloren doppelt."),
     "4.4"},
    {ProfileId::AtKrOoe2023, Phase::Discard, ContractId::None, QT_TR_NOOP("Verlegen"),
     QT_TR_NOOP("Der Spieler legt so viele Karten weg, wie er aus dem Talon genommen hat. "
                "Diese Ablage zählt am Ende zu seinen Stichen."),
     QT_TR_NOOP("Könige und Trullstücke bleiben in der Hand. Am besten wirst du eine Farbe "
                "ganz los – dann kannst du sie später stechen."),
     "4.6"},
    {ProfileId::AtKrOoe2023, Phase::Discard, ContractId::Farbendreier, QT_TR_NOOP("Verlegen"),
     QT_TR_NOOP("Im Farbendreier ist das Verlegen umgekehrt: Zuerst müssen Tarock weg, und "
                "zwar verdeckt."),
     QT_TR_NOOP("Farbkarten darfst du erst verlegen, wenn kein verlegbares Tarock mehr da ist "
                "– und die musst du dann offen hinlegen."),
     "4.6"},
    {ProfileId::AtKrOoe2023, Phase::Announce, ContractId::None, QT_TR_NOOP("Ansagen"),
     QT_TR_NOOP("Jetzt sagt der Spieler seine Prämien an und schließt mit „Ich liege“. Danach "
                "sagen die anderen „gut“, sagen selbst an oder kontrieren."),
     QT_TR_NOOP("Sag nur an, was du wirklich schaffst: Eine stille Prämie zählt die Hälfte, "
                "kann dafür aber nicht verloren gehen."),
     "5.1"},
    {ProfileId::AtKrOoe2023, Phase::Announce, ContractId::Piccolo, QT_TR_NOOP("Ansagen"),
     QT_TR_NOOP("Im Negativspiel gibt es keine Prämien; es geht allein um das Stichziel."),
     QT_TR_NOOP("Sag „Ich liege“ – oder kontriere, wenn du dem Spieler sein Ziel nicht "
                "zutraust. Beim Negativspiel wird gegen jeden Gegner einzeln kontriert."),
     "7.5"},
    {ProfileId::AtKrOoe2023, Phase::Announce, ContractId::Bettel, QT_TR_NOOP("Ansagen"),
     QT_TR_NOOP("Im Negativspiel gibt es keine Prämien; es geht allein um das Stichziel."),
     QT_TR_NOOP("Sag „Ich liege“ – oder kontriere, wenn du dem Spieler sein Ziel nicht "
                "zutraust. Beim Negativspiel wird gegen jeden Gegner einzeln kontriert."),
     "7.5"},
    {ProfileId::AtKrOoe2023, Phase::Play, ContractId::None, QT_TR_NOOP("Spielen"),
     QT_TR_NOOP("Reihum legt jeder eine Karte: Farbzwang, sonst Tarockzwang. Den Stich "
                "gewinnt das höchste Tarock, sonst die höchste Karte der ausgespielten Farbe."),
     QT_TR_NOOP("Denk an die Schwelle von 35 Punkten und 2 Blatt: Schmier dem Partner deine "
                "Figuren und stich die Könige der Gegner ab."),
     "6.2"},
    {ProfileId::AtKrOoe2023, Phase::Play, ContractId::Trischaken, QT_TR_NOOP("Spielen"),
     QT_TR_NOOP("Beim Trischaken spielt jeder für sich und will möglichst wenige Punkte "
                "bekommen."),
     QT_TR_NOOP("Gib die Punkte weg und vermeide Stiche; ideal ist die Jungfrau – kein "
                "einziger Stich."),
     "7.6"},
    {ProfileId::AtKrOoe2023, Phase::Play, ContractId::Piccolo, QT_TR_NOOP("Spielen"),
     QT_TR_NOOP("Im Negativspiel gilt zusätzlich Stichzwang: Du musst überstechen, wenn du "
                "kannst. Der Pagat darf erst als letztes Tarock fallen."),
     QT_TR_NOOP("Zähl deine sicheren Stiche: Der Piccolo will genau einen, nicht mehr und "
                "nicht weniger."),
     "6.4"},
    {ProfileId::AtKrOoe2023, Phase::Play, ContractId::Bettel, QT_TR_NOOP("Spielen"),
     QT_TR_NOOP("Im Negativspiel gilt zusätzlich Stichzwang: Du musst überstechen, wenn du "
                "kannst. Der Pagat darf erst als letztes Tarock fallen."),
     QT_TR_NOOP("Der Bettel will keinen einzigen Stich – spiel deine hohen Karten weg, "
                "solange die Gegner noch darüberstechen können."),
     "6.4"},
    {ProfileId::AtKrOoe2023, Phase::Play, ContractId::Farbendreier, QT_TR_NOOP("Spielen"),
     QT_TR_NOOP("Im Farbenspiel sticht Tarock keine Farbe, und Tarock darf erst ausgespielt "
                "werden, wenn keine Farbkarte mehr im Blatt ist."),
     QT_TR_NOOP("Spiel deine langen Farben und deine Könige; Tarock sind hier fast nur "
                "Abwurf."),
     "6.3"},
    {ProfileId::AtKrOoe2023, Phase::Play, ContractId::Farbensolo, QT_TR_NOOP("Spielen"),
     QT_TR_NOOP("Im Farbenspiel sticht Tarock keine Farbe, und Tarock darf erst ausgespielt "
                "werden, wenn keine Farbkarte mehr im Blatt ist."),
     QT_TR_NOOP("Spiel deine langen Farben und deine Könige; Tarock sind hier fast nur "
                "Abwurf."),
     "6.3"},
    {ProfileId::AtKrOoe2023, Phase::Scoring, ContractId::None, QT_TR_NOOP("Abrechnung"),
     QT_TR_NOOP("Jede Partei zählt ihre Stiche samt Ablage bzw. Talonhälfte. Ab 35 Punkten "
                "und 2 Blatt ist ein positives Spiel gewonnen."),
     QT_TR_NOOP("Schau dir die Posten an: Jede Prämie wird getrennt verrechnet – man kann das "
                "Spiel gewinnen und trotzdem Minus machen."),
     "7.1"},
    {ProfileId::AtKrOoe2023, Phase::HandOver, ContractId::None, QT_TR_NOOP("Abrechnung"),
     QT_TR_NOOP("Jede Partei zählt ihre Stiche samt Ablage bzw. Talonhälfte. Ab 35 Punkten "
                "und 2 Blatt ist ein positives Spiel gewonnen."),
     QT_TR_NOOP("Schau dir die Posten an: Jede Prämie wird getrennt verrechnet – man kann das "
                "Spiel gewinnen und trotzdem Minus machen."),
     "7.1"},
};

// --- glossary, koenigrufen.md §11.6 --------------------------------------------------
//
// term, synonyms (comma separated), definition, rule chapter. Copied from the
// specification table; the synonyms are the parenthesised alternative names,
// so that "Schuss" also finds "Kontra".
struct GlossaryRow {
    const char* term;
    const char* synonyms;
    const char* definition;
    const char* anchor;
};

const GlossaryRow kGlossaryKr[] = {
    { "Ablage", "", QT_TR_NOOP("Die Karten, die der Spieler nach der Talonaufnahme verlegt. Sie zählen zu seinen Stichen."), "4.6" },
    { "Absolut", "", QT_TR_NOOP("Prämie für besonders viele Punkte (45/2). Im OÖ-Profil nicht gespielt."), "1.4" },
    { "Angesagt", "", QT_TR_NOOP("Eine Prämie, die vorher angekündigt wurde. Zählt doppelt so viel wie „still“, kann aber verloren gehen und kontriert werden."), "5.1" },
    { "Ansager", "", QT_TR_NOOP("OÖ-Ausdruck für die Vögel (I, II, III, IIII), daher „A-Rufer“."), "1.3" },
    { "Austarockieren", "", QT_TR_NOOP("Tarock so lange spielen, bis die anderen keine mehr haben. Beim Trischaken ausdrücklich nicht verlangt."), "1.3" },
    { "Besserrufer", "A-Rufer", QT_TR_NOOP("Ruferspiel mit Pflichtansage eines Vogels."), "1.3" },
    { "Bettel", "Bettler", QT_TR_NOOP("Negativspiel: keinen einzigen Stich machen."), "3.1" },
    { "Blatt", "", QT_TR_NOOP("1. die zwölf Karten eines Spielers, 2. eine Karte mit dem kleinsten Wert (⅓ Punkt)."), "1.4" },
    { "Bürgermeister", "", QT_TR_NOOP("Beim Trischaken: wer mindestens 35 Punkte und 2 Blatt bekommen hat; er zahlt doppelt."), "1.4" },
    { "Cavall", "", QT_TR_NOOP("anderer Name für den Reiter."), "1.3" },
    { "Dreier", "", QT_TR_NOOP("Alleinspiel gegen drei mit drei Talonkarten."), "4.6" },
    { "Fahren", "", QT_TR_NOOP("anderer Name für Trischaken."), "3.1" },
    { "Fänge", "", QT_TR_NOOP("Prämien für das Abstechen von Mond, Sküs oder gerufenem König. Im OÖ-Profil nicht gespielt."), "1.3" },
    { "Farbzwang", "Bedienzwang", QT_TR_NOOP("Man muss die ausgespielte Farbe bedienen, wenn man sie hat."), "6.2" },
    { "Farbenspiel", "", QT_TR_NOOP("Farbendreier oder Farbensolo: Tarock stechen keine Farbe."), "1.3" },
    { "Gabel", "", QT_TR_NOOP("Zwei Karten einer Farbe, die eine gegnerische Karte „einklemmen“ (z. B. König und Reiter gegen die Dame)."), "1.3" },
    { "Geber", "", QT_TR_NOOP("Wer mischt und austeilt. Am Fünfertisch setzt er aus."), "3.1" },
    { "Gstieß", "", QT_TR_NOOP("anderer Name für den Sküs."), "1.3" },
    { "Hausmeister", "", QT_TR_NOOP("Das höchste Tarock, das noch im Spiel ist."), "1.3" },
    { "Ich liege", "", QT_TR_NOOP("Formel, mit der der Spieler seine Ansagen beendet und das Spiel eröffnet."), "3.1" },
    { "Jungfrau", "", QT_TR_NOOP("Beim Trischaken: ein Spieler ohne Stich; er kassiert."), "1.4" },
    { "Kaiserstich", "Märchenstich", QT_TR_NOOP("Sonderregel, nach der der Pagat sticht, wenn die ganze Trull in einem Stich fällt. Im OÖ-Profil nicht gespielt."), "1.3" },
    { "Kiebitz", "", QT_TR_NOOP("Zuschauer; am Fünfertisch der aussetzende Geber. Er hat zu schweigen."), "3.1" },
    { "König ultimo", "", QT_TR_NOOP("Prämie: Der gerufene König liegt im letzten Stich, und die Spielerpartei gewinnt ihn."), "3.1" },
    { "Kontra", "Schuss, gespritzt", QT_TR_NOOP("Verdoppelt den Wert eines Postens; Rekontra („retour“) vervierfacht, Subkontra verachtfacht. Zählt nur fürs Geld."), "5.1" },
    { "Lizitation", "", QT_TR_NOOP("Die Versteigerung des Spiels."), "3.1" },
    { "Mond", "", QT_TR_NOOP("Tarock XXI, zweithöchstes Tarock, Trullstück, 5 Punkte."), "1.3" },
    { "Negativspiel", "", QT_TR_NOOP("Trischaken, Piccolo, Bettel und die Ouvertspiele."), "3.1" },
    { "Ouvert", "", QT_TR_NOOP("„offen“: Nach dem ersten Stich legen alle ihre Karten offen hin."), "3.1" },
    { "Pagat", "Spatz", QT_TR_NOOP("Tarock I, kleinstes Tarock, Trullstück, 5 Punkte."), "1.3" },
    { "Pagat ultimo", "", QT_TR_NOOP("Prämie: mit dem Pagat den letzten Stich machen."), "1.3" },
    { "Piccolo", "", QT_TR_NOOP("Negativspiel: genau einen Stich machen."), "3.1" },
    { "Prämie", "Ansage, Zusatzprämie", QT_TR_NOOP("Zusatzziel neben dem Spiel: Trull, Alle Könige, König ultimo, Vögel, Valat."), "1.3" },
    { "Quapil", "Wildsau, Marabu", QT_TR_NOOP("Tarock IIII; Prämie für den viertletzten Stich."), "1.3" },
    { "Radl", "", QT_TR_NOOP("Eine Runde, in der jeder einmal gibt: 4 Spiele zu viert, 5 Spiele zu fünft."), "3.1" },
    { "Reiter", "", QT_TR_NOOP("Figur zwischen Dame und Bube (Pferd im Bild), 3 Punkte."), "1.3" },
    { "Renonce", "", QT_TR_NOOP("Regelverstoß mit Strafwertung. In der App ausgeschlossen, weil unzulässige Züge gar nicht möglich sind."), "6.2" },
    { "Rufer", "", QT_TR_NOOP("Das billigste Partnerspiel der Vorhand."), "3.1" },
    { "Schleifen", "", QT_TR_NOOP("Aufgeben eines Ruferspiels, wenn der gerufene König im Talon liegt."), "4.6" },
    { "Schmieren", "", QT_TR_NOOP("Dem Partner hohe Punktekarten in den Stich legen."), "1.4" },
    { "Sechserdreier", "", QT_TR_NOOP("Alleinspiel der Vorhand mit allen sechs Talonkarten; verloren zählt es doppelt."), "4.6" },
    { "Skartin", "Skartindel, Glatze", QT_TR_NOOP("Die kleinen Farbkarten (1–4 bzw. 7–10)."), "1.3" },
    { "Sküs", "", QT_TR_NOOP("Höchstes Tarock, ohne Zahl, Trullstück, 5 Punkte."), "1.3" },
    { "Solospiel", "", QT_TR_NOOP("Spiel ohne Talonaufnahme (Solorufer, Solodreier, Farbensolo). Alle Prämien zählen doppelt."), "4.6" },
    { "Spieler", "Spielersteher", QT_TR_NOOP("Wer die Lizitation gewonnen hat."), "3.1" },
    { "Stecher", "", QT_TR_NOOP("Umgangssprachlich die hohen Tarock (etwa ab XVI)."), "1.3" },
    { "Stichzwang", "", QT_TR_NOOP("Pflicht, die höchste liegende Karte zu überstechen – nur in Negativspielen."), "3.1" },
    { "Still", "heimlich", QT_TR_NOOP("Eine Prämie ohne Ansage erzielen; sie zählt die Hälfte."), "5.1" },
    { "Talon", "", QT_TR_NOOP("Die sechs Karten in der Tischmitte, in zwei Hälften zu drei."), "4.6" },
    { "Tarock", "", QT_TR_NOOP("1. die 22 Trümpfe, 2. das Spiel selbst."), "1.3" },
    { "Tarockzwang", "", QT_TR_NOOP("Pflicht, Tarock zuzugeben, wenn man die ausgespielte Farbe nicht hat."), "1.3" },
    { "Trischaken", "", QT_TR_NOOP("Vorhandspiel, bei dem jeder gegen jeden möglichst wenige Punkte machen will."), "1.4" },
    { "Trull", "", QT_TR_NOOP("Sküs, Mond und Pagat zusammen; auch die Prämie dafür."), "1.3" },
    { "Uhu", "", QT_TR_NOOP("Tarock II; Prämie für den vorletzten Stich."), "1.3" },
    { "Ultimo", "", QT_TR_NOOP("„zum Schluss“ – bezieht sich auf den letzten Stich."), "5.1" },
    { "Valat", "", QT_TR_NOOP("Alle zwölf Stiche einer Partei."), "5.1" },
    { "Verlegen", "", QT_TR_NOOP("Karten nach der Talonaufnahme ablegen."), "4.6" },
    { "Vogel", "Vogerl", QT_TR_NOOP("Pagat, Uhu, Kakadu, Quapil und die zugehörigen Prämien."), "1.3" },
    { "Vorhand", "", QT_TR_NOOP("Der Spieler rechts vom Geber; spricht zuerst und spielt in positiven Spielen aus."), "3.1" },
    { "Vorhandspiel", "", QT_TR_NOOP("Spiel, das nur die Vorhand ansagen darf: Trischaken, Rufer, Sechserdreier."), "3.1" },
    { "Weiter", "gut", QT_TR_NOOP("Passen in der Lizitation bzw. Kenntnisnahme einer Ansage."), "3.1" },
    { "Zähler", "", QT_TR_NOOP("Karte mit mehr als 1 Punkt (König, Trullstück, Dame, Reiter, Bube)."), "1.3" },
};

// --- typical beginner mistakes, koenigrufen.md §11.5 ---------------------------------
struct MistakeRow {
    int number;
    const char* title;
    const char* text;
    const char* anchor;
};

const MistakeRow kMistakesKr[] = {
    { 1, QT_TR_NOOP("Reiter für den König halten"), QT_TR_NOOP("Der Reiter sitzt auf einem Pferd, der König trägt Krone und Zepter."), "1.2" },
    { 2, QT_TR_NOOP("Rote Kleine falsch einschätzen"), QT_TR_NOOP("In Herz und Karo ist das As die höchste kleine Karte und die 4 die niedrigste; in Pik und Treff ist die 10 die höchste und die 7 die niedrigste."), "1.2" },
    { 3, QT_TR_NOOP("35 Punkte für genug halten"), QT_TR_NOOP("Es braucht 35 und 2 Blatt. Bei 35/1 ist das Spiel verloren."), "1.6" },
    { 4, QT_TR_NOOP("Den eigenen König rufen oder mit vier Königen ein Ruferspiel ansagen"), QT_TR_NOOP("Beides ist verboten."), "4.1" },
    { 5, QT_TR_NOOP("Könige oder Trullstücke verlegen wollen – nie erlaubt; Tarock nur im Notfall und dann offen"), QT_TR_NOOP(""), "4.6" },
    { 6, QT_TR_NOOP("Tarockzwang vergessen"), QT_TR_NOOP("Wer die Farbe nicht hat, muss Tarock zugeben, auch wenn er den Stich nicht gewinnt. Wer stattdessen eine andere Farbe abwirft, macht am Tisch eine Renonce."), "6.2" },
    { 7, QT_TR_NOOP("Im Negativspiel den Pagat zu früh spielen"), QT_TR_NOOP("Er darf erst weg, wenn er das letzte Tarock ist."), "6.4" },
    { 8, QT_TR_NOOP("Im Farbenspiel Tarock ausspielen, obwohl noch Farbkarten in der Hand sind"), QT_TR_NOOP(""), "6.3" },
    { 9, QT_TR_NOOP("Angesagten Vogel zu früh hergeben"), QT_TR_NOOP("Verboten ist das nicht – aber die Prämie ist dann weg. Müssen mehrere weg, zuerst der höhere. Wirklich verboten (Renonce) ist nur, mit dem angesagten Vogel vorzeitig einen Stich zu gewinnen."), "6.5" },
    { 10, QT_TR_NOOP("Vogelgier"), QT_TR_NOOP("Ein stiller Vogel ist schön, kostet aber oft das Spiel: „Das Taktieren mit Vogerln kann dir das Spiel kosten.“"), "5.5" },
    { 11, QT_TR_NOOP("Nicht schmieren"), QT_TR_NOOP("Hohe Figuren gehören in den sicheren Stich des Partners, nicht ins eigene Blatt gehortet."), "10.6" },
    { 12, QT_TR_NOOP("Tarock horten"), QT_TR_NOOP("Wer nie sticht, bleibt am Ende mit wertlosen hohen Tarock sitzen."), "10.6" },
    { 13, QT_TR_NOOP("Kurze Könige zu früh selbst anspielen – als Alleinspieler beinahe immer ein Fehler"), QT_TR_NOOP(""), "10.6" },
    { 14, QT_TR_NOOP("Nicht mitzählen"), QT_TR_NOOP("Wenigstens die gefallenen Tarock und die Vögel muss man sich merken; die liegen gelassene Talonhälfte mitzählen!"), "1.5" },
    { 15, QT_TR_NOOP("Kontra persönlich nehmen"), QT_TR_NOOP("Ein Kontra ist Teil des Spiels und zählt bei uns nur fürs Geld."), "5.6" },
    { 16, QT_TR_NOOP("Den Partner suchen, indem man redet"), QT_TR_NOOP("Erlaubt sind nur Signale durch das Spiel selbst."), "2.8" },
    { 17, QT_TR_NOOP("Vergessen, dass die liegen gelassene Talonhälfte den Gegnern gehört – oft entscheidet sie das Spiel"), QT_TR_NOOP(""), "4.2" },
    { 18, QT_TR_NOOP("Am Fünfertisch mitreden"), QT_TR_NOOP("Der Aussetzer ist Zuschauer und darf den Talon nicht sehen."), "2.7" },
    { 19, QT_TR_NOOP("Beim Trischaken Stiche machen"), QT_TR_NOOP("Ziel ist, möglichst wenige Punkte zu bekommen; ideal ist die Jungfrau (kein Stich)."), "7.6" },
    { 20, QT_TR_NOOP("Zu vorsichtig lizitieren"), QT_TR_NOOP("„Wer nichts wagt, gewinnt auch nichts“ – und wer gar nie ansagt, bekommt das Trischaken serviert."), "3.1" },
};

// --- "Warum es jedes Spiel und jede Prämie gibt", koenigrufen.md §11.3 ---------------
struct PurposeRow {
    ProfileId profile;
    int id;                 // ContractId or BonusId, depending on the table
    const char* purpose;    // "Wozu es da ist"
    const char* when;       // "Wann man es wählt"
};

const PurposeRow kContractPurposeKr[] = {
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Trischaken),
     QT_TR_NOOP("Strafspiel für „Staudenhocker“: Wer nichts ansagt, riskiert, dass die Vorhand "
                "alle zum Punktevermeiden zwingt. Es sorgt dafür, dass mutig lizitiert wird."),
     QT_TR_NOOP("Vorhand mit wenigen, kleinen Tarock und ohne Könige, wenn sonst niemand etwas "
                "will.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Rufer),
     QT_TR_NOOP("Das Grundspiel: billig, aber mit Partner. Es gibt der Vorhand auch mit "
                "mittlerem Blatt eine Aufgabe."),
     QT_TR_NOOP("Einige Tarock mit Stecher, dazu ein Trullstück oder König: zu schwach für "
                "mehr, zu stark fürs Trischaken.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Piccolo),
     QT_TR_NOOP("Chance für schlechte Blätter: Wer fast nichts sticht, kann daraus ein Spiel "
                "machen."),
     QT_TR_NOOP("Genau ein sicherer Stich (Sküs oder kurzer König).")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Bettel),
     QT_TR_NOOP("Chance für schlechte Blätter: Wer fast nichts sticht, kann daraus ein Spiel "
                "machen."),
     QT_TR_NOOP("Keine gefährlichen Karten, viele Schlusskarten.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Solorufer),
     QT_TR_NOOP("Belohnt ein Blatt mit hohen Tarock ohne Talonhilfe; hier zählt jede Prämie "
                "doppelt, vor allem der König ultimo."),
     QT_TR_NOOP("5–6 Tarock mit Sküs oder Mond, ein König, kurze Farben.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Besserrufer),
     QT_TR_NOOP("Das eigentliche Tarockspiel mit Partner: Man verpflichtet sich zu einem Vogel "
                "und bekommt dafür einen hohen Zusatzwert."),
     QT_TR_NOOP("Viele Tarock (Pagat ab 6, Uhu ab 7, Kakadu ab 8–9) mit Stechern.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Farbendreier),
     QT_TR_NOOP("Gibt Blättern mit vielen Königen und Figuren, aber schlechten Tarock eine "
                "Chance, weil Tarock hier nicht stechen."),
     QT_TR_NOOP("6–7 sichere Farbstiche.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Sechserdreier),
     QT_TR_NOOP("Das Wagnis der Vorhand: alle sechs Talonkarten, dafür doppelter Verlust."),
     QT_TR_NOOP("6–7 Tarock mit Trullstück und Stechern, Schmiere zum Verlegen.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Dreier),
     QT_TR_NOOP("Das klassische Alleinspiel: dreifache Bezahlung."),
     QT_TR_NOOP("Wie Sechserdreier, aber etwas stärker, weil nur drei Talonkarten kommen.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Farbensolo),
     QT_TR_NOOP("Gibt Blättern mit vielen Königen und Figuren, aber schlechten Tarock eine "
                "Chance, weil Tarock hier nicht stechen."),
     QT_TR_NOOP("8 sichere Farbstiche.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::PiccoloOuvert),
     QT_TR_NOOP("Die Königsdisziplin der Negativspiele: mit offenen Karten gegen drei "
                "beratende Gegner."),
     QT_TR_NOOP("Sehr viele Schlusskarten, in jeder Farbe zwei Karten.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::BettelOuvert),
     QT_TR_NOOP("Die Königsdisziplin der Negativspiele: mit offenen Karten gegen drei "
                "beratende Gegner."),
     QT_TR_NOOP("Sehr viele Schlusskarten, in jeder Farbe zwei Karten.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(ContractId::Solodreier),
     QT_TR_NOOP("Das höchste Spiel für ein Traumblatt; alle Prämien doppelt."),
     QT_TR_NOOP("9–10 Tarock mit 4–5 Stechern und Königen.")},
};

const PurposeRow kBonusPurposeKr[] = {
    {ProfileId::AtKrOoe2023, static_cast<int>(BonusId::Pagat),
     QT_TR_NOOP("Der Reiz, die kleinste Karte bis zum Schluss durchzubringen. Älteste "
                "Zusatzprämie des Tarocks."),
     QT_TR_NOOP("Wenn man genug hohe Tarock hat, um alle anderen leerzuspielen.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(BonusId::Uhu),
     QT_TR_NOOP("Dasselbe für den vorletzten Stich; je höher der Vogel, desto mehr "
                "Tarockübermacht ist nötig."),
     QT_TR_NOOP("Nur mit klarer Tarockhoheit, sonst still versuchen.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(BonusId::Kakadu),
     QT_TR_NOOP("Dasselbe für den drittletzten Stich; je höher der Vogel, desto mehr "
                "Tarockübermacht ist nötig."),
     QT_TR_NOOP("Nur mit klarer Tarockhoheit, sonst still versuchen.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(BonusId::Quapil),
     QT_TR_NOOP("Dasselbe für den viertletzten Stich; je höher der Vogel, desto mehr "
                "Tarockübermacht ist nötig."),
     QT_TR_NOOP("Nur mit klarer Tarockhoheit, sonst still versuchen.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(BonusId::KingUltimo),
     QT_TR_NOOP("Belohnt das Zusammenspiel: Der Partner bringt den gerufenen König im letzten "
                "Stich heim – und gibt sich dadurch zu erkennen."),
     QT_TR_NOOP("König mindestens zu dritt und 3–4 Tarock mit Stecher.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(BonusId::Trull),
     QT_TR_NOOP("Belohnt den Besitz der drei teuersten Tarock."),
     QT_TR_NOOP("Wenn man zwei Trullstücke hat und das dritte fangen kann; auf Verdacht auch "
                "als Signal an den Partner.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(BonusId::AllKings),
     QT_TR_NOOP("Belohnt Königsblätter, die sonst leicht abgestochen werden."),
     QT_TR_NOOP("Mit drei Königen und Tarockschutz.")},
    {ProfileId::AtKrOoe2023, static_cast<int>(BonusId::Valat),
     QT_TR_NOOP("Der Jackpot: alle zwölf Stiche."),
     QT_TR_NOOP("Nur mit nahezu geschlossener Tarockfolge; sonst still probieren.")},
};

// --- the chapters of the rule browser (§7.7) ----------------------------------------
struct ChapterRow {
    ProfileId profile;
    const char* anchor;
    int level;               // 1 = chapter, 2 = section
    const char* title;
};

const ChapterRow kChaptersKr[] = {
    {ProfileId::AtKrOoe2023, "1", 1, QT_TR_NOOP("Kartensatz, Rangfolgen, Kartenwerte, Zählen")},
    {ProfileId::AtKrOoe2023, "1.2", 2, QT_TR_NOOP("Rangfolge in den Farben")},
    {ProfileId::AtKrOoe2023, "1.3", 2, QT_TR_NOOP("Tarock (Trümpfe)")},
    {ProfileId::AtKrOoe2023, "1.4", 2, QT_TR_NOOP("Kartenwerte")},
    {ProfileId::AtKrOoe2023, "1.5", 2, QT_TR_NOOP("Zählmethode")},
    {ProfileId::AtKrOoe2023, "1.6", 2, QT_TR_NOOP("Gewinnschwelle")},
    {ProfileId::AtKrOoe2023, "2", 1, QT_TR_NOOP("Tisch, Geben, Talon, Vierer- und Fünfertisch")},
    {ProfileId::AtKrOoe2023, "2.4", 2, QT_TR_NOOP("Austeilen")},
    {ProfileId::AtKrOoe2023, "2.7", 2, QT_TR_NOOP("Fünfertisch")},
    {ProfileId::AtKrOoe2023, "2.8", 2, QT_TR_NOOP("Sichtbarkeit während des Spiels")},
    {ProfileId::AtKrOoe2023, "3", 1, QT_TR_NOOP("Lizitation")},
    {ProfileId::AtKrOoe2023, "3.1", 2, QT_TR_NOOP("Die Spiele in Lizitationsreihenfolge")},
    {ProfileId::AtKrOoe2023, "3.3", 2, QT_TR_NOOP("Rechte und Pflichten der Vorhand")},
    {ProfileId::AtKrOoe2023, "3.4", 2, QT_TR_NOOP("Die übrigen Spieler")},
    {ProfileId::AtKrOoe2023, "3.5", 2, QT_TR_NOOP("Lizitation als Zustandsautomat")},
    {ProfileId::AtKrOoe2023, "3.7", 2, QT_TR_NOOP("Blattbedingungen für Gebote")},
    {ProfileId::AtKrOoe2023, "4", 1, QT_TR_NOOP("Partner, Talon und Verlegen")},
    {ProfileId::AtKrOoe2023, "4.1", 2, QT_TR_NOOP("Königsruf")},
    {ProfileId::AtKrOoe2023, "4.2", 2, QT_TR_NOOP("Talonbehandlung je Spiel")},
    {ProfileId::AtKrOoe2023, "4.3", 2, QT_TR_NOOP("Offener Talon")},
    {ProfileId::AtKrOoe2023, "4.4", 2, QT_TR_NOOP("Sechserdreier")},
    {ProfileId::AtKrOoe2023, "4.6", 2, QT_TR_NOOP("Verlegen (Ablage)")},
    {ProfileId::AtKrOoe2023, "5", 1, QT_TR_NOOP("Ansagen (Prämien) und Kontra")},
    {ProfileId::AtKrOoe2023, "5.1", 2, QT_TR_NOOP("Übersicht der Prämien")},
    {ProfileId::AtKrOoe2023, "5.3", 2, QT_TR_NOOP("Wer darf wann was ansagen")},
    {ProfileId::AtKrOoe2023, "5.4", 2, QT_TR_NOOP("Pflichten aus einer Ansage")},
    {ProfileId::AtKrOoe2023, "5.5", 2, QT_TR_NOOP("Still oder angesagt")},
    {ProfileId::AtKrOoe2023, "5.6", 2, QT_TR_NOOP("Kontra, Rekontra, Subkontra")},
    {ProfileId::AtKrOoe2023, "6", 1, QT_TR_NOOP("Spielregeln im Stich")},
    {ProfileId::AtKrOoe2023, "6.1", 2, QT_TR_NOOP("Ausspiel und Stichfolge")},
    {ProfileId::AtKrOoe2023, "6.2", 2, QT_TR_NOOP("Grundzwänge (alle Spiele)")},
    {ProfileId::AtKrOoe2023, "6.3", 2, QT_TR_NOOP("Farbenspiele")},
    {ProfileId::AtKrOoe2023, "6.4", 2, QT_TR_NOOP("Negativspiele")},
    {ProfileId::AtKrOoe2023, "6.5", 2, QT_TR_NOOP("Zugbeschränkungen aus Ansagen")},
    {ProfileId::AtKrOoe2023, "6.7", 2, QT_TR_NOOP("Erlaubte Karten")},
    {ProfileId::AtKrOoe2023, "7", 1, QT_TR_NOOP("Abrechnung")},
    {ProfileId::AtKrOoe2023, "7.1", 2, QT_TR_NOOP("Grundsätze")},
    {ProfileId::AtKrOoe2023, "7.4", 2, QT_TR_NOOP("Valat")},
    {ProfileId::AtKrOoe2023, "7.5", 2, QT_TR_NOOP("Negativspiele")},
    {ProfileId::AtKrOoe2023, "7.6", 2, QT_TR_NOOP("Trischaken")},
    {ProfileId::AtKrOoe2023, "8", 1, QT_TR_NOOP("Partiestruktur")},
    {ProfileId::AtKrOoe2023, "9", 1, QT_TR_NOOP("Sonderfälle und regionale Abweichungen")},
    {ProfileId::AtKrOoe2023, "10.6", 2, QT_TR_NOOP("Kartenspiel-Heuristiken")},
    {ProfileId::AtKrOoe2023, "11.6", 2, QT_TR_NOOP("Glossar")},
};

const char* profileStem(ProfileId profile)
{
    switch (profile) {
    case ProfileId::HuIlluItvb2019: return "hu-illu";
    case ProfileId::AtTappKlassik:  return "at-tapp";
    default:                        return "at-kr-ooe";
    }
}

// The chain of the explanation panel: "3 von 7 · König rufen" (§7.2).
const Phase kChain[] = {Phase::Deal, Phase::Bidding, Phase::Call, Phase::Talon,
                        Phase::Discard, Phase::Announce, Phase::Play};

const PhaseText* findPhaseText(ProfileId profile, Phase phase, ContractId contract)
{
    const PhaseText* fallback = nullptr;
    const PhaseText* anyProfile = nullptr;
    for (const PhaseText& row : kPhaseTexts) {
        if (row.phase != phase)
            continue;
        if (row.profile == profile && row.contract == contract)
            return &row;
        if (row.profile == profile && row.contract == ContractId::None)
            fallback = &row;
        else if (row.contract == ContractId::None && !anyProfile)
            anyProfile = &row;
    }
    // A profile whose tables are not filled in yet still shows the Königrufen
    // wording rather than nothing.
    return fallback ? fallback : anyProfile;
}

} // namespace

// --- the explanation panel ----------------------------------------------------------

QString RulesIndex::phaseKey(Phase phase)
{
    switch (phase) {
    case Phase::Deal: return QStringLiteral("deal");
    case Phase::Bidding: return QStringLiteral("bidding");
    case Phase::Call: return QStringLiteral("call");
    case Phase::Talon: return QStringLiteral("talon");
    case Phase::Discard: return QStringLiteral("discard");
    case Phase::Announce: return QStringLiteral("announce");
    case Phase::Play: return QStringLiteral("play");
    case Phase::Scoring: return QStringLiteral("scoring");
    case Phase::HandOver: return QStringLiteral("handover");
    }
    return QString();
}

int RulesIndex::stepCount()
{
    return static_cast<int>(sizeof(kChain) / sizeof(kChain[0]));
}

int RulesIndex::stepIndex(Phase phase)
{
    for (int i = 0; i < stepCount(); ++i) {
        if (kChain[i] == phase)
            return i + 1;
    }
    return stepCount();   // the settlement comes after the chain
}

QString RulesIndex::phaseTitle(ProfileId profile, Phase phase)
{
    const PhaseText* row = findPhaseText(profile, phase, ContractId::None);
    return row ? tr(row->title) : QString();
}

QVariantMap RulesIndex::explanation(ProfileId profile, Phase phase, ContractId contract)
{
    QVariantMap map;
    const PhaseText* row = findPhaseText(profile, phase, contract);
    const QString happens = row ? tr(row->whatHappens) : QString();
    const QString todo = row ? tr(row->whatToDo) : QString();
    map.insert(QStringLiteral("phaseKey"), phaseKey(phase));
    map.insert(QStringLiteral("phaseTitle"), row ? tr(row->title) : QString());
    map.insert(QStringLiteral("stepIndex"), stepIndex(phase));
    map.insert(QStringLiteral("stepCount"), stepCount());
    map.insert(QStringLiteral("whatHappens"), happens);
    map.insert(QStringLiteral("whatToDo"), todo);
    map.insert(QStringLiteral("anchor"), anchorFor(profile, QString::fromLatin1(row ? row->anchor : "1")));
    map.insert(QStringLiteral("terms"), termsIn(profile, happens + QLatin1Char(' ') + todo));
    map.insert(QStringLiteral("contract"), static_cast<int>(contract));
    return map;
}

// --- glossary -----------------------------------------------------------------------

QVariantList RulesIndex::glossary(ProfileId profile)
{
    QVariantList list;
    for (const GlossaryRow& row : kGlossaryKr) {
        QVariantMap entry;
        entry.insert(QStringLiteral("term"), QString::fromUtf8(row.term));
        entry.insert(QStringLiteral("definition"), tr(row.definition));
        entry.insert(QStringLiteral("anchor"), anchorFor(profile, QString::fromLatin1(row.anchor)));
        QStringList synonyms;
        const QString raw = QString::fromUtf8(row.synonyms);
        if (!raw.isEmpty()) {
            const QStringList parts = raw.split(QLatin1Char(','));
            for (const QString& part : parts) {
                const QString trimmed = part.trimmed();
                if (!trimmed.isEmpty())
                    synonyms.append(trimmed);
            }
        }
        entry.insert(QStringLiteral("synonyms"), synonyms);
        list.append(entry);
    }
    return list;
}

QVariantMap RulesIndex::term(ProfileId profile, const QString& name)
{
    const QString wanted = name.trimmed();
    const QVariantList all = glossary(profile);
    for (const QVariant& item : all) {
        const QVariantMap entry = item.toMap();
        if (entry.value(QStringLiteral("term")).toString().compare(wanted, Qt::CaseInsensitive) == 0)
            return entry;
        const QStringList synonyms = entry.value(QStringLiteral("synonyms")).toStringList();
        for (const QString& synonym : synonyms) {
            if (synonym.compare(wanted, Qt::CaseInsensitive) == 0)
                return entry;
        }
    }
    return QVariantMap();
}

QVariantList RulesIndex::termsIn(ProfileId profile, const QString& text)
{
    QVariantList found;
    if (text.isEmpty())
        return found;
    const QVariantList all = glossary(profile);
    for (const QVariant& item : all) {
        const QVariantMap entry = item.toMap();
        const QString word = entry.value(QStringLiteral("term")).toString();
        // Only whole words, so "Trull" does not also match "Trullstück".
        if (word.length() < 4 || !text.contains(word))
            continue;
        const int at = text.indexOf(word);
        const int after = at + word.length();
        const bool endsWord = after >= text.length() || !text.at(after).isLetter();
        if (endsWord)
            found.append(entry);
    }
    return found;
}

QString RulesIndex::markup(ProfileId profile, const QString& text)
{
    QString result = text;
    const QVariantList terms = termsIn(profile, text);
    for (const QVariant& item : terms) {
        const QString word = item.toMap().value(QStringLiteral("term")).toString();
        // The first occurrence is enough; more links than that make the
        // paragraph unreadable.
        const int at = result.indexOf(word);
        if (at < 0 || result.mid(0, at).contains(QLatin1String("<a href")))
            continue;
        const QString link = QLatin1String("<a href=\"glossary:") + word + QLatin1String("\">")
            + word + QLatin1String("</a>");
        result.replace(at, word.length(), link);
    }
    return result;
}

// --- rule reference -----------------------------------------------------------------

QString RulesIndex::anchorFor(ProfileId profile, const QString& chapter)
{
    return QLatin1String("rules:") + QLatin1String(profileStem(profile)) + QLatin1Char('#')
        + chapter;
}

QVariantList RulesIndex::chapters(ProfileId profile)
{
    QVariantList list;
    // Die Kapitelliste ist die des Königrufens (und, in denselben Nummern,
    // die des ungarischen Blattes). Für das Tapp-Tarock steht das Regelwerk
    // bisher nur in docs/tapptarock.md; lieber keine Kapitel zeigen als
    // fremde (docs/design.md §12, M11).
    if (profile == ProfileId::AtTappKlassik)
        return list;
    for (const ChapterRow& row : kChaptersKr) {
        QVariantMap entry;
        entry.insert(QStringLiteral("chapter"), QString::fromLatin1(row.anchor));
        entry.insert(QStringLiteral("anchor"), anchorFor(profile, QString::fromLatin1(row.anchor)));
        entry.insert(QStringLiteral("level"), row.level);
        entry.insert(QStringLiteral("title"), tr(row.title));
        list.append(entry);
    }
    return list;
}

QString RulesIndex::chapterTitle(ProfileId profile, const QString& anchor)
{
    // Both "6.2" and the full "rules:at-kr-ooe#6.2" are accepted.
    const int hash = anchor.indexOf(QLatin1Char('#'));
    const QString chapter = hash >= 0 ? anchor.mid(hash + 1) : anchor;
    Q_UNUSED(profile);
    for (const ChapterRow& row : kChaptersKr) {
        if (chapter == QLatin1String(row.anchor))
            return tr(row.title);
    }
    return QString();
}

QVariantList RulesIndex::tariffTable(ProfileId profile)
{
    QVariantList list;
    const RuleProfile& rules = RuleProfile::get(profile);
    for (const ContractDef& def : rules.contracts()) {
        QVariantMap entry;
        entry.insert(QStringLiteral("kind"), QStringLiteral("contract"));
        entry.insert(QStringLiteral("key"), QString::fromLatin1(def.key));
        entry.insert(QStringLiteral("id"), static_cast<int>(def.id));
        entry.insert(QStringLiteral("name"), ReasonText::contractName(static_cast<int>(def.id)));
        entry.insert(QStringLiteral("rank"), def.rank);
        entry.insert(QStringLiteral("value"), def.baseValue);
        entry.insert(QStringLiteral("lostValue"), def.lostValue ? def.lostValue : def.baseValue);
        list.append(entry);
    }
    for (const BonusDef& def : rules.bonuses()) {
        QVariantMap entry;
        entry.insert(QStringLiteral("kind"), QStringLiteral("bonus"));
        entry.insert(QStringLiteral("key"), QString::fromLatin1(def.key));
        entry.insert(QStringLiteral("id"), static_cast<int>(def.id));
        entry.insert(QStringLiteral("name"), ReasonText::bonusName(static_cast<int>(def.id)));
        entry.insert(QStringLiteral("silent"), def.silent);
        entry.insert(QStringLiteral("announced"), def.announced);
        entry.insert(QStringLiteral("targetTrick"), static_cast<int>(def.targetTrick));
        list.append(entry);
    }
    return list;
}

// --- §11.3 and §11.5 ----------------------------------------------------------------

QVariantMap RulesIndex::contractPurpose(ProfileId profile, ContractId contract)
{
    QVariantMap map;
    for (const PurposeRow& row : kContractPurposeKr) {
        if (row.id != static_cast<int>(contract))
            continue;
        if (row.profile != profile && profile == ProfileId::HuIlluItvb2019)
            continue;   // the Hungarian table fills this in later
        map.insert(QStringLiteral("id"), static_cast<int>(contract));
        map.insert(QStringLiteral("name"), ReasonText::contractName(static_cast<int>(contract)));
        map.insert(QStringLiteral("purpose"), tr(row.purpose));
        map.insert(QStringLiteral("when"), tr(row.when));
        map.insert(QStringLiteral("anchor"), anchorFor(profile, QStringLiteral("3.1")));
        break;
    }
    return map;
}

QVariantMap RulesIndex::bonusPurpose(ProfileId profile, BonusId bonus)
{
    QVariantMap map;
    for (const PurposeRow& row : kBonusPurposeKr) {
        if (row.id != static_cast<int>(bonus))
            continue;
        if (row.profile != profile && profile == ProfileId::HuIlluItvb2019)
            continue;
        map.insert(QStringLiteral("id"), static_cast<int>(bonus));
        map.insert(QStringLiteral("name"), ReasonText::bonusName(static_cast<int>(bonus)));
        map.insert(QStringLiteral("purpose"), tr(row.purpose));
        map.insert(QStringLiteral("when"), tr(row.when));
        map.insert(QStringLiteral("anchor"), anchorFor(profile, QStringLiteral("5.1")));
        break;
    }
    return map;
}

QVariantList RulesIndex::commonMistakes(ProfileId profile)
{
    QVariantList list;
    for (const MistakeRow& row : kMistakesKr) {
        QVariantMap entry;
        entry.insert(QStringLiteral("number"), row.number);
        entry.insert(QStringLiteral("title"), tr(row.title));
        entry.insert(QStringLiteral("text"), row.text[0] == '\0' ? QString() : tr(row.text));
        entry.insert(QStringLiteral("anchor"), anchorFor(profile, QString::fromLatin1(row.anchor)));
        list.append(entry);
    }
    return list;
}

QVariantMap RulesIndex::mistake(ProfileId profile, int number)
{
    const QVariantList all = commonMistakes(profile);
    for (const QVariant& item : all) {
        const QVariantMap entry = item.toMap();
        if (entry.value(QStringLiteral("number")).toInt() == number)
            return entry;
    }
    return QVariantMap();
}
