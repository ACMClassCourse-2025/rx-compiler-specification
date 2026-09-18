# Literal expressions

r[expr.literal.syntax]
```grammar,expressions
LiteralExpression -> INTEGER_LITERAL | `true` | `false`
```

Literal expressions produce integer or boolean values. [INTEGER_LITERAL] defines the spelling of integer tokens.

Note that Unit `()` uses [UnitExpression] instead.

## Integer typing and range

An integer suffix fixes its type. Without a suffix, use the expected integer type when one is supplied; otherwise use `i32`. Inference requiring propagation through a binary operator, unary operator, or shared/mutable borrow is [course UB](../types.md#coercion-sites-and-expected-types). Selecting the type according to subsequent operands or later use are [course UB](../types.md#coercion-sites-and-expected-types) as well. Runtime integer widths are specified in [Types](../types.md).

A leading minus forms a [NegationExpression]. The unary-minus literal case
permits the signed minimum: `-2147483648i32` uses an integer token of magnitude
2147483648 and represents the valid i32 minimum. Parenthesized magnitudes such
as `-(2147483648i32)` have the same rule.

After selecting the type by suffix, expected type, or `i32` fallback, an integer
literal outside that type's range is course UB; see the centralized
[integer literal range guarantee](../undefined-behavior.md#integer-literal-range).
