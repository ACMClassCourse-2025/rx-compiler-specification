# Crates and source files

```grammar,items
@root Crate -> Item*
```

One course program is one source compilation unit. This trims Rust's crate production by removing inner attributes; modules, use declarations, separate crates, and external crate loading are unsupported.

The top-level [Item] alternatives are functions, named-field structs, constants, and inherent impls. Items cannot be declared in expression blocks. Program entry follows [the main contract](undefined-behavior/builtin.md#program-entry).
