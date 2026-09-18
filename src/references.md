# References and storage duration

## Reference annotations

[Reference types](types/pointer.md) specify the referent type, mutability, and optional lifetime. Lifetime declarations and bounds use [Lifetime parameters and bounds](items/generics.md); this chapter defines their validity and storage-duration contract.

Reference types may appear in fields, arrays, parameters, and results. For example:

```rust,ignore
struct View<'a> { value: &'a i32 }

fn choose<'a>(a: &'a i32, b: &'a i32, first: bool) -> &'a i32 {
    if first { a } else { b }
}
```

## Lifetime validity

Lifetime declarations, annotations, arguments, bounds, and elision follow
Rust 2021's [parameter rules](https://doc.rust-lang.org/reference/items/generics.html)
and [elision rules](https://doc.rust-lang.org/reference/lifetime-elision.html)
for the supported constructs. Tests guarantee their validity
throughout each program, including signatures, struct fields, function bodies,
and unreachable code. An incorrect lifetime declaration or use is course UB
and appears in no positive, negative, or performance test. This covers:

- Undeclared lifetime names, invalid parameter names, duplicate declarations,
  and forbidden lifetime-parameter shadowing.
- Lifetime arguments with an incorrect number, order, or placement, including
  explicit arguments where Rust requires a late-bound lifetime to be inferred.
- Missing annotations where Rust's elision rules cannot determine a lifetime,
  such as a reference field without an explicit lifetime or an ambiguous
  reference-returning signature.
- Unsatisfied lifetime or type-outlives bounds, and function bodies or uses
  that fail to uphold their annotated lifetimes, including `'static`.
- Unused lifetime parameters when Rust rejects the declaration.

The contract applies to well-formed lifetime syntax. Malformed tokens and
syntax retain their ordinary lexical and parsing rules. Other type and
place-mutability errors remain the specified static errors.

An omitted reference lifetime is valid where Rust permits elision. In a
function signature, each elided input reference gets a distinct lifetime; if
there is exactly one input lifetime, it determines elided output lifetimes.
For methods with a reference receiver, that receiver's lifetime determines
elided output lifetimes. Other cases use explicit annotations. `'_` requests
the inferred lifetime where Rust permits that placeholder. For path arguments,
such as a local `View<'_>` type, Rust's corresponding lifetime elision rules
apply.

```rust,ignore
fn identity(value: &i32) -> &i32 {
    value
}

impl<'a> View<'a> {
    fn borrowed(&self) -> &i32 {
        self.value
    }
}
```

Correct annotations describe reference validity; they do not extend the
storage duration of a local, temporary, or heap object. Tests also ensure that
every actual reference use satisfies the borrowing and ownership rules below.

## Static checks and valid borrowing

Ordinary type and mutability checks are required: dereference targets must have appropriate types, mutable borrows need mutable places, writes through shared references are rejected, and shared references cannot become mutable references. These checks do not require borrow dataflow.

Valid programs may have multiple shared references, mutable references to disjoint places, and shorter reborrows of mutable references. After a child reborrow ends, the parent may be used again. Passing a mutable reference to a mutable-reference parameter can reborrow it rather than move it permanently.

No lifetime markers, runtime reference counting, or ownership checks are required for these references. Optimizers may use the specified validity guarantees only in ways consistent with aliasing and reborrowing; distinct reference values are not automatically distinct addresses.

## Storage and implementation

Rust 2021 local/temporary storage scopes and temporary lifetime extension provide the validity reference for supported syntax. A local's value remains valid until the appropriate scope ends unless moved or otherwise invalidated. Shadowing creates a new binding rather than destroying the old value. Ordinary statement temporaries usually last to the statement end; extending let initializers follow the supported Rust rules.

```rust,ignore
fn main() {
    let x: i32 = 1;
    let r: &i32 = &x;
    let x: i32 = 2;
    printlnInt(*r); // 1
    printlnInt(x);  // 2

    let a: &mut [i32; 4] = &mut [0; 4];
    a[0] = 7;
    printlnInt(a[0]);
}
```

There is no requirement to calculate or emit these storage endpoints. A valid baseline is to allocate fixed stack storage per function invocation and retain it until return, with separate storage for objects that must coexist. Recursion gets independent storage; dead objects from previous loop iterations need not accumulate stack space.

Optimized implementations may use constant propagation, mem2reg, scalar replacement, dead-code elimination, inlining, and safe storage reuse. They may also eliminate a source object's physical stack slot when all observable behavior is preserved.

Retaining physical storage does not make an otherwise dangling source reference valid. Constant promotion is not required; tests do not depend on returning a reference to a promoted literal or other program-lifetime temporary. Ordinary extending let borrows can be implemented with stack storage.

## Excluded executions

The test domain excludes:

- Returning and using references to a callee's local or insufficiently long-lived temporary.
- Using references after their target is invalidated, freed, moved in a way that invalidates the reference, or reallocated.
- Accessing overlapping storage through conflicting active shared/mutable borrows.
- Using a parent mutable reference while its conflicting child reborrow is active.
- Moving non-Copy values from borrowed contents or reusing a moved mutable reference.
- References to uninitialized, misaligned, incorrectly typed, or otherwise invalid data.

Shared references may alias, and disjoint mutable borrows are allowed. Tests are responsible for actual validity, including dynamic indices. The compiler is not required to diagnose these cases, and they must not be ownership-error negative tests.

Box/Vec reference invalidation and valid container borrowing follow the [heap operations](heap.md#storage-and-references). The [program-end heap reclamation policy](heap.md#program-end-reclamation) does not extend source-level reference validity; keeping allocated bytes cannot make an expired reference usable. Conservative stack storage remains a separate implementation choice.
