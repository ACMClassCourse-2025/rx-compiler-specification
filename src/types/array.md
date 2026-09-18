# Array types

```grammar,types
ArrayType -> `[` Type `;` ConstValue `]`
```

An array type specifies an element type and a [ConstValue] length. The element type can be any supported concrete type. Length typing follows [constant contexts](../const_eval.md#typing-and-range), and equality of array types follows [type identity](../types.md#supported-types).

Zero lengths are excluded from assessed data uses. See [types and inference](../types.md) and [constant contexts](../const_eval.md).
