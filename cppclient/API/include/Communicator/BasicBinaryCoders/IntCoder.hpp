//
// Created by fedor on 8/16/20.
//

#ifndef CPPCLIENT_INTCODER_HPP
#define CPPCLIENT_INTCODER_HPP

#include "Communicator/Coder.hpp"
#include <cstring>

namespace communicator {
    template <typename inttype>
    class BinnaryIntegerCoder: public Coder<inttype>, public FixedSizeCoder<sizeof(inttype)> {
        static constexpr size_t size = sizeof(inttype);
    public:
        Payload encode(const inttype& val) const override {
            Byte bytes[size];
            memcpy(bytes, &val, size);
            return Payload(bytes, bytes + size);
        }

        inttype decode(const Payload& bin) const override {
            if (bin.size() < size) {
                throw DecodingException(
                        "Payload should be at least " + std::to_string(size) +
                        " bytes for int of size " + std::to_string(size) +
                        ", but got " + std::to_string(bin.size()));
            }
            inttype result;
            memcpy(&result, bin.data(), size);
            return result;
        }
    };

    template <>
    class BinaryCoder<int32_t>: public BinnaryIntegerCoder<int32_t> {
    public:
        Identity getIdentity() const override {
            return identity;
        }
        static const Identity identity;
    };

    template <>
    class BinaryCoder<int64_t>: public BinnaryIntegerCoder<int64_t> {
    public:
        Identity getIdentity() const override {
            return identity;
        }
        static const Identity identity;
    };
}


#endif //CPPCLIENT_INTCODER_HPP
