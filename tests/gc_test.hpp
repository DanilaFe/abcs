extern "C" {
#include "gc.h"
#include "refcount.h"
}
#include "greatest.h"

greatest_test_res list_init();
greatest_test_res add_node();
void helper_free(void* data);
greatest_test_res test_run();
void dummy_visit(void* self, libab_visitor_function_ptr f, void*);

void gc_suite();
