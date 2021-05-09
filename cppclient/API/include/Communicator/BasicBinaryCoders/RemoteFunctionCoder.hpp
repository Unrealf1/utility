//
// Created by fedor on 8/16/20.
//

#ifndef CPPCLIENT_REMOTEFUNCTIONCODER_HPP
#define CPPCLIENT_REMOTEFUNCTIONCODER_HPP

#include "Communicator/Coder.hpp"
#include "Communicator/RemoteFunction.hpp"
#include "TupleCoder.hpp"
#include "EndpointCoder.hpp"
#include "StringCoder.hpp"

namespace communicator {

    template <typename R, typename... Args>
    class BinaryCoder<RemoteFunction<R (Args...)>>: public Coder<RemoteFunction<R (Args...)>> {
    public:
        explicit BinaryCoder(communicator::Transport& transport)
        : transport(transport){

        }

        Payload encode(const RemoteFunction<R (Args...)>& value) const override {
            return elementCoder.encode(std::make_tuple(value.endpoint, value.name));
        }
        RemoteFunction<R (Args...)> decode(const Payload& bin) const override {
            auto decoded = elementCoder.decode(bin);
            return RemoteFunction<R (Args...)>(std::get<0>(decoded), std::get<1>(decoded), transport);
        }
        Identity getIdentity() const override {
            return {};
        }
    private:
        BinaryCoder<std::tuple<Endpoint, std::string>> elementCoder;
        communicator::Transport& transport;
    public:
        static const Identity identity;
    };
    template <typename R, typename... Args>
    const Identity BinaryCoder<RemoteFunction<R (Args...)>>::identity = "RemoteFunction<" +
                                                                        BinaryCoder<R>::identity + ", " + BinaryCoder<Args...>::identity + '>';
}

#endif //CPPCLIENT_REMOTEFUNCTIONCODER_HPP
