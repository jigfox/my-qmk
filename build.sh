#!/usr/bin/env bash
# Overlay this repo's keyboards/ onto a stock qmk_firmware checkout and build.
# Needs `qmk` on PATH (pipx install qmk), an arm-none-eabi toolchain and GNU make.
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
tag=$(tr -d '[:space:]' < "$here/QMK_VERSION")
qmk_dir=${QMK_DIR:-$here/.qmk_firmware}
kb=bastardkb/charybdis/3x5/splinky_3
km=jigfox

# Homebrew's GCC 8 and binutils, when present.
for p in /opt/homebrew/opt/arm-none-eabi-gcc@8/bin /opt/homebrew/opt/arm-none-eabi-binutils/bin; do
    [ -d "$p" ] && PATH=$p:$PATH
done
export PATH

if [ ! -d "$qmk_dir/.git" ]; then
    git clone --depth 1 --branch "$tag" https://github.com/qmk/qmk_firmware.git "$qmk_dir"
else
    git -C "$qmk_dir" fetch --depth 1 origin tag "$tag" --no-tags 2>/dev/null || true
    git -C "$qmk_dir" checkout -q "$tag"
fi
git -C "$qmk_dir" submodule update --init --recursive --depth 1 -q

# A stale overlay must not survive a pin change.
rm -rf "$qmk_dir/keyboards/bastardkb/charybdis/3x5/splinky_3" \
       "$qmk_dir/keyboards/bastardkb/charybdis/3x5/keymaps/$km"
rsync -a "$here/keyboards/" "$qmk_dir/keyboards/"

(cd "$qmk_dir" && QMK_HOME="$qmk_dir" qmk compile -kb "$kb" -km "$km")

mkdir -p "$here/build"
cp "$qmk_dir"/bastardkb_charybdis_3x5_splinky_3_"$km".uf2 "$here/build/"
echo "built: build/bastardkb_charybdis_3x5_splinky_3_$km.uf2"
