# Associated items

r[items.associated.syntax]
```grammar,items
AssociatedItem -> ConstantItem | Function
```

An inherent impl contains associated constants, associated functions, and methods.

Associated functions use the [function and receiver rules](functions.md#receivers). A [PathInExpression] such as `Point::new` or `Self::LIMIT` names an associated item. Method calls use [MethodCallExpression] and the [receiver lookup rules](../expressions/method-call-expr.md#method-lookup).
