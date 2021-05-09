//
// Created by fedor on 8/14/20.
//

#ifndef CPPCLIENT_REMOTEFUNCTION_HPP
#define CPPCLIENT_REMOTEFUNCTION_HPP

#include "Communicator/Transport.hpp"
#include "Communicator/Endpoint.hpp"


namespace communicator {
    template <typename UnusedType>
    class RemoteFunction;

    ///A class that represents a function that would be computed in another environment
    /**
     * @tparam R -- Function result type
     * @tparam Args -- Function arguments
     */
    template <typename R, typename... Args>
    class RemoteFunction<R (Args...)> {
    public:
        explicit RemoteFunction(
                const Endpoint& endpoint /**< [in] functional server address */,
                const std::string& name /**< [in] function name on remote functional server */,
                Transport& transport /**< [in] any transport that will be used to make request to remote functional server */,
                const BinaryCoder<R>& resultCoder = BinaryCoder<R>(),
                const BinaryCoder<Args...>& argsCoder = BinaryCoder<Args...>())
        :endpoint(endpoint),
        name(name),
        remoteCall(transport.channel(endpoint, name)),
        resultCoder(resultCoder),
        argsCoder(argsCoder){}

        R operator() (Args... args) {
            Payload encodedArgs = argsCoder.encode(args...);
            Payload encodedResult = remoteCall(encodedArgs);
            return resultCoder.decode(encodedResult);
        }

        const Endpoint endpoint;
        const std::string name;
    private:
        PayloadFunction remoteCall;
        BinaryCoder<R> resultCoder;
        BinaryCoder<Args...> argsCoder;
    };
}


#endif //CPPCLIENT_REMOTEFUNCTION_HPP
