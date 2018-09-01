#pragma once

#include "ref.hpp"
#include "util.hpp"
#include "function_utils.hpp"
extern "C" {
#include "libabacus.h"
}

FUNCTION(print_string);
FUNCTION(to_string_num);
FUNCTION(to_string_bool);
FUNCTION(to_string_unit);

FUNCTION(plus);
FUNCTION(minus);
FUNCTION(times);
FUNCTION(divide);
FUNCTION(pow);

FUNCTION(lt);
FUNCTION(lte);
FUNCTION(equals);
FUNCTION(gt);
FUNCTION(gte);

FUNCTION(negate);
FUNCTION(factorial);

FUNCTION(ln);
FUNCTION(exp);

FUNCTION(sqrt);

FUNCTION(sin);
FUNCTION(cos);
FUNCTION(tan);

FUNCTION(arcsin);
FUNCTION(arccos);
FUNCTION(arctan);

FUNCTION(quit);
FUNCTION(request_precision);
