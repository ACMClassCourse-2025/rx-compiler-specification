# Structs

r[items.struct.syntax]
```grammar,items
Struct -> OuterAttribute* StructStruct

StructStruct -> `struct` IDENTIFIER GenericParams? WhereClause? `{` StructFields? `}`

StructFields -> StructField (`,` StructField)* `,`?

StructField -> IDENTIFIER `:` Type
```

A struct declaration gives a name, optional lifetime parameters and bounds, and a brace-delimited list of named fields. An optional trailing comma follows the last field. A struct may have [derive attributes](../builtin-traits.md#derive).

Lifetime parameters describe references stored in the struct, as in `struct View<'a> { value: &'a i32 }`. A type path supplies the corresponding arguments with `View<'a>` or uses Rust's permitted lifetime elision. See [lifetime parameters](generics.md) and [lifetime validity](../references.md#lifetime-validity).

Every field has an explicit type. `Self` is valid in its own struct definition. Empty braces are parsed, but zero-sized data uses remain excluded. See [struct semantics](../items.md#structs) and [recursive layouts](../types.md#recursive-types).
