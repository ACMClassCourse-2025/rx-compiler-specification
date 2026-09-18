# Path expressions

r[expr.path.syntax]
```grammar,expressions
PathExpression -> PathInExpression
```

A path expression resolves an unqualified value name, `self`, or an associated item such as `Point::LIMIT`. Path forms and namespace rules are specified in [Paths](../paths.md) and [Names](../names.md).

A function path serves as a call target. Box/Vec constructors are calls to a [PathExpression] whose container segment carries [GenericArgs].
