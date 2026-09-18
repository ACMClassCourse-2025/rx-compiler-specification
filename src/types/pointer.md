# Reference types

```grammar,types
ReferenceType -> `&` `mut`? TypeNoBounds
```

This is Rust's reference-type production with the lifetime-annotation branch removed. Raw pointers and function pointers are unsupported.

`&T` and `&mut T` remain distinct types. Struct fields may contain references without lifetime parameters. Borrow validity is guaranteed by tests; the compiler need not infer or check lifetimes. Ordinary type and place-mutability checks remain required. See [references and storage duration](../references.md).

In type context `&&T` is two reference constructors, by contextual splitting of the combined token. The same applies recursively inside arrays and Box/Vec type arguments.
