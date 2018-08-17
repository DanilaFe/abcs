#include "util.hpp"

template <>
ref create_value<string>(libab* ab, string* param) {
    ref type;
    ref value;
    libab_create_type(ab, type, "str");
    libab_create_value_raw(ab, value, (void*) param, type);
    return value;
}

template <>
ref create_value<number>(libab* ab, number* param) {
    ref type;
    ref value;
    libab_get_type_num(ab, type);
    libab_create_value_raw(ab, value, (void*) param, type);
    return value;
}
