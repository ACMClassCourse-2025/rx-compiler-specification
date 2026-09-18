# References and storage duration

## Lifetime-erased references

Reference types are written `&T` and `&mut T`, where T is a supported concrete type. Their declarations specify the referenced type and mutability. Tests guarantee the validity of each reference's lifetime and use.

Reference types may appear in fields, arrays, parameters, and results. For example:

```rust,ignore
struct View { value: &i32 }

fn choose(a: &i32, b: &i32, first: bool) -> &i32 {
    if first { a } else { b }
}
```

Tests ensure every actual reference use is valid. The compiler need not infer which input a returned reference comes from.

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
