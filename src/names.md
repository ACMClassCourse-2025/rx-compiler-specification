# Names and paths

## Settled scope rules

Top-level functions, structs, constants, and inherent associated items introduce names. Parameters and identifier lets introduce local bindings. A let binding is visible after its initializer through the rest of its block, subject to shadowing. Fields are looked up on the receiver's struct type rather than in the local variable scope.

Top-level function, struct, and constant names can be resolved independently of textual definition order. Associated item lookup through the supported paths likewise does not require an earlier textual declaration. Local let bindings still become visible only after their initializer. Constant forward lookup does not permit names in restricted constant initializers or array lengths.

`self` denotes a method receiver. `Self` denotes the type being defined inside a struct definition, and the implementing type inside an inherent impl, including its methods and associated constants. `Self` is not available in unrelated top-level functions. Neither `self` nor `Self` is an ordinary user-declared identifier.

Types and values occupy separate namespaces. Named-field structs introduce a type name; functions and constants introduce value names. Struct field names are local to their struct. Type annotations, struct construction names, and the prefix of Type::member resolve in the type namespace; an unqualified expression name resolves in the value namespace. A named-field struct name is not a callable value constructor. A struct and a function can therefore have the same spelling without conflict.

Top-level items must be unique within each namespace. Fields must have distinct names within a struct, and parameters within a function must have distinct names. Successive local lets may shadow earlier locals, parameters, and non-protected global function names. Lookup selects the nearest binding; calling a selected non-callable value is a static error, without retrying a hidden function. A let initializer uses the environment before that new binding.

A let or ordinary parameter binding with the same name as a const visible as an unqualified value name at that position is course UB. This applies to both mutable and immutable bindings, regardless of the const's textual definition order. Tests contain no such collision, including in unreachable code, and no diagnostic or constant-pattern interpretation is required. For example, with a visible `const LIMIT: i32 = 1;`, both `let LIMIT = 2;` and a parameter `LIMIT: i32` are excluded. An associated constant accessible only as Type::LIMIT does not by itself exclude a local LIMIT. The static prohibition on shadowing protected builtins below still applies.

All inherent impl blocks for one struct share one associated-value namespace. Duplicate method, associated-function, or associated-constant names are static errors, including across different impl blocks and regardless of signatures. There is no signature overloading. A field and a method may have the same name because field lookup and method lookup are distinct.

## Supported paths

The supported forms are unqualified names, `self`, `Type::member`, and `Self::member`, plus the two explicit heap constructors `Box::<T>::new(value)` and `Vec::<T>::new()`. Associated functions and constants are accessed through a type; ordinary methods can also be invoked with method-call syntax. Type::method may be called with its receiver as an explicit first argument and ordinary argument coercions, without method-call autoref. For example:

```rust,ignore
struct Counter { value: i32 }

impl Counter {
    const LIMIT: i32 = 10;
    fn new() -> Self { Self { value: 0 } }
    fn read(&self) -> i32 { self.value }
}

fn main() {
    let counter: Counter = Counter::new();
    printlnInt(counter.read());
    printlnInt(Counter::LIMIT);
}
```

There is no use, mod, crate/super path, leading ::, longer module path, trait-qualified path, or generic user item. `Box::new` and `Vec::new` are unsupported; their concrete type argument must appear in the turbofish. No other generic expression path is added. The only callable builtin trait surface is `.clone()`; equality uses `==` and `!=`. `Clone::clone`, `<T as Clone>::clone`, and builtin trait `.eq()` / `.ne()` calls are outside this subset. User-defined inherent methods named eq or ne remain ordinary methods.

## Protected builtin names

User declarations and bindings cannot replace a builtin in its protected namespace:

| Namespace | Protected names |
| --- | --- |
| Type | i32, u32, isize, usize, bool, Box, Vec, Copy, Clone, PartialEq, Eq |
| Value | getInt, printInt, printlnInt |
| Derive attribute entries | Copy, Clone, PartialEq, Eq always denote the supported builtin derives |

This is a static naming rule, not new lexical keywords. The namespace distinction still applies: using a spelling in another namespace, or as a field or associated item name, does not shadow the unqualified builtin. For example, a field named Vec is permitted. An inherent method named clone is permitted and participates in the ordinary method candidate order. Users cannot add inherent impl blocks to the builtin primitive or container types; inherent impls belong to user-defined named-field structs.

Rust permits shadowing prelude types and traits and primitive type names in many such positions. This course deliberately forbids those collisions. Ordinary local shadowing, type/value separation, and cross-impl duplicate rejection otherwise retain the rules above.

## Method lookup

Method calls use the candidate ordering in the [Rust Reference](https://doc.rust-lang.org/reference/expressions/method-call-expr.html#method-call-expressions), restricted to the types and operations supported by this language. There is no separate course-specific method priority.

1. Start with the receiver expression's type and repeatedly apply supported dereferencing, recording each type in order.
2. Immediately after each recorded type T, insert &T and &mut T as candidate receiver types.
3. Visit candidates in that order. At each candidate, first search inherent methods whose receiver matches it, then available builtin trait methods whose receiver matches it. Select the first matching method at the first successful priority level; ambiguity at that level is a static error.
4. Check that the selected call satisfies the ordinary argument and receiver-mutability rules. An invalid call is not retried with a later candidate. Ownership and lifetime validity remain guaranteed by tests rather than checked by the compiler.

The priority between inherent and trait methods applies within each candidate, not across the whole list. For a receiver of type S, an available builtin clone with receiver &S can therefore be selected before an inherent clone with receiver &mut S. If both have receiver &S, the inherent method takes priority. Expected return types do not select a different method.

For an ordinary derived-Clone struct S with no inherent clone, calling `r.clone()` on `r: &S` selects S's clone and returns S. If S is not Clone, the shared reference itself still has Clone; the method search can instead select the reference clone with receiver &&S. This distinction follows from the same candidate sequence.

Only specified builtin trait operations participate; the currently exposed Clone method is always available for types with that capability. Copy and Eq add no callable methods. Providing the four builtin traits does not import additional standard-library methods. Builtin array methods are available under their specified receiver signatures.

There are no user Deref implementations, generic bounds, trait objects, slices, or array-to-slice unsizing. Those branches of the full Rust procedure are absent. References and Box provide builtin dereferencing candidates; Vec provides its specified methods and indexing without a slice candidate. See [Builtin traits](builtin-traits.md#clone) and [Heap](heap.md) for the supported operations.
