#pragma once

#include "Communicator/Transport.hpp"
#include "Communicator/TransportServer.hpp"
#include "Communicator/Contractor.hpp"
#include "Communicator/LibraryFunction.hpp"

namespace communicator {
    TransportManager* createDefaultTransportManager();
    FrozenTransportServer* createDefaultTransportServer(const Endpoint& endpoint);
}