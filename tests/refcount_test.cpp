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

greatest_test_res ref_copy_value() {
    libab_ref a;
    libab_ref b;
    int times_freed = 0;
    auto free_function = [](void* to_free) { int* cast = (int*) to_free; (*cast)++; };

    libab_ref_new(&a, &times_freed, free_function);
    ASSERT_EQ(times_freed, 0);
    libab_ref_copy(&a, &b);
    ASSERT_EQ(times_freed, 0);
    libab_ref_free(&a);
    ASSERT_EQ(times_freed, 0);
    libab_ref_free(&b);
    ASSERT_EQ(times_freed, 1);
    PASS();
}
greatest_test_res ref_copy_null() {
    libab_ref a;
    libab_ref b;

    libab_ref_null(&a);
    libab_ref_copy(&a, &b);
    ASSERT_EQ(b.null, 1);
    libab_ref_free(&a);
    libab_ref_free(&b);
    PASS();
}

greatest_test_res ref_swap() {
    libab_ref a;
    libab_ref b;
    int* first_value = new int;
    int* second_value = new int;
    auto free_function = [](void* to_free) { delete (int*) to_free; };

    libab_ref_new(&a, first_value, free_function);
    libab_ref_new(&b, second_value, free_function);
    libab_ref_swap(&a, &b);
    ASSERT_EQ(libab_ref_get(&a), second_value);
    ASSERT_EQ(libab_ref_get(&b), first_value);
    libab_ref_free(&a);
    libab_ref_free(&b);
    PASS();
}

greatest_test_res ref_get() {
    libab_ref ref;
    int* malloced_value = new int;
    auto free_function = [](void* to_free) { delete (int*) to_free; };

    libab_ref_new(&ref, malloced_value, free_function);
    ASSERT_EQ(libab_ref_get(&ref), malloced_value);
    libab_ref_free(&ref);
    PASS();
}

SUITE(refcount_suite) {
    RUN_TEST(ref_new_is_strong);
    RUN_TEST(ref_new_no_gc);
    RUN_TEST(ref_new_free_func);
    RUN_TEST(ref_new_not_null);
    RUN_TEST(ref_copy_value);
    RUN_TEST(ref_copy_null);
    RUN_TEST(ref_swap);
    RUN_TEST(ref_get);
}
