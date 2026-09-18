# 2026 working-revision migration

Date: 2026-09-17.

This file is a chronological migration log. Later entries supersede earlier pending decisions and validation counts; the latest resource/literal/name decisions are recorded at the end.

This is the first migration of confirmed course decisions into the original specification repository. It replaces the active core book with a consolidated course-specific description, retaining historical detailed pages outside the table of contents. Git history preserves the previous active wording.

## Active chapters

- Scope and test guarantees distinguish static errors, unsupported features, and excluded executions/data uses.
- Items and bindings remove nested items, user traits, enum, match, and patterns; lets use optional type annotations with local backward constraints.
- Types, expressions, operators, and control flow consolidate the selected subset and Rust-compatible never/block behavior.
- Constant contexts are restricted to integer/boolean literals, negative integer spellings, and parentheses.
- Builtin traits define Copy/Clone/PartialEq/Eq, finite derives, and source copy/move semantics.
- References define the lifetime-erased test domain and permit conservative stack storage without restricting valid optimization.
- Builtin I/O removes exit and specifies unit-returning source main with a normal machine status of zero.
- Heap records the selected Box/Vec direction and the still-open API/destruction contract.
- Backend incorporates the reviewed draft, including LLVM 22, RV32IM, private internal ABI, optional runtime, and the execution-cost metric.
- Core grammar and open decisions make the current coverage and pending boundaries explicit.

The discussion sources are the workspace spec-draft, semantics/copy-move/reference/heap/name-resolution memos, and backend-contract-draft. They are not needed to read the active book. The workspace documents retain discussion history; later decisions should be reflected in the active chapters and open-decisions page together.

## Remaining publication work

Complete the language decisions in src/open-decisions.md and consolidate the final parser-facing syntax. Assessment, submission protocols, and scoring are tracked in a separate course memo and will be revisited after testcase revision. Box/Vec selection is not a promise to implement the entire Rust standard library. No compiler implementation or testcase change is part of this migration.

Course-language examples are deliberately ignored by rustc doctests. The preprocessor now allows strict warning checks without a rust-lang/rust checkout, and the style checker accepts the bundled C runtime example. A successful documentation build checks the document structure and grammar references, not semantic completeness or executable compiler conformance.

## Local validation

- mdBook 0.4.52 built the active book with SPEC_RELATIVE=0 and SPEC_DENY_WARNINGS=1, including the grammar/reference preprocessor checks.
- The 18 active chapters plus generated index/print pages passed local file and anchor validation: 1,490 references, no broken targets.
- The active source pages, summary, and C example passed the style checker. The full historical src tree still reports pre-existing trailing spaces in archived pages; those pages were not reformatted by this migration.
- The existing mdbook-spec grammar parser test passed; Rust formatting and git diff --check passed.
- Clang 22.1.8 compiled the I/O example for RV32IM/ILP32 and successfully ran the documented relocatable link with a small assembly caller. This does not constitute a REIMU execution test or validation of the pending heap runtime.

## Follow-up decisions

Method candidate ordering follows the Rust Reference within the supported subset. Destructuring assignment and all explicit never-type annotations are excluded. Never behavior of diverging expressions is retained. Assessment/score proposals and release checklists were moved out of the active specification into the workspace assessment-followup memo. The Box/Vec API and reclamation policy remain undecided; proposed minimal APIs in the heap memo are not normative.

After this follow-up, the strict book build, active-source style check, and git diff --check passed. Local link validation covered 20 rendered pages and 1,486 references with no broken files or anchors. Runtime code was unchanged and its compilation was not repeated.

## Recursive container types

Box/Vec composition with supported concrete types, nested containers, and recursive/mutually recursive structs through indirection are now explicitly supported. They are no longer listed as optional heap features. The core grammar includes type-position Box<T>/Vec<T>. Inline struct/array layout cycles remain static errors; finite layout is distinct from the ability to construct a finite value. Container methods, trait capabilities, and reclamation are still separate decisions.

## Accepted Rust-comparison corrections

Self now also resolves inside struct definitions. Temporary places, forward constant lookup, and ASCII underscore-prefixed identifiers without a fixed length limit are explicit. Backward inference retains Rust's need for sufficient receiver/element information at field and method lookup. Primitive operators retain their standard reference variants. A subsequent course decision gives all compound assignments uniform right-first evaluation, including reference RHS forms, intentionally differing from Rust. Tests do not depend on compound-assignment operand order. General coercion/cast expansion is not inferred from this operator change.

