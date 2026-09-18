# Constant contexts

Constant contexts are const-item initializers, array type lengths, and array-repeat lengths. Each uses the literal forms defined below.

## Allowed forms

```grammar,constants
ConstValue -> INTEGER_LITERAL | `true` | `false` | `-` Magnitude | `(` ConstValue `)`

Magnitude -> INTEGER_LITERAL | `(` Magnitude `)`
```

Magnitude means integer syntax without a leading minus, not an unsigned target type. Negative forms require a signed integer type. These forms permit `123`, `-1`, `(-1)`, `-(1)`, `((true))`, and valid suffixes and radices.

Each constant context contains an integer or boolean literal, an optionally parenthesized negative integer, or parentheses around another allowed form. Any other expression is a static form error.

```rust,ignore
const COUNT: usize = 64;
const SENTINEL: i32 = (-1);
const ENABLED: bool = (true);

fn main() {
    let data: [i32; 64] = [0; 64];
    let n: usize = COUNT; // ordinary expressions may read a constant
}
```

Array lengths are written literally at each use. Constant names such as COUNT can be read in ordinary expressions, as shown above.

## Typing and range

Const-item types are explicit. Array lengths have type `usize` and must fit that type; a bool, signed suffix, or negative value cannot serve as a length. Zero lengths are excluded by the zero-sized-data rules and do not require a diagnostic.

Literal range and signed-minimum cases follow [integer literal rules](expressions/literal-expr.md#integer-typing-and-range). Ordinary type and restricted-form errors remain static errors.

The element expression in `[expr; 4]` is an ordinary expression governed by [array repetition](expressions/array-expr.md#array-expressions). Only its length is a constant context.

Constant propagation and folding of ordinary runtime expressions remain permitted optimizations. They do not turn those expressions into const contexts or change their arithmetic semantics.
