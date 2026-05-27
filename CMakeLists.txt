cmake_minimum_required(VERSION 3.20)
project(GTASA_SDK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


# SDK Source
file(GLOB_RECURSE SDK_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# Scripts 
file(GLOB_RECURSE SCRIPTS_SOURCES CONFIGURE_DEPENDS
    scripts/*.cpp
)

# Make Shared library
add_library(GTASA_SDK SHARED
    ${SDK_SOURCES}
    ${SCRIPTS_SOURCES}
)

# Include directories
target_include_directories(GTASA_SDK PRIVATE
    src
)

# DirectX9
target_include_directories(GTASA_SDK PRIVATE "$ENV{DXSDK_DIR}/Include")

target_link_libraries(GTASA_SDK PRIVATE
    d3d9
)

# Detours
add_subdirectory(libs/Detours)
target_include_directories(GTASA_SDK PRIVATE
    libs/Detours/src
)

target_link_libraries(GTASA_SDK PRIVATE detours)

# GTA SA Path
set(GTA_SA_PATH "${GTA_SA_PATH}" CACHE PATH "Path to GTA San Andreas" FORCE)

if(NOT GTA_SA_PATH)
    message(FATAL_ERROR "GTA_SA_PATH must be specified with -DGTA_SA_PATH=\"...\"")
endif()

add_custom_command(TARGET GTASA_SDK POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:GTASA_SDK>
        "${GTA_SA_PATH}/samp.dll"
)