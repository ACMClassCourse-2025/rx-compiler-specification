# Parser conventions

The language grammar is defined by the Lexer and Syntax blocks in each chapter.
The [grammar summary](grammar-summary.md) collects those same productions in
one place. This guide locates the rules for precedence and expression boundaries, and
specifies contextual token interpretation.

<details>
<summary>Grammar coverage by source form</summary>

Each occurrence of [Type] and [Expression] admits the alternatives in their
respective productions. This composition applies recursively: a struct field
may have a Vec of Boxes, a constructor argument may contain a block, and a
constructor result may be followed by indexing or a method call.

| Source form | Defining productions | Examples |
| --- | --- | --- |
| Identifiers and integers | [IDENTIFIER], [INTEGER_LITERAL] | `_value`, `123_i32`, `0b1010` |
| Lifetime tokens and forms | [LIFETIME_TOKEN], [Lifetime] | `'a`, `'static`, `'_` |
| Lifetime parameters and bounds | [GenericParams], [LifetimeParam], [WhereClause] | `fn f<'a>(x: &'a i32)`, `where 'a: 'b` |
| Source compilation unit | [Crate], [Item] | Top-level functions, structs, constants, impls |
| Functions and receivers | [Function], [FunctionParameters], [ShorthandSelf] | `fn f(v: Vec<i32>) -> Box<i32> { ... }`, `&mut self` |
| Struct declarations and construction | [StructStruct], [StructExpression] | `struct S { x: i32 }`, `S { x: 1 }`, `Self { x: 1 }` |
| Associated items | [InherentImpl], [AssociatedItem] | `impl S { fn new() -> Self { ... } }` |
| Derive attributes | [OuterAttribute], [DeriveAttribute] | `#[derive(Clone, PartialEq, Eq)]` |
| Bindings and statements | [LetStatement], [ExpressionStatement] | `let mut v = Vec::<i32>::new();`, `v.push(1);` |
| Type names, arrays, references, and unit | [TypePath], [ArrayType], [ReferenceType], [TupleType] | `Self`, `[Vec<i32>; 2]`, `&mut Box<i32>`, `()` |
| Container types | [TypePath], [TypePathSegment], [GenericArgs] | `Box<[i32; 4]>`, `Vec<Box<Node>>`, `Vec<Vec<i32,>,>` |
| Container construction | [CallExpression], [PathInExpression] | `Box::<i32>::new(7)`, `Vec::<Box<i32>>::new()` |
| Paths, calls, fields, and indexing | [PathExpression], [CallExpression], [MethodCallExpression], [FieldExpression], [IndexExpression] | `S::new()`, `value.clone()`, `value.field`, `values[0]` |
| Arrays, grouping, and unit values | [ArrayExpression], [GroupedExpression], [TupleExpression] | `[1, 2, 3]`, `[0; 4]`, `(value)`, `()` |
| Operators | [OperatorExpression] | `*p`, `&mut values[0]`, `a + b`, `x += &y` |
| Blocks and branching | [BlockExpression], [IfExpression] | `{ make() }.field`, `if flag { a } else { b }` |
| Loops and jumps | [LoopExpression], [BreakExpression], [ContinueExpression], [ReturnExpression] | `loop { break 3; }`, `while condition { ... }`, `return value` |
| Constant contexts | [ConstValue], [Magnitude] | `123`, `COUNT`, `(-1)`, `-(2147483648i32)`, `((true))` |

The grammar describes the shape of a construct. Name resolution, typing, place
mutability, and the [test domain](undefined-behavior.md) determine which parsed
programs are valid. For example, the identifier in a struct construction must
name a struct, a method receiver belongs to an inherent impl, and an assignment
destination must denote a mutable place.

</details>

<details>
<summary>Rule locations</summary>

| Topic | Defining chapter |
| --- | --- |
| Type and expression arguments | [Paths](paths.md#generic-arguments) |
| Lifetime declarations and bounds | [Lifetime parameters](items/generics.md) |
| Derive attributes | [Builtin traits](builtin-traits.md#derive) |
| Operator precedence | [Expressions](expressions.md#precedence) |
| Casts followed by comparison or shift | [Cast parsing](expressions/operator-expr.md#cast-parsing) |
| Struct construction in a condition | [If expressions](expressions/if-expr.md) |
| Expression-statement boundaries | [Statements](statements.md#statement-boundary) |
| Block tails | [Block expressions](expressions/block-expr.md) |
| Break/continue targets and loop results | [Loops](expressions/loop-expr.md#while-and-loop) |
| Never and unreachable code | [Never type](types/never.md) |
| Assignment places | [Assignment destinations](expressions/operator-expr.md#assignment-destinations) |

</details>

## Contextual punctuation

The parser interprets combined punctuation according to its current grammar
context. The lexer may emit a combined token; the parser consumes the needed
prefix and leaves the remaining punctuation for the enclosing construct.

| Combined token | Context | Interpretation |
| --- | --- | --- |
| `&&` | Reference type or prefix borrow | Two `&` tokens, as in `&&i32` or `&&x` |
| `>>` | Closing nested type arguments | Two closing `>` tokens |
| `>=` | Closing type arguments before assignment | A closing `>` followed by `=` |
| `>>=` | Closing nested type arguments before assignment | Two closing `>` tokens followed by `=` |

In ordinary infix expression contexts, these tokens denote their corresponding
operators. Thus the following spellings need no space before the assignment:

```rust,ignore
let values: Vec<i32>=Vec::<i32>::new();
let nested: Vec<Vec<i32>>=Vec::<Vec<i32>>::new();
```
