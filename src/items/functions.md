# Functions

r[items.fn.syntax]
```grammar,items
Function ->
    `fn` IDENTIFIER GenericParams?
        `(` FunctionParameters? `)`
        FunctionReturnType? WhereClause?
        BlockExpression

FunctionParameters ->
      SelfParam `,`?
    | (SelfParam `,`)? FunctionParam (`,` FunctionParam)* `,`?

SelfParam -> ShorthandSelf

ShorthandSelf -> (`&` Lifetime?)? `mut`? `self`

FunctionParam -> IdentifierBinding `:` Type

FunctionReturnType -> `->` Type
```

A function definition consists of its name, optional lifetime parameters, value parameters, optional result type and lifetime bounds, and body. A function in an inherent impl may begin its parameter list with a [SelfParam]: `self`, `mut self`, `&self`, or `&mut self`. A reference receiver may include a lifetime, as in `&'a self` or `&'a mut self`. Top-level functions use ordinary parameters.

[GenericParams] declares lifetime parameters after the function name. [WhereClause] states lifetime bounds after the result type, or after the parameter list when the result annotation is omitted. See [lifetime parameters](generics.md).

Ordinary parameters require explicit types. A trailing comma is allowed after a receiver or the last ordinary parameter. An omitted result means `()`. See [functions](../items.md#functions) and [receivers](../items.md#inherent-impls-and-receivers) for the established typing, calling, and ownership rules.

```rust,ignore
fn add(left: i32, mut right: i32) -> i32 {
    right += left;
    right
}
```
