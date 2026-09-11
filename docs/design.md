# harbour-tarock – Architektur und Implementierungsplan

**Stand:** 12.09.2026
**Zielplattformen:** Sailfish OS (Qt 5.6 / QtQuick 2.6 / Silica) und Android (Qt 6.9+, arm64)
**Sprache des Dokuments:** Deutsch; Bezeichner im Code englisch, Regelbegriffe im Original.

**Eingaben dieses Dokuments**

| Quelle | Was daraus übernommen wird |
|---|---|
| `tarock-spec/koenigrufen.md` (Profil `AT-KR-OOE-2023-04`) | Regelprofil 1: Karten, Lizitation, Talon, Prämien, Stichregeln, Abrechnung, Lernmodus §11, KI §10 |
| `tarock-spec/hungarian.md` (Profil `HU-ILLU-ITVB-2019`) | Regelprofil 2: 42-Blatt, Lizit, Talonverteilung, Figuren, gebundene Karten, Abrechnung, Lernmodus §11 |
| `rpms/tarock-research.json` → `result.artwork` | Kartengrafiken: IuG-Scan Volkskundemuseum Wien (PDM 1.0) + generiertes SVG-Deck, Lizenz- und Retusche-Auflagen |
| `harbour-snapszer` (Branch `android`) | Wiederverwendete Muster: deterministischer Kern, QObject-Engine, host-autoritatives LAN, geteiltes ES5-QML, Sailfish-/Android-Buildpaare |
| `tarock-assets/manifest.json` (bereits geladen, 55 Canvases) | Startpunkt der Asset-Pipeline |

Die beiden weiteren Spezifikationen (`tapptarock.md`, `strohmandeln.md`) sind **nicht** Teil dieser Ausbaustufe,
die Profil-Abstraktion ist aber so geschnitten, dass beide später ohne Kernänderung dazukommen (§4.3, §12).

---

## 1. Ziele, Nicht-Ziele, Leitentscheidungen

### 1.1 Ziele

1. Zwei vollständige, quellengeprüfte Regelprofile (Königrufen OÖ, Ungarisches Tarokk Illusztrált) in **einer** Engine.
2. Vierertisch **und** Fünfertisch (Geber setzt aus) in beiden Profilen.
3. Computergegner mit erklärbaren Entscheidungen.
4. LAN für 2–5 Geräte, Sailfish und Android gemischt, host-autoritativ.
5. **Lernmodus** als gleichwertiges Produktmerkmal, nicht als Hilfetext (§7).
6. Zwei Kartendecks: historisches IuG-Deck und generiertes klares Deck.
7. Eine Codebasis, zwei Builds; nur Plattform-QML ist doppelt.

### 1.2 Nicht-Ziele (bewusst ausgeschlossen)

- Kein Online-Matchmaking, kein Konto, keine Telemetrie. LAN und direkte IPv6-Adresse genügen (wie Snapszer 1.1).
- Kein Renonce-Regime: Die App verhindert unzulässige Züge, statt sie zu bestrafen (`koenigrufen.md` §3.7, `hungarian.md` §6.7).
- Kein Chat während des Spiels, außer dem Gegner-Team-Chat im Ouvertspiel (`koenigrufen.md` §6.4).
- Kein Tarot Nouveau (78 Blatt) und kein French Tarot in dieser Ausbaustufe.

### 1.3 Leitentscheidungen

| # | Entscheidung | Begründung |
|---|---|---|
| L1 | Regelkern ohne Qt, C++17, deterministisch, `serializeState`/`restoreState`/`rotateSeats` wie `MultiCore` | Testbarkeit, LAN-Replay, Wiederaufnahme |
| L2 | Regelprofil = **Datenstruktur** (Tabellen + Flags) plus eine schlanke Regel-Subklasse je Familie | `hungarian.md` §0.3 verlangt Profile ohne `#ifdef`; Lizitautomaten sind zu verschieden für reine Daten |
| L3 | Jede abgelehnte Aktion liefert einen **maschinenlesbaren `ReasonCode`** mit Parametern, nie nur `false` | Fundament des Lernmodus; ersetzt `MultiCore::isLegal() -> bool` |
| L4 | Der Host sendet **je Sitz gefilterte** Zustände, nicht den vollen Zustand | `koenigrufen.md` §2.7/§8.4, `hungarian.md` §2.6: verdeckte Information darf den Client nie erreichen (Abweichung von Snapszer) |
| L5 | Ablage/Skart wird als Folge von `Discard`-Toggles plus `ConfirmDiscard` modelliert | Jede einzelne verbotene Karte kann begründet werden; LAN-Replay bleibt kartengenau |
| L6 | Der Hinweisgeber ist die Bewertungsfunktion des Computerspielers | „Hint“ und „Gegner“ dürfen nie auseinanderlaufen; `koenigrufen.md` §10.1 |
| L7 | Übungspartien sind **Daten** (JSON) und zugleich **Regressionstests** | `koenigrufen.md` §11.7 / §10.10, `hungarian.md` §11.8 |
| L8 | Eckindizes werden im QML gezeichnet, nicht in die Bilder gebacken | `artwork` §5.4 – Decks bleiben austauschbar |

---

## 2. Repository-Layout

```
harbour-tarock/
├─ CMakeLists.txt                     # Sailfish-Build (Qt 5.6)
├─ harbour-tarock.spec                # RPM
├─ LICENSE                            # GPL-3.0-or-later (Code)
├─ README.md
├─ CREDITS/
│  ├─ ASSETS.md                       # Bild-, Schrift- und Quellennachweise (§10.6)
│  └─ licenses/                       # Lizenz-Snapshots (METS, Commons-Wikitext) je Quelle
├─ src/
│  ├─ core/                           # Qt-freier Regelkern
│  │  ├─ Card.h                       # Card, Suit, CardSet, Kartennamen
│  │  ├─ Deck.h/.cpp                  # DeckSpec, Mischen, Geben
│  │  ├─ Action.h                     # Action, ActionType
│  │  ├─ Reason.h                     # ReasonCode, Reason, Severity
│  │  ├─ RuleProfile.h/.cpp           # ContractDef, BonusDef, FlagSet, ProfileRegistry
│  │  ├─ profiles/
│  │  │  ├─ AtKrOoe2023.cpp           # Tabellen AT-KR-OOE-2023-04
│  │  │  ├─ HuIlluItvb2019.cpp        # Tabellen HU-ILLU-ITVB-2019
│  │  │  └─ HuPaskievics.cpp          # Flag-Overlay (§12 hungarian.md)
│  │  ├─ TarockRules.h/.cpp           # abstrakte Basis + gemeinsame Stichlogik
│  │  ├─ KoenigrufenRules.h/.cpp
│  │  ├─ HungarianRules.h/.cpp
│  │  ├─ TarockCore.h/.cpp            # Zustandsautomat, Serialisierung, Rotation
│  │  ├─ Scoring.h/.cpp               # Posten, Ledger, Schrift/Geld
│  │  ├─ Ai.h                         # AiPlayer-Interface, Hint
│  │  ├─ AiHeuristic.cpp              # Lizit/Ruf/Skart/Ansage/Spiel-Heuristik + Begründungen
│  │  ├─ AiSearch.cpp                 # PIMC/ISMCTS für das Kartenspiel
│  │  ├─ Inference.h/.cpp             # Informationsmenge, Voids, Determinisierung
│  │  └─ Lesson.h/.cpp                # Lektions-/Übungspartie-Modell + Loader
│  ├─ TarockEngine.h/.cpp             # QObject-Fassade für QML (Animationsphasen, KI-Timer)
│  ├─ LearnEngine.h/.cpp              # Lernmodus-Fassade (Erklärung, Warum, Hinweis, Lektion)
│  ├─ ReasonText.cpp                  # ReasonCode -> tr()-Texte, je Profil
│  ├─ RulesIndex.h/.cpp               # Regel-Nachschlagewerk + Glossar (Suchindex)
│  ├─ Settings.h/.cpp                 # QSettings-Wrapper, Profil- und Lernstufenwahl
│  ├─ LanSession.h/.cpp               # aus harbour-snapszer übernommen, unverändert
│  └─ LanTable.h/.cpp                 # Lobby, Sitzvergabe, Redirect, Sichtfilter (§8)
├─ qml-common/                        # geteiltes QML, QtQuick 2.6 + ES5
│  ├─ TarockTable.qml                 # Tisch 4/5 Spieler, Portrait
│  ├─ HandFan.qml  SeatPanel.qml  TrickArea.qml
│  ├─ BidBar.qml  CallPanel.qml  TalonPanel.qml  DiscardTray.qml
│  ├─ AnnouncePanel.qml  KontraRow.qml
│  ├─ LearnPanel.qml  WhyDialog.qml  HintBubble.qml  StepBanner.qml
│  ├─ ScoreSheet.qml  CountTutor.qml
│  └─ RulesBrowser.qml  GlossaryList.qml  TariffTable.qml
├─ sailfish/
│  ├─ harbour-tarock.qml  qmldir      # qmldir mit `singleton Style 1.0 Style.qml`
│  ├─ Style.qml  Card.qml  FlyingCard.qml  TableButton.qml  SubPage.qml
│  ├─ TablePage.qml  LessonPage.qml  RulesPage.qml  SettingsPage.qml
│  ├─ LanPage.qml  ScorePage.qml  AboutPage.qml  CreditsPage.qml  CoverPage.qml
│  ├─ desktop/harbour-tarock.desktop  # inkl. [X-Sailjail] Permissions=Internet
│  └─ icons/icon-{86,108,128,172,256}.png
├─ android/
│  ├─ CMakeLists.txt  main.cpp  ScreenHelper.h
│  ├─ qml/  (Main.qml, Theme.qml, Style.qml, Card.qml, … Spiegel von sailfish/)
│  └─ package/
│     ├─ AndroidManifest.xml
│     ├─ res/drawable/icon.png
│     └─ src/org/smatkovi/tarock/LanService.java   # Vordergrunddienst + Wi-Fi-/Wake-Lock
├─ lessons/
│  ├─ schema.json                     # JSON-Schema der Lektionen (§7.6)
│  ├─ at-kr-ooe/  L0…L7.json, practice-1.json … practice-3.json
│  └─ hu-illu/    L0…L7.json, practice-1.json … practice-3.json
├─ rules/                             # Regel-Nachschlagewerk in der App
│  ├─ at-kr-ooe.de.md   hu-illu.de.md
│  └─ glossary.de.tsv                 # Begriff \t Kurzdefinition \t Anker
├─ assets/
│  └─ decks/
│     ├─ iug1904/  card-*.jpg  back.jpg  deck.json  LICENSE
│     └─ clean54/  card-*.png  back.png  deck.json
├─ tools/assets/                      # Build-Zeit, nicht Laufzeit (§10)
│  ├─ fetch_iiif.py  mapping.csv  retouch.py  render_deck.py
│  ├─ masks/                          # Retuschemasken je Karte
│  ├─ gen_clean_deck.py  clean_deck.yaml
│  └─ verify_deck.py
├─ tests/
│  ├─ test_cards.cpp  test_profile_kr.cpp  test_profile_hu.cpp
│  ├─ test_bidding.cpp  test_legality.cpp  test_scoring.cpp
│  ├─ test_lessons.cpp  test_selfplay.cpp  test_rotation.cpp  test_serialize.cpp
│  ├─ lan_tarock4.cpp  lan_tarock5.cpp
│  └─ data/                           # erwartete Ledger je Beispiel aus den Specs
└─ translations/
   ├─ harbour-tarock-de.ts  harbour-tarock-hu.ts  harbour-tarock-en.ts
```

**Sprachpolitik:** Quelltexte in `tr()` sind **deutsch** (die Regelspezifikationen sind deutsch, `koenigrufen.md`
§11.4 und `hungarian.md` §11.6 geben den finalen Wortlaut vor). `harbour-tarock-en.ts` und `-hu.ts` sind
Übersetzungen. Ungarische Fachbegriffe bleiben in allen Sprachen stehen (`hungarian.md` §0.4).

---

## 3. Build

### 3.1 Sailfish OS – `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.6)
project(harbour-tarock LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Core LinguistTools Network Qml Quick)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SAILFISHAPP REQUIRED sailfishapp)

set(TAROCK_CORE_SOURCES
    src/core/Deck.cpp src/core/RuleProfile.cpp
    src/core/profiles/AtKrOoe2023.cpp src/core/profiles/HuIlluItvb2019.cpp
    src/core/profiles/HuPaskievics.cpp
    src/core/TarockRules.cpp src/core/KoenigrufenRules.cpp src/core/HungarianRules.cpp
    src/core/TarockCore.cpp src/core/Scoring.cpp
    src/core/AiHeuristic.cpp src/core/AiSearch.cpp src/core/Inference.cpp
    src/core/Lesson.cpp)

add_executable(harbour-tarock
    main.cpp ${TAROCK_CORE_SOURCES}
    src/TarockEngine.cpp src/LearnEngine.cpp src/ReasonText.cpp
    src/RulesIndex.cpp src/Settings.cpp src/LanSession.cpp src/LanTable.cpp)

target_include_directories(harbour-tarock PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src ${SAILFISHAPP_INCLUDE_DIRS})
target_link_libraries(harbour-tarock
    Qt5::Core Qt5::Network Qt5::Qml Qt5::Quick ${SAILFISHAPP_LIBRARIES})

set(TAROCK_TRANSLATIONS
    translations/harbour-tarock-hu.ts translations/harbour-tarock-en.ts)
qt5_add_translation(TAROCK_QM_FILES ${TAROCK_TRANSLATIONS})
add_custom_target(harbour-tarock-translations ALL DEPENDS ${TAROCK_QM_FILES})
add_dependencies(harbour-tarock harbour-tarock-translations)

