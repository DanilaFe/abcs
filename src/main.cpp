#include <iostream>
#include <map>
#include <mpfr.h>
extern "C" {
#include "libabacus.h"
#include "value.h"
#include "util.h"
}

#define PRECISION 200

// == Global State (uh-oh)
bool close_requested = false;
long requested_precision = 3;

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
        mpfr_init2(value, PRECISION);
        mpfr_set_str(value, new_value, 10, MPFR_RNDN);
    }
    number(mpfr_t&& new_value) {
        std::swap(value, new_value);
    }
    number* operator+(const number& right) {
        mpfr_t new_value;
        mpfr_init2(new_value, 200);
        mpfr_add(new_value, value, right.value, MPFR_RNDN);
        return new number(std::move(new_value));
    }
    number* operator-(const number& right) {
        mpfr_t new_value;
        mpfr_init2(new_value, 200);
        mpfr_sub(new_value, value, right.value, MPFR_RNDN);
        return new number(std::move(new_value));
    }
    number* operator*(const number& right) {
        mpfr_t new_value;
        mpfr_init2(new_value, 200);
        mpfr_mul(new_value, value, right.value, MPFR_RNDN);
        return new number(std::move(new_value));
    }
    number* operator/(const number& right) {
        mpfr_t new_value;
        mpfr_init2(new_value, 200);
        mpfr_div(new_value, value, right.value, MPFR_RNDN);
        return new number(std::move(new_value));
    }
    int to_int() {
        return mpfr_get_si(value, MPFR_RNDN);
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
        template <typename T>
        bool operator==(T data);
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
    char* str = mpfr_get_str(NULL, &exp, 10, requested_precision, num->value, MPFR_RNDN);
    std::string output_string = std::string(str).insert((mpfr_sgn(num->value) < 0) ? 2 : 1, 1, '.');
    if(exp != 1) {
        output_string += "e";
        output_string += std::to_string(exp - 1);
    }
    abacus_ref value = create_value(ab, new string(std::move(output_string)));
    libab_ref_copy(value, into);
    mpfr_free_str(str);
    return LIBAB_SUCCESS;
}

FUNCTION(to_string_bool) {
    int* val = (int*) libab_unwrap_param(params, 0);
    abacus_ref value = create_value(ab, new string(*val ? "true" : "false"));
    libab_ref_copy(value, into);
    return LIBAB_SUCCESS;
}

FUNCTION(to_string_unit) {
    abacus_ref value = create_value(ab, new string("()"));
    libab_ref_copy(value, into);
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

FUNCTION(negate) {
    number* value = (number*) libab_unwrap_param(params, 0);
    mpfr_t negative;
    mpfr_init2(negative, PRECISION);
    mpfr_neg(negative, value->value, MPFR_RNDN);
    abacus_ref to_return = create_value<number>(ab, new number(std::move(negative)));
    libab_ref_copy(to_return, into);
    return LIBAB_SUCCESS;
}

FUNCTION(quit) {
    close_requested = true;
    libab_get_unit_value(ab, into);
    return LIBAB_SUCCESS;
}

FUNCTION(request_precision) {
    number* value = (number*) libab_unwrap_param(params, 0);
    requested_precision = std::min(PRECISION / 4, std::max(2, value->to_int()));
    libab_get_unit_value(ab, into);
    return LIBAB_SUCCESS;
}

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
        template <typename ... Ts>
        abacus_ref call(const std::string& bane, Ts...params);
        std::string to_string(abacus_ref& value);
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

template <typename ... Ts>
abacus_ref abacus::call(const std::string& name, Ts...params) {
    abacus_ref value;
    libab_run_function_scoped(&ab, name.c_str(), scope, value, sizeof...(params), (libab_ref*) params...);
    libab_gc_run(&ab.containers);
    return value;
}


template <typename T>
T* get(libab_ref* ref) {
    return (T*) libab_ref_get(ref);
}

std::string abacus::to_string(abacus_ref& val) {
    abacus_ref string_value = call("to_string", val);
    if(string_value == nullptr) return "Unable to convert to string.";
    libab_basetype* base = get<libab_parsetype>(&get<libab_value>(string_value)->type)->data_u.base;
    if(base != &basetype_string) return "\"to_string\" did not return string.";
    return get<string>(&get<libab_value>(string_value)->data)->value;
}

abacus::~abacus() {
    scope = nullptr;
    libab_free(&ab);
}
int main() {
    abacus ab;
    std::string buffer;

    ab.add_function("quit", function_quit, "()->unit");
    ab.add_function("request_precision", function_request_precision, "(num)->unit");

    ab.add_function("print", function_print_string, "(str)->unit");
    ab.add_function("to_string", function_to_string_num, "(num)->str");
    ab.add_function("to_string", function_to_string_bool, "(bool)->str");
    ab.add_function("to_string", function_to_string_unit, "(unit)->str");

    ab.add_function("plus", function_plus, "(num, num)->num");
    ab.add_function("minus", function_minus, "(num, num)->num");
    ab.add_function("times", function_times, "(num, num)->num");
    ab.add_function("divide", function_divide, "(num, num)->num");
    ab.add_function("negate", function_negate, "(num)->num");

    while(!close_requested) {
        std::cout << "> ";
        std::getline(std::cin, buffer);
        abacus_ref value = ab.run(buffer);
        if(value == nullptr) {
            std::cout << "Invalid expression." << std::endl;
        } else {
            std::cout << ab.to_string(value) << std::endl;
        }
    }
}
