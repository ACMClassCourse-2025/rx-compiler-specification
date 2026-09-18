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
| `&T`, `&mut T`, `&'a T`, `&'a mut T` | Shared or mutable reference with an explicit or elided lifetime |
| `()` | Function/control-flow result; ordinary zero-sized data uses are excluded |
| Builtin [`Box<T>`, `Vec<T>`](types/heap.md) | Owned heap object and dynamic sequence; nesting and recursion through indirection are supported |

The table above defines the source-level types. Type checking also uses never internally for diverging expressions. User-written type annotations use the grammar above; an explicit `!` in a type position is a static error. The expression operator `!` performs boolean or bitwise negation.

Type identity is structural for reference, array, Box, and Vec types and nominal for structs. Array lengths are compared by their resulting literal values, so `[i32; 4]` and `[i32; (4usize)]` are the same type. The four integer names remain distinct even though all have 32 bits.

Lifetime annotations satisfy the [lifetime validity contract](references.md#lifetime-validity). For ordinary type identity, a reference is determined by its referent type and mutability, and a struct by its declaration. Lifetime arguments alone do not distinguish source types in the same-type equality rule. Type compatibility and operations must still satisfy the guaranteed lifetime and borrowing conditions.

Container type composition is defined in [Box and Vec types](types/heap.md).
Their [constructors](heap.md#constructors-and-type-arguments) use the
[generic path syntax](paths.md#generic-arguments).

## Inference

Inference is monomorphic and local to a function body. Item signatures follow their declaration chapters. For a local let, the initializer and later uses constrain the same binding, whether or not it has an explicit annotation.

```rust,ignore
fn take(value: u32) {}

fn main() {
    let a = 123;
    take(a); // a is u32, inferred from its use
    let b: i32 = 123;
    take(b as u32);
}
```

An integer suffix or an explicit annotation fixes the integer type. Unsuffixed literals acquire an integer type from all applicable constraints; still-unconstrained integer variables default to i32 after constraint collection. Contradictory constraints are static errors. An already determined integer type retains its identity.

Function calls, returns, field initialization, array elements, operators, and control-flow joins must satisfy their type rules. For example, an array literal must have a common element type, and a loop's break values must agree with the loop result. Unconstrained non-integer types require sufficient context or an annotation.

Field and method lookup require the receiver's relevant type to be known at the lookup point. An unresolved receiver or element type can require an earlier annotation, even when later uses contribute backward constraints to other expressions. Lookup uses the known receiver type's fields and method candidates.

A [container constructor's](heap.md#constructors-and-type-arguments) explicit type argument supplies its result type to inference. For example, `let mut v = Vec::<Node>::new();` determines the binding's type immediately.

## Conversions and references

Conversion between determined integer types, and conversion from bool to an integer, use `as`; see [Operators](expressions/operator-expr.md#casts). Conditions require bool.

Implicit coercions support mutable-to-shared reborrow, mutable-to-mutable reborrow, builtin reference/Box deref coercions, and never-to-expected-type conversion. Shared access remains shared. Apart from these adjustments, target types must agree. Ordinary type and place-mutability checks are required; tests guarantee borrow validity.

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

The receiver of a method call uses [method lookup and receiver adjustments](expressions/method-call-expr.md#method-lookup), rather than ordinary argument coercion alone. Field access and indexing also apply their supported builtin dereferences. These adjustments do not extend the arithmetic operator table to arbitrary reference operands and do not broaden the numeric `as` cast table. Reference adjustments use the builtin reference and Box operations specified above.

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
