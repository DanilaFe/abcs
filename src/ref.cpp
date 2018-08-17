#include "ref.hpp"

ref::ref() {
    libab_ref_null(&val);
}

ref::ref(void* data, void (*free_func)(void*)) {
    libab_ref_new(&val, data, free_func);
}

ref::ref(const ref& other) {
    libab_ref_copy(&other.val, &val);
}

ref& ref::operator=(const ref& other) {
    libab_ref_copy(&other.val, &val);
    return *this;
}

ref& ref::operator=(std::nullptr_t t) {
    libab_ref_free(&val);
    libab_ref_null(&val);
    return *this;
}

ref::operator libab_ref*() {
    return &val;
}

ref::~ref() {
    libab_ref_free(&val);
}

template <>
bool ref::operator==<std::nullptr_t>(std::nullptr_t t) {
    return libab_ref_get(&val) == t;
}
