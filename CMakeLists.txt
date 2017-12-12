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

find_package(PkgConfig)
pkg_check_modules(hwgraph_PKGCONFIG REQUIRED
    hypergraph yaml-cpp
    )

include_directories(${hwgraph_PKGCONFIG_INCLUDE_DIRS})
#message(${hwgraph_PKGCONFIG_INCLUDE_DIRS})
link_directories(${hwgraph_PKGCONFIG_LIBRARY_DIRS})
add_definitions(${hwgraph_PKGCONFIG_CFLAGS_OTHER} -ggdb --pedantic -Wall -std=c++11)

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

add_executable(dbg-gen-vhdl-toplvl src/gen_vhdl_toplvl.cpp)
target_link_libraries(dbg-gen-vhdl-toplvl hwgraph)
install(TARGETS dbg-gen-vhdl-toplvl
RUNTIME DESTINATION bin)

add_executable(dbg-import-hw-model src/import_hardware_graph.cpp)
target_link_libraries(dbg-import-hw-model hwgraph)
install(TARGETS dbg-import-hw-model
RUNTIME DESTINATION bin)

# pkg-config, to be installed:
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)
# pkg-config for use inside our buildtree
configure_file(${PROJECT_NAME}-uninstalled.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-uninstalled.pc @ONLY)
