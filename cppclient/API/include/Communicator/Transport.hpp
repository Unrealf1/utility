//
// Created by fedor on 8/3/20.
//

#ifndef CPPCLIENT_TRANSPORT_HPP
#define CPPCLIENT_TRANSPORT_HPP

#include <functional>
#include "Communicator/Coder.hpp"
#include "Communicator/Endpoint.hpp"

namespace communicator {

using PayloadFunction = std::function<Payload (Payload)>;

class RemoteFunctionException: public std::exception {
public:
    explicit RemoteFunctionException(std::string reason): reason(std::move(reason)){}

    const char* what() const noexcept override {
        return reason.c_str();
    }
private:
    std::string reason;
};

class UnknownRemoteFunction: public std::exception {
public:
    explicit UnknownRemoteFunction(std::string reason): reason(std::move(reason)){}

    const char* what() const noexcept override {
        return reason.c_str();
    }
private:
    std::string reason;
};

class CoderIdentityNotFound: public std::exception {
    const char* what() const noexcept override {
        return "Function is not supported";
    }
};

class UnexpectedReplyType: public std::exception {
public:
    explicit UnexpectedReplyType(const std::string& type) {
        reason = "Reply type " + type + " is not supported";
    }

    const char* what() const noexcept override {
        return reason.c_str();
    }
private:
    std::string reason;
};



/// ZmqTransportManager worker Will send one time requests to compute a function to a functional server located on a given endpoint
class Transport {
public:
    /**
     * Perform a one time request
     * @param endpoint  remote server address
     * @param name      remote function name
     * @param payload   encoded function arguments
     */
    virtual Payload respond(const Endpoint& endpoint, const std::string& name, const Payload& payload) = 0;
    /**
     * Shortcut to conveniently use one transport with the same function multiple times
     * @param endpoint  remote server address
     * @param name      remote function name
     */
    virtual PayloadFunction channel(const Endpoint& endpoint, const std::string& name) {
        return [this, endpoint, name](const Payload& payload) {
            return respond(endpoint, name, payload);
        };
    }
    virtual ~Transport() = default;
};

/// Class to \a manage, create and destroy Transport instances
class TransportManager {
    public:
        /**
         * Creates a new transport instance, that should be later destroyed via
         * TransportManager.destroy method of the respected transport manager
         * 
         * This method _is_ thread safe
         */ 
        virtual Transport* createTransport() = 0;
        /**
         * Destroys a transport instance
         * 
         * This method _is_ thread safe
         */ 
        virtual void destroyTransport(Transport*) = 0;
        virtual ~TransportManager() = default;
};

}



#endif //CPPCLIENT_TRANSPORT_HPP
