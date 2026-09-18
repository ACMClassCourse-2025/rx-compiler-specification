# Unit type

```grammar,types
TupleType -> `(` `)`
```

The unit type is written `()`, and its value is described by [TupleExpression].

An omitted function result is unit; assignments, while expressions, and blocks without tails also produce unit. Ordinary uses of zero-sized data are [course UB](../undefined-behavior.md); function and control-flow results may have unit type.
