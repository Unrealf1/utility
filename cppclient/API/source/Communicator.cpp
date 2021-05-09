#include "Communicator/Communicator.hpp"
#include "Communicator/ZMQTransport/ZMQFrozenTransportServer.hpp"
#include "Communicator/ZMQTransport/ZMQTransportManager.hpp"
#include "Communicator/ZMQTransport/ZMQContext.hpp"

namespace communicator { 
    zmqpp::context zmq::ContextWrapper::ctx = zmqpp::context();

    TransportManager* createDefaultTransportManager() {
        return new communicator::zmq::ZMQTransportManager(zmq::ContextWrapper::ctx);
    }

    FrozenTransportServer* createDefaultTransportServer(const Endpoint& endpoint) {
        return new communicator::zmq::ZMQFrozenTransportServer(endpoint, zmq::ContextWrapper::ctx);
    }
}