# Loop expressions

## While and loop

```grammar,expressions
LoopExpression -> InfiniteLoopExpression | PredicateLoopExpression

InfiniteLoopExpression -> `loop` BlockExpression

PredicateLoopExpression -> `while` Conditions BlockExpression

BreakExpression -> `break` Expression?

ContinueExpression -> `continue`
```

`while condition { ... }` reevaluates a `bool` condition before each iteration and evaluates to the unit value `()`. Parentheses around the condition are optional. `loop { ... }` repeats until control flow leaves it; `break` values determine the loop expression's result type. The body block of either form has expected type `()`. A non-unit trailing expression in the body block does not become the value of the loop expression, and must satisfy the expected type even when unreachable. Thus, `while false { 1 }` and `loop { 1 }` cause compile-time type errors, whereas a `loop` expression can yield a value via `break value;`.


A `loop` expression without an associated `break` expression is [diverging](../types/never.md) and has type `!`.

A loop with associated break expressions does not [diverge](../types/never.md) if any of the break operands do not diverge. If all of the break operands diverge, then the loop expression also diverges.

Although the outer `loop` below never completes normally at runtime, it is
not considered diverging under this rule: the unreachable `break` still has
the non-diverging implicit operand `()`. The outer loop's type is `()`.

```rust,ignore
loop {
    loop {};
    break;
};
```

`break;` exits the nearest loop with unit. A value-bearing break requires a `loop` target. The loop's expected type applies to all its break values through [one-to-one coercions](../types.md#coercion-sites-and-expected-types); without one, [LUB coercion](../types.md#least-upper-bound-coercions) finds their common type. `continue;` starts the next iteration of the nearest enclosing loop (including reevaluating a `while` condition). Using `break` or `continue` outside a loop is a compile error.

Both `break` and `continue` expressions diverge and have type `!`, regardless
of the target loop's type or the type of any break operand.

A break or continue appearing in a while condition must target a loop nested inside that condition. Targeting the containing while or any outer loop is a compile error. For example, `while loop { break false; } {}` is allowed. Syntactic loop nesting determines the target.

```rust,ignore
fn until(limit: i32) -> i32 {
    let mut n = 0;
    loop {
        if n == limit { break n; }
        n += 1;
    }
}
```

See [Unreachable code](../types/never.md#unreachable-code) for
checks that still apply after control flow leaves a block.
