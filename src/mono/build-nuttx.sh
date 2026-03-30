#!/usr/bin/env bash
#
# Build Mono runtime for NuttX / ARM Cortex-M7 (STM32F777)
#
# Prerequisites:
#   - arm-none-eabi-gcc on PATH
#   - NuttX headers available (NUTTX_INCLUDE_DIR)
#   - CMake >= 3.26, ninja (or make)
#
# Usage:
#   ./build-nuttx.sh [Release|Debug]
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RUNTIME_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
MONO_SRC="${SCRIPT_DIR}"

BUILD_TYPE="${1:-Debug}"
BUILD_TYPE_LOWER="$(echo "${BUILD_TYPE}" | tr '[:upper:]' '[:lower:]')"
BUILD_DIR="${MONO_SRC}/build-nuttx-${BUILD_TYPE_LOWER}"

# Default NuttX include path — sibling Meadow.OS repo
if [ -z "${NUTTX_INCLUDE_DIR:-}" ]; then
  CANDIDATE="${RUNTIME_ROOT}/../Meadow/nuttx/include"
  if [ -d "${CANDIDATE}" ]; then
    NUTTX_INCLUDE_DIR="$(cd "${CANDIDATE}" && pwd)"
  else
    echo "ERROR: NUTTX_INCLUDE_DIR not set and ${CANDIDATE} not found."
    echo "Set NUTTX_INCLUDE_DIR to your NuttX include/ directory."
    exit 1
  fi
fi

echo "=== Mono NuttX Build ==="
echo "  Build type:       ${BUILD_TYPE}"
echo "  Build dir:        ${BUILD_DIR}"
echo "  NuttX includes:   ${NUTTX_INCLUDE_DIR}"
echo "  Toolchain:        $(which arm-none-eabi-gcc)"
echo ""

# Verify toolchain
if ! command -v arm-none-eabi-gcc &>/dev/null; then
  echo "ERROR: arm-none-eabi-gcc not found on PATH"
  exit 1
fi

# Pick generator
if command -v ninja &>/dev/null; then
  GENERATOR="Ninja"
else
  GENERATOR="Unix Makefiles"
fi

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake "${MONO_SRC}" \
  -G "${GENERATOR}" \
  -DCMAKE_TOOLCHAIN_FILE="${MONO_SRC}/cmake/toolchains/arm-none-eabi-nuttx.cmake" \
  -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
  -DNUTTX_INCLUDE_DIR="${NUTTX_INCLUDE_DIR}" \
  -DMONO_ARM_FPU=vfp-hard \
  -DDISABLE_JIT=0 \
  -DDISABLE_AOT=1 \
  -DDISABLE_DEBUGGER_AGENT=1 \
  -DDISABLE_LOG_PROFILER_GZ=1 \
  -DDISABLE_SOCKETS=0 \
  -DDISABLE_EVENTPIPE=1 \
  -DDISABLE_EMBEDDED_PDB=1 \
  -DDISABLE_SIMD=1 \
  -DDISABLE_SSA=1 \
  -DDISABLE_LLDB=1 \
  -DDISABLE_SGEN_SPLIT_NURSERY=1 \
  -DDISABLE_SGEN_DEBUG_HELPERS=1 \
  -DDISABLE_SGEN_BINARY_PROTOCOL=1 \
  -DDISABLE_SGEN_TOGGLEREF=1 \
  -DDISABLE_SGEN_GC_BRIDGE=1 \
  -DENABLE_SMALL_CONFIG=1 \
  -DSTATIC_COMPONENTS=1 \
  -DDISABLE_SGEN_MAJOR_MARKSWEEP_CONC=1

echo ""
echo "=== CMake configure complete. Building... ==="
echo ""

cmake --build . -- -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu)"

echo ""
echo "=== Build complete ==="

# Show the output library
LIB=$(find . -name "libmonosgen*.a" -o -name "libcoreclr*.a" | head -1)
if [ -n "${LIB}" ]; then
  echo "Output: ${BUILD_DIR}/${LIB}"
  ls -lh "${LIB}"
else
  echo "WARNING: No .a library found in build output"
fi
