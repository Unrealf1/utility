//
// Created by fedor on 8/5/20.
//

#ifndef CPPCLIENT_TRANSPORTSERVER_HPP
#define CPPCLIENT_TRANSPORTSERVER_HPP

#include "Communicator/Transport.hpp"
#include <string>
#include <utility>

namespace communicator {

using std::string;

class IncorrectMessageFormat: public std::exception {
public:
    explicit IncorrectMessageFormat(string reason): reason(std::move(reason)) {}

    const char* what() const noexcept override {
        return reason.c_str();
    }
private:
    std::string reason;
};

///  Will wait for the requests from clients, compute functions and send answers
class TransportServer {
public:
    /// Register a function
    virtual void reg(
            const string& name /**< [in] name that will be used to find the function */,
            const PayloadFunction& function /**< [in] */,
            const Identity& args_coder_id /**< [in] */,
            const Identity& res_coder_id /**< [in] */) = 0;

    /// Remove a function from this transport server
    virtual void unreg (const string& name) = 0;
    virtual void stop() = 0;
    virtual ~TransportServer() = default;
};


class MutateAfterStart: std::exception {
    const char* what() const noexcept override {
        return "Attempt to modify ZMQFrozenTransportServer after start";
    }
};

/// Type of TransportServer that cannot register or unregister functions after 'start' was called
class FrozenTransportServer: public TransportServer {
public:
    virtual void start() = 0;
    virtual ~FrozenTransportServer() = default;
};

}



#endif //CPPCLIENT_TRANSPORTSERVER_HPP
