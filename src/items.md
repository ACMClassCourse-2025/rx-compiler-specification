# Items

```grammar,items
Item -> UseDeclaration | Function | Struct | ConstantItem | Implementation
```

Items form a [source compilation unit](crates-and-source-files.md).
The following chapters define [use declarations](items/use-declarations.md),
[functions](items/functions.md),
[structs](items/structs.md), [constants](items/constant-items.md), and
[inherent implementations](items/implementations.md). Impl members use
[associated items](items/associated-items.md); declaration lifetime lists use
[lifetime parameters and bounds](items/generics.md).
