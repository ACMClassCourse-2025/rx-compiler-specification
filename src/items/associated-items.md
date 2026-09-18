# Associated items

r[items.associated.syntax]
```grammar,items
AssociatedItem -> ConstantItem | Function
```

An inherent impl can contain associated constants, associated functions, and methods. This trims the Reference's associated-item alternatives by removing type aliases, macros, visibility modifiers, and arbitrary attributes.

A function with a [SelfParam] is a method; otherwise it is an associated function. A [PathInExpression] such as `Point::new` or `Self::LIMIT` names an associated item. Method calls use [MethodCallExpression] and the [receiver lookup rules](../names.md#method-lookup).
