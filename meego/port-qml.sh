#!/bin/sh
# Converts the shared QML of qml-common/ to QtQuick 1.1 in meego/qml/.
#
# The shared files are copied, never edited in place: Sailfish and Android keep
# QtQuick 2.6. A scan of all 34 QML files found only three constructs that
# QtQuick 1.1 does not have -- no Canvas, no Layouts, no Drag/DropArea, no
# arrow functions -- so this is a rewrite of three spellings and nothing else:
#
#   import QtQuick 2.6   ->  import QtQuick 1.1
#   property var         ->  property variant
#   readonly property    ->  property          (1.1 has no readonly)
#
# "pragma Singleton" is handled elsewhere: meego/main.cpp instantiates Style,
# Prefs and Theme and puts them in the root context under those same names, so
# the 326 "Style." references in the shared QML did not have to change.
set -e
cd "$(dirname "$0")/.."
OUT=meego/qml

for f in qml-common/*.qml; do
    name=$(basename "$f")
    sed -e 's/^import QtQuick 2\.[0-9]*$/import QtQuick 1.1/' \
        -e 's/\breadonly property /property /g' \
        -e 's/\bproperty var /property variant /g' \
        "$f" > "$OUT/$name"
done
# Qt 4.7 pushes the source text of a translation through Latin-1, so an em
# dash inside qsTr() arrives mangled. Plain literals are fine and are left
# alone; see meego/ascii-qstr.py.
# The platform pages come from android/qml, not from qml-common, and have to
# be regenerated here too -- otherwise a fix to the scripts below never
# reaches them.
python3 meego/port-platform.py

# Qt 4.7's QML parser has no adjacent string literals.
python3 meego/join-strings.py "$OUT"/*.qml "$OUT"/context/*.qml

# QtQuick.Controls spellings com.nokia.meego does not have.
python3 meego/fix-meego.py "$OUT"/*.qml

# qml-common hands functions around as property values; QtQuick 1.1 cannot
# hold one, so they become signals here.
python3 meego/fix-callbacks.py "$OUT"/*.qml

python3 meego/ascii-qstr.py qml "$OUT"/*.qml "$OUT"/context/*.qml

echo "== $(ls qml-common/*.qml | wc -l) shared files -> $OUT"
