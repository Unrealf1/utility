#include "Communicator/Communicator.hpp"
#include <spdlog/spdlog.h>

#include <string>
#include <iostream>

int32_t actual_secret_function(std::vector<int32_t> vec) {
    return vec[0] + vec[1];
}

using communicator::Endpoint;
using communicator::Payload;
using communicator::RemoteFunction;

int main() {
    spdlog::set_level(spdlog::level::debug);

    auto tm = communicator::createDefaultTransportManager();
    auto transport = tm->createTransport();
    auto rf = RemoteFunction<int32_t (std::vector<int32_t>)>(Endpoint("tcp", "127.0.0.1:7777"), "secret function", *transport);
    std::vector<int32_t> args = {1, 5};
    auto res = rf(args);
    if (res != actual_secret_function(args)) {
        throw std::exception();
    }

    tm->destroyTransport(transport);
    delete tm;
}