option(TAROCK_BUILD_TESTS "Build rule, lesson and LAN tests" OFF)
if(TAROCK_BUILD_TESTS)
    enable_testing()
    foreach(t test_cards test_profile_kr test_profile_hu test_bidding test_legality
              test_scoring test_lessons test_selfplay test_rotation test_serialize)
        add_executable(${t} tests/${t}.cpp ${TAROCK_CORE_SOURCES})
        target_include_directories(${t} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_compile_definitions(${t} PRIVATE TAROCK_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
        add_test(NAME ${t} COMMAND ${t})
    endforeach()
    set(TAROCK_ENGINE_SOURCES ${TAROCK_CORE_SOURCES}
        src/TarockEngine.cpp src/LearnEngine.cpp src/ReasonText.cpp
        src/RulesIndex.cpp src/Settings.cpp src/LanSession.cpp src/LanTable.cpp)
    foreach(t lan_tarock4 lan_tarock5)
        add_executable(${t} tests/${t}.cpp ${TAROCK_ENGINE_SOURCES})
        target_include_directories(${t} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_link_libraries(${t} Qt5::Core Qt5::Gui Qt5::Network Qt5::Qml)
        add_test(NAME ${t} COMMAND ${t})
    endforeach()
endif()
```

`main.cpp` registriert die Kontexteigenschaften `tarockEngine`, `learnEngine`, `lanBrowser`, `appSettings`
(wie `main.cpp` von Snapszer die `snapszerEngine`/`multiEngine`/`lanBrowser` registriert).

### 3.2 `harbour-tarock.spec`

Wie `harbour-snapszer.spec`, zusätzlich installiert werden `lessons/`, `rules/`, `assets/decks/` und
`CREDITS/`:

```spec
Name: harbour-tarock
Version: 0.1.0
Release: 1
Summary: Königrufen and Hungarian Tarokk with a learning mode
License: GPL-3.0-or-later
Requires:       sailfishsilica-qt5
BuildRequires:  pkgconfig(sailfishapp) pkgconfig(Qt5Core) pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Qml) pkgconfig(Qt5Quick) qt5-qttools-linguist

%install
…
mkdir -p %{buildroot}/usr/share/%{name}/{qml,assets,lessons,rules,translations}
cp -a sailfish/*.qml sailfish/qmldir qml-common/*.qml %{buildroot}/usr/share/%{name}/qml/
cp -a sailfish/icons                                  %{buildroot}/usr/share/%{name}/qml/
cp -a assets/decks   %{buildroot}/usr/share/%{name}/assets/
cp -a lessons rules  %{buildroot}/usr/share/%{name}/
install -m 644 CREDITS/ASSETS.md %{buildroot}/usr/share/doc/%{name}/
install -m 644 sailfish/desktop/%{name}.desktop %{buildroot}/usr/share/applications/%{name}.desktop
```

`sailfish/desktop/harbour-tarock.desktop`:

```ini
[Desktop Entry]
Name=Tarock
Icon=harbour-tarock
Exec=harbour-tarock
Type=Application
Categories=Game;
X-Sailfish-ApplicationType=Silica

[X-Sailjail]
Permissions=Internet
OrganizationName=org.smatkovi
ApplicationName=harbour-tarock
```

`Permissions=Internet` ist die einzige benötigte Sailjail-Berechtigung (TCP 45465 / UDP 45466 wie in
`LanSession`). Kartenbilder, Lektionen und Regeltexte liegen im read-only Anwendungsverzeichnis, dafür ist
keine zusätzliche Berechtigung nötig; `QSettings` schreibt in den von Sailjail bereitgestellten privaten Pfad.

### 3.3 Android – `android/CMakeLists.txt`

Struktur wie bei Snapszer; die Unterschiede sind markiert:

```cmake
cmake_minimum_required(VERSION 3.21)
project(tarock VERSION 0.1.0 LANGUAGES CXX)
find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Network Qml Quick QuickControls2)
qt_standard_project_setup(REQUIRES 6.9)

set(TAROCK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
qt_add_executable(tarock main.cpp ScreenHelper.h ${TAROCK_CORE_SOURCES} …)

set_source_files_properties(qml/Theme.qml qml/Style.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
set(TAROCK_SHARED_QML
    ${TAROCK_ROOT}/qml-common/TarockTable.qml   ${TAROCK_ROOT}/qml-common/HandFan.qml
    ${TAROCK_ROOT}/qml-common/SeatPanel.qml     ${TAROCK_ROOT}/qml-common/TrickArea.qml
    ${TAROCK_ROOT}/qml-common/BidBar.qml        ${TAROCK_ROOT}/qml-common/CallPanel.qml
    ${TAROCK_ROOT}/qml-common/TalonPanel.qml    ${TAROCK_ROOT}/qml-common/DiscardTray.qml
    ${TAROCK_ROOT}/qml-common/AnnouncePanel.qml ${TAROCK_ROOT}/qml-common/KontraRow.qml
    ${TAROCK_ROOT}/qml-common/LearnPanel.qml    ${TAROCK_ROOT}/qml-common/WhyDialog.qml
    ${TAROCK_ROOT}/qml-common/HintBubble.qml    ${TAROCK_ROOT}/qml-common/StepBanner.qml
    ${TAROCK_ROOT}/qml-common/ScoreSheet.qml    ${TAROCK_ROOT}/qml-common/CountTutor.qml
    ${TAROCK_ROOT}/qml-common/RulesBrowser.qml  ${TAROCK_ROOT}/qml-common/GlossaryList.qml
    ${TAROCK_ROOT}/qml-common/TariffTable.qml)
foreach(file ${TAROCK_SHARED_QML})
    get_filename_component(name ${file} NAME)
    set_source_files_properties(${file} PROPERTIES QT_RESOURCE_ALIAS ${name})
endforeach()
qt_add_qml_module(tarock URI Tarock QML_FILES qml/Main.qml qml/Theme.qml qml/Style.qml … ${TAROCK_SHARED_QML})

# Kartenbilder, Lektionen und Regeltexte als Ressourcen (Android hat kein /usr/share)
file(GLOB_RECURSE TAROCK_DECKS "${TAROCK_ROOT}/assets/decks/*")
file(GLOB_RECURSE TAROCK_LESSONS "${TAROCK_ROOT}/lessons/*.json")
file(GLOB_RECURSE TAROCK_RULES "${TAROCK_ROOT}/rules/*")
qt_add_resources(tarock tarock_data PREFIX "/" BASE "${TAROCK_ROOT}"
    FILES ${TAROCK_DECKS} ${TAROCK_LESSONS} ${TAROCK_RULES}
          "${TAROCK_ROOT}/sailfish/icons/icon-256.png")

find_program(TAROCK_LRELEASE NAMES lrelease-qt6 lrelease
             HINTS "${QT_HOST_PATH}/lib/qt6/bin" "${QT_HOST_PATH}/bin" REQUIRED)
# … je .ts eine add_custom_command wie in harbour-snapszer …

set_target_properties(tarock PROPERTIES
    QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/package"
    QT_ANDROID_PACKAGE_NAME "org.smatkovi.tarock"
    QT_ANDROID_APP_NAME "Tarock"
    QT_ANDROID_APP_ICON "@drawable/icon"
    QT_ANDROID_MIN_SDK_VERSION 28
    QT_ANDROID_TARGET_SDK_VERSION 36
    QT_ANDROID_VERSION_CODE 1
    QT_ANDROID_VERSION_NAME "0.1.0")

if(ANDROID)
    qt_add_android_permission(tarock NAME android.permission.ACCESS_WIFI_STATE)
    qt_add_android_permission(tarock NAME android.permission.CHANGE_WIFI_MULTICAST_STATE)
    qt_add_android_permission(tarock NAME android.permission.FOREGROUND_SERVICE)
    qt_add_android_permission(tarock NAME android.permission.FOREGROUND_SERVICE_SPECIAL_USE)
    qt_add_android_permission(tarock NAME android.permission.POST_NOTIFICATIONS)
    qt_add_android_permission(tarock NAME android.permission.WAKE_LOCK)
endif()
```

`android/package/src/org/smatkovi/tarock/LanService.java` ist die umbenannte `LanService.java` aus Snapszer
(`foregroundServiceType="specialUse"`, `PROPERTY_SPECIAL_USE_FGS_SUBTYPE` = „Hosting and playing a card game
with other devices in the local network“), gestartet aus `LanTable`, sobald gehostet oder beigetreten wird.

**Ressourcenpfade:** `Paths::data(const QString& relative)` (in `src/Settings.h`) liefert auf Sailfish
`SailfishApp::pathTo("…")`, auf Android `qrc:/…`. Nur diese eine Funktion kennt den Unterschied.

---

## 4. C++-Kern

### 4.1 Kartenmodell – `src/core/Card.h`

Ein Kartensatz für beide Profile: Das ungarische 42-Blatt ist eine **Teilmenge** des 54-Blatts.

```cpp
namespace tarock {

enum class Suit : std::uint8_t { Heart = 0, Diamond = 1, Spade = 2, Club = 3, Tarock = 4 };

// Karten-Id 0..53, stabil über Profile, Serialisierung und Netz hinweg:
//   0..21  Tarock:       0 = I, 1 = II, … 19 = XX, 20 = XXI, 21 = Sküs/Skíz
//   22..53 Farbkarten:   22 + 8*suit + slot,  slot 0..7
//          slot 0 = König, 1 = Dame, 2 = Reiter/lovas, 3 = Bube,
//          slot 4..7 = die vier Skartins absteigend
//          rot:     slot 4 = As(1), 5 = 2, 6 = 3, 7 = 4
//          schwarz: slot 4 = 10,    5 = 9, 6 = 8, 7 = 7
struct Card {
    std::uint8_t id = 0xFF;
    constexpr bool valid()   const { return id < 54; }
    constexpr bool tarock()  const { return id < 22; }
    constexpr Suit suit()    const { return tarock() ? Suit::Tarock : Suit((id - 22) / 8); }
    constexpr int  slot()    const { return tarock() ? -1 : (id - 22) % 8; }
    constexpr int  tarockNo()const { return tarock() ? id + 1 : 0; }   // Sküs = 22
    constexpr bool king()    const { return !tarock() && slot() == 0; }
    constexpr bool honour()  const { return id == 0 || id == 20 || id == 21; } // I, XXI, Sk = Trull/honőr
    constexpr bool bird()    const { return id <= 3; }                 // I..IIII (nur KR)
};

// Stichkraft innerhalb der eigenen "Farbe": Tarock 1..22, Farbe 8..1 (König am höchsten).
constexpr int power(Card c) { return c.tarock() ? c.tarockNo() : 8 - c.slot(); }

using CardSet = std::bitset<54>;          // Hände, Talon, Ablage, erlaubte Karten, „reserviert“
using CardList = std::vector<Card>;       // geordnete Folgen (Stich, Abwurfreihenfolge)

const char* cardKey(Card);                // "XXI", "Sk", "H:K", "S:10", "D:1"  – stabil, für Lektionen/Tests
bool parseCardKey(std::string_view, Card&);

} // namespace tarock
```

Das ungarische 42-Blatt entsteht durch `DeckSpec{ .suitSlots = 5 }`: Slots 5–7 jeder Farbe entfallen; übrig
bleibt als niedrigste Karte rot das As, schwarz die Zehn – genau `hungarian.md` §1.3. `red_low_card = four`
ändert nur die Beschriftung, nicht die Id.

```cpp
struct DeckSpec {
    std::uint8_t tarocks   = 22;   // immer 22
    std::uint8_t suitSlots = 8;    // 8 = Königrufen (54), 5 = Ungarisch (42)
    CardSet cards() const;         // Bitmaske des Decks
    int size() const { return tarocks + 4 * suitSlots; }
};
```

### 4.2 Punkte und Zählung

```cpp
enum class CountMode : std::uint8_t { ThirdsAustrian, PlainHungarian };

struct CountResult { int units = 0; int points() const; int blatt() const; };   // KR: Drittel; HU: Punkte

// KR  (koenigrufen.md §1.5):  units = 3*Σ wert(c) − 2*|M|      Schwelle 107 (= 35/2)
// HU  (hungarian.md  §1.5):   units =   Σ wert(c)              Schwelle 48 / 47 / 71 / volát
CountResult RuleProfile::count(const CardSet& cards) const;
```

`RuleProfile::cardValue(Card)` ist eine 54-Byte-Tabelle je Profil (KR: Könige/Sk/XXI/I = 5, D 4, R 3, B 2,
Rest 1; HU: identisch, aber nur 42 Karten → Summe 94).

### 4.3 Regelprofil-Abstraktion – `src/core/RuleProfile.h`

**Daten** (dort, wo die Profile sich nur in Werten unterscheiden):

```cpp
enum class TalonMode : std::uint8_t {
    None,          // kein Talon im Spiel (Negativspiele)
    OpenHalves,    // beide Hälften offen, eine wird genommen        (KR: Rufer, Besserrufer, Dreier, Farbendreier)
    AllHidden,     // alle Talonkarten verdeckt an den Spieler       (KR: Sechserdreier)
    ToOpponents,   // Talon bleibt liegen, zählt zu den Gegnern      (KR: Solospiele)
    ToLastTrick,   // Talon an den Gewinner des letzten Stichs       (KR: Trischaken)
    Distributed    // Talon wird auf alle vier verteilt              (HU: 3/1/1/1 … 0/2/2/2)
};
enum class PartnerMode  : std::uint8_t { Alone, CallKing, CallTarock };
enum class PlayMode     : std::uint8_t { Positive, SuitGame, Negative, Trischaken };
enum class LeadRule     : std::uint8_t { Forehand, Declarer };
enum class KontraMode   : std::uint8_t { Party, Individual, None };

struct ContractDef {
    ContractId  id;  const char* key;          // "RUFER" … "SOLODREIER" | "HAROM" … "SZOLO"
    int   rank;                                // Lizitationsrang (KR 1..13, HU 1..4)
    int   baseValue;                           // Schriftwert / Grundwert g
    int   lostValue;                           // abweichender Verlustwert (Sechserdreier 8), sonst = baseValue
    TalonMode talon;  std::int8_t talonToDeclarer;   // 3 | 6 | 0 | HU: 3/2/1/0
    std::int8_t discardCount;                  // 3 | 6 | 0
    PartnerMode partner; PlayMode play; LeadRule firstLead; KontraMode kontra;
    std::uint32_t bonusMask;                   // erlaubte Prämien/Figuren
    bool  soloDoublesBonuses;                  // KR: Solorufer/Solodreier/Farbensolo
    std::uint16_t bidFlags;                    // s. u.
};
// bidFlags: ForehandOnlyOpening | ForehandOnlyAfterAllPass | RequiresBird | ForbidsFourKings
//           | RequiresHonour | AnnouncementsWithBid | DiscardTarockFirst

enum class BonusKind : std::uint8_t {
    Holding,        // Bestandsprämie: Karten am Ende in den Stichen (Trull, Alle Könige, tulétroá, négykirály)
    TrickWithCard,  // bestimmte Karte gewinnt einen bestimmten Stich (Pagat/Uhu/Kakadu/Quapil, ultimó, uhu)
    TrickRun,       // die ersten n Stiche, der n-te mit einer bestimmten Karte (centrum, kismadár, nagymadár)
    PointTarget,    // Punktziel (duplajáték ≥ 71)
    AllTricks,      // Valat / volát
    Capture,        // XXI-fogás, pagátfogás
    TarockCount     // tarokkszám 8/9
};

struct BonusDef {
    BonusId id;  const char* key;              // "TRULL" "PAGAT" "KOENIG_ULTIMO" | "TULETROA" "PAGAT_ULTIMO" "XXI_FOGAS"
    BonusKind kind;
    std::int8_t targetTrick;                   // KR 12/11/10/9, HU 9/8/7/6/5; -1 = n/a
    CardRef   boundCard;                       // feste Karte, CalledKing, NamedKing, HighestTarock
    int announced;  int silent;                // Tarif; silent < 0 → nur angesagt
    std::int8_t timesG;                        // 0 = fester Wert; sonst Faktor auf den Grundwert (HU dupla 4/2)
    std::uint16_t flags;                       // Binding | DoubleInSolo | RequiresCardInHand | KilledByVolat
                                               // | PartyBound | MustAnnounceWhenBid
};

struct RuleProfile {
    ProfileId id;  const char* key;            // "AT-KR-OOE-2023-04" | "HU-ILLU-ITVB-2019" | "HU-PASK-ITVB"
    DeckSpec deck;
    CountMode count;  int winThreshold;  int defendersThreshold;   // KR 107/107, HU 48/47
    int handSize; int tricks; int talonSize;                       // KR 12/12/6, HU 9/9/6
    std::vector<ContractDef> contracts;
    std::vector<BonusDef>    bonuses;
    std::array<std::uint8_t, 54> value;
    FlagSet flags;                             // ⚙-Flags der Specs, String -> int
    std::vector<Phase> phaseOrder;             // für Fortschrittsanzeige und Lernpanel
    const char* rulesDoc;                      // "rules/at-kr-ooe.de.md"
};

class ProfileRegistry {
public:
    static const RuleProfile& get(ProfileId);
    static std::vector<ProfileId> all();
    static std::unique_ptr<TarockRules> makeRules(ProfileId, const FlagSet& overrides);
};
```

`FlagSet` bildet jeden in den Specs mit **⚙ Flag** markierten Streitpunkt ab, z. B.
`hold_rule = new|old`, `uhu_value = 15|20`, `sas_figures`, `volat_kills_silent_bonuses`,
`dupla_volat_same_turn`, `ulti_to_uhu`, `free_call_excludes_honours`, `pagatfogas`, `tie_rule`
(alle `hungarian.md`), sowie `harteVogelreservierung`, `kontra_zaehlt_in_schrift`, `doppeltes_radl`,
`zwiccolo`, `kaiserstich` (alle `koenigrufen.md` §6.5/§8.3). Jedes Flag erscheint in den Einstellungen
und wird beim LAN-Beitritt angezeigt (`koenigrufen.md` §8.3).

**Code** (dort, wo die Profile strukturell verschieden sind): `TarockRules` als schmale Schnittstelle.

```cpp
class TarockRules {
public:
    virtual ~TarockRules() = default;
    const RuleProfile& profile() const;

    // --- Lizitation -------------------------------------------------------
    virtual void   startBidding(TarockCore&) const = 0;
    virtual void   legalBids(const TarockCore&, int seat, std::vector<Action>& out) const = 0;
    virtual Reason checkBid(const TarockCore&, int seat, const Action&) const = 0;
    virtual void   applyBid(TarockCore&, int seat, const Action&) const = 0;
    virtual bool   biddingFinished(const TarockCore&) const = 0;

    // --- Partnerruf / Talon / Ablage --------------------------------------
    virtual void   legalCalls(const TarockCore&, std::vector<Action>&) const = 0;
    virtual Reason checkCall(const TarockCore&, const Action&) const = 0;
    virtual void   distributeTalon(TarockCore&) const = 0;
    virtual Reason checkDiscard(const TarockCore&, int seat, Card, const CardSet& tray) const = 0;
    virtual CardSet discardableCards(const TarockCore&, int seat) const = 0;
    virtual CardSet openDiscards(const TarockCore&, int seat) const = 0;   // offen zu zeigen

    // --- Ansagen und Kontra ----------------------------------------------
    virtual void   legalAnnouncements(const TarockCore&, int seat, std::vector<Action>&) const = 0;
    virtual Reason checkAnnouncement(const TarockCore&, int seat, const Action&) const = 0;
    virtual bool   announcementsFinished(const TarockCore&) const = 0;

    // --- Kartenspiel (gemeinsame Implementierung in TarockRules) ----------
    virtual CardSet legalCards(const TarockCore&, int seat) const;         // koenigrufen.md §6.7
    virtual Reason  checkCard(const TarockCore&, int seat, Card) const;
    virtual Reason  warnCard(const TarockCore&, int seat, Card) const;     // erlaubt, aber verlustbringend
    virtual int     trickWinner(const TarockCore&) const;                  // koenigrufen.md §6.6
    virtual bool    handFinished(const TarockCore&) const;                 // §6.8 vorzeitiges Ende

    // --- Abrechnung -------------------------------------------------------
    virtual Ledger  settle(const TarockCore&) const = 0;                   // §7.11 / hungarian §7.2
    virtual CardSet ownedBy(const TarockCore&, Party) const = 0;           // §1.8 Talon-/Ablagezuordnung

    // --- Sichtbarkeit (LAN und Fünfter) -----------------------------------
    virtual bool    visibleTo(const TarockCore&, int viewer, Visible what, int subject) const;
};
```

`KoenigrufenRules` implementiert den Automaten aus `koenigrufen.md` §3.5 (Vorhand, halten, Sechser nur
vorneweg, Rufer/Trischaken nur hintennach, Endebedingung C für den Solodreier) und den Ansage-/Kontra-Automaten
aus §5.7. `HungarianRules` implementiert §3.8 (`BidState` mit `highestHeld`, `invit`, `inviterLocked`,
`próbahárom`), die Talonverteilung §4.1, `bedobás` §4.2, den `XX-hívás` §4.5 samt „hivatalból kontra“ und
die Ansagerunde §5.1/§5.5.

Die **gemeinsame** Stichlogik in `TarockRules::legalCards` bildet `koenigrufen.md` §6.7 wörtlich ab
(Farbzwang → Tarockzwang → freier Abwurf; `SuitGame`: Tarock sticht nicht und darf erst ohne Farbkarten
ausgespielt werden; `Negative`: Stichzwang plus Pagat-als-letztes-Tarock) und ergänzt die Bindung
reservierter Karten. Ungarisch nutzt davon `Positive` mit **harter** Bindung (`hungarian.md` §6.4,
`lekötött lap`/`rájátszás`), Königrufen mit **weicher** Bindung plus den zwei harten Fällen aus §6.5 –
gesteuert über `BonusDef::flags & Binding` und das Flag `harteVogelreservierung`.

### 4.4 Phasen – `src/core/TarockCore.h`

```cpp
enum class Phase : std::uint8_t {
    Deal       = 0,   // Geben (Animation), Talon anlegen
    Bidding    = 1,   // Lizitation / licitálás
    Call       = 2,   // Königsruf (KR) bzw. XX-hívás (HU)
    Talon      = 3,   // Talonhälfte wählen / Talon verteilen / bedobás-Fenster
    Discard    = 4,   // Verlegen / skartolás
    Announce   = 5,   // Prämien + Kontra ("Ich liege" / a bemondások)
    Play       = 6,   // 12 bzw. 9 Stiche
    Scoring    = 7,   // Zählen und Abrechnen (mit Lern-Erklärung)
    HandOver   = 8    // Ergebnis liegt an, nächste Hand möglich
};
```

Die Reihenfolge ist profil- **und** kontraktabhängig; `TarockCore::advance()` fragt
`rules().nextPhase(*this)`. Beispiele:

- KR Rufer: `Deal → Bidding → Call → Talon → Discard → Announce → Play → Scoring`
- KR Solodreier: `Deal → Bidding(mit Ansagen) → Announce(nur Kontra) → Play → Scoring`
- KR Bettel: `Deal → Bidding → Announce(nur Kontra) → Play → Scoring`
- HU három: `Deal → Bidding → Talon(Verteilung + bedobás) → Discard → Call+Announce → Play → Scoring`

`phaseOrder` im Profil liefert dem Lernpanel und der Fortschrittsleiste die vollständige Kette samt
übersprungener Phasen.

### 4.5 Aktionsmodell – `src/core/Action.h`

```cpp
enum class ActionType : std::uint8_t {
    None = 0,
    Bid          = 1,   // a = ContractId
    Pass         = 2,   // "weiter"/"gut" im Lizit, "passz"
    Hold         = 3,   // "ich halte" / "tartom"
    OpenForehand = 4,   // KR: "Vorhand" (noch kein Spiel)
    CallKing     = 5,   // a = Suit, b = 1 -> "der vierte König" (Farbe wird nicht genannt)
    CallTarock   = 6,   // a = CardId  (HU: XX oder ersatzweise)
    TakeTalon    = 7,   // a = 0|1 gewählte Hälfte; -1 = ganzer Talon (Sechserdreier)
    Concede      = 8,   // "schleifen" (KR §4.5a)
    Throwin      = 9,   // "bedobás" (HU §4.2), a = Grund-Id
    Discard      = 10,  // a = CardId, Toggle in die Ablage
    ConfirmDiscard = 11,
    AnnounceBonus  = 12, // a = BonusId, b = Suit (király ultimó) bzw. -1
    Kontra       = 13,  // a = PostenId (Spiel oder Prämie), b = Stufe 2|4|8
    Ready        = 14,  // "gut" / "Ich liege" (schließt die eigene Ansage ab)
    PlayCard     = 15,  // a = CardId
    ClaimEnd     = 16   // vorzeitiges Ende, nur wenn öffentlich feststehend (§6.8)
};

struct Action {
    ActionType type = ActionType::None;
    std::int16_t a = -1;
    std::int16_t b = -1;
    bool operator==(const Action&) const;
};
```

Die `Discard`-Modellierung als Einzeltoggle (L5) ist der Grund, warum jede unzulässige Ablagekarte einzeln
mit `E_ABLAGE_KOENIG`, `E_ABLAGE_TRULL`, `E_ABLAGE_TAROCK_NOCH_FARBE`, `S_KING`, `S_HONOUR`, `S_XX` usw.
begründet werden kann.

### 4.6 Grund-Codes – `src/core/Reason.h` (Herzstück des Lernmodus)

```cpp
enum class Severity : std::uint8_t { Ok = 0, Info = 1, Warning = 2, Error = 3 };

enum class ReasonCode : std::uint16_t {
    Ok = 0,
    // --- allgemein -------------------------------------------------------
    NotYourTurn, NotYourCard, WrongPhase, SeatSittingOut,
    // --- Lizitation (KR E_*, HU B_*) --------------------------------------
    BidTooLow, ForehandGameOnlyLast, SechserOnlyFirst, AlreadyPassed,
    ForehandMustSpeak, HoldOnlyForehand, HoldNotAllowed, CallerHasFourKings,
    BesserruferWithoutBird, NoHonour, InviterLocked, JumpWithoutCard, PassIsEngedes,
    // --- Ruf ---------------------------------------------------------------
    CallOwnKing, FourthKingNeedsThree, CallMustBeXX, CallOwnSkart, CallHonourForbidden,
    // --- Ablage / Skart ----------------------------------------------------
    DiscardKing, DiscardTrull, DiscardTarockWhileSuit, DiscardSuitWhileTarock,
    DiscardCount, DiscardXX, DiscardInvitCard,
    // --- Ansagen und Kontra -------------------------------------------------
    BonusCardNotHeld, KingUltimoWithoutKing, AnnounceOnlyOnce, BesserruferBirdMandatory,
    NoBonusInNegative, SuitGameBonusLimited, SolodreierAnnounceTooLate,
    KontraBeforeReady, KontraOwnAnnouncement, KontraLevel, KontraTooLate,
    TrischakenNoKontra, SuitGameKontraOrValat,
    TrullNeedsHonour, TrullDeclarerFirstRound, AnnounceAfterVolat, AnnounceRepeat,
    SameTrickTwice, UltiToUhu, UhuToUlti, TarokkszamRequired, TarokkszamWrong,
    // --- Kartenspiel --------------------------------------------------------
    FollowSuit, FollowTarock, MustTarock, MustOvertrumpSuit, MustOvertrumpTarock,
    MustTrumpNegative, PagatNotLastTarock, SuitGameNoTarockLead,
    BoundTooEarly, BoundMustPlay, BoundOrder, BoundWinsTooEarly, PagatUltiHold,
    // --- Warnungen (erlaubt, aber Prämienverlust) ----------------------------
    W_BirdReserved, W_KingUltimoReserved, W_GivesUpSilentBonus,
    // --- Hinweise -----------------------------------------------------------
    I_DiscardShownOpen, I_TalonHalfGoesToOpponents, I_Tie4747, I_HivatalbolKontra,
    I_SilentFigure, I_XXIcatch, I_VolatSilentRule, I_SilentUltiLost
};

struct Reason {
    ReasonCode code = ReasonCode::Ok;
    CardSet   cards;              // "erlaubt wären …" / "du hast noch …"
    Card      card;               // die beanstandete bzw. zu spielende Karte
    std::int16_t seat = -1;       // {Name}
    std::int16_t suit = -1;       // {Farbe}
    std::int16_t bonus = -1;      // {Prämie}/{Figur}
    std::int16_t contract = -1;   // {Spiel}
    std::int16_t trick = -1;      // {n}. Stich
    std::int16_t count = -1;      // {k}/{m}
    constexpr bool ok() const { return code == ReasonCode::Ok; }
};

Severity severityOf(ReasonCode);
const char* reasonKey(ReasonCode);   // "P_FOLLOW_SUIT", "E_ABLAGE_KOENIG" – stabile Schlüssel für Tests/Logs
```

Damit wird die zentrale Kern-API:

```cpp
class TarockCore {
public:
    static constexpr int MaxSeats = 5;

    TarockCore(ProfileId, const FlagSet& = {}, std::uint32_t seed = 1);

    void newMatch(ProfileId, int players /*4|5*/, MatchMode, std::uint32_t seed);
    bool nextHand();
    void dealFixed(const FixedDeal&);              // Übungspartien und Tests (§7.6)

    // ---- Abfrage ---------------------------------------------------------
    const RuleProfile& profile() const;  const TarockRules& rules() const;
    int players() const;  int activePlayers() const;  int sittingOut() const;   // Geber am Fünfertisch
    int dealer() const;   int forehand() const;      int actor() const;
    Phase phase() const;  int handNumber() const;    int radlNumber() const;

    const CardSet& hand(int seat) const;
    const CardList& trick() const;  int trickNumber() const;
    const CardSet& talonHalf(int i) const;  bool talonOpen() const;
    const CardSet& discardTray(int seat) const;  const CardSet& discards(int seat) const;
    const CardSet& won(int seat) const;

    ContractId contract() const; int declarer() const; int partner() const;  // partner: nur serverseitig
    bool partnerKnown() const; Card calledCard() const; int calledSuit() const;
    const std::vector<Posten>& posten() const;      // Spiel + jede Ansage, mit Kontrastufe
    const CardSet& boundCards(int seat) const;  int boundTargetTrick(Card) const;

    // ---- Aktionen ---------------------------------------------------------
    std::vector<Action> legalActions(int seat) const;
    Reason check(int seat, const Action&) const;              // Ok oder Ablehnungsgrund
    Reason warn (int seat, const Action&) const;              // erlaubt, aber verlustbringend
    bool   apply(int seat, const Action&);                    // false, wenn check() nicht Ok
    int    commitTrick();                                     // Stich einziehen, Gewinner zurück
    Ledger settle() const;

    // ---- KI ---------------------------------------------------------------
    Action  chooseAiAction(int seat, int budgetMs) const;
    std::vector<Hint> rankActions(int seat, int budgetMs) const;

    // ---- Netz und Persistenz ----------------------------------------------
    void rotateSeats(int offset);                             // Sitz s -> (s - offset) mod players
    std::string serializeState(int viewer = -1) const;        // viewer >= 0: gefilterte Sicht (§8.3)
    bool restoreState(const std::string&);
    bool validate(std::string* error = nullptr) const;
    bool redacted() const;                                    // Sicht-Kern eines Gastes
};
```

`serializeState()` behält Aufbau und Prüfsumme des Snapszer-Formats (`TAROCK_V1\n<hex-checksum>\n<payload>`),
ergänzt aber den `viewer`-Parameter: verdeckte Karten werden als `?`-Platzhalter mit korrekter **Anzahl**
serialisiert. `restoreState` markiert solche Zustände als `redacted`; `apply()` akzeptiert darauf nur
öffentliche Aktionen und Aktionen des eigenen Sitzes.

### 4.7 Fünfertisch und Sitzrotation

- `m_players ∈ {4,5}`. Bei 5 ist `sittingOut() == dealer()`; `activeSeats()` liefert vier Sitze ab
  `forehand()` gegen den Uhrzeigersinn (`koenigrufen.md` §2.7, `hungarian.md` §2.5).
- Der Aussetzer erhält keine Karten, bekommt 0 Schrift und 0 Geld, sieht den Talon **nicht** und hat einen
  eigenen Zuschauerkanal.
- Ein Radl umfasst 4 bzw. 5 Hände, damit jeder genau einmal gibt (und am Fünfertisch genau einmal aussetzt).
- `rotateSeats(offset)` verschiebt **alle** Sitzbezüge (Hände, Stiche, Ablagen, Masken, `dealer`, `declarer`,
  `partner`, `sittingOut`, `turn`, `boundCards`, `posten[].owner`) modulo `m_players` – wie
  `MultiCore::rotateSeats`, nur mit fünf statt vier Plätzen und zusätzlichen Feldern. Ein LAN-Gast ruft
  `rotateSeats(hostSeat)` auf, danach ist sein eigener Platz **immer Sitz 0**. Fällt ihm in einer Hand der
  Aussetzerplatz zu, schaltet die UI für diese Hand in den Zuschauermodus.
- Invariante (Test `test_rotation.cpp`): `rotateSeats(k); rotateSeats(players-k)` ist identisch zum Ausgang,
  und `legalActions(seat)` ist rotationsäquivariant.

### 4.8 Abrechnung – `src/core/Scoring.h`

```cpp
enum class PostenType : std::uint8_t { Game, Bonus, Valat, Trischaken, Tarokkszam };
enum class Party : std::uint8_t { Declarer, Defenders, Neutral };

