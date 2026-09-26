#!/bin/sh
# Publish the packages of a version as a release of this repository.
#
#   tools/release.sh <version> <package> [<package> ...]
#
# Any package file goes: the two Sailfish RPMs, and the Harmattan .deb for the
# Nokia N9 once it is built. Running it again with more files adds them to the
# release that is already there.
#
# gh is logged in on the Arch machine, so the files go there first. A copy also
# lands in ~/ps/rpms/tarock/, like the other apps keep theirs.
set -e
VERSION=$1
shift 2>/dev/null || true
if [ -z "$VERSION" ] || [ $# -eq 0 ]; then
    echo "usage: tools/release.sh <version> <rpm> [<rpm> ...]" >&2
    exit 2
fi
for FILE in "$@"; do
    [ -f "$FILE" ] || { echo "no such file: $FILE" >&2; exit 2; }
done

if [ -n "$BUILD_HOST" ]; then
    HOST=$BUILD_HOST
elif ssh -o BatchMode=yes -o ConnectTimeout=4 sebastian@192.168.1.21 true 2>/dev/null; then
    HOST=sebastian@192.168.1.21
else
    HOST=arch
fi
REPO=${REPO:-smatkovi/harbour-tarock}
# Das APK geht in das private Repo tarock-releases und traegt dort eine eigene
# Marke, damit die beiden Reihen sich nicht ins Gehege kommen:
#   REPO=smatkovi/tarock-releases TAG=android-0.6.0 tools/release.sh 0.6.0 <apk>
TAG=${TAG:-v$VERSION}
WORK=/tmp/tarock-release

# Keep a local copy next to the other apps' packages — unless the file that was
# handed in already is that copy.
mkdir -p "$HOME/ps/rpms/tarock"
for FILE in "$@"; do
    DEST="$HOME/ps/rpms/tarock/$(basename "$FILE")"
    if [ "$(readlink -f "$FILE")" != "$(readlink -f "$DEST")" ]; then
        cp "$FILE" "$DEST"
    fi
done

# Was in dieser Fassung steckt, steht im Änderungsbuch -- der oberste Eintrag,
# sofern er zu dieser Version gehört. So sagt jede Veröffentlichung, was sie
# wirklich bringt, statt immer denselben Text zu wiederholen.
NOTES=$(mktemp)
{
    echo "Tarock $VERSION"
    echo
    awk -v want="harbour-tarock ($VERSION)" '
        $0 ~ /^harbour-tarock \(/ { if (seen) exit; if (index($0, want) == 1) seen = 1; next }
        seen && $0 !~ /^ -- / { print }
    ' meego/changelog | sed -e 's/^  \* /* /' -e 's/^    //'
    echo
    # Was wirklich dabei ist, statt einer festen Zeile: wer nur das APK
    # veroeffentlicht, soll nicht RPMs versprechen.
    for FILE in "$@"; do
        case "$FILE" in
        *.rpm) HAS_RPM=1 ;;
        *.deb) HAS_DEB=1 ;;
        *.apk) HAS_APK=1 ;;
        esac
    done
    [ -n "$HAS_RPM" ] && echo "Pakete für Sailfish OS (\`.rpm\`, aarch64 und armv7hl)."
    [ -n "$HAS_DEB" ] && echo "Paket für MeeGo Harmattan auf dem Nokia N9/N950 (\`.deb\`, armel)."
    [ -n "$HAS_APK" ] && echo "Paket für Android (\`.apk\`, arm64-v8a)."
    true
} > "$NOTES"

ssh "$HOST" "rm -rf $WORK && mkdir -p $WORK"
scp "$@" "$NOTES" "$HOST:$WORK/"
rm -f "$NOTES"
NOTENAME=$(basename "$NOTES")

# Exactly the files that were handed in — a glob here would quietly drop the
# .deb and upload only the RPMs.
NAMES=""
for FILE in "$@"; do
    NAMES="$NAMES $(basename "$FILE")"
done

ssh "$HOST" "cd $WORK && \
    if gh release view $TAG --repo $REPO >/dev/null 2>&1; then \
        gh release upload $TAG$NAMES --repo $REPO --clobber; \
    else \
        gh release create $TAG$NAMES --repo $REPO --title 'Tarock $VERSION' --notes-file $NOTENAME; \
    fi"
echo "released $TAG to $REPO, copy in ~/ps/rpms/tarock/"