## Revision completeness audit

The confirmed decisions in the workspace draft and design memos have corresponding rules in the active chapters. A follow-up audit corrected stale wording that still marked container element types as undecided, and updated memo status lines that incorrectly said the specification had not been migrated. Box/Vec composition and indirect recursion remain confirmed.

The complete coercion-site and reborrow-versus-move contract is now explicitly tracked in open-decisions.md; the existing basic reference rules are present, but the earlier Rust explanation did not complete that contract. Heap APIs, reclamation and conditional container traits, remaining name/path rules, and final parser/example consistency likewise remain unfinished. This audit adds no new container API, cast, or ownership-checking requirement.

The active table of contents defines the working language. Historical pages outside it, compiler/runtime implementation migration, and testcase revision are separate from incorporating decisions into the specification. The current runtime example still implements I/O only.

## 2026-09-18 interface and grammar decisions

Box/Vec constructors require explicit concrete turbofish arguments; omitted constructor type arguments and type-position `_` are unsupported. The minimal APIs and Box move-out, Vec remove, conditional Clone/PartialEq/Eq, recursive deep cloning of owned contents, and container reference validity are specified. Internal capacity and layout remain implementation choices; physical reclamation is still undecided.

Finite paths exclude use/modules and trait-qualified calls. Type/value namespaces, protected builtin names, local shadowing and cross-impl duplicates are defined. Identifier-only bindings can shadow user constants rather than becoming Rust constant patterns. The reference coercion sites, mutable-reference reborrows, result joins and builtin reference/Box deref coercions are explicit.

Grammar includes mandatory constructor turbofish, repeated Rust-form derive attributes, contextual &&/>> splitting and the established condition/statement disambiguation. The lexer preserves signed-minimum magnitudes for unary-minus type checking. The main remaining language choice is heap destruction/reclamation; parser integration and an allocator example remain release work. No compiler, testcase or REIMU implementation was changed by this documentation revision.

## 2026-09-18 resource policy and excluded constructs

The integer lexer now explicitly accepts arbitrarily long well-formed digit sequences without numeric-magnitude checks. Range overflow after suffix resolution and inference is course UB, including in const contexts and unreachable code, with no required diagnostic. Signed-minimum unary-minus literal forms remain valid; ordinary arithmetic wrapping and static type checking are unchanged.

Let and ordinary parameter bindings that collide with a visible unqualified const name are course UB, excluded from every test category. This replaces the earlier defined constant-shadowing behavior without requiring constant-pattern resolution or a collision diagnostic. Protected builtin names retain their static naming rule.

Heap storage is reclaimed together at program termination. No per-value automatic destruction, final object-graph traversal, object registry, or cleanup ownership flags are required. Runtime/execution-environment arena reclamation or memory reset is sufficient, and a bump allocator is permitted. Valid moves, Clone and reference validity remain mandatory. Correct earlier reclamation may be an optimization; tests must fit cumulative allocation without relying on it.

The active chapters, backend contract, draft and discussion-memo status are synchronized. The prior language choices are settled; the reference allocator, cumulative-allocation validation and supplied-parser integration remain publication work. No compiler, runtime implementation or testcase was changed in this follow-up.

## 2026-09-18 equality, parser, loop and runtime closure

Equality is assessed only when both operands have exactly the same source type. Every cross-type equality expression is course UB and excluded from all tests, including mixed reference mutability and cross-element/container comparisons supported by full Rust. Same-typed references compare targets and same-typed aggregates/containers compare contents.

While and loop bodies must be compatible with unit, with never coercion retained; loop results come from break values. Unlabeled break/continue in a while condition cannot target that while or an enclosing loop, while a nested loop in the condition has its own target. These follow Rust.

Parser-context punctuation now covers type-closing `>=` and `>>=` as well as `>>`. The Rust restriction after an unparenthesized cast is explicit: `<` and `<<` begin type/generic parsing, so comparing or shifting a cast result requires parentheses. These decisions are syntactic and are not deferred to semantic analysis.

The reference C runtime now exports `__rx_alloc(uint32_t size, uint32_t align)` over REIMU malloc and provides no deallocator. The reference Vec baseline uses capacity 0, then 4, then doubling, retaining old buffers until program termination. Formal execution fixes `--memory=256M --stack=1M`; official tests keep reference cumulative heap high-water, including allocator overhead, within 64 MiB.

