cmake_minimum_required(VERSION 2.8)

project(hwGraph)

include_directories(src)
include_directories(include)

SET(ENV{PKG_CONFIG_PATH}
    ${CMAKE_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}
    )

find_package(PkgConfig)
pkg_check_modules(YAML REQUIRED yaml-0.1)
include_directories(${YAML_INCLUDE_DIRS})
link_directories(${YAML_LIBRARY_DIRS})
add_definitions(-DYAML_SUPPORT ${YAML_CFLAGS_OTHER})
set(EXTRA_LIBRARIES ${EXTRA_LIBRARIES} yaml)

include_directories(${hwGraphs_PKGCONFIG_INCLUDE_DIRS})
#message(${hwGraphs_PKGCONFIG_INCLUDE_DIRS})
link_directories(${hwGraphs_PKGCONFIG_LIBRARY_DIRS})
add_definitions(${hwGraphs_PKGCONFIG_CFLAGS_OTHER} -ggdb --pedantic -Wall -Werror)

set(SOURCES
    src/priority_list.c
    src/hwg.c
    src/hwg_yaml_writer.c
    src/hwg_yaml_loader.c
    )
set(HEADERS
    include/priority_list.h
    include/hwg.h
    include/hwg_yaml.h
    )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# at first we have a minimal cmdline app, which exports simply everything into plaintext files
add_library(hwGraph STATIC ${SOURCES})
target_link_libraries(hwGraph
    ${hwGraphs_PKGCONFIG_STATIC_LIBRARIES}
    )

# creating a convenience target, so a user can easily execute ileggui, without cd'ing into the builddir

# pkg-config, to be installed:
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)
# pkg-config for use inside our buildtree
configure_file(${PROJECT_NAME}-uninstalled.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-uninstalled.pc @ONLY)
