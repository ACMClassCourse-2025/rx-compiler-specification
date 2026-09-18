# Rust Reference grammar provenance

The active book uses the Rust Reference's per-chapter Lexer/Syntax grammar blocks,
production names, cross-linked railroad diagrams, and generated grammar summary.
The consolidated custom grammar introduced in e111476 has been replaced by the
Reference's chapter structure, trimmed to the confirmed course subset.

## Sources

- Upstream: [rust-lang/reference](https://github.com/rust-lang/reference/tree/4245282b97496585e73862b966d639cbb553bd6c),
  commit `4245282b97496585e73862b966d639cbb553bd6c` (2026-09-15).
- The original Reference-derived files in this repository are also retained in
  [the initial revision](https://github.com/ACMClassCourse-2025/RCompiler-Spec-Draft/tree/e6765c9).
  The notation page and integer radix productions come from that version,
  compatible with the bundled mdbook-spec grammar parser.
- Visual behavior was compared against the published
  [Functions](https://doc.rust-lang.org/reference/items/functions.html) page.
  The existing Reference renderer, railroad library, CSS, and JavaScript are
  reused; no separate diagram format or image generator is introduced.

The current upstream grammar adds cut and lookahead notation beyond this
repository's bundled grammar parser. Where needed for comments and numeric
boundaries, the same restrictions are written explicitly in prose. This does
not require implementing those operators in a course parser generator.

## Adaptations

| Reference chapters | Retained structure | Course modification |
| --- | --- | --- |
| Notation; identifiers | Lexer boxes, uppercase names, character classes | ASCII character classes; no raw identifiers; `_` alone is not a binding |
| Comments | COMMENT, LINE_COMMENT, nested BLOCK_COMMENT, BLOCK_CHAR | Documentation spellings are ordinary comments; delimiter lookahead is explicit |
| Tokens | Token, INTEGER_LITERAL, radix/digit productions, LIFETIME_TOKEN, LIFETIME_OR_LABEL, PUNCTUATION | Integer value tokens with four suffixes and ASCII lifetime names; numeric and lifetime-token boundaries are stated in prose |
| Crates and items | Crate, Item, Function, FunctionParameters, StructStruct, ConstantItem, InherentImpl, AssociatedItem | Named-field structs, functions with bodies, and inherent impls; lifetime parameters and where clauses occupy their Reference positions |
| Lifetime parameters and bounds | GenericParams, GenericParam, LifetimeParam, Lifetime, LifetimeBounds, WhereClause and its items | GenericParam is LifetimeParam; type bounds contain lifetime bounds; validity follows Rust and incorrect lifetime use is course UB |
| Statements | Statement, LetStatement, ExpressionStatement | Required let initializer; IdentifierBinding replaces patterns; no item statement or let-else |
| Expressions | ExpressionWithoutBlock / ExpressionWithBlock and original expression chapters | Supported expression alternatives; unit TupleExpression; container constructors use ordinary PathExpression and CallExpression |
| Operators | BorrowExpression, DereferenceExpression, NegationExpression, arithmetic, comparison, cast, assignment | Remove raw borrows and try propagation; keep course semantics for typing and order |
| Control flow | LoopExpression, InfiniteLoopExpression, PredicateLoopExpression, Conditions, break/continue/return | No labels, patterns, let chains, iterator loop, or match |
| Types and paths | TypeNoBounds, ParenthesizedType, TypePath, TypePathSegment, PathExprSegment, GenericArgs, ReferenceType, ArrayType, empty TupleType | GenericArg is Lifetime or Type; references accept a lifetime; type paths accept optional `::` before arguments, expression paths use turbofish; Box/Vec require one concrete type argument |
| Array lengths and constants | ArrayExpression / ArrayElements, ArrayType, ConstantItem | ConstValue replaces arbitrary constant expressions |
| Attributes | OuterAttribute | Finite DeriveAttribute for the four builtin capabilities |

The existing semantic chapters remain authoritative for the agreed test domain,
ownership, local inference, equality, ZST exclusions, and program-end heap
reclamation. Restoring a Reference production name does not restore its deleted
alternatives. TupleType and TupleExpression contain only `()`.

Each production is defined once in its relevant chapter. The grammar summary is
generated from those definitions. src/grammar.md now contains parser conventions
and contextual disambiguation, with no independent replacement grammar.

Container syntax uses the same path and call productions as other names.
TypePathSegment accepts optional `::` before GenericArgs; PathExprSegment uses
the expression turbofish. GenericArg retains Lifetime and Type. Type-argument
arity and the finite set of supported entities are checked by name resolution;
lifetime validity is guaranteed by tests. User items declare lifetime parameters,
while the builtin Box/Vec types supply the concrete type-argument interfaces.
StructExpression likewise reuses PathInExpression, and InherentImpl reuses Type
with a semantic requirement for a user-defined named-field struct.

The lifetime parameter and where-clause productions come from the Reference's
items/generics.md and trait-bounds.md chapters. They are collected in the active
items/generics.md chapter with the lifetime-only subset of parameter and bound
forms. ReferenceType and ShorthandSelf include their optional Lifetime, and
Function, StructStruct, and InherentImpl include GenericParams and WhereClause.
The syntax diagrams use the same renderer as the other Reference productions.

## Rule locations

Overview chapters provide production categories, navigation, and rules shared
by their children. Detailed rules have one defining chapter; other chapters
link to it. The generated grammar summary is a view of those same productions.

| Rules | Defining source |
| --- | --- |
| Function signatures and receivers | src/items/functions.md |
| Struct declarations | src/items/structs.md |
| Constants and inherent impls | src/items/constant-items.md, src/items/implementations.md |
| Token spelling and boundaries | src/tokens.md and the other lexical subchapters |
| Integer literal typing and range | src/expressions/literal-expr.md |
| Ordinary evaluation order and places | src/expressions.md |
| Arrays, calls, blocks, and other expression forms | Their src/expressions/ chapters |
| Method lookup | src/expressions/method-call-expr.md |
| Cast parsing and assignment destinations | src/expressions/operator-expr.md |
| Statement boundaries | src/statements.md |
| Contextual punctuation | src/grammar.md |
| Container type composition | src/types/heap.md |
| Container operations and reclamation | src/heap.md |
| Derive capabilities and equality operand rules | src/builtin-traits.md |
| Namespaces and protected names | src/names.md |
| Path syntax and resolution | src/paths.md |
| Source I/O and entry | src/undefined-behavior/builtin.md |
| Machine ABI, runtime interfaces, and resource limits | src/backend.md |
| Reference runtime implementation | src/runtime-example.c, included by the backend chapter |

The [language-scope chapter](../src/undefined-behavior.md#test-guarantees) indexes
and defines the course UB details for literal range, name collisions, equality,
lifetime validity, and zero-sized data. The corresponding language chapters
mention each UB category and link to its centralized definition.

## Verification

Build with SPEC_RELATIVE=0 and SPEC_DENY_WARNINGS=1. Validate the active chapter
sources and all generated local links, including SVG xlink targets. In a browser,
check show/hide, cross-page diagram links, persisted visibility, and narrow
viewports. This validates documentation rendering; supplied-parser and backend
integration remain the separate work recorded in src/open-decisions.md.
