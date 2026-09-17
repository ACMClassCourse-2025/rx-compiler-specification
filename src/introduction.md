# Rx Compiler Specification

This book describes the Rust subset used by the compiler course. The implementation target is RV32IM. Students implement the frontend, IR, code generation, and optimizations; a supplied lexer/parser may recognize a larger syntax than this language.

## Revision status

This is the **2026 working revision**, not a frozen assignment release. Confirmed decisions have been migrated into the chapters listed in the table of contents. Container APIs, conditional traits, finite paths, builtin name protection, reference adjustments, program-end heap reclamation, the reference allocator ABI, and the execution-memory baseline are specified. Integer-literal overflow, let/parameter collisions with visible unqualified const names, and equality between different source types are excluded as course UB. The previously listed language choices are settled; supplied-parser and end-to-end runtime integration remain publication work tracked in [Open decisions](open-decisions.md). Assessment and scoring arrangements are separate course documents.

A pending interface is not an implicit requirement to implement the corresponding Rust standard library API. It must be specified before tests depending on it are released.

## Authority and scope

The chapters in this book's table of contents define this revision. Other Markdown files retained in this repository are historical material from the previous course specification or the upstream Rust Reference; they do not add language features or override these chapters. Old test cases and reference compiler behavior are evidence for migration, not independent specifications.

Only the listed constructs are supported. References to Rust behavior apply to those constructs and the explicitly stated rules; they do not import other syntax, traits, library functions, or diagnostics. Source references have no lifetime annotations or checking, and internal calling conventions are implementation-defined.

Rust 2021 storage-scope rules serve as a reference for valid test construction. Runtime integer behavior uses overflow checks disabled. Neither choice requires students to implement Cargo profiles or edition switching.

## Reading and examples

Code blocks describe the course language unless explicitly labeled as C, assembly, or shell. They are not automatically valid rustc programs: course builtins and lifetime-erased signatures differ from Rust. Examples with unsupported syntax explain exclusions only and are labeled accordingly. The grammar covers the settled core; pending extensions are identified separately.

The source grammar and static rules determine accepted programs. [Test guarantees](undefined-behavior.md) identify cases for which compilation or execution is not assessed. A compiler error is distinct from a crash, and a compiler bug on a valid program is not excused by those guarantees.
