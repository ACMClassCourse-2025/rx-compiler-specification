# Box and Vec

[Box and Vec types](types/heap.md) defines the containers and their type
composition. This chapter specifies their source operations and storage policy.
Their Copy, Clone, and equality capabilities follow the
[builtin capability table](builtin-traits.md#derive).

## Constructors and type arguments

Constructors are written `Box::<T>::new(value)` and `Vec::<T>::new()`. They use [CallExpression] with a [PathExpression] callee and the explicit type argument required by [Paths](paths.md#generic-arguments). Each type argument is concrete at every nesting level. Lifetime arguments within it follow the [lifetime elision rules](references.md#lifetime-validity), as in `Vec::<&'a i32>::new()` and `Vec::<View<'_>>::new()`. The lifetime placeholder `'_` is distinct from type-position `_`.

```rust,ignore
let b = Box::<i32>::new(7);
let mut values = Vec::<i32>::new();
values.push(3);
let mut nested: Vec<Vec<i32>> = Vec::<Vec<i32>>::new();
nested.push(values);
```

Constructor results participate in [ordinary inference](types.md#inference), field access, indexing, method calls, borrowing, and dereferencing according to their type.

Box construction takes one argument of the stated T, allowing the specified argument coercions, and copies or moves it into owned heap storage. An optional trailing comma follows that argument. Vec construction takes no arguments and creates a sequence of length zero. Neither constructor imposes Copy, Clone, or equality bounds on T.

## Box access and moves

`*b` is a place of type T for b of type Box<T>. It is mutable when the owning Box place is mutable, including through a mutable reference; access through a shared reference remains read-only. `&*b` and `&mut *b` borrow the stored T under the ordinary place rules. Field access, indexing a supported contained sequence, and method lookup may dereference Box and references using the specified builtin adjustments. Reference coercions such as `&Box<T>` to `&T` are described in [Types](types.md#conversions-and-references).

Reading a Copy T from `*b` copies it. Moving a non-Copy T out of an owned Box is supported, as in `let value = *b;`; the moved contents cannot be used again without valid reinitialization. Moving non-Copy contents out through a borrowed Box is excluded by the existing ownership-validity guarantees. Valid field moves follow the same source value rules. An emptied allocation may remain until program termination without an immediate release.

A valid Box owns one initialized value of its element type. Its allocation follows the [program-end reclamation](#program-end-reclamation) policy.

## Vec operations

The following table defines Vec's builtin operations for a concrete element type T.

| Operation | Receiver and arguments | Result and behavior |
| --- | --- | --- |
| `len()` | `&self` | usize; number of initialized elements |
| `is_empty()` | `&self` | bool; whether length is zero |
| `push(value)` | `&mut self`, value: T | Unit; copy/move value to the end, increase length by one |
| `remove(index)` | `&mut self`, index: usize | T; move out that element, shift later elements left, decrease length by one |
| `v[index]` | index: usize | Element place; readable through shared access, writable through mutable access |

Indexing and remove require index < len in valid executions; out-of-bounds executions are excluded, with no required checks or panic support. remove preserves element order and transfers the returned T to the caller, including when T is non-Copy and non-Clone. Moving remaining elements or relocating a buffer does not clone them.

```rust,ignore
let mut stack = Vec::<i32>::new();
stack.push(7);
if !stack.is_empty() {
    let last = stack.len() - 1;
    let value = stack.remove(last);
    printlnInt(value);
}
```

Reading a Copy element through indexing copies it. Moving a non-Copy element directly out of `v[index]` is excluded; use remove to transfer ownership while maintaining the initialized sequence. Assignment to a mutable element place remains supported. Storage formerly owned by a replaced element follows the [reclamation policy](#program-end-reclamation).

Vec element access uses indexing and the operations in the table above. Dereferencing requires a reference or Box operand, so `*v` for `v: Vec<T>` is a static error. A reference to a Vec refers to the container itself; it does not coerce to a reference to T.

## Clone and equality

The [capability table](builtin-traits.md#derive) determines whether these
operations are available, including on recursive container graphs.

Cloning a Box produces an independent owned clone of its T; cloning a Vec clones its elements in order into an independent sequence of the same length. Nested owned containers are cloned recursively. Shared-reference elements copy the references and continue to share their referents; deep cloning does not mean following shared references. Internal allocations may be optimized only when all observable behavior is preserved.

Under the [equality operand rules](builtin-traits.md#equality), Box equality compares the stored values. Vec equality compares lengths and corresponding elements, independent of addresses, spare capacity, and padding. These operations compose with struct derives and execute on finite values of recursive types.

## Storage and references

Vec must maintain its initialized element sequence; spare storage cannot be read as initialized elements. Buffer capacity and growth factor are implementation choices and have no source-level query. Reallocation preserves element values and their order but invalidates references into the old buffer. remove shifts later values; old references cannot be used to bypass the operation's exclusive access or refer to removed/moved elements.

Valid tests obey Rust's borrowing restrictions for the supported container operations, including implicit two-phase receiver borrowing where applicable, such as `v.push(v.len())` for a Vec<usize>. A reference into a Vec is not kept and used across a conflicting mutable container operation, even when a particular implementation has spare capacity or does not move an address. Tests must not rely on a chosen capacity policy to justify such a reference. No borrow checker is required.

Zero-sized heap objects are course UB under the [zero-sized-data guarantee](undefined-behavior.md#zero-sized-data). An empty Vec with nonzero-sized T is a valid nonzero-sized container. Its internal zero-capacity pointer representation is implementation-defined; it must not access nonexistent elements or require a dereferenceable buffer for length zero. Allocation sizes and valid element indices fit the target representation; valid test executions do not depend on allocation failure.

## Program-end reclamation

Heap allocations may remain for the entire execution and are reclaimed together when that execution ends. This policy applies across scope exit, overwrite, move-out, return, break, continue, and Vec buffer replacement.

Reclaiming the execution's heap arena or resetting its memory in the runtime or execution environment completes cleanup. Generated code may leave that work entirely to the execution environment. A bump allocator is an acceptable implementation strategy. No per-value automatic cleanup, final object-graph traversal, object registry, or cleanup ownership flags are required.

Source-level ownership and reference validity are unchanged. Moving a value still transfers ownership, Clone still creates independent owned contents, and expired or invalidated references remain unusable even if their bytes remain allocated. Stack storage continues to follow the separate reference and storage-duration rules.

Implementations may eliminate allocations or reclaim provably unused storage earlier when observable behavior and valid references are preserved, including old Vec buffers after growth. Such reclamation is an optional optimization. Tests must be runnable without reclamation during execution; they account for cumulative allocation, buffer growth and allocator overhead, rather than only the maximum simultaneously live payload. The [backend resource baseline](backend.md#42-参考-vec-策略与资源保证) fixes the reference growth strategy, runtime allocation interface, REIMU memory setting and test headroom without making capacity source-observable.

The [backend contract](backend.md#41-box--vec-与内存辅助接口) specifies the
reference allocator ABI, raw-memory helpers, and runtime responsibilities.
