#pragma once

#include "ref.hpp"
extern "C" {
#include "libabacus.h"
#include "table.h"
#include "value.h"
#include "util.h"
}

class abacus {
    private:
        libab ab;
        abacus_ref scope;
        std::map<std::string, abacus_ref> compiled_types;
        libab_basetype basetype_string = { [](void* s) { delete ((string*) s); }, NULL, 0 };
    public:
        abacus();
        void add_variable(const std::string& name, abacus_ref val);
        void add_function(const std::string& name, libab_function_ptr ptr, const std::string& type);
        void add_operator_infix(const std::string& op, const std::string& func, int assoc, int prec);
        void add_operator_prefix(const std::string& op, const std::string& func);
        void add_operator_postfix(const std::string& op, const std::string& func);
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

void abacus::add_variable(const std::string& name, abacus_ref val) {
    libab_table_entry* entry = libab_table_search_entry_value(get<libab_table>(scope), name.c_str());
    if(entry) {
        libab_ref_free(&entry->data_u.value);
        libab_ref_copy(val, &entry->data_u.value);
    } else {
        libab_put_table_value(get<libab_table>(scope), name.c_str(), std::move(val));
    }
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

void abacus::add_operator_infix(const std::string& op, const std::string& func, int assoc, int prec) {
    libab_register_operator_infix(&ab, op.c_str(), prec, assoc, func.c_str());
}
void abacus::add_operator_prefix(const std::string& op, const std::string& func) {
    libab_register_operator_prefix(&ab, op.c_str(), func.c_str());
}
void abacus::add_operator_postfix(const std::string& op, const std::string& func) {
    libab_register_operator_postfix(&ab, op.c_str(), func.c_str());
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
