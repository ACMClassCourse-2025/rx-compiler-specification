# Block expressions

r[expr.block.syntax]
```grammar,expressions
BlockExpression -> `{` Statements? `}`

Statements ->
      Statement+
    | Statement+ ExpressionWithoutBlock
    | ExpressionWithoutBlock
```

A block contains [statements](../statements.md) and an optional final
expression, enclosed in braces. Statements execute in source order.

A final expression without a semicolon is the block's tail expression. It
determines the block's type and, when reached, its result value. A block
without a tail produces `()` if it completes normally. Control flow that
leaves earlier follows the [never rules](loop-expr.md#never-and-unreachable-code).

```rust,ignore
fn select(flag: bool) -> i32 {
    let base = if flag { 10 } else { 20 };
    { base + 1 } // the inner block and function body both yield an i32
}
```

Parsing follows [statement boundaries](../statements.md#statement-boundary);
storage follows [storage duration](../references.md#storage-and-implementation).
