# Language scope and test guarantees

## Three categories

| Category | Compiler responsibility | Test responsibility |
| --- | --- | --- |
| Supported, valid program | Compile and execute with the specified behavior | Provide expected observable results |
| Specified static error | Reject normally; diagnostic wording is not prescribed | Negative tests may target these rules |
| Excluded program construct, execution, or data use (course UB) | No required detection or result | Exclude it from positive, negative, and performance tests |

Unsupported syntax does not require implementing the corresponding feature or its specialized diagnostics. A supplied parser may recognize it; the compiler may reject it at the language-subset boundary. The precise negative-test and submission protocol is still to be published.

## Removed features

The language does not include user traits or trait implementations, user generics, enum, match, general patterns, nested items, tuple values/types other than unit, tuple structs, String, str, char, floating point, slices, closures, function values/pointers, async constructs, unsafe, raw pointers, or general macros. There is no exit builtin.

The finite derive attribute for Copy, Clone, PartialEq, and Eq is an exception to the macro exclusion. Box and Vec are builtin type constructors, not an exception allowing user generic definitions. Their [interfaces and conditional capabilities](heap.md) are defined; heap storage may remain allocated until program termination and be reclaimed together.

Only if, while, and loop are supported control-flow forms. There is no for, iterator protocol, if let, while let, or let-else. Destructuring assignment, type-position `_`, and explicit never-type annotations are unsupported. Diverging expressions still have never behavior. There is no use or module system; only the [listed paths](names.md#supported-paths) are supported. The old reference's module/import and pattern chapters do not introduce requirements.

## Execution guarantees

For the supplied inputs, official valid executions do not divide or take a remainder by zero, evaluate signed MIN / -1 or MIN % -1, access an array out of bounds, read uninitialized data, use moved values, or violate reference validity and borrowing rules. No dynamic checks, panic handling, or stack unwinding are required for these cases.

Wrapping arithmetic and masked shifts are defined by the [operator rules](expressions/operator-expr.md), not excluded as overflow errors. Integer literals outside their determined type's range are course UB; the valid signed-minimum forms remain supported. Let and ordinary parameter bindings that collide with a visible unqualified const name are also course UB. Equality expressions whose operand source types are not exactly identical are course UB, even where full Rust supplies a cross-type PartialEq implementation. These exclusions apply throughout the program, including const contexts where applicable and unreachable code; no positive, negative, or performance test contains them. See [Literals](lexical-structure.md#literals), [Names](names.md#settled-scope-rules), and [Equality](builtin-traits.md#equality).

Ordinary type errors remain compile-time errors. Dead code alone is not a static error; ordinary name and type checking still applies to it. These course UB exclusions do not require overflow, const-binding collision, or cross-type equality diagnostics.

These guarantees concern source semantics. Invalid accesses, unexpected termination, or wrong output introduced by code generation remain compiler failures. No static proof of termination, borrow validity, or ownership validity is required.

## Lifetime and ownership

Lifetime syntax, parameters, bounds, and annotations are absent. The compiler does not implement lifetime inference, borrow checking, or ownership dataflow checking. Tests guarantee the conditions in [References](references.md) and [Copy and move](builtin-traits.md). Ordinary type and place-mutability errors still require diagnosis.

In particular, a reference field without a lifetime annotation and a reference-returning signature with multiple possible input origins are legal course syntax. Tests ensure the actual referent remains valid.

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

There is no course limit on source-program size, compilation time, compiler peak memory, or assembly size. REIMU execution uses 256 MiB total memory and a 1 MiB stack. The reference container growth policy, 64 MiB cumulative heap high-water test guarantee, and validation steps are specified in the [backend contract](backend.md#42-参考-vec-策略与资源保证).
