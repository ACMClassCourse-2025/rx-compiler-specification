# Array and array index expressions

## Array expressions

r[expr.array.syntax]
```grammar,expressions
ArrayExpression -> `[` ArrayElements? `]`

ArrayElements ->
      Expression (`,` Expression)* `,`?
    | Expression `;` ConstValue
```

An array expression lists its elements or repeats one element a [ConstValue] number of times. List elements follow the [ordinary evaluation order](../expressions.md#evaluation-order). Repetition evaluates its element expression once and repeats the resulting value; more than one repetition requires Copy, rather than merely Clone. Its length follows the [constant-context rules](../const_eval.md#typing-and-range). Empty arrays and zero lengths are parsed but their data uses follow the [zero-sized-data rules](../undefined-behavior.md#zero-sized-data).

An expected array type supplies its element type to each listed or repeated element expression through [one-to-one coercions](../types.md#coercion-sites-and-expected-types). Without one, [LUB coercion](../types.md#least-upper-bound-coercions) finds the common element type.

## Array index expressions

r[expr.array.index.syntax]
```grammar,expressions
IndexExpression -> Expression `[` Expression `]`
```

An index has expected type `usize`, so an unsuffixed literal index is inferred as `usize`. An already typed index must have type `usize`. Indexing repeatedly dereferences references and `Box` until it reaches the first array or `Vec`, using the builtin steps under [coercion types](../types.md#coercion-types). Shared access yields a shared element place; mutable access yields a mutable one. Crossing a shared reference prevents mutable access. An immutable binding holding `&mut T` still allows mutable access to `T`; a `Box` preserves its access. The base is evaluated once, and a value base follows the [temporary-place rules](../expressions.md#places-and-values).

Array indexing must stay within its fixed length in valid executions. Runtime bounds checks and panic handling are not required. Reading an element follows [copy/move semantics](../builtin-traits.md#copy), including the test exclusion for non-Copy moves through an array index. Vec access and removal follow the [Vec operation rules](../heap.md#vec-operations).

```rust,ignore
let mut data: [i32; 4] = [0; 4];
data[2] = 7;
printlnInt(data[2]);
```

## Array methods

A fixed array's builtin `.len()` has receiver `&self` and returns its length as usize. Arrays passed by value retain the [value semantics](../builtin-traits.md#copy) of the entire array; source arguments do not convert to pointers.
