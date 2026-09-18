# Implementations

r[items.impl.syntax]
```grammar,items
Implementation -> InherentImpl

InherentImpl -> `impl` GenericParams? Type WhereClause? `{` AssociatedItem* `}`
```

An inherent impl defines associated items for a user-defined named-field struct. The implementing [Type] must resolve to that struct; a different kind of target is a static error. Type parentheses retain their ordinary meaning, so `impl S { ... }` and `impl (S) { ... }` select the same struct.

An impl declares its own lifetime parameters immediately after `impl`, as in `impl<'a> View<'a> { ... }`. Those parameters are in scope throughout its associated items. Methods and associated functions may declare additional lifetime parameters. A [WhereClause] before the opening brace specifies lifetime bounds.

Multiple impl blocks are supported. Their member namespace and duplicate-name checks follow [Names](../names.md#scope-rules). [Associated items](associated-items.md) defines the members; [function receivers](functions.md#receivers) defines methods.

```rust,ignore
impl Point {
    const ZERO: i32 = 0;

    fn total(&self) -> i32 {
        self.x + self.y
    }

    fn translate(&mut self, amount: i32) {
        self.x += amount;
        self.y += amount;
    }
}
```
