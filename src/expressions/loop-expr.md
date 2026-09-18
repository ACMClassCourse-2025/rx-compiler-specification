# Loops and never

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

## Never and unreachable code

Return, break, and continue expressions do not produce a value along their normal successor path and have never behavior, conventionally written `!`. A loop with no associated break expressions has never type. The presence and types of breaks determine typing without requiring proof that a particular branch executes.

Never can fit an expected result type in the supported coercion contexts. This permits an if branch to return early while another branch yields a value. Unit and never are distinct: a unit call returns normally, whereas a return expression does not.

```rust,ignore
fn choose_or_return(flag: bool) -> i32 {
    let n: i32 = if flag { return 7; } else { 3 };
    n + 1
}
```

Unreachable code is accepted subject to ordinary name/type rules. Control flow must preserve the behavior of diverging expressions, including skipping the remainder of a block after a return.

The implementation need not expose a particular never representation in its AST/IR or prove termination. User-written annotations follow the [source-type rules](../types.md#supported-types).

A diverging body can fit an ordinary declared result, for example `fn forever() -> i32 { loop {} }`. A call to that function has its declared type i32. Never behavior is determined within each function body. The expression operator `!` performs boolean or bitwise negation as specified in [Operators](operator-expr.md).
