# Box and Vec

```grammar,types
HeapType -> (`Box` | `Vec`) `<` Type `,`? `>`
```

```grammar,expressions
HeapConstruction ->
      `Box` `::` `<` Type `,`? `>` `::` `new` `(` Expression `,`? `)`
    | `Vec` `::` `<` Type `,`? `>` `::` `new` `(` `)`
```

## Confirmed direction

The language provides builtin `Box<T>` for one owned heap object and `Vec<T>` for an owned contiguous sequence with runtime length. These are finite compiler-known type constructors, not user-defined generics. Source-level unsafe, raw pointers, malloc/free, generic functions, generic structs, and generic impls are not introduced.

Both containers are non-Copy and use the established move semantics. A struct directly containing either cannot derive Copy. Moving a container transfers its owned value; it does not create two independent owners or deep-clone its contents. Shared references to containers retain the ordinary reference Copy rules.

The compiler knows the concrete T, its layout, and the operations required for it. A raw-storage allocator can operate on sizes and alignments without knowing T. This does not require implementing the Rust standard library or a general trait solver.

## Type composition and recursion

Box and Vec compose with the other supported concrete types, including named structs, arrays, references, and nested containers. No scalar-only, non-recursive, or single-level container restriction is imposed. Existing zero-sized-data exclusions still apply.

