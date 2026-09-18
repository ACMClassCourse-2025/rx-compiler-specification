# Reference types

```grammar,types
ReferenceType -> `&` `mut`? TypeNoBounds
```

A reference type begins with `&`, optionally followed by `mut`, and specifies the referenced type.

`&T` and `&mut T` are distinct types. Struct fields use these same reference forms. Borrow validity is guaranteed by tests; the compiler need not infer or check lifetimes. Ordinary type and place-mutability checks are required. See [references and storage duration](../references.md).

In type context `&&T` is two reference constructors, by contextual splitting of the combined token. The same applies recursively inside arrays and Box/Vec type arguments.
