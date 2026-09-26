#!/usr/bin/env python3
"""Prüft das fertige Deck, bevor es ins Paket geht (docs/design.md §10.1).

    tools/assets/verify_deck.py [--deck DIR]

Geprüft wird: alle 54 Karten und die Rückseite in beiden Breiten da, gleiches
Seitenverhältnis, kein Bild schwerer als 90 KB bei 400 px, und die Bereiche,
in denen Schutzmarke und Firmenadresse standen, sind wirklich überdeckt --
dort darf nichts Dunkles mehr stehen.
"""
import argparse
import json
import os
import sys

from PIL import Image, ImageStat

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from retouch import MASKS   # noqa: E402  - dieselbe Wahrheit für beide Seiten

MASKED_CARDS = {"35": "card-26", "50": "card-49", "26": "card-33"}


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser()
    parser.add_argument("--deck", default=os.path.join(here, "..", "..", "assets", "decks", "iug1904"))
    args = parser.parse_args()
    deck = os.path.abspath(args.deck)
    with open(os.path.join(deck, "deck.json"), encoding="utf-8") as handle:
        meta = json.load(handle)

    failures = []
    ratios = []
    for cardId in list(range(54)) + ["back"]:
        stem = "back" if cardId == "back" else "card-%d" % cardId
        for width in meta["widths"]:
            suffix = "" if width == meta["widths"][0] else "@%d" % width
            path = os.path.join(deck, "%s%s.jpg" % (stem, suffix))
            if not os.path.exists(path):
                failures.append("fehlt: %s" % os.path.basename(path))
                continue
            image = Image.open(path)
            ratios.append(image.width / float(image.height))
            if width == meta["widths"][0] and os.path.getsize(path) > 90 * 1024:
                failures.append("zu schwer: %s (%d KB)"
                                % (os.path.basename(path), os.path.getsize(path) // 1024))
    if ratios and (max(ratios) - min(ratios)) > 0.02:
        failures.append("die Seitenverhältnisse gehen auseinander: %.3f .. %.3f"
                        % (min(ratios), max(ratios)))

    # Die überdeckten Stellen: dort darf kein Druck mehr stehen. Papier ist
    # hell und gleichmäßig, Schrift macht die Streuung groß.
    for canvas, stem in MASKED_CARDS.items():
        path = os.path.join(deck, "%s.jpg" % stem)
        if not os.path.exists(path):
            continue
        image = Image.open(path).convert("L")
        for _shape, relative, _extra in MASKS.get(canvas, []):
            # Die Mitte des Bereichs, nicht seine Ecken: ein Oval berührt die
            # Ecken nie, und dort steht noch das Bild.
            width = relative[2] - relative[0]
            height = relative[3] - relative[1]
            box = (int((relative[0] + width * 0.25) * image.width),
                   int((relative[1] + height * 0.25) * image.height),
                   int((relative[2] - width * 0.25) * image.width),
                   int((relative[3] - height * 0.25) * image.height))
            stat = ImageStat.Stat(image.crop(box))
            # Papier wie Bandfläche sind hell und gleichmäßig; Schrift
            # macht die Streuung groß.
            if stat.stddev[0] > 14:
                failures.append("%s: der Bereich %s sieht noch bedruckt aus "
                                "(Mittel %.0f, Streuung %.0f)"
                                % (stem, relative, stat.mean[0], stat.stddev[0]))

    for line in failures:
        print("FEHLT: %s" % line)
    print("Deck %s: %s" % (meta["id"], "in Ordnung" if not failures else "%d Mängel" % len(failures)))
    return 0 if not failures else 1


if __name__ == "__main__":
    sys.exit(main())