struct Posten {
    PostenType type; BonusId bonus = BonusId::None;
    Party owner;                 // wer angesagt hat (Neutral = still)
    bool announced;  bool achieved;
    int  value;                  // Tarifwert je Spieler
    int  kontra = 1;             // 1|2|4|8 – nur Geld (KR) bzw. voller Multiplikator (HU)
    std::int8_t againstSeat = -1;// Negativspiele: Einzelkontra Spieler–Gegner
};

struct Ledger {
    std::array<int, 5> schrift{};   // Turnierwertung, ohne Kontra (KR §7.1)
    std::array<int, 5> geld{};      // Auszahlung, mit Kontra
    std::vector<Posten> items;
    CountResult declarerCards, defenderCards;
    bool zeroSum() const;           // Testinvariante
};
```

`KoenigrufenRules::settle` implementiert `koenigrufen.md` §7.11 wörtlich (Valat verdrängt das Spiel, beim
Valat zählen nur angesagte Prämien, Sechserdreier verdoppelt nur das verlorene **Spiel**, Alleinspiel zahlt
±3·v, Solorufer mit König im Talon setzt alle Kontrastufen auf 1). `HungarianRules::settle` implementiert
`hungarian.md` §7.2 (Reihenfolge: angesagte dupla/volát → stille dupla/volát → parti, danach die übrigen
Figuren; volát verdrängt die stille dupla; `volat_kills_silent_bonuses`). Beide liefern dieselbe `Ledger`-Form,
die `ScoreSheet.qml` ohne Profilwissen darstellt.

### 4.9 KI-Schnittstellen – `src/core/Ai.h`

```cpp
enum class Difficulty : std::uint8_t { Beginner, Club, Tournament };

