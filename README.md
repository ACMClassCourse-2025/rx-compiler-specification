# Rx Compiler Specification

Read the [specification online](https://acmclasscourse-2025.github.io/rx-compiler-specification/).

This repository contains the 2026 working specification for the compiler course's Rust subset and RV32IM target.

The chapters in [`src/SUMMARY.md`](src/SUMMARY.md) are the complete normative specification. Start with the [introduction](src/introduction.md), [language scope](src/undefined-behavior.md), and [publication validation](src/open-decisions.md).

## Build and preview

The book uses mdBook 0.4.x (CI pins 0.4.48) and the bundled `mdbook-spec` preprocessor.

```sh
cargo install --locked mdbook --version 0.4.48
SPEC_RELATIVE=0 SPEC_DENY_WARNINGS=1 mdbook build
```

The generated site is written to `book/`. For a local preview:

```sh
SPEC_RELATIVE=0 mdbook serve --open
```

`SPEC_RELATIVE=0` makes standard-library links absolute. `SPEC_DENY_WARNINGS=1` turns preprocessor warnings into errors. Grammar source syntax is documented in [`docs/grammar.md`](docs/grammar.md).

## Validation

Rust-labelled examples describe the course language and are ignored by `rustc` because the subset has its own builtins and restrictions. A successful book build checks document structure, grammar references, and rendering; compiler and runtime conformance require their respective test suites.
