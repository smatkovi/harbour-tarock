#!/usr/bin/env python3
"""Macht aus den zugeschnittenen Karten das Deck, das die App ausliefert.

    tools/assets/render_deck.py [--cards DIR] [--out DIR]

Zwei Breiten: 400 px für die üblichen Bildschirme, 600 px für die dichten.
JPEG q85 -- Alpha braucht es nicht, die abgerundeten Ecken zeichnet Card.qml
selbst, und ein PNG dieser Größe wäre zehnmal so schwer.
"""
import argparse
import json
import os
import sys

from PIL import Image

# Nur eine Breite wird ausgeliefert: 400 px reichen für jede Karte auf dem
# Tisch, und jede weitere Größe verdoppelt das Paket.
WIDTHS = [400]


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser()
    parser.add_argument("--cards", default=os.path.expanduser("~/tarock-assets/cards"))
    parser.add_argument("--out", default=os.path.join(here, "..", "..", "assets", "decks", "iug1904"))
    args = parser.parse_args()

    out = os.path.abspath(args.out)
    os.makedirs(out, exist_ok=True)
    names = sorted(os.listdir(args.cards))
    if len(names) != 55:
        print("erwartet sind 55 Bilder, gefunden %d" % len(names), file=sys.stderr)
        return 1

    aspect = None
    for name in names:
        image = Image.open(os.path.join(args.cards, name)).convert("RGB")
        ratio = image.width / float(image.height)
        aspect = ratio if aspect is None else (aspect + ratio) / 2
        stem = os.path.splitext(name)[0]
        for width in WIDTHS:
            height = int(round(width / ratio))
            scaled = image.resize((width, height), Image.LANCZOS)
            suffix = "" if width == WIDTHS[0] else "@%d" % width
            # Die Rückseite ist ein dichtes Muster und wird bei q85 schwerer
            # als jede Karte; ihr steht etwas weniger gut.
            quality = 78 if stem == "back" else 85
            scaled.save(os.path.join(out, "%s%s.jpg" % (stem, suffix)),
                        "JPEG", quality=quality, optimize=True, progressive=True)

    deck = {
        "id": "iug1904",
        "name": "Klassisch Wien 1904–12",
        "deck": 54,
        "file": "card-%1.jpg",
        "back": "back.jpg",
        "widths": WIDTHS,
        "aspect": round(aspect, 4),
        "cornerIndex": "overlay",
        "credit": "Volkskundemuseum Wien, ÖMV/87.529, Public Domain Mark 1.0",
        "source": "https://sammlung.volkskundemuseum.at/i3f/v20/201122/manifest",
        "retouched": ["Schutzmarke", "Firmenadressen"],
        "license": "PDM-1.0",
    }
    with open(os.path.join(out, "deck.json"), "w", encoding="utf-8") as handle:
        json.dump(deck, handle, ensure_ascii=False, indent=2)
        handle.write("\n")
    with open(os.path.join(out, "LICENSE"), "w", encoding="utf-8") as handle:
        handle.write(
            "Tarockkartensatz \"Industrie und Glück\"\n"
            "Ferd. Piatnik & Söhne, Wien, um 1904-1912\n"
            "Sammlung: Volkskundemuseum Wien, Inventarnummer ÖMV/87.529\n"
            "Rechte am Digitalisat: Public Domain Mark 1.0 -- gemeinfrei, keine\n"
            "bekannten urheberrechtlichen Beschränkungen.\n"
            "Manifest: https://sammlung.volkskundemuseum.at/i3f/v20/201122/manifest\n\n"
            "Geändert gegenüber dem Digitalisat: zugeschnitten, skaliert, und die\n"
            "Schutzmarke des Herstellers sowie seine Firmenadressen entfernt\n"
            "(tools/assets/retouch.py). Der Herstellername erscheint deshalb weder\n"
            "auf den Karten noch im Namen oder Text dieser App.\n")
    print("%d Karten in %s, Seitenverhältnis %.4f" % (len(names), out, deck["aspect"]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
