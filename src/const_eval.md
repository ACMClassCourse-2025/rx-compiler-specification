# Constant contexts

Constant contexts are const-item initializers, array type lengths, and array-repeat lengths. This revision does not require a general const evaluator.

## Allowed forms

```grammar,constants
ConstValue -> INTEGER_LITERAL | `true` | `false` | `-` Magnitude | `(` ConstValue `)`

Magnitude -> INTEGER_LITERAL | `(` Magnitude `)`
```

Magnitude means integer syntax without a leading minus, not an unsigned target type. Negative forms require a signed integer type. These forms permit `123`, `-1`, `(-1)`, `-(1)`, `((true))`, and valid suffixes and radices.

They do not permit names or paths (including earlier constant names), arithmetic, boolean operations, casts, calls, blocks, borrows, or array/struct construction. `--1`, `!false`, and `1 + 2` are outside this subset.

```rust,ignore
const COUNT: usize = 64;
const SENTINEL: i32 = (-1);
const ENABLED: bool = (true);

fn main() {
    let data: [i32; 64] = [0; 64];
    let n: usize = COUNT; // ordinary expressions may read a constant
}
```

`const OTHER: usize = COUNT`, `[i32; COUNT]`, and `[0; COUNT]` are unsupported const forms. This restriction is intentional; array lengths must be repeated literally.

## Typing and range

Const-item types are explicit. Array lengths have type `usize` and must fit that type; a bool, signed suffix, or negative value cannot serve as a length. Zero lengths are excluded by the zero-sized-data rules and do not require a diagnostic.

A literal outside its determined type's range is course UB, including in const initializers and array lengths; tests exclude it and no range diagnostic is required. The signed minimum-value syntax is supported. An out-of-range literal does not acquire a defined wrapped value as if it were a runtime arithmetic operation. Ordinary type and restricted-form errors remain static errors.

The element expression in `[expr; 4]` is an ordinary expression, not a constant context. It is evaluated once; repetition with a length greater than one requires Copy. There is no const-block or aggregate-constant exception for non-Copy elements.

Constant propagation and folding of ordinary runtime expressions remain permitted optimizations. They do not turn those expressions into const contexts or change their arithmetic semantics.
