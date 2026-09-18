# Never type

The never type, conventionally written `!`, describes expressions that do not
complete normally. It is used internally; `!` is not a valid source type
annotation.

`return`, `break`, and `continue` expressions have never type: they transfer
control instead of producing a value where they appear.

Never can fit an expected result type in the supported [coercion contexts](../types.md#conversions-and-references).
This permits an if branch to return early while another branch yields a value.
Unit and never are distinct: a unit call returns normally, whereas a return
expression does not.

```rust,ignore
fn choose_or_return(flag: bool) -> i32 {
    let n: i32 = if flag { return 7; } else { 3 };
    n + 1
}
```

A diverging body can fit an ordinary declared result, for example
`fn forever() -> i32 { loop {} }`. A call to that function has its declared
type i32. Never behavior is determined within each function body.

## Loop expressions

A `loop` expression has never type if no `break` expression targets it.
A `break` inside a nested loop targets that nested loop, so
`loop { loop { break; } }` still has never type.

If any `break` expressions target the loop, its type must be compatible with
all their values; `break;` supplies `()`. If every break operand has never type,
the loop also has never type. A `break` counts even if it is unreachable:
`loop { if false { break 1i32; } }` has type `i32`.

These rules apply to `loop`. A `while` expression has type `()`, even when its
condition is always true.

## Unreachable code

Unreachable code is accepted subject to ordinary name/type rules. Control flow
must preserve the behavior of diverging expressions, including skipping the
remainder of a block after a return.

The implementation need not expose a particular never representation in its
AST/IR or prove termination.
