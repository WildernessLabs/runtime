#!/usr/bin/env bash
#
# Build Mono CoreLib (System.Private.CoreLib.dll) for NuttX / ARM Cortex-M7
#
# This builds the MANAGED CoreLib that runs on the target, not the native
# libmonosgen-2.0.a (use build-nuttx.sh for that).
#
# The output DLL must be deployed to the device's BCL directory alongside
# the other framework assemblies.
#
# Prerequisites:
#   - .NET SDK provisioned at <runtime>/.dotnet/
#     (run eng/install-scripts/dotnet-install.sh if missing)
#
# Usage:
#   ./build-mono-bcl.sh [Release|Debug]
#
# Output:
#   artifacts/obj/mono/System.Private.CoreLib/linux.arm.<config>/System.Private.CoreLib.dll
#
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RUNTIME_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

BUILD_CONFIG="${1:-Release}"

DOTNET="${RUNTIME_ROOT}/.dotnet/dotnet"
if [ ! -x "${DOTNET}" ]; then
  echo "ERROR: .NET SDK not found at ${DOTNET}"
  echo "Run: curl -sSL https://builds.dotnet.microsoft.com/dotnet/scripts/v1/dotnet-install.sh | bash -s -- --install-dir ${RUNTIME_ROOT}/.dotnet"
  exit 1
fi

CORELIB_PROJ="${SCRIPT_DIR}/System.Private.CoreLib/System.Private.CoreLib.csproj"
if [ ! -f "${CORELIB_PROJ}" ]; then
  echo "ERROR: CoreLib project not found at ${CORELIB_PROJ}"
  exit 1
fi

echo "=== Mono CoreLib Build (NuttX ARM) ==="
echo "  Config:   ${BUILD_CONFIG}"
echo "  Project:  ${CORELIB_PROJ}"
echo "  SDK:      ${DOTNET}"
echo ""

# NuttX-specific properties:
#   FeaturePerfTracing=false  — NuttX native has DISABLE_EVENTPIPE; managed must match
#   TargetArchitecture=arm    — Cortex-M7 (Thumb2)
#   TargetOS=linux            — NuttX uses POSIX/linux target profile
#   RuntimeFlavor=Mono        — Must produce Mono variant (has MonoStackFrame etc.)
export DOTNET_ROOT="${RUNTIME_ROOT}/.dotnet"

"${DOTNET}" build "${CORELIB_PROJ}" \
  -c "${BUILD_CONFIG}" \
  -p:TargetArchitecture=arm \
  -p:TargetOS=linux \
  -p:FeaturePerfTracing=false \
  -p:RuntimeFlavor=Mono

BIN_DLL="${RUNTIME_ROOT}/artifacts/bin/mono/linux.arm.${BUILD_CONFIG}/System.Private.CoreLib.dll"
OBJ_DLL="${RUNTIME_ROOT}/artifacts/obj/mono/System.Private.CoreLib/linux.arm.${BUILD_CONFIG}/System.Private.CoreLib.dll"

echo ""
if [ -f "${BIN_DLL}" ]; then
  SIZE=$(ls -lh "${BIN_DLL}" | awk '{print $5}')
  echo "=== Build complete ==="
  echo "  Output: ${BIN_DLL}"
  echo "  Size:   ${SIZE}"
  echo ""
  echo "Use with Meadow/build-mono-bcl.sh to package into bcl/ directory."
elif [ -f "${OBJ_DLL}" ]; then
  SIZE=$(ls -lh "${OBJ_DLL}" | awk '{print $5}')
  echo "=== Build complete ==="
  echo "  Output: ${OBJ_DLL}"
  echo "  Size:   ${SIZE}"
else
  echo "WARNING: Expected output not found."
  echo "  Checked: ${BIN_DLL}"
  echo "  Checked: ${OBJ_DLL}"
  echo "Check build output for errors."
  exit 1
fi
