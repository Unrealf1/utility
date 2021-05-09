//
// Created by fedor on 8/16/20.
//

#ifndef CPPCLIENT_VECTORCODER_HPP
#define CPPCLIENT_VECTORCODER_HPP

#include "Communicator/Coder.hpp"

namespace communicator {
    template <typename T>
    class BinaryCoder<std::vector<T>>: public Coder<std::vector<T>> {
    public:
        BinaryCoder() = default;

        explicit BinaryCoder(BinaryCoder<T> elementCoder):
        elementCoder(std::move(elementCoder)) {};

        Payload encode(const std::vector<T>& value) const override {
            Payload result;
            for (auto& element : value) {
                auto bin = elementCoder.encode(element);
                result.insert(result.end(), bin.begin(), bin.end());
            }
            return result;
        }
        std::vector<T> decode(const Payload& bin) const override {
            std::vector<T> result;
            Payload rest = bin;
            while (!rest.empty()) {
                T decoded = elementCoder.decode(rest);
                int64_t binarySize = static_cast<int64_t >(elementCoder.encode(decoded).size());
                rest = {rest.begin() + binarySize, rest.end()};
                result.push_back(decoded);
            }
            return result;
        }

        Identity getIdentity() const override {
            return identity;
        }
    private:
        BinaryCoder<T> elementCoder;
    public:
        static const Identity identity;
    };
    template <typename T>
    const Identity BinaryCoder<std::vector<T>>::identity = "List<" + BinaryCoder<T>::identity + '>';
}

#endif //CPPCLIENT_VECTORCODER_HPP
