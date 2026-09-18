# Reference types

```grammar,types
ReferenceType -> `&` Lifetime? `mut`? TypeNoBounds
```

A reference type begins with `&`, an optional [Lifetime], optional `mut`, and the referenced type. Examples are `&i32`, `&'a i32`, `&'a mut Vec<i32>`, and `&'_ i32`.

Shared and mutable references are distinct types. Struct fields use these same reference forms. Lifetime annotations and elision follow the [lifetime validity rules](../references.md#lifetime-validity); tests guarantee that those rules and borrow validity hold. Ordinary type and place-mutability checks are required.

In type context `&&T` is two reference constructors, by contextual splitting of the combined token. The same applies recursively inside arrays and Box/Vec type arguments.
