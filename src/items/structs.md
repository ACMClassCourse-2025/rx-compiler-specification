# Structs

r[items.struct.syntax]
```grammar,items
Struct -> OuterAttribute* StructStruct

StructStruct -> `struct` IDENTIFIER `{` StructFields? `}`

StructFields -> StructField (`,` StructField)* `,`?

StructField -> IDENTIFIER `:` Type
```

A struct declaration gives a name and a brace-delimited list of named fields. An optional trailing comma follows the last field. A struct may have [derive attributes](../builtin-traits.md#derive).

Every field has an explicit type. `Self` is valid in its own struct definition. Empty braces are parsed, but zero-sized data uses remain excluded. See [struct semantics](../items.md#structs) and [recursive layouts](../types.md#recursive-types).
