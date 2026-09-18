# Literal expressions

r[expr.literal.syntax]
```grammar,expressions
LiteralExpression -> INTEGER_LITERAL | `true` | `false`
```

Literal expressions produce integer or boolean values. [INTEGER_LITERAL]
defines the spelling of integer tokens. Unit `()` uses [TupleExpression].

## Integer typing and range

An integer suffix fixes its type; an absent suffix is resolved by
[local type inference](../types.md#inference). Range validity is determined after
type inference, independently of lexical recognition. Runtime integer widths
are specified in [Types](../types.md).

A leading minus forms a [NegationExpression]. The unary-minus literal case
permits the signed minimum: `-2147483648i32` uses an integer token of magnitude
2147483648 and represents the valid i32 minimum. Parenthesized magnitudes such
as `-(2147483648i32)` have the same rule.

After suffix resolution and type inference, an integer literal outside its
determined type's range is course UB; see the centralized
[integer literal range guarantee](../undefined-behavior.md#integer-literal-range).
