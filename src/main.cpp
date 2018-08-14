#include <iostream>
#include <map>
#include <mpfr.h>
extern "C" {
#include "libabacus.h"
#include "util.h"
}

// == STRING Type
struct string {
    std::string value;
    string(std::string&& new_value) : value(std::move(new_value)) {}
    string(const std::string& new_value) : value(new_value) {}
};

// MPFR Type
struct number {
    mpfr_t value;
    number(const char* new_value) {
        mpfr_init_set_str(value, new_value, 10, MPFR_RNDN);
    }
    number(mpfr_t&& new_value) {
        std::swap(value, new_value);
    }
    number* operator+(const number& right) {
        mpfr_t new_value;
        mpfr_init(new_value);
        mpfr_add(new_value, value, right.value, MPFR_RNDN);
        return new number(std::move(new_value));
    }
    number* operator-(const number& right) {
        mpfr_t new_value;
        mpfr_init(new_value);
        mpfr_sub(new_value, value, right.value, MPFR_RNDN);
        return new number(std::move(new_value));
    }
    number* operator*(const number& right) {
        mpfr_t new_value;
        mpfr_init(new_value);
        mpfr_mul(new_value, value, right.value, MPFR_RNDN);
        return new number(std::move(new_value));
    }
    number* operator/(const number& right) {
        mpfr_t new_value;
        mpfr_init(new_value);
        mpfr_div(new_value, value, right.value, MPFR_RNDN);
        return new number(std::move(new_value));
    }
    ~number() {
        mpfr_clear(value);
    }
};

// == Reference Wrapper 
class abacus_ref {
    private:
        libab_ref ref;
    public:
        abacus_ref();
        abacus_ref(void* data, void (*free_func)(void*));
        abacus_ref(const abacus_ref& other);
        abacus_ref& operator=(const abacus_ref& other);
        abacus_ref& operator=(std::nullptr_t);
        operator libab_ref*() {
            return &ref;
        }
        ~abacus_ref();
};

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

abacus_ref::~abacus_ref() {
    libab_ref_free(&ref);
}
//
// == BASIC FUNCTIONS
#define FUNCTION(name) libab_result function_##name(libab* ab, libab_ref* scope, libab_ref_vec* params, libab_ref* into)

template <typename T>
abacus_ref create_value(libab* ab, T* val);

template <>
abacus_ref create_value<string>(libab* ab, string* param) {
    abacus_ref type;
    abacus_ref value;
    libab_create_type(ab, type, "str");
    libab_create_value_raw(ab, value, (void*) param, type);
    return value;
}

template <>
abacus_ref create_value<number>(libab* ab, number* param) {
    abacus_ref type;
    abacus_ref value;
    libab_get_type_num(ab, type);
    libab_create_value_raw(ab, value, (void*) param, type);
    return value;
}

FUNCTION(print_string) {
    string* param = (string*) libab_unwrap_param(params, 0);
    std::cout << param->value << std::endl;
    libab_get_unit_value(ab, into);
    return LIBAB_SUCCESS;
}

FUNCTION(to_string_num) {
    number* num = (number*) libab_unwrap_param(params, 0);
    mpfr_exp_t exp;
    char* str = mpfr_get_str(NULL, &exp, 10, 0, num->value, MPFR_RNDN);
    abacus_ref value = create_value(ab, new string(std::string(str).insert(exp, 1, '.')));
    libab_ref_copy(value, into);
    mpfr_free_str(str);
    return LIBAB_SUCCESS;
}

#define OPERATOR_FUNCTION(name, op) FUNCTION(name) { \
    number* left = (number*) libab_unwrap_param(params, 0); \
    number* right = (number*) libab_unwrap_param(params, 1); \
    abacus_ref value = create_value(ab, *left op *right); \
    libab_ref_copy(value, into); \
    return LIBAB_SUCCESS; \
}

OPERATOR_FUNCTION(plus, +)
OPERATOR_FUNCTION(minus, -)
OPERATOR_FUNCTION(times, *)
OPERATOR_FUNCTION(divide, /)

// == Main class
class abacus {
    private:
        libab ab;
        abacus_ref scope;
        std::map<std::string, abacus_ref> compiled_types;
        libab_basetype basetype_string = { [](void* s) { delete ((string*) s); }, NULL, 0 };
    public:
        abacus();
        void add_function(const std::string& name, libab_function_ptr ptr, const std::string& type);
        abacus_ref run(const std::string& code);
        ~abacus();
};

abacus::abacus() {
    auto parse_function = [](const char* s) {
        return (void*) new number(s);
    };
    auto free_function = [](void* num) {
        delete ((number*) num);
    };
    libab_init(&ab, parse_function, free_function);
    libab_register_basetype(&ab, "str", &basetype_string);
    libab_create_table(&ab, scope, &ab.table);
}

void abacus::add_function(const std::string& name, libab_function_ptr ptr, const std::string& type) {
    if(compiled_types.find(type) != compiled_types.end()) {
        libab_register_function(&ab, name.c_str(), compiled_types[type], ptr);
    } else {
        abacus_ref& new_ref = compiled_types[type];
        libab_create_type(&ab, new_ref, type.c_str());
        libab_register_function(&ab, name.c_str(), new_ref, ptr);
    }
}

abacus_ref abacus::run(const std::string& code) {
    abacus_ref value;
    libab_run_scoped(&ab, code.c_str(), scope, value);
    libab_gc_run(&ab.containers);
    return value;
}

abacus::~abacus() {
    scope = nullptr;
    libab_free(&ab);
}

int main() {
    abacus ab;
    ab.add_function("print", function_print_string, "(str)->unit");
    ab.add_function("to_string", function_to_string_num, "(num)->str");
    ab.add_function("plus", function_plus, "(num, num)->num");
    ab.add_function("minus", function_minus, "(num, num)->num");
    ab.add_function("times", function_times, "(num, num)->num");
    ab.add_function("divide", function_divide, "(num, num)->num");
    ab.run("fun test(): unit {}; print(to_string(plus(1, times(3, 3))))");
}
