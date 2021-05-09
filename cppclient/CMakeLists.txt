cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(communicator-cpp)

option(TESTS "Build tests" ON)
option(EXAMPLE "Build example" OFF)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

set_project_warnings(project_warnings)

include(cmake/Doxygen.cmake)

if (TESTS)
    set(CONAN_EXTRA_REQUIRES "gtest/1.10.0" "spdlog/1.8.5")
else()
    set(CONAN_EXTRA_REQUIRES "spdlog/1.8.5")
endif()


include(cmake/Conan.cmake)
run_conan()

if (NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

add_subdirectory("API")

if (TESTS)
    add_subdirectory("./Test")
endif(TESTS)

if (EXAMPLE)
    add_subdirectory("./Example")
endif(EXAMPLE)
