cmake_minimum_required(VERSION 3.20)

project(launcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(launcher
    src/main.cpp
)

# Include directories
target_include_directories(launcher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(launcher PRIVATE
     Comdlg32
 )
