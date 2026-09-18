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

A function definition consists of its name, optional lifetime parameters, value parameters, optional result type and lifetime bounds, and a required body. Top-level functions use ordinary parameters; functions in inherent impls may also have the receiver described below.

[GenericParams] declares lifetime parameters after the function name. [WhereClause] states lifetime bounds after the result type, or after the parameter list when the result annotation is omitted. See [lifetime parameters](generics.md).

Ordinary parameters use [IdentifierBinding] followed by an explicit type. Function signatures determine parameter and result types independently of their callers. A trailing comma is allowed after a receiver or the last ordinary parameter. An omitted result means `()`.

The [name lookup rules](../names.md#settled-scope-rules) permit forward calls and mutual recursion. Function names serve as call targets rather than first-class values. [Call expressions](../expressions/call-expr.md) specify argument passing; [return expressions](../expressions/return-expr.md) specify function results. The executable entry is defined in [Program entry](../undefined-behavior/builtin.md#program-entry).

```rust,ignore
fn add(left: i32, mut right: i32) -> i32 {
    right += left;
    right
}
```

## Receivers

Within an inherent impl, the first parameter may be [SelfParam]. Its forms are:

| Form | Receiver |
| --- | --- |
| `self` | By-value receiver |
| `mut self` | Mutable by-value receiver |
| `&self` or `&'a self` | Shared reference |
| `&mut self` or `&'a mut self` | Mutable reference |

A function with a receiver is a method; a function without a receiver is an
associated function. Later parameters use [FunctionParam]. Receiver lifetimes
follow the [lifetime validity rules](../references.md#lifetime-validity), and
by-value receivers follow [copy/move semantics](../builtin-traits.md#value-semantics).
