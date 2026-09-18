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

The reference variants supplied by Rust for these primitive types are also supported. The following table is exhaustive for arithmetic and bit operations. T denotes one supported scalar type, I denotes a supported signed integer type, and L/R denote independently chosen supported integer types. These letters describe rules, not user-generic syntax.

| Operation | Operand types | Result |
| --- | --- | --- |
| Binary arithmetic `+`, `-`, `*`, `/`, `%` | Each operand is T or &T, with the same integer T on both sides | T |
| Binary bitwise `&`, <code>&#124;</code>, `^` | Each operand is T or &T, with the same integer or bool T on both sides | T |
| Shift `<<`, `>>` | Left is L or &L; right is R or &R | L |
| Unary `-` | I or &I | I |
| Unary `!` | T or &T, for integer or bool T | T |
| Arithmetic/bitwise compound assignment | A mutable place of primitive type T; right is T or &T for the corresponding operation | Unit |
| Shift compound assignment | A mutable place of primitive integer type L; right is R or &R | Unit |

These arithmetic/bitwise reference variants accept one shared-reference layer, not arbitrary automatic dereferencing. For example, `&a + &b`, `-&a`, and `x += &y` are supported for compatible primitives; `&&a + &b`, `-&mut a`, and `x += &mut y` are not. A mutable-reference operand can be explicitly dereferenced to obtain the scalar value. A reference-valued assignment destination is not automatically dereferenced: use `*p += rhs` to update the primitive behind p.

The operations compute scalar results; they do not change a referent merely because an operand is a reference. Evaluating a reference operand produces its reference value; the scalar load needed by the operation occurs when that operation is performed, after its operands have been evaluated. Compound-assignment ordering is specified below.

These are finite builtin operations. They do not introduce user Add/AddAssign/Deref implementations or make those traits source-visible.

All supported integers have 32 bits. Runtime addition, subtraction, multiplication, and signed negation wrap using two's-complement arithmetic, as with Rust overflow checks disabled. Optimized and unoptimized code have the same behavior. LLVM overflow flags must not assert non-overflow unless the compiler has proved it for the particular operation.

Signed division truncates toward zero; a nonzero remainder has the sign of the dividend. Division/remainder by zero and signed MIN divided or reduced modulo -1 are excluded from valid executions, even though Rust would panic for them in release builds. No check or panic runtime is required.

Shifts accept integer operands, following Rust's primitive shift typing (the right operand need not have the same integer type as the left). The result has the left operand's type. Runtime shift counts use their low five bits. Signed right shift is arithmetic; unsigned right shift is logical. This explicitly specified masking must also hold in constant-folded ordinary expressions.

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

`==` and `!=` are defined only for operands with exactly the same source type after ordinary local inference and the supported PartialEq capability. Equality can constrain an unresolved operand type, including an unsuffixed integer literal, to the other operand's type; it does not coerce already determined, different types. They borrow operands and do not move non-Copy values. Struct/array comparison is structural and same-typed references compare their targets, as specified in [Builtin traits](../builtin-traits.md). Cross-type equality expressions are course UB and absent from all tests; they do not request an implicit coercion or a general two-type trait implementation.

Scalar ordering uses `<`, `<=`, `>`, `>=` on matching integer types and bool (false precedes true). Their Rust reference variants are supported and compare target values, not addresses. The underlying ordering implementations compare matching shared-reference layers or matching mutable-reference layers, recursively ending in the same supported scalar type. At the expression boundary, Rust's permitted right-operand reborrow can convert a mutable reference to a shared one; it does not convert shared to mutable or automatically rewrite nested reference layers. For example, `&a < &b`, `&mut a < &mut b`, and `&a < &mut b` work for matching ordered scalars; `&mut a < &b` and `&&a < &&mut b` do not. There is no automatic value/reference comparison such as `a < &b`.

Same-typed reference equality follows the existing PartialEq rules recursively and is not restricted to the single shared-reference layer of arithmetic. Shared and mutable references have different source types, so mixed-mutability equality is excluded by the same-type test-domain rule. This equality restriction does not change the separately specified scalar-ordering reference variants, or add PartialOrd/Ord derives, struct/array/container ordering, or user-defined operator implementations.

`&&` and `||` require bool values, return bool, and short-circuit from left to right. They have no reference-operand variants: `&true && true` is invalid. There is no integer truthiness. Comparison chains such as `a < b < c` require parentheses and compatible intermediate types.

## Casts

```grammar,expressions
TypeCastExpression -> Expression `as` TypeNoBounds
```

`as` supports integer-to-integer casts and bool-to-integer casts. Since all integers are 32 bits, integer casts preserve the 32-bit pattern and interpret it in the destination signedness. False becomes 0 and true becomes 1. Integer-to-bool and reference-to-integer casts are not supported.

Reference borrowing/reborrowing adjustments are governed by the reference/type rules; this numeric cast table does not introduce raw pointers, arbitrary reinterpretation, or lifetime conversion syntax.

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

## Evaluation order

| Expression form | Evaluation order |
| --- | --- |
| Unary scalar or shared-reference operation | Evaluate the operand, then perform the operation |
| Ordinary binary arithmetic, bitwise, shift, or comparison, including reference variants | Evaluate the left operand, then the right, then perform the operation |
| Lazy boolean `&&` / <code>&#124;&#124;</code> | Evaluate the left bool; evaluate the right only when required |
| Ordinary assignment `place = value` | Evaluate the right value, then the destination address, then store |
| Compound assignment with primitive scalar or shared-reference right operand | Evaluate the right operand, then the destination address, then perform the scalar read/update |

All supported compound assignments use the same right-first order, independently of whether the right operand is a scalar or a shared reference. Each operand expression and destination-address computation is evaluated once. The destination's old value is read after operand evaluation. A reference right operand is evaluated to its reference value first; its target is read when the scalar operation is performed. Borrow validity continues to be guaranteed by tests, not checked by a required borrow checker.

For example, under the course rules:

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

This is an intentional simplification of the [Rust compound-assignment rules](https://doc.rust-lang.org/reference/expressions/operator-expr.html#compound-assignment-expressions), which give the reference variant left-first method-call order. Official tests do not depend on the relative evaluation order of the two operands of compound assignment; in particular, they do not distinguish the course rule from Rust using observable side effects. The example above illustrates the specified behavior rather than an assessment case.

Ending or replacing an old value does not imply clearing its bytes. Replacing a container does not require recursive destruction or immediate deallocation; its old heap storage may remain until [program-end reclamation](../heap.md#program-end-reclamation).
