# Backend

## Target and toolchain

The only target is little-endian RISC-V RV32IM with the ILP32 data model. External C and REIMU interfaces use the base-integer [RISC-V ELF psABI](https://riscv-non-isa.github.io/riscv-elf-psabi-doc/), the processor-specific application binary interface that defines how separately compiled code interoperates. Internal calling conventions are implementation-defined.

| Component | Required configuration |
| --- | --- |
| Clang/LLVM | Version 22 |
| Target | `riscv32-unknown-elf` |
| ISA and ABI | `-march=rv32im -mabi=ilp32` |
| REIMU | To be determined |
| Execution memory | `--memory=256M` |
| Stack | `--stack=1M` |

The compiler emits GNU-style text assembly accepted by both the Clang integrated assembler and the pinned REIMU version. A Clang build or REIMU parse, link, or execution failure violates this contract. Syntax accepted by Clang but unsupported by REIMU is not usable. Tests do not require a specific instruction selection, stack-frame shape, register allocator, symbol layout, or assembly text.

RISC-V instruction behavior follows the course-pinned RISC-V Unprivileged ISA specification. External calling behavior follows the course-pinned RISC-V ELF psABI. Both versions remain fixed for the course run.

## LLVM IR and code generation

Students must use LLVM IR as their intermediate representation and be able to emit valid textual LLVM IR (`.ll`) accepted by Clang/LLVM 22 for the course target. The IR must preserve Rx semantics and use a target triple and data layout consistent with RV32IM/ILP32.

During frontend development, students may compile this IR with Clang and run the resulting assembly in REIMU to check program behavior before their own backend is ready. The completed compiler must generate RISC-V assembly from the LLVM IR using a student-implemented backend. Clang or LLVM code generation may be used for validation, but cannot replace that backend.

Clang may compile the C runtime, and generated programs may call custom helpers. A runtime helper must not interpret the source program or IR in place of code generation. Course staff may use other tools to produce reference results and performance baselines.

<details>
<summary>Frontend validation with LLVM IR</summary>

Given the frontend's `student.ll` and the course `runtime-example.c`, compile both to text assembly and execute them together:

```sh
clang --target=riscv32-unknown-elf -march=rv32im -mabi=ilp32 \
  -O0 -S student.ll -o frontend-check.s
clang --target=riscv32-unknown-elf -march=rv32im -mabi=ilp32 \
  -O2 -fno-builtin -S runtime-example.c -o runtime.s
reimu --file=frontend-check.s,runtime.s --memory=256M --stack=1M \
  --output=frontend-check.out
```

Compare `frontend-check.out` with the expected source behavior to test the frontend independently of the student backend. The program reads from standard input; `--output` keeps its output separate from REIMU's status messages. Use the same RV32 target for this check, including 32-bit addresses and `usize`/`isize`.

</details>

## Data layout

Source integers and addresses are 32 bits. The table below gives the reference storage layout; student implementations may use another internal layout when source behavior and external interfaces remain correct.

| Type | Size (bytes) | Alignment (bytes) | Representation |
| --- | ---: | ---: | --- |
| `i32`, `u32` | 4 | 4 | 32-bit two's-complement or unsigned integer |
| `isize`, `usize` | 4 | 4 | Same width as an address |
| `bool` | 1 | 1 | `false = 0`, `true = 1` |
| `&T`, `&mut T` | 4 | 4 | One machine address |
| `()` | 0 | 1 | No data |

The representation of internal registers and objects is otherwise untested, but observable source behavior and every external interface must remain correct. References carry no runtime lifetime information. Unit return values use no return register.

<details>
<summary>Reference aggregate layout</summary>

The course implementation uses the following convenient layout, but student implementations may choose another internal layout:

- `[T; N]` has `T`'s alignment and size `size(T) * N`; element `i` begins at `i * size(T)`.
- Struct fields appear in declaration order, each at the first offset satisfying its alignment.
- A struct's alignment is the maximum field alignment, and its size is rounded up to that alignment.
- Padding bytes have unspecified values and need not be preserved when a value is copied.
- `&[T; N]` is one word, not a slice fat pointer.

Code crossing a selected runtime interface must use the same layout on both sides.

</details>

## Calling conventions and value semantics

Ordinary functions, methods, recursion, and private assembly helpers may use any consistent internal convention. The psABI is recommended for convenience but is required only at actual external boundaries such as machine `main`, the course C runtime, and REIMU libc.

The psABI's calling convention specifies where arguments and return values go, which registers a caller may expect to survive a call, and how the stack is aligned. For ordinary 32-bit integer or reference arguments, the first eight use `a0` through `a7`, and further arguments use the stack; a 32-bit scalar result uses `a0`. Callees preserve `s0` through `s11` and restore `sp`. A caller that needs a value in `a0` through `a7`, `t0` through `t6`, or `ra` after a call must preserve it itself. At these external boundaries, the stack must follow the psABI's 16-byte alignment rule. The linked specification covers aggregate arguments and the remaining rules.

Machine-level indirection must preserve source by-value semantics. For example, modifying the parameter of `fn f(mut x: [i32; 3])` cannot modify a caller value that remains valid. Storage may be reused after a move when the old value is no longer observable.

Methods may lower the receiver to an explicit parameter. Fixed arrays remain source-level by-value aggregates and do not acquire C array-to-pointer behavior.

## Symbols, entry, and runtime

The source [entry function](undefined-behavior/builtin.md#program-entry) becomes the global machine symbol `main`, with behavior equivalent to `int main(void)`. Normal completion returns status 0. All other source and helper symbol names are implementation-defined but must be unique within the program.

The source I/O builtins use this C ABI:

```c
int32_t getInt(void);
void printInt(int32_t value);
void printlnInt(int32_t value);
```

Their input and output behavior is defined by [Builtin I/O](undefined-behavior/builtin.md#io-functions). Students may use the course runtime or provide a compatible implementation. A runtime symbol is linked exactly once.

<details>
<summary>Reference C runtime</summary>

The repository includes this complete reference implementation:

```c
{{#include runtime-example.c}}
```

The local `int32_t` and `uint32_t` typedefs avoid requiring target C library headers. The `rx_int32_is_four_bytes` and `rx_uint32_is_four_bytes` typedefs are compile-time size checks: each declares an array type of length 1 when the corresponding integer type occupies four bytes, or an invalid negative length otherwise. They allocate no storage and generate no runtime instructions.

It can be compiled with:

```sh
clang --target=riscv32-unknown-elf -march=rv32im -mabi=ilp32 \
  -O2 -fno-builtin -S runtime-example.c -o runtime.s
```

</details>

## Heap allocation interface

Generated code using the course runtime allocates `Box` and `Vec` storage through this fixed C ABI, which is not source-visible:

```c
void *__rx_alloc(uint32_t size, uint32_t align);
```

`size` and `align` are byte counts. Valid calls have nonzero size and alignment 1 or 4. The function returns at least `size` accessible bytes at the requested alignment, disjoint from every allocation that has not been reused. Allocation succeeds for valid tests; failure behavior is unspecified.

There is no required `__rx_dealloc`. A custom runtime may instead use a private allocator, call REIMU `malloc`, or inline equivalent behavior. In every case, the compiler computes type size, alignment, element stride, and container capacity while the runtime manages untyped storage. Valid allocation calculations fit `usize`, so dynamic overflow checks and panic support are unnecessary.

REIMU also provides psABI-compatible `memcpy`, `memmove`, and `memset`. Use `memcpy` only for known-disjoint ranges and `memmove` when ranges may overlap. Byte helpers must not compare padding or uninitialized bytes as values and must not turn an ownership move into two owners.

<details>
<summary>Suggested compiler/runtime split</summary>

| Operation | Compiler responsibility | Runtime responsibility |
| --- | --- | --- |
| `Box` construction | Evaluate, calculate layout, initialize, and return the owner | Allocate aligned bytes |
| `Vec` growth | Calculate capacity, maintain metadata, and move live elements | Allocate a new buffer; optionally provide a helper |
| By-value operations | Select copy or move and preserve value semantics | Optionally copy raw bytes |
| Container clone | Recursively clone the concrete `T` | Allocate result storage |
| Program end | No typed destruction is required | Reclaim the execution's heap |

</details>

## Resource guarantees

<!-- Tests run with 256 MiB total execution memory and a 1 MiB stack. Under the reference runtime and growth strategy below, cumulative heap high-water, including allocator metadata, alignment padding, and retained old buffers, does not exceed 64 MiB. Valid sizes, capacities, and byte-count products fit 32-bit `usize`.

The 64 MiB value is test headroom, not a source-visible heap size or a guarantee that one allocation of that size succeeds. Text, static data, stack, and heap share the 256 MiB execution limit. No separate static-data limit applies.

<details>
<summary>Reference Vec growth and allocation strategy</summary>

The reference implementation uses this baseline for test sizing:

1. An empty `Vec` has zero length and capacity and allocates no buffer.
2. `push` grows capacity from 0 to 4, then doubles it whenever full.
3. Growth allocates a new buffer, moves elements in order, and retains the old buffer until program end.
4. `remove` does not shrink capacity.
5. Cloning an empty `Vec` allocates nothing; cloning a nonempty `Vec` allocates exactly enough space for its current length.
6. A `Box<T>` allocates exactly `size(T)` bytes.

Capacity remains unobservable, and student implementations may use another layout or growth factor. They must still finish the same tests within the published memory limits.

</details> -->

To be determined. Let the TAs finish the testcases first XD.

## Assembly and execution

REIMU accepts text assembly files and assembles, links, and executes them internally. After compiling the selected C runtime to `runtime.s` as shown above, run the student backend's `student.s` with:

```sh
reimu --file=student.s,runtime.s --memory=256M --stack=1M \
  --output=student.out
```

Both assembly inputs are text `.s` files; the program reads from standard input and writes to `student.out`. REIMU resolves calls to its supported libc functions, such as `printf`, `scanf`, and `malloc`. No object file or ELF binary is passed to REIMU. A custom runtime supplies its own assembly in place of `runtime.s`.

Normal source completion returns status 0, and every output byte, including whitespace, is observable.

Valid programs may contain constants and stack offsets outside a 12-bit immediate and control-flow spans beyond one conditional branch. The specification places no separate limit on source size, compile time, compiler memory, or generated assembly size.

Submission commands and grading policy are course administration rather than language or execution semantics.

## Optimization and execution cost

No specific optimization is mandatory. A stack-slot baseline is valid if it handles calls, finite registers, and all valid control flow. Optimizations may change IR, registers, storage, and instruction sequences but must preserve observable source behavior and external interfaces.

Performance uses `Total cycles` reported by the pinned REIMU version with its default weights and no cost-model-changing options. The count includes generated code, runtime instructions, and priced libc operations. Baselines, aggregation, scores, and rankings are separate assessment policy.