LLVM 22.1.8 compiled the updated runtime and a generated-code-style caller for RV32IM/ILP32 with warnings denied. Their relocatable link produced an ELF32 little-endian RISC-V object with the expected `__rx_alloc` symbol and unresolved REIMU libc symbols. The available local REIMU executable ran the two assembly files under the formal memory/stack parameters, allocated and accessed two i32 values, printed `42`, returned status 0 and reported 406 weighted cycles. The REIMU source worktree has separate uncommitted allocator changes and was not rebuilt by this documentation task, so publication must still pin and rebuild the chosen commit. Full compiler-generated Box/Vec scenarios remain integration validation; the testcase and reference compiler were not changed here.

## 2026-09-18 Reference grammar restoration

The initial restoration in e111476 centralized a custom lexer/syntax grammar.
It is superseded by the original Reference-style chapter organization: Notation,
lexical subchapters, item subchapters, expression-specific chapters, and type/path
chapters now contain their own grammar blocks and linked railroad diagrams.
The original production names and alternatives were restored and then trimmed
against the settled subset. Course-specific binding, const, derive, and heap
rules are explicit additions. The grammar summary is generated from these
chapters; parser conventions retain the established context rules separately.
See reference-grammar.md for upstream provenance and the adaptation table.

Existing consolidated semantic rules were retained. If and return descriptions
were moved to their corresponding Reference chapters with compatibility links.
The course compiler, testcase submodule, and runtime implementation are unchanged.

Strict mdBook generation and active-source style checks passed. The resulting
50 active/generated HTML pages have 4,714 resolving local file, fragment, and SVG
links, with no duplicate page IDs. There are 102 unique grammar productions.
Browser checks compared the published Reference with six local grammar pages,
verified show/hide and cross-page diagram navigation, and checked 390px layouts
for the token and function chapters without horizontal page overflow.

## 2026-09-18 generic paths and specification prose review

Three delegated reviews checked syntax coverage, semantic boundaries, and
feature descriptions. The preceding revision already reached container types
and constructors through specialized productions; the main issues were their
separate syntax structure and discoverability. Box/Vec now use the Reference's
shared TypePathSegment, PathExprSegment, and GenericArgs structure. GenericArg
is Type, and constructors use ordinary PathExpression and CallExpression.
Specialized container type and construction productions have been removed.
The container type chapter explains composition and recursion using those
shared productions.

Type positions accept both `Box<T>` and `Box::<T>`, including nested concrete
types and trailing commas. Expression paths use explicit turbofish arguments.
Name resolution checks the supported entities and their arity: an empty or
multi-argument generic list can have a syntax tree while failing the container's
one-argument requirement. Ordinary functions and methods retain their existing
signatures. Struct construction uses PathInExpression with a struct-resolution
check. InherentImpl uses Type and checks that its target is a user-defined
named-field struct, including a parenthesized spelling.

The review also made Vec elements explicit in the place and assignment rules.
Cast disambiguation now distinguishes a following generic list from a completed
parenthesized type: `x as (usize) < y` is a comparison, while
`x as usize<i32>` parses a type path and fails its type-argument check.

Active chapters describe the current features directly. Static errors, course
UB, ownership/reference test guarantees, and the heap reclamation contract
retain their established meanings. The publication page now lists the remaining
parser/runtime integration checks. Editorial history and upstream comparisons
remain in these maintenance documents.

The warning-denying mdBook build and active-source style check passed. The
rendered grammar summary contains 103 productions. A temporary derivability
audit over the 76 non-lexer productions passed 126 syntax cases, including
nested containers, ordinary constructor calls, receivers, derives, and
deliberate syntax and semantic-error forms. It supplies identifier/integer token
rules and does not validate precedence, contextual parsing, semantic checking,
or the supplied parser implementation.

The final 51 active/generated HTML pages contain 4,955 resolving local file,
fragment, and SVG links, with no duplicate page IDs. Browser checks verified
the shared type/path diagrams, constructor-call diagrams, four cross-chapter
navigation paths, visibility persistence, and the new container examples.
Paths, Types, and Box/Vec type pages fit a 390px viewport without horizontal
page overflow. rustc independently accepted the parenthesized cast-type,
grouped constructor-call, and parenthesized impl-target examples. These checks
cover the documentation and selected Rust syntax; compiler and runtime
integration remain publication work.
