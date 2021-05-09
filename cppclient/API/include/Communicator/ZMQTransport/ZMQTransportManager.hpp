//
// Created by fedor on 8/17/20.
//

#ifndef CPPCLIENT_ZMQTRANSPORTMANAGER_HPP
#define CPPCLIENT_ZMQTRANSPORTMANAGER_HPP

#include "ZMQTransport.hpp"
#include "ZMQTMWorker.hpp"
#include <atomic>
#include <unordered_map>
#include <zmqpp/socket.hpp>
#include <zmqpp/socket_types.hpp>
#include <zmqpp/reactor.hpp>
#include <thread>
#include <zmqpp/message.hpp>
#include <list>

namespace communicator { namespace zmq {

// For now new functionServerSockets and transport sockets are just created and never deleted. This might be a problem.
// Possible solution: "unregisterFunctionServer" function here that will be called from
// ~RemoteFunction(simple reference count) and "unregister" rquest from transport on destruction. Another variant - just use shared_ptr.

/// ZMQ based implementation of TransportManager
/**
 * Generates ZMQTransport instances Thread safe(but transports are NOT thread safe)
 * 
 * There should only be one instance of ZMQTransportManager per context
 * 
 * Here is an illustration of what is happening inside:
 * 
 * ![ZMQTransportManager](../../../API/doc/img/Transport.png) 
 */
class ZMQTransportManager : public TransportManager {
public:
    explicit ZMQTransportManager(const zmqpp::context& context): context(context) {
        spdlog::info("ZMQTransportManager is created");
        _worker_thread = std::thread([&context] {
            auto worker = ZMQTMWorker(context);
            worker.start(register_new_transport_endpoint, death_endpoint);
        });
    }

    ZMQTransport create() {
        std::string current_id = std::to_string(getNextTransportId());
        return ZMQTransport(
                register_new_transport_endpoint,
                base_transport_messages_endpoint + current_id,
                context);
    }

    ZMQTransport* createTransport() {
        std::string current_id = std::to_string(getNextTransportId());
        return new ZMQTransport(
                register_new_transport_endpoint,
                base_transport_messages_endpoint + current_id,
                context);
    }

    void destroyTransport(Transport* transport) {
        delete transport;
    }

    ~ZMQTransportManager() {
        zmqpp::socket worker_death(context, zmqpp::socket_type::request);
        worker_death.connect(death_endpoint);
        zmqpp::message msg("die");
        spdlog::debug("requesting death of ZMQTMWorker");
        worker_death.send(msg, false);
        worker_death.receive(msg, false);
        _worker_thread.join();
        spdlog::info("ZMQTransportManager destroyed");
    }
private:
    const zmqpp::context& context;
    std::thread _worker_thread;

    static std::atomic<size_t> next_transport_id;
    static const std::string death_endpoint;
    static const std::string register_new_transport_endpoint;
    static const std::string register_new_fserver_endpoint;
    static const std::string base_transport_messages_endpoint;

    inline static size_t getNextTransportId() {
        return ++next_transport_id;
    }
};

}}

#endif //CPPCLIENT_ZMQTRANSPORTMANAGER_HPP
