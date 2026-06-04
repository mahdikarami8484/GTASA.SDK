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

# Output directories
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/out")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

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

# Launcher
add_subdirectory(launcher)



target_link_libraries(GTASA_SDK PRIVATE detours)

