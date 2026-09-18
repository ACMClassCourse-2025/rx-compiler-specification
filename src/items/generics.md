# Lifetime parameters and bounds

## Parameter declarations

r[items.generics.syntax]
```grammar,items
GenericParams -> `<` (GenericParam (`,` GenericParam)* `,`?)? `>`

GenericParam -> LifetimeParam

LifetimeParam -> Lifetime (`:` LifetimeBounds)?
```

Functions, named-field structs, and inherent impls may declare lifetime
parameters. A function or struct places [GenericParams] after its name; an impl
places the list after `impl`. An optional trailing comma follows the last
parameter. Each parameter is a lifetime, optionally with outlives bounds.

```rust,ignore
struct View<'a> {
    value: &'a i32,
}

impl<'a> View<'a> {
    fn new(value: &'a i32) -> Self {
        Self { value: value }
    }

    fn get(&self) -> &'a i32 {
        self.value
    }
}

fn choose<'a>(left: &'a i32, right: &'a i32, first: bool) -> &'a i32 {
    if first { left } else { right }
}
```

Lifetime parameters are in scope throughout their item. An impl's lifetimes
also apply inside its associated items; a method may declare additional
lifetimes. Lifetime names occupy their own namespace. Incorrect lifetime
declarations, uses, or bounds are undefined behavior; see the
[lifetime validity guarantee](../undefined-behavior.md#lifetime-validity).

## Lifetime forms and bounds

```grammar,types
Lifetime -> LIFETIME_OR_LABEL | `'static` | `'_`

LifetimeBounds -> (Lifetime `+`)* Lifetime?

TypeParamBounds -> TypeParamBound (`+` TypeParamBound)* `+`?

TypeParamBound -> Lifetime
```

A named lifetime such as `'a` refers to an in-scope parameter. `'static` denotes
the lifetime covering the entire program. `'_` requests the lifetime inferred
under Rust's elision rules at that occurrence. Parameter declarations use
ordinary named lifetimes; `'static` and `'_` have their special meanings at use
sites.

The bound `'a: 'b` means that `'a` outlives `'b`. A list such as
`'a: 'b + 'c` states both bounds. A type bound such as `View<'a>: 'b` means that
references contained in the type remain valid for `'b`. The types in these
bounds are the concrete types described by [Type].

```rust,ignore
fn shorten<'long: 'short, 'short>(value: &'long i32) -> &'short i32 {
    value
}
```

## Where clauses

r[items.generics.where.syntax]
```grammar,items
WhereClause -> `where` (WhereClauseItem `,`)* WhereClauseItem?

WhereClauseItem -> LifetimeWhereClauseItem | TypeBoundWhereClauseItem

LifetimeWhereClauseItem -> Lifetime `:` LifetimeBounds

TypeBoundWhereClauseItem -> Type `:` TypeParamBounds?
```

A where clause states lifetime bounds after a function's signature, a struct's
name and lifetime parameters, or an impl's implementing type. Bounds are
comma-separated with an optional trailing comma. A lifetime bound may be
written here or directly on its parameter declaration.

```rust,ignore
fn shorten_where<'long, 'short>(value: &'long i32) -> &'short i32
where
    'long: 'short,
{
    value
}
```

These productions use Rust's parameter-list and bound syntax. [GenericParam]
defines lifetime parameters; [GenericArg] defines the lifetime and concrete
type arguments used in paths. The supported named types and callable
signatures are specified in [Paths](../paths.md).
