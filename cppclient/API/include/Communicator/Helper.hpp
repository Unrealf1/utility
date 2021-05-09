//
// Created by fedor on 8/11/20.
//

#ifndef CPPCLIENT_DARKMAGIC_HPP
#define CPPCLIENT_DARKMAGIC_HPP

#include <tuple>
#include <functional>

namespace communicator { namespace internal {

template <typename T, typename... Ts>
auto head(const std::tuple<T,Ts...>& t) {
    return std::get<0>(t);
}

template <std::size_t... Ns, typename... Ts>
auto tail_impl(std::index_sequence<Ns...>, const std::tuple<Ts...>& t) {
    return std::make_tuple(std::get<Ns+1u>(t)... );
}

template <typename... Ts>
auto tail(const std::tuple<Ts...>& t) {
    return tail_impl(std::make_index_sequence<sizeof...(Ts) - 1u>(), t);
}

}}


#endif //CPPCLIENT_DARKMAGIC_HPP
