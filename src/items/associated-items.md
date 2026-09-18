# Associated items

r[items.associated.syntax]
```grammar,items
AssociatedItem -> ConstantItem | Function
```

An inherent impl contains associated constants, associated functions, and methods.

A function with a [SelfParam] is a method; otherwise it is an associated function. A [PathInExpression] such as `Point::new` or `Self::LIMIT` names an associated item. Method calls use [MethodCallExpression] and the [receiver lookup rules](../names.md#method-lookup).
