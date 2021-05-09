//
// Created by fedor on 8/16/20.
//

#ifndef CPPCLIENT_TUPLECODER_HPP
#define CPPCLIENT_TUPLECODER_HPP

#include "Communicator/Coder.hpp"
#include "Communicator/Helper.hpp"
#include "IntCoder.hpp"
#include <tuple>

namespace communicator {

    template <typename T, typename... Other>
    class BinaryCoder<std::tuple<T, Other...>>: public Coder<std::tuple<T, Other...>> {
    public:
        BinaryCoder() = default;
        explicit BinaryCoder(std::tuple<BinaryCoder<T>, BinaryCoder<Other>...> coders)
        : tCoder(std::move(internal::head(coders))), othersCoder(internal::tail(coders)){}

        Payload encode(const std::tuple<T, Other...>& value) const override {
            auto encoded = tCoder.encode(internal::head(value));
            auto encodedOther = othersCoder.encode(internal::tail(value));
            encoded.insert(
                    encoded.end(),
                    std::make_move_iterator(encodedOther.begin()),
                    std::make_move_iterator(encodedOther.end())
            );
            return encoded;
        }

        std::tuple<T, Other...> decode(const Payload& bin) const override {
            T decoded = tCoder.decode(bin);
            auto binarySize = static_cast<int64_t>(tCoder.encode(decoded).size());
            Payload rest = {bin.begin() + binarySize, bin.end()};
            return std::tuple_cat(std::make_tuple(decoded), othersCoder.decode(rest));
        }

        Identity getIdentity() const override {
            return identity;
        }

        static Identity getElementCodersIdentity() {
            return BinaryCoder<T>::identity + ", " + BinaryCoder<std::tuple<Other...>>::getElementCodersIdentity();
        }
    private:
        BinaryCoder<T> tCoder;
        BinaryCoder<std::tuple<Other...>> othersCoder;
    public:
        static const Identity identity;
    };
    template <typename T, typename... Other>
    const Identity BinaryCoder<std::tuple<T, Other...>>::identity =
            "Object<" + BinaryCoder<std::tuple<T, Other...>>::getElementCodersIdentity() + '>';

    template <typename T>
    class BinaryCoder<std::tuple<T>>: public Coder<std::tuple<T>> {
    public:
        BinaryCoder() = default;
        explicit BinaryCoder(std::tuple<BinaryCoder<T>> coder)
        : elementCoder(std::move(internal::head(coder))){}

        Payload encode(const std::tuple<T>& value) const override {
            return elementCoder.encode(std::get<0>(value));
        }

        std::tuple<T> decode(const Payload& bin) const override {
            return std::make_tuple(elementCoder.decode(bin));
        }

        Identity getIdentity() const override {
            return identity;
        }

        static Identity getElementCodersIdentity() {
            return BinaryCoder<T>::identity;
        }
    private:
        BinaryCoder<T> elementCoder;
    public:
        static const Identity identity;
    };

    template <typename T>
    const Identity BinaryCoder<std::tuple<T>>::identity = "Object<" + BinaryCoder<T>::identity + '>';


    template <>
    class BinaryCoder<std::tuple<>>: public Coder<std::tuple<>> {
    public:
        Payload encode(const std::tuple<>& value) const override {
            (void)value;
            return {};
        }

        std::tuple<> decode(const Payload& bin) const override {
            if (!bin.empty()) {
                throw DecodingException("Expected empty bin for std::tuple<>, but it isn't empty");
            }
            return {};
        }

        Identity getIdentity() const override {
            return identity;
        }

        static Identity getElementCodersIdentity() {
            return "";
        }

        static const Identity identity;
    };

}

#endif //CPPCLIENT_TUPLECODER_HPP
