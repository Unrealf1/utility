//
// Created by fedor on 8/24/20.
//

#include "test_main.hpp"
#include "Communicator/ZMQTransport/ZMQTransportManager.hpp"
#include "Communicator/ZMQTransport/ZMQFrozenTransportServer.hpp"
#include "Communicator/ZMQTransport/ZMQTransport.hpp"
#include "Communicator/LibraryFunction.hpp"
#include "Communicator/Endpoint.hpp"
#include <zmqpp/context.hpp>

auto f1 = [](int32_t a) {
    return std::to_string(a);
};

// auto f5 = [](int32_t a1, std::function<int32_t(int32_t)> a2) {
//     return a2(a1);
// };

TEST(ZMQTransport, ImmutabilityOfTransportServer) {
    const zmqpp::context& ctx = context_wrapper::context;
    communicator::Endpoint ep("tcp", "127.0.0.1:7777");
    communicator::zmq::ZMQFrozenTransportServer tServer(ep, ctx);
    auto lf1 = communicator::make_library_function("f1", std::function<std::string (int32_t)>(f1));
    tServer.reg(lf1.name, lf1.asPayloadFunction(), lf1.getArgsCoderIdentity(), lf1.getResCoderIdentity());
    tServer.unreg(lf1.name);
    tServer.reg(lf1.name, lf1.asPayloadFunction(), lf1.getArgsCoderIdentity(), lf1.getResCoderIdentity());
    tServer.start();
    ASSERT_THROW(tServer.unreg(lf1.name), communicator::MutateAfterStart);
    tServer.stop();
}

TEST(ZMQTransport, ZMQTransportManager) {
    const zmqpp::context& ctx = context_wrapper::context;
    communicator::zmq::ZMQTransportManager manager(ctx);
    auto transport1 = manager.create();
}