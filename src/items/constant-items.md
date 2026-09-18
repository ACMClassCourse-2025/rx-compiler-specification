# Constant items

r[items.const.syntax]
```grammar,items
ConstantItem -> `const` IDENTIFIER `:` Type `=` ConstValue `;`
```

A constant declaration gives a name, an explicit type, and an initializer. [ConstValue] specifies the integer, boolean, negative-integer, and parenthesized initializer forms.

Constants are declared at the top level or as associated items in an inherent impl. Declaration-order lookup and the distinction between ordinary expressions and constant contexts follow [Constants](../items.md#constants) and [Constant contexts](../const_eval.md).
