#!/usr/bin/env python3
"""Schneidet die Karten aus den Scans und nimmt die Herstellerzeichen heraus.

    tools/assets/retouch.py [--raw DIR] [--out DIR]

Aus jedem Scan (Karte auf hellem Hintergrund) wird die Karte selbst
ausgeschnitten. Zwei Karten tragen Zeichen des Herstellers, die laut
docs/design.md §10.1 nicht mitgeliefert werden dürfen: die Schutzmarke auf dem
Herz-As und die Firmenadresse auf dem Kreuz-Buben (dieselbe Schrift steht
kleiner auf dem Karo-Buben). Sie werden mit dem Papierton der Umgebung
überdeckt -- das Blatt selbst ist gemeinfrei, die Marke ist es nicht.

Das Ergebnis ist ein gerade geschnittenes, nicht skaliertes PNG je Karte;
render_deck.py macht daraus die Größen, die die App ausliefert.
"""
import argparse
import csv
import os
import sys

from PIL import Image, ImageDraw, ImageFilter


def card_box(image):
    """Der Kasten der Karte im Scan.

    Das Papier ist cremefarben, der Hintergrund des Scans neutral bis
    bläulich: die Differenz von Rot und Blau trennt beide sauber, ohne dass
    eine feste Farbe geraten werden müsste.
    """
    small = image.convert("RGB").resize((image.width // 8, image.height // 8))
    red, _green, blue = small.split()
    warm = Image.eval(Image.merge("L", [red]), lambda value: value)
    difference = Image.new("L", small.size)
    difference.putdata([max(0, r - b) for r, b in zip(warm.getdata(), blue.getdata())])
    mask = difference.point(lambda value: 255 if value > 12 else 0)
    mask = mask.filter(ImageFilter.MedianFilter(5))
    box = mask.getbbox()
    if not box:
        return None
    left, top, right, bottom = [value * 8 for value in box]
    # Ein Hauch Rand bleibt stehen, damit die abgerundeten Ecken nicht
    # angeschnitten werden.
    margin = max(2, image.width // 200)
    return (max(0, left - margin), max(0, top - margin),
            min(image.width, right + margin), min(image.height, bottom + margin))


def ring_colour(image, box):
    """Die Farbe rundum den Fleck, aus dem Ring knapp außerhalb.

    Auf dem Herz-As ist das Papier, auf den Buben die Fläche des Bandes, auf
    dem die Adresse steht -- so bleibt ein leeres Band stehen statt eines
    beigen Klebers mitten im Bild.
    """
    left, top, right, bottom = box
    pad = max(3, (right - left) // 12)
    outer = image.crop((max(0, left - pad), max(0, top - pad),
                        min(image.width, right + pad), min(image.height, bottom + pad)))
    inner = (pad, pad, outer.width - pad, outer.height - pad)
    pixels = []
    data = outer.convert("RGB").load()
    for y in range(outer.height):
        for x in range(outer.width):
            if inner[0] <= x < inner[2] and inner[1] <= y < inner[3]:
                continue
            pixels.append(data[x, y])
    if not pixels:
        return (235, 225, 200)
    pixels.sort(key=lambda rgb: rgb[0] + rgb[1] + rgb[2])
    return pixels[len(pixels) // 2]


def light_colour(image, box):
    """Der helle Grundton **innerhalb** eines Bereichs.

    Auf den Bändern der Buben steht dunkle Schrift auf heller Fläche; der
    Mittelwert wäre grau, der obere Rand der Helligkeit trifft das Band.
    """
    pixels = list(image.crop(box).convert("RGB").getdata())
    pixels.sort(key=lambda rgb: rgb[0] + rgb[1] + rgb[2])
    return pixels[int(len(pixels) * 0.8)]


# Bereiche in Anteilen der Kartenbreite/-höhe, die überdeckt werden müssen,
# und wie. Abgelesen an den Scans; siehe docs/design.md §10.1.
#   patch   -- ein Stück sauberes Papier von woanders auf derselben Karte,
#              mit dem Versatz dahinter; das sieht man nicht
#   ellipse -- ein Oval im Ton der Umgebung, für die Schilde der Buben
#   light   -- der helle Grundton des Bandes, für die Schriftbänder
MASKS = {
    # Herz-As: Pferd, Firmenname und "Registrirte Schutzmarke". Darüber liegt
    # leeres Papier, das genau so aussieht wie das darunter.
    "35": [("patch", (0.25, 0.615, 0.75, 0.915), (0.0, -0.56))],
    # Kreuz-Bube: zwei Schilde mit den Adressen in Wien und Budapest
    "50": [("ellipse", (0.615, 0.195, 0.885, 0.445), None),
           ("ellipse", (0.115, 0.535, 0.385, 0.775), None)],
    # Karo-Bube: zwei Bänder mit "Ferd. Piatnik" und "& Söhne."
    "26": [("light", (0.620, 0.215, 0.815, 0.450), None),
           ("light", (0.180, 0.495, 0.375, 0.780), None)],
}


def mask_regions(card, canvas):
    for shape, relative, extra in MASKS.get(canvas, []):
        box = (int(relative[0] * card.width), int(relative[1] * card.height),
               int(relative[2] * card.width), int(relative[3] * card.height))
        if shape == "patch":
            dx = int(extra[0] * card.width)
            dy = int(extra[1] * card.height)
            source = (box[0] + dx, box[1] + dy, box[2] + dx, box[3] + dy)
            patch = card.crop(source)
            mask = Image.new("L", patch.size, 255)
            # Weiche Kante, damit der Übergang im Papier verschwindet.
            border = max(4, patch.size[0] // 20)
            inner = Image.new("L", (patch.size[0] - 2 * border, patch.size[1] - 2 * border), 255)
            mask = Image.new("L", patch.size, 0)
            mask.paste(inner, (border, border))
            mask = mask.filter(ImageFilter.GaussianBlur(border / 2.0))
            card.paste(patch, box[:2], mask)
            continue
        colour = light_colour(card, box) if shape == "light" else ring_colour(card, box)
        patch = Image.new("RGB", (box[2] - box[0], box[3] - box[1]), colour)
        mask = Image.new("L", patch.size, 0)
        drawing = ImageDraw.Draw(mask)
        if shape == "ellipse":
            drawing.ellipse((0, 0, patch.size[0] - 1, patch.size[1] - 1), fill=255)
        else:
            drawing.rectangle((0, 0, patch.size[0] - 1, patch.size[1] - 1), fill=255)
        mask = mask.filter(ImageFilter.GaussianBlur(3))
        card.paste(patch, box[:2], mask)
    return card


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser()
    parser.add_argument("--raw", default=os.path.expanduser("~/tarock-assets/raw"))
    parser.add_argument("--out", default=os.path.expanduser("~/tarock-assets/cards"))
    parser.add_argument("--mapping", default=os.path.join(here, "mapping.csv"))
    args = parser.parse_args()

    os.makedirs(args.out, exist_ok=True)
    with open(args.mapping, encoding="utf-8") as handle:
        rows = [row for row in csv.reader(handle, delimiter=";")
                if row and not row[0].startswith("#")]

    for canvas, cardId, key, note in rows:
        source = os.path.join(args.raw, "%s.jpg" % canvas)
        if not os.path.exists(source):
            print("fehlt: %s" % source, file=sys.stderr)
            return 1
        image = Image.open(source).convert("RGB")
        box = card_box(image)
        if not box:
            print("keine Karte gefunden in %s" % source, file=sys.stderr)
            return 1
        card = image.crop(box)
        card = mask_regions(card, canvas)
        name = "back" if cardId == "-1" else "card-%s" % cardId
        card.save(os.path.join(args.out, "%s.png" % name))
        print("%s -> %s  %dx%d" % (canvas, name, card.width, card.height))
    return 0


if __name__ == "__main__":
    sys.exit(main())
