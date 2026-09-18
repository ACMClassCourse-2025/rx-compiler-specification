# Method-call expressions

r[expr.method.syntax]
```grammar,expressions
MethodCallExpression -> Expression `.` PathExprSegment `(` CallParams? `)`
```

A method call gives a receiver expression, a method identifier, and a parenthesized argument list. An optional trailing comma follows the last argument.

[PathExprSegment] supplies the shared path-segment syntax. Methods use their identifier with optional lifetime arguments where Rust permits them. [Path resolution](../paths.md#path-resolution) specifies these arguments and the lifetime validity guarantee. Type arguments on a method segment are static errors.

## Method lookup

Method calls resolve using the following candidate order:

1. Start with the receiver expression's type and repeatedly apply supported dereferencing, recording each type in order.
2. Immediately after each recorded type T, insert &T and &mut T as candidate receiver types.
3. Visit candidates in that order. At each candidate, first search inherent methods whose receiver matches it, then available builtin trait methods whose receiver matches it. Select the first matching method at the first successful priority level; ambiguity at that level is a static error.
4. Check that the selected call satisfies the ordinary argument and receiver-mutability rules. An invalid call is not retried with a later candidate. Ownership and lifetime validity remain guaranteed by tests rather than checked by the compiler.

The priority between inherent and trait methods applies within each candidate, not across the whole list. For a receiver of type S, an available builtin clone with receiver &S can therefore be selected before an inherent clone with receiver &mut S. If both have receiver &S, the inherent method takes priority. Expected return types do not select a different method.

<details>
<summary>Clone lookup examples</summary>

For an ordinary derived-Clone struct S with no inherent clone, calling `r.clone()` on `r: &S` selects S's clone and returns S. If S is not Clone, the shared reference itself still has Clone; the method search can instead select the reference clone with receiver &&S. This distinction follows from the same candidate sequence.

</details>

The builtin Clone method participates for types with that capability. Copy and Eq are marker capabilities. Builtin array methods participate under their specified receiver signatures.

References and Box provide builtin dereferencing candidates. Vec provides its specified methods and indexing. See [Builtin traits](../builtin-traits.md#clone) and [Heap](../heap.md) for the supported operations.
