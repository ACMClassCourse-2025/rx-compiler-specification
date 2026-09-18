# Path expressions

r[expr.path.syntax]
```grammar,expressions
PathExpression -> PathInExpression
```

A path expression resolves an unqualified value name, `self`, or an associated item such as `Point::LIMIT`. The retained path forms and namespace rules are specified in [Paths](../paths.md) and [Names](../names.md).

A function path can be a call target but is not a first-class function value. Explicit Box/Vec constructors use [HeapConstruction]; general qualified paths and user generic calls are unsupported.
