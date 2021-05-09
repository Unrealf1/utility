//
// Created by fedor on 8/11/20.
//

#include "test_main.hpp"
#include "userapi_tests.hpp"
#include "Communicator/Contractor.hpp"
#include "Communicator/ZMQTransport/ZMQFrozenTransportServer.hpp"
#include "Communicator/ZMQTransport/ZMQTransportManager.hpp"
#include "Communicator/RemoteFunction.hpp"
#include <string>
#include <functional>
#include <zmqpp/socket.hpp>
#include <zmqpp/socket_types.hpp>
#include <zmqpp/message.hpp>
#include <zmqpp/context.hpp>
#include <thread>
#include <atomic>


using std::string;
using communicator::Contractor;
using communicator::LibraryFunction;
using communicator::Endpoint;
using communicator::make_library_function;
using communicator::make_contractor;
using communicator::Payload;

std::string f1(int32_t a) {
    return std::to_string(a);
}

auto f2 = [](std::string a1, int32_t a2) {
    return (a1 == std::to_string(a2)) ? 1 : 0;
};

auto f3 = [](int32_t a1, int32_t a2, int32_t a3, int32_t a4, int32_t a5) {
    return (a1 + a2 == a3 + a4 + a5) ? 1 : 0;
};

auto f4 = [](int32_t a1, std::vector<int32_t> a2) {
    return a1 == a2[0] ? 1 : 0;
};

auto f5 = [](int32_t a1, std::function<int32_t(int32_t)> a2) {
    return a2(a1);
};


TEST(ZMQContractor, Success) {
    const zmqpp::context& ctx = context_wrapper::context;
    Endpoint ep("tcp", "127.0.0.1:7777");
    communicator::zmq::ZMQFrozenTransportServer ts(ep, ctx);
    auto lf1 = make_library_function("f1", std::function<std::string (int32_t)>(f1));
    auto lf2 = make_library_function("f2", std::function<int32_t (std::string, int32_t)>(f2));
    auto lf3 = make_library_function("f3", std::function<int32_t (int32_t, int32_t, int32_t, int32_t, int32_t)>(f3));
    auto lf4 = make_library_function("f4", std::function<int32_t (int32_t, std::vector<int32_t>)>(f4));
    auto contractor = make_contractor(ts, lf1, lf2, lf3, lf4);
    contractor.Start();
    std::atomic_bool finished;
    finished.store(false);
    auto customer = [ep, &finished]() {
        const zmqpp::context& ctx = context_wrapper::context;
        zmqpp::socket socket(ctx, zmqpp::socket_type::dealer);
        socket.connect(ep.asString());
        zmqpp::message msg;
        std::string command = u8"QUERY";
        std::string request_id = "123";
        int32_t argument = 17;
        auto binaryArgument = BinaryCoder<int32_t>().encode(argument);
        std::string function_name = "f1";
        msg << command << request_id << std::string(binaryArgument.begin(), binaryArgument.end()) << function_name;
        socket.send(msg, false);
        std::cout << "CUSTOMER: sent" << std::endl;
        zmqpp::message reply;
        socket.receive(reply, false);
        std::cout << "CUSTOMER: got message with " << reply.parts() << " parts" << std::endl;
        auto reply_status = reply.get(0);
        ASSERT_EQ(u8"RESPONSE_RESULT", reply_status);
        auto reply_id = reply.get(1);
        ASSERT_EQ(reply_id, request_id);
        auto str_res = reply.get(2);
        Payload reply_res = {str_res.begin(), str_res.end()};
        ASSERT_EQ(f1(argument), BinaryCoder<std::string>().decode(reply_res));
        finished.store(true);
    };

    std::thread t(customer);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    ASSERT_TRUE(finished.load());
    t.join();
}

TEST(ZMQContractor, UnknownFunction) {
    const zmqpp::context& ctx = context_wrapper::context;
    Endpoint ep("tcp", "127.0.0.1:7777");
    communicator::zmq::ZMQFrozenTransportServer ts(ep, ctx);
    auto lf1 = make_library_function("f1", std::function<std::string (int32_t)>(f1));
    auto lf2 = make_library_function("f2", std::function<int32_t (std::string, int32_t)>(f2));
    auto lf3 = make_library_function("f3", std::function<int32_t (int32_t, int32_t, int32_t, int32_t, int32_t)>(f3));
    auto lf4 = make_library_function("f4", std::function<int32_t (int32_t, std::vector<int32_t>)>(f4));
    auto contractor = make_contractor(ts, lf1, lf2, lf3, lf4);
    contractor.Start();
    std::atomic_bool finished;
    finished.store(0);
    auto customer = [ep, &finished]() {
        const zmqpp::context& ctx = context_wrapper::context;
        zmqpp::socket socket(ctx, zmqpp::socket_type::dealer);
        socket.connect(ep.asString());
        zmqpp::message msg;
        std::string command = u8"QUERY";
        std::string request_id = "123";
        int32_t argument = 17;
        auto binaryArgument = BinaryCoder<int32_t>().encode(argument);
        std::string function_name = "f111";
        msg << command << request_id << std::string(binaryArgument.begin(), binaryArgument.end()) << function_name;
        socket.send(msg, false);
        zmqpp::message reply;
        socket.receive(reply, false);
        auto reply_status = reply.get(0);
        ASSERT_EQ(u8"CODER_IDENTITY_NOT_FOUND", reply_status);
        auto reply_id = reply.get(1);
        ASSERT_EQ(reply_id, request_id);
        finished.store(true);
    };
    std::thread t(customer);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    ASSERT_TRUE(finished.load());
    t.join();
}

