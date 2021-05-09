cmake_minimum_required(VERSION 3.5)

project(communicator-cpp)

find_library(ZMQPP NAMES zmqpp)
find_library(ZMQ NAMES zmq)

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_library(CommunicatorApi SHARED ${SOURCES})

target_link_libraries(CommunicatorApi PUBLIC ${ZMQPP} ${ZMQ} CONAN_PKG::spdlog)
target_link_libraries(CommunicatorApi PRIVATE project_options project_warnings)

target_include_directories(CommunicatorApi PUBLIC "./include")

file(GLOB_RECURSE CODERS_HEADERS . include/Communicator/BasicBinaryCoders/*.hpp)

set(CommunicatorApiPublicHeaders_list "include/Communicator/Communicator.hpp" 
"include/Communicator/Transport.hpp" 
"include/Communicator/TransportServer.hpp" 
"include/Communicator/Endpoint.hpp" 
"include/Communicator/Coder.hpp"
"include/Communicator/LibraryFunction.hpp"
"include/Communicator/Contractor.hpp"
"include/Communicator/BasicCoders.hpp"
"include/Communicator/Helper.hpp"
"include/Communicator/RemoteFunction.hpp"
)

set_target_properties(
    CommunicatorApi
    PROPERTIES
    PUBLIC_HEADER "${CommunicatorApiPublicHeaders_list}"
    SOVERSION 0
    VERSION 0.2.0
)

include(GNUInstallDirs)

install(TARGETS CommunicatorApi
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT   CommunicatorApi_Runtime
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT   CommunicatorApi_Runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT   CommunicatorApi_Development
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/Communicator"
            COMPONENT CommunicatorApi_Development
)

install(FILES  ${CODERS_HEADERS}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/Communicator/BasicBinaryCoders"
)
enable_doxygen()
