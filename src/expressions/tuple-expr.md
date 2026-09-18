# Unit expressions

```grammar,expressions
TupleExpression -> `(` `)`
```

Only the empty branch of Rust's tuple-expression grammar is retained. It constructs the unit value. Nonempty tuples and tuple indexing are unsupported.

Unit is useful as the result of a function or control-flow expression. The [zero-sized-data exclusions](../undefined-behavior.md) still apply to using it as ordinary variable, argument, aggregate, or heap data. Keeping this syntactic branch does not expand that test domain.
