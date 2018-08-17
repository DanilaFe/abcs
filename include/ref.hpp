#pragma once

#include <cstddef>
extern "C" {
#include "libabacus.h"
#include "util.h"
}

class ref {
    private:
        libab_ref val;
    public:
        ref();
        ref(void* data, void (*free_func)(void*));
        ref(const ref& other);
        ref& operator=(const ref& other);
        ref& operator=(std::nullptr_t);
        template <typename T>
        bool operator==(T data);
        operator libab_ref*();
        ~ref();
};

template <typename T>
T* get(libab_ref* ref) {
    return (T*) libab_ref_get(ref);
}


template <typename T>
bool ref::operator==(T data) {
    return libab_ref_get(&val) == (void*) data;
}

template <>
bool ref::operator==<std::nullptr_t>(std::nullptr_t t);
