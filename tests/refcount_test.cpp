#include "refcount_test.hpp"

greatest_test_res ref_new_is_strong() {
    libab_ref ref;
    int* malloced_value = new int;
    auto free_function = [](void* to_free) { delete (int*) to_free; };
    
    libab_ref_new(&ref, malloced_value, free_function);
    ASSERT_EQm("Newly allocated reference is not strong!", ref.strong, 1);
    libab_ref_free(&ref);
    PASS();
}
greatest_test_res ref_new_no_gc() {
    libab_ref ref;
    int* malloced_value = new int;
    auto free_function = [](void* to_free) { delete (int*) to_free; };

    libab_ref_new(&ref, malloced_value, free_function);
    ASSERT_FALSE(ref.count == NULL);
    ASSERT_EQ(ref.count->prev, NULL);
    ASSERT_EQ(ref.count->next, NULL);
    ASSERT_EQ(ref.count->visit_children, NULL);
    libab_ref_free(&ref);
    PASS();
}
greatest_test_res ref_new_free_func() {
    libab_ref ref;
    int* malloced_value = new int;
    auto free_function = [](void* to_free) { delete (int*) to_free; };

    libab_ref_new(&ref, malloced_value, free_function);
    ASSERT_EQm("Newly allocated reference has wrong free function", ref.count->free_func, free_function);
    libab_ref_free(&ref);
    PASS();
}
greatest_test_res ref_new_not_null() {
    libab_ref ref;
    int* malloced_value = new int;
    auto free_function = [](void* to_free) { delete (int*) to_free; };

    libab_ref_new(&ref, malloced_value, free_function);
    ASSERT_EQm("Newly allocated reference doesn't hold correct value", ref.count->data, malloced_value);
    libab_ref_free(&ref);
    PASS();
}

SUITE(refcount_suite) {
    RUN_TEST(ref_new_is_strong);
    RUN_TEST(ref_new_no_gc);
    RUN_TEST(ref_new_free_func);
    RUN_TEST(ref_new_not_null);
}
