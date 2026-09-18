# Operators

```grammar,expressions
OperatorExpression ->
      BorrowExpression
    | DereferenceExpression
    | NegationExpression
    | ArithmeticOrLogicalExpression
    | ComparisonExpression
    | LazyBooleanExpression
    | TypeCastExpression
    | AssignmentExpression
    | CompoundAssignmentExpression
```

## Arithmetic and bits

```grammar,expressions
NegationExpression ->
      `-` Expression
    | `!` Expression

ArithmeticOrLogicalExpression ->
      Expression `+` Expression
    | Expression `-` Expression
    | Expression `*` Expression
    | Expression `/` Expression
    | Expression `%` Expression
    | Expression `&` Expression
    | Expression `|` Expression
    | Expression `^` Expression
    | Expression `<<` Expression
    | Expression `>>` Expression
```

Arithmetic `+`, `-`, `*`, `/`, `%` operates on compatible integer operands without implicit integer conversion. Unary minus requires a signed integer. Bitwise `&`, `|`, `^`, and `!` operate on integers; they also provide non-short-circuit boolean operations. Boolean `!` negates truth.

Primitive arithmetic and bit operations support the scalar and reference operands in the following table. T denotes one supported scalar type, I denotes a supported signed integer type, and L/R denote independently chosen supported integer types.

| Operation | Operand types | Result |
| --- | --- | --- |
| Binary arithmetic `+`, `-`, `*`, `/`, `%` | Each operand is T or &T, with the same integer T on both sides | T |
| Binary bitwise `&`, <code>&#124;</code>, `^` | Each operand is T or &T, with the same integer or bool T on both sides | T |
| Shift `<<`, `>>` | Left is L or &L; right is R or &R | L |
| Unary `-` | I or &I | I |
| Unary `!` | T or &T, for integer or bool T | T |
| Arithmetic/bitwise compound assignment | A mutable place of primitive type T; right is T or &T for the corresponding operation | Unit |
| Shift compound assignment | A mutable place of primitive integer type L; right is R or &R | Unit |

<details>
<summary>Reference operand details</summary>

These arithmetic/bitwise reference variants accept one shared-reference layer, not arbitrary automatic dereferencing. For example, `&a + &b`, `-&a`, and `x += &y` are supported for compatible primitives; `&&a + &b`, `-&mut a`, and `x += &mut y` are not. A mutable-reference operand can be explicitly dereferenced to obtain the scalar value. A reference-valued assignment destination is not automatically dereferenced: use `*p += rhs` to update the primitive behind p.

The operations compute scalar results; they do not change a referent merely because an operand is a reference. Evaluating a reference operand produces its reference value; the scalar load needed by the operation occurs when that operation is performed, after its operands have been evaluated. Compound-assignment ordering is specified below.

The table defines the complete builtin operand combinations for these operations.

</details>

All supported integers have 32 bits. Runtime addition, subtraction, multiplication, and signed negation wrap using two's-complement arithmetic, as with Rust overflow checks disabled. Optimized and unoptimized code have the same behavior. LLVM overflow flags must not assert non-overflow unless the compiler has proved it for the particular operation.

Signed division truncates toward zero; a nonzero remainder has the sign of the dividend. Division/remainder by zero and signed MIN divided or reduced modulo -1 are excluded from valid executions. No check or panic runtime is required.

Shifts accept integer operands whose types may differ. The result has the left operand's type. Runtime shift counts use their low five bits. Signed right shift is arithmetic; unsigned right shift is logical. This masking also applies to constant-folded ordinary expressions.

## Comparison and logic

```grammar,expressions
ComparisonExpression ->
      Expression `==` Expression
    | Expression `!=` Expression
    | Expression `>` Expression
    | Expression `<` Expression
    | Expression `>=` Expression
    | Expression `<=` Expression

LazyBooleanExpression ->
      Expression `||` Expression
    | Expression `&&` Expression
```

