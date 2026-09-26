#!/bin/sh
# Builds the MeeGo Harmattan (Nokia N9 / N950) edition of Tarock on the build
# machine.
#
#   meego/build.sh arm     -> build/meego/arm/harbour-tarock
#   meego/build.sh check   -> build/meego/check/harbour-tarock, der QML-Prüfer
#                             (meego/tests/check-qml.sh baut und ruft ihn auf)
#
# MADDE's own GCC 4.4 cannot compile the C++17 core, so this uses the GCC 14
# cross toolchain from harbour-snapszer/meego/toolchain.sh (XGCC) against the
# MADDE sysroot. moc and lrelease come from the Qt Simulator's Qt 4.7.4, which
# is the version on the device.
set -e

HERE=$(cd "$(dirname "$0")/.." && pwd)
MODE=${1:-arm}
XGCC=${XGCC:-/tmp/xgcc-harmattan}
SYSROOT=${SYSROOT:-$HOME/QtSDK/Madde/sysroots/harmattan_sysroot_10.2011.34-1_slim}
SIMQT=${SIMQT:-$HOME/QtSDK/Simulator/Qt/gcc}
JOBS=${JOBS:-8}
OUT=$HERE/build/meego/$MODE
mkdir -p "$OUT"

ENGINE_SRC="src/TarockEngine.cpp src/LearnEngine.cpp src/Lesson.cpp src/ReasonText.cpp \
 src/RulesIndex.cpp \
 src/core/Action.cpp src/core/Ai.cpp src/core/Card.cpp src/core/Deck.cpp src/core/Reason.cpp \
 src/core/RuleProfile.cpp src/core/Scoring.cpp src/core/TarockCore.cpp \
 src/core/profiles/AtKrOoe2023.cpp src/core/profiles/HuIlluItvb2019.cpp \
 src/core/profiles/AtTappKlassik.cpp src/core/profiles/AtStrohMsErw.cpp \
 src/net/BtLink.cpp src/net/LanSession.cpp src/net/LanTable.cpp"
MOC_HEADERS="src/TarockEngine.h src/LearnEngine.h src/net/BtLink.h src/net/LanSession.h \
 src/net/LanTable.h"

QT4_FLAGS="-std=gnu++17 -O2 -Wall -Wno-register -Wno-deprecated-declarations -Wno-nonnull \
 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DQT_NO_DEBUG \
 -I$HERE/meego/compat -include $HERE/meego/compat/qt4compat.h -I$HERE/src -I$HERE/src/core"
QT4_MODULES="QtCore QtDBus QtGui QtNetwork QtScript QtDeclarative"

case "$MODE" in
arm)
    CXX=$XGCC/bin/arm-none-linux-gnueabi-g++
    [ -x "$CXX" ] || { echo "cross compiler missing: $CXX" >&2; exit 1; }
    MOC=$SIMQT/bin/moc
    QTINC=$SYSROOT/usr/include/qt4
    CXXFLAGS="--sysroot=$SYSROOT $QT4_FLAGS -I$QTINC"
    for m in $QT4_MODULES; do CXXFLAGS="$CXXFLAGS -I$QTINC/$m"; done
    # Hard-float Harmattan still uses ld-linux.so.3; --exclude-libs keeps the
    # static libstdc++/libgcc private so Qt stays on its own GCC 4.4 runtime.
    LDFLAGS="--sysroot=$SYSROOT -static-libstdc++ -static-libgcc -Wl,-O1 -Wl,--as-needed \
 -Wl,--exclude-libs,ALL -Wl,--dynamic-linker=/lib/ld-linux.so.3"
    LIBS="-lQtDeclarative -lQtScript -lQtNetwork -lQtDBus -lQtGui -lQtCore -lpthread"
    ;;
