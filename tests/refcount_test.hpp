extern "C" {
#include "refcount.h"
}
#include "greatest.h"

greatest_test_res ref_new_is_strong();
greatest_test_res ref_new_no_gc();
greatest_test_res ref_new_free_func();
greatest_test_res ref_new_not_null();

void refcount_suite();
