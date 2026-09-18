r[comments]
# Comments

r[comments.syntax]
```grammar,lexer
@root COMMENT -> LINE_COMMENT | BLOCK_COMMENT

LINE_COMMENT -> `//` ~LF*

BLOCK_COMMENT -> `/*` ( BLOCK_COMMENT | BLOCK_CHAR )* `*/`

BLOCK_CHAR -> CHAR _not at the start of `/*` or `*/`_
```

Comments have line (`//`) and nestable block (`/* ... */`) forms and act as whitespace. A line comment ends before LF or at end of file; a CR in CRLF is part of the ignored comment text. An empty line comment at end of file is valid.

At each position in a block comment, `/*` opens a nested comment and `*/` closes the current one. Otherwise, one character is consumed as [BLOCK_CHAR]. Delimiters take priority over ordinary content, and each opening delimiter requires a matching closing delimiter. Unterminated comments are invalid.

Spellings such as `///`, `//!`, `/** ... */`, and `/*! ... */` are ordinary comments.

```rust,ignore
// A line comment.
/* outer /* inner */ outer again */
/***/
/* a trailing slash / */
fn main() {}
```
