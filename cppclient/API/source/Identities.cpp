//
// Created by fedor on 8/26/20.
//

#include "Communicator/BasicCoders.hpp"

namespace communicator {
    const Identity BinaryCoder<int64_t>::identity = "Long";
    const Identity BinaryCoder<int32_t>::identity = "Int";
    const Identity BinaryCoder<double>::identity = "Double";
    const Identity BinaryCoder<std::string>::identity = "String";
    const Identity BinaryCoder<std::tuple<>>::identity = "Object<>";
    const Identity BinaryCoder<Endpoint>::identity = "Endpoint";
}