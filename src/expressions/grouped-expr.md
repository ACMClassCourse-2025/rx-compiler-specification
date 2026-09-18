# Grouped expressions

r[expr.paren.syntax]
```grammar,expressions
GroupedExpression -> `(` Expression `)`
```

A parenthesized expression has the value and type of its operand and preserves place status. Parentheses can force an expression context at a statement boundary and can disambiguate struct construction in conditions or a cast before `<` / `<<`.

Empty parentheses `()` form the unit expression described by [UnitExpression].
