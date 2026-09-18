# Struct expressions

r[expr.struct.syntax]
```grammar,expressions
StructExpression -> PathInExpression `{` StructExprFields? `}`

StructExprFields -> StructExprField (`,` StructExprField)* `,`?

StructExprField -> IDENTIFIER `:` Expression
```

A struct expression gives a path followed by a brace-delimited list of `field: expression` initializers. The path must resolve in the type namespace to a declared named-field struct. The supported forms are a struct name with permitted lifetime arguments or elision, such as `View::<'a> { value: x }` or `View { value: x }`, and `Self`. [Path resolution](../paths.md#path-resolution) determines the named struct, while lifetime validity is guaranteed by tests.

Each declared field must appear exactly once with a value compatible with its declared type. Unknown fields and incompatible values are compile errors. Initializers follow the [ordinary evaluation order](../expressions.md#evaluation-order). At condition boundaries, the [condition parsing rule](if-expr.md) distinguishes the struct initializer brace from the body brace.

```rust,ignore
struct Point { x: i32, y: i32 }
fn main() {
    let point = Point { y: 2, x: 1 };
    printlnInt(point.x);
}
```
