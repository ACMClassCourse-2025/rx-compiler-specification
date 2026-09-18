r[lex.token]
# Tokens

r[lex.token.syntax]
```grammar,lexer
@root Token ->
      IDENTIFIER_OR_KEYWORD
    | INTEGER_LITERAL
    | PUNCTUATION
```

These are the Rust Reference token alternatives retained by the course. Keywords, including `true` and `false`, have identifier-like spellings. Their syntactic roles are determined by the keyword rules and [LiteralExpression]. Character, string, byte, C-string, floating-point, lifetime, raw-identifier, and macro token forms do not add supported source constructs.

## Integer literals

r[lex.token.literal.int.syntax]
```grammar,lexer
INTEGER_LITERAL ->
    ( DEC_LITERAL | BIN_LITERAL | OCT_LITERAL | HEX_LITERAL ) INTEGER_SUFFIX?

DEC_LITERAL -> DEC_DIGIT (DEC_DIGIT|`_`)*

BIN_LITERAL -> `0b` (BIN_DIGIT|`_`)* BIN_DIGIT (BIN_DIGIT|`_`)*

OCT_LITERAL -> `0o` (OCT_DIGIT|`_`)* OCT_DIGIT (OCT_DIGIT|`_`)*

HEX_LITERAL -> `0x` (HEX_DIGIT|`_`)* HEX_DIGIT (HEX_DIGIT|`_`)*

BIN_DIGIT -> [`0`-`1`]

OCT_DIGIT -> [`0`-`7`]

DEC_DIGIT -> [`0`-`9`]

HEX_DIGIT -> [`0`-`9` `a`-`f` `A`-`F`]
```

```grammar,lexer
INTEGER_SUFFIX -> `i32` | `u32` | `isize` | `usize`
```

The radix and digit productions retain the original Reference grammar. The general Rust literal suffix is narrowed to the four supported integer types. Decimal digits and radix prefixes use Rust spelling, with underscore separators; a non-decimal literal must contain at least one digit after the prefix.

| Form | Examples |
| --- | --- |
| Decimal | `123`, `1_234`, `123_i32` |
| Binary | `0b1010`, `0b____1`, `0b10_u32` |
| Octal | `0o77`, `0o7_usize` |
| Hexadecimal | `0xff`, `0xAB_CD`, `0xff_isize` |

The lexer consumes the complete Rust-style numeric token. An invalid suffix or radix digit cannot be repaired by splitting the spelling into a valid integer followed by another token: `123i32foo`, `123bad`, `0b102`, and `0x` are invalid spellings. A supplied Rust lexer may retain a general suffix and reject unsupported suffixes at the language-subset boundary.

There is no token-length or numeric-magnitude limit. Preserve the digits and suffix without requiring the magnitude to fit a host integer. After type determination, out-of-range literals are course UB, as specified in [Lexical structure](lexical-structure.md#literals).

A minus is a separate token. `-2147483648i32` and `-(2147483648i32)` are valid signed-minimum forms. `0x01_f32` is a hexadecimal integer magnitude with no suffix, not a floating-point literal.

## Punctuation

r[lex.token.punct.syntax]
```grammar,lexer
PUNCTUATION ->
      `=` | `<` | `<=` | `==` | `!=` | `>=` | `>`
    | `&&` | `||` | `!`
    | `+` | `-` | `*` | `/` | `%` | `^` | `&` | `|` | `<<` | `>>`
    | `+=` | `-=` | `*=` | `/=` | `%=` | `^=` | `&=` | `|=` | `<<=` | `>>=`
    | `.` | `,` | `;` | `:` | `::` | `->` | `#` | `_`
    | `{` | `}` | `[` | `]` | `(` | `)`
```

Punctuation retains its Rust spelling. Comments take priority over `/` punctuation; combined operators are recognized before shorter prefixes. The parser may split `&&`, `>>`, `>=`, and `>>=` in the specified reference/type contexts. See [contextual token interpretation](lexical-structure.md#punctuation).

An integer is not followed by a supported fractional or exponent part. The absence of floating-point expressions does not make a malformed Rust numeric token valid. Brackets and braces remain paired delimiters; `#` is used only in the supported outer derive attribute.
