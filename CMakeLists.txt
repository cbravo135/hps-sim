
project(HPS_SIM)
set(HPS_SIM_VERSION 0.0.1)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 14)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_VERSION LESS 6.1)
    message(FATAL_ERROR "The gcc compiler version '${CMAKE_CXX_COMPILER_VERSION}' is too old (at least 6.1 required).")
endif()
#
# OPTIMIZE Release
#
# Set a default build type if none was specified
set(default_build_type "Release")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(XERCES REQUIRED)
if(XERCES_DIR)
    if (NOT EXISTS "${XERCES_DIR}")
        message(FATAL_ERROR "Unable to guess location of Xerces lib.  Please specify using: -DXERCES_DIR")
  endif()
  message(STATUS "XercesC dir set to: ${XERCES_DIR}")
  message(STATUS "XercesC include dir set to: ${XERCES_INCLUDE_DIR}")
  message(STATUS "XercesC library set to: ${XERCES_LIBRARY}")
endif()

find_package(Geant4 REQUIRED ui_all vis_all)

find_package(GDML REQUIRED)
if(GDML_DIR)
    if (NOT EXISTS "${GDML_DIR}")
        message(FATAL_ERROR "Unable to guess location of Xerces lib.  Please specify using: -DXERCES_DIR")
  endif()
  message(STATUS "GDML dir set to: ${GDML_DIR}")
  message(STATUS "GDML include dir set to: ${GDML_INCLUDE_DIR}")
  message(STATUS "GDML library set to: ${GDML_LIBRARY}")
endif()

find_package(LCDD REQUIRED)
if (LCIO_DIR)
    message(STATUS "LCDD dir set to: ${LCDD_DIR}")
    message(STATUS "LCDD include dir set to: ${LCDD_INCLUDE_DIR}")
    message(STATUS "LCDD library set to: ${LCDD_LIBRARY}")
endif()

find_package(LCIO REQUIRED)
if (LCIO_DIR)
    message(STATUS "LCIO dir set to: ${LCIO_DIR}")
    message(STATUS "LCIO include dir set to: ${LCIO_INCLUDE_DIR}")
    message(STATUS "LCIO library set to: ${LCIO_LIBRARY}")
endif()

# import macro for declaring modules
include(MacroModule)

# import macro for declaring external dependencies
include(MacroExtDeps)

set(MODULES sim_app filters)

# build each module in the list
foreach(module ${MODULES})
  message(STATUS "Adding module: ${module}")
  add_subdirectory(${module})
  if(EXTRA_INC)
      message(STATUS "Adding include directory: ${EXTRA_INC}")
      include_directories(${EXTRA_INC}) 
      unset(EXTRA_INC)
  endif()
endforeach()

# configure and install env setup script
configure_file(scripts/hps-sim-env.sh.in ${CMAKE_CURRENT_BINARY_DIR}/hps-sim-env.sh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hps-sim-env.sh DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
