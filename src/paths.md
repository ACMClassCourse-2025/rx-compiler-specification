# Paths

A path names a value, type, or associated item through segments separated by
`::`. A segment consists of a name and, where applicable, type arguments.
Type paths and expression paths share the same [GenericArgs] production.

## Paths in expressions

```grammar,paths
PathInExpression -> PathExprSegment (`::` PathExprSegment)*

PathExprSegment -> PathIdentSegment (`::` GenericArgs)?

PathIdentSegment -> IDENTIFIER | `self` | `Self`
```

In an expression, `::` introduces type arguments. It distinguishes an argument
list from comparison operators. `Box::<i32>::new` consists of a Box segment
with the argument i32 followed by the member segment new.

Unqualified names and `self` resolve in the value namespace. A prefix such as
`Point` or `Self` in `Point::new` or `Self::LIMIT` resolves in the type namespace;
the final segment names an associated function, method, or constant.

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

Type names resolve to primitive types, declared structs, or the builtin
containers. `Self` denotes the surrounding struct in its declaration and impls.
A Box or Vec segment has exactly one explicit concrete type argument. Primitive
and user-defined struct type paths use their declared names directly.

## Generic arguments

```grammar,paths
GenericArgs -> `<` GenericArgList? `>`

GenericArgList -> (GenericArg `,`)* GenericArg `,`?

GenericArg -> Type
```

A generic argument is a [Type]. The list grammar permits comma-separated
arguments and an optional trailing comma. Name resolution and the named item's
signature determine the required arguments. In this language, the builtin
Box and Vec types each require exactly one explicit argument; `Box<>` and
`Vec<i32, u32>` are static arity errors even though the generic list itself has
a syntax tree.

A type argument can recursively contain arrays, references, named structs,
parentheses, unit, and further generic type paths. The type and data-use rules
apply to the resulting type. See [Box and Vec types](types/heap.md) for examples.

| Context | Example |
| --- | --- |
| Type annotation | `let values: Vec<i32> = Vec::<i32>::new();` |
| Optional separator in a type path | `let value: Box::<i32> = Box::<i32>::new(7);` |
| Nested type arguments | `Vec<Box<[i32; 4]>>` |
| Trailing type-argument comma | `Box::<i32,>::new(7,)` |
| Constructor with a nested element type | `Vec::<Box<i32>>::new()` |

## Path resolution

The path grammar records segment names and type arguments. The
[namespaces](names.md#settled-scope-rules) then resolve these segments to the
following supported entities:

| Path role | Resolved form |
| --- | --- |
| Ordinary value | A binding, constant, function name, or `self` |
| Ordinary type | A primitive name, a declared struct name, or `Self` |
| Container type | Box or Vec with one concrete type argument |
| Associated item | A struct name or `Self`, followed by a declared member |
| Builtin constructor | `Box::<T>::new` or `Vec::<T>::new` |

The type-argument list belongs to the Box or Vec segment. Ordinary function,
method, and member segments use their identifier without a type-argument list.
Unresolved segments and type-argument lists on those ordinary segments are
static errors. A constructor call must also match its argument signature:
Box new takes one value of T, and Vec new takes an empty value-argument list.

The parser can form a path tree before these checks. This separates the shared
path syntax from the finite set of names and callable signatures supplied by
the language.
