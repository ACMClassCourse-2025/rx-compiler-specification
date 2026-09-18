# Return expressions

r[expr.return.syntax]
```grammar,expressions
ReturnExpression -> `return` Expression?
```

`return expression` evaluates its operand and returns that value from the current function using copy/move semantics. `return;` returns unit. Function tails provide the normal return value when execution reaches them. Return values must match the declared result.

See [Never type](../types/never.md) for typing and reachability.
