# Field access expressions

r[expr.field.syntax]
```grammar,expressions
FieldExpression -> Expression `.` IDENTIFIER
```

The Reference production is retained. The identifier selects a declared named field; tuple indexing is absent. Builtin dereferences allow access through supported references and Box values.

The result is a place whose mutability follows its base. A value base is evaluated once and materialized as a temporary when required. Reading the field obeys Copy/move rules. See [places and values](../expressions.md#places-and-values) and [reference storage duration](../references.md).
