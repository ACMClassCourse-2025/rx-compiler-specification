# Parser conventions

The language grammar is defined by the Lexer and Syntax blocks in each chapter.
The [grammar summary](grammar-summary.md) collects those same productions in
one place. The rules below specify precedence, contextual token interpretation,
and expression boundaries for the resulting syntax trees.

## Syntax coverage

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
| Constant contexts | [ConstValue], [Magnitude] | `123`, `(-1)`, `-(2147483648i32)`, `((true))` |

The grammar describes the shape of a construct. Name resolution, typing, place
mutability, and the [test domain](undefined-behavior.md) determine which parsed
programs are valid. For example, the identifier in a struct construction must
name a struct, a method receiver belongs to an inherent impl, and an assignment
destination must denote a mutable place.

## Items and types

A receiver and the final ordinary parameter each allow a trailing comma.
Derive attributes accept an optional trailing comma; each capability occurs at
most once across the attributes on a struct. `#[derive()]` is an empty list.

Generic type paths use [TypePathSegment] with [GenericArgs]. In type contexts,
the separator before the argument list is optional: `Box<T>` and `Box::<T>`
name the same type. Expression paths introduce the list with `::`, as in
`Box::<T>::new(value)` and `Vec::<T>::new()`. [GenericArgList] accepts
comma-separated lifetime and type arguments and an optional trailing comma.
Box and Vec each require one concrete type argument; this arity is checked
against the resolved builtin. [Type] recursively describes each type argument,
including arrays, references, structs, `Self`, and container paths.

[GenericParams] introduces lifetime parameters on functions, structs, and
impls; [WhereClause] expresses lifetime bounds. [ReferenceType] and reference
receivers accept a [Lifetime] after `&`. Path lifetime arguments precede type
arguments. Their declarations, arguments, bounds, and elision follow the
[lifetime validity contract](references.md#lifetime-validity), including the
classification of incorrect lifetime use as course UB. Type-argument checks
continue to enforce the declared type parameters of the resolved name.

`Self` denotes the struct in its declaration and its inherent impls. The
[naming rules](names.md) determine builtin-name protection and lookup. Integer
and identifier tokens may have any finite length. Literal overflow and
let/parameter collisions with visible unqualified const names follow the
[course UB rules](undefined-behavior.md).

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

## Casts and conditions

The [operator precedence table](expressions.md#precedence) governs expression
nesting. After a type-path segment in a cast, `<` begins [GenericArgs] rather
than a comparison. A leading `<` from `<<` likewise enters type-argument
parsing. Parenthesizing the cast makes the intended operation explicit:
`(x as usize) < y` and `(x as usize) << y`. A parenthesized type already closes
the type syntax, so `x as (usize) < y` and `x as (usize) << y` also parse as
comparison and shift. Operators such as `<=`, `>`, `>>`, and `==` follow
ordinary precedence after the cast type.

The parser checks the resulting syntax; name resolution checks its type
arguments. Thus `x as usize<i32>` has a type-path syntax tree but is a static
error because the primitive usize takes no type arguments.

[Conditions] has boolean type. At an ambiguous `Name {` boundary in an if or
while condition, the brace begins the body block. Delimited expressions can
contain a struct construction, as in `if (S { flag: true }).flag { ... }` and
`if check(S { flag: true }) { ... }`. A block expression may itself be the
condition: `if { true } { ... }`. See [If expressions](expressions/if-expr.md).

## Expressions and statement boundaries

At an expression-statement boundary, a complete outer expression-with-block
finishes the statement before a following ambiguous infix operator. An
attached else or else-if belongs to its if expression. Field and method
postfix continuations can remain part of the expression, as in
`{ make() }.field;`.

A let initializer and other value contexts continue parsing the full expression
according to precedence. A final expression before the enclosing `}` is the
block's tail and supplies its result. A semicolon-free expression-with-block
used as a preceding statement must have unit type or diverge. These rules also
apply when the final [ExpressionStatement] represents a tail with an outer
block form. See [Statement boundary](statements.md#statement-boundary).

Function and method calls resolve a declared callable and match its parameter
list. Box construction takes one argument, Vec construction takes an empty
argument list, and both state their element type explicitly. Assignment updates
one mutable place, including an array or Vec element.

The bodies of while and loop must be compatible with unit; a diverging body is
compatible. Break values determine a loop expression's result. A loop nested
inside a while condition provides its own jump target. Unlabeled break and
continue in that condition must target a loop inside the condition. See
[Control flow](expressions/loop-expr.md#while-and-loop).
