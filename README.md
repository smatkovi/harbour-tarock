# harbour-tarock

Tarock for Sailfish OS and Android: the Austrian and Hungarian games played
with the 54-card *Industrie und Glück* pack, with computer opponents, play over
the local network and a learning mode that explains every decision.

The app is being written against four written rule specifications that live in
`docs/`; every rule it enforces can be traced back to a numbered section in
them.

## What it will be

- **Königrufen** (Upper Austrian rules, profile `AT-KR-OOE-2023-04`) at a table
  of four or five, with the full contract ladder, the called king, the talon,
  announcements, Kontra and the usual bonuses.
- **Hungarian Tarokk** (*Illusztrált*, profile `HU-ILLU-ITVB-2019`) with the
  42-card pack, its own bidding, the distributed talon, the figures and the
  bound cards.
- **Tapp- and Dreiertarock** and **Strohmandeln** — specified in `docs/` and
  planned after 1.0; the rule profiles are a data structure, so they need no
  change to the engine core.
- **Computer players** whose decisions are explainable: the same evaluation
  that picks the opponent's card also produces the hints you get.
- **Play over the local network** for two to five devices, Sailfish OS and
  Android mixed, host-authoritative, with IPv6 addresses accepted directly.
  No account, no matchmaking server, no telemetry.
- **Learning mode** as a first-class feature rather than a help text: every
  rejected move states its reason, every phase can be explained, and a set of
  practice hands teaches the games step by step.
- **Two card decks**: a retouched historical *Industrie und Glück* scan and a
  generated high-contrast deck, which is also the accessibility deck.

## Status

Milestones **M0 to M6** of `docs/design.md` §12: Königrufen is playable at a
table of four against computer opponents, with the full learning mode — the
reasons behind every refusal, hints, the rule reference and the glossary, the
eight course modules and the three practice hands. On top of that sits the
tutorial of §7.9: a guided tour of the table and the course as one chain that
remembers how far you have come.

Still open: the computer players of M4 are the heuristic ones, LAN play and the
five-seat table (M7), the card decks (M8) and the Hungarian profile (M9).

## Layout

| Path | Contents |
|---|---|
| `docs/` | the design document and the four rule specifications |
| `src/` | the Sailfish entry point and the C++ engine; the Qt-free rule core moves into `src/core/` in M1 |
| `qml-common/` | QML shared by both platforms — QtQuick 2.6 and ES5, no Silica and no Controls imports |
| `assets/lessons/` | the course: `index.json` per rule profile plus one file per lesson |
| `sailfish/` | Silica pages, the `Style` singleton, `qmldir`, desktop file and icons |
| `android/` | Qt 6 build, `main.cpp`, the Android package and the mirror of the platform QML |
| `assets/` | card decks, added in M8 |
| `tests/` | rule, lesson and LAN tests |
| `tools/` | build-time helpers; not shipped |
| `translations/` | German and Hungarian message catalogues |

## Building

### Sailfish OS

Needs the Sailfish SDK (Qt 5.6, `sailfishapp`, `qt5-qttools-linguist`). Inside
the build engine, from the source directory:

    sfdk config --push target SailfishOS-4.6.0.15-aarch64
    sfdk build

or, to get an RPM from a plain checkout:

    mb2 -t SailfishOS-4.6.0.15-aarch64 build

To build by hand instead:

    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)

Add `-DTAROCK_BUILD_TESTS=ON` to build the console tests and run them with
`ctest`.

### Android

Needs Qt 6.9 or newer for Android (arm64), a host Qt of the same version for
`lrelease`, the Android SDK with platform 36 and the NDK:

    cd android
    ~/Qt/6.9.0/android_arm64_v8a/bin/qt-cmake -S . -B build \
        -DQT_HOST_PATH=~/Qt/6.9.0/gcc_64 \
        -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target apk

The APK is written to `build/android-build/build/outputs/apk/`.

## Licence

GPL-3.0-or-later, Copyright 2026 smatkovi. See `LICENSE`.

Asset credits are collected in `CREDITS/ASSETS.md`.
