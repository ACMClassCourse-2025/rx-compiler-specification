# Field access expressions

r[expr.field.syntax]
```grammar,expressions
FieldExpression -> Expression `.` IDENTIFIER
```

The identifier after `.` selects a declared named field after builtin [autoderef](../types.md#autoderef-and-autoref); an unknown field is a static error.

The result is a place whose mutability follows its base. A value base is evaluated once and materialized as a temporary when required. Reading the field obeys Copy/move rules. See [places and values](../expressions.md#places-and-values) and [reference storage duration](../references.md).
