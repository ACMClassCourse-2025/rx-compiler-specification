# Array types

```grammar,types
ArrayType -> `[` Type `;` ConstValue `]`
```

The Reference array-type production is narrowed to a [ConstValue] length. The element type can be any supported concrete type. The length has type usize, and type identity uses its resulting literal value.

Zero lengths are excluded from assessed data uses. Named lengths, const arithmetic, inferred lengths, and slices are unsupported. See [types and inference](../types.md) and [constant contexts](../const_eval.md).
