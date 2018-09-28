#pragma once

#include "ref.hpp"
#include "types.hpp"
#include <string>
#include <map>
extern "C" {
#include "libabacus.h"
#include "table.h"
#include "value.h"
#include "util.h"
}

class abacus {
    private:
        libab ab;
        ref scope;
        ref rc_scope;
        std::map<std::string, ref> compiled_types;
        libab_basetype basetype_string = { [](void* s) { delete ((string*) s); }, NULL, 0 };
    public:
        abacus();
        const libab_basetype* get_basetype_string();
        void add_variable(const std::string& name, ref val);
        void add_function(const std::string& name, libab_function_ptr ptr, const std::string& type);
        void add_operator_infix(const std::string& op, const std::string& func, int assoc, int prec);
        void add_operator_prefix(const std::string& op, const std::string& func);
        void add_operator_postfix(const std::string& op, const std::string& func);
        void add_standard();
        void add_rc(const std::string& file);
        ref run(const std::string& code);
        template <typename ... Ts>
        ref call(const std::string& bane, Ts...params);
        std::string to_string(ref& value);
        ~abacus();
};

template <typename ... Ts>
ref abacus::call(const std::string& name, Ts...params) {
    ref value;
    libab_call_function_scoped(&ab, name.c_str(), scope, value, sizeof...(params), (libab_ref*) params...);
    libab_gc_run(&ab.containers);
    return value;
}
