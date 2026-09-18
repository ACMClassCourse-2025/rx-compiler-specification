# Builtin I/O and program entry

## Program entry

Every executable program has one top-level entry function:

```rust,ignore
fn main() -> () {
    printlnInt(42);
}
```

The return annotation may be omitted. Main has an empty value-parameter list and no generic parameters. Reaching its end or executing `return;` ends the source program normally. The generated machine entry returns status 0 as specified by the [backend contract](../backend.md#4-符号入口和-runtime). Source programs cannot call main.

Programs print their answers using the I/O functions below.

## I/O functions

The following interface notation describes the builtin signatures:

```rust,ignore
fn getInt() -> i32;
fn printInt(value: i32) -> ();
fn printlnInt(value: i32) -> ();
```

getInt reads a signed decimal i32 from standard input. Official inputs contain enough valid, in-range integers separated by ASCII whitespace; malformed input and premature end of input are excluded. printInt writes the signed decimal representation with no additional characters. printlnInt writes the same representation followed by one LF byte, 0x0a.

Arguments are ordinary i32 values. A value of another integer type needs an explicit cast. Unit results can be discarded as expression statements; they do not permit ordinary zero-sized bindings.

These three functions form the source-visible I/O interface. Students may use the provided runtime or implement a compatible one under the backend contract.

## Other builtin facilities

The [Copy, Clone, PartialEq, and Eq](../builtin-traits.md) capabilities, fixed-array len, and [Box/Vec facilities](../heap.md) are described in their own chapters. Heap storage may remain until program termination and be reclaimed together. The reference C runtime implements I/O and the `__rx_alloc` wrapper used by generated code.
