# Assignment 2: Unit Testing

Due February 5, 2019 at 11:59 PM

Team Name: nullptr

Team Members: Ryan Alder, Danila Fedorin, Matthew Sessions

Github Project Fork URL: [https://github.com/sessionm21/abcs](https://github.com/sessionm21/abcs)

## Test Plan

### Methodology (libraries used, etc.)

abcs is a command-line program. Therefore, the only way to interact with it was through the console. In order to test the various capabilities, we decided to input various commands that would push the limits of the program. These commands include edge cases, inputs that take too long to evaluate, and inputs that are deliberately engineered to stress the logic. In order to figure out the most important capabilities, we combine our knowledge of the purpose of the program (to work as a calculator), as well as the results of running a profiler on a long-running input. Combined, this allowed to us to find capabilities that were directly needed to the user, as well as those that are required for the “smooth” operation of the software. The capabilities in the first category included function calls, addition, and loops, while those in the latter included reference counting and garbage collection (abcs should be able to run in the background, and be used indefinitely).

### Units tested

#### Test: Addition / Subtraction / other binary arithmetic operators
This is the most important feature of abcs. This is because most common user calculation involve addition, subtraction, and other basic arithmetic operations: this can be tentatively verified by the design of modern calculator applications on on phones. Theiy mimic a 4-function calculator. Because of this, addition, subtraction, and other common operations must work. The user should be able to trust the calculator, and not have to guess whether or not the result of their requested arithmetic expressions is correct. This involves several things:
1. Correct output from +, -, etc
2. Correct order of operations
3. Associativity

Additionally, just like any user-facing capability, arithmetic operations must have good error recovery. Performing operations that are invalid or impossible (adding a number to a function) should not be allowed, and errors must be reported to the user rather than corrupting program state.

Internally, abcs uses functions to represent operators. As such, declaring a function with a conflicting name has a high chance of breaking the given operator.

__Input__:
```
3+3
```
__Expected output__: `6`

__Real output__: `6`

__Notes__: This is a sanity check test. There’s no reason it should fail.



__Input__: 

```
fun plus(x: num, y: num): num {
    x+y
}
3+3
```

__Expected output__: `error`

__Real output__: `error`

__Notes__: Adding a second function with the same number of arguments is expected to crash the addition operator, since the overloading code can no longer figure out which function to call.



__Input__:

```
fun plus(x: num): num {
    x+1
}
3+3
```

__Expected output__: 6

__Real output__: 6

__Notes__: Adding a second function with less arguments than the addition operator takes should not crash the interpreter, as it is not a viable candidate when in the overload resolving code.



__Input__:

```
fun plus(x: num, y: num, z: num): num {
    x+y+z
}
3+3
```

__Expected output__: 6

__Real output__: 6

__Notes__: regular function calls should be checked before partial function application, so this should actually work, although the 3-parameter add function _can_ be used with two arguments.



__Input__:

```
1--2
```

__Expected output__: 3

__Real output__: 3

__Notes__: this is a test to differentiate operators that are both infix and prefix. This is equivalent to 1-(-2), and is the only case that covers this branch in the code.



__Input__:

```
3!
```

__Expected output__: 6

__Real output__: 6

__Notes__: Sanity check test for postfix operators



__Input__:

```
3+!4
```

__Expected output__: error

__Real output__: error

__Notes__: This is a common error from an earlier version of the program. Depending on how the internal stack is used to re-order arguments into reverse polish notation, this could be equivalent to 3+4!, which is incorrect.  

__Input__:
```
6-3*2+3^2^3+2
```
__Expected output__: 6563

__Real output__:  6563

__Notes__: This is a test of order of operations. It's very important that this is satisfied. Exponentiation is used twice to make sure its right associativity is correctly implemented.

__Input__:
```
(6-3)*2
```
__Expected output__: 6

__Real output__: 6

__Notes__: This is a test of parenthesized expressions. They should behave like they do in...normal math.

__Input__:
```
(5-3
5-3)
```
__Expected output__: error both times

__Real output__: error both times

__Notes__: This test exercises the error handling branches of the Shunting Yard algorithm: the parentheses are mismatched.

#### Test: Function calls (type substitution, generics, function overloading)

__Description:__ A lot of complexity lies under the surface of a simple trigonometric expression, arctan(½). This is is because internally, abcs uses a type substitution mechanism to verify that the arguments being passed are of a correct type. It also uses function overloading, which means that it must attempt to call one of many functions with the same name. In fact, even declaring functions is no easy task: a function with a name that has already been used must be added to the list of functions with the same name, while a function with a brand new name must be added as a category of its own. Because abcs also features generics (functions that take parameters of any type), the level of complexity (and the number of potential bugs) greatly increases. From all of this, we know some of the things we want to test:

1. Function calls: correct / incorrect types of parameters

2. Substitution of parameters into functions that accept generic arguments

3. Function overloading (one function, many functions, variable with same name)



__Input__:

```
fun test(x: 'T, y: 'T): 'T {
    x    
}
test(3, 3)
test(true, false)
test(3, true)
test(true, 3)
```

__Expected output__: 3, true for the first two test cases, errors for the second two cases

__Real output__:

__Notes__: this is a basic test for generics. As the code goes through the parameters, it stores the substituted variables into the scope table. Thus, it should accept any first parameter, but accept / reject the second parameter depending on whether or not it has the same type. Thus, the first two examples should work because of their matching types, but the second two should fail.



__Input__:

```
fun one_or_another(x: 'T, y: 'T): (bool)->'T {
    fun test(b: bool) {
        if(b) x else y
    }
}
fun test(x: 'T, y: 'T): 'T {
    x
}
first = one_or_another(1, 2);
second = one_or_another(true, false);
third = one_or_another(1, 3);
fourth = one_or_another(false, true);
test(first, first)
test(first, third)
test(second, second)
test(second, fourth)
test(first, second)
test(second, third)
```

__Expected output__:  No errors in the first 4 (abcs can't print functions, but that's OK), errors in the last two

__Real output__:

__Notes__: This tests the recursive type comparison functionality. The code that calls a function must recursively walk two trees in parallel, ensuring that the types are matched up. Thus, types which are the same at the top level but have differently-typed children are used (function, in this case)



__Input__:

```
fun test(a: num, b: num): num {
    a + b
}
fun test(a: num): num {
    a    
}
fun test(a: num, b: bool): num {
    a * 2
}
test(3)
test(3, 4)
test(3, true)
```

__Expected output__: 3, 7, 6

__Real output__:

__Notes__: The function should pick the second test function when one parameter is given, since function calls are supposed to be given precedence over partial application. The second call should go through both the first and third test, but only by accepted by the first one since the third test requires the second parameter to be a boolean. The last call should also go through both the first and the third call, but since the second parameter is a boolean, it should be accepted by the third function.



__Input__:

```
fun test(a: num, b: num): num {
    a + b
}
fun test(a: num, b: num): num {
    a - b
}
test(a, b)
```

__Expected output__: error

__Real output__:

__Notes__: This is an example of an ambigous function call. It's not clear which "test" should be called. abcs should generate an error here.



__Input__:

```
fun test(a: num, b: num): num {
    a + b
}
fun test(a: num): num {
    a
}
test = fun weird(a: num): num { -a }
test(3)
test(3, 4)
```

__Expected output__: -3, error

__Real output__:

__Notes__: Assigning to a variable clears all the functions associated with that name. Thus, the first two declarations should be effectively erased.



#### Test: First class functions (partial function application, returning functions as values, closures)

__Description:__ abcs, like many modern programming languages, features first class functions. This means that functions can be given as arguments to other functions, and passed around like other variables. While this is not used by novice users, it is a useful concept for those who are more advanced. Since abcs tailors to power users, this advanced feature must also be tested. At its core, it is composed of several things, each of which need to be tested:

1. Partial function application: functions can be given less than their required number of arguments. This is an idea borrowed from functional languages like Haskell.

2. Functions as values: functions should behave exactly the same way as normal value.

3. Closures: functions capture the scope in which they’re created, allowing access to variables outside of the function.



__Input__:

```
a = plus(3)
a(7)
```

Expected ouput: 10

__Real output__:

__Notes__: This tests partial function application, as well as some branches on actual function calls. A function that's given less parameters than it takes returns another function, which takes the remaining parameters. Thus, add took one parameter and returned a, a function, which takes another parameter and returns the result of adding 3 to that parameter.



__Input__:

```
fun do_something(f: (num)->num, x: num): num {
    f(x)
}
a = plus(3)
do_something(a, 7)
```

__Expected output__: 10

__Real output__:

__Notes__: Here, the function a is passed as a value to do\_something, which accepts a function as its first parameter. This is the baseline test for first-class functions. Note: type checking for functions is covered in the previous capability tests, making it unnecessary to try it here.



__Input__:

```
x = 0
fun increment_x(): unit {
    x = x + 1;
}
increment_x()
increment_x()
x
```

__Expected output__: 2

__Real output__:

__Notes__: Here, the function must capture the scope in which it's declared, and therefore have access to the variable x. Modifying x inside the function should affect x outside the function.



__Input__:

```
fun adder(start: num): () -> num {
    fun add(): num {
        prev = start;
        start = start + 1;
        prev
    }    
}
a = adder(4)
a()
a()
a()
```

__Expected output__: 4, 5, 6

__Real output__:

__Notes__: Functions should be able to capture variables that would otherwise be completely unreachable. For instance, once the call to "adder" completes, there's no way to read the start variable from outside. However, since the function was declared inside of it, it should be able to read "start", as it captured the scope in which it was declared.



#### Test: Garbage collection

__Description:__ Garbage collection is an integral feature of interpreted (and some compiled) programming languages. If the memory is not to managed manually (which it isn’t at the desired level of abstraction of abcs), it must be managed by the language. Some programming languages use reference counting to handle this: Rust, for instance, does not have a garbage collector. However, this is only possible because of its complex ownership system, which abcs does not have. Functionality such as functions and scopes requires dependency cycles. Since abcs may be open for prolonged periods of time, it’s important that memory usage does not creep up. Garbage collection is the only other way to find and free unused memory, and therefore serves a key purpose in abcs.



__Input__:

```
fun waste_space(x: num): (num)->num {
    if(x < 1) {
        fun waste_base(y: num): num { y + 1 }
    } else {
        waste_next = waste_space(x - 1);
        fun waste_recursive(y: num): num { waste_next(y) + 1}
    }
}
weird = waste_space(5000)
weird(0)
weird = 0
```

__Expected output__: 501, memory usage goes up until last line, at which point it decreases

__Real output__: Segmentation fault as a result of executing waste_space(5000)
```bash
 > fun waste_space(x: num): (num)->num { if(x < 1) { fun waste_base(y: num): num { y + 1 } } else { waste_next = waste_space(x-1); fun waste_recursive(y: num): num {waste_next(y) + 1}}}
r0 = Unable to convert to string.
 > weird = waste_space(5000)
Segmentation fault (core dumped)
```

__Notes__: waste_space creates a lot of closures. Functions and their lexical scope have dependency cycles: a scope can't be freed until the function referencing it is freed, and a function can't be freed until the scope it's referencing is freed. This should be handled by GC: when "weird" is no longer used, GC should catch it and free the entire chain.



#### Test: Control flow

__Description:__ We’ve talked about functions and power users above, but without the ability to branch, abcs would not be turing complete. Therefore, control flow is an integral part of abcs’s scripting language. The following components need to be tested:

1. Condition requirements (non-boolean condition, missing condition)

2. Correct evaluation (branch to if branch on true condition, branch to else on false condition)

__Input__:

```
if(true) 3 else 4
if(false) 3 else 4
```

__Expected output__: 3, 4

__Real output__: 3, 4
```bash
 > if(true) 3 else 4
r0 = 3.00
 > if(false) 3 else 4
r1 = 4.00

```

__Notes__: if / else is an expression. It evalutes to a value. This value is given by the expression for each branch. For instance, if the "true" branch is taken, and 3 is evaluated, 3 also becomes the return value of the if/else. So, this test covers both the "check the condition" part of the capability, and the "evaluate to correct expression".



__Input__:

```
if 3 else 4
if () 3 else 4
```

__Expected output__:  error in both cases

__Real output__: Both resulted in errors
```bash
 > if 3 else 4
Invalid expression.
 > if () 3 else 4
Invalid expression.
```

__Notes__: If/else needs a condition. If one is not provided, it should error.



__Input__:

```
if(3) 3 else 4
if(add) 3 else 4
```

__Expected output__: error in both cases

__Real output__: Both resulted in errors
```bash
 > if(3) 3 else 4
Invalid expression.
 > if(add) 3 else 4
Invalid expression.
```

__Notes__: If/else should not accept non-boolean values as conditions. This exercises the "REQUIRE_BOOL" branches of the code.



__Input__:

```
x = 0
while(x < 5) {
    x = x + 1
}
x = 0
do {
    x = x + 1
} while(x < 5)
```

__Expected output__: 5, 5

__Real output__: 5, 5
```bash
 > x = 0
r0 = 0.00e-1
 > while(x < 5) { x = x + 1 }
r1 = 5.00
 > x = 0
r2 = 0.00e-1
 > do { x = x + 1 } while(x < 5)
r3 = 5.00
```

__Notes__: Just like if / else, while is an expression. It should return the result of the last expression. The last time the condition is true is when x = 4, so after the body is evaluated, x will be equal to 5. The same happens with do/while: the last time the condition is true, x is equal to 5. So, the result will be 5.



__Input__:

```
x = 0
while { x = x + 1}
do { x = x + 1 }
```

__Expected output__: error both times

__Real output__: Both statements resulted in errors
```bash
 > x = 0
r0 = 0.00e-1
 > while { x = x + 1}
Invalid expression.
 > do { x = x + 1 }
Invalid expression.
```

__Notes__: Just like if/else, while and do while should behave correctly when no condition is given (they should error!)



__Input__:

```
x = 0
while (3) { x = x + 1}
do { x = x + 1} while (3)
```

__Expected output__: error both times

__Real output__: Neither resulted in errors
```bash
 > x = 0
r0 = 0.00e-1
 > while (3) {x = x + 1}
r1 = ()
 > do { x = x + 1 } while (3)
r2 = 1.00
```

__Notes__: Once again, providing a condition of a non-boolean type should case an error in both while and do while.



__Input__:

```
x = 0
while(false) {
    x = 2
}
x = 0
do {
    x = 2
} while(false)
```

__Expected output__: 0 in the first case, 2 in the second case.

__Real output__: 0, then 2
```bash
 > x = 0
r0 = 0.00e-1
 > while(false) { x = 2 }
r1 = ()
 > x
r2 = 0.00e-1
 > do { x = 2 } while (false)
r3 = 2.00
```

__Notes__: unlike while, do while should execute one iteration of the loop before testing the condition.


#### Test: Variables

__Description:__ Variables are very useful for storing results of past computations, as well as constants. Graphing and scientific calculators include variables, which is a strong indication that a competing product would also have support them. Bugs in variables can cause very subtle issues - the users would not be able to tell if a variable has an incorrect value if it’s used as part of a compound expression. Similarly, variables need to keep their value unless explicitly modified by the user. Some things that need to be tested:

1. Variables named after functions. Intuitively, this should override the function declaration.

2. Variables named after types. Intuitively, this should cause an explicit error.

3. Variables declared in scopes, then let go out of scope. For example, a variable in a function should not be visible outside.

__Input__:
```
a = 1
a
```
__Expected output__: 1

__Real output__: 1

__Notes__: This is the baseline test for assigning to a variable. When a variable is given a value, the value can be accessed again by using the variable's name. This should persist between expressions.

__Input__:
```
a = 1
a = 2
a
```
__Expected output__: 2

__Real output__: 2

__Notes__: This exercises the "reassignment" branch of the code, which is supposed to release the memory associated with the previous value of the variable, and replace its content with a new value.

__Input__:
```
fun test(): num {
    3
}
test = 3
test()
test
```
__Expected output__: error, 3

__Real output__: error, 3
```bash
 > fun test(): num { 3 }
r0 = Unable to convert to string.
 > test = 3
r1 = 3.00
 > test()
Invalid expression.
 > test
r2 = 3.00
```

__Notes__: When a variable is declared with the same name as a function, the same mechanism should apply as when a variable is declared with the same name as an existing variable: the value associated with the name should be replaced with the new one. Thus, the function "test" should stop existing.


__Input__:
```
{ a = 1 }
a
```
__Expected output__: error

__Real output__: error
```bash
 > { a = 1 }
r0 = 1.00
 > a
Invalid expression.
```

__Notes__: `a` was declared inside a different scope, and goes out of scope after the end of the curly braces. Thus, it should not be accessible in the top level scope.

__Input__:
```
fun test(a: num): num {
    x = a
}
test(3)
x
```
__Expected output__: error

__Real output__: error
```bash
 > fun test(a: num): num { x = a }
r0 = Unable to convert to string.
 > test(3)
r1 = 3.00
 > x
Invalid expression.
```

__Notes__: Once again, x was declared in a different scope. This should exercise the function call code that sets the "create new scope" flag, which is different from how it's handled in the `{}` test above.

### Analyze the coverage of the unit test. Are any “edge cases” are covered?
While the logic of a lot of the tests was covered pretty thoroughly, it was difficult to achieve high line coverage. This was because a __lot__ of abcs / libabacus code is for error handling, and a very large chunk of possible errors are caused by allocation failures. Because malloc failures are impossible to reproduce without digging in the program's internals, a lot of the potential bugs lay undiscovered (and probably will in 99.99% of the cases - modern operating systems would use swap before running out of memory altogether). Effectively, the same is the case with branch coverage - unless allocation failure occurs, data usually takes only one path through the program. A lot of the branching is caused by if/else checks regarding memory allocation conditions. While looking through the source code of some of the functions in abcs, we found a bug that would only be triggered by allocation failure - but this remains for automated testing, and even there, we're not sure how we can test such rare conditions.

Another problem with coverage is that the only path through the program between input and output lies through most of the program's code. Since abcs is an interpeter, it always has to read the string, and it always has to convert it into tokens. It then always must run the parser, and finally, the interpreter on the string. It's very difficult to guide program execution through this long path.

Individually, here are the coverage of the 6 capability tests:
#### Basic Arithmetic
The basic arithmetic code achieved fairly good line coverage in both the parser and the interpreter sections. It executed all lines in the parser for expressions that didn't handle calling functions and malloc failures (which, as discussed above, is impossible to check manually). In the interpeter, it executed most of the code that handles function overloading, as well as the code that handles looking up operators in the symbol tables. It also executed a lot of the function calling code, and all the operator handling code in the interpreter. The coverage therefore is good (except, once again, for malloc failures).

#### Functions
The function testing code reached a lot of the expression code from the previous capability. Its main benefit over the arithmetic code was that it tested all the function parsing and declaration (creating a function instance for a given parse tree, storing it into a symbol table), as well as the parameter substitution code. Just like basic arithmetic, it couldn't cover cases where a memory allocation failed, and our team actually discovered a bug that hinges on this.

#### First class functions
These test reached the partial function application sections of the interpreter code, which are normally burried pretty deep in the function application code. Although it covered a lot of the code that had to do with searching through scopes and returning functions as values, this was actually previously covered by the other two tests: though these are parts of the capability that need to be tested, they rely on the surrounding "return value" and "search scope" code, which is very "hot" in the program - almost all valid inputs use that code.

#### Garbage Collection
Ideally, this test should cover a lot of the branching that wouldn't otherwise be executed in the garbage collection code. While GC objects are always added to a list of "potentially free-able data", in the above tests, such data never went out of scope, which made it impossible to check the freeing code. This turned out to be important, as we have discovered an issue: taking a function value out of scope doesn't seem to trigger garbage collection. This is really strange, because valgrind and other memory profiling tools report 0 memory allocateds after execution. This is a bug that we will investigate in later assignments.

#### If/Else/While/Do-While
These tests covered small, but important sections of the code. They tested the parsing logic for the control flow expressions, as well as the 20-25 lines that actually implemented the logic for these conditions.

#### Variables
Variable assignment actually contributed significantly to the code coverage of the program. This is because variable assignment is actually a special case - `=` is a reserved operator. As such, the code must go through an entirely different path. All of that branch was covered if allocation failures are to be ignored, since assignment overrides any value stored under that name previously.

In all of the above, we believe that we have handlded all of the special "edge" cases - the code is written to minimize those, which certainly helps. However, as mentioned several times, we're unable to handle the very special case of memory allocation failure - it's just not possible to do using manual testing, and it would be difficult to do even automatically - perhaps using a script that finds all uses of malloc, and tests all permutations of replacing "malloc" with "NULL".

## Results

1. Test outcomes, bugs documented
- All arithmetic tests passed as expected



2. Estimated coverage achieved (number of units tested / est. number of units in the program).
We believe that we achieved ~90% line coverage in tested areas. This is because code was written specifically to hit every possible if statement at least once. However, if we switch to looking at path coverage, this figure drops significantly. A large amount of control flow in abcs is dedicated to recovering from errors, and in a lot of cases, the only way for an error to propagate into a section of code is from an allocation further down the call stack. As such, most of the paths that are possible through the program are error recovery paths, and cannot be tested manually because we are unable to replicate allocation failure.

## Recommendations

We recommend that the garbage collection code is examined and debugged to determine why memory usage does not increase following a large number of elements going out of scope. This is a very significant issue, as it can cause allocated memory to creep up as the program is kept open.
