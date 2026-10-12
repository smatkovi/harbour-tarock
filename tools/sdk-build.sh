#!/bin/sh
# Runs on the build machine (arch): hands the synchronised tree in
# ~/tarock-build/src to the Sailfish SDK container and builds there.
#
#   tools/sdk-build.sh tests          # cmake + ctest against the i486 target
#   tools/sdk-build.sh rpms           # aarch64 and armv7hl RPMs -> ~/tarock-build/out
#
# The container's trees: /home/mersdk/tktest (tests, with build/) and
# /home/mersdk/tkbuild-<arch> (RPMs, with RPMS/); mb2 builds incrementally.
set -e
SRC=${SRC:-$HOME/tarock-build/src}
OUT=${OUT:-$HOME/tarock-build/out}
C=sfossdk52
MODE=${1:-rpms}
VERSION=$(sed -n 's/^Version: *//p' "$SRC/rpm/harbour-tarock.spec" | head -1)

docker start $C >/dev/null 2>&1 || true
tar -C "$SRC" --exclude=./build --exclude=./.git -czf /tmp/tarock-src.tgz .
docker cp /tmp/tarock-src.tgz $C:/tmp/tarock-src.tgz

case "$MODE" in
tests)
    docker exec $C sh -c 'mkdir -p /home/mersdk/tktest && cd /home/mersdk/tktest && tar xzf /tmp/tarock-src.tgz && mkdir -p build && cd build && sb2 -t SailfishOS-5.2.0.15-i486 cmake .. -DTAROCK_BUILD_TESTS=ON >/dev/null && sb2 -t SailfishOS-5.2.0.15-i486 make -j8 2>&1 | grep -E "error|Error|warning: unused" ; cd /home/mersdk/tktest/build && sb2 -t SailfishOS-5.2.0.15-i486 ctest --output-on-failure 2>&1 | tail -25'
    ;;
rpms)
    mkdir -p "$OUT"
    for arch in aarch64 armv7hl; do
        docker exec $C sh -c "mkdir -p /home/mersdk/tkbuild-$arch && cd /home/mersdk/tkbuild-$arch && tar xzf /tmp/tarock-src.tgz && mb2 -t SailfishOS-5.2.0.15-$arch build 2>&1 | tail -3"
        docker cp "$C:/home/mersdk/tkbuild-$arch/RPMS/harbour-tarock-$VERSION-1.$arch.rpm" "$OUT/"
    done
    ls -la "$OUT"/harbour-tarock-$VERSION-1.*.rpm
    ;;
*) echo "usage: $0 tests|rpms" >&2; exit 2 ;;
esac
