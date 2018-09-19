#include "abacus.hpp"
#include "functions.hpp"

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

const libab_basetype* abacus::get_basetype_string() {
    return &basetype_string;
}

void abacus::add_variable(const std::string& name, ref val) {
    libab_set_variable((libab_table*) libab_ref_get(&ab.table), name.c_str(), val);
}

void abacus::add_function(const std::string& name, libab_function_ptr ptr, const std::string& type) {
    if(compiled_types.find(type) != compiled_types.end()) {
        libab_register_function(&ab, name.c_str(), compiled_types[type], ptr);
    } else {
        ref& new_ref = compiled_types[type];
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

void abacus::add_standard() {
    add_function("lt", function_lt, "(num, num)->num");
    add_function("lte", function_lte, "(num, num)->num");
    add_function("equals", function_equals, "(num, num)->num");
    add_function("gt", function_gt, "(num, num)->num");
    add_function("gte", function_gte, "(num, num)->num");
    add_function("plus", function_plus, "(num, num)->num");
    add_function("minus", function_minus, "(num, num)->num");
    add_function("times", function_times, "(num, num)->num");
    add_function("divide", function_divide, "(num, num)->num");
    add_function("pow", function_pow, "(num, num)->num");
    add_function("negate", function_negate, "(num)->num");
    add_function("factorial", function_factorial, "(num)->num");

    add_function("ln", function_ln, "(num)->num");
    add_function("exp", function_exp, "(num)->num");
    add_function("sqrt", function_sqrt, "(num)->num");

    add_function("sin", function_sin, "(num)->num");
    add_function("cos", function_cos, "(num)->num");
    add_function("tan", function_tan, "(num)->num");

    add_function("arcsin", function_arcsin, "(num)->num");
    add_function("arccos", function_arccos, "(num)->num");
    add_function("arctan", function_arctan, "(num)->num");

    add_operator_infix("<", "lt", -1, 1);
    add_operator_infix("<=", "lte", -1, 1);
    add_operator_infix("==", "equals", -1, 1);
    add_operator_infix(">", "gt", -1, 1);
    add_operator_infix(">=", "gte", -1, 1);
    add_operator_infix("+", "plus", -1, 2);
    add_operator_infix("-", "minus", -1, 2);
    add_operator_infix("*", "times", -1, 3);
    add_operator_infix("/", "divide", -1, 3);
    add_operator_infix("^", "pow", 1, 3);
    add_operator_prefix("-", "negate");
    add_operator_postfix("!", "factorial");
}

ref abacus::run(const std::string& code) {
    ref value;
    libab_run_scoped(&ab, code.c_str(), scope, value);
    libab_gc_run(&ab.containers);
    return value;
}

std::string abacus::to_string(ref& val) {
    ref string_value = call("to_string", val);
    if(string_value == nullptr) return "Unable to convert to string.";
    libab_basetype* base = get<libab_parsetype>(&get<libab_value>(string_value)->type)->data_u.base;
    if(base != &basetype_string) return "\"to_string\" did not return string.";
    return get<string>(&get<libab_value>(string_value)->data)->value;
}

abacus::~abacus() {
    scope = nullptr;
    libab_free(&ab);
}
