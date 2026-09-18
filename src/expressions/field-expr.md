# Field access expressions

r[expr.field.syntax]
```grammar,expressions
FieldExpression -> Expression `.` IDENTIFIER
```

The identifier after `.` selects a declared named field; an unknown field is a static error. Builtin dereferences allow access through references and Box values.

The result is a place whose mutability follows its base. A value base is evaluated once and materialized as a temporary when required. Reading the field obeys Copy/move rules. See [places and values](../expressions.md#places-and-values) and [reference storage duration](../references.md).
