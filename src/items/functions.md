# Functions

r[items.fn.syntax]
```grammar,items
Function ->
    `fn` IDENTIFIER
        `(` FunctionParameters? `)`
        FunctionReturnType?
        BlockExpression

FunctionParameters ->
      SelfParam `,`?
    | (SelfParam `,`)? FunctionParam (`,` FunctionParam)* `,`?

SelfParam -> ShorthandSelf

ShorthandSelf -> `&`? `mut`? `self`

FunctionParam -> IdentifierBinding `:` Type

FunctionReturnType -> `->` Type
```

This is Rust's function production after removing qualifiers, generic parameters, where clauses, declarations without bodies, variadics, parameter attributes, and patterns. A [SelfParam] is allowed only as the first parameter of a function in an inherent impl; top-level functions cannot have receivers. Typed receivers and lifetime annotations are excluded.

Ordinary parameters require explicit types. A trailing comma is allowed after a receiver or the last ordinary parameter. An omitted result means `()`. See [functions](../items.md#functions) and [receivers](../items.md#inherent-impls-and-receivers) for the established typing, calling, and ownership rules.

```rust,ignore
fn add(left: i32, mut right: i32) -> i32 {
    right += left;
    right
}
```
