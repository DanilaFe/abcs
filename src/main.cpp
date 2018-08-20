#include <iostream>
#include <map>
#include <mpfr.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fstream>
#include <sstream>
#include "types.hpp"
#include "ref.hpp"
#include "functions.hpp"
#include "abacus.hpp"
extern "C" {
#include "libabacus.h"
#include "table.h"
#include "value.h"
#include "util.h"
}

// == Global State (uh-oh)
bool close_requested = false;
long requested_precision = 3;

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

void run_rc(abacus& ab) {
    std::ifstream rcfile("./.abcsrc");
    std::ostringstream str;
    if(rcfile.good()) {
        str << rcfile.rdbuf();
        ab.run(str.str());
    }
}

int main() {
    abacus ab;
    rl_bind_key('\t', rl_insert);
    size_t index = 0;

    ab.add_function("quit", function_quit, "()->unit");
    ab.add_function("request_precision", function_request_precision, "(num)->unit");

    ab.add_function("print", function_print_string, "(str)->unit");
    ab.add_function("to_string", function_to_string_num, "(num)->str");
    ab.add_function("to_string", function_to_string_bool, "(bool)->str");
    ab.add_function("to_string", function_to_string_unit, "(unit)->str");

    ab.add_function("lt", function_lt, "(num, num)->num");
    ab.add_function("lte", function_lte, "(num, num)->num");
    ab.add_function("equals", function_equals, "(num, num)->num");
    ab.add_function("gt", function_gt, "(num, num)->num");
    ab.add_function("gte", function_gte, "(num, num)->num");
    ab.add_function("plus", function_plus, "(num, num)->num");
    ab.add_function("minus", function_minus, "(num, num)->num");
    ab.add_function("times", function_times, "(num, num)->num");
    ab.add_function("divide", function_divide, "(num, num)->num");
    ab.add_function("pow", function_pow, "(num, num)->num");
    ab.add_function("negate", function_negate, "(num)->num");
    ab.add_function("factorial", function_factorial, "(num)->num");

    ab.add_function("ln", function_ln, "(num)->num");
    ab.add_function("exp", function_exp, "(num)->num");
    ab.add_function("sqrt", function_sqrt, "(num)->num");

    ab.add_function("sin", function_sin, "(num)->num");
    ab.add_function("cos", function_cos, "(num)->num");
    ab.add_function("tan", function_tan, "(num)->num");

    ab.add_function("arcsin", function_arcsin, "(num)->num");
    ab.add_function("arccos", function_arccos, "(num)->num");
    ab.add_function("arctan", function_arctan, "(num)->num");

    ab.add_operator_infix("<", "lt", -1, 1);
    ab.add_operator_infix("<=", "lte", -1, 1);
    ab.add_operator_infix("==", "equals", -1, 1);
    ab.add_operator_infix(">", "gt", -1, 1);
    ab.add_operator_infix(">=", "gte", -1, 1);
    ab.add_operator_infix("+", "plus", -1, 2);
    ab.add_operator_infix("-", "minus", -1, 2);
    ab.add_operator_infix("*", "times", -1, 3);
    ab.add_operator_infix("/", "divide", -1, 3);
    ab.add_operator_infix("^", "pow", 1, 3);
    ab.add_operator_prefix("-", "negate");
    ab.add_operator_postfix("!", "factorial");

    run_rc(ab);
    while(!close_requested) {
        char* data = readline(" > ");
        std::string buffer(data);
        add_history(data);
        free(data);

        ref value = ab.run(buffer);
        if(value == nullptr) {
            std::cout << "Invalid expression." << std::endl;
        } else {
            std::string name = "r" + std::to_string(index);
            std::string ans = "ans";
            ab.add_variable(name, value);
            ab.add_variable(ans, value);
            index++;

            std::cout << name << " = " << ab.to_string(value) << std::endl;
        }
    }
}
