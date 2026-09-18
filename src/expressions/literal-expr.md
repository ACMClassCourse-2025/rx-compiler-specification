# Literal expressions

r[expr.literal.syntax]
```grammar,expressions
LiteralExpression -> INTEGER_LITERAL | `true` | `false`
```

The integer and boolean branches of Rust's literal-expression grammar remain. An integer suffix fixes its type; unsuffixed integers use [local type inference](../types.md#inference). Literals do not introduce implicit conversions between determined integer types.

A minus is a [NegationExpression], including the signed-minimum literal special case. Unit `()` uses the empty branch of [TupleExpression]. Character, string, byte, C-string, and floating-point expressions are unsupported.
