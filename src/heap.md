# Box and Vec

## Containers

The language provides builtin `Box<T>` for one owned heap object and `Vec<T>` for an owned contiguous sequence with runtime length. Each takes one concrete element type T and provides the operations defined in this chapter.

Both containers are non-Copy and use the established move semantics. A struct directly containing either cannot derive Copy. Moving a container transfers its owned value; it does not create two independent owners or deep-clone its contents. Shared references to containers retain the ordinary reference Copy rules.

The compiler knows the concrete T, its layout, and the operations required for it. The runtime allocator operates on sizes and alignments, while the compiler supplies the type-dependent operations.

## Type composition and recursion

Box and Vec compose with the other supported concrete types, including named structs, arrays, references, and nested containers. The zero-sized-data exclusions apply to their element types.

Recursive types such as `struct Node { children: Vec<Node> }` and mutually recursive structs through Box/Vec are supported. These containers provide indirection, so their stored representations do not require expanding all contained values inline. An empty Vec supplies a leaf for a tree. The [recursive-type rules](types.md#recursive-types) distinguish finite layouts from invalid inline containment cycles.

The conditional Clone and equality operations below support recursive type graphs. Capability checking handles cycles while enforcing each field's requirements; execution applies the operation to a finite value. Heap storage is reclaimed together at program termination.

## Constructors and type arguments

Constructors are written `Box::<T>::new(value)` and `Vec::<T>::new()`. The single type argument T is an explicit concrete type, including at every nesting level. Type paths accept `Box<T>` / `Vec<T>` and the equivalent `Box::<T>` / `Vec::<T>` spellings. Expression paths introduce the arguments with `::<T>`, following the [path syntax](paths.md). Both constructors use [CallExpression] with a [PathExpression] callee.

```rust,ignore
let b = Box::<i32>::new(7);
let mut values = Vec::<i32>::new();
values.push(3);
let mut nested: Vec<Vec<i32>> = Vec::<Vec<i32>>::new();
nested.push(values);
```

A local binding can omit its own annotation because the constructor's explicit element type supplies the information. Other expressions in the function participate in the ordinary local inference rules, including backward constraints.

Box construction takes one argument of the stated T, allowing the specified argument coercions, and copies or moves it into owned heap storage. Vec construction takes no arguments and creates a sequence of length zero. Neither constructor imposes Copy, Clone, or equality bounds on T.

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

Reading a Copy element through indexing copies it. Moving a non-Copy element directly out of `v[index]` is excluded; use remove to transfer ownership while maintaining the initialized sequence. Assignment to a mutable element place remains supported. Storage formerly owned by a replaced element may remain until program termination; no recursive destruction or immediate release is required.

Vec element access uses indexing and the operations in the table above. Dereferencing requires a reference or Box operand, so `*v` for `v: Vec<T>` is a static error. A reference to a Vec refers to the container itself; it does not coerce to a reference to T.

## Clone and equality

| Container | Clone | PartialEq | Eq |
| --- | --- | --- | --- |
| Box<T> | When T is Clone | When T is PartialEq | When T is Eq |
| Vec<T> | When T is Clone | When T is PartialEq | When T is Eq |

Cloning a Box produces an independent owned clone of its T; cloning a Vec clones its elements in order into an independent sequence of the same length. Nested owned containers are cloned recursively. Shared-reference elements copy the references and continue to share their referents; deep cloning does not mean following shared references. Internal allocations may be optimized only when all observable behavior is preserved.

Equality is tested between two Box<T> values with the same T, or two Vec<T> values with the same T. Box equality compares the stored values. Vec equality compares lengths and corresponding elements, independent of addresses, spare capacity, and padding. Containers with different element types, and a Vec compared with an array, are cross-type equality cases excluded as course UB. Eq marks equality as an equivalence relation. These conditional capabilities compose with struct derives, including `#[derive(Clone, PartialEq, Eq)] struct Node { children: Vec<Node> }`.

## Storage and references

Vec must maintain its initialized element sequence; spare storage cannot be read as initialized elements. Buffer capacity and growth factor are implementation choices and have no source-level query. Reallocation preserves element values and their order but invalidates references into the old buffer. remove shifts later values; old references cannot be used to bypass the operation's exclusive access or refer to removed/moved elements.

Valid tests obey Rust's borrowing restrictions for the supported container operations, including implicit two-phase receiver borrowing where applicable, such as `v.push(v.len())` for a Vec<usize>. A reference into a Vec is not kept and used across a conflicting mutable container operation, even when a particular implementation has spare capacity or does not move an address. Tests must not rely on a chosen capacity policy to justify such a reference. No borrow checker is required.

Zero-sized heap objects are excluded. An empty Vec with nonzero-sized T is a valid nonzero-sized container. Its internal zero-capacity pointer representation is implementation-defined; it must not access nonexistent elements or require a dereferenceable buffer for length zero. Allocation sizes and valid element indices fit the target representation; valid test executions do not depend on allocation failure.

## Program-end reclamation

Heap allocations may remain for the entire execution and are reclaimed together when that execution ends. This policy applies across scope exit, overwrite, move-out, return, break, continue, and Vec buffer replacement.

Reclaiming the execution's heap arena or resetting its memory in the runtime or execution environment completes cleanup. Generated code may leave that work entirely to the execution environment. A bump allocator is an acceptable implementation strategy.

Source-level ownership and reference validity are unchanged. Moving a value still transfers ownership, Clone still creates independent owned contents, and expired or invalidated references remain unusable even if their bytes remain allocated. Stack storage continues to follow the separate reference and storage-duration rules.

Implementations may eliminate allocations or reclaim provably unused storage earlier when observable behavior and valid references are preserved, including old Vec buffers after growth. Such reclamation is an optional optimization. Tests must be runnable without reclamation during execution; they account for cumulative allocation, buffer growth and allocator overhead, rather than only the maximum simultaneously live payload. The [backend resource baseline](backend.md#42-参考-vec-策略与资源保证) fixes the reference growth strategy, runtime allocation interface, REIMU memory setting and test headroom without making capacity source-observable.

## Implementation workload

Neither an ownership checker nor code generation for automatic per-value cleanup is required. Valid moves, branch-dependent transfers, partial moves, and early returns must still preserve source value semantics; they do not require cleanup flags under the program-end policy.

Vec growth must move initialized elements rather than clone them. The old buffer may remain allocated until program termination and no longer owns the moved values. remove must maintain the initialized sequence and transfer the removed value to its caller. Reallocation and mutation still obey the reference-validity rules above.

The reference runtime uses `__rx_alloc` over REIMU malloc, and the backend contract gives the cumulative-allocation baseline. Release validation must execute the specified container scenarios on the pinned REIMU commit.

The [backend contract](backend.md#41-box--vec-与内存辅助接口) specifies the reference allocator ABI and the division of responsibilities. `__rx_alloc` is a generated-code/runtime interface, not a source builtin. The bundled runtime example provides this allocator wrapper together with I/O.