enum class HintReason : std::uint16_t {
    None,
    // Lizit
    HandTooWeak, HandStrongEnough, BirdCountSufficient, SoloWorthIt, ContractEconomy, SeatPosition,
    // Ruf / Talon / Ablage
    CallFromGuardedQueen, CallShortSuit, PreferTarockOverPoints, VoidASuit, KeepCalledSuit, ProtectKing,
    // Ansage
    BonusSafe, BonusRisky, KontraWorthwhile, SignalToPartner,
    // Spiel
    DrawTrumps, SmearToPartner, DuckCheaply, KeepStopper, LeadShortKing, SaveBirdForTarget,
    LastTrickPlan, CountSaysEnough, CountSaysMustAttack
};

struct Hint {
    Action action;
    int    score;          // Erwartungswert in Abrechnungspunkten ×100
    int    rank;           // 0 = Empfehlung
    HintReason reason;
    Reason detail;         // Karten/Parameter für den Text
};

class AiPlayer {
public:
    AiPlayer(const TarockRules&, Difficulty);
    Action bestAction(const TarockCore&, int seat, int budgetMs) const;
    std::vector<Hint> rank(const TarockCore&, int seat, int budgetMs) const;   // Lernmodus (L6)
    HintReason explain(const TarockCore&, int seat, const Action&) const;
private:
    Inference m_info;      // Voids, gefallene Karten, Ansagen, offene Talonhälfte
};
```

- **Heuristik** (`AiHeuristic.cpp`) für Lizit, Ruf, Talonwahl, Ablage, Ansagen und Kontra, mit den Richtwerten
  aus `koenigrufen.md` §10.2–10.5 (10-/12-/16-Punkte-Regel, Vogelbedingungen) und `hungarian.md` §10.2–10.6.
  Jede Entscheidung liefert einen `HintReason` – das ist genau der Text, den der Lernmodus zeigt.
- **Suche** (`AiSearch.cpp`) für das Kartenspiel: Determinisierung + PIMC, 20–60 Samples, Zeitbudget
  50–250 ms je Zug, Endspiel ≤ 5 Karten exakt (`koenigrufen.md` §10.1). Bewertet wird der **Abrechnungssaldo**
  aus `settle()`, nicht nur Kartenpunkte – damit berücksichtigt die KI Prämien automatisch.
- **Fairness-Pflicht** (`koenigrufen.md` §10.8, `hungarian.md` §10.8): `AiPlayer` erhält nur eine
  `TarockCore`-Sicht mit `viewer = seat`; die Determinisierung darf nur `Inference`-Wissen verwenden.
  Test `test_selfplay.cpp` prüft, dass `AiPlayer` nie auf `hand(other)` zugreift (der Sicht-Kern liefert dort
  `?`-Platzhalter, ein Zugriff schlägt fehl).
- **Schwierigkeitsgrade:** `Beginner` = Heuristik ohne Suche und mit gedrosselter Inferenz,
  `Club` = Heuristik + 20 Samples, `Tournament` = volle Suche.

---

## 5. Engine/QML-Kontrakt – `src/TarockEngine.h`

Aufbau wie `MultiEngine`: Sitz 0 ist immer der lokale Spieler, Animationsphasen entkoppeln Kern und UI,
ein Timer steuert die Computerzüge, ein Watchdog fängt verlorene Animationen ab.

```cpp
class TarockEngine : public QObject
{
    Q_OBJECT
    // --- Partie und Phase ---------------------------------------------------
    Q_PROPERTY(bool active        READ active        NOTIFY stateChanged)
    Q_PROPERTY(bool canResume     READ canResume     NOTIFY stateChanged)
    Q_PROPERTY(QString profileKey READ profileKey    NOTIFY settingsChanged)
    Q_PROPERTY(QString profileName READ profileName  NOTIFY settingsChanged)
    Q_PROPERTY(int players        READ players       NOTIFY stateChanged)
    Q_PROPERTY(int phase          READ phase         NOTIFY stateChanged)
    Q_PROPERTY(QString phaseName  READ phaseName     NOTIFY stateChanged)
    Q_PROPERTY(QVariantList phaseChain READ phaseChain NOTIFY stateChanged)  // Fortschrittsleiste
    Q_PROPERTY(QString status     READ status        NOTIFY stateChanged)
    Q_PROPERTY(int actor          READ actor         NOTIFY stateChanged)
    Q_PROPERTY(bool myTurn        READ myTurn        NOTIFY stateChanged)

    // --- Tisch ---------------------------------------------------------------
    Q_PROPERTY(QVariantList seats READ seats         NOTIFY stateChanged)
    // seat: { name, seat, isMe, isDealer, isForehand, isSittingOut, isDeclarer,
    //         partnerState: "unknown"|"partner"|"opponent", cardCount, tricks,
    //         schrift, geld, lastBid, announcements: [ … ] }
    Q_PROPERTY(QVariantList hand  READ hand          NOTIFY stateChanged)
    // card: { id, key, label, suit, power, legal, warn, bound, boundTrick, recommended, dimReason }
    Q_PROPERTY(QVariantList trick READ trick         NOTIFY stateChanged)
    Q_PROPERTY(QVariantList talonHalves READ talonHalves NOTIFY stateChanged)
    Q_PROPERTY(QVariantList discardTray READ discardTray NOTIFY stateChanged)
    Q_PROPERTY(int  discardTarget READ discardTarget NOTIFY stateChanged)
    Q_PROPERTY(QVariantList openDiscards READ openDiscards NOTIFY stateChanged)
    Q_PROPERTY(int trickNumber  READ trickNumber     NOTIFY stateChanged)
    Q_PROPERTY(int tricksTotal  READ tricksTotal     NOTIFY stateChanged)

    // --- Entscheidungen -------------------------------------------------------
    Q_PROPERTY(QVariantList options READ options     NOTIFY stateChanged)
    // option: { type, a, b, label, speech, group, enabled, reasonKey, hint, hintText }
    Q_PROPERTY(QVariantList bonusOptions READ bonusOptions NOTIFY stateChanged)
    Q_PROPERTY(QVariantList kontraTargets READ kontraTargets NOTIFY stateChanged)
    Q_PROPERTY(QString contractName READ contractName NOTIFY stateChanged)
    Q_PROPERTY(QString calledCard   READ calledCard   NOTIFY stateChanged)
    Q_PROPERTY(QVariantList posten  READ posten       NOTIFY stateChanged)

    // --- Ergebnis ---------------------------------------------------------------
    Q_PROPERTY(bool handOver     READ handOver       NOTIFY stateChanged)
    Q_PROPERTY(bool matchOver    READ matchOver      NOTIFY stateChanged)
    Q_PROPERTY(QVariantMap ledger READ ledger        NOTIFY stateChanged)
    Q_PROPERTY(QVariantMap liveCount READ liveCount  NOTIFY stateChanged)   // laufender Punktestand

