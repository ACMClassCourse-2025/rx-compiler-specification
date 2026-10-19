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

A block is considered to be [diverging](../types/never.md) if all reachable control flow paths contain a diverging expression.

A block's type and result value are determined as follows:

- If the block has a tail expression (a final expression without a semicolon), that expression determines the block's type and, when reached, its result value. A tail expression determines the block's type even if prior statements diverge (the unreachable tail expression is still type-checked against any expected type).
- If the block has no tail expression and does not diverge, its type is `()` and it evaluates to `()`.
- If the block has no tail expression and diverges, the block has the [never type](../types/never.md) `!`.

```rust,ignore
fn select(flag: bool) -> i32 {
    let base = if flag { 10 } else { 20 };
    { base + 1 } // the inner block and function body both yield an i32
}
```

Even though the formal grammar permits parsing an un-semicoloned [ExpressionWithBlock](../expressions.md) under statement sequences, in the final position of a block it acts as the block's tail expression and determines the block's value.

Parsing of blocks and trailing statements follows the [statement boundary rules](../statements.md#statement-boundary);
local variable storage follows [storage duration](../references.md#storage-and-implementation).
