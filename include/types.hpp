#pragma once
#include <mpfr.h>
#include <string>
#include "config.hpp"

struct string {
    std::string value;
    string(std::string&& new_value) : value(std::move(new_value)) {}
    string(const std::string& new_value) : value(new_value) {}
};

struct number {
    mpfr_t value;
    number(const char* new_value) {
        mpfr_init2(value, PRECISION);
        mpfr_set_str(value, new_value, 10, MPFR_RNDN);
    }
    number(mpfr_t&& new_value) {
        std::swap(value, new_value);
    }
    int to_int() {
        return mpfr_get_si(value, MPFR_RNDN);
    }
    ~number() {
        mpfr_clear(value);
    }
};

