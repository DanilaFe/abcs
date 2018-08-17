#pragma once
#include <mpfr.h>
#include <string>
#include "config.hpp"

struct string {
    std::string value;
    string(std::string&& new_value);
    string(const std::string& new_value);
};

struct number {
    mpfr_t value;
    number(const char* new_value);
    number(mpfr_t&& new_value);
    int to_int();
    ~number();
};

