//
// Created by fedor on 8/3/20.
//

#include "test_main.hpp"
#include "Communicator/Endpoint.hpp"
#include <zmqpp/zmqpp.hpp>
#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include "Communicator/ZMQTransport/ZMQTransport.hpp"

using std::string;
using namespace communicator;

TEST(Utility, Endpoint) {
    std::string host = "123.123.123.123";
    int32_t port = 4444;
    std::string address = host + ":" + std::to_string(port);

    auto ep = Endpoint("tcp", address);

    ASSERT_EQ(host, ep.host());
    ASSERT_EQ(port, ep.port());

    std::string sep = "tcp://" + host + ":" + std::to_string(port);
    ASSERT_EQ(sep, ep.asString());
    Endpoint same(sep);
    ASSERT_EQ(ep, same);
    ASSERT_EQ(ep.address, same.address);
    ASSERT_EQ(ep.protocol, same.protocol);
}

TEST(Utility, ZMQHelloWorld) {
    auto server = [](const string& expected_text, const string& answer) {
        const string endpoint = "tcp://*:5555";
        zmqpp::context context;
        zmqpp::socket_type type = zmqpp::socket_type::reply;
        zmqpp::socket socket (context, type);
        socket.bind(endpoint);
        for (int i = 0; i < 2; ++i) {
            zmqpp::message message;
            socket.receive(message);
            string text;
            message >> text;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            ASSERT_EQ(text, expected_text);
            socket.send(answer);
        }
    };

    auto client = [](const string& msg_text, const string& expected_text) {
        const string endpoint = "tcp://localhost:5555";
        zmqpp::context context;
        zmqpp::socket_type type = zmqpp::socket_type::req;
        zmqpp::socket socket (context, type);
        socket.connect(endpoint);
        zmqpp::message message;
        message << msg_text;
        socket.send(message);
        string buffer;
        socket.receive(buffer);
        ASSERT_EQ(buffer, expected_text);
    };

    std::atomic_bool finished;
    finished.store(false);

    auto tester = [&client, &server, &finished]() {
        string msg1 = "Hello,";
        string msg2 = "World!";

        std::thread s(server, msg1, msg2);
        std::thread c(client, msg1, msg2);
        std::thread c2(client, msg1, msg2);

        s.join();
        c.join();
        c2.join();
        finished.store(true);
    };

    std::thread t(tester);
    std::this_thread::sleep_for(std::chrono::milliseconds(220));
    ASSERT_TRUE(finished.load());
    t.join();
}
