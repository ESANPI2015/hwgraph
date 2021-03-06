cmake_minimum_required(VERSION 2.8)

project(hwgraph)

include_directories(src)
include_directories(include)

SET(ENV{PKG_CONFIG_PATH}
    ${CMAKE_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}
    )

# Yes, we have to add the directory at least
if(NOT TARGET hypergraph)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../hypergraph hypergraph)
endif(NOT TARGET hypergraph)
if(NOT TARGET componentnet)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../componentnet componentnet)
endif(NOT TARGET componentnet)
if(NOT TARGET yaml-cpp)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ext/yaml-cpp yaml-cpp)
endif(NOT TARGET yaml-cpp)

find_package(PkgConfig)

pkg_check_modules(hwgraph_PKGCONFIG REQUIRED
    hypergraph componentnet yaml-cpp
    )

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 11)
include_directories(${hwgraph_PKGCONFIG_INCLUDE_DIRS})
#message(${hwgraph_PKGCONFIG_INCLUDE_DIRS})
link_directories(${hwgraph_PKGCONFIG_LIBRARY_DIRS})
add_definitions(${hwgraph_PKGCONFIG_CFLAGS_OTHER} --pedantic -Wall)

set(SOURCES
    src/ComputationalGraph.cpp
    )
set(HEADERS
    include/ComputationalGraph.hpp
    )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(hwgraph STATIC ${SOURCES})
target_link_libraries(hwgraph
    ${hwgraph_PKGCONFIG_STATIC_LIBRARIES}
    )

#add_executable(dbg-gen-vhdl-toplvl src/gen_vhdl_toplvl.cpp)
#target_link_libraries(dbg-gen-vhdl-toplvl hwgraph)
#install(TARGETS dbg-gen-vhdl-toplvl
#RUNTIME DESTINATION bin)

add_executable(dbg-import-hw-model src/import_hardware_graph.cpp)
target_link_libraries(dbg-import-hw-model hwgraph)
install(TARGETS dbg-import-hw-model
RUNTIME DESTINATION bin)

add_executable(dbg-export-hw-model src/export_hardware_graph.cpp)
target_link_libraries(dbg-export-hw-model hwgraph)
install(TARGETS dbg-export-hw-model
RUNTIME DESTINATION bin)

# pkg-config, to be installed:
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)
# pkg-config for use inside our buildtree
configure_file(${PROJECT_NAME}-uninstalled.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-uninstalled.pc @ONLY)
