# Language scope and test guarantees

## Language features

You are not expected to implement the entire Rust programming language. Instead, we picked out a subset of the language we call RX. Every Rx program is a valid Rust(.rs) program.

In Rx you should support these features:

- Named-field structs, inherent impls.
- Limited derivation attributes `Copy`, `Clone`, `PartialEq`, and `Eq` on structs.
- Integer and boolean constants, without constant propagation;
- Functions;
- Identifier bindings, integer and boolean operations, fixed arrays, references, builtin Box and Vec containers and control flow;
- Type inference.

You need NOT support these features that Rust does:

- Traits, trait bounds, and trait objects;
- Enums, tuples, tuple structs, tuple variants, and unit variants;
- Pattern matching, destructuring, and guards;
- Generics, type parameters, and lifetimes (Rx has lifetime markers but you need not check them and we guarantee that they are always valid);
- Modules, visibility, and privacy;
- Macros and arbitrary attributes;
- Unsafe rust;
- Constant propagation, const generics, and const evaluation;
- Integer overflow checks;
- The standard library.

For more information: Source types and their composition are listed in [Types](types.md), and the [grammar summary](grammar-summary.md) collects the syntax used by Rx.

## Test guarantees

There are many corner cases that is rather difficult to implement and so we excluded these from the test suite. The following table lists UB (Undefined Behavior) that is guaranteed to be excluded from the tests.

| Course UB | Defining rules |
| --- | --- |
| Integer literals outside the determined type's range | [Integer literal range](#integer-literal-range) |
| Let/parameter collisions with visible unqualified constants | [Constant-name collisions](#constant-name-collisions) |
| Equality between different source types | [Cross-type equality](#cross-type-equality) |
| Incorrect lifetime declarations, uses, or elision | [Lifetime validity](#lifetime-validity) |
| Zero-sized data as a parameter, binding, constant, static, assignment destination, or array element | [Zero-sized data](#zero-sized-data) |

### Integer literal range

After suffix resolution and type inference, an integer literal outside its determined type's range is course UB. For example, positive `2147483648i32` and `4294967296u32` are excluded. Tests contain no such literal, including in constant contexts and unreachable code; no overflow diagnostic or wrapped result is required.

The [integer typing and range rules](expressions/literal-expr.md#integer-typing-and-range) permit signed-minimum unary-minus literal forms such as `-2147483648i32` and `-(2147483648i32)`. These forms are valid. Ordinary static type errors and the [wrapping behavior of integer arithmetic](expressions/operator-expr.md#arithmetic-and-bits) retain their specified rules.

### Constant-name collisions

A let or ordinary parameter binding with the same name as a const visible as an unqualified value name at that position is course UB. This applies to both mutable and immutable bindings, regardless of the const's textual definition order. Tests contain no such collision, including in unreachable code, and no diagnostic or constant-pattern interpretation is required.

For example, with a visible `const LIMIT: i32 = 1;`, both `let LIMIT = 2;` and a parameter `LIMIT: i32` are excluded. An associated constant accessible only as `Type::LIMIT` does not by itself exclude a local `LIMIT`. Visibility follows the [name and scope rules](names.md#settled-scope-rules). The static prohibition on shadowing [protected builtins](names.md#protected-builtin-names) still applies.

### Cross-type equality

Every `==` or `!=` expression whose operands have different source types after ordinary local type inference is course UB. Tests exclude all such expressions; no diagnostic, coercion, or cross-type PartialEq implementation is required. Excluded cases include shared versus mutable references, different array lengths or element types, different Box/Vec element types, and Vec versus array comparisons.

The [equality rules](builtin-traits.md#equality) define operand type inference, the PartialEq requirement, and comparisons between operands with the same source type. Lifetime arguments alone do not distinguish source types under the [type identity rules](types.md#supported-types).

### Lifetime validity

Lifetime declarations, annotations, arguments, bounds, and elision must satisfy the [lifetime validity rules](references.md#lifetime-validity). Tests guarantee their validity throughout each program, including signatures, struct fields, function bodies, and unreachable code. An incorrect lifetime declaration or use is course UB and appears in no positive, negative, or performance test. This covers:

- Undeclared lifetime names, invalid parameter names, duplicate declarations, and forbidden lifetime-parameter shadowing.
- Lifetime arguments with an incorrect number, order, or placement, including explicit arguments where Rust requires a late-bound lifetime to be inferred.
- Missing annotations where Rust's elision rules cannot determine a lifetime, such as a reference field without an explicit lifetime or an ambiguous reference-returning signature.
- Unsatisfied lifetime or type-outlives bounds, and function bodies or uses that fail to uphold their annotated lifetimes, including `'static`.
- Unused lifetime parameters when Rust rejects the declaration.

The contract applies to well-formed lifetime syntax. Malformed tokens and syntax retain their ordinary lexical and parsing rules. Other type and place-mutability errors remain the specified static errors. No lifetime or ownership checks are required; actual reference uses must satisfy the [borrowing and storage-validity guarantees](references.md#static-checks-and-valid-borrowing).

### Zero-sized data

Unit `()` is used as a function or control-flow result. A void call may occur as an expression statement; functions may omit their return type, return naturally, use `return;`, or explicitly return unit.

The following are course UB and occur in no tests; no diagnosis is required:

- Zero-sized function or method parameters, bindings, constants, statics, or assignment destinations.
- Empty structs or zero-sized struct fields.
- Zero-length arrays or zero-sized array elements.
- Taking a reference to a zero-sized value or dereferencing a reference to zero-sized data.
- Other uses requiring an observable storage location for zero-sized data.

An empty Vec with a nonzero-sized element type is **not** itself zero-sized. Its internal representation is implementation-defined.
