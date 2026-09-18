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

A function definition consists of its name, parameter list, optional result type, and body. A function in an inherent impl may begin its parameter list with a [SelfParam]: `self`, `mut self`, `&self`, or `&mut self`. Top-level functions use ordinary parameters.

Ordinary parameters require explicit types. A trailing comma is allowed after a receiver or the last ordinary parameter. An omitted result means `()`. See [functions](../items.md#functions) and [receivers](../items.md#inherent-impls-and-receivers) for the established typing, calling, and ownership rules.

```rust,ignore
fn add(left: i32, mut right: i32) -> i32 {
    right += left;
    right
}
```
