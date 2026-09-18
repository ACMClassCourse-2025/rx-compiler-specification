# Box and Vec

[Box and Vec types](types/heap.md) defines container composition. This chapter defines their operations, references, and allocation lifetime. The [capability table](builtin-traits.md#trait-capability-summary) defines `Copy`, `Clone`, and equality support.

## Builtin signatures

In these signatures, `T` stands for the container's concrete type argument.
`&self` takes a shared reference to the container; `&mut self` takes a mutable
reference. `new` is an associated function with no receiver. The other entries are methods and
follow the ordinary [method-call rules](expressions/method-call-expr.md).
None of the operations in this table requires a builtin trait capability of
`T`; arguments follow the ordinary [call and coercion rules](expressions/call-expr.md).

| Container | Signature | Behavior |
| --- | --- | --- |
| `Box<T>` | `fn new(value: T) -> Box<T>` | Moves or copies `value` into new owned storage. |
| `Vec<T>` | `fn new() -> Vec<T>` | Creates an empty vector. |
| `Vec<T>` | `fn len(&self) -> usize` | Returns the number of initialized elements. |
| `Vec<T>` | `fn is_empty(&self) -> bool` | Returns whether the length is zero. |
| `Vec<T>` | `fn push(&mut self, value: T) -> ()` | Appends one element by copy or move. |
| `Vec<T>` | `fn remove(&mut self, index: usize) -> T` | Moves out and returns the indexed element, shifting later elements left. |

Constructors require explicit concrete type arguments:

```rust,ignore
let boxed = Box::<i32>::new(7);
let mut values = Vec::<i32>::new();
```

Every nested container constructor has its own explicit concrete type argument.
Lifetime arguments inside `T` follow the [lifetime rules](references.md#lifetime-validity);
type-position `_` is unsupported.

## Box access and moves

For `b: Box<T>`, the builtin dereference expression `*b` is a `T` place.
It is mutable through a mutable owner or mutable reference and
read-only through shared access. Normal field, index, method, and borrow
operations apply after supported `Box` and reference dereferences.

Reading `Copy` contents copies them. Reading non-`Copy` contents from an owned `Box` moves them; moving through a borrowed `Box` is excluded by the ownership guarantee. An allocation whose contents were moved may remain allocated until program end.

## Vec operations

For `v: Vec<T>` and `index: usize`, the builtin index expression `v[index]`
is a `T` place. It is mutable through mutable access and read-only through
shared access.

Indexing and removal require `index < len`; out-of-bounds execution is excluded with no required check or panic. `remove` preserves order and supports non-`Copy`, non-`Clone` elements. Reading a `Copy` indexed element copies it; directly moving a non-`Copy` indexed element is excluded, so use `remove` instead. Assignment to a mutable indexed place is supported.

`Vec` itself is not dereferenceable and never coerces to an element or slice
reference.

## Clone and equality

When `T` is `Clone`, the containers support these builtin `Clone` methods:

| Type | Method signature | Result |
| --- | --- | --- |
| `Box<T>` | `fn clone(&self) -> Box<T>` | Clones the stored `T` into independently owned storage. |
| `Vec<T>` | `fn clone(&self) -> Vec<T>` | Clones elements in order into an independent vector of equal length. |

Owned nested containers clone recursively; shared references are copied without
cloning their targets. The `Clone` capability is specified in [Builtin traits](builtin-traits.md#clone).
Dot calls remain subject to the [competing-method exclusion](expressions/method-call-expr.md#method-lookup): for example, use `Box::<i32>::clone(&boxed)` to select the box's clone explicitly.

`Box` equality compares stored values. `Vec` equality compares length and corresponding elements, ignoring allocation address, capacity, and padding. These operations compose with struct derives and finite values of recursive types.

## Storage and references

A `Vec` stores exactly its initialized sequence; spare capacity is not initialized data. Capacity and growth are unobservable implementation choices. Reallocation preserves values and order but invalidates references into the old buffer. `remove` invalidates references to removed or shifted elements.

Tests obey Rust's borrowing rules, including supported two-phase receiver borrowing such as `v.push(v.len())`. They never use an element reference across a conflicting mutable container operation, regardless of spare capacity. No borrow checker is required.

Zero-sized heap objects are course UB under the [zero-sized-data guarantee](undefined-behavior.md#zero-sized-data). An empty `Vec<T>` for nonzero-sized `T` is valid and need not have a dereferenceable buffer. Valid tests fit the target size representation and do not depend on allocation failure.

## Heap cleanup

Student-generated code does not need to free heap allocations. Heap leaks are
allowed: storage from `Box` and `Vec` may remain allocated after scope exit,
overwrite, move-out, return, jumps, or `Vec` buffer replacement. No per-value
destruction, deallocation call, or final heap-cleanup code is required.

This does not change source ownership or reference validity. Moves still
transfer ownership, clones still create independent owned contents, and a
reference invalidated by reallocation or removal remains invalid even if the
old bytes are still present.

An implementation may reclaim or reuse storage earlier when it preserves
program behavior and valid references. Tests fit the cumulative heap budget
defined by the [backend resource guarantees](backend.md#resource-guarantees).
