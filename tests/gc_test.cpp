#include "gc_test.hpp"

greatest_test_res list_init() {
  libab_gc_list list;
  libab_gc_list_init(&list);
  ASSERT_EQ(list.head_sentinel.next, &list.tail_sentinel);
  ASSERT_EQ(list.tail_sentinel.prev, &list.head_sentinel);
  PASS();
}

greatest_test_res add_node() {
  libab_ref ref;
  libab_gc_list list;
  libab_gc_list_init(&list);
  libab_ref_count* node = new libab_ref_count;
  ref.count = node;
  libab_gc_add(&ref, NULL, &list);
  ASSERT_EQ(list.head_sentinel.next, node);
  ASSERT_EQ(list.tail_sentinel.prev, node);
  ASSERT_EQ(node->next, &list.tail_sentinel);
  ASSERT_EQ(node->prev, &list.head_sentinel);
  ASSERT_EQ(ref.count->visit_children, NULL);
  delete node;
  PASS();
}

void helper_free(void* data) {
  delete [] (int*)data;
}

void dummy_visit(void* self, libab_visitor_function_ptr f, void*) {
   // nothing
}

greatest_test_res test_run() {
  // would also benefit from a test with valgrind
  libab_gc_list list;
  libab_gc_list_init(&list);
  libab_ref ref1;
  libab_ref_new(&ref1, new int[5], helper_free);
  libab_gc_add(&ref1, dummy_visit, &list);

  libab_gc_run(&list);

  ASSERT_EQ(list.head_sentinel.next, &list.tail_sentinel);
  ASSERT_EQ(list.tail_sentinel.prev, &list.head_sentinel);
  libab_ref_free(&ref1);

  PASS();
}

SUITE(gc_suite) {
  RUN_TEST(list_init);
  RUN_TEST(add_node);
  RUN_TEST(test_run);
}