//TSAN IS FAILING ON THIS TEST!!!
TEST(ZMQContractor, MultipleClients) {
    const zmqpp::context& ctx = context_wrapper::context;
    Endpoint ep("tcp", "127.0.0.1:7777");
    communicator::zmq::ZMQFrozenTransportServer ts(ep, ctx);
    auto lf1 = make_library_function("f1", std::function<std::string (int32_t)>(f1));
    auto contractor = make_contractor(ts, lf1);
    contractor.Start();
    std::atomic_size_t finished;
    finished.store(0);
    auto customer = [ep, &finished](int32_t argument) {
        const zmqpp::context& local_ctx = context_wrapper::context;
        zmqpp::socket socket(local_ctx, zmqpp::socket_type::dealer);
        socket.connect(ep.asString());
        zmqpp::message msg;
        std::string command = u8"QUERY";
        std::string request_id = "123";
        std::string function_name = "f1";
        auto binaryArgument = BinaryCoder<int32_t>().encode(argument);
        msg << command << request_id << std::string(binaryArgument.begin(), binaryArgument.end()) << function_name;
        socket.send(msg, false);
        zmqpp::message reply;
        socket.receive(reply, false);
        auto reply_status = reply.get(0);
        ASSERT_EQ(u8"RESPONSE_RESULT", reply_status);
        auto reply_id = reply.get(1);
        ASSERT_EQ(reply_id, request_id);
        auto str_res = reply.get(2);
        Payload reply_res = {str_res.begin(), str_res.end()};
        ASSERT_EQ(std::to_string(argument), BinaryCoder<std::string>().decode(reply_res));
        ++finished;
    };
    for (int i = 3; i < 15; ++i) {
        std::thread t(customer, i);
        t.detach();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    ASSERT_TRUE(finished.load());
}

TEST(ZMQContractor, HigherOrderFunction) {
    // Acquiring context
    const zmqpp::context& ctx = context_wrapper::context;
    Endpoint ep("tcp", "127.0.0.1:7777");
    // Creating transport server and function wrappers
    communicator::zmq::ZMQFrozenTransportServer ts(ep, ctx);
    auto lf1 = make_library_function("f1", std::function<std::string (int32_t)>(f1));
    auto lf2 = make_library_function("f2", std::function<int32_t (std::string, int32_t)>(f2));
    // Creating transport
    communicator::zmq::ZMQTransportManager manager(ctx);
    auto transport = manager.create();
    BinaryCoder<RemoteFunction<int32_t (int32_t)>> remoteFunctionCoder(transport);
    BinaryCoder<std::tuple<int32_t, RemoteFunction<int32_t (int32_t)>>> argumentsCoder(std::make_tuple(
            BinaryCoder<int32_t>(), remoteFunctionCoder));
    auto lf5 = make_library_function(
            "f5",
            std::function<int32_t (int32_t, RemoteFunction<int32_t(int32_t)>)>(f5),
            BinaryCoder<int32_t>(),
            argumentsCoder);
    auto contractor = make_contractor(ts, lf1, lf2, lf5);
    contractor.Start();
    std::atomic_size_t finished;
    finished.store(0);
    auto functionalServer = [&finished] {
        //const zmqpp::context ctx = zmqpp::context();
        Endpoint ep("tcp", "127.0.0.1:8888");
        communicator::zmq::ZMQFrozenTransportServer ts(ep, ctx);
        auto tripler = [](int32_t arg) {return 3 * arg;};
        auto lf = make_library_function("tripler", std::function<int32_t (int32_t)>(tripler));
        auto contractor = make_contractor(ts, lf);
        contractor.Start();
        while (finished.load() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };

    auto customer = [ep, &finished, &manager]() {
        const zmqpp::context& local_ctx = context_wrapper::context; //zmqpp::context();
        zmqpp::socket socket(local_ctx, zmqpp::socket_type::dealer);
        socket.connect(ep.asString());
        zmqpp::message msg;
        std::string command = u8"QUERY";
        std::string request_id = "123";
        std::string function_name = "f5";
        //communicator::zmq::ZMQTransportManager manager(ctx);
        auto transport = manager.create();
        communicator::RemoteFunction<int32_t (int32_t)> remoteFunction(
                Endpoint("tcp", "127.0.0.1:8888"),
                "tripler",
                transport
        );
        int32_t argument = 33;
        BinaryCoder<RemoteFunction<int32_t (int32_t)>> remoteFunctionCoder(transport);
        BinaryCoder<std::tuple<int32_t, RemoteFunction<int32_t (int32_t)>>> argumentsCoder(std::make_tuple(
            BinaryCoder<int32_t>(), remoteFunctionCoder));
        Payload encodedArgs = argumentsCoder.encode(std::make_tuple(argument, remoteFunction));
        msg << command << request_id << std::string(encodedArgs.begin(), encodedArgs.end()) << function_name;
        std::cout << "TEST: sending request" << std::endl;
        socket.send(msg, false);
        zmqpp::message reply;
        std::cout << "TEST: waiting for reply" << std::endl;
        socket.receive(reply, false);
        auto reply_status = reply.get(0);
        ASSERT_EQ(u8"RESPONSE_RESULT", reply_status);
        auto reply_id = reply.get(1);
        ASSERT_EQ(reply_id, request_id);
        auto str_res = reply.get(2);
        Payload reply_res = {str_res.begin(), str_res.end()};
        ASSERT_EQ(3 * argument, BinaryCoder<int32_t>().decode(reply_res));
        ++finished;
    };

    std::thread(functionalServer).detach();
    std::thread(customer).detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_TRUE(finished.load());
}
