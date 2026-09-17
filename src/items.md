# Items and functions

A program is a single source compilation unit containing top-level functions, named-field structs, constants, and inherent impl blocks. Items cannot appear inside expression blocks, including a local const, struct, function, or impl. Methods and associated constants can appear in an inherent impl as described below.

## Functions

```rust,ignore
fn add(a: i32, b: i32) -> i32 {
    a + b
}
```

Ordinary parameters are identifiers with an optional `mut`, followed by an explicit type. Parameter types and the return type are not inferred across functions. An omitted return annotation means `()`. Every function definition has a body; const functions and foreign declarations are not course features.

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

Structs are nominal types with explicitly typed, named fields. Tuple structs are unsupported, and empty structs are excluded by the zero-sized-data rules. Fields are accessible without visibility modifiers. Struct names and field names must resolve consistently; construction must provide each declared field exactly once with a compatible type.

Within a struct definition, `Self` denotes the struct being defined. For example, `struct Node { children: Vec<Self> }` has the same field type as the version using `Vec<Node>`. This does not relax the prohibition on infinite inline layouts.

A constructor uses `Point { x: 1, y: 2 }`; field initializers may appear in any order and are evaluated in the order written. Field shorthand and functional-update syntax are not added by this revision. Field access is `p.x`. Layout is implementation-defined within the [backend contract](backend.md).

The only supported outer attribute is the finite [derive](builtin-traits.md#derive). User attributes and macros are not supported.

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

An inherent impl belongs to a named struct. It may contain functions/methods and associated constants, but not associated type definitions or nested type declarations. `Self` denotes the implementing type. There is no `impl Trait for Type`.

A method's first parameter may be `self`, `mut self`, `&self`, or `&mut self`. Other parameters use the ordinary identifier-and-type syntax. These receiver forms mean respectively a by-value receiver, mutable by-value receiver, shared reference, and mutable reference. Typed-self syntax is not supported. A function without a receiver is an associated function.

Multiple inherent impl blocks are supported. They share one associated-item namespace per struct: duplicate names are static errors across all such blocks, even if signatures differ. There is no signature overloading. Builtin names and namespaces follow [Names](names.md).

## Constants

A top-level or associated constant has an explicit type and a [restricted constant initializer](const_eval.md). Constants are used by value; they do not acquire a mutable storage location. There are no local const items or user static items.

```rust,ignore
const LIMIT: i32 = 100;
const NEGATIVE: i32 = (-1);
```

Constant names can be resolved independently of declaration order, including top-level constants and associated constants reached through the supported paths. An ordinary expression may use a constant declared later in the source. This is name lookup, not an extension of constant evaluation: const-to-const initializers and named array lengths remain excluded by the restricted-constant rules.
