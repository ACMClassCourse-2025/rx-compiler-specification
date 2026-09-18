# Types and inference

```grammar,types
Type -> TypeNoBounds

TypeNoBounds ->
      ParenthesizedType
    | TypePath
    | UnitType
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

Diverging expressions use the [never type](types/never.md) internally.

Struct types are nominal. References, arrays, `Box`, and `Vec` are structural. The four integer types remain distinct. Array lengths are compared by value, so `[i32; 4]` and `[i32; (4usize)]` are identical. Reference identity uses referent type and mutability; lifetime arguments do not otherwise distinguish source types and must satisfy the [lifetime validity guarantee](undefined-behavior.md#lifetime-validity).

## Inference

Inference is monomorphic. Item signatures are explicit. Each local binding's type is determined from its annotation, initializer, and already determined bindings. Later uses, including block tails, do not revise that type.

Programs that need a later use to supply a missing type or change an otherwise unconstrained integer's default to another supported integer type are course UB. A later use incompatible with a type fixed by an annotation, suffix, or initializer constraint remains a static error.

Within an initializer or other expression, operands and results constrain one another according to their type rules. An enclosing expected type also supplies constraints. Resolve these constraints before defaulting remaining unsuffixed integers to `i32`; finish before checking later statements. This applies inside references and arrays too. Unifying unresolved integer types is inference, not coercion. Contradictory constraints are static errors; determined integer types never change implicitly.

Unconstrained non-integer values need an annotation or context within the same initializer or expression. Field and method lookup requires the receiver type to be known before lookup; the result type does not select a receiver type or method. `Box` and `Vec` constructors require explicit concrete type arguments.

<details>
<summary>Inference examples</summary>

```rust,ignore
fn take_u32(value: u32) {}

fn examples() {
    1 + 2;                         // i32; the semicolon discards the result
    let sum = 1 + 2u32;             // u32, from constraints within the initializer
    let refs = [&1, &2u32];         // [&u32; 2], without a reference coercion
    let mut nodes = Vec::<i32>::new();

    let n = 1;
    take_u32(n);                    // course UB: needs inference from a later statement
}
```

Writing `let n: u32 = 1;` or `let n = 1u32;` makes the call valid. Writing `let n: i32 = 1;` or `let n = 1i32;` makes it a static type error. Constraints do not pass backwards through a local binding, even within the same enclosing block: `let n: u32 = { let x = 1; x };` is also course UB.

</details>

## Conversions and references

Integer-to-integer and bool-to-integer conversions require `as`. Conditions require `bool`.

Coercion adjusts an expression to an expected type. The following rules are exhaustive:

| Source | Target | Requirement |
| --- | --- | --- |
| `T` | `T` | No type change; ordinary copy/move rules apply, except for mutable-reference reborrowing below |
| `&mut T` | `&mut T` | May reborrow for an expected mutable-reference type |
| `&S` or `&mut S` | `&T` | The source reference reaches a `T` place through zero or more builtin dereferences |
| `&mut S` | `&mut T` | The source reference reaches a mutable `T` place through zero or more builtin dereferences |
| Never | Any expected type | No value is produced on that path |

For the reference rules, first dereference the source reference, then follow the [builtin dereference rules](#autoderef-and-autoref) until its place type exactly matches the target's referent. A mutable target requires an outer `&mut` and no shared reference anywhere along that path. This supports `&Box<T>` to `&T`, `&mut Box<T>` to `&mut T` or `&T`, and further reference or `Box` layers. The adjustment borrows the reached place; it does not move its contents.

Ordinary coercion never borrows an owned value implicitly: neither `Box<T>` to `T` nor `Box<T>` to `&T` is supported. It does not convert stored elements of arrays, `Box`, or `Vec`, rewrite nested reference types, or unsize arrays or `Vec` to slices. All other types must agree.

### Expected types

These contexts provide expected types for ordinary coercions:

| Coercion site | Expected type |
| --- | --- |
| Annotated `let` initializer | Annotation |
| Assignment right operand | Destination |
| Function, method, constructor, or `Vec` argument | Declared parameter |
| Function tail or `return` operand | Declared result |
| Struct field initializer | Declared field |

An expected type propagates through parentheses and block tails, to both result branches of an `if`, and to the values of all `break` expressions targeting a `loop`. An expected array type `[T; N]` supplies `T` to its element expressions and requires length `N`. These rules apply recursively; they do not propagate through local bindings to their earlier initializers.

Conditions require `bool`, indices require `usize`, and operators constrain operands according to their own rules. Unit-result contexts, including loop bodies and an `if` without `else`, require `()`. A call's declared parameter type supplies an expected type to its argument even if the call's result has no expected type. Merely examining another branch or array element does not supply an expected type for reference coercion.

### Results without an expected type

For `if` branches, array elements, and a loop's `break` values, first ignore never results and infer the remaining types without reference coercions. All remaining results must have the same type after inference and integer defaulting. If every result is never, the common result type is never. Empty arrays follow the [zero-sized-data exclusion](undefined-behavior.md#zero-sized-data).

If different result types could be unified only by the supported reference reborrowing or dereference coercions, the source form is course UB. This includes finding a third reference type to which the results could all coerce. Other incompatible types remain static errors. Never fitting another result type is always supported, including a reference or container type.

This exclusion does not affect same-type results, integer inference, or ordinary construction of references and containers. It also does not change operator-specific adjustments or method receiver adjustments.

<details>
<summary>Result type examples</summary>

Assume `x` and `y` are `i32`, `x` is mutable, `b` is `Box<i32>`, and `flag` is `bool`. Each row is a separate example.

| Expression | Requirement |
| --- | --- |
| `let r = if flag { &x } else { &y };` | Infer `&i32`; no coercion |
| `let r = if flag { &mut x } else { &y };` | Course UB: no expected type for the reference coercion |
| `let r = if flag { &b } else { &x };` | Course UB: no expected type for the dereference coercion |
| `let a = [&mut x, &y];` | Course UB for the same reason |
| `let r: &i32 = if flag { &mut x } else { &y };` | Valid: both branches have expected type `&i32` |
| `let a: [&i32; 2] = [&mut x, &y];` | Valid: each element has expected type `&i32` |
| `let r = if flag { &x } else { 1i32 };` | Static error: no supported conversion unifies the types |

</details>

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

After the move, `p` cannot be used without reinitialization. Passing `p` to an `&mut i32` parameter may reborrow it, allowing later calls. The type of `moved` is still `&mut i32`; no later use turns this move into a reborrow.

</details>

## Autoderef and autoref

Autoderef follows this fixed set of builtin dereferences, one layer at a time:

| Place type | Dereferenced place | Access |
| --- | --- | --- |
| `&T` | `T` | Shared |
| `&mut T` | `T` | Mutable unless reached through a shared reference |
| `Box<T>` | `T` | Follows the box's access |

An immutable binding containing `&mut T` can still provide mutable access to `T`. Once a shared reference is crossed, further dereferences on that path cannot restore mutable access. `Vec` has no builtin dereference.

Field access stops at the first type declaring the field; indexing stops at the first array or `Vec`. Method calls use the [receiver candidate rules](expressions/method-call-expr.md#method-lookup). Each base or receiver is evaluated once. Explicit `*` performs one dereference.

Autoref inserts `&` or `&mut` for a method receiver when its selected signature requires it. Ordinary function arguments do not get autoref; write the borrow explicitly. For example, a parameter of type `&i32` accepts `&b` for `b: Box<i32>` by coercion, but does not accept `b`. These rules do not expand the [operator](expressions/operator-expr.md) or cast tables.

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
