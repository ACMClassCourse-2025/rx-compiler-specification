# Box and Vec

[Box and Vec types](types/heap.md) defines container composition. This chapter defines their operations, references, and allocation lifetime. The [capability table](builtin-traits.md#derive) defines `Copy`, `Clone`, and equality support.

## Constructors and type arguments

Constructors require explicit concrete type arguments:

```rust,ignore
let boxed = Box::<i32>::new(7);
let mut values = Vec::<i32>::new();
```

`Box::<T>::new(value)` moves or copies one coercible `T` into owned heap storage. `Vec::<T>::new()` creates an empty sequence. Every nested container constructor has its own explicit concrete type argument. Neither constructor requires `T` to implement a builtin trait. Lifetime arguments inside `T` follow the [lifetime rules](references.md#lifetime-validity); type-position `_` is unsupported.

## Box access and moves

For `b: Box<T>`, `*b` is a `T` place. It is mutable through a mutable owner or mutable reference and read-only through shared access. Normal field, index, method, and borrow operations apply after supported `Box` and reference dereferences.

Reading `Copy` contents copies them. Reading non-`Copy` contents from an owned `Box` moves them; moving through a borrowed `Box` is excluded by the ownership guarantee. An allocation whose contents were moved may remain allocated until program end.

## Vec operations

| Operation | Receiver and arguments | Result and behavior |
| --- | --- | --- |
| `len()` | `&self` | `usize` initialized element count |
| `is_empty()` | `&self` | Whether length is zero |
| `push(value)` | `&mut self`, `value: T` | Append by copy or move; return unit |
| `remove(index)` | `&mut self`, `index: usize` | Move out one element, shift later elements left, and return it |
| `v[index]` | `index: usize` | Shared or mutable element place according to access |

Indexing and removal require `index < len`; out-of-bounds execution is excluded with no required check or panic. `remove` preserves order and supports non-`Copy`, non-`Clone` elements. Reading a `Copy` indexed element copies it; directly moving a non-`Copy` indexed element is excluded, so use `remove` instead. Assignment to a mutable indexed place is supported.

`Vec` itself is not dereferenceable and never coerces to an element or slice reference.

## Clone and equality

Cloning a `Box` clones its stored value into independently owned storage. Cloning a `Vec` clones elements in order into an independent sequence of equal length. Owned nested containers clone recursively; shared references are copied without cloning their targets.

`Box` equality compares stored values. `Vec` equality compares length and corresponding elements, ignoring allocation address, capacity, and padding. These operations compose with struct derives and finite values of recursive types.

## Storage and references

A `Vec` stores exactly its initialized sequence; spare capacity is not initialized data. Capacity and growth are unobservable implementation choices. Reallocation preserves values and order but invalidates references into the old buffer. `remove` invalidates references to removed or shifted elements.

Tests obey Rust's borrowing rules, including supported two-phase receiver borrowing such as `v.push(v.len())`. They never use an element reference across a conflicting mutable container operation, regardless of spare capacity. No borrow checker is required.

Zero-sized heap objects are course UB under the [zero-sized-data guarantee](undefined-behavior.md#zero-sized-data). An empty `Vec<T>` for nonzero-sized `T` is valid and need not have a dereferenceable buffer. Valid tests fit the target size representation and do not depend on allocation failure.

## Program-end reclamation

Allocations may remain until execution ends and then be reclaimed together. This applies after scope exit, overwrite, move-out, return, jumps, and `Vec` buffer replacement. A runtime arena reset, environment cleanup, or bump allocator is sufficient; generated code need not walk object graphs or track per-value cleanup.

This storage policy does not change source ownership or reference validity. Moves still transfer ownership, clones still create independent owned contents, and invalidated references remain unusable while bytes are retained.

Earlier reclamation or allocation elimination is allowed when behavior and valid references are preserved. Tests fit the cumulative allocation budget even when old storage is never reclaimed during execution. The [backend resource guarantees](backend.md#resource-guarantees) define the allocator ABI, reference growth strategy, and memory limits.
