# Use declarations

```grammar,items
UseDeclaration -> `use` UseTree `;`

UseTree ->
      (UsePath? `::`)? (`*` | `{` (UseTree (`,` UseTree)* `,`?)? `}`)
    | UsePath (`as` (IDENTIFIER | `_`))?

UsePath -> `::`? UsePathSegment (`::` UsePathSegment)*

UsePathSegment -> IDENTIFIER | `self` | `super` | `crate`
```

Use declarations are accepted as top-level items so that the same test source
can be compiled with Rust and the course support crate. For example:

```rust,ignore
use rx::core::*;

fn main() {
    printlnInt(42);
}
```

The syntax accepts individual paths, `*` globs, nested brace groups with optional
trailing commas, and `as` aliases. For example, `use rx::core::printlnInt;` and
`use rx::core::{getInt, printlnInt};` are also accepted. Use declarations cannot
appear in expression blocks or impls, and have no visibility modifier or
attributes.

After parsing, the compiler may discard the whole declaration. It does not
resolve the import path, load a crate or module, introduce names or aliases,
check import conflicts, or emit LLVM IR for the declaration. Builtins such as
`printlnInt` are already available in Rx, with or without a `use` declaration.

The [use compatibility guarantee](../undefined-behavior.md#use-compatibility)
ensures that tests do not depend on additional import semantics. The longer
paths, leading `::`, `crate`, and `super` accepted here do not extend the
[type and expression path rules](../paths.md). See also
[syntax that may be discarded after parsing](../grammar.md#syntax-that-may-be-discarded-after-parsing).