Equality operators use the [PartialEq and equality rules](../builtin-traits.md#equality), including operand typing, implicit borrowing, and structural comparison. Equality between different source types is course UB; see the [cross-type equality guarantee](../undefined-behavior.md#cross-type-equality).

Scalar ordering uses `<`, `<=`, `>`, `>=` on matching integer types and bool (false precedes true). Their Rust reference variants are supported and compare target values, not addresses. The underlying ordering implementations compare matching shared-reference layers or matching mutable-reference layers, recursively ending in the same supported scalar type. At the expression boundary, Rust's permitted right-operand reborrow can convert a mutable reference to a shared one; it does not convert shared to mutable or automatically rewrite nested reference layers. For example, `&a < &b`, `&mut a < &mut b`, and `&a < &mut b` work for matching ordered scalars; `&mut a < &b` and `&&a < &&mut b` do not. There is no automatic value/reference comparison such as `a < &b`.

`&&` and `||` require bool values, return bool, and short-circuit from left to right. They have no reference-operand variants: `&true && true` is invalid. There is no integer truthiness. Comparison chains such as `a < b < c` require parentheses and compatible intermediate types.

## Casts

```grammar,expressions
TypeCastExpression -> Expression `as` TypeNoBounds
```

`as` supports integer-to-integer casts and bool-to-integer casts. Since all integers are 32 bits, integer casts preserve the 32-bit pattern and interpret it in the destination signedness. False becomes 0 and true becomes 1. Integer-to-bool and reference-to-integer casts are not supported.

Reference borrowing and reborrowing adjustments follow the [reference coercion rules](../types.md#conversions-and-references).

### Cast parsing

<details>
<summary>Disambiguating generic arguments from operators</summary>

The [operator precedence table](../expressions.md#precedence) governs expression nesting. After a type-path segment in a cast, `<` begins [GenericArgs] rather than a comparison. A leading `<` from `<<` likewise enters type-argument parsing. Parenthesizing the cast makes the intended operation explicit: `(x as usize) < y` and `(x as usize) << y`. A parenthesized type already closes the type syntax, so `x as (usize) < y` and `x as (usize) << y` also parse as comparison and shift. Operators such as `<=`, `>`, `>>`, and `==` follow ordinary precedence after the cast type.

The parser checks the resulting syntax; name resolution checks its type arguments. Thus `x as usize<i32>` has a type-path syntax tree but is a static error because the primitive usize takes no type arguments.

</details>

## Borrow, dereference, and assignment

```grammar,expressions
BorrowExpression ->
      (`&` | `&&`) Expression
    | (`&` | `&&`) `mut` Expression

DereferenceExpression -> `*` Expression

AssignmentExpression -> Expression `=` Expression

CompoundAssignmentExpression ->
      Expression `+=` Expression
    | Expression `-=` Expression
    | Expression `*=` Expression
    | Expression `/=` Expression
    | Expression `%=` Expression
    | Expression `&=` Expression
    | Expression `|=` Expression
    | Expression `^=` Expression
    | Expression `<<=` Expression
    | Expression `>>=` Expression
```

`&place` forms a shared reference; `&mut place` requires a mutable place. Applied to a value expression, borrowing materializes a temporary. `*reference` accesses its target, and `*box` accesses the owned T under the [Box rules](../heap.md#box-access-and-moves). Other types, including Vec, are not dereference operands. In prefix borrow position, `&&x` means `&(&x)`; infix `left && right` is short-circuit boolean and.

`place = value` stores with the required copy/move behavior. Compound assignment applies the corresponding operation to the current scalar value at the destination. Both forms produce unit. The following order rules apply even if reference-operation lowering ultimately uses the same scalar instructions.

## Assignment destinations

An assignment destination is one mutable [place](../expressions.md#places-and-values). The assigned value must have a compatible type. A destination that fails these requirements is a static error.

Assignments such as `s = other;` and `a = other_array;` store an entire struct or array in one place.

## Evaluation order

| Expression form | Evaluation order |
| --- | --- |
| Unary scalar or shared-reference operation | Evaluate the operand, then perform the operation |
| Ordinary binary arithmetic, bitwise, shift, or comparison, including reference variants | Evaluate the left operand, then the right, then perform the operation |
| Lazy boolean `&&` / <code>&#124;&#124;</code> | Evaluate the left bool; evaluate the right only when required |
| Ordinary assignment `place = value` | Evaluate the right value, then the destination address, then store |
| Compound assignment with primitive scalar or shared-reference right operand | Evaluate the right operand, then the destination address, then perform the scalar read/update |

All supported compound assignments use the same right-first order, independently of whether the right operand is a scalar or a shared reference. Each operand expression and destination-address computation is evaluated once. The destination's old value is read after operand evaluation. A reference right operand is evaluated to its reference value first; its target is read when the scalar operation is performed. Borrow validity continues to be guaranteed by tests, not checked by a required borrow checker.

<details>
<summary>Compound-assignment order example</summary>

Under the course rules:

```rust,ignore
fn destination(p: &mut i32) -> &mut i32 {
    printlnInt(2);
    p
}

fn right_value() -> i32 {
    printlnInt(1);
    3
}

fn right_reference(p: &i32) -> &i32 {
    printlnInt(1);
    p
}

fn main() {
    let mut x = 0;
    let y = 3;
    *destination(&mut x) += right_value();       // prints 1, then 2
    *destination(&mut x) += right_reference(&y); // prints 1, then 2
}
```

Official tests are independent of the relative evaluation order of the two operands of compound assignment. The example above illustrates the specified behavior and is outside that assessment domain.

</details>

Ending or replacing an old value does not imply clearing its bytes. Replacing a container does not require recursive destruction or immediate deallocation; its old heap storage may remain until [program-end reclamation](../heap.md#program-end-reclamation).
