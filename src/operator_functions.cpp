#include "functions.hpp"

FUNCTION_MPFR2(plus, add)
FUNCTION_MPFR2(minus, sub)
FUNCTION_MPFR2(times, mul)
FUNCTION_MPFR2(divide, div)
FUNCTION_MPFR2(pow, pow);

FUNCTION_MPFR(negate, neg);

FUNCTION_COMPARE(lt, <);
FUNCTION_COMPARE(lte, <=);
FUNCTION_COMPARE(equals, ==);
FUNCTION_COMPARE(gt, >);
FUNCTION_COMPARE(gte, >=);
