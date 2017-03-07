 #!/bin/bash

cmake_minimum_required(VERSION 2.8.0)

# RPATH Settings
# These settings are used to link the libtcod libraries with the executable
# upon installation
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

FILE(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_executable(RogueRiver ${SOURCES})
target_link_libraries(RogueRiver ${bearlibterminal})

# Installation
install(TARGETS RogueRiver DESTINATION bin)
