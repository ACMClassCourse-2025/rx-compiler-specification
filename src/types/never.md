# Never type

The never type, conventionally written `!`, describes expressions that do not
complete normally. It is used internally; `!` is not a valid source type
annotation.

Return, break, and continue expressions do not produce a value along their
normal successor path and have never behavior. A loop with no associated
break expressions has never type. The presence and types of breaks determine
typing without requiring proof that a particular branch executes.

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

## Unreachable code

Unreachable code is accepted subject to ordinary name/type rules. Control flow
must preserve the behavior of diverging expressions, including skipping the
remainder of a block after a return.

The implementation need not expose a particular never representation in its
AST/IR or prove termination.
