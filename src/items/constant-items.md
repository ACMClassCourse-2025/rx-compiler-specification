# Constant items

r[items.const.syntax]
```grammar,items
ConstantItem -> `const` IDENTIFIER `:` Type `=` ConstValue `;`
```

A constant declaration gives a name, an explicit type, and an initializer. The initializer follows the [constant-context rules](../const_eval.md) through [ConstValue]. A constant initializer may reference other constant items, subject to the same rules and the acyclic-dependency check. Each ordinary expression use of a constant produces its value.

Constants are declared at the top level or as associated items in an inherent impl. The [name lookup rules](../names.md#scope-rules) permit uses before the declaration.

```rust,ignore
const LIMIT: i32 = 100;
const NEGATIVE: i32 = (-1);
```
