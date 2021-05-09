//
// Created by fedor on 8/16/20.
//

#ifndef CPPCLIENT_TRANSPORTSERVERREPLY_HPP
#define CPPCLIENT_TRANSPORTSERVERREPLY_HPP

#include <string>
#include <zmqpp/message.hpp>
#include <Communicator/Coder.hpp>


namespace communicator { namespace zmq {

/// Conveniently constructs different replies
class Reply {
public:
    virtual zmqpp::message& operator>>(zmqpp::message& reply) = 0;
    virtual ~Reply() = default;
};

class SuccessReply: public Reply {
public:
    SuccessReply(const std::string& id, const Payload& result): id(id), result(result) {}
    zmqpp::message& operator>>(zmqpp::message& msg) override {
        return msg << type << id << std::string(result.begin(), result.end());
    }
private:
    const std::string type = u8"RESPONSE_RESULT";
    const std::string& id;
    const Payload& result;
};

class FunctionExceptionReply: public Reply {
public:
    FunctionExceptionReply(const std::string& id, const std::string& what): id(id), what(what) {}
    zmqpp::message& operator>>(zmqpp::message& msg) override {
        return msg << type << id << what;
    }
private:
    const std::string type = u8"RESPONSE_EXCEPTION";
    const std::string& id;
    const std::string& what;
};

class UnknownFunction: public Reply {
public:
    UnknownFunction(const std::string& id, const Identity& argument_coder, const Identity& result_coder):
            id(id), argument_coder(argument_coder), result_coder(result_coder) {}
    zmqpp::message& operator>>(zmqpp::message& msg) override {
        return msg << type << id
                   << std::string(argument_coder.begin(), argument_coder.end())
                   << std::string(result_coder.begin(), result_coder.end());
    }
private:
    const std::string type = "RESPONSE_UNKNOWN_FUNCTION";
    const std::string& id;
    const Identity& argument_coder;
    const Identity& result_coder;
};

class FunctionSupported: public Reply {
public:
    FunctionSupported(const std::string& id, const Identity& argument_coder, const Identity& result_coder):
            id(id), argument_coder(argument_coder), result_coder(result_coder) {}
    zmqpp::message& operator>>(zmqpp::message& msg) override {
        return msg << type << id
        << std::string(argument_coder.begin(), argument_coder.end())
        << std::string(result_coder.begin(), result_coder.end());
    }
private:
    const std::string type = u8"CODER_IDENTITY_FOUND";
    const std::string& id;
    const Identity& argument_coder;
    const Identity& result_coder;
};

class FunctionNotSupported: public Reply {
public:
    explicit FunctionNotSupported(const std::string& id): id(id) {}
    zmqpp::message& operator>>(zmqpp::message& msg) override {
        return msg << type << id;
    }
private:
    const std::string type = u8"CODER_IDENTITY_NOT_FOUND";
    const std::string& id;
};

class QueryReceived: public Reply {
public:
    explicit QueryReceived(const std::string& id): id(id) {}
    zmqpp::message& operator>>(zmqpp::message& msg) override {
        return msg << type << id;
    }
private:
    const std::string type = u8"QUERY_RECEIVED";
    const std::string& id;
};

}}

#endif //CPPCLIENT_TRANSPORTSERVERREPLY_HPP
