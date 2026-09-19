# Builtin I/O and program entry

## Program entry

Every executable program has one top-level entry function:

```rust,ignore
fn main() -> () {
    println_i32(42);
}
```

The return annotation may be omitted. Main has an empty value-parameter list and no generic parameters. 

Program entry invokes this source function and returns status 0 after that initial invocation completes. The recommended implementation gives the source function a unique mangled internal symbol and exports a C ABI wrapper as `main`. The wrapper calls the source function and returns the signed 32-bit value 0. Equivalent lowering must preserve these behaviors, as specified by the [backend contract](../backend.md#symbols-entry-and-runtime).

Programs print their answers using the I/O functions below.

## I/O functions

The following interface notation describes the builtin signatures:

```rust,ignore
fn get_i32() -> i32;
fn print_i32(value: i32) -> ();
fn println_i32(value: i32) -> ();
```

get_i32 reads a signed decimal i32 from standard input. Official inputs contain enough valid, in-range integers separated by ASCII whitespace; malformed input and premature end of input are excluded. print_i32 writes the signed decimal representation with no additional characters. println_i32 writes the same representation followed by one LF byte, 0x0a.

Arguments are ordinary i32 values. A value of another integer type needs an explicit cast. Unit results can be discarded as expression statements; they do not permit ordinary zero-sized bindings.

These three functions form the source-visible I/O interface. Students may use the provided runtime or implement a compatible one under the backend contract.

## Other builtin facilities

The [builtin traits](../builtin-traits.md), [array methods](../expressions/array-expr.md#array-methods), and [Box/Vec operations](../heap.md) are defined in their own chapters. The [runtime contract](../backend.md#heap-allocation-interface) defines the generated-code allocation interface.
