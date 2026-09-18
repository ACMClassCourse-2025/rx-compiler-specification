# Array and array index expressions

## Array expressions

r[expr.array.syntax]
```grammar,expressions
ArrayExpression -> `[` ArrayElements? `]`

ArrayElements ->
      Expression (`,` Expression)* `,`?
    | Expression `;` ConstValue
```

The list and repetition branches come from the Reference. The repetition length is narrowed from an arbitrary expression to [ConstValue]. Lists evaluate elements in order; repetition evaluates the element expression once. More than one repetition requires Copy. Empty arrays and zero lengths are parsed but excluded by the zero-sized-data rules.

## Array index expressions

r[expr.array.index.syntax]
```grammar,expressions
IndexExpression -> Expression `[` Expression `]`
```

An index must have type usize. Arrays and Vec support indexing, including the builtin receiver adjustments in the [array rules](../expressions.md#arrays) and [heap rules](../heap.md). Writing requires a mutable element place. Tests guarantee bounds and ownership validity; no panic or bounds-check runtime is required.

```rust,ignore
let mut data: [i32; 4] = [0; 4];
data[2] = 7;
printlnInt(data[2]);
```
