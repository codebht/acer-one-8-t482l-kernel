# Acer One 8 T482L Kernel Tree

Linux 4.9.190 kernel tree for the **Acer One 8 T482L** (MediaTek MT6761).

## Current hardware status

- **Boot:** Boots Android 10/11 (header v2 with stock DTB) and TWRP cleanly.
- **Display & Backlight:** `acer_t482l_ili9881` (`800x1280` DSI) + `DISP_PWM0` (`0..1023`) working.
- **Touchscreen:** `gslX680` (`G863_8_DFL_PG`) working.
- **Connectivity:** Built-in WMT/WLAN/BT (`wlan_drv_gen4m`, `bt_drv`) working.
- **Audio:** Speaker, headphone, and microphone working.
- **Cameras & Torch:** Rear (`ov8858_mipi_raw`), Front (`gc2385_mipi_raw`), and Torch (`flashlights-mt6370`) working.
- **Sensors:** Accelerometer, orientation, and proximity sensors active.

See [PORTING.md](PORTING.md) for the engineering log, packaging parameters, and commit history.

## Build

Requirements: an AArch64 Android GCC toolchain compatible with this 4.9 tree, GNU make, and the usual kernel build dependencies. The toolchain is not included.

```sh
CROSS_COMPILE=/path/to/toolchain/bin/aarch64-linux-android- JOBS=10 ./build.sh
```

The script defaults to `acer_t482l_defconfig`, `ARCH=arm64`, and `out/acer_t482l/`. Override the configuration, output directory, architecture, toolchain prefix, or job count with environment variables:

```sh
DEFCONFIG=acer_t482l_defconfig OUT_DIR=out/test CROSS_COMPILE=/path/to/toolchain/bin/aarch64-linux-android- JOBS=8 ./build.sh
```

The default build target is `Image.gz`; unrelated donor-board DTBs are not built by default because some are incomplete. Add Make targets with the space-separated `BUILD_TARGETS` variable if needed. The GitHub Actions workflow performs a compile-only check and does not publish packages or artifacts. The build script does not package an Android boot image. Boot-image packaging must use the correct stock ramdisk, DTB, and header parameters for the target; see the packaging records in `PORTING.md`.

## Porting notes

`PORTING.md` is the chronological engineering log. It records donor selection, stock image inspection, changes, build results, device observations, and unresolved issues. Build outputs and device dumps are intentionally kept outside the source repository.
