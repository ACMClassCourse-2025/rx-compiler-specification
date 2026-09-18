# Copy, move, and builtin traits

The compiler recognizes four builtin traits: Copy, Clone, PartialEq, and Eq. Their capabilities and operations are defined below. Struct declarations request these capabilities with derive attributes, and builtin types obtain them according to the capability table.

## Value semantics

When a place produces a value, a Copy type is copied; otherwise it is moved. This applies uniformly to let initialization, assignment, by-value arguments and receivers, returns, and field reads. Non-Copy bindings do not become implicit aliases.

A move makes the source value unavailable until it is initialized again. Tests exclude reads of uninitialized values, use-after-move, double move, invalid partial moves, and moves from borrowed contents or non-Copy indexed array elements. The compiler need not diagnose these ownership violations. Valid moves, including branch-dependent transfers, partial moves, and early returns, must preserve value semantics.

Copy produces a semantically independent value. Copying a shared reference copies that reference, not its referent. Passing an array/struct by an address at machine level must not let the callee modify a Copy value which the caller can still observe. Move may reuse storage when observationally equivalent; it does not require clearing source memory.

Container move-out operations are specified in [Heap](heap.md), and allocation lifetime follows [program-end reclamation](heap.md#program-end-reclamation).

## Derive

```grammar,attributes
OuterAttribute -> `#` `[` DeriveAttribute `]`

DeriveAttribute -> `derive` `(` (DeriveName (`,` DeriveName)* `,`?)? `)`

DeriveName -> `Copy` | `Clone` | `PartialEq` | `Eq`
```

```rust,ignore
#[derive(Clone, PartialEq, Eq)]
struct Record {
    value: i32,
}
```

Write `#[derive(...)]` before a top-level named-field struct. Zero or more such attributes may occur, and their lists are combined. Each list contains unqualified names from Copy, Clone, PartialEq, Eq in any order, with an optional trailing comma; an empty `#[derive()]` has no effect. Requesting the same capability twice, within one list or across attributes, is a static error.

| Requested capability | Requirements |
| --- | --- |
| Clone | Every field is Clone; Copy is not required |
| Copy | The struct also has Clone and every field is Copy |
| PartialEq | Every field is PartialEq; no Copy/Clone requirement |
| Eq | The struct also has PartialEq and every field is Eq |

A struct obtains each capability through an explicit derive request satisfying the table above. Invalid requests are static errors.

| Type | Copy | Clone | PartialEq / Eq with the identical source type |
| --- | --- | --- | --- |
| Supported integers, bool | Yes | Yes | Both |
| `&T` | Yes, regardless of T | Yes, regardless of T | Require the respective capability of T |
| `&mut T` | No | No | Require the respective capability of T |
| `[T; N]` | If T is Copy | If T is Clone | Require the respective capability of T |
| Struct | By valid explicit derive | By valid explicit derive | By valid explicit derive |
| Box<T> / Vec<T> | No | If T is Clone | Require the respective capability of T |

Zero-sized data remains excluded under the [zero-sized-data guarantee](undefined-behavior.md#zero-sized-data). Capability requirements form a finite graph of declared types and builtin operations. References and recursive container types can create cycles in this graph. Checking terminates by handling cycles while enforcing every explicit derive and field requirement. For example, a Node deriving Clone/PartialEq/Eq with a Vec<Node> field has those capabilities when its other fields meet the requirements.

## Clone

A Clone value supports `.clone()`, with the builtin signature `fn clone(&self) -> Self`. Derivation recursively clones fields/elements, producing a new value. It copies scalar values and shared references, without cloning the objects behind shared references. A mutable reference itself has no Clone capability.

```rust,ignore
fn main() {
    let a = Record { value: 1 };
    let mut b = a.clone();
    b.value = 2;
    printlnInt(a.value); // 1
    let c = a;          // move: Record was not derived Copy
    if c != b { printlnInt(3); }
}
```

An explicit `.clone()` call uses [method lookup](expressions/method-call-expr.md#method-lookup), including receiver adjustments and same-named inherent methods. Generated field clones use the field's builtin Clone operation directly.

With no owned-resource fields, copying bytes may implement the required clone result when equivalent. Copy remains a separately requested capability. [Container cloning](heap.md#clone-and-equality) defines the treatment of owned heap contents.

## Equality

`==` / `!=` compare operands with exactly the same source type after ordinary local type inference, and that type must have PartialEq. Equality constrains two still-inferred operand types to agree, so an unsuffixed literal may acquire the other operand's integer type. Already determined types retain their identities. Equality between different source types is course UB; see the centralized [cross-type equality guarantee](undefined-behavior.md#cross-type-equality).

For identical operand types, equality borrows its operands and does not consume non-Copy values. Scalars compare values, arrays of the same type compare elements, and two values of the same derived struct type compare fields. References of the same reference type compare their targets, not their addresses. These operations ignore padding; whole-object memcmp is not generally a correct implementation.

[Container equality](heap.md#clone-and-equality) defines comparison of Box and Vec contents. Eq marks equality as an equivalence relation. The builtin trait operations are `.clone()`, `==`, and `!=`. Scalar ordering, arithmetic, and bit operations have their own finite operand rules in [Operators](expressions/operator-expr.md).
