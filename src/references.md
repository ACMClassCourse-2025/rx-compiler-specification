# References and storage duration

## Reference annotations

[Reference types](types/pointer.md) record referent type, mutability, and an optional lifetime. They may appear in fields, arrays, parameters, results, and container arguments.

```rust,ignore
struct View<'a> { value: &'a i32 }

fn choose<'a>(a: &'a i32, b: &'a i32, first: bool) -> &'a i32 {
    if first { a } else { b }
}
```

## Lifetime validity

Declarations, annotations, arguments, bounds, and elision follow Rust 2021's [generic parameter](https://doc.rust-lang.org/reference/items/generics.html) and [lifetime elision](https://doc.rust-lang.org/reference/lifetime-elision.html) rules for supported constructs. Incorrect lifetime use is course UB under the centralized [lifetime validity guarantee](undefined-behavior.md#lifetime-validity).

Each elided input reference in a function signature gets a distinct lifetime. If exactly one input lifetime exists, it determines elided outputs. A method's reference receiver determines elided outputs. Other ambiguous outputs require explicit annotations. `'_` requests inference where Rust permits it.

Correct annotations describe validity; they never extend the storage duration of a local, temporary, or heap object.

## Borrowing rules

Ordinary type and mutability checks are required: mutable borrows need mutable places, shared references cannot become mutable, and writes through shared references are rejected. Borrow dataflow analysis is not required because tests guarantee actual borrowing and ownership validity.

Valid programs may use multiple shared references, disjoint mutable references, and shorter child reborrows. A parent mutable reference becomes usable again after its child reborrow ends. Passing a mutable reference to an `&mut` parameter may reborrow it.

References need no runtime lifetime markers, reference counts, or ownership checks. Optimizations must still respect aliasing and reborrowing; distinct reference values need not denote distinct addresses.

## Storage and implementation

Supported Rust 2021 local and temporary scopes define source validity. A local remains valid until its scope ends unless moved or otherwise invalidated. Shadowing creates a new binding without ending the old value's storage duration. Statement temporaries normally last through the statement, and eligible `let` initializers receive Rust's temporary-lifetime extension.

A conforming baseline may allocate fixed stack storage per call and retain it until return. Recursive calls need independent storage; old loop iterations need not accumulate storage. Implementations may promote values, reuse slots, or eliminate storage whenever observable behavior and valid references are preserved.

Retaining bytes does not make a dangling source reference valid. Constant promotion is optional, and tests do not return references to promoted literals or other program-lifetime temporaries.

<details>
<summary>Excluded reference executions</summary>

Tests never:

- Use a reference after its target expires, moves in an invalidating way, is freed, or is reallocated.
- Return and use a reference to a callee local or insufficiently long-lived temporary.
- Access overlapping storage through conflicting active shared and mutable borrows.
- Use a parent mutable reference while a conflicting child reborrow is active.
- Move non-`Copy` data from borrowed contents or reuse a moved mutable reference.
- Reference uninitialized, misaligned, incorrectly typed, or otherwise invalid data.

Shared references may alias, and disjoint mutable borrows remain valid. Dynamic indices must also satisfy these rules.

</details>

Container-specific invalidation follows [Box and Vec storage rules](heap.md#storage-and-references). Program-end reclamation does not extend source-level validity.
