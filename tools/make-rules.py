#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Erzeugt die Regeltexte des Nachschlagewerks (docs/design.md §7.7).

    tools/make-rules.py            # schreibt assets/rules/<profil>.de.txt

Quelle sind die Kapitel 1–9 der vier Regelwerke in docs/ -- die Kapitel, die
die Spielregeln beschreiben. Kapitel 0 (Quellenlage), 10 (Computerspieler),
11 (Lernmodus) und alles danach bleiben draußen. Jede Überschrift zweiter
und dritter Ebene wird ein Kapitel mit der Nummer als Anker ("6.2"), genau
die Nummern, auf die Reason.anchor und die Erklärleiste zeigen.

Das Markdown wird in die Teilmenge von HTML übersetzt, die Qt 4.7 wie Qt 5
in Text.RichText zeigen: p, b, i, ul, ol, li, br. Tabellen werden zu Listen,
eine Zeile je Eintrag, weil eine HTML-Tabelle auf 480 Pixeln aus dem
Bildschirm läuft. Was nur dem Programmierer gilt, fällt weg: Absätze, die mit "Engine"
oder "Implementierung" anfangen. Codeblöcke bleiben als Absätze mit
Zeilenumbrüchen, denn dort stehen auch die Lizitbeispiele.

Dateiformat (eine Datei je Profil, UTF-8):

    # Kommentar, nur vor dem ersten Kapitel
    @ <nummer> <ebene> <titel>
    <html des kapitels, beliebig viele zeilen>
    @ ...

RulesIndex::chapters() liest genau das; keine JSON-Bibliothek nötig.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)

# Profilstamm (RulesIndex.cpp profileStem) -> Regelwerk
DOCS = {
    "at-kr-ooe": "koenigrufen.md",
    "hu-illu": "hungarian.md",
    "at-tapp": "tapptarock.md",
    "at-stroh": "strohmandeln.md",
}
FIRST_CHAPTER, LAST_CHAPTER = 1, 9

# Absätze und Listenpunkte, die nur der Umsetzung gelten.
DEV_PREFIXES = ("Engine", "Implementierung", "⚙")


def escape(text):
    return text.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def inline(text):
    """Markdown-Auszeichnung innerhalb einer Zeile -> HTML."""
    text = escape(text)
    # Verweise [Text](url) -> Text; Quellenkürzel wie [WIKI] bleiben.
    text = re.sub(r"\[([^\]]+)\]\((?:[^)]+)\)", r"\1", text)
    text = re.sub(r"`([^`]*)`", r"\1", text)
    text = re.sub(r"\*\*(.+?)\*\*", r"<b>\1</b>", text)
    text = re.sub(r"(?<![\w*])\*(?!\s)(.+?)(?<!\s)\*(?![\w*])", r"<i>\1</i>", text)
    text = re.sub(r"(?<![\w_])_(?!\s)(.+?)(?<!\s)_(?![\w_])", r"<i>\1</i>", text)
    # Ein einzelnes ** ohne Partner (ungerade Zahl) bliebe als Markdown stehen.
    text = text.replace("**", "")
    return text


def is_dev(text):
    plain = re.sub(r"^[*_\s]+", "", text)
    return plain.startswith(DEV_PREFIXES)


def split_row(line):
    cells = line.strip().strip("|").split("|")
    return [c.strip() for c in cells]


def is_separator_row(line):
    return re.match(r"^\s*\|?\s*:?-{2,}", line) is not None and set(line.strip()) <= set("|:- ")


