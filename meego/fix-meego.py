#!/usr/bin/env python3
"""QtQuick.Controls spellings com.nokia.meego does not have.

Run on meego/qml after the platform files are copied from android/qml.
Each of these was found by opening the page on an N950 and reading the error.
"""
import os
import re
import sys

SUBST = [
    # Controls' Switch and CheckBox report toggled(); the MeeGo Switch only
    # has checked, so the handler hangs off its change.
    (r'^(\s*)onToggled:', r'\1onCheckedChanged:'),
    # Controls 2's Switch carries its own caption; the MeeGo one does not, so
    # the local TextSwitch (a caption plus that Switch) takes its place.
    (r'^(\s*)Switch \{', r'\1TextSwitch {'),
    # QtQuick 2 can shrink a Text to fit; QtQuick 1.1 cannot, so the two
    # properties that ask for it go and the text keeps its size.
    (r'^\s*fontSizeMode:.*$\n', ''),
    (r'^\s*minimumPixelSize:.*$\n', ''),
    # Controls 2's Slider reports a drag as moved(); the MeeGo one only has
    # the value itself.
    (r'^(\s*)onMoved:', r'\1onValueChanged:'),
    # QtQuick 1.1 has no point/size/rect property types.
    (r'^(\s*)property (point|size|rect) ', r'\1property variant '),
]


def fix_progressbars(text):
    """Controls 2's ProgressBar and Slider take their range as from/to; the
    MeeGo ones use minimumValue/maximumValue.

    Scoped to the ProgressBar block by brace matching, and for a good reason:
    a blanket rename also hits NumberAnimation { to: ... }, where "to" is
    exactly right, and the animation then refuses to load.
    """
    out = []
    i = 0
    while True:
        m = re.search(r'^(\s*)(?:ProgressBar|Slider) \{[ \t]*$', text[i:], flags=re.M)
        if not m:
            out.append(text[i:])
            break
        brace = i + m.end() - 1
        depth = 0
        j = brace
        while j < len(text):
            if text[j] == '{':
                depth += 1
            elif text[j] == '}':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        if j >= len(text):
            out.append(text[i:])
            break
        block = text[i + m.start():j + 1]
        block = re.sub(r'^(\s*)from:', r'\1minimumValue:', block, flags=re.M)
        block = re.sub(r'^(\s*)to:', r'\1maximumValue:', block, flags=re.M)
        out.append(text[i:i + m.start()])
        out.append(block)
        i = j + 1
    return ''.join(out)


def fix_menuitems(text):
    """Controls' MenuItem reports triggered(); the MeeGo one clicked().

    Scoped to the MenuItem block: a Timer also has onTriggered, and renaming
    that one breaks it.
    """
    out = []
    i = 0
    while True:
        m = re.search(r'^(\s*)MenuItem \{[ \t]*$', text[i:], flags=re.M)
        if not m:
            out.append(text[i:])
            break
        brace = i + m.end() - 1
        depth = 0
        j = brace
        while j < len(text):
            if text[j] == '{':
                depth += 1
            elif text[j] == '}':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        if j >= len(text):
            out.append(text[i:])
            break
        block = re.sub(r'^(\s*)onTriggered:', r'\1onClicked:',
                       text[i + m.start():j + 1], flags=re.M)
        out.append(text[i:i + m.start()])
        out.append(block)
        i = j + 1
    return ''.join(out)


def wrap_script_bindings(text):
    """QtQuick 2 accepts a statement block as a property binding; QtQuick 1.1
    wants an expression. The block becomes an immediately-called function.

    The block is found by matching braces from the opening one, so a nested
    if or function inside it does not end it early.
    """
    out = []
    i = 0
    while True:
        m = re.search(r'^(\s*)property (\w+) (\w+): \{[ \t]*$', text[i:], flags=re.M)
        if not m:
            out.append(text[i:])
            break
        start = i + m.start()
        brace = i + m.end() - 1          # the opening brace
        depth = 0
        j = brace
        while j < len(text):
            if text[j] == '{':
                depth += 1
            elif text[j] == '}':
                depth -= 1
                if depth == 0:
                    break
            j += 1
        if j >= len(text):
            out.append(text[i:])
            break
        indent = m.group(1)
        head = '%sproperty %s %s: (function() {' % (indent, m.group(2), m.group(3))
        body = text[brace + 1:j]
        out.append(text[i:start])
        out.append(head + body + '})()')
        i = j + 1
    return ''.join(out)


# Hand-written stand-ins are not touched: TextSwitch.qml contains the very
# Switch the substitution below looks for, and rewriting it would make the
# component reference itself.
HAND_WRITTEN = {'TextSwitch.qml', 'ComboBox.qml', 'ItemDelegate.qml', 'SubPage.qml'}


def main(paths):
    changed = 0
    for path in paths:
        if os.path.basename(path) in HAND_WRITTEN:
            continue
        with open(path, encoding='utf-8') as fh:
            before = fh.read()
        after = before
        for pattern, repl in SUBST:
            after = re.sub(pattern, repl, after, flags=re.M)
        after = fix_progressbars(after)
        after = fix_menuitems(after)
        after = wrap_script_bindings(after)
        if after != before:
            with open(path, 'w', encoding='utf-8') as fh:
                fh.write(after)
            changed += 1
    print("meego: %d of %d files rewritten" % (changed, len(paths)))


if __name__ == '__main__':
    main(sys.argv[1:])
