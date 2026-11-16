# my-qmk

Keymap and keyboard definition for a BastardKB Charybdis Nano (3x5, shield v2) with
Splinky v3 (RP2040) controllers. Upstream QMK dropped the `splinky_3` target, so this repo
holds a ported `splinky_3` variant (a sibling of upstream's `elitec`) plus the `jigfox`
keymap, and overlays both onto a pinned stock `qmk_firmware`.

- `keyboards/` mirrors the qmk_firmware tree and is copied over it
- `QMK_VERSION` is the pinned qmk_firmware tag
- `build.sh` clones/checks out that tag into `.qmk_firmware/` (gitignored), overlays
  `keyboards/`, compiles and puts the `.uf2` into `build/` (gitignored)

Needs `qmk` on PATH (`pipx install qmk`), an arm-none-eabi toolchain and GNU make
(`gmake` on macOS; build.sh prefers Homebrew's `arm-none-eabi-gcc@8` when installed).

    ./build.sh
    # build/bastardkb_charybdis_3x5_splinky_3_jigfox.uf2

Flash by double-tapping reset on each half and copying the file to the RPI-RP2 drive.
The right half is the USB side (`MASTER_RIGHT`).

## CI and releases

`.github/workflows/build.yml` builds on every push and pull request (Ubuntu's
`gcc-arm-none-eabi`) and keeps the `.uf2` as the `firmware` artifact of the run.
Pushing a tag `v*` also publishes a GitHub release with
`charybdis-3x5-splinky3-jigfox-<tag>.uf2` attached:

    git tag v1.0.0 && git push origin v1.0.0
