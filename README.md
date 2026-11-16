# my-qmk

Keymap and keyboard definition for a BastardKB Charybdis Nano (3x5, shield v2) with
Splinky v3 (RP2040) controllers. Upstream QMK dropped the `splinky_3` target, so this repo
holds a ported `splinky_3` variant (a sibling of upstream's `elitec`) plus the `jigfox`
keymap, and overlays both onto a pinned stock `qmk_firmware`.

- `keyboards/` mirrors the qmk_firmware tree and is copied over it
- `QMK_VERSION` is the pinned qmk_firmware tag, `requirements.txt` the qmk CLI
- `mise.toml` pins every tool: Arm GNU Toolchain (GCC + newlib), GNU make, Python,
  uv and picotool; the same versions are used locally and in CI

## Building and flashing

Only [mise](https://mise.jdx.dev) is needed:

    mise install             # once: toolchain, make, python, uv, picotool
    mise run build           # -> build/bastardkb_charybdis_3x5_splinky_3_jigfox.uf2
    mise run flash           # build, then flash the right half, then the left
    mise run flash --half left
    mise run clean

`build` runs `setup` first, which checks out the pinned tag into `.qmk_firmware/`
(gitignored, with submodules) and installs the qmk CLI into `.venv/`.

`flash` waits for each half in turn: plug it in, double-tap its reset button, and
picotool writes and verifies the firmware and restarts it. It only writes the program
range, so each half keeps its saved settings. Without mise, copying the `.uf2` to the
`RPI-RP2` drive works too. The right half is the USB side (`MASTER_RIGHT`).

## CI and releases

`.github/workflows/build.yml` builds on every push and pull request with the tools from
`mise.toml` (`mise run build`) and keeps the `.uf2` as the `firmware` artifact of the run.
Pushing a tag `v*` also publishes a GitHub release with
`charybdis-3x5-splinky3-jigfox-<tag>.uf2` attached:

    git tag v1.0.0 && git push origin v1.0.0

## License

GPL-2.0-or-later, like QMK; see `LICENSE`. The keyboard and keymap files keep their
original copyright headers.
