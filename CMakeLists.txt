cmake_minimum_required(VERSION 4.5.0) # `import std`

set(CMAKE_CXX_EXTENSIONS OFF)

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

find_package(TBB)

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

if (TBB_FOUND)
    message(STATUS "TBB version: ${TBB_VERSION}")
    target_link_libraries(cpptoy PRIVATE TBB::tbb)
else()
    message(WARNING "TBB not found")
endif()
