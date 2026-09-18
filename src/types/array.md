# Array types

```grammar,types
ArrayType -> `[` Type `;` ConstValue `]`
```

An array type specifies an element type and a [ConstValue] length. The element type can be any supported concrete type. The length has type usize, and type identity uses its resulting literal value.

Zero lengths are excluded from assessed data uses. See [types and inference](../types.md) and [constant contexts](../const_eval.md).
