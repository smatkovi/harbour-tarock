# Regeltexte des Nachschlagewerks

Eine Datei je Regelprofil, erzeugt von `tools/make-rules.py` aus den
Regelwerken in `docs/` (Kapitel 1 bis 9, die Spielregeln). Nicht von Hand
ändern -- Änderungen gehören in `docs/*.md`, danach den Erzeuger laufen
lassen.

| Datei | Quelle | Profil |
|---|---|---|
| `at-kr-ooe.de.txt` | `docs/koenigrufen.md` | `AT-KR-OOE-2023` |
| `hu-illu.de.txt` | `docs/hungarian.md` | `HU-ILLU-ITVB-2019` |
| `at-tapp.de.txt` | `docs/tapptarock.md` | `AT-TAPP-KLASSIK` |
| `at-stroh.de.txt` | `docs/strohmandeln.md` | `AT-STROH-MS-ERW` |

## Format

UTF-8, zeilenweise. Vor dem ersten Kapitel stehen Kommentarzeilen mit `#`.
Jedes Kapitel beginnt mit einer Kopfzeile

    @ <nummer> <ebene> <titel>

(`6.2`, Ebene `1` = Kapitel oder `2` = Abschnitt), darauf folgt bis zur
nächsten Kopfzeile der Text als Qt-Rich-Text: `p`, `b`, `i`, `ul`, `ol`,
`li`, `br`. Tabellen der Vorlage sind Listen, eine Zeile je Eintrag;
Codeblöcke und Absätze an den Programmierer fehlen.

`RulesIndex::chapters()` liest die Datei (`src/RulesIndex.cpp`) und sucht
sie unter `/usr/share/harbour-tarock/rules/` (Sailfish),
`/opt/harbour-tarock/assets/rules/` (MeeGo) und `:/rules/` (Android);
`tests/test_rules.cpp` prüft, dass jeder Anker der Erklärtexte ein Kapitel
mit Inhalt findet.
