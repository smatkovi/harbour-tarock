Name: harbour-tarock
Version: 0.1.0
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

This early version is the project skeleton: it starts and shows the table.
The games themselves are added release by release.

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

# Card decks; filled by the asset pipeline (docs/design.md §10). The lessons
# and the rule reference are installed here as well once they exist.
mkdir -p %{buildroot}/usr/share/%{name}/assets
cp -a assets/decks %{buildroot}/usr/share/%{name}/assets/

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
* Sat Sep 12 2026 smatkovi - 0.1.0-1
- Project skeleton: builds and starts on Sailfish OS and Android, shows an empty table
