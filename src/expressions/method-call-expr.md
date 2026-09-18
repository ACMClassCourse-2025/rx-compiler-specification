# Method-call expressions

r[expr.method.syntax]
```grammar,expressions
MethodCallExpression -> Expression `.` PathExprSegment `(` CallParams? `)`
```

A method call gives a receiver expression, a method identifier, and a parenthesized argument list. An optional trailing comma follows the last argument.

[PathExprSegment] supplies the shared path-segment syntax. The supported methods use their identifier directly; [path resolution](../paths.md#path-resolution) checks this after parsing and diagnoses a type-argument list on a method segment.

Receiver candidate order, builtin reference/Box dereferencing, autoref, inherent methods, and the finite Clone operation follow [method lookup](../names.md#method-lookup). The selected method must satisfy argument and mutability rules. Array and container builtin methods retain their specified operation sets.
