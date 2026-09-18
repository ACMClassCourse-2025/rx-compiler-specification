# Struct expressions

r[expr.struct.syntax]
```grammar,expressions
StructExpression -> PathInExpression `{` StructExprFields? `}`

StructExprFields -> StructExprField (`,` StructExprField)* `,`?

StructExprField -> IDENTIFIER `:` Expression
```

This keeps the named-field branch of Rust's struct-expression grammar. The path must be one unqualified struct name or `Self`, resolved in the type namespace. Field shorthand, numeric tuple fields, functional update (`..base`), and attributes are removed.

Each declared field must appear exactly once. Initializers are evaluated in source order, even when it differs from declaration order. At condition boundaries, the [Rust condition parsing restriction](if-expr.md) distinguishes the struct initializer brace from the body brace.

```rust,ignore
struct Point { x: i32, y: i32 }
fn main() {
    let point = Point { y: 2, x: 1 };
    printlnInt(point.x);
}
```
