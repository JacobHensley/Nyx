# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget spirv-cross-c-shared)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target spirv-cross-c-shared
add_library(spirv-cross-c-shared SHARED IMPORTED)

set_target_properties(spirv-cross-c-shared PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross"
)

# Import target "spirv-cross-c-shared" for configuration "Debug"
set_property(TARGET spirv-cross-c-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(spirv-cross-c-shared PROPERTIES
  IMPORTED_IMPLIB_DEBUG "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross/build/Debug/spirv-cross-c-sharedd.lib"
  IMPORTED_LOCATION_DEBUG "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross/build/Debug/spirv-cross-c-sharedd.dll"
  )

# Import target "spirv-cross-c-shared" for configuration "Release"
set_property(TARGET spirv-cross-c-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(spirv-cross-c-shared PROPERTIES
  IMPORTED_IMPLIB_RELEASE "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross/build/Release/spirv-cross-c-shared.lib"
  IMPORTED_LOCATION_RELEASE "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross/build/Release/spirv-cross-c-shared.dll"
  )

# Import target "spirv-cross-c-shared" for configuration "MinSizeRel"
set_property(TARGET spirv-cross-c-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(spirv-cross-c-shared PROPERTIES
  IMPORTED_IMPLIB_MINSIZEREL "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross/build/MinSizeRel/spirv-cross-c-shared.lib"
  IMPORTED_LOCATION_MINSIZEREL "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross/build/MinSizeRel/spirv-cross-c-shared.dll"
  )

# Import target "spirv-cross-c-shared" for configuration "RelWithDebInfo"
set_property(TARGET spirv-cross-c-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(spirv-cross-c-shared PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross/build/RelWithDebInfo/spirv-cross-c-shared.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "C:/dev/Nyx/Nyx/vendor/SPIRV/SPIRV-Cross/build/RelWithDebInfo/spirv-cross-c-shared.dll"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)