##########################################################
# CMake configuration for HPS sim application.           #
#                                                        #
# @author Jeremy McCormick <jeremym@slac.stanford.edu>   #
#                                                        #
##########################################################

project(HPSSIM)
set(HPSSIM_VERSION 0.0.1)
cmake_minimum_required(VERSION 3.0)

if(CMAKE_CXX_COMPILER_VERSION LESS 4.8)
    message(FATAL_ERROR "The gcc compiler version '${CMAKE_CXX_COMPILER_VERSION}' is too old (at least 4.8 required).")
endif()

#
# This is needed for the JLab compilation to not use c++98
#
set (CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(XERCES REQUIRED)
find_package(Geant4 REQUIRED ui_all vis_all)
find_package(GDML REQUIRED)
find_package(LCDD REQUIRED)
find_package(LCIO REQUIRED)

file(GLOB_RECURSE library_sources ${PROJECT_SOURCE_DIR}/src/*.cxx)
add_executable(hps-sim ${library_sources} src/hps-sim.cxx)

include(${Geant4_USE_FILE})

include_directories(include/)
include_directories(${XERCES_INCLUDE_DIR} ${LCIO_INCLUDE_DIRS} ${Geant4_INCLUDE_DIRS} ${GDML_INCLUDE_DIR} ${LCDD_INCLUDE_DIR}) 

# build user plugin library
FILE(GLOB_RECURSE plugin_sources plugins/*.cxx)
ADD_LIBRARY(SimPlugins SHARED ${plugin_sources})
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   target_link_libraries(SimPlugins "-undefined dynamic_lookup" ${Geant4_LIBRARIES})
endif()
INSTALL(TARGETS SimPlugins DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
ADD_DEPENDENCIES(hps-sim SimPlugins)
    
target_link_libraries(hps-sim ${XERCES_LIBRARY} ${Geant4_LIBRARIES} ${GDML_LIBRARY} ${LCDD_LIBRARY} ${LCIO_LIBRARIES} dl)
link_directories(${GDML_LIBRARY_DIR} ${LCDD_LIBRARY_DIR} ${LCIO_LIBRARY_DIRS})

install(TARGETS hps-sim hps-sim DESTINATION bin)

configure_file(scripts/hps-sim-env.sh.in ${CMAKE_CURRENT_BINARY_DIR}/hps-sim-env.sh)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hps-sim-env.sh DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
