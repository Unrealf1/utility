//
// Created by fedor on 3/18/19.
//

#ifndef FORMALLANGUAGES_TEST_MAIN_HPP
#define FORMALLANGUAGES_TEST_MAIN_HPP

#include "gtest/gtest.h"
#include "zmqpp/context.hpp"

class context_wrapper{
public:
    const static zmqpp::context context;
};

#endif //FORMALLANGUAGES_TEST_MAIN_HPP
