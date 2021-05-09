//
// Created by fedor on 8/16/20.
//

#ifndef CPPCLIENT_STRINGCODER_HPP
#define CPPCLIENT_STRINGCODER_HPP

#include "Communicator/Coder.hpp"
#include "IntCoder.hpp"

namespace communicator {
    template <>
    class BinaryCoder<std::string>: public Coder<std::string> {
    public:
        Payload encode(const std::string& val) const override {
            Payload binSize = BinaryCoder<int32_t>().encode(
                    static_cast<int32_t>(val.size()));
            Payload binData = {val.begin(), val.end()};
            binSize.insert(binSize.end(),
                           std::make_move_iterator(binData.begin()),
                           std::make_move_iterator(binData.end()));
            return binSize;
        }

        std::string decode(const Payload& bin) const override {
            auto size = BinaryCoder<int32_t>().decode({bin.data(), bin.data() + sizeof(int32_t)});
            auto data_start = bin.begin() + sizeof(size);
            return {data_start, data_start + size};
        }

        Identity getIdentity() const override {
            return identity;
        }
        static const Identity identity;
    };

}

#endif //CPPCLIENT_STRINGCODER_HPP
