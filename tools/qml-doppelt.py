#!/usr/bin/env python3
"""Sucht Eigenschaften, die im selben Element zweimal gesetzt werden.

    tools/qml-doppelt.py [DATEI ...]      (ohne Angabe: alle QML des Baums)

QML verzeiht das nicht: "Property value set multiple times" lässt das ganze
Bauteil nicht laden -- und wer es nicht an der richtigen Stelle ausprobiert,
merkt es erst am leeren Tisch. Genau so ist es mit den Kartenbildern passiert
(0.4.0 bis 0.4.2): `visible` stand zweimal in Card.qml, und damit gab es auf
keiner Plattform mehr eine Karte.
"""
import glob
import os
import re
import sys

SKIP = ("property", "function", "import", "signal", "on")


def check(path):
    problems = []
    scopes = [{}]
    # In eckigen Klammern steht JavaScript, keine QML-Elemente: ein
    # `model: [{ title: ... }, { title: ... }]` setzt nichts doppelt.
    brackets = 0
    for number, raw in enumerate(open(path, encoding="utf-8"), 1):
        line = raw.split("//")[0]
        # Ein Element beginnt mit "{" und endet mit "}"; mehr Struktur braucht
        # es für diese Frage nicht.
        name = None
        match = re.match(r"\s*([a-zA-Z_][\w.]*)\s*:", line)
        if match and "{" not in line:
            name = match.group(1)
        for char in line:
            if char == "[":
                brackets += 1
            elif char == "]" and brackets > 0:
                brackets -= 1
            elif char == "{" and brackets == 0:
                scopes.append({})
            elif char == "}" and brackets == 0 and len(scopes) > 1:
                scopes.pop()
        if name and brackets == 0 and name.split(".")[0] not in SKIP:
            if name in scopes[-1]:
                problems.append((number, name, scopes[-1][name]))
            else:
                scopes[-1][name] = number
    return problems


def main():
    files = sys.argv[1:]
    if not files:
        here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        for folder in ("sailfish", "android/qml", "meego/qml", "qml-common"):
            files.extend(sorted(glob.glob(os.path.join(here, folder, "*.qml"))))
    bad = 0
    for path in files:
        for number, name, first in check(path):
            print("%s:%d: %s steht schon in Zeile %d" % (path, number, name, first))
            bad += 1
    if bad:
        print("%d doppelte Zuweisungen" % bad)
    else:
        print("keine doppelten Zuweisungen in %d Dateien" % len(files))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
