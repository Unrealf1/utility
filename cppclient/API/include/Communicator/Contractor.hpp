//
// Created by fedor on 8/7/20.
//

#ifndef CPPCLIENT_CONTRACTOR_HPP
#define CPPCLIENT_CONTRACTOR_HPP

#include "Communicator/TransportServer.hpp"
#include "LibraryFunction.hpp"


namespace communicator {


/// A thin wrapper around TransportServer, made purely for convenience
class Contractor {
public:
    // Probably should make better templates and constructor blocking, because if
    // functions get destroyed while this working, everything will explode with passion
    explicit Contractor(FrozenTransportServer& tServer): tServer(tServer){}

    template <typename T, typename... Rest>
    void Register(T& f, Rest&... rest) {
        Register(f);
        Register(rest...);
    }

    template <typename T>
    void Register(T& f) {
        tServer.reg(f.name, f.asPayloadFunction(), f.getArgsCoderIdentity(), f.getResCoderIdentity());
    }

    void Start() {
        tServer.start();
    }
private:
    FrozenTransportServer& tServer;
};

template <typename... LFs>
Contractor make_contractor(FrozenTransportServer& tServer, LFs&... functions) {
    Contractor contractor(tServer);
    contractor.Register(functions...);
    return contractor;
}

}

#endif //CPPCLIENT_CONTRACTOR_HPP
