#!/bin/sh
# Builds the MeeGo Harmattan (Nokia N9 / N950) edition of Tarock on the build
# machine.
#
#   meego/build.sh arm     -> build/meego/arm/harbour-tarock
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
 src/core/profiles/AtKrOoe2023.cpp src/core/profiles/HuIlluItvb2019.cpp"
MOC_HEADERS="src/TarockEngine.h src/LearnEngine.h"

QT4_FLAGS="-std=gnu++17 -O2 -Wall -Wno-register -Wno-deprecated-declarations -Wno-nonnull \
 -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DQT_NO_DEBUG \
 -I$HERE/meego/compat -include $HERE/meego/compat/qt4compat.h -I$HERE/src -I$HERE/src/core"
QT4_MODULES="QtCore QtGui QtNetwork QtScript QtDeclarative"

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
    LIBS="-lQtDeclarative -lQtScript -lQtNetwork -lQtGui -lQtCore -lpthread"
    ;;
*)
    echo "usage: $0 arm" >&2; exit 2 ;;
esac

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
    echo "main.moc: \$(SRC)/meego/main.cpp"; printf '\t$(MOC) $< -o $@\n'
    echo "main.o: \$(SRC)/meego/main.cpp main.moc"; printf '\t$(CXX) $(CXXFLAGS) -I. -c $< -o $@\n'
    echo "harbour-tarock: main.o \$(ENGINE_OBJS)"
    printf '\t$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)\n'
} > "$MK"

nice make -C "$OUT" -j"$JOBS" all
echo "== built $OUT/harbour-tarock"