class Renderer(object):
    """Setzt die Zeilen eines Kapitels zu HTML zusammen."""

    def __init__(self):
        self.out = []
        self.para = []
        self.list_stack = []   # ("ul"|"ol", indent)
        self.quote = []
        self.table = []
        self.item = None       # der laufende Listenpunkt, roh
        self.code = []

    # --- Hilfen ---------------------------------------------------------------
    def flush_para(self):
        if self.para:
            text = " ".join(self.para)
            if not is_dev(text):
                self.out.append("<p>%s</p>" % inline(text))
            self.para = []

    def flush_quote(self):
        if self.quote:
            text = " ".join(self.quote)
            self.out.append("<p><i>%s</i></p>" % inline(text))
            self.quote = []

    def flush_item(self):
        if self.item is not None:
            if not is_dev(self.item):
                self.out.append("<li>%s</li>" % inline(self.item))
            self.item = None

    def flush_code(self):
        if self.code:
            # Programmcode (Pseudocode der Abrechnung, Engine-Formeln) bleibt
            # draußen; Beispiele in Textform (Lizitfolgen) bleiben drin.
            program = any(re.match(r"^\s*(if|for|while|def|return|elif|else)\b", l)
                          or "==" in l or "+=" in l or "**" in l for l in self.code)
            lines = [escape(l.rstrip()) for l in self.code]
            while lines and lines[-1] == "":
                lines.pop()
            if lines and not program:
                self.out.append("<p>%s</p>" % "<br/>".join(lines))
            self.code = []

    def close_lists(self, down_to=-1):
        self.flush_item()
        while self.list_stack and self.list_stack[-1][1] > down_to:
            kind, _ = self.list_stack.pop()
            self.out.append("</%s>" % kind)

    def flush_table(self):
        rows = [r for r in self.table if not is_separator_row(r)]
        self.table = []
        if not rows:
            return
        header = split_row(rows[0])
        items = []
        for row in rows[1:]:
            cells = split_row(row)
            if not any(cells):
                continue
            parts = []
            for i, cell in enumerate(cells):
                if cell == "":
                    continue
                name = header[i] if i < len(header) else ""
                if i == 0:
                    parts.append("<b>%s</b>" % inline(cell))
                elif name and not is_separator_row(name):
                    parts.append("%s: %s" % (inline(name), inline(cell)))
                else:
                    parts.append(inline(cell))
            if parts:
                items.append("<li>%s</li>" % " · ".join(parts))
        if items:
            self.out.append("<ul>%s</ul>" % "".join(items))

    def flush_all(self):
        self.flush_para()
        self.flush_quote()
        self.flush_table()
        self.close_lists()
        self.flush_code()

    # --- die Zeilen -------------------------------------------------------------
    def feed(self, lines):
        in_fence = False
        for raw in lines:
            line = raw.rstrip("\n")
            if line.strip().startswith("```"):
                in_fence = not in_fence
                self.flush_all()
                continue
            if in_fence:
                self.code.append(line)
                continue
            stripped = line.strip()

            if stripped.startswith("|"):
                self.flush_para(); self.flush_quote(); self.close_lists()
                self.table.append(line)
                continue
            elif self.table:
                self.flush_table()

            if stripped == "":
                self.flush_para(); self.flush_quote(); self.close_lists()
                continue

            m = re.match(r"^(#{4,6})\s+(.*)$", stripped)
            if m:
                self.flush_all()
                self.out.append("<p><b>%s</b></p>" % inline(m.group(2)))
                continue

            if stripped.startswith(">"):
                self.flush_para(); self.close_lists()
                text = stripped.lstrip(">").strip()
                if text == "":
                    self.flush_quote()
                else:
                    self.quote.append(text)
                continue
            elif self.quote:
                self.flush_quote()

            m = re.match(r"^(\s*)([-*+]|\d+[.)])\s+(.*)$", line)
            if m:
                self.flush_para()
                indent = len(m.group(1).expandtabs(4))
                kind = "ol" if m.group(2)[0].isdigit() else "ul"
                text = m.group(3)
                self.close_lists(indent)
                if not self.list_stack or self.list_stack[-1][1] < indent:
                    self.list_stack.append((kind, indent))
                    self.out.append("<%s>" % kind)
                self.item = text
                continue

            # Fortsetzung eines Listenpunkts (eingerückt) oder Fließtext.
            if self.item is not None and line.startswith((" ", "\t")):
                self.item += " " + stripped
                continue
            if self.list_stack:
                self.close_lists()
            self.para.append(stripped)
        self.flush_all()

    def html(self):
        return "\n".join(self.out)


def chapters_of(path):
    """Liefert [(nummer, ebene, titel, zeilen)] der Kapitel 1–9."""
    with open(path, encoding="utf-8") as f:
        lines = f.readlines()
    result = []
    current = None
    for line in lines:
        m = re.match(r"^(##|###)\s+(\d+)(?:\.(\d+))?\.?\s+(.*?)\s*$", line)
        if m:
            level = 1 if m.group(1) == "##" else 2
            major = int(m.group(2))
            if level == 2 and m.group(3) is None:
                m = None
            else:
                number = m.group(2) + ("." + m.group(3) if m.group(3) else "")
                title = m.group(4).strip()
                if FIRST_CHAPTER <= major <= LAST_CHAPTER:
                    current = (number, level, title, [])
                    result.append(current)
                else:
                    current = None
                continue
        if m is None and re.match(r"^##\s", line):
            # Eine Überschrift zweiter Ebene ohne Nummer beendet das Kapitel.
            current = None
            continue
        if current is not None:
            current[3].append(line)
    return result


def write_profile(stem, docname):
    source = os.path.join(ROOT, "docs", docname)
    chapters = chapters_of(source)
    if not chapters:
        sys.exit("%s: keine Kapitel gefunden" % source)
    out_dir = os.path.join(ROOT, "assets", "rules")
    os.makedirs(out_dir, exist_ok=True)
    target = os.path.join(out_dir, stem + ".de.txt")
    with open(target, "w", encoding="utf-8") as f:
        f.write("# Erzeugt von tools/make-rules.py aus docs/%s -- nicht von Hand ändern.\n" % docname)
        f.write("# Kapitel %d bis %d des Regelwerks; Format siehe assets/rules/README.md.\n"
                % (FIRST_CHAPTER, LAST_CHAPTER))
        for number, level, title, body in chapters:
            renderer = Renderer()
            renderer.feed(body)
            html = renderer.html()
            # Das Kapitel selbst hat oft keinen eigenen Text, nur Abschnitte;
            # dann nennt es seine Abschnitte, damit der Leser etwas sieht.
            if html.strip() == "":
                sections = [c for c in chapters if c[1] == 2 and c[0].split(".")[0] == number]
                if sections:
                    html = "<ul>%s</ul>" % "".join(
                        "<li>%s  %s</li>" % (s[0], inline(s[2])) for s in sections)
            f.write("@ %s %d %s\n" % (number, level, inline(title)))
            f.write(html.strip() + "\n")
    print("%s: %d Kapitel -> %s" % (docname, len(chapters), os.path.relpath(target, ROOT)))


def main():
    for stem, docname in DOCS.items():
        write_profile(stem, docname)


if __name__ == "__main__":
    main()
