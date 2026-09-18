# Loop expressions

## While and loop

```grammar,expressions
LoopExpression -> InfiniteLoopExpression | PredicateLoopExpression

InfiniteLoopExpression -> `loop` BlockExpression

PredicateLoopExpression -> `while` Conditions BlockExpression

BreakExpression -> `break` Expression?

ContinueExpression -> `continue`
```

`while condition { ... }` reevaluates a bool condition before each iteration and has unit result. Parentheses around the condition are optional. `loop { ... }` repeats until control flow leaves it; break values determine the loop expression's result type. The body block of either form must be compatible with unit; a diverging body is compatible. A non-unit body tail does not become the value of the loop expression. Thus `while false { 1 }` and `loop { 1 }` are static type errors, while a value can leave loop through `break value`.

`break;` exits the nearest loop with unit. A value-bearing break requires a `loop` target. All break values belonging to a loop must be type-compatible. `continue;` starts the next iteration of the nearest loop (including the next while condition evaluation). Break/continue outside a loop are static errors.

A break or continue appearing in a while condition must target a loop nested inside that condition. Targeting the containing while or any outer loop is a static error. For example, `while loop { break false; } {}` is allowed. Syntactic loop nesting determines the target.

```rust,ignore
fn until(limit: i32) -> i32 {
    let mut n = 0;
    loop {
        if n == limit { break n; }
        n += 1;
    }
}
```

Diverging expressions and unreachable code follow [Never type](../types/never.md).
