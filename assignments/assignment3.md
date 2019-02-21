# Assignment 2: Unit Testing

Due February 5, 2019 at 11:59 PM

Team Name: nullptr

Team Members: Ryan Alder, Danila Fedorin, Matthew Sessions

Github Project Fork URL: [https://github.com/sessionm21/abcs](https://github.com/sessionm21/abcs)

## Sections Reviewed:
Assignment 3: Code Review
Due February 21, 2019 at 11:59 PM

There should be at least one File/Section per team member. Each section should be 200 ~ 400 lines. Delete extraneous headers. You should identify any faults or “code smells” you find and include suggested fixes. For each section, briefly justify why you chose that section to review. 

### Section 1 (parser.c, Matthew Sessions)
Why is this in a do/while for 0

parser.c line 23
```
/* Utilities */
#define PARSE_CHILD(result, state, parse_function, parse_into, into)           \
    do {                                                                       \
        result = parse_function(state, &parse_into);                           \
        if (result == LIBAB_SUCCESS) {                                         \
            result = libab_convert_ds_result(vec_add(into, parse_into));       \
            if (result != LIBAB_SUCCESS) {                                     \
                libab_tree_free_recursive(parse_into);                         \
            }                                                                  \
        }                                                                      \
    } while (0);
```


va\_list args is not used within function, used for function signature

parser.c line 43
```
int _parser_foreach_free_tree(void* data, va_list args) {
    libab_tree_free_recursive(data);
    return 0;
}
```

Init function could be higher up in source file, linked list has a typedef but parser_state does not

parser.c line 61
```
void _parser_state_init(struct parser_state* state, ll* tokens,
                        const char* string, libab_table* table) 
```

Would these be better as macros?

parser.c line 77
```
int _parser_is_char(struct parser_state* state, char to_expect) {
    return (state->current_match && state->current_match->type == TOKEN_CHAR &&
            state->string[state->current_match->from] == to_expect);
}
int _parser_is_type(struct parser_state* state, libab_lexer_token to_expect) {
    return (state->current_match && state->current_match->type == to_expect);
}

int _parser_eof(struct parser_state* state) {
    return state->current_match == NULL;
}
```

Kinda weird to have function prototypes partway down the file

parser.c line 118
```
libab_result _parse_block(struct parser_state*, libab_tree**, int);
libab_result _parse_expression(struct parser_state* state,
                               libab_tree** store_into);
libab_result _parse_type(struct parser_state* state, libab_ref* ref);
```

### Section 2 (interpreter.c, Danila Fedorin)

### Section 3 (gc.c, Ryan Alder)
Function libab_gc_visit_children is never actually used

gc.c lines 17-19
```
void libab_gc_visit_children(libab_ref* ref, libab_visitor_function_ptr func, void* data) {
    if(!ref->null) _gc_count_visit_children(ref->count, func, data);
}
```

Unnecessary to declare list here, just pass data instead to \_libab_gc_list_append

gc.c lines 48-55
```
void _gc_save(libab_ref_count* count, void* data) {
    libab_gc_list* list = data;
    if(count->visit_children && count->gc >= 0) {
        count->gc = -1;
        _libab_gc_list_append(list, count);
        _gc_count_visit_children(count, _gc_save, data);
    }
}```
