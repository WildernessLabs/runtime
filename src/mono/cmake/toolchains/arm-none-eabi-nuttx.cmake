# CMake toolchain file for cross-compiling Mono to ARM Cortex-M7 / NuttX
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=<path>/arm-none-eabi-nuttx.cmake \
#         -DNUTTX_INCLUDE_DIR=<path-to-nuttx>/include \
#         ...
#
# Required variables (pass via -D or environment):
#   NUTTX_INCLUDE_DIR  — Path to NuttX include/ directory (for pthread.h, etc.)

# cmake_minimum_required is set in the project CMakeLists.txt, not here.
# Toolchain files are loaded before project() — setting it here can
# cause version conflicts with the invoking CMake.

# Use "Generic" since CMake doesn't have a built-in NuttX system name.
# We set CMAKE_SYSTEM_VARIANT to "nuttx" so CMakeLists.txt can detect it.
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VARIANT nuttx)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain executables
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_RANLIB arm-none-eabi-ranlib)
set(CMAKE_STRIP arm-none-eabi-strip)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)

# Architecture flags: Cortex-M7, Thumb2-only, hard float VFPv5-D16
set(NUTTX_ARCH_FLAGS "-mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16")

# NOTE on int32_t / uint32_t:
# NuttX 12 (nuttx/arch/arm/include/types.h) takes _int32_t from the compiler's
# __INT32_TYPE__, which is 'long int' for arm-none-eabi GCC.  Mono code that
# mixes 'int *' and 'int32_t *' therefore gets -Wincompatible-pointer-types,
# which GCC 14 and later treat as an error; CMakeLists.txt downgrades that one
# diagnostic to a warning for HOST_NUTTX.  The two types have the same size and
# alignment on ARM.  Declaration/definition mismatches ('conflicting types')
# are fixed in the source instead.

# NuttX 12 defines errno as (*__errno()), a per-thread lvalue, and supports
# O_CLOEXEC, so the old errno.h override and -DO_CLOEXEC=0 are no longer used.

# Definitions the NuttX headers still lack are provided by a force-included
# compat header rather than by modifying NuttX headers.
set(NUTTX_COMPAT_FIX "-include ${CMAKE_CURRENT_LIST_DIR}/../nuttx-compat.h")

set(CMAKE_C_FLAGS_INIT "${NUTTX_ARCH_FLAGS} ${NUTTX_COMPAT_FIX} -O1")
set(CMAKE_CXX_FLAGS_INIT "${NUTTX_ARCH_FLAGS} ${NUTTX_COMPAT_FIX} -O1")
set(CMAKE_ASM_FLAGS_INIT "${NUTTX_ARCH_FLAGS}")

# NuttX sysroot / include path
if(DEFINED ENV{NUTTX_INCLUDE_DIR} AND NOT NUTTX_INCLUDE_DIR)
  set(NUTTX_INCLUDE_DIR "$ENV{NUTTX_INCLUDE_DIR}")
endif()

if(NUTTX_INCLUDE_DIR)
  include_directories(SYSTEM "${NUTTX_INCLUDE_DIR}")
  # Also add the arch-specific includes
  if(EXISTS "${NUTTX_INCLUDE_DIR}/arch")
    include_directories(SYSTEM "${NUTTX_INCLUDE_DIR}/arch")
  endif()
endif()

# Static libraries only — no shared libs for embedded
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# No executables for embedded target (we produce .a libraries)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Don't search host paths for programs/libraries/includes
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Suppress newlib's warnings about nano specs not being available
set(CMAKE_EXE_LINKER_FLAGS_INIT "--specs=nosys.specs")
