#!/usr/bin/env python3
"""Regenerates the platform pages of meego/qml from android/qml.

The shared panels come from qml-common (meego/port-qml.sh); the pages around
them come from the Android build, because QtQuick.Controls maps onto the
Harmattan components far more directly than Silica does.

Hand-written files are left alone: SubPage.qml, ComboBox.qml, ItemDelegate.qml,
harbour-tarock.qml and everything under context/.
"""
import io
import os
import re
import sys

FROM_ANDROID = [
    'MainPage', 'TablePage', 'ScorePage', 'LessonPage', 'LearnPage',
    'GlossaryPage', 'AboutPage', 'RulesPage', 'SettingsPage', 'TutorialPage',
    'Card', 'FlyingCard', 'TableButton', 'TextBlock', 'SectionLabel',
]

# --- the mechanical pass ----------------------------------------------------

def convert(text):
    text = text.replace('pragma Singleton\n', '')
    text = re.sub(r'^import QtQuick\.Controls\.Material\s*$', '', text, flags=re.M)
    text = re.sub(r'^import QtQuick\.Controls\s*$', 'import com.nokia.meego 1.0', text, flags=re.M)
    text = re.sub(r'^import QtQuick\s*$', 'import QtQuick 1.1', text, flags=re.M)
    text = text.replace('readonly property ', 'property ')
    text = text.replace('property var ', 'property variant ')
    # com.nokia.meego brings a Theme of its own that beats a context property
    # of that name; ours is AppTheme (meego/main.cpp).
    text = re.sub(r'\bTheme\.', 'AppTheme.', text)
    # A MeeGo Page carries its stack as pageStack.
    text = text.replace('page.StackView.view', 'page.pageStack')
    text = re.sub(r'^\s*Material\.[a-zA-Z]+:.*$\n', '', text, flags=re.M)
    text = re.sub(r'^\s*ScrollIndicator\.vertical:.*$\n', '', text, flags=re.M)
    # QtQuick 1.1 runs ES5: no arrow functions. A signal parameter is in scope
    # by name there anyway, so the head just goes.
    text = re.sub(r':\s*\((\w+)\)\s*=>\s*\{', r': {', text)
    text = re.sub(r':\s*\((\w+)\)\s*=>\s*', r': ', text)
    # Page activation is reported through status, not an attached property.
    text = wrap_activated(text)
    # A plain Flickable never gets to flick when the drag starts on a child
    # MouseArea; pressDelay lets it steal the press back, which is what
    # QtQuick.Controls does for itself.
    text = re.sub(r'^(\s*)Flickable \{\s*$\n(\s*)(anchors\.fill: parent)',
                  r'\1Flickable {\n\2\3\n\2pressDelay: 150', text, flags=re.M)
    # QtQuick 1.1 positioners and Text have no padding properties.
    text = re.sub(r'^(\s*)topPadding:(.*)$', r'\1y:\2   // QtQuick 1.1 has no padding here', text, flags=re.M)
    # A MeeGo Page has no background property.
    text = re.sub(r'^(\s*)background: Rectangle \{ color: (.*) \}\s*$',
                  r'\1Rectangle { anchors.fill: parent; color: \2; z: -1 }', text, flags=re.M)
    return text


def wrap_activated(text):
    """StackView.onActivated is an attached property and works on any Item;
    onStatusChanged only exists on a Page, and TablePage's root is a plain
    Item. A short timer after construction says the same thing: wait until the
    page is up and the table has a size.

    The block is closed by brace matching so the extra brace the Timer needs
    lands in the right place.
    """
    out = []
    i = 0
    while True:
        m = re.search(r'^(\s*)StackView\.onActivated: \{[ \t]*$', text[i:], flags=re.M)
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
        ind = m.group(1)
        body = text[brace + 1:j]
        out.append(text[i:i + m.start()])
        out.append('%sComponent.onCompleted: activatedTimer.start()\n'
                   '%sTimer {\n%s    id: activatedTimer\n%s    interval: 400\n'
                   '%s    onTriggered: {%s}\n%s}'
                   % (ind, ind, ind, ind, ind, body, ind))
        i = j + 1
    return ''.join(out)


def main():
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    out = os.path.join(here, 'meego', 'qml')
    for name in FROM_ANDROID:
        src = os.path.join(here, 'android', 'qml', name + '.qml')
        if not os.path.exists(src):
            sys.stderr.write('missing %s\n' % src)
            return 1
        with io.open(src, encoding='utf-8') as fh:
            text = convert(fh.read())
        with io.open(os.path.join(out, name + '.qml'), 'w', encoding='utf-8') as fh:
            fh.write(text)
    print('platform: %d pages from android/qml' % len(FROM_ANDROID))
    return 0


if __name__ == '__main__':
    sys.exit(main())
