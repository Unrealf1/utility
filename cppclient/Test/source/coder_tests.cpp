//
// Created by fedor on 8/3/20.
//

#include "Communicator/BasicCoders.hpp"
#include "Communicator/RemoteFunction.hpp"
#include "Communicator/ZMQTransport/ZMQTransportManager.hpp"
#include "test_main.hpp"
#include <cstdint>
#include <vector>
#include <array>
#include <limits>


using namespace communicator;

TEST(Coder, Int32) {
    BinaryCoder<int32_t> coder;
    ASSERT_EQ(coder.identity, "Int");
    for (int32_t i = -2; i < 3; ++i) {
        auto j = coder.decode(coder.encode(i));
        ASSERT_EQ(i, j);
    }
    auto mx = std::numeric_limits<int32_t>::max();
    auto mn = std::numeric_limits<int32_t>::min();
    ASSERT_EQ(mx, coder.decode(coder.encode(mx)));
    ASSERT_EQ(mn, coder.decode(coder.encode(mn)));
}

TEST(Coder, Int64) {
    BinaryCoder<int64_t> coder;
    ASSERT_EQ(coder.identity, "Long");
    for (int32_t i = -2; i < 3; ++i) {
        auto j = coder.decode(coder.encode(i));
        ASSERT_EQ(i, j);
    }
    auto mx = std::numeric_limits<int64_t>::max();
    auto mn = std::numeric_limits<int64_t>::min();
    ASSERT_EQ(mx, coder.decode(coder.encode(mx)));
    ASSERT_EQ(mn, coder.decode(coder.encode(mn)));
}


TEST(Coder, Double) {
    BinaryCoder<double> coder;
    std::array<double, 6> samples = {
            0.1, 0.3, 0.1+0.2, 0.0,
            std::numeric_limits<double>::max(), std::numeric_limits<double>::min()};

    for (auto i : samples) {
        auto j = coder.decode(coder.encode(i));
        ASSERT_EQ(i, j);
    }
}

TEST(Coder, String) {
    BinaryCoder<std::string> coder;
    ASSERT_EQ(coder.identity, "String");
    std::vector<std::string> test_strings = {"", "asdf", "1-3aA.\t<\n0"};
    for (auto s : test_strings) {
        auto j = coder.decode(coder.encode(s));
        ASSERT_EQ(s, j);
    }
}

TEST(Coder, Vector) {
    BinaryCoder<std::vector<int32_t>> coder;
    ASSERT_EQ(coder.identity, "List<Int>");
    std::vector<int32_t> sample = {1, 4, 2, 6, 8};
    ASSERT_EQ(sample, coder.decode(coder.encode(sample)));
}

TEST(Coder, Endpoint) {
    BinaryCoder<Endpoint> coder;
    Endpoint sample("tcp", "127.0.0.1:12345");
    ASSERT_EQ(sample, coder.decode(coder.encode(sample)));
}

TEST(Coder, RemoteFunction) {
    communicator::zmq::ZMQTransportManager manager(context_wrapper::context);
    auto transport = manager.create();
    BinaryCoder<RemoteFunction<int32_t (std::string)>> coder(transport);
    ASSERT_EQ(coder.identity, "RemoteFunction<Int, String>");
    Endpoint ep("tcp", "127.0.0.1:12345");
    std::string name = "some name";
    RemoteFunction<int32_t (std::string)> sample(ep, name, transport);
    auto decoded = coder.decode(coder.encode(sample));
    ASSERT_EQ(sample.endpoint, decoded.endpoint);
    ASSERT_EQ(sample.name, decoded.name);
}

TEST(Coder, Tuple) {
    BinaryCoder<std::tuple<int32_t, std::string, int64_t>> coder;
    ASSERT_EQ(coder.identity, "Object<Int, String, Long>");
    auto sample = std::make_tuple(1, "2", 3);
    ASSERT_EQ(sample, coder.decode(coder.encode(sample)));
}
