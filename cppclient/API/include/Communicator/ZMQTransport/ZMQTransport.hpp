//
// Created by fedor on 8/3/20.
//

#ifndef CPPCLIENT_ZMQ_TRANSPORT_HPP
#define CPPCLIENT_ZMQ_TRANSPORT_HPP

#include "Communicator/Transport.hpp"
#include <zmqpp/context.hpp>
#include <zmqpp/socket.hpp>
#include <zmqpp/socket_types.hpp>
#include <zmqpp/message.hpp>
#include <spdlog/spdlog.h>

namespace communicator { namespace zmq {

/// ZMQ based transport implementation. Instances should be created via ZMQTransportManager.createTransport
class ZMQTransport: public Transport {
    friend class ZMQTransportManager;
private:
    explicit ZMQTransport(
            const std::string& register_endpoint,
            const std::string& callback_endpoint,
            const zmqpp::context& context)
            :manager_socket(context, zmqpp::socket_type::pair) {
        spdlog::info("ZMQTransport is created with callback address \"{}\"", callback_endpoint);
        manager_socket.bind(callback_endpoint);
        zmqpp::socket register_socket(context, zmqpp::socket_type::push);
        register_socket.connect(register_endpoint);
        zmqpp::message reg_request;
        reg_request << callback_endpoint;
        register_socket.send(reg_request);
    }
public:

    Payload respond(const Endpoint& endpoint, const std::string& name, const Payload& payload) override {
        zmqpp::message request(endpoint.asString(), name, std::string(payload.begin(), payload.end()));
        spdlog::debug("ZMQTransport: requesting forwarding from manager");
        manager_socket.send(request, false);
        zmqpp::message reply;
        spdlog::debug("ZMQTransport: awaiting manager reply");
        manager_socket.receive(reply, false);
        spdlog::debug("ZMQTransport: got reply from manager");
        std::string type;
        std::string id;
        reply.extract(type, id);
        constexpr size_t current_part = 2;
        if (type == "RESPONSE_RESULT") {
            std::string result;
            reply.extract<current_part>(result);
            return {result.begin(), result.end()};
        } else if (type == "RESPONSE_EXCEPTION") {
            std::string what;
            reply.extract<current_part>(what);
            spdlog::warn("ZMQT got exception in response. what():\n{}", what);
            throw RemoteFunctionException(what);
        } else if (type == "RESPONSE_UNKNOWN_FUNCTION") {
            std::string functionName;
            reply.extract<current_part>(functionName);
            spdlog::warn("ZMQT got \"unknown function response\" for {}", functionName);
            throw UnknownRemoteFunction(functionName);
        } else if (type == "CODER_IDENTITY_FOUND") {
            std::string argumentCoderIdentity;
            std::string resultCoderIdentity;
            reply.extract<current_part>(argumentCoderIdentity, resultCoderIdentity);
            std::string result = argumentCoderIdentity + resultCoderIdentity;
            return {result.begin(), result.end()};
        } else if (type == "CODER_IDENTITY_NOT_FOUND") {
            spdlog::warn("ZMQT: coder identity not found");
            throw CoderIdentityNotFound();
        } else if (type == "QUERY_RECEIVED") {
            spdlog::warn("ZMQT: type {} is not supported yet", type);
            return Payload();
        } else {
            spdlog::error("ZMQT: type {} is not supported", type);
            throw UnexpectedReplyType(type);
        }
    }
private:
    zmqpp::socket manager_socket;
};
}}

#endif //CPPCLIENT_ZMQ_TRANSPORT_HPP
