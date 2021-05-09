//
// Created by fedor on 8/26/20.
//

#ifndef CPPCLIENT_FLOATINGPOINTCODER_HPP
#define CPPCLIENT_FLOATINGPOINTCODER_HPP

#include "Communicator/Coder.hpp"
#include <cstring>

namespace communicator {
    template <>
    class BinaryCoder<double>: public Coder<double>, public FixedSizeCoder<sizeof(double)> {
    public:
        Payload encode(const double& val) const override {
            Byte bytes[sizeof(double)];
            memcpy(bytes, &val, sizeof(val));
            return Payload(bytes, bytes + sizeof(double));
        }

        double decode(const Payload& bin) const override {
            if (bin.size() < sizeof(double)) {
                throw DecodingException(
                        "Payload should be at least " + std::to_string(sizeof(double)) +
                        " bytes for double, but got " + std::to_string(bin.size()));
            }
            double result;
            memcpy(&result, bin.data(), sizeof(double));
            return result;
        }

        Identity getIdentity() const override {
            return identity;
        }
        static const Identity identity;
    };
}


#endif //CPPCLIENT_FLOATINGPOINTCODER_HPP
