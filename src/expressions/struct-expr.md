# Struct expressions

r[expr.struct.syntax]
```grammar,expressions
StructExpression -> PathInExpression `{` StructExprFields? `}`

StructExprFields -> StructExprField (`,` StructExprField)* `,`?

StructExprField -> IDENTIFIER `:` Expression
```

A struct expression gives a path followed by a brace-delimited list of `field: expression` initializers. The path must resolve in the type namespace to a declared named-field struct. The supported forms are an unqualified struct name and `Self`; [path resolution](../paths.md#path-resolution) diagnoses other paths.

Each declared field must appear exactly once. Initializers are evaluated in source order, even when it differs from declaration order. At condition boundaries, the [condition parsing rule](if-expr.md) distinguishes the struct initializer brace from the body brace.

```rust,ignore
struct Point { x: i32, y: i32 }
fn main() {
    let point = Point { y: 2, x: 1 };
    printlnInt(point.x);
}
```
