#pragma once

#include <zmqpp/context.hpp>

namespace communicator { namespace zmq {
    class ContextWrapper {
    public:
        static zmqpp::context ctx;
    };
}}

