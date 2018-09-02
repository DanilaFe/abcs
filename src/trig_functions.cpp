#include "trig_functions.hpp"
#include <mpfr.h>
#include "ref.hpp"
#include "types.hpp"
#include "util.hpp"

FUNCTION_MPFR(sin, sin);
FUNCTION_MPFR(cos, cos);
FUNCTION_MPFR(tan, tan);

FUNCTION_MPFR(arcsin, asin);
FUNCTION_MPFR(arccos, acos);
FUNCTION_MPFR(arctan, atan);
