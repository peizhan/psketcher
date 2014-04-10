# Minimum required version of CMake
cmake_minimum_required(VERSION 2.8.4)
if(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
  CMAKE_POLICY(SET CMP0007 OLD)
  CMAKE_POLICY(SET CMP0017 OLD)
  if ("${CMAKE_VERSION}" VERSION_GREATER 2.99)
    CMAKE_POLICY(SET CMP0026 OLD)
  endif ("${CMAKE_VERSION}" VERSION_GREATER 2.99)
endif(COMMAND CMAKE_POLICY)

# set CMake project name
project(DIME)

set(DIME_MAJOR 0)
set(DIME_MINOR 9)
set(DIME_PATCH 1)
set(DIME_VERSION_STRING "${DIME_MAJOR}.${DIME_MINOR}.${DIME_PATCH}")
set(DIME_VERSION ${DIME_VERSION_STRING})

set(DIME_ABI_VERSION 0)
set(DIME_ABI_AGE 0)
set(DIME_ABI_REVISION 0)

add_definitions(-DDIME_INTERNAL)

include(CheckIncludeFiles)
CHECK_INCLUDE_FILES(ieeefp.h HAVE_IEEEFP_H)

include(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(isinf HAVE_ISINF)
CHECK_FUNCTION_EXISTS(isnan HAVE_ISNAN)
CHECK_FUNCTION_EXISTS(_isnan HAVE__ISNAN)
CHECK_FUNCTION_EXISTS(finite HAVE_FINITE)
CHECK_FUNCTION_EXISTS(_finite HAVE__FINITE)
CHECK_FUNCTION_EXISTS(fpclass HAVE_FPCLASS)
CHECK_FUNCTION_EXISTS(_fpclass HAVE__FPCLASS)

configure_file(include/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

add_subdirectory(src)

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8