Recursive types such as `struct Node { children: Vec<Node> }` and mutually recursive structs through Box/Vec are supported. These containers provide indirection, so their stored representations do not require expanding all contained values inline. An empty Vec supplies a leaf for a tree. The [recursive-type rules](types.md#recursive-types) distinguish finite layouts from invalid inline containment cycles.

The conditional Clone and equality operations below must handle supported recursive type graphs; the presence of a type cycle alone is not a reason to reject an otherwise valid operation. Checking a type graph and executing an operation over a finite value are separate tasks. Heap storage is reclaimed together at program termination; per-value automatic destruction is not required.

## Constructors and type arguments

The only constructors are `Box::<T>::new(value)` and `Vec::<T>::new()`. The single type argument T must be written explicitly and be a supported concrete type; type-position `_` is unsupported at every nesting level. The type position uses `Box<T>` / `Vec<T>`, while an expression constructor uses the `::<T>` turbofish.

```rust,ignore
let b = Box::<i32>::new(7);
let mut values = Vec::<i32>::new();
values.push(3);
let mut nested: Vec<Vec<i32>> = Vec::<Vec<i32>>::new();
nested.push(values);
```

`Box::new(value)` and `Vec::new()` are unsupported even if an annotation or later use would determine T. `Box::<_>::new(value)` and `Vec::<_>::new()` are also unsupported. A local binding can still omit its own annotation; the explicit constructor type supplies the information. This restriction does not remove backward inference elsewhere in a function. Turbofish is restricted to these two constructor forms; it does not add generic user functions or generic methods.

Box construction takes one argument of the stated T, allowing the specified argument coercions, and copies or moves it into owned heap storage. Vec construction takes no arguments and creates a sequence of length zero. Neither constructor imposes Copy, Clone, or equality bounds on T.

## Box access and moves

`*b` is a place of type T for b of type Box<T>. It is mutable when the owning Box place is mutable, including through a mutable reference; access through a shared reference remains read-only. `&*b` and `&mut *b` borrow the stored T under the ordinary place rules. Field access, indexing a supported contained sequence, and method lookup may dereference Box and references using the specified builtin adjustments. Reference coercions such as `&Box<T>` to `&T` are described in [Types](types.md#conversions-and-references).

Reading a Copy T from `*b` copies it. Moving a non-Copy T out of an owned Box is supported, as in `let value = *b;`; the moved contents cannot be used again without valid reinitialization. Moving non-Copy contents out through a borrowed Box is excluded by the existing ownership-validity guarantees. Valid field moves follow the same source value rules. An emptied allocation may remain until program termination without an immediate release.

Box is not a nullable source-level pointer. There is no source-level null constructor, raw-pointer conversion, allocator argument, or explicit free/drop interface.

## Vec operations

The following signatures describe builtin operations for a concrete T; they are not user-generic declarations.

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

Reading a Copy element through indexing copies it. Moving a non-Copy element directly out of `v[index]` is excluded; use remove to transfer ownership while maintaining the initialized sequence. Assignment to a mutable element place remains supported. Storage formerly owned by a replaced element may remain until program termination; no recursive destruction or immediate release is required.

There is no pop, get, last, insert, clear, reserve, capacity, with_capacity, iterator, slice, vec macro, or additional constructor. Vec has no dereference-to-element operation: `*v` is invalid, and `&Vec<T>` does not coerce to `&T`. Its supported indexing and methods are finite builtins, without an observable slice type.

## Clone and equality

| Container | Clone | PartialEq | Eq |
| --- | --- | --- | --- |
| Box<T> | When T is Clone | When T is PartialEq | When T is Eq |
| Vec<T> | When T is Clone | When T is PartialEq | When T is Eq |

Cloning a Box produces an independent owned clone of its T; cloning a Vec clones its elements in order into an independent sequence of the same length. Nested owned containers are cloned recursively. Shared-reference elements copy the references and continue to share their referents; deep cloning does not mean following shared references. Internal allocations may be optimized only when all observable behavior is preserved.

Equality is tested only between two Box<T> values with the same T, or two Vec<T> values with the same T. Box equality compares the stored values. Vec equality compares lengths and corresponding elements, independent of addresses, spare capacity, and padding. Containers with different element types, and a Vec compared with an array, are cross-type equality cases excluded as course UB even where full Rust defines them. Eq adds no operation. These conditional capabilities compose with the existing struct derives, including `#[derive(Clone, PartialEq, Eq)] struct Node { children: Vec<Node> }`. Copy, PartialOrd, Ord, and user trait implementations are not added for containers.

## Storage and references

Vec must maintain its initialized element sequence; spare storage cannot be read as initialized elements. Buffer capacity and growth factor are implementation choices and have no source-level query. Reallocation preserves element values and their order but invalidates references into the old buffer. remove shifts later values; old references cannot be used to bypass the operation's exclusive access or refer to removed/moved elements.

Valid tests obey Rust's borrowing restrictions for the supported container operations, including implicit two-phase receiver borrowing where applicable, such as `v.push(v.len())` for a Vec<usize>. A reference into a Vec is not kept and used across a conflicting mutable container operation, even when a particular implementation has spare capacity or does not move an address. Tests must not rely on a chosen capacity policy to justify such a reference. No borrow checker is required.

Zero-sized heap objects are excluded. An empty Vec with nonzero-sized T is a valid nonzero-sized container. Its internal zero-capacity pointer representation is implementation-defined; it must not access nonexistent elements or require a dereferenceable buffer for length zero. Allocation sizes and valid element indices fit the target representation; valid test executions do not depend on allocation failure.

## Program-end reclamation

Heap allocations may remain for the entire execution and are reclaimed together when that execution ends. Scope exit, overwrite, move-out, return, break, continue, and Vec buffer replacement do not require recursive destruction or immediate deallocation. This is the course resource policy, rather than Rust's per-value automatic destruction.

There are no user-defined destructors. Reclaiming the execution's heap arena or resetting its memory in the runtime or execution environment is sufficient; generated code need not traverse owned values at program end, keep a registry of objects, or generate per-type drop helpers and ownership flags for cleanup. A bump allocator is an acceptable implementation strategy.

Source-level ownership and reference validity are unchanged. Moving a value still transfers ownership, Clone still creates independent owned contents, and expired or invalidated references remain unusable even if their bytes remain allocated. Stack storage continues to follow the separate reference and storage-duration rules.

Implementations may eliminate allocations or reclaim provably unused storage earlier when observable behavior and valid references are preserved, including old Vec buffers after growth. Such reclamation is an optional optimization. Tests must be runnable without reclamation during execution; they account for cumulative allocation, buffer growth and allocator overhead, rather than only the maximum simultaneously live payload. The [backend resource baseline](backend.md#42-参考-vec-策略与资源保证) fixes the reference growth strategy, runtime allocation interface, REIMU memory setting and test headroom without making capacity source-observable.

## Implementation workload

Neither an ownership checker nor code generation for automatic per-value cleanup is required. Valid moves, branch-dependent transfers, partial moves, and early returns must still preserve source value semantics; they do not require cleanup flags under the program-end policy.

Vec growth must move initialized elements rather than clone them. The old buffer may remain allocated until program termination and no longer owns the moved values. remove must maintain the initialized sequence and transfer the removed value to its caller. Reallocation and mutation still obey the reference-validity rules above.

The reference runtime uses `__rx_alloc` over REIMU malloc, and the backend contract gives the cumulative-allocation baseline. Release validation must execute the specified container scenarios on the pinned REIMU commit; this is verification work, not a remaining destruction-semantics choice.

The [backend contract](backend.md#41-box--vec-与内存辅助接口) specifies the reference allocator ABI and the division of responsibilities. `__rx_alloc` is a generated-code/runtime interface, not a source builtin. The bundled runtime example provides this allocator wrapper together with I/O.
