# Names and scopes

## Settled scope rules

Top-level functions, structs, constants, and inherent associated items introduce names. Parameters and identifier lets introduce local bindings. A let binding is visible after its initializer through the rest of its block, subject to shadowing. Fields are looked up on the receiver's struct type rather than in the local variable scope.

Top-level function, struct, and constant names can be resolved independently of textual definition order. Associated item lookup through the supported paths likewise does not require an earlier textual declaration. Local let bindings still become visible only after their initializer. Constant forward lookup does not permit names in restricted constant initializers or array lengths.

`self` denotes a method receiver. `Self` denotes the type being defined inside a struct definition, and the implementing type inside an inherent impl, including its methods and associated constants. `Self` is not available in unrelated top-level functions. Neither `self` nor `Self` is an ordinary user-declared identifier.

Lifetime names have a separate namespace with the scopes specified by [lifetime parameters](items/generics.md). Their validity is covered by the [lifetime test guarantee](undefined-behavior.md#lifetime-validity).

Types and values occupy separate namespaces. Named-field structs introduce a type name; functions and constants introduce value names. Struct field names are local to their struct. Type annotations, struct construction names, and the prefix of Type::member resolve in the type namespace; an unqualified expression name resolves in the value namespace. A named-field struct name is not a callable value constructor. A struct and a function can therefore have the same spelling without conflict.

Top-level items must be unique within each namespace. Fields must have distinct names within a struct, and parameters within a function must have distinct names. Successive local lets may shadow earlier locals, parameters, and non-protected global function names. Lookup selects the nearest binding; calling a selected non-callable value is a static error, without retrying a hidden function. A let initializer uses the environment before that new binding.

A let or ordinary parameter binding that collides with a visible unqualified const name is course UB; see the centralized [constant-name collision guarantee](undefined-behavior.md#constant-name-collisions).

All inherent impl blocks for one struct share one associated-value namespace. Duplicate method, associated-function, or associated-constant names are static errors, including across different impl blocks and regardless of signatures. There is no signature overloading. A field and a method may have the same name because field lookup and method lookup are distinct.

## Protected builtin names

User declarations and bindings cannot replace a builtin in its protected namespace:

| Namespace | Protected names |
| --- | --- |
| Type | i32, u32, isize, usize, bool, Box, Vec, Copy, Clone, PartialEq, Eq |
| Value | getInt, printInt, printlnInt |
| Derive attribute entries | Copy, Clone, PartialEq, Eq always denote the supported builtin derives |

These names are lexically identifiers whose protection is checked in the stated namespace. A spelling may be used in another namespace, or as a field or associated item name. For example, a field named Vec is permitted. An inherent method named clone participates in the ordinary method candidate order. Inherent impls belong to user-defined named-field structs.

Redeclaring a protected builtin in its namespace is a static error. Ordinary local shadowing, type/value separation, and cross-impl duplicate checks follow the rules above.
