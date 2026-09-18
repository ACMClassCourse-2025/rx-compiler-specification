# Block expressions

r[expr.block.syntax]
```grammar,expressions
BlockExpression -> `{` Statements? `}`

Statements ->
      Statement+
    | Statement+ ExpressionWithoutBlock
    | ExpressionWithoutBlock
```

A block contains [statements](../statements.md) and an optional tail expression, enclosed in braces.

Statements execute in source order. A final expression without a semicolon is the block's tail. The grammar represents an expression-with-block tail through the last [ExpressionStatement]; that final position is a value context and may yield a non-unit result. Preceding statements follow the [expression-statement rules](../statements.md#expression-statements).

A block without a tail yields `()`. Its result is the tail expression's value when it has one; control flow that leaves earlier follows the [never rules](loop-expr.md#never-and-unreachable-code). See [statement boundaries](../statements.md#statement-boundary) and [storage duration](../references.md#storage-and-implementation).
