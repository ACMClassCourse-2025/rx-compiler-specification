# Rx Compiler Specification

Welcome to the Rx Compiler course! In this course, you are required to write a compiler for a subset of [Rust](https://www.rust-lang.org/) we call *Rx*. The compiler will target the [RV32IM](https://en.wikipedia.org/wiki/RISC-V) instruction set architecture. To speed things up, we will provide you with the [ANTLR G4](https://www.antlr.org/) representation for this language, which automates the parsing process so you will start working with an AST tree.

The compiler must use LLVM IR as its intermediate representation and support emitting it as text. This lets you validate the frontend using Clang before implementing your own RISC-V backend, as described in [Backend](backend.md#llvm-ir-and-code-generation).

For Rust compatibility, the parser also accepts top-level `use` declarations and lifetime syntax. You may [discard both after parsing](grammar.md#syntax-that-may-be-discarded-after-parsing); import resolution and lifetime checking are not part of the assignment.

This book defines the syntax and semantics of Rx rigorously, and it, alongside the official Rust Compiler, serves as the source truth for this course. If you find any ambiguities or disagreements between the two, do not hesitate to reach out to your TA for clarification.

## How to read the specification

Start by reading [language scope and test guarantees](undefined-behavior.md), which distinguish valid programs, static errors, and excluded cases i.e. *course UB*. Get an idea of what features are required, and which are not.

Then jump into the [lexical structure](lexical-structure.md), as well as [items](items.md), [types](types.md), and [expressions](expressions.md). These sections define the complete grammar of the Rx language. There is a quick lookup table in the [grammar summary](grammar-summary.md) section.

While reading, we recommend doing experiments with the rust compiler. You may also do experiments on any online Rust compiler like [this](https://play.rust-lang.org/?version=stable&mode=debug&edition=2024) or [this](https://godbolt.org/) to see what compiles or what not. Note that we **do not treat compiler warnings as static errors**.

## How to get started

Before you start, we recommend taking a look into any beginner tutorial for Rust to get started. You do not need to dive deep since we are implementing a subset, but some basic familiarity with the Rust syntax is definitely recommended.

There are lots of good resources online that teaches you each step of the compiler. Some blogs from your TAs (or their TAs) include:

- [Compiler for Mx* 编译器随笔](https://darksharpness.top/CompilerMx/) by legendary TA [@darksharpness](https://darksharpness.top/). This is a classic blog that focuses much on various optimization techniques in Mx (the old course language before Rx).
- [Writing a Compiler for Rust](https://norb.space/blog/writing-a-compiler-for-rust) by RayZh, who is technically not a TA for this course but who participated actively in the design of this year's specification. This blog focuses on the overall pipeline more. Note that some techniques are no longer applicable or no longer needed due to the changes in the spec this year.

Additional information:

- [Compiler 101](https://github.com/ACMClassCourses/Compiler-101), work by various ACM students over the years. Much of the tricks are Mx only and may not be applicable to Rx though.
- [The Rust Reference](https://doc.rust-lang.org/reference/) by the Rust team, which is a very detailed reference on the Rust language. Theoretically this spec will be enough, but if you want to read more about the Rust language, this is a good place to go.
- [Rust Compiler Development Guide](https://rustc-dev-guide.rust-lang.org/) by the Rust team, which is a very detailed guide on how to develop the Rust compiler. It includes valuable information on the compiler architecture, but is rather advanced. Use it for reference.
