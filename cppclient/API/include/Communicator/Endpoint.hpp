//
// Created by fedor on 8/3/20.
//

#ifndef CPPCLIENT_ENDPOINT_HPP
#define CPPCLIENT_ENDPOINT_HPP

#include <string>
#include <iostream>

namespace communicator {

using std::string;
using std::stoi;

/// Class for convenient work with addresses. Immutable after creation
class Endpoint {
public:
    /**
     * @param protocol  A string that represents protocol. For example "tcp", "inproc"
     * @param address   A string that represents full address. For example "127.0.0.1:8888"
     */
    Endpoint(const string& protocol, const string& address):
            protocol(protocol), address(address), endpoint(protocol+protocol_address_delimiter+address){}

    explicit Endpoint(const string& ep):
    protocol(ep.substr(0, ep.find(protocol_address_delimiter))),
    address(ep.substr(ep.find(protocol_address_delimiter) + protocol_address_delimiter.size())),
    endpoint(ep){}

    bool operator==(const Endpoint& other) const {
        return other.endpoint == endpoint;
    }
    explicit operator std::string() const {
        return endpoint;
    }

    const string protocol;
    const string address;
    const string endpoint;

    string host() const {
        return address.substr(0, address.find(':'));
    }

    int16_t port() const {
        return static_cast<int16_t>(stoi(address.substr(address.find(':') + 1)));
    }

    std::string asString() const {
        return endpoint;
    };
private:
    // This should later be improved with something like
    // static constexpr const char * const
    static const std::string protocol_address_delimiter;
};

}

#endif //CPPCLIENT_ENDPOINT_HPP
