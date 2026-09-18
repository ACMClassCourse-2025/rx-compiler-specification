# Structs

r[items.struct.syntax]
```grammar,items
Struct -> OuterAttribute* StructStruct

StructStruct -> `struct` IDENTIFIER `{` StructFields? `}`

StructFields -> StructField (`,` StructField)* `,`?

StructField -> IDENTIFIER `:` Type
```

Only the named-field branch of Rust's struct grammar remains. There are no tuple structs, unit structs written with a semicolon, generic parameters, where clauses, visibility modifiers, or field attributes. The outer attributes are the finite [derive form](../builtin-traits.md#derive).

Every field has an explicit type. `Self` is valid in its own struct definition. Empty braces are parsed, but zero-sized data uses remain excluded. See [struct semantics](../items.md#structs) and [recursive layouts](../types.md#recursive-types).