    // --- Darstellung ------------------------------------------------------------
    Q_PROPERTY(int visualPhase READ visualPhase NOTIFY visualPhaseChanged)  // Idle|CardFlight|TrickPause|TrickFlight|Reveal
    Q_PROPERTY(bool paused     READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(QString deck    READ deck   WRITE setDeck   NOTIFY settingsChanged)

    // --- Netz --------------------------------------------------------------------
    Q_PROPERTY(bool networkGame  READ networkGame  NOTIFY networkChanged)
    Q_PROPERTY(bool lanGuest     READ lanGuest     NOTIFY networkChanged)
    Q_PROPERTY(bool lanHosting   READ lanHosting   NOTIFY networkChanged)
    Q_PROPERTY(bool lanBusy      READ lanBusy      NOTIFY networkChanged)
    Q_PROPERTY(bool spectating   READ spectating   NOTIFY networkChanged)
    Q_PROPERTY(QString networkStatus READ networkStatus NOTIFY networkChanged)
    Q_PROPERTY(QVariantList lobby READ lobby       NOTIFY networkChanged)

public:
    enum VisualPhase { Idle = 0, CardFlight = 1, TrickPause = 2, TrickFlight = 3, Reveal = 4 };
    Q_ENUM(VisualPhase)

    Q_INVOKABLE void startMatch(const QString& profileKey, int players, int mode);
    Q_INVOKABLE void resume();
    Q_INVOKABLE void newMatch();
    Q_INVOKABLE void nextHand();

    // Eine Aktion; `type` ist der Kleinbuchstabenname aus ActionType ("bid", "pass",
    // "hold", "vorhand", "callking", "calltarock", "taketalon", "concede", "throwin",
    // "discard", "confirmdiscard", "bonus", "kontra", "ready", "play", "claim").
    Q_INVOKABLE bool act(const QString& type, int a = -1, int b = -1);
    // Wie act(), unterdrückt aber eine bereits angezeigte Warnung (W_*).
    Q_INVOKABLE bool actConfirmed(const QString& type, int a = -1, int b = -1);

    Q_INVOKABLE void completeCardAnimation();
    Q_INVOKABLE void completeTrickAnimation();
    Q_INVOKABLE QString cardLabel(int cardId) const;
    Q_INVOKABLE QString contractLabel(int contractId) const;
    Q_INVOKABLE QString bonusLabel(int bonusId) const;
    Q_INVOKABLE QVariantList lastTrick() const;
    Q_INVOKABLE QVariantList ownTricks() const;

    Q_INVOKABLE void hostLanGame(const QString& profileKey, int players);
    Q_INVOKABLE void startLanMatch();
    Q_INVOKABLE void joinLanGame(const QString& address);
    Q_INVOKABLE void cancelLan();

signals:
    void stateChanged();
    void settingsChanged();
    void visualPhaseChanged();
    void pausedChanged();
    void networkChanged();
    void networkNotice(const QString& text);
    void resetVisuals();
    void matchStarted();
    void handFinished();
    void lanRedirect(const QString& address, const QString& profileKey, int players);
    void cardAnimationRequested(int cardId, int seat);
    void trickAnimationRequested(int winnerSeat);
    void revealRequested(const QString& what);           // "talon", "ouvert", "skart"
    // Vom Kern abgelehnte oder gewarnte Aktion; Nutzlast für WhyDialog/HintBubble.
    void actionRejected(const QVariantMap& reason);
    void actionWarned(const QVariantMap& reason, const QString& type, int a, int b);
    void speech(int seat, const QString& formula);        // "Vorhand", "ich halte", "tartom", "Ich liege"
};
```

`reason`-Map (identisch für `actionRejected`, `actionWarned` und `learnEngine.why*`):

```js
{ key: "P_FOLLOW_SUIT", severity: 3, short: "Farbzwang",
  text: "Es wurde ♣ ausgespielt und du hast noch ♣ auf der Hand …",
  cards: [31, 33], card: 12, seat: 1, suit: 3, bonus: -1, contract: 2, trick: 4,
  anchor: "rules:at-kr-ooe#6.2" }
```

**Statusmaschine der Animation** (übernommen von `MultiEngine`): `visualPhase` durchläuft
`Idle → CardFlight → (TrickPause → TrickFlight) → Idle`; solange nicht `Idle`, nimmt die Engine keine neuen
Aktionen an. `m_aiTimer` plant den nächsten Computerzug, `m_watchdog` erzwingt `finishIdle()`, falls eine
Animation nicht quittiert wird. Neu gegenüber Snapszer: `Reveal` für Talonaufdecken, offene Ablage und
Ouvertspiele.

---

## 6. Benutzeroberfläche

### 6.1 Seiten

| Seite | Sailfish | Android | Inhalt |
|---|---|---|---|
| Start | `harbour-tarock.qml` (PageStack) | `Main.qml` (StackView) | Profilwahl, 4/5 Spieler, Modus, „Weiterspielen“, Lernmodus-Einstieg |
| Tisch | `TablePage.qml` | `TablePage.qml` | `qml-common/TarockTable.qml` |
| Lektionen | `LessonPage.qml` | dito | L0–L7 + drei Übungspartien je Profil |
| Regeln | `RulesPage.qml` | dito | `RulesBrowser.qml`, `TariffTable.qml`, `GlossaryList.qml` |
| Punkte | `ScorePage.qml` | dito | `ScoreSheet.qml` – Schrift/Geld je Spieler, Postenliste, Radl-Zähler |
| LAN | `LanPage.qml` | dito | 1:1 aus Snapszer, plus Profil- und Flag-Anzeige |
| Einstellungen | `SettingsPage.qml` | dito | Profil, ⚙-Flags, Deck, Lernstufe, Animationstempo, Namen |
| Credits | `CreditsPage.qml` | dito | gerendertes `CREDITS/ASSETS.md` |

Die Trennung ist dieselbe wie bei Snapszer: Alles Spiellogische liegt in `qml-common/` (QtQuick 2.6, ES5,
keine `Sailfish.Silica`- und keine `QtQuick.Controls`-Importe); die Plattformen liefern nur
`Style` (Singleton, Maße und Farben), `Card`, `FlyingCard`, `TableButton`, `SubPage`. Auf Sailfish steht
`singleton Style 1.0 Style.qml` in `sailfish/qmldir`, auf Android setzt
`set_source_files_properties(qml/Style.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)` dasselbe.

### 6.2 Tischlayout, Portrait, 4 und 5 Spieler

```
┌──────────────────────────────────────────┐
│ ⟵  Königrufen · Dreier · Radl 2/4    ⋮   │  Kopfzeile: Profil, Vertrag, Fortschritt, Menü
├──────────────────────────────────────────┤
│                ┌────────┐                │
│                │ Sitz 2 │  ← gegenüber   │  SeatPanel: Name, Kartenzahl, Stichstapel,
│                └────────┘                │  Partnerfarbe (grün/rot wie MultiTable),
│  ┌────────┐   ╔══════════╗   ┌────────┐  │  Geber-/Vorhandmarke, letzte Ansage
│  │ Sitz 1 │   ║  Stich   ║   │ Sitz 3 │  │
│  └────────┘   ╚══════════╝   └────────┘  │  TrickArea: bis zu 4 Karten kreisförmig
│           ┌──────────────┐               │
│           │  Kiebitz  🔒 │  (nur zu fünft)│  Aussetzer als flacher Chip neben dem Geber
│           └──────────────┘               │
├──────────────────────────────────────────┤
│  Aktionsband  (BidBar / TalonPanel /     │  phasenabhängig eingeblendet
│  DiscardTray / AnnouncePanel / KontraRow)│
├──────────────────────────────────────────┤
│  ♠ ♣ ♥ ♦ │ Tarock │  ← Handfächer        │  HandFan: nach Farben gruppiert, Tarock links,
│  [ Karten des lokalen Spielers        ]  │  12 bzw. 9 Karten, überlappend, Tap = spielen
├──────────────────────────────────────────┤
│  💡 Hinweis   ❓ Warum?   📊 Stand        │  Lernleiste (nur wenn Lernmodus ≠ Aus)
└──────────────────────────────────────────┘
```

- **Zwölf Karten im Portrait:** `HandFan.qml` legt die Karten mit `overlap = clamp((width - cardWidth) /
  (count - 1), cardWidth * 0.28, cardWidth * 0.62)` übereinander; die angetippte Karte hebt sich um
  `Style.paddingLarge` und zeigt ihren vollen Kartenspiegel. Bei > 10 Karten wird zusätzlich nach Farben
  gruppiert mit `Style.paddingSmall` Abstand zwischen den Gruppen; Tarock steht links, dann ♥ ♦ ♠ ♣.
- **Fünftes Panel:** Am Fünfertisch sitzt der Aussetzer nicht am Tisch, sondern erscheint als
  `SeatPanel { compact: true; muted: true }` unter der Tischmitte, mit Schlosssymbol („sieht den Talon nicht“).
  Ist der lokale Spieler der Aussetzer, wird das Aktionsband durch ein Hinweisband ersetzt und der
  Handfächer entfällt.
- **Legalität ist immer sichtbar:** Nicht spielbare Karten werden mit `opacity 0.35` gezeichnet; ein Tap
  darauf spielt nicht, sondern löst `actionRejected` aus → Toast (Kurztext) bzw. `WhyDialog` (Langtext).
  Gebundene Karten (angesagte Vögel, `lekötött lap`) tragen ein kleines Schloss mit der Zielstichnummer.
- **Empfehlung:** Im Lernmodus *Neuling* bekommt die von `rankActions` empfohlene Karte einen grünen Rahmen.

### 6.3 Lizitation – `BidBar.qml`

Eine horizontal scrollende Reihe von Chips in Lizitationsreihenfolge, links „weiter“, rechts das höchste
Gebot. Nicht erreichbare Gebote sind ausgegraut und tragen beim Tap den Grund (`E_GEBOT_ZU_NIEDRIG`,
`E_SECHSER_NUR_VORNEWEG`, `B_NO_HONOUR` …). Sonderchips: „Vorhand“, „ich halte“/`tartom`, im ungarischen
Profil zusätzlich die Sprunggebote `invit XIX`/`invit XVIII` und `engedés`. Über der Reihe steht die
Sprechformel des letzten Spielers (`speech`-Signal, `koenigrufen.md` §3.8).

### 6.4 Talon und Verlegen – `TalonPanel.qml` + `DiscardTray.qml`

- **KR, offener Talon:** Zwei Karten-Tripel nebeneinander, jeweils mit einer Zeile „2 Tarock · 1 Punkt“
  bzw. „0 Tarock · 6 Punkte“ (im Lernmodus eingeblendet). Tap wählt die Hälfte. Liegt der gerufene König
  im Talon, erscheint zusätzlich der Knopf „Schleifen“ (`Concede`) mit Erklärung.
- **KR, Sechserdreier:** sechs verdeckte Karten, ein Knopf „Aufnehmen“.
- **HU:** Der Talon wird nach `hungarian.md` §4.1 automatisch verteilt; die eigenen Karten fliegen in die Hand,
  danach öffnet sich – falls berechtigt – das `bedobás`-Fenster mit den fünf Einwurfgründen.
- **DiscardTray:** ein Ablagefach mit `discardTarget` Slots (3 oder 6). Tap auf eine Handkarte legt sie ins
  Fach, Tap im Fach nimmt sie zurück; unzulässige Karten sind ausgegraut, beim Tap kommt der Grund. Karten,
  die **offen** hingelegt werden müssen, tragen ein Augensymbol (`I_ABLAGE_OFFEN` / `S_TAROCK_INFO`).
  Erst wenn das Fach voll ist, wird „Fertig“ (`ConfirmDiscard`) aktiv.

### 6.5 Ansagen – `AnnouncePanel.qml` (viele Optionen)

Das Problem: Königrufen hat 8 Prämien, Ungarisch bis zu 17 Figuren, teils mit Farbparameter.

```
┌── Ansagen ──────────────────────────────────────────┐
│ Besitz     [ Trull ]  [ Alle Könige ]               │   Gruppe 1: BonusKind::Holding
│ Verlauf    [ Pagat ] [ Uhu ] [ Kakadu ] [ Quapil ]  │   Gruppe 2: TrickWithCard / TrickRun
│            [ König ultimo ]                          │
│ Spielziel  [ Valat ]                                 │   Gruppe 3: PointTarget / AllTricks
│ ─────────────────────────────────────────────────── │
│ Deine Ansage: Pagat · Trull            [ Ich liege ]│
└──────────────────────────────────────────────────────┘
```

- Ein `Flow` aus Toggle-Chips, gruppiert nach `BonusKind`, damit die Liste auch mit 17 Figuren im Portrait
  lesbar bleibt. Gesperrte Figuren bleiben sichtbar (das ist Lernstoff) und nennen beim Tap ihren Grund
  (`E_VOGEL_NICHT_IM_BLATT`, `A_TRULL_NO_HONOUR`, `E_FARBENSPIEL_ANSAGE` …).
- Figuren mit Parameter (`király ultimó`, `király uhu`) öffnen einen kleinen Farbwähler.
- `Ready` heißt je nach Sitz „Ich liege“ (Spieler) oder „gut“ (übrige) – Text aus `speech`.
- **`KontraRow.qml`** zeigt darunter jeden offenen `Posten` als Zeile „Spiel · ×1“, „Pagat · ×2“ mit Knopf
  `Kontra`/`Rekontra`/`Subkontra`, je nach `checkAnnouncement`. Negativspiele zeigen die Gegner einzeln
  (`koenigrufen.md` §7.5).
- Im ungarischen Profil ergänzt eine Zeile „Konvention“ die Bedeutung der eigenen Ansage
  („tulétroá in Runde 1 heißt: beide großen honőr“) – `hungarian.md` §5.5 / §11.4.

### 6.6 Nach dem Spiel – `ScoreSheet.qml` und `CountTutor.qml`

`ScoreSheet` zeigt die Postenliste („Rufer gewonnen (1) +1/−1/+1/−1“) exakt in der Form der Beispieltabellen
der Specs, darunter Schrift und Geld je Spieler und den Radl-/Durchgangsstand. `CountTutor` blendet im
Lernmodus die Zählung in Dreierpaketen ein (`koenigrufen.md` §1.5) bzw. die einfache Summe (`hungarian.md` §1.5)
und lässt den Lernenden das Ergebnis selbst tippen, bevor es aufgelöst wird.

---

## 7. Lernmodus

> Ausdrücklicher Nutzerwunsch: „ein Modus, in dem alles und die Regeln erklärt werden.“

Der Lernmodus ist eine **Schicht über dem normalen Spiel** (`koenigrufen.md` §11.1), kein zweiter
Spielmodus – jede Partie, auch eine LAN-Partie, kann ihn eingeschaltet haben. Er besteht aus sechs Bausteinen:
Erklärpanel (§7.2), „Warum nicht?“ (§7.3), Hinweisgeber (§7.4), Übungspartien (§7.6), Nachschlagewerk (§7.7)
und Einstellungen (§7.8).

### 7.1 Stufen

```cpp
enum class LearnLevel : std::uint8_t { Off = 0, Learning = 1, Novice = 2 };
```

| Stufe | Erklärpanel | Warum-Texte | Hinweise | Warnungen | Empfehlungsrahmen |
|---|---|---|---|---|---|
| **Aus** | – | Kurz-Toast | auf Knopfdruck | Standarddialog | – |
| **Lernend** | einklappbar, auf Anforderung | Lang, mit Regelverweis | auf Knopfdruck | Langtext | – |
| **Neuling** | dauerhaft offen | Lang, automatisch | automatisch vor jedem eigenen Zug | Langtext + Alternativenliste | ja |

Die Stufe ist eine normale Einstellung und mitten in der Partie umschaltbar.

### 7.2 Kontextuelles Erklärpanel – `LearnPanel.qml` + `LearnEngine`

Das Panel liegt als ein-/ausklappbare Leiste unter der Kopfzeile und zeigt zu **jeder** Phase und **jeder**
Entscheidung drei Ebenen:

1. **Wo bin ich?** – Name der Phase, Position in der Kette (`phaseChain`), z. B. „3 von 7 · König rufen“.
2. **Was passiert hier?** – der Absatz aus `koenigrufen.md` §11.2 bzw. `hungarian.md` §11.2 in einfacher
   Sprache, gekürzt auf zwei Sätze, mit „Mehr dazu“ → Regelkapitel.
3. **Was soll ich jetzt tun?** – die Entscheidungsfrage plus die Bedeutung der aktuellen Optionen
   (`koenigrufen.md` §11.3 „Wozu es das gibt / wann man es wählt“, `hungarian.md` §11.3).

```cpp
class LearnEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int level READ level WRITE setLevel NOTIFY settingsChanged)   // LearnLevel
    Q_PROPERTY(bool panelOpen READ panelOpen WRITE setPanelOpen NOTIFY panelChanged)
    Q_PROPERTY(QVariantMap explanation READ explanation NOTIFY explanationChanged)
    // { phaseKey, phaseTitle, stepIndex, stepCount, whatHappens, whatToDo,
    //   terms: [{term, definition, anchor}], anchor }
    Q_PROPERTY(QVariantMap hint READ hint NOTIFY hintChanged)
    Q_PROPERTY(QVariantList history READ history NOTIFY historyChanged)  // letzte 10 gesperrte Aktionen
    Q_PROPERTY(bool lessonActive READ lessonActive NOTIFY lessonChanged)
    Q_PROPERTY(QVariantMap lessonStep READ lessonStep NOTIFY lessonChanged)

public:
    // "Warum darf ich das nicht?" – ohne die Aktion auszuführen.
    Q_INVOKABLE QVariantMap why(const QString& type, int a = -1, int b = -1) const;
    Q_INVOKABLE QVariantMap whyCard(int cardId) const;
    // "Warum ist das ein guter Zug?" für eine beliebige erlaubte Aktion.
    Q_INVOKABLE QVariantMap whyGood(const QString& type, int a = -1, int b = -1) const;
    // "Wie stehe ich gerade?" – Punktezähler, Tarockzähler, gefallene Vögel/honőr.
    Q_INVOKABLE QVariantMap standing() const;
    Q_INVOKABLE void requestHint();
    Q_INVOKABLE void explainTerm(const QString& term);
    Q_INVOKABLE QVariantList commonMistakes() const;      // §11.5 beider Specs
    Q_INVOKABLE QVariantMap debrief() const;              // Nachbesprechung nach der Hand

    Q_INVOKABLE void startLesson(const QString& lessonId);
    Q_INVOKABLE void lessonNext();
    Q_INVOKABLE void lessonRestartStep();
    Q_INVOKABLE void stopLesson();

