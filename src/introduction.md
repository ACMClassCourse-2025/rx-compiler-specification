# Rx Compiler Specification

This book defines the Rust subset used by the compiler course and its RV32IM execution target. Students implement the frontend, IR, optimizations, and code generation on top of the supplied lexer and parser.

## Status and authority

This is the **2026 working revision**. Every assessed interface must be specified before tests depend on it. [Publication validation](open-decisions.md) lists the remaining integration checks; assessment and scoring are separate course documents.

Only chapters listed in this book's table of contents are normative. The grammar and static rules define the supported source language. A reference to Rust behavior applies only to supported constructs and the rules stated here.

## How to read the specification

Code blocks use the course language unless labeled otherwise. Grammar defines accepted forms, while [language scope and test guarantees](undefined-behavior.md) distinguish valid programs, static errors, and excluded cases called *course UB*.

A compiler error is not a runtime crash. Likewise, course UB does not excuse incorrect behavior for a valid program. Runtime integer behavior is defined by [Operators](expressions/operator-expr.md), and storage duration follows the supported Rust 2021 rules in [References](references.md).
