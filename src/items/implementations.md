# Implementations

r[items.impl.syntax]
```grammar,items
Implementation -> InherentImpl

InherentImpl -> `impl` Type `{` AssociatedItem* `}`
```

An inherent impl defines associated items for a user-defined named-field struct. The implementing [Type] must resolve to that struct; a different kind of target is a static error. Type parentheses retain their ordinary meaning, so `impl S { ... }` and `impl (S) { ... }` select the same struct.

Multiple impl blocks share the same associated-item namespace. Duplicate member names are static errors across blocks. See [inherent impls and receivers](../items.md#inherent-impls-and-receivers).
