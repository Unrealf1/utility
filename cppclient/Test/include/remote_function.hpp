//
// Created by fedor on 8/23/20.
//

#ifndef CPPCLIENT_REMOTE_FUNCTION_HPP
#define CPPCLIENT_REMOTE_FUNCTION_HPP

#include "Communicator/Transport.hpp"

class FakeTransport: public Transport {
    virtual Payload respond(const Endpoint& endpoint, const std::string& name, const Payload& payload) override {
        (void)endpoint;
        auto f1 = [](int32_t a) {
            return std::to_string(a);
        };

        auto f2 = [](std::string a1, int32_t a2) {
            return (a1 == std::to_string(a2)) ? 1 : 0;
        };

        auto lf1 = make_library_function("f1", std::function<std::string (int32_t)>(f1));
        auto lf2 = make_library_function("f2", std::function<int32_t (std::string, int32_t)>(f2));

        if (name == "f1") {
            return lf1.asPayloadFunction(payload);
        } else {
            return lf2.asPayloadFunction(payload);
        }
    }
};

#endif //CPPCLIENT_REMOTE_FUNCTION_HPP
