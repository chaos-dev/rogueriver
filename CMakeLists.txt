#!/bin/bash

# We require "target_compile_features", which is only available in 3.1+
cmake_minimum_required(VERSION 3.1.0)

project(RogueRiver C CXX)
set (RogueRiver_VERSION_MAJOR 0)
set (RogueRiver_VERSION_MINOR 1)

# We require C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# Our CMake Modules (For SDL2)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)

# When building, put the executable in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add bearlibterminal
find_library(bearlibterminal NAME BearLibTerminal
                             PATHS lib/Linux32
                                   lib/Linux64
                                   lib/OSX
                                   lib/Windows32
                                   lib/Windows64 )
message(STATUS "Found BearLibTerminal at: ${bearlibterminal}")

# Make the project
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
file(COPY ${CMAKE_SOURCE_DIR}/graphics DESTINATION ${CMAKE_BINARY_DIR} )

# Installation details
install(FILES ${bearlibterminal} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY graphics DESTINATION ${CMAKE_INSTALL_PREFIX})



