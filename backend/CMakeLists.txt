cmake_minimum_required(VERSION 3.15)
project(EDR_System)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(WIN32)
    add_definitions(-DWINDOWS_PLATFORM)
    set(PLATFORM_LIBS ws2_32)
elseif(UNIX)
    add_definitions(-DLINUX_PLATFORM)
    set(PLATFORM_LIBS pthread rt)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Source files
file(GLOB_RECURSE SOURCES 
    "src/core/*.cpp"
    "src/monitoring/*.cpp"
    "src/simulation/*.cpp"
    "src/utils/*.cpp"
)

# Main executable
add_executable(edr_system src/main.cpp ${SOURCES})
target_link_libraries(edr_system ${PLATFORM_LIBS})

# Enable warnings
if(MSVC)
    target_compile_options(edr_system PRIVATE /W4)
else()
    target_compile_options(edr_system PRIVATE -Wall -Wextra -pedantic)
endif()
