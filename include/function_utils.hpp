#pragma once

extern "C" {
#include "libabacus.h"
}

#define FUNCTION(name) libab_result function_##name( \
        libab* ab, libab_ref* scope, libab_ref_vec* params, libab_ref* into)

#define FUNCTION_MPFR(name, func) FUNCTION(name) { \
    number* value = (number*) libab_unwrap_param(params, 0); \
    mpfr_t output; \
    mpfr_init2(output, PRECISION); \
    mpfr_##func(output, value->value, MPFR_RNDN); \
    ref to_return = create_value<number>(ab, new number(std::move(output))); \
    libab_ref_copy(to_return, into); \
    return LIBAB_SUCCESS; \
}

#define FUNCTION_MPFR2(name, func) FUNCTION(name) { \
    number* left = (number*) libab_unwrap_param(params, 0); \
    number* right = (number*) libab_unwrap_param(params, 1); \
    mpfr_t output; \
    mpfr_init2(output, PRECISION); \
    mpfr_##func(output, left->value, right->value, MPFR_RNDN); \
    ref to_return = create_value<number>(ab, new number(std::move(output))); \
    libab_ref_copy(to_return, into); \
    return LIBAB_SUCCESS; \
}

#define FUNCTION_COMPARE(name, op) FUNCTION(name) { \
    number* left = (number*) libab_unwrap_param(params, 0); \
    number* right = (number*) libab_unwrap_param(params, 1); \
    libab_get_bool_value(ab, mpfr_cmp(left->value, right->value) op 0, into); \
    return LIBAB_SUCCESS; \
}
