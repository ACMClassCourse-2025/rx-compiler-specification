r[lex.whitespace]
# Whitespace

```grammar,lexer
@root WHITESPACE -> ( SPACE | TAB | LF | CR LF )+
```

The supported separators are space, horizontal tab, LF, and CRLF. This is the course's ASCII whitespace subset; Rust's other Unicode whitespace characters are not required.

Whitespace separates tokens and otherwise has no semantic significance. Replacing a separator by another supported separator preserves the token sequence. [Comments](comments.md) also act as separators. Whitespace cannot be inserted inside an identifier, an integer token, or the two characters of a comment delimiter.
