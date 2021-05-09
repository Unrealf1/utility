cmake_minimum_required(VERSION 3.5)

project(cppclient-Test)

enable_testing()

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_executable(Test ${SOURCES})

find_library(ZMQPP NAMES zmqpp)
find_library(ZMQ NAMES zmq)

target_include_directories(Test PUBLIC "./include")

target_link_libraries(Test PUBLIC project_options project_warnings CommunicatorApi CONAN_PKG::gtest ${ZMQPP} ${ZMQ})

add_test(CommonTestsAll Test)
