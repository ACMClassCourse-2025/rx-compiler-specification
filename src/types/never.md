# Divergence and Never type

The never type, conventionally written `!`, has no values and represents
computations that do not complete normally. It is used internally;
`!` is not a valid source type annotation.

A *diverging expression* is an expression that never completes normal execution.
Any expression of type `!` is a diverging expression. However, diverging expressions are not limited to type `!`.

For example, the following [block expression](../expressions/block-expr.md) diverges but has type `i32`:

```rust,ignore
{
    loop {};
    42i32 // The tail expression determines the block's type, even if unreachable.
}
```

See [block expressions](../expressions/block-expr.md), [if expressions](../expressions/if-expr.md), and [loop expressions](../expressions/loop-expr.md) for their divergence rules.

`return`, `break`, and `continue` expressions diverge and have the never type: they transfer
control instead of producing a value where they appear.

The never type can coerce to any expected result type in the supported [coercion contexts](../types.md#conversions-and-coercions).
Without an expected type, never results do not constrain the other results' common type; if all results are never, that common type is never. This permits an if branch to return early while another branch yields a value, including a reference or container.
The unit type `()` and the never type `!` are distinct: an expression evaluating to `()` completes normally, whereas an expression of the never type does not.

```rust,ignore
fn choose_or_return(flag: bool) -> i32 {
    let n: i32 = if flag { return 7; } else { 3 };
    n + 1
}
```

A function body of type `!` satisfies any declared return type, such as
`fn forever() -> i32 { loop {} }`. A call to that function has its declared
type `i32`. Never behavior is determined locally within each function body.

## Unreachable code

Unreachable code still requires the usual semantic checks, including name
resolution, type checking, and place-mutability checking. Cases explicitly
classified as undefined behavior do not require a diagnostic.

For programs within the test guarantees, generated code must respect
control-flow transfers. A `return` exits the current function, a `break` exits
its target loop, and a `continue` proceeds to the next iteration. Code bypassed
by these transfers must not execute.

Place-mutability violations in unreachable code exhibit undefined behavior under the
[test guarantees](../undefined-behavior.md#test-guarantees). This includes ordinary
and compound assignment to immutable places, mutable borrowing of an immutable
place, and method or indexing adjustments that require unavailable mutable access.
The rule covers local variables, struct fields, indexed elements, and dereferenced places. No
diagnostic is required for these violations. For example:

```rust,ignore
fn example() {
    let value = 1;
    let shared = &value;
    return;
    value = 2; // undefined behavior: immutable local assignment after return
    let borrowed = &mut *shared; // undefined behavior: mutable borrow after return
    *shared = 3; // undefined behavior: write through a shared reference after return
}
```

This exclusion applies specifically to place mutability. Name resolution errors, type
mismatch errors, and assignment destinations that are not place expressions remain
required compile errors even in unreachable code.

The compiler implementation is not required to represent the never type explicitly in its
AST or IR, nor is it required to solve the halting problem or prove termination.
