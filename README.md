# Acer One 8 T482L kernel port

This repository contains an experimental Linux kernel port for the Acer One 8 T482L. The current donor is the Lenovo Akita MediaTek tree (Linux 4.9.190); the earlier LibreHU AC8257 tree was an abandoned first attempt and is retained only in the chronological porting notes.

## Current test status

- Build #18 reaches the Android boot animation, but Android boot completion has not been confirmed.
- The displayed image is clipped at the top and bottom. The GSL touchscreen driver still fails I2C probe with `-121`.
- CCCI/CLDMA and LTE/Wi-Fi coexistence are disabled in the Acer test defconfig after the donor modem path faulted while dumping an invalid CLDMA register address. LTE service is unavailable in this test configuration.
- This is a development port, not a validated daily-use kernel. See [PORTING.md](PORTING.md) for evidence, changes, image hashes, and next steps.

## Build

Requirements: an AArch64 Android GCC toolchain compatible with this 4.9 tree, GNU make, and the usual kernel build dependencies. The toolchain is not included.

```sh
CROSS_COMPILE=/path/to/toolchain/bin/aarch64-linux-android- JOBS=10 ./build.sh
```

The script defaults to `acer_t482l_defconfig`, `ARCH=arm64`, and `out/acer_t482l/`. Override the configuration, output directory, architecture, toolchain prefix, or job count with environment variables:

```sh
DEFCONFIG=acer_t482l_defconfig OUT_DIR=out/test CROSS_COMPILE=/path/to/toolchain/bin/aarch64-linux-android- JOBS=8 ./build.sh
```

The build creates `Image.gz` and device-tree outputs under the selected output directory. It does not package an Android boot image. Boot-image packaging must use the correct stock ramdisk, DTB, and header parameters for the target; see the packaging records in `PORTING.md`.

## Porting notes

`PORTING.md` is the chronological engineering log. It records donor selection, stock image inspection, changes, build results, device observations, and unresolved issues. Build outputs and device dumps are intentionally kept outside the source repository.
