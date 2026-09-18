r[lex.keywords]
# Keywords

The Rust 2021 strict and reserved keywords cannot be ordinary identifiers, including keywords for features removed from the course language. The lists are string-table productions as described in [Notation](notation.md#string-table-productions).

r[lex.keywords.strict]
## Strict keywords

`as`, `break`, `const`, `continue`, `crate`, `else`, `enum`, `extern`, `false`, `fn`, `for`, `if`, `impl`, `in`, `let`, `loop`, `match`, `mod`, `move`, `mut`, `pub`, `ref`, `return`, `self`, `Self`, `static`, `struct`, `super`, `trait`, `true`, `type`, `unsafe`, `use`, `where`, `while`, `async`, `await`, `dyn`.

r[lex.keywords.reserved]
## Reserved keywords

`abstract`, `become`, `box`, `do`, `final`, `macro`, `override`, `priv`, `typeof`, `unsized`, `virtual`, `yield`, `try`.

## Contextual names

Rust's weak keywords are not globally reserved as identifiers here. For example, `union` and `macro_rules` can be ordinary identifiers because their special syntactic constructs are absent. `gen` is not reserved in Rust 2021. Builtin type/trait names and I/O names instead follow the [protected namespace rules](names.md#protected-builtin-names).
