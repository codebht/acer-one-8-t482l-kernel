#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ARCH="${ARCH:-arm64}"
DEFCONFIG="${DEFCONFIG:-acer_t482l_defconfig}"
OUT_DIR="${OUT_DIR:-${ROOT}/out/acer_t482l}"
CROSS_COMPILE="${CROSS_COMPILE:-aarch64-linux-android-}"
JOBS="${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || nproc)}"
BUILD_TARGETS="${BUILD_TARGETS:-Image.gz}"
read -r -a BUILD_TARGET_ARRAY <<< "${BUILD_TARGETS}"
HOSTCFLAGS="${HOSTCFLAGS:--fcommon}"
KCFLAGS="${KCFLAGS:--Wno-error=maybe-uninitialized}"

BUILD_BOOTIMG="${BUILD_BOOTIMG:-1}"
RAMDISK="${RAMDISK:-${ROOT}/boot/acer_t482l/ramdisk.img}"
DTB="${DTB:-${ROOT}/boot/acer_t482l/dtb}"
BOOT_IMG="${BOOT_IMG:-${OUT_DIR}/boot.img}"
MKBOOTIMG="${MKBOOTIMG:-${ROOT}/tools/mkbootimg/mkbootimg.py}"

make -C "${ROOT}" O="${OUT_DIR}" ARCH="${ARCH}" \
    CROSS_COMPILE="${CROSS_COMPILE}" HOSTCFLAGS="${HOSTCFLAGS}" \
    KCFLAGS="${KCFLAGS}" "${DEFCONFIG}"

make -C "${ROOT}" O="${OUT_DIR}" ARCH="${ARCH}" \
    CROSS_COMPILE="${CROSS_COMPILE}" HOSTCFLAGS="${HOSTCFLAGS}" \
    KCFLAGS="${KCFLAGS}" -j"${JOBS}" "${BUILD_TARGET_ARRAY[@]}"

KERNEL_IMG="${OUT_DIR}/arch/${ARCH}/boot/Image.gz"
printf "Kernel image: %s\n" "${KERNEL_IMG}"

if [ "${BUILD_BOOTIMG}" = "1" ]; then
    python3 "${MKBOOTIMG}" \
        --kernel "${KERNEL_IMG}" \
        --ramdisk "${RAMDISK}" \
        --dtb "${DTB}" \
        --cmdline "bootopt=64S3,32N2,64N2 buildvariant=user" \
        --base 0x40000000 \
        --kernel_offset 0x00080000 \
        --ramdisk_offset 0x11b00000 \
        --tags_offset 0x07880000 \
        --dtb_offset 0x07880000 \
        --pagesize 2048 \
        --header_version 2 \
        --os_version 10.0.0 \
        --os_patch_level 2021-11 \
        -o "${BOOT_IMG}"
    printf "Boot image:   %s\n" "${BOOT_IMG}"
fi
