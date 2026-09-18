# Box and Vec types

Box and Vec use [TypePath] with [GenericArgs]. Their syntax follows the
[type-path rules](../paths.md#paths-in-types); name resolution supplies the
container type and its single concrete type argument.

## Box types

`Box<T>` is an owning container for one value of type T. A Box has a
fixed-size representation referring to separately allocated storage for its
contents. Its source operations are described in [Box access and
moves](../heap.md#box-access-and-moves).

## Vec types

`Vec<T>` is an owning sequence of T values with a runtime length. The type
argument determines the element type. The sequence's elements occupy contiguous heap
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
| `Vec<&i32>` or `Vec<&'a i32>` | A sequence of shared references |
| `Vec<View<'a>>` | A sequence of structs with a lifetime argument |
| `&mut Vec<Box<i32>>` | A mutable reference to a vector of boxes |
| `[Vec<i32>; 2]` | A two-element array of vectors |
| `Vec<Vec<i32,>,>` | Nested vectors with trailing type-argument commas |

```rust,ignore
struct Storage {
    values: Vec<Box<i32>>,
    counts: Box<[i32; 4]>,
}

fn append(values: &mut Vec<Box<i32>>, value: Box<i32>) {
    values.push(value);
}
```

The [recursive-type rules](../types.md#recursive-types) define how container indirection permits finite recursive layouts.
Each example's element type is specified by its type syntax; lifetime arguments
within that type follow the [lifetime elision rules](../references.md#lifetime-validity).
Local binding annotations may be inferred from a constructor or other uses in
the function.

Container values are created by the [constructors](../heap.md#constructors-and-type-arguments).
Path separators, turbofish, and trailing argument commas follow
[Paths](../paths.md); combined closing brackets follow
[contextual punctuation](../grammar.md#contextual-punctuation).
