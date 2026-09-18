# Literal expressions

r[expr.literal.syntax]
```grammar,expressions
LiteralExpression -> INTEGER_LITERAL | `true` | `false`
```

Literal expressions produce integer or boolean values. An integer suffix fixes its type; unsuffixed integers use [local type inference](../types.md#inference).

A leading minus forms a [NegationExpression], including the signed-minimum literal special case. Unit `()` uses [TupleExpression].
