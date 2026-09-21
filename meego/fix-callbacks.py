#!/usr/bin/env python3
"""Turns the shared QML's callback properties into signals, for Qt 4.7.

qml-common passes functions around as property values:

    // TarockTable
    WhyDialog { openAnchor: table.openRule }
    // WhyDialog
    property variant openAnchor: null
    ...
    if (dialog.openAnchor) dialog.openAnchor(target)

QtQuick 2 is happy to keep a function in a var property. QtQuick 1.1 is not --
it answers "Unable to assign a function to a property" and the component then
fails to build, which left the table as a black rectangle with two cards on it.

The property becomes a signal of the same name. Calling a signal emits it, so
every call site and every "if (x.cb)" guard keeps working untouched; only the
declaration and the parent's assignment change:

    signal openAnchor(variant a1)
    WhyDialog { onOpenAnchor: table.openRule(a1) }

Only properties that are actually called somewhere are converted -- "engine"
and "learn" are objects, not callbacks, and must stay properties.

This is deliberately a port-only transform: qml-common stays as it is, and
Sailfish and Android are untouched.
"""
import glob
import os
import re
import sys


def collect(paths):
    """{name: arity} for every property that gets a function assigned to it.

    Detection goes by the assignment, not by the call: a panel may declare a
    callback it never invokes itself, and that assignment fails just the same.
    The right-hand side has to name a function declared in the assigning file,
    which also gives the arity.
    """
    texts = {p: open(p, encoding='utf-8').read() for p in paths}

    # functions per file, with their parameter count
    functions = {}
    for path, text in texts.items():
        for m in re.finditer(r'^\s*function (\w+)\(([^)]*)\)', text, flags=re.M):
            args = m.group(2).strip()
            functions.setdefault(path, {})[m.group(1)] = 0 if not args else len(split_args(args))

    declared = set()
    for text in texts.values():
        declared.update(re.findall(r'^\s*property variant (\w+): null\s*$', text, flags=re.M))

    callbacks = {}
    for path, text in texts.items():
        own = functions.get(path, {})
        for m in re.finditer(r'^\s*(\w+): [A-Za-z_][\w]*\.(\w+)\s*$', text, flags=re.M):
            name, func = m.group(1), m.group(2)
            if name in declared and func in own:
                callbacks[name] = max(callbacks.get(name, 0), own[func])
    return texts, callbacks


def split_args(args):
    """Top-level comma split, so f(a, g(b, c)) counts as two."""
    out, depth, current = [], 0, ''
    for ch in args:
        if ch in '([':
            depth += 1
        elif ch in ')]':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(current)
            current = ''
        else:
            current += ch
    if current.strip():
        out.append(current)
    return out


def main(paths):
    texts, callbacks = collect(paths)
    if not callbacks:
        print('callbacks: none found')
        return 0

    params = {n: ', '.join('variant a%d' % (i + 1) for i in range(a))
              for n, a in callbacks.items()}
    actuals = {n: ', '.join('a%d' % (i + 1) for i in range(a))
               for n, a in callbacks.items()}

    changed = 0
    for path, text in texts.items():
        after = text
        for name, arity in callbacks.items():
            after = re.sub(r'^(\s*)property variant %s: null\s*$' % re.escape(name),
                           r'\1signal %s(%s)' % (name, params[name]),
                           after, flags=re.M)
            # The parent hands over a function reference: a bare path, no call.
            after = re.sub(
                r'^(\s*)%s: ([A-Za-z_][\w]*(?:\.[A-Za-z_][\w]*)+)\s*$' % re.escape(name),
                r'\1on%s%s: \2(%s)' % (name[0].upper(), name[1:], actuals[name]),
                after, flags=re.M)
        if after != text:
            open(path, 'w', encoding='utf-8').write(after)
            changed += 1

    print('callbacks: %s -> signals, %d files rewritten'
          % (', '.join('%s/%d' % (n, a) for n, a in sorted(callbacks.items())), changed))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:] or sorted(glob.glob('meego/qml/*.qml'))))
