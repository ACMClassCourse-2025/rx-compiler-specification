# Lexical structure

The productions in this chapter describe the course lexer. Select a production
name or use **Show syntax diagram** to switch between the textual grammar and
the railroad diagrams. The complete lexer and parser productions are also
collected in the [grammar summary](grammar-summary.md).

## Lexer grammar

```grammar,lexer
@root SourceFile -> (Whitespace | LineComment | BlockComment | Token)*

Whitespace -> (SPACE | TAB | LF | CR LF)+

LineComment -> `//` LineCommentCharacter*

LineCommentCharacter -> <a 7-bit ASCII source character except LF or CR>

BlockComment -> `/*` BlockCommentContent* `*/`

BlockCommentContent ->
      BlockComment
    | <a 7-bit ASCII source character except `*` or `/`>
    | `*` <a 7-bit ASCII source character except `/`>
    | `/` <a 7-bit ASCII source character except `*`>

Token -> Identifier | Keyword | IntegerLiteral | Punctuation

Identifier -> IdentifierOrKeyword _except `_` and Keyword_

IdentifierOrKeyword -> IdentifierStart IdentifierContinue*

IdentifierStart -> ASCII_ALPHA | `_`

IdentifierContinue -> ASCII_ALPHA | ASCII_DIGIT | `_`

ASCII_ALPHA -> [`a`-`z` `A`-`Z`]

ASCII_DIGIT -> [`0`-`9`]

Keyword ->
      `as` | `break` | `const` | `continue` | `crate` | `else` | `enum`
    | `extern` | `false` | `fn` | `for` | `if` | `impl` | `in` | `let`
    | `loop` | `match` | `mod` | `move` | `mut` | `pub` | `ref` | `return`
    | `self` | `Self` | `static` | `struct` | `super` | `trait` | `true`
    | `type` | `unsafe` | `use` | `where` | `while`
    | `async` | `await` | `dyn`
    | `abstract` | `become` | `box` | `do` | `final` | `macro` | `override`
    | `priv` | `typeof` | `unsized` | `virtual` | `yield` | `try`

BooleanLiteral -> `true` | `false`

IntegerLiteral ->
    (DecimalLiteral | BinaryLiteral | OctalLiteral | HexLiteral) IntegerSuffix?

DecimalLiteral -> DEC_DIGIT (DEC_DIGIT | `_`)*

BinaryLiteral -> `0b` (BIN_DIGIT | `_`)* BIN_DIGIT (BIN_DIGIT | `_`)*

OctalLiteral -> `0o` (OCT_DIGIT | `_`)* OCT_DIGIT (OCT_DIGIT | `_`)*

HexLiteral -> `0x` (HEX_DIGIT | `_`)* HEX_DIGIT (HEX_DIGIT | `_`)*

IntegerSuffix -> `i32` | `u32` | `isize` | `usize`

BIN_DIGIT -> [`0`-`1`]

OCT_DIGIT -> [`0`-`7`]

DEC_DIGIT -> [`0`-`9`]

HEX_DIGIT -> [`0`-`9` `a`-`f` `A`-`F`]

Punctuation ->
      `=` | `<` | `<=` | `==` | `!=` | `>=` | `>`
    | `&&` | `||` | `!`
    | `+` | `-` | `*` | `/` | `%` | `^` | `&` | `|` | `<<` | `>>`
    | `+=` | `-=` | `*=` | `/=` | `%=` | `^=` | `&=` | `|=` | `<<=` | `>>=`
    | `.` | `,` | `;` | `:` | `::` | `->` | `#` | `_`
    | `{` | `}` | `[` | `]` | `(` | `)`

SPACE -> U+0020

TAB -> U+0009

LF -> U+000A

CR -> U+000D
```

## Source and identifiers

The existing course restriction to 7-bit ASCII source is retained. Identifiers begin with an ASCII letter or underscore and continue with ASCII letters, digits, or underscores, except that the single token `_` is not an identifier. Names are case-sensitive and have no fixed length limit. Names such as `_value`, `_1`, and `__` are ordinary identifiers; supporting them does not add wildcard bindings or underscore assignees. Raw identifiers with an `r#` prefix are outside this subset.