check)
    # Der QML-Prüfer tritt an die Stelle von meego/main.cpp und wird gegen das
    # schlichte Qt des SDK (4.8.1) gebaut: das Qt des Simulators (4.7.4) bricht
    # ohne Bildschirm selbst für ein Programm ohne Oberfläche ab, und das Qt
    # des Geräts läuft hier gar nicht. QtDeclarative 1 liest beide Male
    # dasselbe, und darum geht es.
    CXX=${CXX:-g++}
    PROBEQT=${PROBEQT:-$HOME/QtSDK/Desktop/Qt/4.8.1/gcc}
    [ -x "$PROBEQT/bin/moc" ] || { echo "Qt 4.8 des SDK fehlt: $PROBEQT" >&2; exit 1; }
    MOC=$PROBEQT/bin/moc
    QTINC=$PROBEQT/include
    CXXFLAGS="$QT4_FLAGS -I$QTINC"
    for m in $QT4_MODULES; do CXXFLAGS="$CXXFLAGS -I$QTINC/$m"; done
    LDFLAGS="-L$PROBEQT/lib -Wl,-rpath,$PROBEQT/lib"
    LIBS="-lQtDeclarative -lQtScript -lQtNetwork -lQtDBus -lQtGui -lQtCore -lpthread"
    MAIN=meego/tests/qml_check.cpp
    ;;
*)
    echo "usage: $0 arm|check" >&2; exit 2 ;;
esac
MAIN=${MAIN:-meego/main.cpp}

MK=$OUT/Makefile
{
    echo "CXX=$CXX"; echo "MOC=$MOC"; echo "CXXFLAGS=$CXXFLAGS"
    echo "LDFLAGS=$LDFLAGS"; echo "LIBS=$LIBS"; echo "SRC=$HERE"; echo
    objs=
    for s in $ENGINE_SRC; do
        o=$(basename "$s" .cpp).o; objs="$objs $o"
        echo "$o: \$(SRC)/$s"; printf '\t$(CXX) $(CXXFLAGS) -c $< -o $@\n'
    done
    for h in $MOC_HEADERS; do
        n=$(basename "$h" .h); objs="$objs moc_$n.o"
        echo "moc_$n.cpp: \$(SRC)/$h"; printf '\t$(MOC) $< -o $@\n'
        echo "moc_$n.o: moc_$n.cpp"; printf '\t$(CXX) $(CXXFLAGS) -c $< -o $@\n'
    done
    echo "ENGINE_OBJS=$objs"; echo
    echo "all: harbour-tarock"
    echo "main.moc: \$(SRC)/$MAIN"; printf '\t$(MOC) $< -o $@\n'
    echo "main.o: \$(SRC)/$MAIN main.moc"; printf '\t$(CXX) $(CXXFLAGS) -I. -c $< -o $@\n'
    echo "harbour-tarock: main.o \$(ENGINE_OBJS)"
    printf '\t$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)\n'
} > "$MK"

nice make -C "$OUT" -j"$JOBS" all

if [ "$MODE" = check ]; then
    echo "== built $OUT/harbour-tarock (der QML-Prüfer)"
    exit 0
fi

# Translations. Qt 4.7's lrelease only knows TS version 2.0 while the
# catalogues say 2.1, so the header is rewritten on the way in.
LRELEASE=$SIMQT/bin/lrelease
mkdir -p "$OUT/translations"
for lang in de hu; do
    src=$HERE/translations/harbour-tarock-$lang.ts
    [ -f "$src" ] || continue
    sed 's/<TS version="2\.1"/<TS version="2.0"/' "$src" > "$OUT/harbour-tarock-$lang.ts"
    # The same Latin-1 substitution the QML got, so the keys still match.
    python3 "$HERE/meego/ascii-qstr.py" ts "$OUT/harbour-tarock-$lang.ts" >/dev/null
    "$LRELEASE" -silent "$OUT/harbour-tarock-$lang.ts" \
        -qm "$OUT/translations/harbour-tarock-$lang.qm"
done

echo "== built $OUT/harbour-tarock"
