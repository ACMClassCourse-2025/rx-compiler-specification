# Publication validation

This working specification defines the language and execution contract for the
2026 course. Publication requires the following integration checks against the
selected frontend and execution environment.

## Remaining publication work

| Area | Validation | Reference |
| --- | --- | --- |
| Parser integration | Parse the documented type, item, expression, and statement forms, including nested containers, explicit turbofish, contextual punctuation, and condition/statement boundaries | [Grammar summary](grammar-summary.md), [Parser conventions](grammar.md) |
| Runtime integration | Execute Box/Vec construction, growth, non-Copy remove, nested and recursive Clone/equality, and per-execution heap reset with the published runtime and pinned REIMU commit | [Heap](heap.md), [Backend contract](backend.md) |

The reference allocator ABI, Vec growth baseline, memory/stack settings, and
cumulative heap test budget are specified in the
[backend contract](backend.md#42-参考-vec-策略与资源保证). Heap storage is reclaimed
at program end; source ownership and reference validity follow the
[heap rules](heap.md#program-end-reclamation).

## Specification status

The active table of contents defines the course language. Its chapters specify
identifier bindings, local type inference, literal constant contexts, value
semantics, four builtin derives, references, container types and operations,
and the I/O and program-entry contract. The Syntax and Lexer blocks define the
corresponding source forms. Documentation generation and grammar review check
these definitions; the integration checks above validate their implementations.

Course assessment, submissions, and scoring are recorded separately. The
[repository migration record](https://github.com/ACMClassCourse-2025/RCompiler-Spec-Draft/blob/master/docs/2026-migration.md)
tracks editorial history and verification results.
