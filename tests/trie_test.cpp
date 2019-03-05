#include "trie_test.hpp"


greatest_test_res trie_init() {
  libab_trie trie;
  libab_trie_init(&trie);
  ASSERT_EQ(trie.head, NULL);
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);
  PASS();
}
/*
void libab_trie_init(libab_trie* trie);
libab_result libab_trie_put(libab_trie* trie, const char* key, void* value);
const ll* libab_trie_get(const libab_trie* trie, const char* key);
int libab_trie_foreach(const libab_trie* trie, void* data, compare_func compare,
                       foreach_func foreach);
void libab_trie_clear(libab_trie* trie);
void libab_trie_free(libab_trie* trie);
*/

greatest_test_res trie_clear() {
  libab_trie trie;
  libab_trie_init(&trie);

  trie.empty_list.head= (ll_node_s*)malloc(sizeof(libab_trie_node));
  trie.empty_list.tail= (ll_node_s*)malloc(sizeof(libab_trie_node));
  trie.head = (libab_trie_node_s*)malloc(sizeof(libab_trie_node_s));

  libab_trie_clear(&trie);

  ASSERT_EQ(trie.head, NULL);
  ASSERT_EQ(trie.empty_list.head, NULL);
  ASSERT_EQ(trie.empty_list.tail, NULL);
}

greatest_test_res trie_put_get() {
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

  libab_trie *derp = (libab_trie*)libab_trie_get(&trie, "doge");
 
  ASSERT_EQ(derp->empty_list.head->data, &trie);

  PASS();
}

SUITE(trie_suite) {
  RUN_TEST(trie_init);
  RUN_TEST(trie_clear);
  RUN_TEST(trie_put_get);
  //RUN_TEST(add_node);
  //RUN_TEST(test_run);
}
