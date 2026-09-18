# Rx Compiler Specification

Welcome to the Rx Compiler course! In this course, you are required to write a compiler for a subset of [Rust](https://www.rust-lang.org/) we call *Rx*. The compiler will target the [RV32IM](https://en.wikipedia.org/wiki/RISC-V) instruction set architecture. To speed things up, we will provide you with the [ANTLR G4](https://www.antlr.org/) representation for this language, which automates the parsing process so you will start working with an AST tree.

This book defines the syntax and semantics of Rx rigorously, and it, alongside the official Rust Compiler, serves as the source truth for this course. If you find any ambiguities or disagreements between the two, do not hesitate to reach out to your TA for clarification.

## How to read the specification

Before you start, we recommend taking a look into any beginner tutorial for Rust to get started. You do not need to dive deep since we are implementing a subset, but some basic familiarity with the Rust syntax is definitely recommended.

Start by reading [language scope and test guarantees](undefined-behavior.md), which distinguish valid programs, static errors, and excluded cases i.e. *course UB*. Get an idea of what features are required, and which are not.

Then jump into the [lexical structure](lexical-structure.md), as well as [items](items.md), [types](types.md), and [expressions](expressions.md). These sections define the complete grammar of the Rx language. There is a quick lookup table in the [grammar summary](grammar-summary.md) section.

While reading, we recommend doing experiments with the rust compiler. You may also do experiments on any online Rust compiler like [this](https://play.rust-lang.org/?version=stable&mode=debug&edition=2024) or [this](https://godbolt.org/) to see what compiles or what not. Note that we **do not treat compiler warnings as static errors**.
