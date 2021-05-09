//
// Created by fedor on 8/11/20.
//

#ifndef CPPCLIENT_LIBRARYFUNCTION_HPP
#define CPPCLIENT_LIBRARYFUNCTION_HPP

#include <string>
#include <functional>
#include <utility>
#include <tuple>
#include "Communicator/Transport.hpp"
#include "Communicator/Coder.hpp"
#include "Communicator/BasicCoders.hpp"
#include "Communicator/Helper.hpp"


namespace communicator {

template <typename UnusedType>
class LibraryFunction;

/// Function wrapper. Will mask any function as PayloadFunction to be used later in TransportServer
/**
 * @tparam R -- Function result type
 * @tparam Args -- Function arguments
 */
template <typename R, typename... Args>
class LibraryFunction<R (Args...)> {
public:
    LibraryFunction(std::string  name, const std::function<R (Args...)>& f)
            :name(std::move(name)), f(f){}

    LibraryFunction(
            std::string  name,
            const std::function<R (Args...)>& f,
            const BinaryCoder<R>& resultCoder,
            const BinaryCoder<std::tuple<Args...>>& argsCoder)
            :name(std::move(name)), f(f), resultCoder(resultCoder), argsCoder(argsCoder){}

    PayloadFunction asPayloadFunction() {
        return [this](Payload payload) {
            auto args = argsCoder.decode(payload);
            return resultCoder.encode(std::apply(f, args));
        };
    }

    Identity getArgsCoderIdentity() const {
        return argsCoder.getIdentity();
    }

    Identity getResCoderIdentity() const {
        return resultCoder.getIdentity();
    }

    ~LibraryFunction() = default;
    const std::string name;
private:
    const std::function<R (Args...)> f;
    BinaryCoder<R> resultCoder;
    BinaryCoder<std::tuple<Args...>> argsCoder;
};

template <typename R, typename... Args>
LibraryFunction<R (Args...)> make_library_function(const std::string& name, const std::function<R (Args...)>& f) {
    return LibraryFunction<R (Args...)>(name, f);
}

template <typename R, typename... Args>
LibraryFunction<R (Args...)> make_library_function(const std::string& name,
                                                  const std::function<R (Args...)>& f,
                                                  const BinaryCoder<R>& resultCoder,
                                                  const BinaryCoder<std::tuple<Args...>>& argsCoder) {
    return LibraryFunction<R (Args...)>(name, f, resultCoder, argsCoder);
}

}


#endif //CPPCLIENT_LIBRARYFUNCTION_HPP
