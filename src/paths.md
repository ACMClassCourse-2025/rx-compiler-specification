# Paths

A path names a value, type, or associated item through segments separated by
`::`. A segment consists of a name and, where applicable, lifetime or type arguments.
Type paths and expression paths share the same [GenericArgs] production.

## Paths in expressions

```grammar,paths
PathInExpression -> PathExprSegment (`::` PathExprSegment)*

PathExprSegment -> PathIdentSegment (`::` GenericArgs)?

PathIdentSegment -> IDENTIFIER | `self` | `Self`
```

In an expression, `::` introduces generic arguments. It distinguishes an argument
list from comparison operators. `Box::<i32>::new` consists of a Box segment
with the argument i32 followed by the member segment new.

Expression paths follow the [namespace rules](names.md#settled-scope-rules). The
final segment of `Point::new` or `Self::LIMIT` names an associated item.

[CallExpression] applies an argument list to the resulting path expression.
For example, `Box::<i32>::new(7)` and `Vec::<i32>::new()` are calls to the builtin
associated constructors. Method syntax uses [MethodCallExpression].

## Paths in types

```grammar,paths
TypePath -> TypePathSegment (`::` TypePathSegment)*

TypePathSegment -> PathIdentSegment (`::`? GenericArgs)?
```

A type segment may introduce its arguments directly with `<` or with `::<`.
`Box<i32>` and `Box::<i32>` therefore name the same type, as do `Vec<T>` and
`Vec::<T>` for a concrete T. The surrounding type context determines how the
angle brackets are parsed.

[Path resolution](#path-resolution) determines the named type. User-defined
struct paths may supply lifetime arguments, as in `View<'a>` or `View<'_>`,
or use the permitted lifetime elision.

## Generic arguments

```grammar,paths
GenericArgs -> `<` GenericArgList? `>`

GenericArgList -> (GenericArg `,`)* GenericArg `,`?

GenericArg -> Lifetime | Type
```

A generic argument is a [Lifetime] or a [Type]. The list grammar permits
comma-separated arguments and an optional trailing comma. Lifetime arguments
precede type arguments, following Rust's ordering. The named item's declaration
determines its lifetime arguments, and their correct use is part of the
[lifetime validity guarantee](undefined-behavior.md#lifetime-validity).

Box and Vec each take exactly one explicit concrete type argument. `Box<>` and
`Vec<i32, u32>` are static type-argument arity errors even though their generic
lists have syntax trees. User-defined structs and functions declare lifetime
parameters; their paths take lifetime arguments. Type arguments on those
items, or on primitive types, are static errors.

A type argument can recursively contain arrays, references, named structs,
parentheses, unit, and further generic type paths. The type and data-use rules
apply to the resulting type. See [Box and Vec types](types/heap.md) for examples.

| Context | Example |
| --- | --- |
| Struct lifetime argument | `View<'a>` or `View::<'a>` |
| Lifetime inside a container element | `Vec::<&'a i32>::new()` |
| Type annotation | `let values: Vec<i32> = Vec::<i32>::new();` |
| Optional separator in a type path | `let value: Box::<i32> = Box::<i32>::new(7);` |
| Nested type arguments | `Vec<Box<[i32; 4]>>` |
| Trailing type-argument comma | `Box::<i32,>::new(7,)` |
| Constructor with a nested element type | `Vec::<Box<i32>>::new()` |

## Path resolution

The path grammar records segment names and generic arguments. The
[namespaces](names.md#settled-scope-rules) then resolve these segments to the
following supported entities:

| Path role | Resolved form |
| --- | --- |
| Ordinary value | A binding, constant, function name, or `self` |
| Ordinary type | A primitive name, a declared struct with its lifetime arguments or permitted elision, or `Self` |
| Container type | Box or Vec with one concrete type argument |
| Associated item | A struct path such as `View::<'a>`, or `Self`, followed by a declared member |
| Builtin constructor | `Box::<T>::new` or `Vec::<T>::new` |

An ordinary function or method may use explicit lifetime arguments where Rust
permits them, for example `shorten::<'long, 'short>(value)` for a function with
those parameters and an outlives bound. Omitted lifetime arguments follow
Rust's inference and elision rules. Correct explicit lifetime arguments,
including Rust's distinction between early-bound and late-bound function
lifetimes, are guaranteed by tests. See [lifetime parameters](items/generics.md).

Lifetimes may also occur within a container's type argument, as in
`Box::<&'a i32>::new(value)` or `Vec::<View<'a>>::new()`.

Unresolved type, value, and member names are static errors. After resolution,
[call expressions](expressions/call-expr.md) check the callable's value-argument
signature, including the [container constructors](heap.md#constructors-and-type-arguments).
Incorrect lifetime arguments or elision are course UB under the
[lifetime validity guarantee](undefined-behavior.md#lifetime-validity).

The parser can form a path tree before these checks. This separates the shared
path syntax from the finite set of names and callable signatures supplied by
the language.

```rust,ignore
struct Counter { value: i32 }

impl Counter {
    const LIMIT: i32 = 10;
    fn new() -> Self { Self { value: 0 } }
    fn read(&self) -> i32 { self.value }
}

fn main() {
    let counter: Counter = Counter::new();
    printlnInt(counter.read());
    printlnInt(Counter::LIMIT);
}
```
