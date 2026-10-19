# If expressions

r[expr.if.syntax]
```grammar,expressions
IfExpression ->
    `if` Conditions BlockExpression
    (`else` (BlockExpression | IfExpression))?

Conditions -> Expression _except an unparenthesized [StructExpression] at the condition/body boundary_
```

```rust,ignore
fn choose(flag: bool) -> i32 {
    if flag { 1 } else { 2 }
}
```

The condition expression must evaluate to type `bool`; enclosing parentheses are optional. `else if` chains are supported; execution evaluates conditions in order and executes the body of the first condition that evaluates to `true` (or the `else` block if present and no conditions match). A struct construction at the condition/body boundary requires explicit delimitation as described below.

When an unparenthesized `Name {` at the condition/body boundary could begin a struct literal, treat `{` as the start of the consequent block. To put such a struct construction in the condition, delimit it explicitly, for example `if (S { flag: true }).flag { ... }`. A function argument such as `if check(S { flag: true }) { ... }` is already delimited. This rule does not mean every brace after if starts its consequent: `if { true } { ... }` has a block-valued condition and a separate consequent block.

With `else`, an enclosing expected type applies to both result branches through [one-to-one coercions](../types.md#coercion-sites-and-expected-types). Without one, [LUB coercion](../types.md#least-upper-bound-coercions) finds their common type. A branch with the never type `!` can always coerce to the type of the other branch. Without `else`, the expression has unit type `()` and the `then` block has expected type `()`.

An `if` expression [diverges](../types/never.md) if either the condition expression diverges or if all arms diverge.

```rust,ignore
// These function bodies diverge despite their declared i32 return type.
fn diverging_condition() -> i32 {
    // Diverges because the condition expression diverges
    if loop {} {
        ()
    } else {
        ()
    };
    // The semicolon above is important: The type of the `if` expression is
    // `()`, despite being diverging. When the final body expression is
    // elided, the type of the body is inferred to ! because the function body
    // diverges. Without the semicolon, the `if` would be the tail expression
    // with type `()`, which would fail to match the return type `i32`.
}

fn diverging_arms() -> i32 {
    // Diverges because all arms diverge
    if true {
        loop {}
    } else {
        loop {}
    }
}
```
