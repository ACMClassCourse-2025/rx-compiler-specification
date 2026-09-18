# Block expressions

r[expr.block.syntax]
```grammar,expressions
BlockExpression -> `{` Statements? `}`

Statements ->
      Statement+
    | Statement+ ExpressionWithoutBlock
    | ExpressionWithoutBlock
```

This retains the Reference's block and statement-list productions after removing inner attributes. [Statement] no longer has an item branch, so a block cannot contain local functions, constants, structs, or impls.

Statements execute in source order. A final expression without a semicolon is the block's tail. The Reference grammar represents an expression-with-block tail through the last [ExpressionStatement]; that final position is a value context and may yield a non-unit result. An expression-with-block used as a preceding semicolon-free statement must instead have unit type or diverge.

A block without a tail yields `()`. See [block and statement semantics](../statements.md#blocks-and-statements), [statement boundaries](../statements.md#statement-boundary), and [storage duration](../references.md).
