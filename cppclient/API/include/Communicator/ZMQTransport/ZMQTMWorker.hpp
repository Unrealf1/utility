#pragma once

#include <unordered_map>
#include <map>
#include <zmqpp/socket.hpp>
#include <zmqpp/socket_types.hpp>
#include <zmqpp/poller.hpp>
#include <zmqpp/message.hpp>
#include <functional>


namespace communicator { namespace zmq {

///  ZmqTransportManager worker
/**
 * Will block thread on `start` call, listen for new transports to register,
 * forward their requests to remote function servers and dispatch answers back to transports
*/
class ZMQTMWorker
{
public:

    /**
     * @param [in] context context should not die before ZMQTMWorker
     */ 
    ZMQTMWorker(const zmqpp::context& context);
    ~ZMQTMWorker();

    /**
     * Start worker and block the thread
     * @param register_new_transport_endpoint zmq address of \a _transport_inserter socket
     * @param death_endpoint zmq address of \a _death socket
     */
    void start(std::string register_new_transport_endpoint, std::string death_endpoint); 
private:
    const zmqpp::context& context;

    std::map<std::string, zmqpp::socket*> _endpoint_to_socket; ///< remote fServer addresses to sockets
    std::map<std::string, zmqpp::socket*> _transport_sockets; ///< transport callback adresses to sockets
    std::unordered_map<std::string, std::string> _reqIdToTransport; ///< request id to transport callback address
    
    zmqpp::poller _poller;
    zmqpp::socket _transport_inserter; ///< Socket to register new transports
    zmqpp::socket _death; ///< Socket to receive stop signal

    bool _alive;

    void dispatchAnswer(zmqpp::message& answer); 

    inline size_t getNextRequestId() {
        return ++next_request_id;
    }

    void onPoll();
    void onInsert();
    void onForward(const std::string& callback_address);
    void onAnswer(const std::string& endpoint);

    size_t next_request_id = 0;
};

}}
