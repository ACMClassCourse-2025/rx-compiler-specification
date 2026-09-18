# Crates and source files

```grammar,items
@root Crate -> Item*
```

One program is one source compilation unit containing a sequence of top-level items. Its functions, structs, constants, and inherent impls share the namespaces described in [Names](names.md).

The top-level [Item] alternatives are use declarations, functions, named-field structs, constants, and inherent impls. [Use declarations](items/use-declarations.md) serve Rust compatibility and may be discarded after parsing; they do not add names to Rx. Items cannot be declared in expression blocks. Program entry follows [the main contract](undefined-behavior/builtin.md#program-entry).
