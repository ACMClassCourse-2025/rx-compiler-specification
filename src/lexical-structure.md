# Lexical structure

Lexical productions are organized into [input format](input-format.md), [keywords](keywords.md), [identifiers](identifiers.md), [comments](comments.md), [whitespace](whitespace.md), and [tokens](tokens.md). Each chapter presents its rules in a Lexer box with syntax diagrams. See [Notation](notation.md) for how to read them.

## Source and identifiers

Source text uses 7-bit ASCII. Identifiers begin with an ASCII letter or underscore and continue with ASCII letters, digits, or underscores. The spellings `_value`, `_1`, and `__` are identifiers, while a single `_` is punctuation. Names are case-sensitive and have no fixed length limit.

Spaces, horizontal tabs, LF, and CRLF separate tokens. `//` line comments and nestable `/* ... */` block comments act as whitespace.

Tokenization selects the longest token, subject to the parser-context splits
specified below. Comments are recognized before treating `/` as punctuation.
The [keyword lists](keywords.md) define the strict and reserved words.

Strict and reserved keywords are reserved across the language. `self` denotes a method receiver; `Self` has its struct-definition and impl meanings. `Copy`, `Clone`, `PartialEq`, `Eq`, `Box`, and `Vec` are lexically identifiers with the protected builtin meanings specified in [Names](names.md).

## Literals

Value literals are integers and the booleans `true` and `false`. Integer literals may be decimal, binary (`0b`), octal (`0o`), or hexadecimal (`0x`) and may contain underscore separators. A suffix, if present, is one of `i32`, `u32`, `isize`, and `usize`; it fixes the type. An absent suffix is resolved by [type inference](types.md#inference).

The integer lexer accepts any finite number of digits and preserves the digits and suffix. Numeric range is determined after type inference, independently of lexical recognition. Runtime integer values have the widths specified in [Types](types.md).

A leading minus is a separate operator token. `-2147483648i32` consists of `-` and an integer token with magnitude 2147483648 and suffix i32. The unary-minus literal case, including parenthesized magnitude `-(2147483648i32)`, represents the valid signed minimum.

After suffix resolution and type inference, an integer literal outside its determined type's range is course UB, not a required static error. The signed-minimum forms above are within range. For example, positive `2147483648i32` and `4294967296u32` are course UB. Tests exclude these cases everywhere, including const contexts and unreachable code; no overflow diagnostic or wrapped result is required. This exclusion does not affect the defined wrapping behavior of ordinary integer arithmetic or relax ordinary type errors.

The unit expression is written `()` and follows the zero-sized-data rules.

## Punctuation

Operators and delimiters are listed in the [grammar](grammar-summary.md). A lexer may emit one `&&` token. In infix position it is lazy boolean and; in prefix borrow position `&&x` means `&(&x)`, and in type position `&&T` means `&(&T)`. These forms work with adjacent ampersands or with whitespace between separate borrow tokens.

Likewise, a parser consumes one or more leading `>` characters from combined punctuation when closing type arguments, leaving the remaining punctuation for the enclosing syntax. This covers `>>` as two type closers, `>=` as a type closer followed by assignment, and `>>=` as two type closers followed by assignment. Thus `Vec<Vec<i32>>` and `let v: Vec<Vec<i32>>=Vec::<Vec<i32>>::new();` need no separating whitespace. In ordinary expression context, `>>`, `>=`, and `>>=` retain their operator meanings. Constructors require `Box::<T>::new(...)` and `Vec::<T>::new()`; the turbofish makes the start of expression-level type arguments explicit.

After a type-path segment in a cast, `<` or the leading `<` of `<<` starts type-argument parsing. Parenthesizing the cast result makes comparison and shift explicit: `(x as usize) < y` and `(x as usize) << y`. A parenthesized type also closes the type syntax, so `x as (usize) < y` is a comparison. The [cast parsing rules](grammar.md#casts-and-conditions) distinguish syntax parsing from subsequent type-argument checks. Operators such as `<=`, `>`, `>>`, and `==` can directly follow the cast.

`#`, `[`, `]`, and attribute tokens form the [derive attributes](builtin-traits.md#derive).
