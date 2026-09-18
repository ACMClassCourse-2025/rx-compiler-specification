# Call expressions

r[expr.call.syntax]
```grammar,expressions
CallExpression -> Expression `(` CallParams? `)`

CallParams -> Expression ( `,` Expression )* `,`?
```

The original Reference production is retained. Arguments evaluate from left to right and obey the declared parameter types and copy/move rules. The callee must resolve to a supported declared function, associated function, or builtin. Expression-shaped callee syntax does not introduce closures or function pointers.

An associated method may also be called as `Type::method(receiver, ...)`, with its receiver supplied as an ordinary first argument. That form uses ordinary argument coercions rather than method-call autoref. Explicit heap constructors have the narrower [HeapConstruction] syntax.
