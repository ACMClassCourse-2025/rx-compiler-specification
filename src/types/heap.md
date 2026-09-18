# Box and Vec types

Box and Vec use [TypePath] with [GenericArgs]. Their syntax follows the
[type-path rules](../paths.md#paths-in-types); name resolution supplies the
container type and its single concrete type argument.

## Box types

`Box<T>` is an owning container for one value of type T. Its [GenericArgs] must contain
one explicitly written [Type], with an optional trailing comma. A Box has a
fixed-size representation referring to separately allocated storage for its
contents. Its source operations are described in [Box access and
moves](../heap.md#box-access-and-moves).

## Vec types

`Vec<T>` is an owning sequence of T values with a runtime length. The type
argument determines the element type. The sequence's elements occupy separate
storage, while the Vec value has a fixed-size representation. Its operations
are specified in [Vec operations](../heap.md#vec-operations).

## Type arguments and composition

The type argument may be any supported concrete type, including another
container, an array, a reference, or a named struct. Each occurrence of [Type]
in the grammar admits Box and Vec types: local annotations, parameters,
function results, struct fields, array elements, reference targets, and nested
container arguments. The [zero-sized-data rules](../undefined-behavior.md#zero-sized-data)
govern the assessed element and referent types.

| Type spelling | Meaning |
| --- | --- |
| `Box<i32>` or `Box::<i32>` | One owned integer |
| `Vec<u32>` | A sequence of unsigned integers |
| `Box<[i32; 4]>` | One owned four-element array |
| `Vec<Box<i32>>` | A sequence of owned integer boxes |
| `Box<Vec<i32>>` | One owned vector |
| `Vec<&i32>` | A sequence of shared references |
| `&mut Vec<Box<i32>>` | A mutable reference to a vector of boxes |
| `[Vec<i32>; 2]` | A two-element array of vectors |
| `Vec<Vec<i32,>,>` | Nested vectors with trailing type-argument commas |

```rust,ignore
struct Node {
    value: i32,
    children: Vec<Self>,
}

struct Storage {
    nodes: Vec<Box<Node>>,
    counts: Box<[i32; 4]>,
}

fn wrap(value: Node) -> Box<Node> {
    Box::<Node>::new(value)
}

fn append(nodes: &mut Vec<Box<Node>>, value: Box<Node>) {
    nodes.push(value);
}
```

The container indirection supports recursive definitions such as Node above;
the [recursive-type rules](../types.md#recursive-types) determine finite layout.
Each example's T is fully specified by its type syntax. Local binding
annotations may be inferred from a constructor or other uses in the function.

## Constructors

Type paths accept `<T>` or `::<T>` after the name. Constructor expressions use
`::<T>` and then `::new` with the value-argument list:

| Type | Constructor expression | Initial value |
| --- | --- | --- |
| `Box<T>` | `Box::<T>::new(value)` | One owned value of T |
| `Vec<T>` | `Vec::<T>::new()` | An empty sequence of T |

Both constructors are [CallExpression] forms whose callee is a
[PathExpression] with type arguments. Box new takes one expression and an
optional trailing comma; Vec new takes an empty value-argument list. The
constructor's result participates in ordinary field access, indexing, method
calls, borrowing, and dereferencing according to its type.

Nested closing brackets follow the [contextual token rules](../grammar.md#contextual-punctuation).
For example, `let values: Vec<Vec<i32>>=Vec::<Vec<i32>>::new();` closes the nested
types before parsing the assignment, with no space required before `=`.
