# Types and inference

```grammar,types
Type -> TypeNoBounds

TypeNoBounds ->
      ParenthesizedType
    | TypePath
    | TupleType
    | ReferenceType
    | ArrayType

ParenthesizedType -> `(` Type `)`
```

## Supported types

| Type | Meaning |
| --- | --- |
| `i32`, `isize` | Distinct signed 32-bit integers |
| `u32`, `usize` | Distinct unsigned 32-bit integers |
| `bool` | Boolean |
| Named struct; `Self` in its declaration or impl | Nominal named-field product |
| `[T; N]` | Fixed array with a restricted `usize` constant length |
| `&T`, `&mut T`, `&'a T`, `&'a mut T` | Shared or mutable reference |
| `()` | Function or control-flow result |
| [`Box<T>`, `Vec<T>`](types/heap.md) | Owned heap object or dynamic sequence |

Diverging expressions also use never internally, but `!` is not a valid source type annotation. The `!` expression operator remains boolean or bitwise negation.

Struct types are nominal. References, arrays, `Box`, and `Vec` are structural. The four integer types remain distinct. Array lengths are compared by value, so `[i32; 4]` and `[i32; (4usize)]` are identical. Reference identity uses referent type and mutability; lifetime arguments do not otherwise distinguish source types and must satisfy the [lifetime validity guarantee](undefined-behavior.md#lifetime-validity).

## Inference

Inference is monomorphic and local to one function body. Item signatures are explicit. A local initializer and all later uses constrain the same binding, whether or not the `let` has a type annotation.

An integer suffix or explicit annotation fixes the integer type. An unsuffixed integer collects constraints from its uses and defaults to `i32` only if still unconstrained. Contradictory constraints are static errors; determined integer types never change implicitly.

Calls, returns, fields, arrays, operators, and control-flow joins must satisfy their own type rules. Unconstrained non-integer values need an annotation or other context. Field and method lookup requires the relevant receiver type to be known at that point, even if later uses could otherwise constrain it. Explicit container constructor arguments determine their result immediately:

```rust,ignore
let mut nodes = Vec::<Node>::new();
```

## Conversions and references

Integer-to-integer and bool-to-integer conversions require `as`. Conditions require `bool`.

Implicit coercions are limited to mutable-reference reborrows, mutable-to-shared reborrows, builtin reference and `Box` dereference coercions, and never-to-expected-type conversion. All other target types must agree.

| Coercion site | Expected type |
| --- | --- |
| Annotated `let` initializer | Annotation |
| Assignment right operand | Destination |
| Function, method, constructor, or `Vec` argument | Declared parameter |
| Function tail or `return` operand | Declared result |
| Struct field initializer | Declared field |

Expected types propagate through parentheses and block tails, and into array elements, `if` branches, and loop breaks where applicable. Those multi-value forms use Rust's join rules within the supported coercions: for example, `&mut T` and `&T` may join as `&T`, and never may fit the other branch. An incompatible join is a static error and does not trigger an integer conversion or a search across unrelated types.

Reference and `Box` dereferencing may repeat when Rust permits it. For example, `&Box<T>` may coerce to `&T`, and `&mut Box<T>` to `&mut T` or `&T`. Shared access never becomes mutable. `Vec` does not dereference to its elements or a slice; array-to-slice unsizing is unsupported. Owned `Box<T>` never coerces to `T`.

Method receivers use their own [lookup and adjustment rules](expressions/method-call-expr.md#method-lookup). Field access and indexing also apply supported builtin dereferences. These adjustments do not expand the operator or cast tables.

<details>
<summary>Mutable-reference reborrows and moves</summary>

An expected `&mut T` can reborrow a mutable reference, but an unconstrained `let` moves it:

```rust,ignore
let mut x = 1;
let p = &mut x;
{
    let q: &mut i32 = p; // reborrow
    *q = 2;
}
*p = 3;
let moved = p;           // move
*moved = 4;
```

After the move, `p` cannot be used without reinitialization. Passing `p` to an `&mut i32` parameter may reborrow it, allowing later calls. Later inference never turns `let moved = p` into the annotated reborrow above.

</details>

## Recursive types

A by-value struct or array containment cycle has no finite size and is a static error. References, `Box`, and `Vec` provide indirection, so recursive and mutually recursive structs through those types are supported.

```rust,ignore
struct Node {
    value: i32,
    children: Vec<Node>,
}
```

<details>
<summary>Recursive layout details</summary>

`struct Infinite { child: Infinite }` is invalid because the child is inline; an inline array also fails to break the cycle. Wrapping an independently invalid type in a container does not repair its declaration.

Finite layout does not imply constructibility. `struct Chain { next: Box<Chain> }` has finite layout but no terminating value because `Box` is non-nullable. This does not make its declaration invalid.

An implementation may check cycles only through inline struct and array dependencies. Container APIs, trait capabilities, and reclamation are defined separately in [Box and Vec](heap.md).

</details>
