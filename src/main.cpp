#include <iostream>
#include <map>
#include <mpfr.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "types.hpp"
#include "ref.hpp"
#include "repl_functions.hpp"
#include "string_functions.hpp"
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
abacus abcs;

FUNCTION(quit) {
    close_requested = true;
    libab_get_unit_value(ab, into);
    return LIBAB_SUCCESS;
}

FUNCTION(reload_rc) {
    abcs.add_rc("./.abcsrc");
    libab_get_unit_value(ab, into);
    return LIBAB_SUCCESS;
}

FUNCTION(request_precision) {
    number* value = (number*) libab_unwrap_param(params, 0);
    requested_precision = std::min(PRECISION / 4, std::max(2, value->to_int()));
    libab_get_unit_value(ab, into);
    return LIBAB_SUCCESS;
}

int main() {
    rl_bind_key('\t', rl_insert);
    size_t index = 0;
    
    abcs.add_standard();
    abcs.add_function("quit", function_quit, "()->unit");
    abcs.add_function("request_precision", function_request_precision, "(num)->unit");
    abcs.add_function("reload_rc", function_reload_rc, "()->unit");

    abcs.add_function("print", function_print_string, "(str)->unit");
    abcs.add_function("to_string", function_to_string_num, "(num)->str");
    abcs.add_function("to_string", function_to_string_bool, "(bool)->str");
    abcs.add_function("to_string", function_to_string_unit, "(unit)->str");

    abcs.add_rc("./.abcsrc");
    while(!close_requested) {
        char* data = readline(" > ");
        std::string buffer(data);
        add_history(data);
        free(data);

        ref value = abcs.run(buffer);
        if(value == nullptr) {
            std::cout << "Invalid expression." << std::endl;
        } else {
            std::string name = "r" + std::to_string(index);
            std::string ans = "ans";
            abcs.add_variable(name, value);
            abcs.add_variable(ans, value);
            index++;

            std::cout << name << " = " << abcs.to_string(value) << std::endl;
        }
    }
}
