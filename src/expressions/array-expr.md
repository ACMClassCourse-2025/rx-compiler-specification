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

An expected array type supplies its element type to each element expression. Without one, elements follow [results without an expected type](../types.md#results-without-an-expected-type).

## Array index expressions

r[expr.array.index.syntax]
```grammar,expressions
IndexExpression -> Expression `[` Expression `]`
```

An index has expected type `usize`. Arrays and `Vec` support indexing after builtin [autoderef](../types.md#autoderef-and-autoref). Shared access yields a shared element place; mutable access yields a mutable one. A value base follows the [temporary-place rules](../expressions.md#places-and-values).

Array indexing must stay within its fixed length in valid executions. Runtime bounds checks and panic handling are not required. Reading an element follows [copy/move semantics](../builtin-traits.md#copy), including the test exclusion for non-Copy moves through an array index. Vec access and removal follow the [Vec operation rules](../heap.md#vec-operations).

```rust,ignore
let mut data: [i32; 4] = [0; 4];
data[2] = 7;
printlnInt(data[2]);
```

## Array methods

A fixed array's builtin `.len()` has receiver `&self` and returns its length as usize. Arrays passed by value retain the [value semantics](../builtin-traits.md#copy) of the entire array; source arguments do not convert to pointers.
