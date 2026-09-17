# Bindings, statements, and blocks

## Identifier bindings

```text
Binding      := mut? IDENTIFIER
LetStatement := let Binding (: Type)? = Expression ;
Parameter    := Binding : Type
```

Every let requires an initializer. The binding is a single identifier, optionally mutable; there is no destructuring, `_`, `ref`, or reference pattern. Reference types and borrow expressions remain supported.

Let and ordinary parameter bindings that collide with a visible unqualified const name are course UB and absent from all tests, as specified in [Names](names.md#settled-scope-rules). No constant-pattern handling or collision diagnostic is required.

A local type annotation may be omitted. Type constraints from later uses in the same function can determine an earlier binding's type. An annotation fixes the type; it does not request an implicit numeric conversion. An immutable binding cannot be assigned after initialization. Assignment to a mutable place must preserve its type.

A binding's name is visible after its let statement through the rest of the enclosing block, subject to shadowing. The initializer sees the previous environment. Shadowing introduces a distinct binding and does not overwrite an older object's storage merely because the names match.

## Blocks and statements

A block executes its statements in order and may end with a tail expression. Its result is the tail expression's value, or `()` when there is no tail. Control flow which does not reach the end follows the [never rules](expressions/loop-expr.md#never-and-unreachable-code).

Statements include initialized lets, expression statements, and empty `;` statements. Items are not statements. A semicolon after an expression discards its result without discarding its effects.

An expression with an outer block/control-flow form can be a statement without a semicolon. If it is a statement without a semicolon, it must have unit type or diverge. A final expression used as the enclosing block's tail can have a non-unit type. These are the Rust distinctions, including the expression-statement parsing rule below.

```rust,ignore
fn select(flag: bool) -> i32 {
    let base = if flag { 10 } else { 20 };
    { base + 1 }
}
```

## Statement boundary

At a position where an expression statement is being parsed, an expression with an outer block form is completed as that statement rather than greedily consuming a following infix operator. In an initializer or other value-expression context, the expression continues normally. Parentheses can force an ordinary expression context.

This is Rust's syntactic disambiguation, not a literal stop at the first `}`. An attached else/else-if remains part of its if expression, and Rust-permitted field/method postfix continuations still apply. Other expression statements run to their semicolon; the enclosing block can instead end with a final tail expression before `}`. For example, `{ make() }.value;` can remain a field-access expression statement when make returns a suitable struct.

```rust,ignore
let value = if true { 10 } else { 20 } - 1; // initializer is the whole subtraction
if true {} else {} -1;                      // if statement, then -1 expression statement
(if true { 10 } else { 20 }) - 1;            // one expression statement
```

The parser-facing grammar and examples must be checked against the supplied parser before publication. The rule is settled Rust-compatible block/statement behavior; the remaining work is implementation compatibility, not another language-design choice.

## Assignment destinations

An assignment destination must be a single place: a variable, field, array element, or dereference, possibly parenthesized. Its type and mutability must be valid. Destructuring assignment is unsupported, including array and struct assignees. The underscore assignee `_ = expr` is also unsupported. These are static language-subset restrictions, not course UB.

Assigning an entire struct or array to a variable remains supported: `s = other;` and `a = other_array;` assign one aggregate value to one place. They do not destructure the value into several destinations.
