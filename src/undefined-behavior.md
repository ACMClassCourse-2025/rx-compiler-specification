# Language scope and test guarantees

## Three categories

| Category | Compiler responsibility | Test responsibility |
| --- | --- | --- |
| Supported, valid program | Compile and execute with the specified behavior | Provide expected observable results |
| Specified static error | Reject normally; diagnostic wording is not prescribed | Negative tests may target these rules |
| Excluded program construct, execution, or data use (course UB) | No required detection or result | Exclude it from positive, negative, and performance tests |

Unsupported syntax does not require implementing the corresponding feature or its specialized diagnostics. A supplied parser may recognize it; the compiler may reject it at the language-subset boundary. The precise negative-test and submission protocol is still to be published.

## Language features

The language provides functions, named-field structs, constants, inherent impls, identifier bindings, integer and boolean operations, fixed arrays, references, and builtin Box/Vec containers. Source types and their composition are listed in [Types](types.md), and the [grammar summary](grammar-summary.md) collects the syntax.

Structs can derive Copy, Clone, PartialEq, and Eq. Box and Vec are builtin type constructors with the [interfaces and conditional capabilities](heap.md) specified in this book. Heap storage may remain allocated until program termination and be reclaimed together.

Control flow uses if, while, loop, return, break, and continue. Diverging expressions have never behavior during type checking. Names belong to a single source compilation unit and resolve through the [specified paths](paths.md#path-resolution). The source grammar and static rules determine which other forms are rejected.

## Execution guarantees

Official valid executions satisfy the following contracts:

| Contract | Defining rules |
| --- | --- |
| Integer operation preconditions and defined arithmetic | [Operators](expressions/operator-expr.md#arithmetic-and-bits) |
| Array indices | [Index expressions](expressions/array-expr.md#array-index-expressions) |
| Container access and mutation | [Heap operations](heap.md) |
| Initialization, moves, and ownership | [Value semantics](builtin-traits.md#value-semantics) |
| Lifetime annotations, borrowing, and storage validity | [References](references.md) |

Tests also exclude these source constructs throughout the program, including
unreachable code and constant contexts where applicable:

| Course UB | Defining rules |
| --- | --- |
| Integer literals outside the determined type's range | [Integer literal range](expressions/literal-expr.md#integer-typing-and-range) |
| Let/parameter collisions with visible unqualified constants | [Name rules](names.md#settled-scope-rules) |
| Equality between different source types | [Equality](builtin-traits.md#equality) |
| Incorrect lifetime declarations, uses, or elision | [Lifetime validity](references.md#lifetime-validity) |

These are guarantees about source programs. Invalid accesses, unexpected
termination, or wrong output introduced by code generation remain compiler
failures. Ordinary name, type, and place-mutability errors remain static
errors, including in unreachable code. No static proof of termination,
borrow validity, or ownership validity is required. Runtime checks, panic
handling, and stack unwinding for the excluded executions are not required.

## Zero-sized data

Unit `()` is used as a function or control-flow result. A void call may occur as an expression statement; functions may omit their return type, return naturally, use `return;`, or explicitly return unit.

The following are course UB and occur in no tests; no diagnosis is required:

- Zero-sized function or method parameters, bindings, constants, statics, or assignment destinations.
- Empty structs or zero-sized struct fields.
- Zero-length arrays or zero-sized array elements.
- Taking a reference to a zero-sized value or dereferencing a reference to zero-sized data.
- Other uses requiring an observable storage location for zero-sized data.

An empty Vec with a nonzero-sized element type is not itself zero-sized. Its internal representation is implementation-defined and its operations follow the container contract.

## Resources

Execution memory, stack size, the reference allocation baseline, and resource
guarantees are specified by the [backend contract](backend.md#42-参考-vec-策略与资源保证).
[Compilation and output resource rules](backend.md#5-汇编产物与执行行为) are defined there as well.
