#!/bin/bash

cmake_minimum_required(VERSION 2.8.0)

project(RogueRiver C CXX)
set (RogueRiver_VERSION_MAJOR 0)
set (RogueRiver_VERSION_MINOR 1)
set (RogueRiver_VERSION_PATCH 1)

# We require C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
# Manually specify C++14 for Travis CI
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(CMAKE_USE_RELATIVE_PATHS True)

# Our CMake Modules (For SDL2)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake-modules)

# Find out if this is a 32 or 64 bit build
if( CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE(STATUS "64 bit system detected.")
    SET( EX_PLATFORM 64)
    SET( EX_PLATFORM_NAME "x64")
else ()
    MESSAGE(STATUS "32 bit system detected.")
    SET( EX_PLATFORM 32)
    SET( EX_PLATFORM_NAME "x86")
endif ()

# When building, put the executable in the root directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add bearlibterminal
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(LIB_PATHS lib/OSX)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(LIB_PATHS "lib/Windows${EX_PLATFORM}")
else ()
    set(LIB_PATHS "lib/Linux${EX_PLATFORM}")
endif ()
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows") 
    # Windows doesn't automatically include both .lib and .dll
    if (${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
    else (${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
        set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
    endif (${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
    find_library(bearlibterminal NAME BearLibTerminal PATHS ${LIB_PATHS})
else (${CMAKE_SYSTEM_NAME} MATCHES "Windows") 
    find_library(bearlibterminal NAME BearLibTerminal PATHS ${LIB_PATHS})
endif (${CMAKE_SYSTEM_NAME} MATCHES "Windows") 
message(STATUS "Found BearLibTerminal at: ${bearlibterminal}")

# Make the project
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
file(COPY ${CMAKE_SOURCE_DIR}/graphics DESTINATION ${CMAKE_BINARY_DIR})

# Installation details
FILE(GLOB assets ${CMAKE_SOURCE_DIR}/graphics/*.jpg)
install(FILES ${assets}          DESTINATION graphics)
install(FILES ${bearlibterminal} DESTINATION ./)
if (${CMAKE_CXX_COMPILER_ID} EQUAL "MSVC")
    install(FILES lib/Windows${EX_PLATFORM}/BearLibTerminal.dll DESTINATION ./)
endif ()

include (InstallRequiredSystemLibraries)
if (${WIN32})
    set(CPACK_GENERATOR "ZIP")
else ()
    set(CPACK_GENERATOR "TGZ")
endif ()
set(CPACK_PACKAGE_NAME "RogueRiver")
set(CPACK_PACKAGE_VERSION_MAJOR ${RogueRiver_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${RogueRiver_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${RogueRiver_VERSION_PATCH})
set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${EX_PLATFORM_NAME}-${CMAKE_CXX_COMPILER_ID}")
include(CPack)



