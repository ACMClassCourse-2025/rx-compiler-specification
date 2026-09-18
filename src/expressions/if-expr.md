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

The condition must be bool. Parentheses are optional. A struct construction at the condition/body boundary uses the delimiters described below. `else if` chains are supported, and execution selects one branch.

When an unparenthesized `Name {` at the condition/body boundary could begin a struct literal, treat `{` as the start of the consequent block. To put such a struct construction in the condition, delimit it explicitly, for example `if (S { flag: true }).flag { ... }`. A function argument such as `if check(S { flag: true }) { ... }` is already delimited. This rule does not mean every brace after if starts its consequent: `if { true } { ... }` has a block-valued condition and a separate consequent block.

With else, branches must produce compatible result types, allowing a diverging branch to fit the other branch's expected type. Without else, the if expression has unit type; the then block must be compatible with unit.