Spaces, horizontal tabs, LF, and CRLF separate tokens. Rust-style `//` line comments and nestable `/* ... */` block comments are accepted. Comments do not create declarations or attributes. The supplied frontend's Rust lexical handling may be reused; recognition of additional tokens does not add language constructs.

Tokenization uses Rust's longest-token rule, except for the parser-context splits
specified below. Comments are recognized before treating `/` as punctuation.
The `Keyword` production contains all Rust 2021 strict and reserved keywords;
removed language constructs therefore remain unavailable as identifiers. The
lexer may recognize unsupported punctuation or literal forms, but doing so does
not add them to `Token` above or to the course language.

Strict and reserved Rust 2021 keywords cannot be ordinary identifiers. Keywords of removed constructs remain reserved. `self` denotes a method receiver; `Self` has its struct-definition and impl meanings. `Copy`, `Clone`, `PartialEq`, `Eq`, `Box`, and `Vec` are builtin names, not new lexical keywords. The prohibition on shadowing builtins is a namespace-level rule in [Names](names.md).

## Literals

The only value literal families are integers and the booleans `true` and `false`. Integer literals may be decimal, binary (`0b`), octal (`0o`), or hexadecimal (`0x`) and may use Rust-style underscore separators. A suffix, if present, is one of `i32`, `u32`, `isize`, and `usize`; it fixes the type. An absent suffix is resolved by [type inference](types.md#inference).

The integer lexer accepts any finite number of digits, with no fixed token-length or numeric-magnitude limit. It preserves the digits and suffix rather than requiring the token's value to fit a host or target integer. This follows Rust's separation of lexical recognition from numeric range handling; it does not introduce arbitrary-precision runtime integers. Literal spelling and supported-suffix restrictions still apply.

A leading minus is an operator, not part of the integer token. `-2147483648i32` consists of `-` and an integer token with magnitude 2147483648 and suffix i32. The unary-minus literal case, including parenthesized magnitude `-(2147483648i32)`, represents the valid signed minimum. No negative-literal token is needed.

After suffix resolution and type inference, an integer literal outside its determined type's range is course UB, not a required static error. The signed-minimum forms above are within range. For example, positive `2147483648i32` and `4294967296u32` are course UB. Tests exclude these cases everywhere, including const contexts and unreachable code; no overflow diagnostic or wrapped result is required. This exclusion does not affect the defined wrapping behavior of ordinary integer arithmetic or relax ordinary type errors.

Character, string, raw-string, byte-string, C-string, and floating-point literal expressions are not supported. Unit is written `()` and is restricted by the zero-sized-data rules.

## Punctuation

Operators and delimiters used in the [core grammar](grammar.md) retain their Rust spelling and contextual token interpretation. A lexer may emit one `&&` token. In infix position it is lazy boolean and; in prefix borrow position `&&x` means `&(&x)`, and in type position `&&T` means `&(&T)`. Whitespace between the two ampersands is not required. This does not create a unary boolean-and operator or permit reference operands for lazy boolean and.

Likewise, a parser consumes one or more leading `>` characters from combined punctuation when closing type arguments, leaving the remaining punctuation for the enclosing syntax. This covers `>>` as two type closers, `>=` as a type closer followed by assignment, and `>>=` as two type closers followed by assignment. Thus `Vec<Vec<i32>>` and `let v: Vec<Vec<i32>>=Vec::<Vec<i32>>::new();` need no separating whitespace. In ordinary expression context, `>>`, `>=`, and `>>=` retain their operator meanings. Constructors require `Box::<T>::new(...)` and `Vec::<T>::new()`; the turbofish makes the start of expression-level type arguments explicit.

Rust's cast parsing restriction is retained. After `expression as Type`, a following `<` or `<<` is parsed as the beginning of type/generic syntax rather than a comparison or shift and is rejected when that syntax is invalid. Parenthesize the cast result to use those operators: `(x as usize) < y` and `(x as usize) << y`. This is a parser decision based on syntactic context; it is not postponed until name resolution or semantic type checking. Other operators such as `<=`, `>`, `>>`, and `==` do not have this particular cast ambiguity.

`#`, `[`, `]`, and attribute tokens support the finite Rust-form derive attributes. Other attributes and user macros remain unsupported.
