# Never type

The never type, conventionally written `!`, describes expressions that do not
complete normally. It is used internally; `!` is not a valid source type
annotation.

`return`, `break`, and `continue` expressions have never type: they transfer
control instead of producing a value where they appear.

Never can fit an expected result type in the supported [coercion contexts](../types.md#conversions-and-coercions).
Without an expected type, never results do not constrain the other results' common type; if all results are never, that common type is never. This permits an if branch to return early while another branch yields a value, including a reference or container.
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

If any `break` expressions target the loop, their values follow the
[common result rules](../types.md#least-upper-bound-coercions), using the loop's expected type when supplied; `break;` supplies `()`. If every break operand has never type,
the loop also has never type. A `break` counts even if it is unreachable:
`loop { if false { break 1i32; } }` has type `i32`.

These rules apply to `loop`. A `while` expression has type `()`, even when its
condition is always true.

## Unreachable code

Unreachable code still requires the usual semantic checks, including name
resolution, type checking, and place-mutability checking. Cases explicitly
classified as undefined behavior do not require a diagnostic.

For programs within the test guarantees, generated code must respect
control-flow transfers. A `return` exits the current function, a `break` exits
its target loop, and a `continue` proceeds to the next iteration. Code bypassed
by these transfers must not execute.

Assigning to an immutable local in unreachable code is undefined behavior under the
[test guarantees](../undefined-behavior.md#test-guarantees). This includes
ordinary and compound assignment. No diagnostic is required. For example:

```rust,ignore
fn example() {
    let value = 1;
    return;
    value = 2; // undefined behavior: immutable local assignment after return
}
```

The implementation need not expose a particular never representation in its
AST/IR or prove termination.
