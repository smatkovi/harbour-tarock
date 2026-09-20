Name: harbour-tarock
Version: 0.2.3
Release: 1
Summary: Königrufen and Hungarian Tarokk with a learning mode
License: GPL-3.0-or-later
URL: https://github.com/smatkovi/harbour-tarock
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root

Requires:       sailfishsilica-qt5
BuildRequires:  pkgconfig(sailfishapp)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  qt5-qttools-linguist

%description
Tarock is a native Sailfish OS implementation of the Austrian and Hungarian
tarock games played with the 54-card Industrie und Glück pack: Königrufen,
Hungarian Tarokk, Tapp- and Dreiertarock and Strohmandeln. Play against
computer opponents or against other phones in the same network, and let the
built-in learning mode explain every bid, every discard and every trick.

This version plays Königrufen at a table of four against computer opponents
and carries the learning mode in full: the reason behind every refusal, hints,
the rule reference, the glossary and a tutorial that walks through the table
and then through the course. LAN play, the five-seat table and the Hungarian
profile follow release by release.

%prep
%setup -q

%build
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/bin
install -m 755 build/harbour-tarock %{buildroot}/usr/bin/

# Platform QML, shared QML and the qmldir that declares the Style singleton
# all end up in one directory, the way the imports expect them.
mkdir -p %{buildroot}/usr/share/%{name}/qml
cp -a sailfish/*.qml sailfish/qmldir qml-common/*.qml %{buildroot}/usr/share/%{name}/qml/
cp -a sailfish/icons %{buildroot}/usr/share/%{name}/qml/

# Card decks; filled by the asset pipeline (docs/design.md §10).
mkdir -p %{buildroot}/usr/share/%{name}/assets
cp -a assets/decks %{buildroot}/usr/share/%{name}/assets/

# The lessons of the learning mode (docs/design.md §7.6); LearnEngine looks
# for them here first on the device.
mkdir -p %{buildroot}/usr/share/%{name}/lessons
cp -a assets/lessons/* %{buildroot}/usr/share/%{name}/lessons/

mkdir -p %{buildroot}/usr/share/%{name}/translations
install -m 644 build/harbour-tarock-de.qm %{buildroot}/usr/share/%{name}/translations/
install -m 644 build/harbour-tarock-hu.qm %{buildroot}/usr/share/%{name}/translations/

mkdir -p %{buildroot}/usr/share/applications
install -m 644 sailfish/desktop/%{name}.desktop %{buildroot}/usr/share/applications/%{name}.desktop

for size in 86 108 128 172 256; do
    mkdir -p %{buildroot}/usr/share/icons/hicolor/${size}x${size}/apps
    install -m 644 sailfish/icons/icon-${size}.png \
        %{buildroot}/usr/share/icons/hicolor/${size}x${size}/apps/%{name}.png
done

# cp -a keeps the group-writable permissions of the working tree; the package
# wants plain 755 directories and 644 files (rpmlint non-standard-dir-perm).
find %{buildroot}/usr/share/%{name} -type d -exec chmod 755 {} \;
find %{buildroot}/usr/share/%{name} -type f -exec chmod 644 {} \;

mkdir -p %{buildroot}/usr/share/doc/%{name}
install -m 644 README.md %{buildroot}/usr/share/doc/%{name}/
install -m 644 CREDITS/ASSETS.md %{buildroot}/usr/share/doc/%{name}/
mkdir -p %{buildroot}/usr/share/licenses/%{name}
install -m 644 LICENSE %{buildroot}/usr/share/licenses/%{name}/

%files
%defattr(-,root,root,-)
/usr/bin/%{name}
/usr/share/%{name}
/usr/share/icons/hicolor/*/apps/%{name}.png
/usr/share/applications/%{name}.desktop
/usr/share/doc/%{name}
/usr/share/licenses/%{name}

%changelog
* Sat Sep 12 2026 smatkovi - 0.2.3-1
- The learning mode reaches the interface at all again: the `learn` property had
  no metatype, so QML saw `undefined` — the tutorial listed no lessons, and the
  explanation bar, the hints, the "why not?" dialog and the lesson band were all
  silently switched off
- tests/test_qmlbridge.cpp checks the engine contract from a real QQmlEngine

* Sat Sep 12 2026 smatkovi - 0.2.2-1
- Settings are stored where the Sailjail sandbox allows it again: the learning
  level, the tutorial progress and a saved match survive the next start
- "Learn to play" on the start page, opening with the overview of goal, run of
  a hand and rules
- The interface is German throughout; the lesson band, the panels and the
  dialogs were still English

* Sat Sep 12 2026 smatkovi - 0.2.1-1
- An overview lesson for both rule profiles: the goal of the game and the whole
  hand, from the deal to the settlement
- The shipped lessons load again: traps without a card, the long form of "auto"
  and the named Kontra posten are understood as documented
- tests/test_lessons.cpp plays every lesson through

* Sat Sep 12 2026 smatkovi - 0.2.0-1
- Tutorial: a guided tour of the table and the course as one chain with progress
- Learning mode: reasons, hints, lessons, rule reference and glossary
- Königrufen playable at a table of four against computer opponents

* Sat Sep 12 2026 smatkovi - 0.1.0-1
- Project skeleton: builds and starts on Sailfish OS and Android, shows an empty table
