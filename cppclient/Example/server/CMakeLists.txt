cmake_minimum_required(VERSION 3.5)

project(cppclient-example-server)

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_executable(ExampleContractor ${SOURCES})
target_include_directories(ExampleContractor PUBLIC "./include")
target_link_libraries(ExampleContractor PUBLIC CommunicatorApi ${ZMQPP} ${ZMQ})
target_link_libraries(ExampleContractor PRIVATE project_options project_warnings)
