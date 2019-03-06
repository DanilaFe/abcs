#include "trie_test.hpp"

greatest_test_res test_trie_init() {
  libab_trie trie;
  libab_trie_init(&trie);
  ASSERT_EQ(trie.head, NULL);
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);
  PASS();
}

greatest_test_res test_trie_clear() {
  libab_trie trie;
  libab_trie_init(&trie);

  trie.empty_list.head= (ll_node_s*)malloc(sizeof(libab_trie_node));
  trie.empty_list.tail= (ll_node_s*)malloc(sizeof(libab_trie_node));
  trie.head = (libab_trie_node_s*)malloc(sizeof(libab_trie_node_s));

  libab_trie_clear(&trie);

  ASSERT_EQ(trie.head, NULL);
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);

  libab_trie_init(&trie);

  trie.head = 0;
  trie.empty_list.head = NULL;
  trie.empty_list.tail = NULL;

  ASSERT_EQ(libab_trie_put(&trie, "derp", &trie), LIBAB_SUCCESS);

  GREATEST_ASSERT(trie.head != NULL);  
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);

  ASSERT_EQ(libab_trie_put(&trie, "doge", &trie), LIBAB_SUCCESS);

  GREATEST_ASSERT(trie.head != NULL);  
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);

  libab_trie_clear(&trie);

  ASSERT_EQ(trie.head, NULL);
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);
  
  PASS();
}

greatest_test_res test_trie_put_get() {

  libab_trie trie;

  libab_trie_init(&trie);

  trie.head = 0;
  trie.empty_list.head = NULL;
  trie.empty_list.tail = NULL;

  ASSERT_EQ(libab_trie_put(&trie, "derp", &trie), LIBAB_SUCCESS);

  GREATEST_ASSERT(trie.head != NULL);  
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);

  ASSERT_EQ(libab_trie_put(&trie, "doge", &trie), LIBAB_SUCCESS);

  GREATEST_ASSERT(trie.head != NULL);  
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);
   
  libab_trie *ptr = (libab_trie*)libab_trie_get(&trie, "doge"); 
  ASSERT_EQ(ptr->empty_list.head->data, &trie);

  ptr  = (libab_trie*)libab_trie_get(&trie, "derp");
  libab_trie_clear(&trie);

  PASS();
}

SUITE(trie_suite) {
  RUN_TEST(test_trie_init);
  RUN_TEST(test_trie_clear);
  RUN_TEST(test_trie_put_get);
}
