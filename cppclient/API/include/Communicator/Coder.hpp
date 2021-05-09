//
// Created by fedor on 8/3/20.
//

#ifndef CPPCLIENT_CODER_HPP
#define CPPCLIENT_CODER_HPP

#include <utility>
#include <vector>
#include <array>
#include <bitset>
#include <tuple>


namespace communicator {

using Byte = int8_t;
using Identity = std::string;
using Payload = std::vector<Byte>;

class DecodingException: public std::exception {
public:
    explicit DecodingException(std::string reason): reason(std::move(reason)) {}

    const char* what() const noexcept override {
        return reason.c_str();
    }
private:
    std::string reason;
};

/// Provides functionality to serialize/deserialize objects of given type
template <typename T>
class Coder {
public:
    // I'm not sure, whether encode/decode should be const or not.
    virtual Payload encode(const T& value) const = 0;
    virtual T decode(const Payload& bin) const = 0;
    virtual Identity getIdentity() const = 0;
    virtual ~Coder() = default;
};

template <typename T>
class BinaryCoder: public Coder<T> {};


template <size_t size>
class FixedSizeCoder {
    size_t getSize() const {
        return size;
    }
};

}


#endif //CPPCLIENT_CODER_HPP
