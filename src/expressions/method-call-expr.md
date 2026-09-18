# Method-call expressions

r[expr.method.syntax]
```grammar,expressions
MethodCallExpression -> Expression `.` PathExprSegment `(` CallParams? `)`
```

A method call gives a receiver expression, a method identifier, and a parenthesized argument list. An optional trailing comma follows the last argument.

[PathExprSegment] supplies the shared path-segment syntax. Methods use their identifier with optional lifetime arguments where Rust permits them. [Path resolution](../paths.md#path-resolution) specifies these arguments and the lifetime validity guarantee. Type arguments on a method segment are compile errors.

## Method lookup

The receiver type must be known before lookup. Method calls find the receiver adjustments as follows:

1. Start with the receiver expression's type and repeatedly dereference references and `Box`, recording each type in order. `Vec` adds no dereference step.
2. Immediately after each recorded type `T`, insert `&T` and `&mut T` as candidate receiver types.
3. Find all methods with the requested name whose declared receiver type exactly matches any candidate. Include inherent methods, available builtin `Clone` methods, and the specified array and container methods. Count the same method for the same concrete `Self` type only once, even if it matches at several positions.
4. No matching method is a compile error. More than one distinct matching method is undefined behavior, even if Rust's candidate priority would select one. Argument types, expected result types, and receiver mutability do not filter this count.
5. With one matching method, use its first matching candidate. Apply the dereferences and any borrow for that candidate; an existing mutable-reference receiver may reborrow under the [ordinary reference rules](../types.md#mutable-reference-reborrowing). Then check receiver mutability and ordinary arguments. A failed check is a compile error, subject to the separate ownership and lifetime test guarantees.

Tests exclude competing methods; the compiler need not detect them or implement Rust's method priorities. Compiler-generated cloning invokes the builtin operation directly and does not perform this lookup. Duplicate inherent declarations remain [name errors](../names.md#name-collisions); methods on unrelated types outside the receiver's candidate list do not compete.

<details>
<summary>Receiver and clone examples</summary>

For `Box<S>`, the candidate receiver types are `Box<S>`, `&Box<S>`, `&mut Box<S>`, `S`, `&S`, and `&mut S`. For a unique method on `S` taking `&self`, calling it on the box dereferences to `S` and borrows that place. An `&mut self` method also requires mutable access along this path.

If `S` derives `Clone` and has no inherent `clone`, `s.clone()` for `s: S` has one method and returns `S`. For `r: &S`, both `S`'s clone (receiver `&S`) and the reference's clone (receiver `&&S`) match, so `r.clone()` is undefined behavior. Write `S::clone(r)` to select `S`'s builtin clone explicitly. If `S` is not `Clone` and has no inherent `clone`, `r.clone()` instead has only the reference clone and returns `&S`.

Likewise, `b.clone()` for `b: Box<i32>` is undefined behavior because both the box and its integer support `Clone`. `Box::<i32>::clone(&b)` explicitly selects the box's operation. An inherent `clone` competing with a derived `clone` also makes the dot call undefined behavior, regardless of their receiver forms.

</details>

An explicit `Type::method(receiver, ...)` call selects that type's associated operation and uses [ordinary argument coercions](call-expr.md), not receiver lookup or autoref. If an inherent method and a builtin method on that exact type share the name, the inherent method is selected. See [Builtin traits](../builtin-traits.md#clone) and [Heap](../heap.md) for available builtin operations.
