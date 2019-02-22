# Assignment 2: Unit Testing

Due February 5, 2019 at 11:59 PM

Team Name: nullptr

Team Members: Ryan Alder, Danila Fedorin, Matthew Sessions

Github Project Fork URL: [https://github.com/sessionm21/abcs](https://github.com/sessionm21/abcs)

## Sections Reviewed
### Section 1 (parser.c, Matthew Sessions) 
_Lines completed: 0-400 of 1182_

__Purpose:__ This section was revied because it is critically important to the
final program. Since all data fed into abcs goes through the parser
before any processing happens, it's highly important that all code here
is of high quality. We have found no significant defects, although
some issues with code style have been pointed out.

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

Init function could be higher up in source file, linked list has a typedef but parser\_state does not

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

### Section 2 (parser.c, Matthew Sessions) 
_Lines completed: 400-800 of 1182_

__Purpose:__ The same as section 1. However, because code reviews
should not be longer than 400 lines at a time as per the IBM guidelines,
this review was conducted some time after the initial review.

These functions do effectively the same thing, could use a macro to define these functions
parser.c line 409
```
libab_result _parse_true(struct parser_state* state, libab_tree** store_into) {
    libab_result result = _parser_consume_type(state, TOKEN_KW_TRUE);
    if(result == LIBAB_SUCCESS) {
        if ((*store_into = malloc(sizeof(**store_into)))) {
            (*store_into)->variant = TREE_TRUE;
        } else {
            result = LIBAB_MALLOC;
        }
    }

    if(result != LIBAB_SUCCESS) {
        *store_into = NULL;
    }
    return result;
}

libab_result _parse_false(struct parser_state* state, libab_tree** store_into) {
    libab_result result = _parser_consume_type(state, TOKEN_KW_FALSE);
    if(result == LIBAB_SUCCESS) {
        if ((*store_into = malloc(sizeof(**store_into)))) {
            (*store_into)->variant = TREE_FALSE;
        } else {
            result = LIBAB_MALLOC;
        }
    }

    if(result != LIBAB_SUCCESS) {
        *store_into = NULL;
    }
    return result;
}
```
Consume statements could be written as macros
parser.c 597, etc.
```
if (result == LIBAB_SUCCESS) {
    result = _parser_consume_char(state, '(');
}
```

### Section 3 (interpreter.c, Danila Fedorin)
_Lines completed: 1046-1284 of 1356_

__Purpose:__ The interpreter is the heart of libabacus, and
therefore the heart of abcs. It contains all of the code responsible
for executing the scripting language, which includes not just the logic
behind function calls, variable declarations, and the like, but also
the type substituion algorithms, which we now know contain a bug. Furthermore,
interpreter.c is nearly 2k source lines of code, which statistically suggests
there will be issues.

`_interpreter_run` is a very long function (spans 113 lines). This is a code smell.

Preventing scope creation will work for only one node, not any deeper. This may be an issue,
or at least a pitfall that needs to be documented.

interpreter.c line 1178
```
int needs_scope = (mode == SCOPE_FORCE) || 
    (mode == SCOPE_NORMAL && libab_tree_has_scope(tree->variant));
```

Is a "base" tree type even necessary its behavior has no distinction from a code block?

interpreter.c line 1188
```
else if (tree->variant == TREE_BASE || tree->variant == TREE_BLOCK)
```

Why are "void" nodes even necessary, if they don't do anything? Aren't they just wasting space?
Perhaps we can assume "NULL" is a valid parameter to the function, and return unit in that case?
I know most functions implictly assume valid parameters, and NULL isn't a "valid parameter" in
the common way, but still. Perhaps this will make some special cases easier down the line.

interpreter.c line 1199
```
else if (tree->variant == TREE_VOID) {
```

Make sure operator returned won't be NULL.

interpreter.c line 1208
```
libab_operator* to_call = libab_table_search_operator(
    libab_ref_get(scope), tree->string_value, OPERATOR_INFIX);
```

Is `into` always initialized? Or will there be a `ref_free` call missing in some cases?

interpreter.c line 1235
```
libab_ref_copy(&function, into);
```

Shouldn't we error on unexpected tree type?

interpreter.c line 1276
```
libab_get_unit_value(state->ab, into);
```

This is a declaration after a function call, which ins't C89 compliant.

interpreter.c line 1151
```
libab_result result = _interpreter_run(state, tree, &output, scope, mode);
libab_value* value;
libab_parsetype* type;
```

Output value isn't defined when an error occurs.

interpreter.c line 1167
```
if(result == LIBAB_SUCCESS) {
    value = libab_ref_get(&output);
    type = libab_ref_get(&value->type);

    if(type->data_u.base != libab_get_basetype_bool(state->ab)) {
        result = LIBAB_BAD_CALL;
    }

    if(result == LIBAB_SUCCESS) {
        *into = *((int*) libab_ref_get(&value->data));
    }
}
libab_ref_free(&output);
return result;
```

Perhaps it's a good idea, as Matt pointed out, to move away form va-based generic arguments
to "map" and similar-in-nature functions. A void pointer which can be cast to a struct may
not be such a bad idea.

interpreter.c line 1058
```
int _interpreter_foreach_insert_function_param(
        void* data, va_list args) {
```

This might be better as a macro.

interpreter.c line 1040

```
int _interpreter_compare_function_param(
        const void* left, const void* right) {
    const libab_tree* data = right;
    return data->variant == TREE_FUN_PARAM;
}
```

### Section 4 (gc.c, Ryan Alder)
_Lines completed: 0-100 of 100 (Entire file)_

__Purpose:__ As with interpreter.c, we have discovered a problem with GC code
during manual testig: it didn't seem to work in all cases. Furthermore, as
described in the previous assignment, garbage collection is integral to
keeping abcs running for longer periods of time (which is assumed to be
a common pattern in the usage of calculators). Thus, the source code
needs to be examined for potential issues, and later, tested automatically.

Function `libab_gc_visit_children` is never actually used

gc.c lines 17-19
```
void libab_gc_visit_children(libab_ref* ref, libab_visitor_function_ptr func, void* data) {
    if(!ref->null) _gc_count_visit_children(ref->count, func, data);
}
```

Unnecessary to declare list here, just pass data instead to `_libab_gc_list_append`

gc.c lines 48-55
```
void _gc_save(libab_ref_count* count, void* data) {
    libab_gc_list* list = data;
    if(count->visit_children && count->gc >= 0) {
        count->gc = -1;
        _libab_gc_list_append(list, count);
        _gc_count_visit_children(count, _gc_save, data);
    }
}
```

May want to keep the original style, or switch all loops to this style. Previously head was declared outside of the while loop and incremented at the end of each loop.

gc.c line 85
```
while ((head = list->head_sentinel.next) != &list->tail_sentinel)
```

`head->prev->next` is the same as `list->head_sentinel.next`, may want to change to make it more readable

gc.c line 86
```
head->prev->next = head->next;
```

## Summary:

Although no significant bugs or defects were discovered during the review process, a number of stylistic imporvements were suggested. A common complaint was the usage of functions instead of macros. A more serious issue was the presence of unused functions (small ones, admittelty, but unused nonetheless). Since this could be an issue in other, non-reviewed files, it might be a good idea to run a static analyzer with the goal of detecting more dead code. Lastly, since no bugs were discovered in the type checking code, a more thorough and potentially automated approach is necessary.
