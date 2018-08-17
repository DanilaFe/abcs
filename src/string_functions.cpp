#include "functions.hpp"
#include <string>
#include <iostream>

extern int requested_precision;

FUNCTION(print_string) {
    string* param = (string*) libab_unwrap_param(params, 0);
    std::cout << param->value << std::endl;
    libab_get_unit_value(ab, into);
    return LIBAB_SUCCESS;
}

FUNCTION(to_string_num) {
    number* num = (number*) libab_unwrap_param(params, 0);
    mpfr_exp_t exp;
    char* str = mpfr_get_str(NULL, &exp, 10, requested_precision, num->value, MPFR_RNDN);
    std::string output_string = std::string(str).insert((mpfr_sgn(num->value) < 0) ? 2 : 1, 1, '.');
    if(exp != 1) {
        output_string += "e";
        output_string += std::to_string(exp - 1);
    }
    ref value = create_value(ab, new string(std::move(output_string)));
    libab_ref_copy(value, into);
    mpfr_free_str(str);
    return LIBAB_SUCCESS;
}

FUNCTION(to_string_bool) {
    int* val = (int*) libab_unwrap_param(params, 0);
    ref value = create_value(ab, new string(*val ? "true" : "false"));
    libab_ref_copy(value, into);
    return LIBAB_SUCCESS;
}

FUNCTION(to_string_unit) {
    ref value = create_value(ab, new string("()"));
    libab_ref_copy(value, into);
    return LIBAB_SUCCESS;
}