signals:
    void settingsChanged(); void panelChanged(); void explanationChanged();
    void hintChanged(); void historyChanged(); void lessonChanged();
    void lessonStepPassed(const QString& text);
    void lessonTrap(const QVariantMap& reason);   // erwarteter Anfängerfehler getroffen
};
```

Texte liegen in `src/ReasonText.cpp` und `src/RulesIndex.cpp` in `tr()`, damit `lupdate` sie findet; die
Zuordnung Phase → Text ist eine Tabelle `{ProfileId, Phase, ContractId} → TextId`, sodass etwa die
Talonphase des Sechserdreiers einen anderen Text bekommt als die des Rufers.

### 7.3 „Warum nicht?“ – Grundcodes bis in die UI

Der Kern liefert nie nur `false`. Der Weg einer abgelehnten Aktion:

```
QML  card.tap()  →  tarockEngine.act("play", 27)
                 →  TarockCore::check(0, {PlayCard, 27})  →  Reason{ FollowSuit, cards={31,33}, suit=3 }
                 →  TarockEngine::act() gibt false zurück und emittiert actionRejected(map)
                 →  ReasonText::format(reason, profile, level)  liefert short/text/anchor
                 →  LearnPanel/Toast zeigt "Farbzwang"; Tap darauf öffnet WhyDialog mit dem Langtext,
                    markiert `cards` im Handfächer und verlinkt "Mehr dazu" auf rules:…#6.2
```

Die Zuordnung `ReasonCode → Text` ist **profilabhängig**, weil die Specs unterschiedliche Formulierungen
vorgeben. Deckungsgleiche Regeln teilen sich den Code, aber nicht zwingend den Text:

| `ReasonCode` | KR-Schlüssel (`koenigrufen.md` §11.4) | HU-Schlüssel (`hungarian.md` §11.6) |
|---|---|---|
| `NotYourTurn` | `E_NICHT_AM_ZUG` | `P_NOT_YOUR_TURN` |
| `FollowSuit` | `E_FARBZWANG` | `P_FOLLOW_SUIT` |
| `FollowTarock` | `E_FARBZWANG_TAROCK` | `P_TAROCK_ON_TAROCK` |
| `MustTarock` | `E_TAROCKZWANG` | `P_MUST_TAROCK` |
| `MustOvertrumpSuit` | `E_STICHZWANG_FARBE` | – (kein Stichzwang in HU) |
| `PagatNotLastTarock` | `E_PAGAT_NEGATIV` | – |
| `SuitGameNoTarockLead` | `E_FARBENSPIEL_TAROCK_AUSSPIEL` | – |
| `BoundTooEarly` | `W_VOGEL_RESERVIERT` (Warnung!) | `P_BOUND_TOO_EARLY` (Verbot) |
| `BoundWinsTooEarly` | `E_VOGEL_STICHT_ZU_FRUEH` | `P_PAGAT_ULTI_HOLD` |
| `BoundMustPlay` | `E_VOGEL_FAELLIG` | `P_BOUND_MUST_PLAY` |
| `BoundOrder` | `E_HOEHERER_VOGEL` (höherer zuerst) | `P_BOUND_ORDER` (früherer Stich zuerst) |
| `DiscardKing` | `E_ABLAGE_KOENIG` | `S_KING` |
| `DiscardTrull` | `E_ABLAGE_TRULL` | `S_HONOUR` |
| `DiscardTarockWhileSuit` | `E_ABLAGE_TAROCK_NOCH_FARBE` | – |
| `DiscardXX` | – | `S_XX` |
| `DiscardCount` | `E_ABLAGE_ANZAHL` | `S_COUNT` |
| `BidTooLow` | `E_GEBOT_ZU_NIEDRIG` | `B_TOO_LOW` |
| `NoHonour` | – | `B_NO_HONOUR` |
| `CallerHasFourKings` | `E_RUFER_VIER_KOENIGE` | – |
| `BesserruferWithoutBird` | `E_BESSERRUFER_OHNE_VOGEL` | – |
| `HoldOnlyForehand` | `E_HALTEN_NUR_VORHAND` | `B_HOLD_NOT_ALLOWED` |
| `CallOwnKing` | `E_EIGENER_KOENIG` | `A_CALL_OWN_SKART` |
| `BonusCardNotHeld` | `E_VOGEL_NICHT_IM_BLATT` | `A_TRULL_NO_HONOUR` |
| `KontraBeforeReady` | `E_KONTRA_VOR_ICH_LIEGE` | `A_KONTRA_OWN_TEAM` |

`reasonKey(code)` liefert den **profilunabhängigen** Schlüssel für Tests und Log; `ReasonText::key(profile,
code)` den obigen Spec-Schlüssel für Regelverweise und Übungsskripte.

Der Unterschied `BoundTooEarly` Warnung (KR) vs. Verbot (HU) ist nicht kosmetisch, sondern die
Kernaussage von `koenigrufen.md` §5.4/§6.5: Der Grad steht in `severityOf()` und ist im Profil per
`harteVogelreservierung` umschaltbar. `TarockEngine::act()` löst bei `Severity::Warning` `actionWarned` aus
und führt die Aktion erst nach `actConfirmed()` durch („Damit ist {Prämie} verloren – trotzdem spielen?“).

Die letzten zehn abgelehnten Aktionen stehen unter „Warum war das verboten?“ als Historie bereit
(`hungarian.md` §11.1).

### 7.4 Hinweisgeber

`LearnEngine::requestHint()` ruft `TarockCore::rankActions(0, budgetMs)`, also **denselben** Bewerter, der
die Computerspieler steuert (L6). Angezeigt wird:

```js
hint = {
  action: { type: "play", a: 19 },          // XX
  label:  "XX spielen",
  quality: "best",                           // best | good | playable | bad
  reasonKey: "H_DRAW_TRUMPS",
  short: "Tarock ziehen",
  text:  "Du hast mit dem XX das höchste noch lebende Tarock. Zieh damit die restlichen
          hohen Tarock heraus, bevor die Gegner deine Könige abstechen.",
  alternatives: [ { label: "♥K spielen", quality: "playable",
                    text: "Geht auch, ist aber schwächer: …" } ]
}
```

`quality` ergibt sich aus dem Abstand des `score` zum Bestwert (`best` < 0,25 Punkte, `good` < 1,0,
`playable` < 3,0, sonst `bad`). Der Text entsteht aus `HintReason` plus den Parametern in `Hint::detail` –
er ist damit immer konsistent zu dem, was ein Computergegner an derselben Stelle täte. Auf Stufe *Neuling*
wird der Hinweis automatisch nach 1,5 s Nachdenkzeit eingeblendet (`HintBubble.qml`), auf *Lernend* nur
auf Knopfdruck.

`whyGood(type, a, b)` bewertet eine **vom Nutzer gewählte** Aktion und formuliert den Vergleich
(„Das ist spielbar (Rang 3 von 7). Besser wäre … weil …“).

### 7.5 Laufende Hilfen

- **`standing()`** – „Wie stehe ich gerade?“: Kartenpunkte beider Parteien bisher (im gerade zählbaren Umfang),
  Zahl der noch nicht gefallenen Tarock je Spieler-Schätzung, gefallene Vögel/honőr, Status jeder offenen
  Ansage, und – wichtig, weil häufigster Anfängerfehler – die Erinnerung, zu welcher Partei die liegen
  gelassene Talonhälfte zählt (`koenigrufen.md` §11.5 Nr. 17).
- **`commonMistakes()`** – die 20 bzw. 12 typischen Anfängerfehler aus §11.5 beider Specs als durchblätterbare
  Kartenliste, aus dem Menü und aus der Nachbesprechung erreichbar.
- **`debrief()`** – Nachbesprechung nach jeder Hand: Punktestand, gefallene Prämien, und der Zug mit der
  größten Abweichung zwischen gespielter und bester Aktion („Entscheidend war Stich 5: …“), berechnet aus
  den während der Hand mitgeschriebenen `rankActions`-Bestwerten.

### 7.6 Übungspartien und Lektionen – Datenformat

Lektionen sind JSON-Dateien unter `lessons/<profil>/`, validiert gegen `lessons/schema.json`, geladen von
`Lesson.cpp`. Sie beschreiben ein festes Blatt, eine Folge von Schritten und die erwarteten Entscheidungen.
Die Module L0–L7 (`hungarian.md` §11.1) und die je drei Übungsspiele (`koenigrufen.md` §11.7,
`hungarian.md` §11.8) nutzen dasselbe Format.

```json
{
  "id": "kr-practice-1",
  "profile": "AT-KR-OOE-2023-04",
  "module": "L5",
  "title": "Mein erster Rufer",
  "goals": ["Lizitation der Vorhand", "König rufen", "Talon und Verlegen",
            "Farb- und Tarockzwang", "Partner erkennen", "Zählen", "stille Prämien"],
  "players": 4,
  "seatNames": ["Du", "Berta", "Carl", "Dora"],
  "dealer": 3,
  "deal": {
    "hands": [
      ["XX","XVII","XI","VIII","V","H:K","H:B","D:3","S:D","S:9","C:K","C:8"],
      ["Sk","XVI","XIII","IX","III","H:R","H:2","D:D","D:B","S:B","C:D","C:R"],
      ["XIX","XV","XII","IIII","II","H:1","H:3","D:K","D:2","S:K","S:10","C:10"],
      ["XXI","XIV","X","VII","I","H:4","D:1","S:R","S:8","S:7","C:B","C:7"]
    ],
    "talon": [["XVIII","VI","D:4"], ["H:D","D:R","C:9"]]
  },
  "steps": [
    { "id": "s1", "phase": "bidding", "seat": 0,
      "expect": { "type": "vorhand" },
      "text": "Dein Blatt: fünf Tarock, kein Trullstück, zwei Könige. Für einen Sechserdreier zu schwach, fürs Trischaken zu stark. Mit „Vorhand“ hältst du dir alles offen.",
      "highlight": { "hand": ["XX","XVII","XI","VIII","V"] },
      "traps": [ { "action": { "type": "bid", "a": "SECHSERDREIER" },
                   "text": "Für den Sechserdreier bräuchtest du 6–7 Tarock mit Trullstück und Stechern." } ] },

    { "id": "s2", "auto": ["pass","pass","pass"],
      "text": "Berta, Carl und Dora sagen „weiter“. Jetzt darfst du jedes Spiel außer dem Sechserdreier wählen." },

    { "id": "s3", "phase": "bidding", "seat": 0, "expect": { "type": "bid", "a": "RUFER" },
      "text": "Der Rufer ist das passende Spiel: billig, aber mit Partner." },

    { "id": "s4", "phase": "call", "seat": 0, "expect": { "type": "callking", "a": "S" },
      "text": "Du hast in Pik die Dame und die 9. Man ruft am liebsten aus einer Farbe mit besetzter Dame …",
      "traps": [ { "action": { "type": "callking", "a": "H" }, "reason": "E_EIGENER_KOENIG" },
                 { "action": { "type": "callking", "a": "C" }, "reason": "E_EIGENER_KOENIG" } ] },

    { "id": "s5", "phase": "talon", "seat": 0, "expect": { "type": "taketalon", "a": 0 },
      "text": "Hälfte A bringt zwei Tarock, Hälfte B sechs Punkte. Mit fünf Tarock bist du dünn besetzt – Tarock sind hier mehr wert. Hälfte B zählt am Ende zu den Gegnern!",
      "compare": [ { "half": 0, "note": "2 Tarock, davon 1 Stecher" },
                   { "half": 1, "note": "0 Tarock, 6 Punkte" } ] },

    { "id": "s6", "phase": "discard", "seat": 0,
      "expect": { "type": "discardSet", "cards": ["D:3","D:4","H:B"] },
      "text": "Mit den beiden Karo bist du in dieser Farbe blank und kannst Karo künftig stechen. Der Herz-Bube bringt zwei Punkte sicher in deine Stiche.",
      "traps": [ { "card": "C:K",   "reason": "E_ABLAGE_KOENIG" },
                 { "card": "XVIII", "reason": "E_ABLAGE_TAROCK_NOCH_FARBE" } ] },

    { "id": "s7", "phase": "announce", "seat": 0, "expect": { "type": "ready" },
      "text": "„Ich liege“ – keine Prämie: du hast kein Trullstück und keinen Vogel.",
      "auto": ["ready","ready","ready"] },

    { "id": "t1", "phase": "play", "trick": 1,
      "moves": [ {"seat":0,"card":"C:K"}, {"seat":1,"card":"C:R"},
                 {"seat":2,"card":"C:10"}, {"seat":3,"card":"C:7"} ],
      "lead": 0, "expectWinner": 0, "trickUnits": 22,
      "text": "Du spielst den Treff-König. In Treff hast du nur zwei Karten; je länger du wartest, desto eher sticht ihn jemand ab." },

    { "id": "t3", "phase": "play", "trick": 3,
      "moves": [ {"seat":3,"card":"D:1"}, {"seat":0,"card":"V"},
                 {"seat":1,"card":"D:B"}, {"seat":2,"card":"D:K"} ],
      "lead": 3, "expectWinner": 0, "trickUnits": 19,
      "text": "Dora spielt Karo. Du hast keines mehr: Tarockzwang, du musst ein Tarock zugeben …",
      "traps": [ { "card": "H:K", "reason": "E_TAROCKZWANG" } ] }
    /* … Stiche 2, 4–12 analog … */
  ],
  "result": {
    "declarerParty": [0, 2],
    "cardUnits": [125, 85],
    "cardText": ["41 Punkte 2 Blatt", "28 Punkte 1 Blatt"],
    "bonuses": [ { "key": "TRULL", "announced": false, "party": "defenders", "achieved": true },
                 { "key": "ALLE_KOENIGE", "announced": false, "party": "declarer", "achieved": true } ],
    "schrift": [1, -1, 1, -1],
    "geld":    [1, -1, 1, -1]
  },
  "moral": "Das Spiel zu gewinnen ist nur die halbe Miete. Die Trull rutschte zu den Gegnern, weil du die drei Trullstücke nie eingefangen hast."
}
```

**Semantik der Felder**

| Feld | Bedeutung |
|---|---|
| `deal` | wird über `TarockCore::dealFixed()` gesetzt; der Loader prüft, dass die Karten zusammen genau das Deck ohne Dubletten ergeben |
| `steps[].expect` | genau diese Aktion des lokalen Spielers führt weiter; jede andere erlaubte Aktion wird sanft abgelehnt („In dieser Übung machen wir …“) |
| `steps[].auto` | Aktionen der Computerspieler, die ohne Zutun ablaufen |
| `steps[].traps` | erwartete Anfängerfehler: entweder mit `reason` (der Kern lehnt ohnehin ab, die Übung ergänzt den Kontext) oder mit eigenem `text` (die Aktion wäre legal, ist hier aber falsch) |
| `steps[].highlight` | Karten/Bereiche, die das Panel hervorhebt |
| `steps[].moves` | vollständiger Stich; der Lernende spielt nur die Karten seines Sitzes, die übrigen laufen automatisch |
| `expectWinner`, `trickUnits` | Prüfwerte – im Spielbetrieb Anzeige, im Test Assertion |
| `result` | Sollzustand nach der Hand – die Grundlage des Regressionstests (§11.3) |

Freie Texte werden **nicht** über Qt Linguist übersetzt (JSON ist für `lupdate` unsichtbar), sondern über
Overlay-Dateien: `lessons/at-kr-ooe/kr-practice-1.hu.json` überschreibt einzelne Felder nach `id`. Fehlt ein
Overlay, wird der deutsche Originaltext gezeigt.

**Freies Üben:** Jede Übungspartie lässt sich nach Abschluss mit „Frei weiterspielen“ ab einem beliebigen
Schritt ohne Skript fortsetzen – der Kern ist derselbe, nur `LearnEngine::lessonActive` wird falsch.

### 7.7 Regel-Nachschlagewerk und Glossar

- `rules/at-kr-ooe.de.md` und `rules/hu-illu.de.md` sind gekürzte, in der App gerenderte Fassungen der
  Kapitel 1–9 der jeweiligen Spezifikation, mit stabilen Ankern (`#1.5`, `#6.7`, `#7.3`). Jeder
  `Reason.anchor` und jeder „Mehr dazu“-Link zeigt auf einen dieser Anker.
