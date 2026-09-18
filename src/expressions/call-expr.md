# Call expressions

r[expr.call.syntax]
```grammar,expressions
CallExpression -> Expression `(` CallParams? `)`

CallParams -> Expression ( `,` Expression )* `,`?
```

Call arguments follow the [ordinary evaluation order](../expressions.md#evaluation-order) and are passed according to the declared parameter types and [copy/move rules](../builtin-traits.md#copy). The callee must resolve to a declared function, associated function, or builtin. An optional trailing comma follows the last argument.

An associated method may also be called as `Type::method(receiver, ...)`, with its receiver supplied as an ordinary first argument. That form uses ordinary argument coercions rather than method-call autoref. Box/Vec constructor calls use the same call syntax with explicit type arguments on the container path segment.
