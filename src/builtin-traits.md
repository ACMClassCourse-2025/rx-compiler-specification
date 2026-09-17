# Copy, move, and builtin traits

The compiler recognizes exactly Copy, Clone, PartialEq, and Eq for the operations defined here. There are no user trait declarations, trait impls, generic bounds, trait objects, or user-defined operator overloads. No general trait solver or macro expander is required.

## Value semantics

When a place produces a value, a Copy type is copied; otherwise it is moved. This applies uniformly to let initialization, assignment, by-value arguments and receivers, returns, and field reads. Non-Copy bindings do not become implicit aliases.

A move makes the source value unavailable until it is initialized again. Tests exclude use-after-move, double move, invalid partial moves, and moves from borrowed contents or non-Copy indexed array elements. The compiler need not diagnose these ownership violations. It must still implement correct value semantics on valid programs.

Copy produces a semantically independent value. Copying a shared reference copies that reference, not its referent. Passing an array/struct by an address at machine level must not let the callee modify a Copy value which the caller can still observe. Move may reuse storage when observationally equivalent; it does not require clearing source memory.

Box and Vec are non-Copy. Their operations, including Box move-out and Vec remove, are specified in [Heap](heap.md). Heap storage may remain until program termination; generating recursive per-value cleanup is not required.

## Derive

```rust,ignore
#[derive(Clone, PartialEq, Eq)]
struct Record {
    value: i32,
}
```

Use Rust's outer `#[derive(...)]` form before a top-level named-field struct. Zero or more such attributes may occur, and their lists are combined. Each list contains unqualified names from Copy, Clone, PartialEq, Eq in any order, with an optional trailing comma; an empty `#[derive()]` has no effect. Requesting the same capability twice, within one list or across attributes, is a static error. Qualified derive paths, other attributes, and general macros remain outside this subset.

| Requested capability | Requirements |
| --- | --- |
| Clone | Every field is Clone; Copy is not required |
| Copy | The struct also has Clone and every field is Copy |
| PartialEq | Every field is PartialEq; no Copy/Clone requirement |
| Eq | The struct also has PartialEq and every field is Eq |

A struct does not obtain these capabilities merely because its fields have them. Derives do not implicitly add other derives: Copy alone lacks Clone; Eq alone lacks PartialEq. Invalid requests are ordinary static errors, not ownership-checking tasks.

| Type | Copy | Clone | PartialEq / Eq with the identical source type |
| --- | --- | --- | --- |
| Supported integers, bool | Yes | Yes | Both |
| `&T` | Yes, regardless of T | Yes, regardless of T | Require the respective capability of T |
| `&mut T` | No | No | Require the respective capability of T |
| `[T; N]` | If T is Copy | If T is Clone | Require the respective capability of T |
| Struct | By valid explicit derive | By valid explicit derive | By valid explicit derive |
| Box<T> / Vec<T> | No | If T is Clone | Require the respective capability of T |

Zero-sized data remains excluded. Capability requirements form a finite graph of declared types and builtin operations. References and supported recursive container types can create cycles in this graph. Checking them must terminate without granting an undeclared derive or ignoring a failing field requirement; a cycle alone is not a reason to reject a derive. For example, a Node deriving Clone/PartialEq/Eq with a Vec<Node> field has those capabilities when its other fields meet the requirements. This is not a requirement for a general trait solver.

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

An explicit method call uses the [Rust method candidate order](names.md#method-lookup) within the supported subset. For a Clone struct S without an inherent clone, `r.clone()` on `r: &S` selects S's clone and returns S; it is not implemented by always copying the receiver's unadjusted type. Same-named inherent methods participate under that ordering. Generated field clones use the field's builtin Clone operation, not a same-named inherent method. Trait-associated path forms such as `Clone::clone` are unsupported.

With no owned-resource fields, copying bytes may implement the required clone result when equivalent. This does not turn Clone-only types into Copy. Box/Vec clone recursively clones the owned contents into independent storage; shallow pointer duplication is not sufficient. Shared-reference contents still share their referents. Additional standard library methods such as clone_from are unsupported.

## Equality

`==` / `!=` are specified only when the two operands have exactly the same source type after ordinary local type inference and that type has PartialEq. Equality contributes the constraint that two still-inferred operand types agree, so an unsuffixed literal may acquire the other operand's integer type. It does not coerce two already determined, different types. Tests exclude every remaining cross-type equality expression as course UB; no diagnostic, coercion, or cross-type PartialEq implementation is required. In particular, the excluded cases include shared versus mutable references, different array lengths or element types, different Box/Vec element types, and Vec versus array comparisons, even where full Rust provides such a comparison.

For identical operand types, equality borrows its operands and does not consume non-Copy values. Scalars compare values, arrays of the same type compare elements, and two values of the same derived struct type compare fields. References of the same reference type compare their targets, not their addresses. These operations ignore padding; whole-object memcmp is not generally a correct implementation.

Two Box<T> values compare their contents, and two Vec<T> values compare lengths and corresponding elements under the conditional rules above. Eq adds no runtime operation. PartialOrd, Ord, Ordering, and Option are not added as source-level traits/types. Scalar ordering and its separately specified reference variants remain finite builtin operations, as do primitive arithmetic and bit operations; they do not enable additional derives or user operator impls. The exposed trait interface is `.clone()`, `==`, and `!=`; other builtin trait method/path forms, including `.eq()` and `.ne()`, are unsupported.
