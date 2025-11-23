cmake_minimum_required(VERSION 3.24)

# Set policy to use uppercase *_ROOT variables
cmake_policy(SET CMP0144 NEW)

project(BattleBomber)

find_package(raylib QUIET)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Allow user to explicitly point to a raylib directory:
if(NOT DEFINED RAYLIB_ROOT)
    if(EXISTS "${CMAKE_SOURCE_DIR}/external/raylib-master")
        set(RAYLIB_ROOT "${CMAKE_SOURCE_DIR}/external/raylib-master")
    else()
        set(RAYLIB_ROOT "${CMAKE_SOURCE_DIR}/external/raylib")
    endif()
endif()

# Try to find installed raylib first
find_package(raylib QUIET)

# If not found but external/raylib exists, add it as a subproject
if(NOT raylib_FOUND AND EXISTS "${RAYLIB_ROOT}")
    message(STATUS "Adding external raylib from: ${RAYLIB_ROOT}")
    add_subdirectory("${RAYLIB_ROOT}" "external/raylib-build")
    # raylib CMake usually creates a target called raylib or raylib_static
    if(TARGET raylib)
        set(RAYLIB_TARGET raylib)
    elseif(TARGET raylib_static)
        set(RAYLIB_TARGET raylib_static)
    endif()
else()
    if(raylib_FOUND)
        message(STATUS "Found installed raylib: ${raylib_VERSION}")
        # CMake find_package will provide raylib::raylib target
        set(RAYLIB_TARGET raylib::raylib)
    endif()
endif()

if(NOT RAYLIB_TARGET)
    message(FATAL_ERROR "raylib not found. Install raylib or put a copy in external/raylib-master or set -DRAYLIB_ROOT=path/to/raylib")
endif()

# Collect sources
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
    "${CMAKE_SOURCE_DIR}/src/*.rc"
)

# Create executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# On Windows, mark the resource file so MSVC links it properly (CMake will handle .rc automatically when in sources)
if(WIN32)
    # ensure we have the application.rc included (if present)
    if(EXISTS "${CMAKE_SOURCE_DIR}/src/application.rc")
        target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/application.rc")
    endif()
endif()

# Include headers
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
)

# Link raylib
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_TARGET})

# Platform-specific flags/links
if(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads dl m)
endif()

# Output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Helpful CMake options
option(BUILD_EXAMPLES "Build example executables" OFF)

message(STATUS "Project configured. To build:")
message(STATUS "  cmake -S . -B build -G \"Visual Studio 17 2022\"")
message(STATUS "  cmake --build build --config Debug")
message(STATUS "Or with MinGW:")
message(STATUS "  cmake -S . -B build -G \"MinGW Makefiles\"")
message(STATUS "  cmake --build build --config Debug")