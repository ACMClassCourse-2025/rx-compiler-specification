# Implementations

r[items.impl.syntax]
```grammar,items
Implementation -> InherentImpl

InherentImpl -> `impl` IDENTIFIER `{` AssociatedItem* `}`
```

Only Rust's inherent-impl branch remains. The implementing identifier must resolve to a user-defined named-field struct. Generics, where clauses, trait impls, inner attributes, and impls on builtin types are unsupported.

Multiple impl blocks share the same associated-item namespace. Duplicate member names are static errors across blocks. See [inherent impls and receivers](../items.md#inherent-impls-and-receivers).
