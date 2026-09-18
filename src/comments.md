r[comments]
# Comments

r[comments.syntax]
```grammar,lexer
@root COMMENT -> LINE_COMMENT | BLOCK_COMMENT

LINE_COMMENT -> `//` ~LF*

BLOCK_COMMENT -> `/*` ( BLOCK_COMMENT | BLOCK_CHAR )* `*/`

BLOCK_CHAR -> CHAR _not at the start of `/*` or `*/`_
```

Comments follow Rust's line (`//`) and nestable block (`/* ... */`) forms and act as whitespace. A line comment ends before LF or at end of file; a CR in CRLF is part of the ignored comment text. An empty line comment at end of file is valid.

At each position in a block comment, `/*` opens a nested comment and `*/` closes the current one; only otherwise can one character be consumed as [BLOCK_CHAR]. This is the Reference's nested-comment structure with the delimiter lookahead written as a prose restriction. A comment cannot skip a closing delimiter or consume half of it as ordinary content. Unterminated comments are invalid.

The course does not interpret documentation comments as attributes. Spellings such as `///`, `//!`, `/** ... */`, and `/*! ... */` remain ordinary comments.

```rust,ignore
// A line comment.
/* outer /* inner */ outer again */
/***/
/* a trailing slash / */
fn main() {}
```
