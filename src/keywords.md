r[lex.keywords]
# Keywords

The strict and reserved keywords below are reserved across the language. The lists are string-table productions as described in [Notation](notation.md#string-table-productions).

r[lex.keywords.strict]
## Strict keywords

`as`, `break`, `const`, `continue`, `crate`, `else`, `enum`, `extern`, `false`, `fn`, `for`, `if`, `impl`, `in`, `let`, `loop`, `match`, `mod`, `move`, `mut`, `pub`, `ref`, `return`, `self`, `Self`, `static`, `struct`, `super`, `trait`, `true`, `type`, `unsafe`, `use`, `where`, `while`, `async`, `await`, `dyn`.

r[lex.keywords.reserved]
## Reserved keywords

`abstract`, `become`, `box`, `do`, `final`, `macro`, `override`, `priv`, `typeof`, `unsized`, `virtual`, `yield`, `try`.

## Contextual names

Other identifier spellings, including `union`, `macro_rules`, and `gen`, are available as ordinary identifiers. Builtin type/trait names and I/O names follow the [protected namespace rules](names.md#protected-builtin-names).
