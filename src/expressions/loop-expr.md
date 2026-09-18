# Control flow and never

## If

See [If expressions](if-expr.md) for the grammar, condition/body disambiguation, and branch typing.

## While and loop

```grammar,expressions
LoopExpression -> InfiniteLoopExpression | PredicateLoopExpression

InfiniteLoopExpression -> `loop` BlockExpression

PredicateLoopExpression -> `while` Conditions BlockExpression

BreakExpression -> `break` Expression?

ContinueExpression -> `continue`
```

`while condition { ... }` reevaluates a bool condition before each iteration and has unit result. Parentheses around the condition are optional. `loop { ... }` repeats until control flow leaves it; break values determine the loop expression's result type. The body block of either form must be compatible with unit; a diverging body is compatible. A non-unit body tail does not become the value of the loop expression. Thus `while false { 1 }` and `loop { 1 }` are static type errors, while a value can leave loop through `break value`.

`break;` exits the nearest loop with unit. A value-bearing break is allowed in loop but not while. All break values belonging to a loop must be type-compatible. `continue;` starts the next iteration of the nearest loop (including the next while condition evaluation). Break/continue outside a loop are static errors. Loop labels and labeled blocks are not added by this revision.

Following Rust, an unlabeled break or continue appearing in a while condition cannot target that while or any loop outside the condition; such a use is a static error. A loop expression nested inside the condition creates its own target, so `while loop { break false; } {}` is allowed. This is determined from the syntactic loop nesting and does not require reachability analysis.

```rust,ignore
fn until(limit: i32) -> i32 {
    let mut n = 0;
    loop {
        if n == limit { break n; }
        n += 1;
    }
}
```

There is no for loop, range iterator, while-let, or iterator protocol.

## Return

See [Return expressions](return-expr.md) for the grammar and return-value rules.

## Never and unreachable code

Return, break, and continue expressions do not produce a value along their normal successor path and have never behavior, conventionally written `!`. A loop with no associated break expressions has never type. The presence and types of breaks determine typing without requiring proof that a particular branch executes.

Never can fit an expected result type in the supported coercion contexts. This permits an if branch to return early while another branch yields a value. Unit and never are distinct: a unit call returns normally, whereas a return expression does not.

```rust,ignore
fn choose_or_return(flag: bool) -> i32 {
    let n: i32 = if flag { return 7; } else { 3 };
    n + 1
}
```

Unreachable code is not itself an error, but it is still subject to ordinary name/type rules. A diverging expression must be represented correctly in control flow; deleting the old unreachable-code rejection does not permit executing statements after return.

The implementation need not expose a particular never representation in its AST/IR or prove termination. Never is not a user-written type: explicit `!` annotations are unsupported in all type positions, including `fn f() -> !` and `let x: ! = ...`.

A diverging body can still fit an ordinary declared result, for example `fn forever() -> i32 { loop {} }`. A call to that function has its declared type i32; the compiler need not infer a never result across function boundaries. The ban on explicit annotations does not change the never behavior of return, break, continue, or a loop with no breaks, and does not remove the unary `!` operator.
