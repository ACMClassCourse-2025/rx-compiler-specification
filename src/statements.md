# Statements and bindings

```grammar,statements
Statement -> `;` | LetStatement | ExpressionStatement
```

## Identifier bindings

```grammar,statements
LetStatement -> `let` IdentifierBinding (`:` Type)? `=` Expression `;`

IdentifierBinding -> `mut`? IDENTIFIER
```

Every let binds a single identifier to an initializer. An optional `mut` makes the binding mutable. The initializer may produce any supported value, including a reference formed by a borrow expression.

Binding visibility and shadowing follow [Names](names.md#scope-rules). A let binding that collides with a visible unqualified const name is undefined behavior; see the [constant-name collision guarantee](undefined-behavior.md#constant-name-collisions).

The optional annotation and initializer follow [local type inference](types.md#inference) and [coercion rules](types.md#conversions-and-coercions). Subsequent writes follow [assignment destinations](expressions/operator-expr.md#assignment-destinations).

## Expression statements

```grammar,statements
ExpressionStatement ->
      ExpressionWithoutBlock `;`
    | ExpressionWithBlock `;`?
```

An expression statement evaluates an expression and discards its result while
preserving its effects. An empty `;` statement has no effect.

[ExpressionWithoutBlock] requires a semicolon. [ExpressionWithBlock]
(`{ ... }`, `if`, `while`, or `loop`) may omit it. Without the semicolon, the
expression must be compatible with `()`; a diverging expression is also valid.
A block's final expression follows the [tail-expression rules](expressions/block-expr.md).

```rust,ignore
fn show(flag: bool) {
    if flag { print_i32(1); }    // unit result: semicolon optional
    if flag { 1 } else { 2 };   // integer result: semicolon discards it
}
```

## Statement boundary

At a position where an expression statement is being parsed, an expression with an outer block form is completed as that statement rather than greedily consuming a following infix operator. In an initializer or other value-expression context, the expression continues normally. Parentheses can force an ordinary expression context.

An attached else/else-if remains part of its if expression, and field/method postfix continuations apply to a completed block expression. Other expression statements run to their semicolon; the enclosing block can instead end with a final tail expression before `}`. For example, `{ make() }.value;` is a field-access expression statement when make returns a suitable struct.

```rust,ignore
let value = if true { 10 } else { 20 } - 1; // initializer is the whole subtraction
if true {} else {} -1;                      // if statement, then -1 expression statement
(if true { 10 } else { 20 }) - 1;           // one expression statement
```

The supplied parser must implement these statement boundaries and the grammar above.
