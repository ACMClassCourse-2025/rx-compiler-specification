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
| `i32`, `isize` | Distinct signed 32-bit integer types |
| `u32`, `usize` | Distinct unsigned 32-bit integer types |
| `bool` | Boolean value |
| Named struct, `Self` in its struct definition or impl | Nominal product of named fields |
| `[T; N]` | Fixed-length array; N is a restricted constant of type usize |
| `&T`, `&mut T` | Shared or mutable reference, without a lifetime annotation |
| `()` | Function/control-flow result; ordinary zero-sized data uses are excluded |
| Builtin `Box<T>`, `Vec<T>` | Owned heap object and dynamic sequence; nesting and recursion through indirection are supported |

No floating point, other integer widths, char, string, tuple, enum, slice, raw pointer, function-value, trait-object, or user-generic type is supported. Never is used internally to type diverging expressions. Explicit `!` is not permitted in any user-written type position, including function results, parameters, local annotations, fields, and nested types. The unary `!` operator remains supported.

Type identity is structural for reference, array, Box, and Vec types and nominal for structs. Array lengths are compared by their resulting literal values, so `[i32; 4]` and `[i32; (4usize)]` are the same type. The four integer names remain distinct even though all have 32 bits.

Box and Vec each take one supported concrete type argument: `Box<T>` and `Vec<T>`. They may contain structs, arrays, references, and other containers, subject to the existing type-validity and zero-sized-data rules. This composition does not require Copy or Clone. Nested closing angle brackets follow Rust type syntax, including `Vec<Vec<i32>>`. Their only constructors are the explicitly typed `Box::<T>::new(value)` and `Vec::<T>::new()`, as specified in [Heap](heap.md). No type-position `_` is supported, including within nested type arguments.

## Inference

Inference is monomorphic and local to a function body. Function parameters and results, struct fields, and const declarations have explicit types; omitted function return annotations mean unit. Local lets may omit the type. The initializer and later uses constrain the same binding.

```rust,ignore
fn take(value: u32) {}

fn main() {
    let a = 123;
    take(a); // a is u32, inferred from its use
    let b: i32 = 123;
    take(b as u32);
}
```

An integer suffix or an explicit annotation fixes the integer type. Unsuffixed literals acquire an integer type from all applicable constraints; still-unconstrained integer variables default to i32 after constraint collection. Contradictory constraints are static errors. This is not an implicit conversion of an already determined integer value.

Function calls, returns, field initialization, array elements, operators, and control-flow joins must satisfy their type rules. For example, an array literal must have a common element type, and a loop's break values must agree with the loop result. Unconstrained non-integer types require sufficient context or an annotation; arbitrary type-position `_` syntax is not introduced by allowing omitted let annotations.

Backward constraints do not require arbitrary deferred field or method resolution. As in Rust, the receiver's relevant type must be known when field or method lookup needs it; an unresolved receiver or element type can require an earlier annotation. The compiler need not enumerate all structs that have a given field or revisit an unresolved member access after later unrelated statements.

Container constructors require an explicit concrete element type, so `let mut v = Vec::<Node>::new();` already determines it. `Vec::new()` with a later push is not a supported way to infer an omitted constructor argument. Omitted local annotations and backward constraints elsewhere remain supported; required field/method lookup still needs enough receiver type information at that point.

## Conversions and references

There are no implicit conversions between determined integer types. Integer-to-integer and bool-to-integer conversion use `as`; see [Operators](expressions/operator-expr.md#casts). Integers do not convert to bool for conditions.

Implicit coercions follow the Rust rules restricted to the following operations: mutable-to-shared reborrow, mutable-to-mutable reborrow, builtin reference/Box deref coercions, and never-to-expected-type conversion. Shared access cannot be upgraded to mutable access. Apart from these adjustments, target types must agree. Ordinary type and place-mutability checks are required; tests guarantee borrow validity.

| Coercion site | Expected type |
| --- | --- |
| Let initializer with an explicit annotation | The annotated type |
| Ordinary assignment right operand | The destination type |
| Function/method argument, including constructor and Vec arguments | The declared parameter type after substituting the explicit concrete container type |
| Function tail and return operand | The declared result type |
| Named-field struct initializer | The declared field type |

An expected type propagates through parenthesized expressions and block tails, into array elements when the expected type is an array, and into if branches and loop break values. Common types for if branches, array elements, and loop break values are determined using Rust's coercion/join rules within the finite set above. For example, shared and mutable references to the same T can join at &T, and a diverging branch can fit the other branch's type. Incompatible joins are static errors; there is no integer conversion to repair them. If more type information is required for a join or member lookup, an explicit annotation supplies it; arbitrary searches over unrelated types are not required.

A borrow of a Box can dereference to a borrow of its contents: `&Box<T>` to `&T`, `&mut Box<T>` to `&mut T` or `&T`. Builtin reference dereferencing can also participate, for example `&&T` to `&T`; repeated reference/Box steps obey Rust's permitted coercions and cannot turn shared access into mutable access. This is not a conversion of an owned Box<T> to T or an implicit borrow of an arbitrary T value. Vec does not dereference to T or an exposed slice, and neither `&Vec<T>` to `&T` nor array-to-slice unsizing is supported.

The absence of a let annotation matters for mutable-reference moves:

```rust,ignore
let mut x = 1;
let p = &mut x;
{
    let q: &mut i32 = p; // reborrow, approximately &mut *p
    *q = 2;
}
*p = 3;
let moved = p;          // move, not an implicit Copy or unconditional reborrow
*moved = 4;
```

After the final move, p cannot be used again without reinitialization; tests exclude such misuse. Passing p to a parameter whose type is &mut i32 can instead reborrow it, allowing successive calls with p. Later inference constraints do not turn an unannotated `let moved = p` into the annotated reborrow above.

The receiver of a method call uses [method lookup and receiver adjustments](names.md#method-lookup), rather than ordinary argument coercion alone. Field access and indexing also apply their supported builtin dereferences. These adjustments do not extend the arithmetic operator table to arbitrary reference operands and do not broaden the numeric `as` cast table. There are no user Deref implementations, raw-pointer coercions, function-pointer coercions, or lifetime annotations.

## Recursive types

A by-value struct/array containment cycle with no indirection has no finite size and is a static type error. References, Box, and Vec provide indirection: the stored reference or container representation has finite size without embedding the target or element values inline. Recursive and mutually recursive named structs through these types are supported.

```rust,ignore
struct Node {
    value: i32,
    children: Vec<Node>,
}
```

The Node definition is valid. Its Vec field does not inline an unbounded number of Nodes. An empty child vector provides a finite leaf, so no Option or enum is needed to terminate such a tree. Internal layout remains implementation-defined under the backend contract.

In contrast, the following definition is a static error because its child is inline:

```rust,ignore
struct Infinite {
    child: Infinite,
}
```

An inline array does not break such a cycle either. Each named type must itself be well-formed; wrapping an invalid infinite-size type in a container does not repair its definition. Implementations can resolve named types first and check for cycles in inline struct/array layout dependencies, without recursively expanding names into syntax trees.

Finite layout does not guarantee that a value can be constructed. For example, `struct Chain { next: Box<Chain> }` has finite layout but supplies no terminating case for a finite owned chain. This does not make its declaration an infinite-size type error or introduce nullable Box values. Container APIs, trait capabilities, and destruction remain separate parts of the [heap contract](heap.md).
