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

### Index operand types

An index has expected type `usize`, so an unsuffixed literal index is inferred as
`usize`. An already typed index must have type `usize`.

An index operand with never type, is
undefined behavior under the [test guarantees](../undefined-behavior.md#test-guarantees).
This applies to both arrays and `Vec`, including `data[return]` and
`data[loop {}]`.

### Indexed places

Indexing repeatedly dereferences references and `Box` until it reaches the first
array or `Vec`, using the builtin steps under
[coercion types](../types.md#coercion-types).

The base is the expression before `[index]`. Evaluate it exactly once. If it
produces a value, such as an array returned by a function call, materialize that
value in temporary storage when needed to access its element. The temporary's
mutability and lifetime follow the [temporary-place rules](../expressions.md#temporary-places).

Array indexing projects an element place without inserting a borrow of the
array. Its mutability follows the [place rules](../expressions.md#places-and-values).
Vec indexing follows the separate [mutable-access rule and examples](../heap.md#indexing-and-mutable-access).

Array indexing must stay within its fixed length in valid executions. Runtime bounds checks and panic handling are not required. Reading an element follows [copy/move semantics](../builtin-traits.md#copy), including the test exclusion for non-Copy moves through an array index. Vec access and removal follow the [Vec operation rules](../heap.md#vec-operations).

```rust,ignore
let mut data: [i32; 4] = [0; 4];
data[2] = 7;
println_i32(data[2]);
```

## Array methods

A fixed array's builtin `.len()` has receiver `&self` and returns its length as usize. Arrays passed by value retain the [value semantics](../builtin-traits.md#copy) of the entire array; source arguments do not convert to pointers.
