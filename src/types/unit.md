# Unit type

```grammar,types
TupleType -> `(` `)`
```

Only the empty tuple-type branch of Rust's grammar remains. Its type is unit, written `()`, and its value is described by [TupleExpression]. Nonempty tuple types are unsupported.

An omitted function result is unit; assignments, while expressions, and blocks without tails also produce unit. Ordinary uses of zero-sized data remain [course UB](../undefined-behavior.md). This does not prohibit unit as a function/control-flow result.
