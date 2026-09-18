# Paths

## Paths in expressions

```grammar,paths
PathInExpression ->
      PathIdentSegment
    | `self`
    | PathIdentSegment `::` PathExprSegment

PathIdentSegment -> IDENTIFIER | `Self`

PathExprSegment -> IDENTIFIER
```

These are the Reference's expression-path productions restricted to the finite course namespace. Unqualified value names and `self` are values; a bare `Self` path is available for struct construction but is not a standalone value; the prefix in `Type::member` or `Self::member` resolves in the type namespace. Longer module paths, leading `::`, trait-qualified paths, and generic user paths are absent.

## Paths in types

```grammar,paths
TypePath -> TypePathSegment

TypePathSegment -> PathIdentSegment | HeapType
```

An identifier names a primitive or a declared struct; `Self` names the surrounding struct. [HeapType] is the sole retained type-argument form. The broad identifier branch does not permit `Box` or `Vec` without their required element type. There are no type aliases, associated types, user generics, or type-position `_`.

## Explicit builtin constructors

[HeapConstruction] is the restricted turbofish path used only by `Box::<T>::new(value)` and `Vec::<T>::new()`. It does not permit `Box::new`, `Vec::new`, or other generic calls. See [Names](names.md#supported-paths) for lookup and builtin-name protection.
