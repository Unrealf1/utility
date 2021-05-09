//
// Created by fedor on 8/16/20.
//

#ifndef CPPCLIENT_ENDPOINTCODER_HPP
#define CPPCLIENT_ENDPOINTCODER_HPP

#include "Communicator/Coder.hpp"
#include "Communicator/Endpoint.hpp"
#include "StringCoder.hpp"

namespace communicator {
    template <>
    class BinaryCoder<Endpoint>: public Coder<Endpoint> {
    public:
        Payload encode(const Endpoint& value) const override {
            return stringCoder.encode(value.endpoint);
        }
        Endpoint decode(const Payload& bin) const override {
            return Endpoint(stringCoder.decode(bin));
        }
        Identity getIdentity() const override {
            return identity;
        }
    private:
        BinaryCoder<std::string> stringCoder;
    public:
        static const Identity identity;
    };
}

#endif //CPPCLIENT_ENDPOINTCODER_HPP
