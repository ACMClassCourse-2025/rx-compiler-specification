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

Ordinary assignment and compound assignments evaluate the right operand first, then the destination address. This includes compound assignment with a shared-reference right operand. The destination's side effects occur once; the compound update reads the destination after both operands have been evaluated. See the [operator evaluation table](expressions/operator-expr.md#evaluation-order). Lazy `&&` / <code>&#124;&#124;</code> and control-flow constructs evaluate only the selected operands or branches. Array repetition evaluates its element expression once.

These rules constrain optimizations whenever evaluation has observable effects, including I/O and writes through references. Removing unused values does not authorize removing effects that are still observable.

## Places and values

A place denotes storage: a variable, dereference, field expression, indexed array or Vec element, or parenthesized place. A field or index base may itself be a value expression. In that case, the base is evaluated once and materialized in temporary storage. The same applies to a value borrowed explicitly or by a method receiver adjustment. Using a place as a value copies or moves according to its type. Assignment requires a mutable place, and borrowing uses the place's address.

A mutable local can be assigned. Dereferencing `&mut T` gives a mutable place; dereferencing `&T` does not. In particular, `let p = &mut x;` need not make the binding p mutable to permit `*p = value`; reassignment of p itself does require a mutable binding.

Materialized temporary values can be mutable places even though they have no named mutable binding. For example, `make().value = 4` is supported when make returns a struct with a compatible field; `let p = &mut make().value;` can extend the relevant temporary's storage duration under the supported Rust rules. A temporary can also be the receiver of an `&mut self` method. These cases do not make an ordinary immutable variable mutable.

Temporary lifetime and allowed implementations are described in [References](references.md). Assignment requires an actual place expression: `1 = 2` and `1 += 2` are invalid. Blocks produce values with the ordinary copy/move semantics.

## Arrays

`[a, b, c]` evaluates each element once in order. `[expr; N]` evaluates expr once and repeats the resulting value. For N greater than one the element type must be Copy, not merely Clone. N is a [restricted constant](const_eval.md); zero lengths are excluded.

`array[index]` requires a usize index. Arrays have fixed length and can be indexed through shared or mutable references, including nested reference adjustments. Writing an element requires a mutable element place. Valid execution stays in bounds; no runtime bounds check is required.

A fixed array's builtin `.len()` returns its length as usize. There is no array-to-pointer conversion in source-level by-value arguments. Moving a non-Copy value out through an array index or borrowed contents is outside valid tests. Vec indexing likewise does not allow non-Copy move-out; Vec remove and owned Box dereference provide the [specified owning operations](heap.md).

## Structs and calls

Struct construction uses `S { field: expr, ... }` and the rules in [Items](items.md#structs). Field access through references performs the necessary builtin dereferences. Unknown fields and incompatible field values are static errors.

Calls resolve a declared function or associated function and match its argument list. Methods additionally adjust the receiver within the supported reference rules. Builtin clone and equality follow [Builtin traits](builtin-traits.md).

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
