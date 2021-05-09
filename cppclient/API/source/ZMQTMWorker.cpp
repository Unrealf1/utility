#include "Communicator/ZMQTransport/ZMQTMWorker.hpp"
#include <spdlog/spdlog.h>


namespace communicator { namespace zmq {

ZMQTMWorker::ZMQTMWorker(const zmqpp::context& context) 
: context(context), _transport_inserter(context, zmqpp::socket_type::pull), _death(context, zmqpp::socket_type::reply), _alive(true) {

}

ZMQTMWorker::~ZMQTMWorker() {
    for (auto& pr : _endpoint_to_socket) {
        _poller.remove(*(pr.second));
        delete pr.second;
    }
    for (auto& pr : _transport_sockets) {
        _poller.remove(*(pr.second));
        delete pr.second;
    }

    spdlog::debug("ZMQTMWorker destroyed");
}

void ZMQTMWorker::start(
    std::string register_new_transport_endpoint,
    std::string death_endpoint
) {
    _transport_inserter.bind(register_new_transport_endpoint);
    _poller.add(_transport_inserter);
    _death.bind(death_endpoint);
    _poller.add(_death);

    while(_alive) {
        if (_poller.poll()) {
            onPoll();
        } else {
            spdlog::error("ZMQTMWorker: poller has stopped without new events. Stopping worker");
            _alive = false;
        }
    }
    spdlog::info("ZMQTM worker finished!");
}

void ZMQTMWorker::onPoll() {
    if (_poller.has_input(_death)) {
        zmqpp::message msg;
        _death.receive(msg);
        _alive = false;
        _death.send(msg, false);
        spdlog::debug("ZMQTMWorker got death signal");
    } 
    if (_poller.has_input(_transport_inserter)) {
        onInsert();        
    }
    for (auto& pr : _transport_sockets) {
        if (_poller.has_input(*(pr.second))) {
            onForward(pr.first);
        }
    }
    for (auto& pr : _endpoint_to_socket) {
        if (_poller.has_input(*(pr.second))) {
            onAnswer(pr.first);
        }
    }
}

void ZMQTMWorker::onInsert() {
    zmqpp::message insert_request;
    _transport_inserter.receive(insert_request);
    std::string callback_address = insert_request.get(0);
    spdlog::info("ZMQTMWorker got request to register transport on address {}", callback_address);

    auto [item_iter, inserted] = _transport_sockets.emplace(callback_address, new zmqpp::socket(context, zmqpp::socket_type::pair));
        if (!inserted) {
            spdlog::warn("Transport callback address was already taken!");
            return;
        }

    item_iter->second->connect(callback_address);
    _poller.add(*(item_iter->second));
}

void ZMQTMWorker::onForward(const std::string& callback_address) {
    spdlog::info("Forward request from transport at address {}", callback_address);
    auto& listener = *_transport_sockets.at(callback_address);
    zmqpp::message forwardRequest;
    listener.receive(forwardRequest);
    std::string endpoint;
    std::string name;
    std::string payload;
    spdlog::info("ZMQTMWorker got forward request to endpoint {} (function name is '{}')", endpoint, name);
    forwardRequest.extract(endpoint, name, payload);
    auto [item_iter, inserted] = _endpoint_to_socket.emplace(endpoint, new zmqpp::socket(context, zmqpp::socket_type::dealer));
    zmqpp::socket& fServer = *(item_iter->second);
    //if this is our first connection to this server
    if (inserted) {
        spdlog::info("Connecting to remote functional server with endpoint '{}'", endpoint);
        fServer.connect(endpoint);
        _poller.add(fServer);
    }
    zmqpp::message request;
    auto req_id = std::to_string(getNextRequestId());
    request << u8"QUERY" << req_id << payload << name;
    _reqIdToTransport.emplace(req_id, callback_address);
    fServer.send(request, true);
    spdlog::debug("request sent");
}

void ZMQTMWorker::onAnswer(const std::string& endpoint) {
    auto& fServer = *(_endpoint_to_socket.at(endpoint));
    spdlog::info("Got answer from functional server at endpoint '{}'", endpoint);
    zmqpp::message answer;
    fServer.receive(answer);
    dispatchAnswer(answer);
}

void ZMQTMWorker::dispatchAnswer(
    zmqpp::message& answer
) {
    if (answer.parts() < 2) {
        // incorrect format
        spdlog::warn("Incorrect message format: message size is {}", answer.parts());
        return;
    }
    auto id = answer.get(1);
    spdlog::info("Dispatching answer, id={}", id);
    auto found = _reqIdToTransport.find(id);
    if (found == _reqIdToTransport.end()) {
        spdlog::warn("Unexpected response with id {}", id);
        return;
    }
    auto& transport_addr = found->second;
    auto& transport = *_transport_sockets.at(transport_addr);
    transport.send(answer, true);
    _reqIdToTransport.erase(found);
}

}}