//
// Created by fedor on 8/5/20.
//

#ifndef CPPCLIENT_ZMQFROZENTRANSPORTSERVER_HPP
#define CPPCLIENT_ZMQFROZENTRANSPORTSERVER_HPP

#include <exception>
#include <unordered_map>
#include <zmqpp/context.hpp>
#include <zmqpp/zmqpp.hpp>
#include <zmqpp/socket_types.hpp>
#include <zmqpp/reactor.hpp>
#include <zmqpp/message.hpp>
#include <spdlog/spdlog.h>
#include <thread>
#include <atomic>
#include "Communicator/TransportServer.hpp"
#include "Communicator/Endpoint.hpp"
#include "TransportServerReply.hpp"


namespace communicator { namespace zmq {
inline Payload fromZmqMessage(const zmqpp::message& msg, size_t part) {
    std::string str = msg.get(part);
    return {str.begin(), str.end()};
}

/// ZMQ based FrozenTransportServer implementation
class ZMQFrozenTransportServer: public FrozenTransportServer {
public:
    explicit ZMQFrozenTransportServer(const Endpoint& endpoint, const zmqpp::context& context)
    : context(context), worker_endpoint("inproc://" + endpoint.address) {
        spdlog::info("Created ZMQFRozenTransportServer on endpoint {}", endpoint.asString());
        _worker_thread = std::thread([this, endpoint]() {
            zmqpp::socket worker_socket(
                    communicator::zmq::ZMQFrozenTransportServer::context,
                    zmqpp::socket_type::pull);
            worker_socket.bind(worker_endpoint);

            zmqpp::message empty;
            worker_socket.receive(empty, false);

            bool alive = true;
            zmqpp::socket socket(
                    communicator::zmq::ZMQFrozenTransportServer::context,
                    zmqpp::socket_type::router);
            socket.bind(endpoint.asString());

            zmqpp::reactor reactor;
            reactor.add(socket, [this, &socket](){
                communicator::zmq::ZMQFrozenTransportServer::answer(socket, this->functions, this->coders_ids);
            });
            reactor.add(worker_socket, [&alive]() { alive = false; });

            while(alive) {
                reactor.poll();
            }
        });
    }

    void reg(
            const string& name,
            const PayloadFunction& function,
            const Identity& args_coder_id,
            const Identity& res_coder_id) override {
        if (!stopped.load()) {
            throw MutateAfterStart();
        }
        coders_ids[name] = {args_coder_id, res_coder_id};
        functions[name] = function;
    }

    void unreg(const string& name) override {
        if (!stopped.load()) {
            throw MutateAfterStart();
        }
        functions.erase(name);
        coders_ids.erase(name);
    }

    void stop() override {
        if (stopped.exchange(true)) return;
        zmqpp::message msg;
        msg.add("die");
        zmqpp::socket worker_death = zmqpp::socket(context, zmqpp::socket_type::push);
        worker_death.connect(worker_endpoint);
        worker_death.send(msg, true);
    }

    void start() override {
        stopped.store(false);
        zmqpp::message msg;
        msg.add("die");
        zmqpp::socket worker_start = zmqpp::socket(context, zmqpp::socket_type::push);
        worker_start.connect(worker_endpoint);
        worker_start.send(msg, true);
    }

    ~ZMQFrozenTransportServer() {
        stop();
        _worker_thread.join();
    }
private:
    struct FunctionCodersIdentity {
        Identity arguments_coder;
        Identity result_coder;
    };

    std::thread _worker_thread;

    const zmqpp::context& context;
    const std::string worker_endpoint;
    std::atomic<bool> stopped = {true};

    std::unordered_map<string, PayloadFunction> functions;
    std::unordered_map<string, FunctionCodersIdentity> coders_ids;

    static void answer(
            zmqpp::socket& socket,
            std::unordered_map<string, PayloadFunction>& functions,
            std::unordered_map<string, FunctionCodersIdentity>& coder_ids) {
        zmqpp::message msg;
        socket.receive(msg);
        auto identity = msg.get(0);
        std::string command = msg.get(1);
        spdlog::info("ZMQFTS got request. command={}", command);
        constexpr size_t current_part = 2;
        try {
            zmqpp::message reply = processCommand<current_part>(command, msg, functions, coder_ids);
            socket.send(reply, true);
        } catch(IncorrectMessageFormat& e) {
            spdlog::warn("ZMQFTS: Can't answer");
        }
    }

    template <size_t current_part>
    static zmqpp::message processCommand(
            const std::string& command,
            zmqpp::message& msg,
            std::unordered_map<string, PayloadFunction>& functions,
            std::unordered_map<string, FunctionCodersIdentity>& coder_ids) {
        zmqpp::message reply;
        reply << msg.get(0);
        if (command == u8"QUERY") {
            std::string request_id;
            std::string str_args;
            std::string function_name;
            msg.extract<current_part>(request_id, str_args, function_name);
            spdlog::debug("function name = {}; request id = {}", function_name, request_id);
            auto fun_iter = functions.find(function_name);
            if (fun_iter == functions.end()) {
                spdlog::warn("function {} is not supported", function_name);
                FunctionNotSupported(request_id) >> reply;
                return reply;
            }
            auto fun = fun_iter->second;
            Payload bin_args = {str_args.begin(), str_args.end()};
            try {
                auto result = fun(bin_args);
                SuccessReply(request_id, result) >> reply;
            } catch (std::exception& e) {
                spdlog::warn("caught exception during function execution. what():\n", e.what());
                FunctionExceptionReply(request_id, e.what()) >> reply;
            }
            return reply;
        } else if(command == u8"CODER_IDENTITY_QUERY") {
            std::string request_id;
            std::string function_name;
            msg.extract<current_part>(request_id, function_name);
            auto ids_iter = coder_ids.find(function_name);
            if (ids_iter == coder_ids.end()) {
                spdlog::warn("function {} is not supported", function_name);
                FunctionNotSupported(request_id) >> reply;
                return reply;
            }
            FunctionSupported(request_id, ids_iter->second.arguments_coder, ids_iter->second.result_coder) >> reply;
            return reply;
        } else if (command == u8"RESPONSE_RECEIVED") {
            spdlog::warn("ZMQFTS got unsupported command: {}", command);
            return reply;
        } else {
            spdlog::error("ZMQFTS got invalid command: {}", command);
            throw IncorrectMessageFormat(fmt::format("Invalid command: \"{}\"", command));
        }
    }
};
}}


#endif //CPPCLIENT_ZMQFROZENTRANSPORTSERVER_HPP
