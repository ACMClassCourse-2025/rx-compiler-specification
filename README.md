# Rx Compiler Specification

This repository contains the Rust subset used by the compiler course, targeting RV32IM. It is currently the **2026 working revision**, not a frozen assignment release.

Start with [Introduction](src/introduction.md), the [table of contents](src/SUMMARY.md), and [Open decisions](src/open-decisions.md). The core, explicit Box/Vec APIs, conditional container traits, finite name/path rules, reference adjustments, program-end heap reclamation, reference allocator ABI, and execution-memory baseline have been specified. Integer-literal overflow, let/parameter collisions with visible unqualified const names, and equality between different source types are course UB. Supplied-parser and end-to-end runtime integration still need validation. Method lookup follows Rust within the supported subset. Assessment and scoring are separate course arrangements. The [backend draft](src/backend.md) is included in the book. The [C runtime example](src/runtime-example.c) implements I/O and the reference `__rx_alloc` wrapper.

Only table-of-contents chapters define this revision. Other source pages are historical material from the previous specification or the upstream Rust Reference. Old tests do not independently define the language. See the [migration record](docs/2026-migration.md) for the scope of this documentation revision.

## Build

Use the Rust toolchain selected by rust-toolchain.toml and mdBook 0.4.x; CI currently pins mdBook 0.4.48. The repository's mdbook-spec preprocessor is built through Cargo.

```sh
cargo install --locked mdbook --version 0.4.48
SPEC_RELATIVE=0 SPEC_DENY_WARNINGS=1 mdbook build
```

The generated site is in book/. For local preview:

```sh
SPEC_RELATIVE=0 mdbook serve --open
```

SPEC_RELATIVE=0 makes upstream standard-library links absolute. SPEC_DENY_WARNINGS=1 treats preprocessor warnings as errors. The book grammar is checked and rendered by the existing preprocessor; its source format is described in [docs/grammar.md](docs/grammar.md).

## Examples and validation

Rust-labelled snippets describe the course language. They are marked rust,ignore because I/O builtins, erased lifetime annotations, excluded-syntax examples, and partial snippets are not standalone rustc programs. Running mdbook test is not a course-language conformance check.

Documentation validation includes the warning-denying book build, grammar/reference checks, local links and anchors in the rendered active chapters, and git diff --check. Language conformance and backend execution require the corresponding compiler and runtime test suites; this documentation migration does not modify or claim to validate those implementations.
