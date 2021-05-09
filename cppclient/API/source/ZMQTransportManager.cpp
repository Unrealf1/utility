//
// Created by fedor on 8/23/20.
//

#include "Communicator/ZMQTransport/ZMQTransportManager.hpp"

namespace communicator {namespace zmq {
    std::atomic<size_t> ZMQTransportManager::next_transport_id = {0};

    const std::string ZMQTransportManager::death_endpoint = Endpoint(
            "inproc", "ZMQTransportManager/death").asString();

    const std::string ZMQTransportManager::register_new_transport_endpoint = Endpoint(
            "inproc", "ZMQTransportManager/transport_reg").asString();

    const std::string ZMQTransportManager::register_new_fserver_endpoint = Endpoint(
            "inproc", "ZMQTransportManager/register").asString();

    const std::string ZMQTransportManager::base_transport_messages_endpoint = Endpoint(
            "inproc", "ZMQTransportManager/transport/").asString();
}}