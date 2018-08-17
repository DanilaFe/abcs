#pragma once

extern "C" {
#include "libabacus.h"
#include "util.h"
}

class abacus_ref {
    private:
        libab_ref ref;
    public:
        abacus_ref();
        abacus_ref(void* data, void (*free_func)(void*));
        abacus_ref(const abacus_ref& other);
        abacus_ref& operator=(const abacus_ref& other);
        abacus_ref& operator=(std::nullptr_t);
        template <typename T>
        bool operator==(T data);
        operator libab_ref*() {
            return &ref;
        }
        ~abacus_ref();
};

template <typename T>
T* get(libab_ref* ref) {
    return (T*) libab_ref_get(ref);
}

abacus_ref::abacus_ref() {
    libab_ref_null(&ref);
}

abacus_ref::abacus_ref(void* data, void (*free_func)(void*)) {
    libab_ref_new(&ref, data, free_func);
}

abacus_ref::abacus_ref(const abacus_ref& other) {
    libab_ref_copy(&other.ref, &ref);
}

abacus_ref& abacus_ref::operator=(const abacus_ref& other) {
    libab_ref_copy(&other.ref, &ref);
    return *this;
}

abacus_ref& abacus_ref::operator=(std::nullptr_t t) {
    libab_ref_free(&ref);
    libab_ref_null(&ref);
    return *this;
}

template <typename T>
bool abacus_ref::operator==(T data) {
    return libab_ref_get(&ref) == (void*) data;
}

template <>
bool abacus_ref::operator==<std::nullptr_t>(std::nullptr_t t) {
    return libab_ref_get(&ref) == t;
}

abacus_ref::~abacus_ref() {
    libab_ref_free(&ref);
}
