# Expressions and evaluation

```grammar,expressions
Expression -> ExpressionWithoutBlock | ExpressionWithBlock

ExpressionWithoutBlock ->
      LiteralExpression
    | PathExpression
    | OperatorExpression
    | GroupedExpression
    | ArrayExpression
    | IndexExpression
    | TupleExpression
    | StructExpression
    | CallExpression
    | MethodCallExpression
    | FieldExpression
    | ContinueExpression
    | BreakExpression
    | ReturnExpression

ExpressionWithBlock ->
      BlockExpression
    | LoopExpression
    | IfExpression
```

Expressions form integer, boolean, and unit values; resolve names and associated paths; construct Box/Vec values, arrays, and named-field structs; access fields and indexed elements; call functions and methods; apply operators; and control execution through blocks, if/while/loop, and return/break/continue. Parentheses group expressions. Heap operations are specified in [Box and Vec](heap.md).

## Evaluation order

Operands of ordinary expressions are evaluated left to right as written before the enclosing operation. This includes call arguments, array elements, struct field initializers, comparisons, arithmetic, and indexing. Struct field evaluation follows source order rather than declaration/layout order.

The specialized rules for [assignment](expressions/operator-expr.md#evaluation-order), [short-circuit logic](expressions/operator-expr.md#comparison-and-logic), [array repetition](expressions/array-expr.md#array-expressions), and control flow specify which operands execute and their order.

These rules constrain optimizations whenever evaluation has observable effects, including I/O and writes through references. Removing unused values does not authorize removing effects that are still observable.

## Places and values

A place denotes storage: a variable, dereference, field expression, indexed array or Vec element, or parenthesized place. A field or index base may itself be a value expression. In that case, the base is evaluated once and materialized in temporary storage. The same applies to a value borrowed explicitly or by a method receiver adjustment. Using a place as a value copies or moves according to its type. Assignment requires a mutable place, and borrowing uses the place's address.

A mutable local can be assigned. Dereferencing `&mut T` gives a mutable place; dereferencing `&T` does not. In particular, `let p = &mut x;` need not make the binding p mutable to permit `*p = value`; reassignment of p itself does require a mutable binding.

Materialized temporary values can be mutable places even though they have no named mutable binding. For example, `make().value = 4` is supported when make returns a struct with a compatible field; `let p = &mut make().value;` can extend the relevant temporary's storage duration under the supported Rust rules. A temporary can also be the receiver of an `&mut self` method. These cases do not make an ordinary immutable variable mutable.

Temporary lifetime and allowed implementations are described in [References](references.md). Assignment requires an actual place expression: `1 = 2` and `1 += 2` are invalid. Blocks produce values with the ordinary copy/move semantics.

## Precedence

The following groups run from strongest to weakest. Operators in a row share precedence.

| Group | Associativity |
| --- | --- |
| Paths, literals, grouping | Primary forms |
| Field and method access, calls, indexing | Postfix |
| Unary `-`, `!`, `*`, `&`, `&mut` | Prefix |
| `as` | Left |
| `*`, `/`, `%` | Left |
| `+`, `-` | Left |
| `<<`, `>>` | Left |
| `&` | Left |
| `^` | Left |
| <code>&#124;</code> | Left |
| `==`, `!=`, `<`, `<=`, `>`, `>=` | Cannot be chained without parentheses |
| `&&` | Left |
| <code>&#124;&#124;</code> | Left |
| `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `^=`, <code>&#124;=</code>, `<<=`, `>>=` | Right |
| `return`, `break` with a value | Consume the following expression |

Block/statement disambiguation additionally follows [Statements](statements.md#statement-boundary).
