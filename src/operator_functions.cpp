#include "functions.hpp"

FUNCTION_MPFR2(plus, add)
FUNCTION_MPFR2(minus, sub)
FUNCTION_MPFR2(times, mul)
FUNCTION_MPFR2(divide, div)
FUNCTION_MPFR2(pow, pow);

FUNCTION_MPFR(negate, neg);
FUNCTION(factorial) {
    number* left = (number*) libab_unwrap_param(params, 0);   
    mpfr_t plus_one;
    mpfr_t gamma;

    mpfr_init2(plus_one, PRECISION);
    mpfr_init2(gamma, PRECISION);
    mpfr_add_ui(plus_one, left->value, 1, MPFR_RNDN);
    mpfr_gamma(gamma, plus_one, MPFR_RNDN);

    number* to_return = new number(std::move(gamma));
    ref return_value = create_value<number>(ab, to_return);
    libab_ref_copy(return_value, into);

    return LIBAB_SUCCESS;
}

FUNCTION_COMPARE(lt, <);
FUNCTION_COMPARE(lte, <=);
FUNCTION_COMPARE(equals, ==);
FUNCTION_COMPARE(gt, >);
FUNCTION_COMPARE(gte, >=);
