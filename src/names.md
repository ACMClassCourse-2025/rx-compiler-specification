# Names and scopes

## Scope rules

Top-level functions, structs, constants, and inherent associated items introduce names. Parameters and `let` statements introduce local bindings. Fields belong to their struct and are found through the receiver type.

[Use declarations](items/use-declarations.md) introduce no names in Rx and do not participate in name resolution or collision checks. Imported paths and aliases may be discarded after parsing; the builtin environment is independent of these declarations.

Top-level and associated-item lookup is independent of declaration order. A local binding is visible only after its initializer and until the end of its block, subject to shadowing. Constant initializers, array lengths, and array-repeat lengths may use constant-item paths under the [constant-context rules](const_eval.md), which also require acyclic constant dependencies.

`self` denotes a method receiver. `Self` denotes the struct being declared or the type of the current inherent implementation, including inside its associated items. Neither is an ordinary user-defined identifier, and `Self` is unavailable in unrelated top-level functions.

Lifetime names occupy a separate namespace governed by [lifetime parameters](items/generics.md) and the [lifetime validity guarantee](undefined-behavior.md#lifetime-validity).

### Type and value namespaces

Types and values use separate namespaces:

| Context | Namespace |
| --- | --- |
| Struct name, primitive type, `Box`, `Vec`, or `Self` in a type position | Type |
| Function, constant, local binding, or `self` in an expression | Value |
| Prefix of `Type::member` | Type |
| Struct field | That struct's field namespace |

A named-field struct is not a callable value constructor. A struct and function may therefore share a spelling.

## Name collisions

Top-level names must be unique within their namespace. Fields within a struct and parameters within a function must also be unique. All inherent implementations of a struct share one associated-value namespace, so duplicate methods, associated functions, or associated constants are compile errors regardless of signature or impl block. A field and method may share a name.

Successive `let` bindings may shadow earlier locals, parameters, and non-protected global functions. Lookup chooses the nearest binding; if it is not callable, a call is a compile error rather than a retry with a hidden function. A `let` initializer sees the environment before the new binding.

A `let` or ordinary parameter that collides with a visible unqualified constant is undefined behavior under the centralized [constant-name collision guarantee](undefined-behavior.md#constant-name-collisions).

## Protected builtin names

User declarations and bindings cannot replace these names in their protected namespace:

| Namespace | Protected names |
| --- | --- |
| Type | `i32`, `u32`, `isize`, `usize`, `bool`, `Box`, `Vec`, `Copy`, `Clone`, `PartialEq`, `Eq` |
| Value | `get_i32`, `print_i32`, `println_i32` |
| Derive entry | `Copy`, `Clone`, `PartialEq`, `Eq` |

The spellings remain lexical identifiers. They may be used in another namespace or as field and associated-item names; for example, a field named `Vec` is valid. Inherent methods belong only to user-defined structs. Redeclaring a protected builtin in its namespace is a compile error.
