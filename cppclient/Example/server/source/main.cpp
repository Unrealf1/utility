#include "Communicator/Communicator.hpp"

#include <string>
#include <iostream>

int32_t secret_function(std::vector<int32_t> vec) {
    return vec[0] + vec[1];
}

using communicator::make_contractor;
using communicator::make_library_function;
using communicator::Endpoint;

int main() {
    auto ts_ptr = communicator::createDefaultTransportServer(Endpoint("tcp", "127.0.0.1:7777"));
    auto secret_lf = make_library_function(
            "secret function",
            std::function<int32_t (std::vector<int32_t>)>(secret_function)
    );
    auto contractor = make_contractor(*ts_ptr, secret_lf);
    contractor.Start();
    std::cin.ignore();
    delete ts_ptr;
}
