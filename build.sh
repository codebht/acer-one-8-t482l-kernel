#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ARCH="${ARCH:-arm64}"
DEFCONFIG="${DEFCONFIG:-acer_t482l_defconfig}"
OUT_DIR="${OUT_DIR:-${ROOT}/out/acer_t482l}"
CROSS_COMPILE="${CROSS_COMPILE:-aarch64-linux-android-}"
JOBS="${JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || nproc)}"
HOSTCFLAGS="${HOSTCFLAGS:--fcommon}"
KCFLAGS="${KCFLAGS:--Wno-error=maybe-uninitialized}"

make -C "${ROOT}" O="${OUT_DIR}" ARCH="${ARCH}" \
    CROSS_COMPILE="${CROSS_COMPILE}" HOSTCFLAGS="${HOSTCFLAGS}" \
    KCFLAGS="${KCFLAGS}" "${DEFCONFIG}"

make -C "${ROOT}" O="${OUT_DIR}" ARCH="${ARCH}" \
    CROSS_COMPILE="${CROSS_COMPILE}" HOSTCFLAGS="${HOSTCFLAGS}" \
    KCFLAGS="${KCFLAGS}" -j"${JOBS}" Image.gz dtbs

printf "Kernel image: %s/arch/%s/boot/Image.gz\n" "${OUT_DIR}" "${ARCH}"
