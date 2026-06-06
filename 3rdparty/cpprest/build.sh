#!/usr/bin/env bash
#
# build.sh — Build C++ REST SDK (cpprestsdk) 2.10.19 from the bundled tarball.
#
# Intended for a REST API server, so websocket support (the websocketpp
# submodule, which is NOT included in the tarball) is excluded by default.
# Nothing is installed (no cmake --install): the built .so files and headers
# are collected into lib/ and include/ respectively.
#
# Usage:
#   ./build.sh                # build, then collect headers->include/, .so->lib/ (default: Release, shared, no websockets)
#   ./build.sh clean          # remove build artifacts (build/, lib/, include/)
#   ./build.sh distclean      # clean + remove the extracted source tree
#
# Environment variables:
#   BUILD_TYPE=Debug          # default Release
#   BUILD_SHARED=OFF          # build static libraries (default ON = .so)
#   WITH_WEBSOCKETS=ON        # include websockets (requires websocketpp source)
#   JOBS=4                    # parallel build jobs (default: nproc)

set -euo pipefail

# --- Paths --------------------------------------------------------------------
# This script lives in 3rdparty/cpprest/, and the tarball is in the same dir.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CPPREST_VERSION="2.10.19"
TARBALL="${SCRIPT_DIR}/cpprestsdk-${CPPREST_VERSION}.tar.gz"
SRC_DIR="${SCRIPT_DIR}/cpprestsdk-${CPPREST_VERSION}"
BUILD_DIR="${SCRIPT_DIR}/build"
LIB_DIR="${SCRIPT_DIR}/lib"
INCLUDE_DIR="${SCRIPT_DIR}/include"

# --- Option defaults ----------------------------------------------------------
BUILD_TYPE="${BUILD_TYPE:-Release}"
BUILD_SHARED="${BUILD_SHARED:-ON}"
WITH_WEBSOCKETS="${WITH_WEBSOCKETS:-OFF}"
JOBS="${JOBS:-$(nproc)}"

# --- Colored log helpers ------------------------------------------------------
log()  { printf '\033[1;32m[build]\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m[warn]\033[0m %s\n' "$*"; }
die()  { printf '\033[1;31m[error]\033[0m %s\n' "$*" >&2; exit 1; }

# --- clean targets ------------------------------------------------------------
case "${1:-}" in
    clean)
        log "Removing build/, lib/, include/"
        rm -rf "${BUILD_DIR}" "${LIB_DIR}" "${INCLUDE_DIR}"
        exit 0
        ;;
    distclean)
        log "Removing build/, lib/, include/, and extracted source"
        rm -rf "${BUILD_DIR}" "${LIB_DIR}" "${INCLUDE_DIR}" "${SRC_DIR}"
        exit 0
        ;;
    "")
        ;;
    *)
        die "Unknown target: ${1}  (use: clean | distclean)"
        ;;
esac

# --- Preflight checks ---------------------------------------------------------
log "Checking required tools..."
for tool in cmake g++ tar; do
    command -v "${tool}" >/dev/null 2>&1 || die "'${tool}' not found. Please install it."
done

# Pick the build generator: prefer ninja, fall back to Unix Makefiles.
if command -v ninja >/dev/null 2>&1; then
    GENERATOR="Ninja"
else
    GENERATOR="Unix Makefiles"
fi
log "Using generator: ${GENERATOR}"

[ -f "${TARBALL}" ] || die "Tarball not found: ${TARBALL}"

# --- Extract source -----------------------------------------------------------
if [ ! -d "${SRC_DIR}" ]; then
    log "Extracting ${TARBALL##*/} ..."
    tar -xzf "${TARBALL}" -C "${SCRIPT_DIR}"
    [ -d "${SRC_DIR}" ] || die "Extraction did not produce expected dir: ${SRC_DIR}"
else
    log "Source already extracted: ${SRC_DIR}"
fi

# If websockets are requested but the websocketpp source is missing, fail early.
if [ "${WITH_WEBSOCKETS}" = "ON" ]; then
    if [ ! -f "${SRC_DIR}/Release/libs/websocketpp/CMakeLists.txt" ]; then
        die "WITH_WEBSOCKETS=ON but websocketpp source is missing (git submodule not included). \
Build with WITH_WEBSOCKETS=OFF, or place websocketpp under ${SRC_DIR}/Release/libs/websocketpp."
    fi
    EXCLUDE_WS="OFF"
else
    EXCLUDE_WS="ON"
fi

# --- CMake configure ----------------------------------------------------------
log "Configuring (type=${BUILD_TYPE}, shared=${BUILD_SHARED}, websockets=${WITH_WEBSOCKETS})"
cmake -S "${SRC_DIR}" -B "${BUILD_DIR}" -G "${GENERATOR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DBUILD_SHARED_LIBS="${BUILD_SHARED}" \
    -DCPPREST_EXCLUDE_WEBSOCKETS="${EXCLUDE_WS}" \
    -DBUILD_TESTS=OFF \
    -DBUILD_SAMPLES=OFF \
    -DWERROR=OFF \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON

# --- Build --------------------------------------------------------------------
log "Building with ${JOBS} jobs..."
cmake --build "${BUILD_DIR}" --parallel "${JOBS}"

# --- Collect shared libraries -------------------------------------------------
# No install step: pick up the libcpprest shared libraries built in the tree
# and copy them into lib/. Use -a to preserve symlinks.
log "Collecting shared libraries into ${LIB_DIR}"
mkdir -p "${LIB_DIR}"

mapfile -t SO_FILES < <(find "${BUILD_DIR}" -name 'libcpprest*.so*' -type f)
[ "${#SO_FILES[@]}" -gt 0 ] || die "No built .so files found. (BUILD_SHARED=${BUILD_SHARED})"

# Copy both the real files and the symlinks pointing to them
# (e.g. libcpprest.so -> libcpprest.so.2.10).
for so in "${SO_FILES[@]}"; do
    cp -av "${so%/*}/"libcpprest*.so* "${LIB_DIR}/" 2>/dev/null || true
done

# --- Collect headers ----------------------------------------------------------
# cpprestsdk public headers live under the source tree's Release/include.
log "Collecting headers into ${INCLUDE_DIR}"
rm -rf "${INCLUDE_DIR}"
mkdir -p "${INCLUDE_DIR}"
cp -a "${SRC_DIR}/Release/include/." "${INCLUDE_DIR}/"

# --- Remove intermediate artifacts --------------------------------------------
# Only reached after a successful build that collected headers/libs (set -e).
# Keep the outputs (include/, lib/) and drop the build tree and source.
log "Removing intermediate artifacts (build/, extracted source)"
rm -rf "${BUILD_DIR}" "${SRC_DIR}"

log "Done."
log "  Headers     : ${INCLUDE_DIR}/"
log "  Shared libs : ${LIB_DIR}/"
ls -l "${LIB_DIR}"
log "  Example     : g++ main.cpp -I${INCLUDE_DIR} -L${LIB_DIR} -lcpprest -lssl -lcrypto -lboost_system -pthread"
