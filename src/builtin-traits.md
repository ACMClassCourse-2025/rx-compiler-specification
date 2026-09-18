# Builtin traits

The compiler recognizes `Copy`, `Clone`, `PartialEq`, and `Eq`. Structs request them through [derive attributes](traits-and-attributes.md#attribute-grammar). The only builtin trait operations are `.clone()`, `==`, and `!=`; ordering and arithmetic use [Operators](expressions/operator-expr.md).

## Copy

**Functionality.** Reading a place copies a `Copy` value and moves any other value. This applies to initialization, assignment, by-value arguments and receivers, returns, and field reads. A move makes its source unavailable until reinitialized; it never creates an implicit alias. `Copy` is a marker and adds no method.

Copies are semantically independent. Copying a shared reference copies the reference, not its target. A machine-level address-passing convention must preserve by-value semantics. Moves may reuse storage when no observable behavior changes.

Tests exclude reading uninitialized values, use after move, double move, invalid partial move, and moving non-`Copy` values from borrowed contents or array indices. The compiler need not diagnose these ownership violations, but valid moves across branches, fields, and early returns must work.

**Requirements.** Deriving `Copy` requires every field to be `Copy` and `Clone` to be explicitly requested for the struct. `Copy` remains a separate request even when byte copying can implement a clone.

**Example.** Copy a small value while keeping the original usable.

```rust,ignore
#[derive(Clone, Copy)]
struct Point { x: i32, y: i32 }

fn main() {
    let original = Point { x: 1, y: 2 };
    let mut copied = original;
    copied.x = 9;
    printInt(original.x); // 1
}
```

## Clone

**Functionality.** `Clone` provides `fn clone(&self) -> Self`. Derived clone recursively clones fields and elements into an independent value. Scalars and shared references are copied; a shared reference's target is not cloned. [Container cloning](heap.md#clone-and-equality) defines owned heap contents.

An explicit `.clone()` uses [method lookup](expressions/method-call-expr.md#method-lookup). Multiple matching methods, including same-named inherent methods or clones at different dereference levels, make the call course UB. Compiler-generated field clones call the field's builtin operation directly.

**Requirements.** Deriving `Clone` requires every field to be `Clone`. Mutable references are not `Clone`.

**Example.** Duplicate owned heap data so that each value can change independently.

```rust,ignore
#[derive(Clone)]
struct Buffer { value: Box<i32> }

fn main() {
    let original = Buffer { value: Box::<i32>::new(7) };
    let mut cloned = original.clone();
    *cloned.value = 9;
    printInt(*original.value); // 7
}
```

## PartialEq

**Functionality.** `PartialEq` enables `==` and `!=`, which borrow their operands. Scalars compare values; arrays compare corresponding elements; derived structs compare fields; references compare their targets, not their addresses. Padding is ignored, so whole-object `memcmp` is not generally valid. `Box` and `Vec` follow [container equality](heap.md#clone-and-equality).

**Requirements.** Deriving `PartialEq` requires every field to be `PartialEq`. Both operands must have exactly the same source type with `PartialEq`; equality does not coerce them to a common type or supply an expected integer type to either operand. Thus `1 == 1i32` is valid, while `1 == 1u32` is course UB under the [cross-type equality guarantee](undefined-behavior.md#cross-type-equality).

**Example.** Compare records without consuming them.

```rust,ignore
#[derive(PartialEq)]
struct Reading { value: i32 }

fn main() {
    let first = Reading { value: 7 };
    let second = Reading { value: 7 };
    let same = first == second; // true
    let different = first != second; // false; both remain usable
}
```

## Eq

**Functionality.** `Eq` marks equality as an equivalence relation: reflexive, symmetric, and transitive. It adds no operation; comparisons still use `PartialEq`.

**Requirements.** Deriving `Eq` requires every field to be `Eq` and `PartialEq` to be explicitly requested for the struct.

**Example.** Declare that record identifiers have equivalence-based equality.

```rust,ignore
#[derive(PartialEq, Eq)]
struct Key { id: i32 }

fn same_key(left: &Key, right: &Key) -> bool {
    left == right // compares id fields
}
```

## Trait capability summary

| Type | `Copy` | `Clone` | `PartialEq` / `Eq` with the same source type |
| --- | --- | --- | --- |
| Integers, `bool` | Yes | Yes | Both |
| `&T` | Yes | Yes | Requires the corresponding capability of `T` |
| `&mut T` | No | No | Requires the corresponding capability of `T` |
| `[T; N]` | If `T` is `Copy` | If `T` is `Clone` | Requires the corresponding capability of `T` |
| Struct | Valid explicit derive | Valid explicit derive | Valid explicit derive |
| `Box<T>`, `Vec<T>` | No | If `T` is `Clone` | Requires the corresponding capability of `T` |

Zero-sized data follows the [zero-sized-data guarantee](undefined-behavior.md#zero-sized-data). Capability checking must terminate across recursive type graphs while enforcing every field requirement. For example, `struct Node { children: Vec<Node> }` may derive `Clone`, `PartialEq`, and `Eq` when all other fields qualify.
