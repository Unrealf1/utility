cmake_minimum_required(VERSION 3.5)

project(cppclient-example-client)

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_executable(ExampleClient ${SOURCES})
target_include_directories(ExampleClient PUBLIC "./include")
target_link_libraries(ExampleClient PUBLIC CommunicatorApi ${ZMQPP} ${ZMQ})
target_link_libraries(ExampleClient PRIVATE project_options project_warnings)
