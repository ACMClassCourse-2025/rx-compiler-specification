# Constant items

r[items.const.syntax]
```grammar,items
ConstantItem -> `const` IDENTIFIER `:` Type `=` ConstValue `;`
```

Rust's constant-item grammar is narrowed to a named constant with an explicit type and required initializer. [ConstValue] permits only the settled literal, negative-integer, and parenthesized forms. `_` constants and uninitialized trait constants are unsupported.

Constants can be top-level or associated items. They cannot be local items. Declaration-order lookup and the distinction between ordinary expressions and constant contexts follow [Constants](../items.md#constants) and [Constant contexts](../const_eval.md).
