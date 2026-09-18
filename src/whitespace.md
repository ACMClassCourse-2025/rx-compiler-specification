r[lex.whitespace]
# Whitespace

```grammar,lexer
@root WHITESPACE -> ( SPACE | TAB | LF | CR LF )+
```

Whitespace separators are space, horizontal tab, LF, and CRLF.

Whitespace separates tokens and otherwise has no semantic significance. Replacing a separator by another supported separator preserves the token sequence. [Comments](comments.md) also act as separators. Whitespace cannot be inserted inside an identifier, an integer token, or the two characters of a comment delimiter.