- `RulesIndex` baut beim Start einen einfachen Wortindex über beide Dateien und das Glossar; `RulesBrowser.qml`
  bietet Volltextsuche, Kapitelbaum und die Tariftabelle.
- `rules/glossary.de.tsv` enthält das Glossar aus `koenigrufen.md` §11.6 und `hungarian.md` §11.7
  (Begriff ⇥ Kurzdefinition ⇥ Regelanker ⇥ Synonyme). Jeder Fachbegriff in einem Erklärtext wird von
  `LearnPanel` automatisch antippbar gemacht (`RulesIndex::markup(text)` setzt `<a href="glossary:…">`),
  genau wie `hungarian.md` §11 es verlangt.
- `TariffTable.qml` zeigt die Spiel- und Prämienwerte des aktiven Profils direkt aus `RuleProfile::contracts`
  und `::bonuses` – es gibt keine zweite, abschreibbare Tabelle.

### 7.8 Einstellungen

```
Lernmodus
  ( ) Aus        ( ) Lernend        (•) Neuling
  [x] Hinweise automatisch anzeigen
  [x] Vor dem Verlust einer Prämie warnen
  [x] Unerlaubte Karten ausgrauen statt verbergen
  [x] Zählhilfe nach jedem Stich
  [ ] Positive Spiele immer bis zum 12. Stich ausspielen   (koenigrufen.md §6.8)
  Lektionen …                                              → LessonPage
  Regeln und Glossar …                                     → RulesPage
```

Gespeichert in `QSettings` unter `learn/level`, `learn/autoHint`, `learn/warnBonusLoss`, `learn/dimIllegal`,
`learn/countTutor`, `learn/playToEnd`. Die Stufe wirkt sofort, auch mitten in einer LAN-Partie – der
Lernmodus ist rein lokal und verändert keine Regel.

---

## 8. LAN

### 8.1 Was unverändert übernommen wird

`src/LanSession.{h,cpp}` aus Snapszer wandert unverändert mit: TCP-Server auf Port 45465, UDP-Responder auf
45466, zeilenweises JSON, `LanBrowser` mit Broadcast-Suche, `localAddresses()`/`internetAddresses()` für die
IPv6-Direktverbindung, Idle-Ping, `normalizeAddress()` für getippte IPv6-Adressen. Ebenso das Muster
**host-autoritativ**: Der Host führt den Kern, Gäste schicken Wünsche und übernehmen Zustände.

### 8.2 Nachrichtentypen

Alle Nachrichten tragen `t` (Typ) und, wo es um Reihenfolge geht, `s` (Sequenznummer).

| Richtung | `t` | Felder | Bedeutung |
|---|---|---|---|
| G→H | `hello` | `kind:"tarock"`, `v`, `profile`, `flags`, `players`, `name` | Beitritt |
| H→G | `welcome` | `seat`, `profile`, `flags`, `players`, `mode`, `state` | Platz zugewiesen, erster Sichtzustand |
| H→G | `redirect` | `address`, `profile`, `players` | Host betreibt einen anderen Tisch |
| H→G | `lobby` | `names`, `humans`, `seat`, `ready` | Wartezimmer |
| H→G | `names` | `names`, `humans` | Namen/Besetzung geändert |
| H→G | `deal` | `s`, `state` | neue Hand, vollständiger Sichtzustand |
| H→G | `act` | `s`, `seat`, `type`, `a`, `b`, `pre`, `reveal` | eine Aktion **mit dem Vorzustand** |
| H→G | `sync` | `s`, `state` | Resynchronisation nach Fehler |
| H→G | `nack` | `s` | Wunsch abgelehnt, Sequenz zurückgesetzt |
| H→G | `result` | `s`, `ledger` | Abrechnung der Hand (vollständig, nichts mehr geheim) |
| G→H | `req` | `s`, `op:"act"`, `type`, `a`, `b` | Aktionswunsch |
| G→H | `req` | `s`, `op:"nextHand"` \| `"newMatch"` | Ablaufwunsch |
| beide | `bye` | `reason` | Verabschiedung |

Das Muster **„Aktion samt Vorzustand“** (`pre`) aus `MultiEngine` bleibt: Der Gast setzt seinen Kern auf
`pre`, wendet die Aktion an und ist damit garantiert synchron; scheitert das, trennt er mit „aus dem Takt
geraten“ und der Host schickt `sync`.

### 8.3 Neu: sichtgefilterte Zustände (L4)

Tarock hat verdeckte Information, die ein Client nie sehen darf (`koenigrufen.md` §2.7 und §8.4,
`hungarian.md` §2.6). Anders als bei Snapszer sendet der Host deshalb **je Empfänger** einen eigenen
Zustand:

```cpp
class LanTable {
    void broadcastAct(int seat, const Action& a) {
        for (const Guest& g : m_guests) {
            QVariantMap m;
            m["t"] = "act";  m["s"] = ++m_seq;  m["seat"] = seat;
            m["type"] = int(a.type);  m["a"] = a.a;  m["b"] = a.b;
            m["pre"]  = QString::fromStdString(m_pre.serializeState(g.hostSeat));  // gefiltert!
            m["reveal"] = revealedBy(a);      // Karten, die diese Aktion öffentlich macht
            m_session->sendTo(g.peer, m);
        }
    }
};
```

`serializeState(viewer)` ersetzt nach `visibleTo()`:

| Information | wer sieht sie |
|---|---|
| eigene Handkarten | nur der Sitz selbst |
| fremde Handkarten | niemand (Ouvertspiel ab dem 2. Ausspiel: alle aktiven Spieler) |
| offene Talonhälften | die vier **aktiven** Spieler; der Fünfte nie (`koenigrufen.md` §2.7, `hungarian.md` §2.5) |
| liegen gelassene Hälfte | aktive Spieler bis zum Ende des 1. Stichs, danach verdeckt |
| verdeckter Talon (Solo, Sechser, Negativ, HU-Talon) | niemand |
| offen verlegte Karten | die vier aktiven Spieler |
| verdeckte Ablage/Skart | nur ihr Eigentümer, bis zur Abrechnung |
| Partneridentität | nur der Partner selbst, bis der gerufene König fällt bzw. die Ansage sie verrät |
| Gebote, Ansagen, Kontras, gespielte Karten, Stichgewinner, Abrechnung | alle, auch der Fünfte |

Unsichtbare Karten werden als `?` mit korrekter Anzahl serialisiert. Der Gast-Kern ist damit `redacted()`
und lehnt jede Aktion ab, die verdeckte Information bräuchte – ein Gast kann also selbst bei manipulierter
App nichts erfahren, was er nicht sehen darf. Der Fünfte (Aussetzer) bekommt eine noch engere Sicht
(`Viewer::Kibitz`) und keinerlei Eingabemöglichkeit.

### 8.4 Lobby, Sitze, Redirect, Ausfall

- Die Lobby zeigt Profil, Spielerzahl (4 oder 5) und alle abweichenden ⚙-Flags (`koenigrufen.md` §8.3:
  jede Hausregel muss beim Beitritt sichtbar sein). Ein Gast mit anderem Profil bekommt `redirect` oder
  eine klare Ablehnung.
- Freie Plätze übernimmt der Computer, wie in Snapszer 1.1.
- Bei Verbindungsverlust spielt ein Computerspieler weiter; beim Wiedereinstieg liefert der Host per
  `welcome` + `state` die vollständige Sicht des Sitzes neu aus (`koenigrufen.md` §8.4).
- Android hält währenddessen `LanService` als Vordergrunddienst plus Wi-Fi- und Wake-Lock.
- Die Partie endet immer am Ende eines Radls, Speichern und Fortsetzen ist mitten in der Partie möglich
  (`koenigrufen.md` §8.2); persistiert werden Sitzordnung, Geber, Radl-Zähler, beide Konten und der
  vollständige Zug-Log der laufenden Hand.

---

## 9. Persistenz und Protokoll

- `QSettings` (Organisation `org.smatkovi`, Anwendung `harbour-tarock`) für Einstellungen, Namen, Lernstufe,
  Deckwahl und den letzten Partiezustand (`serializeState(-1)` als Base64).
- Zusätzlich ein **Ereignisprotokoll** je Hand: `handNumber`, `seed`, `FixedDeal`, dann jede `Action` mit
  Sitz. Daraus lässt sich jede Sicht rekonstruieren – Grundlage für Wiederaufnahme, Rückschau nach der Hand
  und die Nachbesprechung des Lernmodus (`koenigrufen.md` §8.4). Das Protokoll kann als `.tarocklog`
  exportiert und als Übungspartie importiert werden (`Lesson::fromLog()`), womit sich eigene Partien
  in Lektionen verwandeln lassen.

---

## 10. Kartengrafiken – Pipeline und Credits

Grundlage: `rpms/tarock-research.json` → `result.artwork`. Zwei Decks werden ausgeliefert.

### 10.1 Deck A – „Klassisch Wien 1904–12“ (historischer Scan)

Quelle: Volkskundemuseum Wien, IuG-Tarock Ferd. Piatnik & Söhne, ÖMV/87.529, **Public Domain Mark 1.0**,
55 Bilder (54 Karten + Rückseite), ca. 2306×4096 px. Das IIIF-Manifest liegt bereits als
`/home/defaultuser/ps/tarock-assets/manifest.json` vor (55 Canvases, Labels nur `[1]`…`[55]`).

```
tools/assets/
  fetch_iiif.py      # liest manifest.json, lädt <image-id>/full/full/0/default.jpg nach raw/<nn>.jpg,
                     # archiviert METS (accessCondition „CC PDM 1.0“) und Abrufdatum nach CREDITS/licenses/
  mapping.csv        # canvas;cardKey;note     – handgepflegt, da das Manifest keine Kartentitel führt
                     # 01;Sk;Sküs   02;I;Pagat  …  22;XXI;Mond  23;H:K  …  54;C:7  55;BACK
  masks/<cardKey>.png# Retuschemasken: Piatnik-Schutzmarke (♥As), Adressen (♣Bube), Schriftband (♦Bube)
  retouch.py         # Zuschnitt auf das Kartenbild, Entzerrung, Weißabgleich/Farbausgleich gegen
                     # Papieralterung, Inpainting der Masken, abgerundete Ecken mit Alpha
  render_deck.py     # Skalierung 400 px (Standard) und 600 px (HiDPI), JPEG q85,
                     # Ausgabe assets/decks/iug1904/card-<id>.jpg + back.jpg + deck.json
  verify_deck.py     # prüft: 54 Karten + Rückseite vorhanden, gleiche Seitenverhältnisse,
                     # Dateigröße < 90 KB bei 400 px, keine Restpixel in den Maskenbereichen
```

`deck.json`:

```json
{ "id": "iug1904", "name": "Klassisch Wien 1904–12", "deck": 54,
  "file": "card-%1.jpg", "back": "back.jpg", "widths": [400, 600],
  "aspect": 0.5625, "cornerIndex": "overlay",
  "credit": "Volkskundemuseum Wien, ÖMV/87.529, Public Domain Mark 1.0",
  "source": "https://sammlung.volkskundemuseum.at/i3f/v20/201122/manifest",
  "retouched": ["Schutzmarke", "Firmenadressen"], "license": "PDM-1.0" }
```

Auflagen aus der Recherche, die die Pipeline erzwingt:
- Piatnik-Schutzmarke und Firmenadressen **müssen** entfernt werden (`artwork` §1.3); `verify_deck.py`
  schlägt fehl, wenn eine Maske nicht angewandt wurde.
- Herstellername nicht in App-Titel, Store-Text oder Werbung.
- Nur datierte Originale vor ca. 1920 – keine Scans moderner Drucke (`artwork` §1.1).
- Eckindizes werden **nicht** ins Bild gerendert, sondern in `Card.qml` als Overlay gezeichnet
  (Rang oben links/unten rechts, Farbsymbol daneben) – so bleiben Decks austauschbar (`artwork` §5.4).

### 10.2 Deck B – „Modern/Klar“ (generiert, Standard)

```
tools/assets/
  clean_deck.yaml    # Farbpalette, Schrift (Noto Sans / Noto Sans Symbols 2, OFL), Layoutmaße
  gen_clean_deck.py  # erzeugt 54 SVGs: Tarock mit großer römischer Zahl + Vignette,
                     # Hofkarten als Silhouette, Zahlkarten mit Farbsymbolen, Rückseite als Muster
                     # -> danach Rasterung mit rsvg-convert nach PNG 400/600 px
```

- Eigene Lizenz (GPL-3.0-or-later, identisch zum Code), keine Herkunftsfragen.
- **PNG, nicht SVG zur Laufzeit:** Qt SVG beherrscht nur SVG 1.2 Tiny, und Sailfish läuft auf Qt 5.6;
  QtQuick.Shapes ist dort nicht verfügbar (`artwork` §6).
- Motive dürfen nur nach gemeinfreien Vorlagen (Deck A) gezeichnet werden, nie nach modernen Drucken.
- Dieses Deck ist **Standard und Fallback**, auch weil es auf kleinen Displays besser lesbar ist; es dient
  zugleich als Barrierefreiheits-Deck (großer Index, hoher Kontrast, Vierfarb-Option).

