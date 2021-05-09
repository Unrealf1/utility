#include "test_main.hpp"
#include <spdlog/spdlog.h>
#include <thread>

const zmqpp::context context_wrapper::context;

int main(int argc, char** argv) {
    spdlog::set_level(spdlog::level::debug);
    ::testing::InitGoogleTest(&argc, argv);
    int test_res = RUN_ALL_TESTS();
    spdlog::shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    return test_res;
}
