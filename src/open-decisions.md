# Open decisions before publication

This working revision is ready for continued editing, but not for a frozen assignment release. Confirmed rules are in the other table-of-contents chapters. This page lists unresolved boundaries rather than additional implementation requirements. Tests must not depend on an unresolved choice.

## Remaining publication work

| Area | Remaining decision | Impact |
| --- | --- | --- |
| Parser integration | Check the supplied parser and examples against the settled grammar, including statement/condition boundaries, explicit turbofish and contextual token splitting | Verification work, not an undecided syntax extension |
| Runtime integration | Execute Box/Vec construction, growth, remove, recursive Clone/equality and per-execution heap reset with the published C runtime and pinned REIMU commit | The allocator ABI, reference growth strategy and resource budget are settled |

The previously listed language choices and runtime contract are settled. See [Heap](heap.md#program-end-reclamation) for the program-end reclamation policy: no per-value automatic cleanup or final object-graph traversal is required. The reference `__rx_alloc` wrapper, Vec growth baseline, fixed REIMU memory/stack settings and cumulative heap headroom are in the [backend contract](backend.md#42-参考-vec-策略与资源保证). Local backward type inference, literal-only constants, four builtin traits, integer behavior, and conservative stack storage are also settled. Box/Vec compose with supported concrete types, and recursive definitions through indirection are supported. Method candidate ordering follows Rust within the supported subset. Destructuring assignment, type-position `_`, and all explicit never-type annotations are excluded. These decisions are not being reopened.

The Rust comparison also settled Self within struct definitions, temporary places, constant forward lookup, ordinary underscore-prefixed identifiers without a fixed length limit, reference variants of primitive operators, and the boundary on backward inference at field/method lookup. All compound assignments use uniform right-first evaluation, including reference variants; tests do not depend on compound-assignment operand order. Those are specified in their respective chapters.

The 2026-09-18 decisions also settled explicit `Box::<T>::new(value)` / `Vec::<T>::new()` constructors, Box access/move-out, the minimal Vec operations, conditional container Clone/PartialEq/Eq, finite paths without use/modules, protected builtin names, cross-impl duplicates, and the finite reference coercion sites. Equality is assessed only when both operand source types are exactly identical; every cross-type equality expression is course UB. Rust-form derives can be split across attributes. Conditional struct-literal disambiguation, optional condition parentheses, contextual punctuation splitting, Rust cast parsing and signed-minimum literal handling are specified. While/loop bodies and jumps in while conditions follow Rust. Integer tokens have no fixed length or magnitude limit at lexing; overflow after type determination is course UB. Let/parameter collisions with visible unqualified const names are also course UB. All tests exclude these UB cases. Numeric casts remain restricted as previously specified.

## Migration status

The active book now replaces the previous core chapters with the confirmed 2026 rules: identifier bindings, no nested items/enum/match/user traits, restricted consts, local inference, Rust copy/move value semantics, finite derives, lifetime-erased references, and print-based program output. A backend draft and the selected heap direction are now included in the same book.

Detailed pages inherited from the previous specification and upstream Rust Reference remain outside the table of contents as historical material. They do not add supported features. Before release, review the consolidated core for omissions, complete the runtime and parser verification above, and audit examples against the resulting frozen version. Assessment and scoring arrangements are handled separately after testcase revision and do not belong to this specification. The current migration changes documentation only, not the reference compiler or testcase submodule.
