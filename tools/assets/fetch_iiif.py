#!/usr/bin/env python3
"""Lädt die Kartenbilder des IuG-Decks über IIIF (docs/design.md §10.1).

    tools/assets/fetch_iiif.py [--width N] [--out DIR] [--manifest DATEI]

Quelle ist das Manifest des Volkskundemuseums Wien (ÖMV/87.529, Public Domain
Mark 1.0). Das Manifest führt keine Kartentitel, nur "[1]".."[55]" -- welche
Leinwand welche Karte ist, steht in mapping.csv daneben und wurde an den
Bildern selbst abgelesen.

Ohne --width kommt das volle Bild (ca. 2306x4096), mit --width ein
verkleinertes; die Vorschau zum Zuordnen reicht mit 160 Pixeln.
"""
import argparse
import json
import os
import sys
import time
import urllib.request

UA = "harbour-tarock/asset-fetch (+https://github.com/smatkovi/harbour-tarock)"


def canvases(manifest):
    with open(manifest, encoding="utf-8") as handle:
        document = json.load(handle)
    out = []
    for canvas in document["sequences"][0]["canvases"]:
        resource = canvas["images"][0]["resource"]
        service = resource.get("service", {})
        out.append({
            "label": canvas.get("label", ""),
            "service": service.get("@id") or resource["@id"],
            "width": canvas.get("width"),
            "height": canvas.get("height"),
        })
    return out


def fetch(url, path):
    request = urllib.request.Request(url, headers={"User-Agent": UA})
    with urllib.request.urlopen(request, timeout=120) as response:
        data = response.read()
    with open(path, "wb") as handle:
        handle.write(data)
    return len(data)


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    parser = argparse.ArgumentParser()
    parser.add_argument("--manifest",
                        default=os.path.expanduser("~/ps/tarock-assets/manifest.json"))
    parser.add_argument("--out", default=os.path.expanduser("~/ps/tarock-assets/raw"))
    parser.add_argument("--width", type=int, default=0,
                        help="0 = volles Bild, sonst auf diese Breite skaliert")
    args = parser.parse_args()

    os.makedirs(args.out, exist_ok=True)
    items = canvases(args.manifest)
    print("%d Leinwände im Manifest" % len(items))
    size = "full" if args.width <= 0 else "%d," % args.width
    for index, item in enumerate(items, 1):
        path = os.path.join(args.out, "%02d.jpg" % index)
        if os.path.exists(path) and os.path.getsize(path) > 0:
            continue
        url = "%s/full/%s/0/default.jpg" % (item["service"], size)
        try:
            written = fetch(url, path)
        except Exception as error:      # noqa: BLE001 - jede Ursache ist gleich fatal
            print("  %02d FEHLER %s" % (index, error), file=sys.stderr)
            return 1
        print("  %02d %s %d KB" % (index, item["label"], written // 1024))
        time.sleep(0.3)                 # der Server gehört einem Museum, nicht uns
    # Die Lizenzlage kommt mit ins Paket, samt Abrufdatum.
    credits = os.path.join(here, "..", "..", "CREDITS")
    os.makedirs(os.path.join(credits, "licenses"), exist_ok=True)
    with open(os.path.join(credits, "licenses", "iug1904-source.txt"), "w",
              encoding="utf-8") as handle:
        handle.write(
            "Tarockkartensatz \"Industrie und Glück\", Ferd. Piatnik & Söhne, Wien\n"
            "Volkskundemuseum Wien, Inventarnummer ÖMV/87.529\n"
            "Rechte: Public Domain Mark 1.0 (gemeinfrei)\n"
            "Manifest: https://sammlung.volkskundemuseum.at/i3f/v20/201122/manifest\n"
            "Abgerufen: %s\n" % time.strftime("%Y-%m-%d"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
