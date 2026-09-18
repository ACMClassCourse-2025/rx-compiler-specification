# Unit expressions

```grammar,expressions
UnitExpression -> `(` `)`
```

The expression `()` constructs the unit value.

Unit is the result of a function or control-flow expression that completes without producing data. Its use as ordinary variable, argument, aggregate, or heap data is course UB under the [zero-sized-data guarantee](../undefined-behavior.md#zero-sized-data).
