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

greatest_test_res test_run() {
  // would also benefit from a test with valgrind
  libab_gc_list list;
  libab_gc_list_init(&list);
  libab_ref_count* ref1 = new libab_ref_count;

  ref1->data = new int[5];
  ref1->free_func = helper_free;
  ref1->strong = 0;
  ref1->weak = 0;
  libab_ref_count* before;
  if(ref1->next) ref1->next->prev = ref1->prev;
  if(ref1->prev) ref1->prev->next = ref1->next;
  before = &list.tail_sentinel;
  ref1->next = before;
  ref1->prev = before->prev;
  before->prev->next = ref1;
  before->prev = ref1;
  libab_gc_run(&list);

  ASSERT_EQ(list.head_sentinel.next, &list.tail_sentinel);
  ASSERT_EQ(list.tail_sentinel.prev, &list.head_sentinel);

  libab_gc_list_init(&list);
  libab_gc_list list1;
  libab_gc_list_init(&list1);
  libab_ref_count* ref2 = new libab_ref_count;

  ref2->data = NULL;
  ref2->free_func = NULL;
  ref2->strong = 0;
  ref2->weak = 0;
  libab_ref_count* before1;
  if(ref2->next) ref2->next->prev = ref2->prev;
  if(ref2->prev) ref2->prev->next = ref2->next;
  before1 = &list1.tail_sentinel;
  ref2->next = before1;
  ref2->prev = before1->prev;
  before1->prev->next = ref2;
  before1->prev = ref2;
  libab_gc_run(&list1);

  ASSERT_EQ(list.head_sentinel.next, &list.tail_sentinel);
  ASSERT_EQ(list.tail_sentinel.prev, &list.head_sentinel);

  PASS();
}

SUITE(gc_suite) {
  RUN_TEST(list_init);
  RUN_TEST(add_node);
  RUN_TEST(test_run);
}
