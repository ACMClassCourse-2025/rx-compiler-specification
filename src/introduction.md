# Rx Compiler Specification

This book defines the Rust subset used by the compiler course. The implementation target is RV32IM. Students implement the frontend, IR, code generation, and optimizations, using the supplied lexer/parser as the frontend foundation.

## Revision status

This is the **2026 working revision**. It specifies the language, builtin containers and traits, reference validity, program-end heap reclamation, the runtime interface, and the RV32IM execution contract. Supplied-parser and end-to-end runtime integration are tracked in [publication validation](open-decisions.md). Assessment and scoring arrangements are separate course documents.

Each assessed interface must be specified before tests depending on it are released.

## Authority and scope

The chapters in this book's table of contents define the language and its execution contract. Other repository documents, migration records, test cases, and reference implementations provide supporting material; the chapters listed here take precedence.

The grammar and static rules specify the supported constructs. References to Rust behavior apply to those constructs and the explicitly stated rules. Reference syntax records a referent type and mutability, while tests guarantee lifetime and borrowing validity. Internal calling conventions are implementation-defined.

Rust 2021 storage-scope rules serve as a reference for valid test construction. Runtime integer arithmetic uses the wrapping and shift rules in [Operators](expressions/operator-expr.md).

## Reading and examples

Code blocks describe the course language unless explicitly labeled as C, assembly, or shell. They use the builtin functions and reference signatures defined in this book. Examples of invalid or excluded programs are labeled accordingly. Each language chapter presents its grammar and the corresponding static and execution rules.

The source grammar and static rules determine accepted programs. [Test guarantees](undefined-behavior.md) identify cases for which compilation or execution is not assessed. A compiler error is distinct from a crash, and a compiler bug on a valid program is not excused by those guarantees.
