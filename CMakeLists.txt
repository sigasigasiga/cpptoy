cmake_minimum_required(VERSION 3.30)


# stolen from Hana Dusikova's CTRE library
# https://github.com/hanickadot/compile-time-regular-expressions/blob/dab09336d/CMakeLists.txt#L5-L16
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.30" AND CMAKE_VERSION VERSION_LESS_EQUAL "4.0")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0" AND CMAKE_VERSION VERSION_LESS "4.0.3")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.3" AND CMAKE_VERSION VERSION_LESS "4.3")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
    set(CMAKE_CXX_MODULE_STD 1)
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.3")
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
    set(CMAKE_CXX_MODULE_STD 1)
else()
    message(WARNING "Update the `import std` gate ID for CMake version ${CMAKE_VERSION}")
endif()

project(
    cpptoy
    VERSION 0.1
    LANGUAGES CXX
)

# Generate a .gitignore in the build directory to ignore all its contents
if (NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  file(WRITE "${CMAKE_BINARY_DIR}/.gitignore" "*\n")
endif()

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Dependencies

include(FetchContent)

FetchContent_Declare(
    grace

    GIT_REPOSITORY https://github.com/sigasigasiga/grace.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(grace)

find_package(Boost)

add_subdirectory(dummy_a)
add_subdirectory(dummy_b)

# ===

add_executable(
    cpptoy

    main.cpp
)

target_link_libraries(
    cpptoy

    PRIVATE
    dummy_a
    dummy_b
    grace
)

if (Boost_FOUND)
    message(STATUS "Boost version: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(WARNING "Boost not found")
endif()
