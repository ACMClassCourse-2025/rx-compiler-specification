# Method-call expressions

r[expr.method.syntax]
```grammar,expressions
MethodCallExpression -> Expression `.` PathExprSegment `(` CallParams? `)`
```

This is the Reference method-call production with [PathExprSegment] narrowed to an identifier. There are no generic method arguments.

Receiver candidate order, builtin reference/Box dereferencing, autoref, inherent methods, and the finite Clone operation follow [method lookup](../names.md#method-lookup). The selected method must satisfy argument and mutability rules. Array and container builtin methods retain their specified operation sets.
