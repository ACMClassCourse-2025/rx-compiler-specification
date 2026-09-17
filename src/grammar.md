# Core grammar

This grammar collects the settled core syntax. Semantic restrictions in the other chapters still apply: receiver parameters are only for methods, constant forms have restricted types, derives must be valid, assignment requires a place, and zero-sized data is outside the test domain.

It is an intentionally precedence-neutral grammar. Use the [operator precedence](expressions.md#precedence), [statement boundary](statements.md#statement-boundary), and Rust-compatible condition/struct-literal disambiguation rules when constructing a parser. It is not intended to be fed unchanged to an LR or recursive-descent generator.

The name/path surface is the finite one specified in [Names](names.md). Type arguments on expressions appear only in `Box::<T>::new(value)` and `Vec::<T>::new()`, with explicit concrete T. Type-position `_`, destructuring assignment, and explicit never-type annotations are unsupported; `!` appears only as an expression operator in this grammar.

## Tokens, items, and types

```grammar,syntax
@root Crate -> Item*

Item -> Function | Struct | ConstItem | InherentImpl

Binding -> `mut`? Identifier

Function -> `fn` Identifier `(` Parameters? `)` ReturnType? Block

Parameters -> Parameter (`,` Parameter)* `,`?

Parameter -> Binding `:` Type

ReturnType -> `->` Type

Struct -> Derive* `struct` Identifier `{` StructFields? `}`

StructFields -> StructField (`,` StructField)* `,`?

StructField -> Identifier `:` Type

Derive -> `#` `[` `derive` `(` (DeriveName (`,` DeriveName)* `,`?)? `)` `]`

DeriveName -> `Copy` | `Clone` | `PartialEq` | `Eq`

ConstItem -> `const` Identifier `:` Type `=` ConstValue `;`

InherentImpl -> `impl` Identifier `{` AssociatedItem* `}`

AssociatedItem -> AssociatedFunction | ConstItem

AssociatedFunction -> `fn` Identifier `(` MethodParameters? `)` ReturnType? Block

MethodParameters -> Parameters | Receiver `,`? | Receiver `,` Parameters

Receiver -> `self` | `mut` `self` | `&` `self` | `&` `mut` `self`

Type -> PrimitiveType | TypeName | ReferenceType | ArrayType | HeapType | `(` `)` | `(` Type `)`

PrimitiveType -> `i32` | `u32` | `isize` | `usize` | `bool`

TypeName -> Identifier | `Self`

ReferenceType -> `&` `mut`? Type

ArrayType -> `[` Type `;` ConstValue `]`

HeapType -> (`Box` | `Vec`) `<` Type `,`? `>`

ConstValue -> IntegerLiteral | BooleanLiteral | `-` Magnitude | `(` ConstValue `)`

Magnitude -> IntegerLiteral | `(` Magnitude `)`
```

The optional comma after a receiver alone and the comma after a final ordinary parameter each allow one trailing comma. Repeated commas are not accepted. Derive names cannot repeat within or across attributes on one struct; `#[derive()]` has no effect. Empty structs and zero-length arrays are parsed forms whose use is excluded by the zero-sized-data rules. Identifiers and integer tokens have no fixed length limit. Integer-literal overflow after type determination and let/parameter collisions with a visible unqualified const name are course UB, not required diagnostics. Signed-minimum literal forms remain valid. Self is available in struct definitions and inherent impls. Builtin type and value names are protected under the namespace rules.

In a type context, the parser consumes leading `>` characters from combined punctuation as closing delimiters and leaves any remaining punctuation to the enclosing syntax. This handles `>>`, `>=`, and `>>=` in examples such as `Vec<Vec<i32>>` and `let v: Vec<Vec<i32>>=Vec::<Vec<i32>>::new();`. In a reference-type or prefix-borrow context, `&&` is interpreted as two `&` tokens, so `&&i32` and `&&x` are supported. A lexer may produce combined punctuation tokens; the parser interprets or splits them according to syntactic context. In ordinary infix expression context, the combined tokens retain their operator meanings.

## Statements and expressions

```grammar,syntax
Block -> `{` Statement* Expression? `}`

Statement -> `;` | LetStatement | Expression `;` | ExpressionWithBlock

LetStatement -> `let` Binding (`:` Type)? `=` Expression `;`

Expression -> ExpressionWithBlock | ExpressionWithoutBlock

ExpressionWithBlock -> Block | IfExpression | WhileExpression | LoopExpression

ExpressionWithoutBlock ->
      IntegerLiteral | BooleanLiteral | Path | `(` `)` | `(` Expression `)`
    | ArrayExpression | StructExpression
    | CallExpression | HeapConstruction | MethodCallExpression | FieldExpression | IndexExpression
    | UnaryExpression | BinaryExpression | CastExpression | AssignmentExpression
    | `return` Expression? | `break` Expression? | `continue`

Path -> Identifier | `self` | TypeName `::` Identifier

HeapConstruction ->
      `Box` `::` `<` Type `,`? `>` `::` `new` `(` Expression `,`? `)`
    | `Vec` `::` `<` Type `,`? `>` `::` `new` `(` `)`

ArrayExpression -> `[` (Arguments | Expression `;` ConstValue)? `]`

Arguments -> Expression (`,` Expression)* `,`?

StructExpression -> TypeName `{` StructInitializers? `}`

StructInitializers -> StructInitializer (`,` StructInitializer)* `,`?

StructInitializer -> Identifier `:` Expression

CallExpression -> Expression `(` Arguments? `)`

MethodCallExpression -> Expression `.` Identifier `(` Arguments? `)`

FieldExpression -> Expression `.` Identifier

IndexExpression -> Expression `[` Expression `]`

UnaryExpression -> (`-` | `!` | `*` | `&` `mut`?) Expression

BinaryExpression -> Expression BinaryOperator Expression

BinaryOperator ->
      `+` | `-` | `*` | `/` | `%` | `&` | `|` | `^` | `<<` | `>>`
    | `==` | `!=` | `<` | `<=` | `>` | `>=` | `&&` | `||`

CastExpression -> Expression `as` Type

AssignmentExpression -> Expression AssignmentOperator Expression

AssignmentOperator ->
      `=` | `+=` | `-=` | `*=` | `/=` | `%=` | `&=` | `|=` | `^=` | `<<=` | `>>=`

IfExpression -> `if` Condition Block (`else` (Block | IfExpression))?

WhileExpression -> `while` Condition Block

Condition -> Expression

LoopExpression -> `loop` Block
```

Calls must resolve to supported functions or methods; the expression-shaped callee does not introduce function pointers. Although a broad Path production can parse `Box::new` or `Vec::new`, these forms are outside the subset: constructors require HeapConstruction with explicit T. The constructor's T may itself contain nested Box/Vec and references, but no `_`. No other turbofish syntax is supported.

Cast parsing retains Rust's syntactic restriction: an unparenthesized `<` or `<<` immediately after `Expression as Type` is treated as the beginning of type/generic syntax rather than as a comparison or shift. Because the corresponding type syntax is unsupported here, that source is rejected by the parser. `(x as usize) < y` and `(x as usize) << y` are the supported forms. This decision uses the grammar context alone and is not deferred to semantic analysis.

A struct literal requires each field exactly once. Condition denotes an expression parsed with Rust's condition restrictions: at an ambiguous top-level `Name {`, the brace starts the consequent/body block rather than a struct initializer. This restriction does not remove explicitly delimited expressions such as `(S { field: value })`, `check(S { field: value })`, or a block expression serving as the condition. Conditions still have bool type. See [Control flow](expressions/loop-expr.md#if).

At expression-statement boundaries, follow Rust's block-expression disambiguation. A complete outer expression-with-block is not extended by a following ambiguous infix operator; in a value context such as a let initializer, parsing continues normally. This is not a scan that stops at the first closing brace: attached else branches belong to the if, Rust-permitted field/method postfix continuations still apply, and a final expression before the enclosing `}` can be its tail without a semicolon. A semicolon-free expression-with-block statement must satisfy the statement typing rules.

An assignment's left expression must denote a single mutable place; array/struct destructuring assignees and `_` are unsupported.

The body block of while and loop must be compatible with unit; a diverging body is compatible. Break values determine a loop expression's result. An unlabeled break/continue in a while condition cannot target that while or an enclosing loop; a loop nested inside the condition still provides its own target. See [Control flow](expressions/loop-expr.md#while-and-loop).
