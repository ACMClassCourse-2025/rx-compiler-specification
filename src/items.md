# Items and functions

```grammar,items
Item -> Function | Struct | ConstantItem | Implementation
```

A program is a single source compilation unit containing top-level functions, named-field structs, constants, and inherent impl blocks. An inherent impl contains methods, associated functions, and associated constants as described below. Expression blocks contain [statements and expressions](statements.md).

## Functions

```rust,ignore
fn add(a: i32, b: i32) -> i32 {
    a + b
}
```

Ordinary parameters are identifiers with an optional `mut`, followed by an explicit type. Function signatures determine parameter and result types independently of their callers. An omitted return annotation means `()`. Every function definition has a body.

Top-level functions may be called before their definitions and may be mutually recursive. Calls evaluate arguments in source order. Arguments and results obey [copy/move value semantics](builtin-traits.md) independently of their machine-level passing convention. Function names may be call targets but are not first-class values.

The entry function and its runtime behavior are specified in [Program entry](undefined-behavior/builtin.md#program-entry).

## Structs

```rust,ignore
#[derive(Clone, Copy, PartialEq, Eq)]
struct Point {
    x: i32,
    y: i32,
}
```

Structs are nominal types with explicitly typed, named fields. Their fields are accessible throughout the source compilation unit. Struct names and field names must resolve consistently; construction must provide each declared field exactly once with a compatible type. Empty structs are covered by the zero-sized-data exclusions.

Within a struct definition, `Self` denotes the struct being defined. For example, `struct Node { children: Vec<Self> }` has the same field type as the version using `Vec<Node>`. All struct definitions must have [finite layouts](types.md#recursive-types).

A constructor uses `Point { x: 1, y: 2 }`; each initializer gives an explicit field name and expression. Field initializers may appear in any order and are evaluated in the order written. Field access is `p.x`. Layout is implementation-defined within the [backend contract](backend.md).

Struct declarations may carry [derive attributes](builtin-traits.md#derive) for Copy, Clone, PartialEq, and Eq.

## Inherent impls and receivers

```rust,ignore
impl Point {
    const ZERO: i32 = 0;

    fn total(&self) -> i32 {
        self.x + self.y
    }

    fn translate(&mut self, amount: i32) {
        self.x += amount;
        self.y += amount;
    }
}
```

An inherent impl belongs to a named struct and contains functions, methods, and associated constants. `Self` denotes the implementing type.

A method's first parameter is `self`, `mut self`, `&self`, or `&mut self`. Other parameters use the ordinary identifier-and-type syntax. These receiver forms mean respectively a by-value receiver, mutable by-value receiver, shared reference, and mutable reference. A function without a receiver is an associated function.

Multiple inherent impl blocks are supported. They share one associated-item namespace per struct: duplicate names are static errors across all such blocks, even if signatures differ. There is no signature overloading. Builtin names and namespaces follow [Names](names.md).

## Constants

A top-level or associated constant has an explicit type and a [restricted constant initializer](const_eval.md). Each use of a constant produces its value.

```rust,ignore
const LIMIT: i32 = 100;
const NEGATIVE: i32 = (-1);
```

Constant names can be resolved independently of declaration order, including top-level constants and associated constants reached through the supported paths. An ordinary expression may use a constant declared later in the source. Constant initializers and array lengths use the literal forms specified in [Constant contexts](const_eval.md).