### 10.3 Deckwahl in der App

`assets/decks/*/deck.json` wird beim Start gescannt; `Settings` speichert `deck/id`. `Card.qml` lädt
`Paths::data("assets/decks/" + deck + "/card-" + id + ".jpg")` und legt den Index-Overlay darüber. Für das
ungarische Profil werden schlicht 12 der 54 Bilder nicht verwendet – kein zweiter Kartensatz nötig.

### 10.4 Credits

`CREDITS/ASSETS.md` listet je Datei Quelle, URL, Abrufdatum, Lizenz, Urheber und die vorgenommenen Änderungen;
`CREDITS/licenses/` enthält die archivierten Lizenz-Snapshots (METS-`accessCondition`, Commons-Wikitext).
Pro Datei werden REUSE-konforme `.license`-Dateien erzeugt (`artwork` §5.5). Die `CreditsPage` rendert
`ASSETS.md` in der App; die Schriftlizenzen (OFL) stehen darin ebenfalls.

---

## 11. Testplan

Alle Tests sind Konsolenprogramme ohne QtQuick; die LAN-Tests brauchen `Qt::Core`/`Qt::Network` und laufen
über Loopback – genau das Muster von `tests/lan_twoplayer.cpp` und `tests/lan_multiplayer.cpp`.

### 11.1 Kartenmodell – `test_cards.cpp`

- 54 Ids sind bijektiv zu `cardKey()`/`parseCardKey()`.
- `DeckSpec{5}` liefert genau 42 Karten; die niedrigste rote Karte ist das As, die niedrigste schwarze die 10.
- `power()` erzeugt in Rot und Schwarz die Reihenfolgen aus `koenigrufen.md` §1.2 bzw. `hungarian.md` §1.3.
- Kartenwertsummen: KR 54 Karten = 70 Punkte = 210 Drittel; HU 42 Karten = 94 Punkte.

### 11.2 Regeltests je Profil – `test_profile_kr.cpp`, `test_profile_hu.cpp`, `test_scoring.cpp`

Jedes durchgerechnete Beispiel der Specs wird eine benannte Assertion:

| Test | Quelle | Erwartung |
|---|---|---|
| `count_exampleA` | `koenigrufen.md` §1.7 A | 8 Karten → 50 Drittel = 16/2 |
| `count_exampleB` | §1.7 B | 125 + 85 = 210 Drittel, 125 ≥ 107 → gewonnen |
| `threshold_35_1_loses` | §1.6 | 106 Drittel = verloren |
| `bidding_normal`, `bidding_hold`, `bidding_sechser_overbid`, `bidding_forehand_only`, `bidding_vorhand_bid` | §3.6 Beispiele 1–5 | jeweils Spieler und Kontrakt |
| `solodreier_ends_bidding` | §3.5 Endebedingung C | Lizit endet sofort |
| `discard_validation` | §4.6 | Könige/Trull nie; Tarock nur ohne verlegbare Farbkarten; Farbendreier umgekehrt |
| `bonus_values` | §7.3 | Solorufer angesagter König ultimo = 4; Sechser angesagter Uhu = 4 je Gegner; Solorufer stiller Uhu = 4 |
| `sechser_lost_doubles_game_only` | §7.2, §5.5 | Spiel ×2, Prämien ×1 |
| `valat_announced_lost` | §7.4 | Sechserdreier: 4×8×2 = 64 je Gegner |
| `solorufer_king_in_talon_kontra_void` | §4.5 b | alle Kontrastufen = 1 |
| `ledger_zero_sum` | §7.1 | Σ Schrift = 0 und Σ Geld = 0, Fünfter = 0 |
| `hu_talon_distribution` | `hungarian.md` §4.1 | 3/1/1/1 · 2/2/1/1 · 1/2/2/1 · 0/2/2/2 |
| `hu_bidding_hold_new_rule` | §3.4 Beispiel | A wird felvevő mit szóló |
| `hu_no_honour_must_pass` | §3.2 | ohne honőr kein Gebot; `próbahárom` als Ausnahme |
| `hu_multiplier_table` | §7.3 | vollständige Verifikationstabelle Zeile für Zeile |
| `hu_scoring_examples` | §7.5 | die durchgerechneten Beispiele |
| `hu_volat_kills_silent` | §5.4 | stille tulétroá/négykirály entfallen bei volát |
| `hu_tie_47_47` | §1.7 | 47:47 → Gegenpartei gewinnt |
| `hu_hivatalbol_kontra` | §4.5 | Ruf im Skart → allein gegen drei, alle 6 Skarts zum Spielmacher, Spiel ×2 |

### 11.3 Übungspartien als Regressionstests – `test_lessons.cpp`

Für jede Datei in `lessons/`:

1. `deal` bildet genau das Deck ohne Dubletten.
2. Jeder Zug in `steps[].moves` ist nach `TarockCore::check()` legal.
3. Jeder `expectWinner` stimmt mit `trickWinner()` überein, jeder `trickUnits` mit `count()`.
4. Die Kartenpunkte summieren sich auf 210 Drittel (KR) bzw. 94 Punkte (HU).
5. `result.schrift`, `result.geld` und die Prämienliste stimmen mit `settle()` überein.
6. Jeder `traps[].reason` wird vom Kern tatsächlich mit **genau diesem** `ReasonCode` abgelehnt – das hält
   Spec-Texte und Engine dauerhaft synchron.
7. Jeder `anchor` in jedem Text existiert in `rules/*.md`.

Damit sind die drei Übungsspiele aus `koenigrufen.md` §11.7 und die drei aus `hungarian.md` §11.8 zugleich
Lernstoff und Regressionsnetz (`koenigrufen.md` §10.10).

### 11.4 Legalität und Grundcodes – `test_legality.cpp`

- `legalCards()` liefert nie die leere Menge, in keiner erreichbaren Stellung.
- Für jede abgelehnte Karte liefert `check()` einen `ReasonCode ≠ Ok`, und jeder definierte Code wird von
  mindestens einem Testfall erzeugt (Abdeckungsprüfung über `reasonKey()`).
- `ReasonText::format()` findet für **jeden** Code in **jedem** Profil einen Kurz- und einen Langtext
  (fehlender Text = Testfehler), und alle Platzhalter `{…}` sind belegt.
- Die harte Sperre `BoundWinsTooEarly` stützt sich nur auf öffentlich bekannte Karten: Ein Test setzt eine
  Stellung auf, in der der Stichgewinn nur mit Kenntnis fremder Hände feststünde – der Zug muss **erlaubt**
  bleiben (`koenigrufen.md` §6.5/§10.8).

### 11.5 Zufällige Vollspiele – `test_selfplay.cpp`

10 000 Partien je Profil, je 4 und 5 Spieler, mit festen Seeds:

- kein illegaler Zug; jede Hand endet nach 12 (KR) bzw. 9 (HU) Stichen oder korrekt vorzeitig;
- `Ledger::zeroSum()` gilt immer; der Aussetzer bekommt exakt 0;
- nach jeder Aktion `validate()` ohne Fehler;
- Serialisierungs-Rundlauf: `restoreState(serializeState())` liefert einen identischen Zustand (`test_serialize.cpp`);
- **Rotationsprüfung** (`test_rotation.cpp`): Nach `rotateSeats(k)` sind `legalActions`, `settle()` und
  `serializeState(-1)` bis auf die Sitzpermutation identisch; `rotateSeats(k)` gefolgt von
  `rotateSeats(players-k)` ist die Identität;
- **Sichtfilter:** `serializeState(v)` enthält für keinen `v` eine Karte, die `visibleTo()` verneint
  (Prüfung über die `?`-Zähler);
- **Fairness:** über viele Partien darf kein Sitz systematisch im Vorteil sein, außer dem echten
  Vorhandvorteil (`koenigrufen.md` §10.10).

### 11.6 Kopflose LAN-Tests – `lan_tarock4.cpp`, `lan_tarock5.cpp`

Nach dem Muster von `tests/lan_multiplayer.cpp`: ein Host plus zwei bzw. drei Gäste im selben Prozess über
Loopback, freie Plätze vom Computer besetzt.

- Alle Instanzen erreichen nach jeder Aktion denselben öffentlichen Zustand (Vergleich der `serializeState(v)`
  mit den erwarteten Filtern).
- **Kein Leck:** Der Zustand eines Gastes enthält niemals eine fremde Handkarte, keinen verdeckten Talon und
  bei fünf Spielern beim Aussetzer auch keine offene Talonhälfte.
- Sequenznummern: Ein veralteter `req` wird mit `nack` beantwortet, ohne den Kern zu verändern.
- Abbruch und Wiedereinstieg eines Gastes mitten in der Ansagephase führen zur korrekten Sicht.
- `redirect` bei falscher Tischgröße oder falschem Profil.
- 5-Spieler-Test zusätzlich: Der aussetzende Gast bekommt keine Eingabemöglichkeit, und nach `nextHand()`
  wandert der Aussetzerplatz korrekt weiter.

### 11.7 Manuelle Prüfliste je Release

Portrait auf Sailfish (Xperia 10 III) und Android (arm64): 12-Karten-Fächer lesbar, Ansagepanel mit allen
17 ungarischen Figuren scrollbar, Talonwahl und Ablage per Daumen bedienbar, Lernpanel überdeckt den Tisch
nicht, LAN zwischen Sailfish und Android gemischt, Vordergrunddienst überlebt Bildschirmsperre.

---

## 12. Meilensteine

| M | Inhalt | Ergebnis / Abnahme |
|---|---|---|
| **M0** | Gerüst: Repo-Layout, beide CMake-Dateien, `.spec`, Desktop-/Sailjail-Datei, `Style`-Singletons, leere Seiten, `main.cpp` beider Plattformen | RPM und APK bauen und starten, zeigen einen leeren Tisch |
| **M1** | Kartenmodell, `DeckSpec`, Werte und Zählung beider Profile, `RuleProfile`-Tabellen für KR | `test_cards`, `count_example*`, `threshold_*` grün |
| **M2** | `TarockCore` + `KoenigrufenRules`: Geben, Lizitation, Ruf, Talon, Ablage, Ansagen/Kontra, Stichregeln, Abrechnung; `ReasonCode` durchgängig | `test_profile_kr`, `test_scoring`, `test_legality` grün; Konsolenspiel gegen Zufalls-KI läuft durch |
| **M3** | `TarockEngine` + `qml-common`-Tisch: Handfächer, Stich, Sitzpanels, `BidBar`, `TalonPanel`, `DiscardTray`, `AnnouncePanel`, `KontraRow`, Animationsphasen; Sailfish-UI | Königrufen zu viert gegen Computerspieler vollständig spielbar |
| **M4** | Computerspieler: `Inference`, Heuristik für Lizit/Ruf/Talon/Ablage/Ansage, PIMC fürs Kartenspiel, drei Schwierigkeitsgrade | `test_selfplay` 10 000 Partien grün; Spielstärke gegen Heuristik-Baseline messbar besser |
| **M5** | **Lernmodus Teil 1:** `LearnEngine`, Erklärpanel je Phase, „Warum nicht?“ mit allen KR-Texten aus §11.4, Warnungen, Hinweisgeber, `standing()`, Nachbesprechung, Einstellungen | Jede Ablehnung zeigt einen Text; `test_legality`-Textabdeckung grün |
| **M6** | **Lernmodus Teil 2:** Lektionsformat, Loader, `LessonPage`, L0–L7 und drei Übungspartien für KR, Regel-Nachschlagewerk, Glossar, Tariftabelle, `CountTutor` | `test_lessons` grün; Übungsspiel 1 aus §11.7 Schritt für Schritt spielbar |
| **M7** | Fünfertisch und LAN: `LanTable`, Sichtfilter, `rotateSeats` auf fünf Sitze, Lobby, Redirect, Ausfallübernahme, Android-Vordergrunddienst | `lan_tarock4`, `lan_tarock5`, `test_rotation`, `test_serialize` grün; Sailfish↔Android gemischt getestet |
| **M8** | Asset-Pipeline: `fetch_iiif.py`, `mapping.csv`, Retusche, beide Decks, `deck.json`, `CREDITS/ASSETS.md`, Deckwahl | `verify_deck.py` grün; beide Decks in der App wählbar, Credits vollständig |
| **M9** | **Zweites Regelprofil:** `HungarianRules`, Profiltabellen, ⚙-Flags, ungarische Ansage-UI, Lektionen L0–L7 und drei Übungspartien | `test_profile_hu`, `hu_*`-Tests und `test_lessons` (HU) grün |
| **M10** | Android-Feinschliff, Übersetzungen (de/hu/en), Portrait-Prüfliste, Icons, Release-Notizen, RPM + APK | Abnahme nach §11.7; Version 1.0 |

**Reihenfolgebegründung.** Königrufen zuerst, weil es das reichere Regelwerk ist (13 Spiele, Negativspiele,
Farbenspiele) – wer damit fertig ist, hat die Profil-Abstraktion bewiesen, und das ungarische Profil in M9
ist dann überwiegend Datenpflege plus zwei Automaten. Der Lernmodus kommt **vor** LAN und vor dem zweiten
Profil (M5/M6 vor M7/M9), weil er die `ReasonCode`-Disziplin im Kern erzwingt: Jede später ergänzte Regel
muss von Anfang an einen Grund liefern, statt nachträglich mit Texten versehen zu werden. Die Assets
kommen spät (M8), weil bis dahin Platzhalterkarten genügen und die Retusche-Auflagen aus der Recherche
in einem Zug abgearbeitet werden sollen.

---

## 13. Offene Punkte

| # | Punkt | Vorschlag |
|---|---|---|
| O1 | Paket- und Organisationsname (`org.smatkovi.tarock` vs. `org.edp17.*`) | eigener Namensraum, da Eigenentwicklung; vor M0 festlegen |
| O2 | Lizenz des Codes: Snapszer ist GPL-3.0-or-later; der Tarock-Code ist neu geschrieben, übernimmt aber `LanSession` | GPL-3.0-or-later beibehalten |
| O3 | Fünfertisch-Spielzahl je Durchgang (`koenigrufen.md` §8.1 **[OFFEN] M-1**) | 20 Spiele (4 Radl), einstellbar |
| O4 | Trischaken-Wertung mit zwei Jungfrauen erzeugt Drittelbeträge (`koenigrufen.md` §7.6) | `Ledger` in Dritteln führen, erst in der Anzeige runden |
| O5 | Ouvert-Team-Chat im LAN (`koenigrufen.md` §6.4) | in M7 als einfache, vorgefertigte Signalliste statt Freitext |
| O6 | Weitere Profile (`tapptarock.md`, `strohmandeln.md`) | passen ohne Kernänderung in `ProfileRegistry`; nach 1.0 |
| O7 | Übersetzung der Lektionstexte (JSON außerhalb von Linguist) | Overlay-Dateien je Sprache, §7.6 |
