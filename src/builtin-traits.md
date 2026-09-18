# Copy, move, and builtin traits

The compiler recognizes four builtin traits: `Copy`, `Clone`, `PartialEq`, and `Eq`.

## Value semantics

Reading a place copies a `Copy` value and moves any other value. The same rule applies to initialization, assignment, by-value arguments and receivers, returns, and field reads. A move makes its source unavailable until reinitialized; it never creates an implicit alias.

Tests exclude ownership violations such as reading uninitialized values, use after move, double move, invalid partial move, and moving non-`Copy` values from borrowed contents or array indices. The compiler need not diagnose them, but valid moves across branches, fields, and early returns must work.

Copies are semantically independent. Copying a shared reference copies the reference, not its target. A machine-level address-passing convention must still preserve by-value semantics. Moves may reuse storage when no observable behavior changes.

## Derive

```grammar,attributes
OuterAttribute -> `#` `[` DeriveAttribute `]`

DeriveAttribute -> `derive` `(` (DeriveName (`,` DeriveName)* `,`?)? `)`

DeriveName -> `Copy` | `Clone` | `PartialEq` | `Eq`
```

Zero or more `#[derive(...)]` attributes may precede a top-level named-field struct. Lists may be empty or end with a comma. Repeating a capability, within or across attributes, is a static error.

| Requested capability | Requirement |
| --- | --- |
| `Clone` | Every field is `Clone` |
| `Copy` | `Clone` is also requested and every field is `Copy` |
| `PartialEq` | Every field is `PartialEq` |
| `Eq` | `PartialEq` is also requested and every field is `Eq` |

| Type | `Copy` | `Clone` | `PartialEq` / `Eq` with the same source type |
| --- | --- | --- | --- |
| Integers, `bool` | Yes | Yes | Both |
| `&T` | Yes | Yes | Requires the corresponding capability of `T` |
| `&mut T` | No | No | Requires the corresponding capability of `T` |
| `[T; N]` | If `T` is `Copy` | If `T` is `Clone` | Requires the corresponding capability of `T` |
| Struct | Valid explicit derive | Valid explicit derive | Valid explicit derive |
| `Box<T>`, `Vec<T>` | No | If `T` is `Clone` | Requires the corresponding capability of `T` |

Zero-sized data follows the [zero-sized-data guarantee](undefined-behavior.md#zero-sized-data). Capability checking must terminate across recursive type graphs while enforcing every field requirement. For example, `struct Node { children: Vec<Node> }` may derive `Clone`, `PartialEq`, and `Eq` when all other fields qualify.

## Clone

A `Clone` value supports `fn clone(&self) -> Self`. Derived clone recursively clones fields and elements into an independent value. Scalars and shared references are copied; a shared reference's target is not cloned. Mutable references are not `Clone`.

An explicit `.clone()` uses normal [method lookup](expressions/method-call-expr.md#method-lookup), including same-named inherent methods. Compiler-generated field clones call the field's builtin operation directly. `Copy` remains a separate requested capability even when byte copying can implement a clone. [Container cloning](heap.md#clone-and-equality) defines owned heap contents.

## Equality

`==` and `!=` require operands with exactly the same inferred source type and that type's `PartialEq` capability. The operands constrain one another while still unresolved, so an unsuffixed literal may acquire the other operand's integer type; already determined types do not convert. Different source types are course UB under the centralized [cross-type equality guarantee](undefined-behavior.md#cross-type-equality).

Equality borrows rather than consumes its operands. Scalars compare values; arrays compare corresponding elements; derived structs compare fields; references compare their targets, not their addresses. Padding is ignored, so whole-object `memcmp` is not generally valid. `Box` and `Vec` equality is defined by [container equality](heap.md#clone-and-equality).

`Eq` marks equality as an equivalence relation. The only builtin trait operations are `.clone()`, `==`, and `!=`; ordering and arithmetic use [Operators](expressions/operator-expr.md).
