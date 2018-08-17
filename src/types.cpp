#include "types.hpp"

string::string(std::string&& new_value): value(std::move(new_value)) {}
string::string(const std::string& new_value) : value(new_value) {}

number::number(const char* new_value) {
    mpfr_init2(value, PRECISION);
    mpfr_set_str(value, new_value, 10, MPFR_RNDN);
}
number::number(mpfr_t&& new_value) {
    std::swap(value, new_value);
}
int number::to_int() {
    return mpfr_get_si(value, MPFR_RNDN);
}
number::~number() {
    mpfr_clear(value);
}
