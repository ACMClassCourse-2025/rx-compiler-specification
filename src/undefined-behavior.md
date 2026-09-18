# Language scope and test guarantees

## Conformance categories

| Category | Compiler requirement | Test policy |
| --- | --- | --- |
| Valid program | Compile and produce the specified behavior | May appear in any applicable test |
| Static error | Reject normally; diagnostic wording is unspecified | May appear in negative tests |
| Course UB | No required diagnostic or behavior | Excluded as stated by its defining rule |

Unsupported syntax may be rejected at the language-subset boundary even if the supplied parser recognizes it. Ordinary name, type, and place-mutability errors remain static errors, including in unreachable code. The compiler need not prove termination, borrow validity, or ownership validity. Runtime checks, panic handling, and stack unwinding are not required for excluded executions.

## Language subset

Rx is a subset of Rust 2021. Every Rx source program uses valid Rust syntax, although this specification defines its builtin environment and required behavior.

Rx includes:

- Functions, named-field structs, constants, and inherent implementations.
- Identifier bindings and local type inference.
- Integers, booleans, fixed arrays, references, and builtin `Box` and `Vec` containers.
- `if`, `while`, `loop`, `return`, `break`, and `continue` control flow.
- Struct derives for `Copy`, `Clone`, `PartialEq`, and `Eq`.

<details>
<summary>Unsupported Rust features</summary>

The course subset excludes:

- User traits, trait bounds, and trait objects.
- Enums, non-unit tuples, tuple structs, tuple variants, and unit variants.
- Pattern matching, destructuring, and guards.
- Type parameters and general generics. Lifetime syntax is supported, but its validity is guaranteed rather than checked.
- Modules, visibility, privacy, macros, and arbitrary attributes.
- Unsafe Rust and the standard library beyond the builtins defined here.
- General constant evaluation and const generics. Constant propagation remains an optional optimization.
- Integer overflow checks.

</details>

[Types](types.md), [expressions](expressions.md), and the [grammar summary](grammar-summary.md) define the complete subset. Forms outside that grammar are unsupported.

## Test guarantees

Valid executions obey these contracts:

| Contract | Rules |
| --- | --- |
| Integer arithmetic and preconditions | [Operators](expressions/operator-expr.md#arithmetic-and-bits) |
| Array indices | [Index expressions](expressions/array-expr.md#array-index-expressions) |
| Container operations | [Box and Vec](heap.md) |
| Initialization, copying, moves, and ownership | [Value semantics](builtin-traits.md#value-semantics) |
| Lifetimes, borrowing, and storage | [References](references.md) |

The following source forms are course UB and never appear in tests, including in unreachable code and constant contexts where applicable:

| Course UB | Defining rules |
| --- | --- |
| Integer literal outside its determined type | [Integer literal range](#integer-literal-range) |
| Let or parameter name colliding with a visible unqualified constant | [Constant-name collisions](#constant-name-collisions) |
| Equality between different source types | [Cross-type equality](#cross-type-equality) |
| Invalid lifetime declaration, use, bound, or elision | [Lifetime validity](#lifetime-validity) |
| Observable zero-sized data use outside the unit-result exception | [Zero-sized data](#zero-sized-data) |

These guarantees constrain source programs. Invalid memory access, incorrect termination, or wrong output introduced by compilation is still a compiler failure.

### Integer literal range

After suffix resolution and inference, a literal outside its determined type's range is course UB. For example, positive `2147483648i32` and `4294967296u32` are excluded, with no required overflow diagnostic or wrapped result.

The [literal rules](expressions/literal-expr.md#integer-typing-and-range) still permit signed minima such as `-2147483648i32` and `-(2147483648i32)`. Ordinary type errors and [runtime arithmetic wrapping](expressions/operator-expr.md#arithmetic-and-bits) are unchanged.

### Constant-name collisions

A `let` or ordinary parameter that matches a visible unqualified constant is course UB, regardless of mutability or declaration order. No diagnostic or constant-pattern interpretation is required. An associated constant reachable only as `Type::NAME` does not exclude a local `NAME`. The [protected builtin](names.md#protected-builtin-names) prohibition still applies.

### Cross-type equality

Every `==` or `!=` whose inferred operands have different source types is course UB. No diagnostic, coercion, or cross-type `PartialEq` implementation is required. This includes shared versus mutable references, different array types, different container element types, and `Vec` versus array comparisons.

The [equality rules](builtin-traits.md#equality) define inference, capability requirements, and same-type comparison. Lifetime arguments alone do not distinguish source types under [type identity](types.md#supported-types).

### Lifetime validity

Lifetime declarations, annotations, arguments, bounds, and elision follow the supported Rust 2021 rules described in [References](references.md#lifetime-validity). Tests guarantee their validity throughout the program. Invalid lifetime use is course UB and appears in no positive, negative, or performance test.

<details>
<summary>Excluded lifetime errors</summary>

Tests exclude undeclared or duplicate lifetimes, invalid shadowing, incorrect argument counts or placement, forbidden explicit late-bound arguments, ambiguous missing annotations, unsatisfied outlives bounds, invalid `'static` claims, rejected unused parameters, and bodies or uses that fail to uphold their annotations.

Malformed tokens and syntax retain their ordinary lexer and parser behavior. Other type and place-mutability errors remain static errors. No lifetime or ownership checker is required; actual references must still obey the [borrowing rules](references.md#borrowing-rules).

</details>

### Zero-sized data

Unit `()` is valid as a function or control-flow result. A void call may be an expression statement; a function may omit its return type, end naturally, use `return;`, or explicitly return unit.

<details>
<summary>Excluded zero-sized uses</summary>

All other uses requiring observable zero-sized storage are course UB, including:

- Zero-sized parameters, bindings, constants, statics, or assignment destinations.
- Empty structs or zero-sized fields.
- Zero-length arrays or zero-sized array elements.
- References to zero-sized values and dereferences of zero-sized data.

</details>

An empty `Vec<T>` with nonzero-sized `T` is valid and is not itself zero-sized. Its representation is implementation-defined.

Resource limits, stack size, allocation policy, and output constraints are part of the [backend contract](backend.md#resource-guarantees).
