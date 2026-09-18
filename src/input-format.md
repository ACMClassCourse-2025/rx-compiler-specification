# Input format

A source compilation unit is a single file containing 7-bit ASCII source text. LF and CRLF line endings are supported. Unicode identifiers, byte-order marks, and interpreter directives are outside the course subset.

[Whitespace](whitespace.md) and [comments](comments.md) separate tokens. The parser consumes the resulting [Token] sequence according to [Crate].
