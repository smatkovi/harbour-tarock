#!/usr/bin/env python3
"""Generate the placeholder application icon.

Draws two stylised tarock cards on a green table and writes the Sailfish icon
sizes plus the Android drawable. The real artwork is produced in M8 together
with the card decks (docs/design.md §10); this only exists so that the package
has an icon at all.

Usage:  python3 tools/make_icon.py
"""

import os

from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SAILFISH_SIZES = (86, 108, 128, 172, 256)

TABLE = (11, 93, 54, 255)      # the table green also used by the QML table
CARD = (245, 240, 228, 255)    # aged card stock
CARD_EDGE = (120, 104, 76, 255)
INK = (26, 26, 26, 255)
GOLD = (176, 137, 58, 255)

SUPERSAMPLE = 4


def font(size):
    for path in ("/usr/share/fonts/dejavu/DejaVuSerif-Bold.ttf",
                 "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf"):
        if os.path.exists(path):
            return ImageFont.truetype(path, size)
    return ImageFont.load_default()


def centred(draw, box, text, fnt, fill):
    left, top, right, bottom = draw.textbbox((0, 0), text, font=fnt)
    x = box[0] + (box[2] - box[0] - (right - left)) / 2 - left
    y = box[1] + (box[3] - box[1] - (bottom - top)) / 2 - top
    draw.text((x, y), text, font=fnt, fill=fill)


def fitted_font(draw, text, max_width, start):
    """Largest font size at which `text` still fits into `max_width`."""
    size = start
    while size > 8:
        fnt = font(size)
        left, _, right, _ = draw.textbbox((0, 0), text, font=fnt)
        if right - left <= max_width:
            return fnt
        size -= 2
    return font(8)


def blank_card(width, height):
    image = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    radius = int(width * 0.10)
    draw.rounded_rectangle((0, 0, width - 1, height - 1), radius=radius,
                           fill=CARD, outline=CARD_EDGE, width=max(2, width // 40))
    inset = int(width * 0.09)
    draw.rounded_rectangle((inset, inset, width - inset, height - inset),
                           radius=int(radius * 0.7), outline=GOLD, width=max(2, width // 56))
    return image, draw, inset


def face_card(width, height, numeral):
    """A tarock trump seen from the front: big roman numeral plus corner index."""
    image, draw, inset = blank_card(width, height)

    body = fitted_font(draw, numeral, (width - 2 * inset) * 0.82, int(height * 0.30))
    centred(draw, (0, int(height * 0.20), width, int(height * 0.80)), numeral, body, INK)

    index = fitted_font(draw, numeral, width * 0.30, int(height * 0.10))
    draw.text((int(width * 0.17), int(height * 0.12)), numeral, font=index, fill=GOLD)
    return image


def back_card(width, height):
    """A card seen from the back: a plain lattice pattern."""
    image, draw, inset = blank_card(width, height)
    step = max(6, int(width * 0.16))
    line = max(1, width // 90)
    for offset in range(-height, width + height, step):
        draw.line((inset + offset, inset, inset + offset - height, height - inset),
                  fill=GOLD, width=line)
        draw.line((inset + offset, inset, inset + offset + height, height - inset),
                  fill=GOLD, width=line)
    frame = Image.new("RGBA", (width, height), (0, 0, 0, 0))
    mask = Image.new("L", (width, height), 0)
    ImageDraw.Draw(mask).rounded_rectangle((inset, inset, width - inset, height - inset),
                                           radius=int(width * 0.07), fill=255)
    frame.paste(image, (0, 0), mask)
    base, base_draw, _ = blank_card(width, height)
    base.alpha_composite(frame)
    base_draw.rounded_rectangle((inset, inset, width - inset, height - inset),
                                radius=int(width * 0.07), outline=GOLD,
                                width=max(2, width // 56))
    return base


def icon(size):
    px = size * SUPERSAMPLE
    image = Image.new("RGBA", (px, px), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    draw.rounded_rectangle((0, 0, px - 1, px - 1), radius=int(px * 0.22), fill=TABLE)

    card_w = int(px * 0.42)
    card_h = int(card_w / 0.62)

    back = back_card(card_w, card_h).rotate(14, resample=Image.BICUBIC, expand=True)
    front = face_card(card_w, card_h, "XXI").rotate(-7, resample=Image.BICUBIC, expand=True)

    image.alpha_composite(back, (int(px * 0.42), int(px * 0.15)))
    image.alpha_composite(front, (int(px * 0.11), int(px * 0.20)))

    return image.resize((size, size), resample=Image.LANCZOS)


def main():
    sailfish = os.path.join(ROOT, "sailfish", "icons")
    os.makedirs(sailfish, exist_ok=True)
    for size in SAILFISH_SIZES:
        icon(size).save(os.path.join(sailfish, "icon-%d.png" % size))

    drawable = os.path.join(ROOT, "android", "package", "res", "drawable")
    os.makedirs(drawable, exist_ok=True)
    icon(192).save(os.path.join(drawable, "icon.png"))


if __name__ == "__main__":
    main()
