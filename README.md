# abcs
A libabacus front end for the command-line.

## Syntax
Please see the libabacus documentation (external/libabacus/README.md) for a guide through the syntax.

## Implementation
abcs uses GNU MPFR for its number implementations. It keeps 200 digits of precision, and displays up to 50. By default,
3 digits are displayed. The `request_precision` function can be used to request more digits to be displayed.

## RC File
abcs loads, by default, an `.abcsrc` file in the same directory in which it is run. It parses the whole file ingoring
newlines and spaces, and evaluates it in order. The RC file can be used to specify constants or declare functions that
the user wants to re-use between sessions.

## Provided Functions and Operators
|Operator|Function|Precedence|
|--------|--------|----------|
| <  | lt | 1 |
| <= | lte | 1 |
| == | equals | 1 |
| >  | gt | 1 |
| >= | gte | 1 |
| +  | plus | 2 |
| -  | minus | 2 |
| *  | times | 3 |
| /  | divide | 3 |
| ^  | pow | 3 |
| -  | negate | (prefix) |
| !  | factorial | (postfix) |

|Function|Description|
|--------|----|
|lt | less than comparator.|
|lte | less than or equal comparator.|
|equals| equality comparator. |
|gt| greater than comparator. |
|gte| greater than or equal comparator. |
|plus| addition function. |
|minus| subtraction function. |
|times| multiplication function. |
|divide| division function. |
|pow| exponentiation function |
|negate| flip sign of number. |
|factorial| factorial via the gamma function. |
|ln|natural logarithm function.|
|exp| e to the nth power function.|
|sqrt| square root function.|
|sin| trigonometric sine function. |
|cos| trigonometric cosine function. |
|tan| trigonometric tangent function. |
|arcsin| trigonometric inverse sine function. |
|arccos| trigonometric inverse cosine function. |
|arctan| trigonometric inverse tangent function. |
|quit| quits the REPL. |
|request\_precision| requests the given number of digits to be displayed in the REPL. |
