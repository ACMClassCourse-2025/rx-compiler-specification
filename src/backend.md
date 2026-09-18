# 后端与执行契约（工作草案）

> 本文已纳入 2026 工作版规范，尚未冻结。堆接口与回收策略见 [Box and Vec](heap.md)，发布验证见 [Publication validation](open-decisions.md)。

## 1. 规范范围

课程后端的唯一目标平台为：

- RISC-V `RV32IM`；
- little-endian；
- `ILP32` 数据模型；
- 平台入口和所调用 C / REIMU libc 接口采用 RISC-V ELF psABI 的 base integer
  calling convention；学生代码的内部调用约定自行决定。

除本文明确补充或修改的部分外，指令语义以 RISC-V Unprivileged ISA
Specification 为准。平台入口和 C 接口的调用约定以课程公布版本的 RISC-V ELF
psABI 为准；不要求内部函数与其他编译器互调用。课程在开课前公布这两份文档的
版本或 commit，并在本学期内保持不变。

学生编译器输出 Clang integrated assembler 接受的 GNU-style 文本汇编。正式输出
必须同时：

1. 能被 Clang/LLVM 22 以 target triple `riscv32-unknown-elf`、
   `-march=rv32im -mabi=ilp32` 构建；
2. 能被课程指定版本的 REIMU 解析并正确执行。

Clang 支持但 REIMU 不支持的语法不属于可用范围。评测不比较汇编文本，也不要求
特定的指令选择、栈帧形式、寄存器分配算法或 section 排列。

正式工具链统一使用 LLVM 22，不依赖 GCC 或 GNU binutils。REIMU 采用课程发布时的
最新版本，并记录该版本的 commit；评测镜像在本学期内保持不变。发布信息为：

| 组件 | 正式版本 |
| --- | --- |
| Clang/LLVM | 22 |
| target triple | `riscv32-unknown-elf` |
| ISA 与 ABI 参数 | `-march=rv32im -mabi=ilp32` |
| REIMU | 课程发布时的最新 commit；发布时记录 hash |
| REIMU memory 参数 | `--memory=256M` |
| REIMU stack 参数 | `--stack=1M` |

评测器不设置独立的“assembler 检查”阶段。Clang 构建学生的 `.s` 文件时已经完成
汇编；该构建失败即不符合本契约。REIMU 随后对同一汇编及 runtime 进行解析、
链接和执行；任一步失败均不符合本契约。

使用课程 C runtime 时，兼容性构建采用下面的一次性 relocatable link：

```sh
clang --target=riscv32-unknown-elf -march=rv32im -mabi=ilp32 \
  -fuse-ld=lld -nostdlib -fno-builtin -O2 -Wl,-r \
  student.s runtime-example.c -o check.o
```

这里使用 `-r` 是因为 `printf`、`scanf` 等符号在实际执行时由 REIMU 提供；
`check.o` 不作为执行产物。评测仍将原始文本汇编交给 REIMU。使用自定义 runtime
时，以其源码或汇编替换命令中的 `runtime-example.c`。

学生需自行完成源程序及其 IR 到 RISC-V 的 CodeGen，不能直接交由 Clang/LLVM
后端代替。允许用 Clang 编译 C runtime；不为自定义 helper 设置固定清单。runtime
不得通过解释执行源程序或 IR 代替生成目标代码。课程组生成参考结果和性能基线
不受上述后端实现要求限制。

## 2. 数据布局

源码整数位宽和目标地址宽度为 32 位。下列存储布局供官方实现和学生实现参考，
不要求学生的内部对象与其他编译器互操作；内部布局可自行调整，只要保持源语言
的可观察行为。跨所选 runtime 接口传递数据时，调用双方必须使用一致的表示。

| 类型 | 大小（byte） | 对齐（byte） | 表示 |
| --- | ---: | ---: | --- |
| `i32`、`u32` | 4 | 4 | 32-bit two's complement / unsigned integer |
| `isize`、`usize` | 4 | 4 | 与目标指针等宽 |
| `bool` | 1 | 1 | `false = 0x00`，`true = 0x01` |
| `&T`、`&mut T` | 4 | 4 | 一个 machine address |
| `()` | 0 | 1 | 无数据 |

参考布局中，`bool` 对象用 `0` / `1` 表示。内部寄存器和存储表示不单独验收，
但布尔运算、整数转换以及实际外部接口的结果必须符合源语言语义。

参考布局中，`[T; N]` 的对齐与 `T` 相同，大小为 `size(T) * N`，第 `i` 个元素位于
`i * size(T)`。多维数组递归应用本规则。

普通 struct 的推荐布局如下：

1. 字段顺序与声明顺序相同；
2. 每个字段放在满足其对齐要求的最小 offset；
3. struct 的对齐为所有字段对齐的最大值；
4. struct 的大小向其对齐向上取整，因此可以包含字段间 padding 和 tail
   padding。

padding byte 的值未指定。复制一个值必须复制其字段值，不要求保留 padding 的
内容。

引用不需要携带生命周期信息。参考表示中，`&[T; N]` 和 `&mut [T; N]` 都是一个
machine word，而不是 slice fat pointer。若以后加入能观察布局的原始指针运算、
`size_of` 或外部聚合接口，再单独明确相应保证。

### 2.1 零大小类型

核心语言只使用 `()` 表示函数或控制流没有结果。unit 返回值没有机器级表示，也不
占用返回寄存器。返回 `()` 的函数调用可以正常作为 expression statement 使用，
例如 `printlnInt(1);`。

零大小类型不作为普通数据类型使用。包含下列任一情况的程序具有未定义行为，编译器
不需要诊断，官方测试保证不包含这些情况：

- 使用零大小类型作为函数或方法的参数类型；
- 将零大小值绑定到局部变量、常量或静态变量，或者对这样的 place 赋值；
- 在 struct 中声明零大小字段，或者声明空 struct；
- 使用零长度数组，或者使用零大小类型作为数组元素；
- 对零大小值取引用，或者解引用一个指向零大小类型的引用；
- 以其他方式要求零大小值具有可观察的存储位置或地址。

函数省略返回类型、显式返回 `()`、自然执行到函数末尾以及 `return;` 不属于上述
情况。

## 3. 函数 ABI

普通函数、方法、递归调用及自有汇编 helper 的调用约定由实现自行决定。建议使用
RISC-V ELF psABI 以方便调试和复用工具，但不单独检查内部参数寄存器、聚合分类、
隐藏返回指针或保存寄存器的选择，也不要求与另一份编译器生成的函数互调用。

机器级 `main`、课程 C runtime 和 REIMU libc 等实际外部边界必须遵守其已公布
接口。若自定义 runtime 与生成代码共同使用私有约定，只需双方一致；调用由
Clang 编译的 C 函数时，仍需正确适配其 psABI。

方法的 receiver 可作为显式参数降低。`self`、`&self` 和 `&mut self` 的源码值／
引用语义保持不变。固定数组仍是 by-value aggregate，不因内部采用地址传参就
获得 C 风格的 array-to-pointer 源码语义。

### 3.1 源语言值语义

机器层面的间接传递不得改变源语言的 by-value 语义。采用地址传参时，调用双方
必须保证参数在语义上独立，或使用能够证明等价的优化。

例如，调用 `fn f(mut x: [i32; 3])` 后，`f` 对 `x` 的修改不能改变 caller 中仍然
有效的原值。对于已经 move 且之后不可观察的实参，实现可以复用其存储。`Copy`
性质影响 caller 是否仍可观察原值，不能由选用的调用约定反过来改变。

## 4. 符号、入口和 runtime

除 `main` 和官方 runtime 符号外，普通函数及方法的 symbol mangling 由编译器自行
决定，但必须避免同一编译单元内的冲突。调用约定按第 3 节由实际边界决定。

源语言入口为：

```rust,ignore
fn main() -> () {
    // ...
}
```

省略返回类型等同于返回 `()`。最终汇编必须定义全局符号 `main`。机器级入口按
`int main(void)` 处理；源语言 `main` 自然结束或执行 `return;` 后均返回状态
`0`。源程序不能调用 `main`。

runtime 包括源语言可直接调用的 builtin，以及编译器生成代码时使用的内存管理等
辅助接口。下面三个函数是当前 I/O 部分，不代表最终 runtime 的全部接口。

当前 I/O 接口为：

```rust,ignore
fn getInt() -> i32;
fn printInt(value: i32) -> ();
fn printlnInt(value: i32) -> ();
```

其对应机器级接口与下面的 C declarations ABI-compatible：

```c
int32_t getInt(void);
void printInt(int32_t value);
void printlnInt(int32_t value);
```

`printInt` 输出参数的有符号十进制表示，不输出额外字符；`printlnInt` 在相同内容后
输出一个 LF byte (`0x0a`)。测试输入只包含能够被 `getInt` 读取的、位于 `i32`
范围内的十进制整数，整数之间以 ASCII whitespace 分隔。非法输入和输入提前结束
不在程序定义域内。

课程提供一份可选 runtime 实现并参与链接或 REIMU 加载。课程 C runtime 和
REIMU libc 只承诺其接口规定的寄存器保持关系；自定义 runtime 的内部约定由
实现自行决定。

课程同时提供下面的 C 参考 runtime；仓库中的完整文件为
<a href="runtime-example.c">runtime-example.c</a>：

```c
typedef int int32_t;
typedef unsigned int uint32_t;

extern int printf(const char *format, ...);
extern int scanf(const char *format, ...);
extern void *malloc(uint32_t size);

void *__rx_alloc(uint32_t size, uint32_t align) {
  (void)align;
  return malloc(size);
}

int32_t getInt(void) {
  int32_t value;
  (void)scanf("%d", &value);
  return value;
}

void printInt(int32_t value) { (void)printf("%d", value); }
void printlnInt(int32_t value) { (void)printf("%d\n", value); }
```

参考 runtime 使用 Clang 22 按如下方式生成汇编：

```sh
clang --target=riscv32-unknown-elf -march=rv32im -mabi=ilp32 \
  -O2 -fno-builtin -S runtime-example.c -o runtime.s
```

学生可以使用课程提供的 runtime，也可以自行实现。不限制自定义 helper 的数量、
签名和具体实现；生成代码与 runtime 合起来必须实现规定的源码行为，且不能以
runtime 解释执行源程序或 IR 代替目标代码生成。每个符号只链接一份 runtime
实现，不设置学生 runtime 与其他编译器之间的互调用测试。
正式性能评测把 runtime 指令和 REIMU libc 成本一并计入
`Total cycles`。性能用例应保持较低的 I/O 占比，避免 runtime 实现主导编译器优化
成绩。

### 4.1 Box / Vec 与内存辅助接口

源语言已选定内建 `Box<T>` / `Vec<T>` 作为堆方案，不因此开放源语言 unsafe 或
原始指针分配接口。容器可组合支持的合法具体类型，包括嵌套容器和经间接存储的
递归类型。构造必须显式写 `Box::<T>::new(value)` / `Vec::<T>::new()`；最小操作集
和条件 Clone / PartialEq / Eq 已在 [堆章节](heap.md) 规定。堆内存在程序结束时统一
回收，不要求在作用域退出、覆盖赋值、move-out 或 Vec 扩容时立即释放。
使用课程参考 runtime 时，生成代码通过下面的固定 C ABI 接口分配存储；它不是
源语言 builtin：

```c
void *__rx_alloc(uint32_t size, uint32_t align);
```

`uint32_t` 表示本目标的 32 位无符号整数，size 和 align 的单位是字节。有效调用的
size 非零，align 为 1 或 4，size 与整个执行的累计分配均满足下面的资源保证。
`__rx_alloc` 返回满足 align、与其他未重用分配不重叠且至少可访问 size 字节的地址。
有效测试中分配成功；不规定失败返回值或 panic 行为。参考实现在 REIMU 上调用
`malloc`，其返回对这两个 align 均有效。

课程不提供也不要求 `__rx_dealloc`。学生使用参考 runtime 时可以直接调用
`__rx_alloc`；自定义 runtime 可以使用私有分配接口、直接调用 REIMU `malloc` 或
内联等价实现，只要与生成代码一致并满足源码语义。程序结束时由 REIMU 执行环境
释放该次运行的全部内存，不需要源码或生成代码调用一个终结函数。

编译器根据 `T` 的布局计算对象大小、元素步长和对齐；对容器还需计算运行时容量
对应的字节数。runtime 管理原始存储，无需理解泛型 `T`。建议分工如下：

下表是参考实现的组织方式，不限制学生把更多操作封装成自定义 helper。

| 功能 | 编译器负责 | runtime 负责 |
| --- | --- | --- |
| Box 构造 | 求值、计算布局、初始化对象并构造返回值 | 按大小和对齐分配存储 |
| `Vec` 扩容 | 计算容量及字节数、维护指针/长度/容量、搬移有效元素 | 分配新缓冲区；旧缓冲区可保留到程序结束，可选提供扩容辅助函数 |
| 按值传递和赋值 | 决定何处发生 copy/move、保持值语义 | 可选提供字节复制辅助函数 |
| Box / Vec 的 Clone | 按具体 T 递归克隆，构造独立拥有存储；共享引用仍只复制引用 | 分配结果所需的原始存储 |
| 程序结束时回收堆 | 无需生成按类型递归析构或逐对象清理代码 | runtime 或执行环境统一回收该次执行的堆存储 |

`Vec` 扩容可由“分配新缓冲区、搬移有效元素、保留旧缓冲区到程序结束”实现，无需必做
`realloc`。搬移非 Copy 元素不等于调用 `.clone()`；原位置不再拥有该值。
允许使用 bump allocator。程序结束时由 runtime 或执行环境统一释放 arena、
重置该次执行的内存即可，无需遍历 Box / Vec 对象图、登记每个拥有对象或维护
析构用的所有权标记。可证明不影响可观察行为与合法引用的提前回收仍可作为优化。

即使不引入堆，数组和 struct 的复制或初始化也可能使用 `memcpy`、`memmove`、
`memset`。建议允许生成代码使用它们的标准 C ABI，或自行生成等价的访存代码。
这些操作只处理字节，不代替类型层面的 Clone。

容器内部布局、零容量表示和扩容因子由实现自行决定，不提供 capacity/reserve
等观察或控制接口；有效元素、move、Clone 和引用失效规则见堆章节。保留堆存储
不延长源码值或引用的有效期。测试必须能够在执行期间不回收的参考实现上完成，
按累计分配量、扩容和 allocator 开销核对资源，不能只按同时存活的数据量设计。
沿用有效测试不发生分配失败的前提，不要求分配大小的动态溢出检查或 panic。
零大小堆对象仍不在测试范围内；空 `Vec<T>`（其中 T 非零大小）是合法容器，
不得访问未初始化或不存在的元素存储。

REIMU 还提供标准 C ABI 的 `memcpy`、`memmove` 和 `memset`。生成代码可以调用它们，
也可以自行生成等价访存。`memcpy` 只用于已知不重叠区域，可能重叠时使用
`memmove`；任何一种都不能读取 padding／未初始化存储来代替类型层面的比较，
也不能把 move 变成浅复制出的两个 owner。这些 libc 调用及 `__rx_alloc` 的运行
成本按第 7 节计入 `Total cycles`。

### 4.2 参考 Vec 策略与资源保证

容器容量仍不是源码可观察行为。为统一测试规模和 runtime 验证，课程参考实现使用
下列基线：

1. 空 Vec 的 length/capacity 均为 0，不分配 buffer；
2. push 发现容量不足时，容量从 0 增为 4，之后每次翻倍；
3. 新 buffer 通过 `__rx_alloc` 分配，按顺序搬移 `[0, len)`，旧 buffer 保留到程序结束；
4. remove 不缩容；
5. Clone 空 Vec 时不分配，Clone 非空 Vec 时为恰好 len 个元素分配新 buffer；
6. Box 为一个 T 分配恰好 `size(T)` 字节；所有 size、capacity 和乘法结果在有效测试中可用 32 位 usize 表示。

以上是课程设计测试和提供参考实现时采用的资源基线，不使 capacity 成为源码接口，
也不强迫学生采用相同布局或增长因子。正式测试在锁定的 REIMU 上，按参考 runtime
和上述策略测得的累计 heap high-water（包括 malloc header、对齐 padding 和保留的
旧 Vec buffer）不超过 64 MiB。学生实现仍须在相同的 256 MiB 总内存和 1 MiB stack
配置下完成这些测试；明显更浪费空间的自定义布局或增长策略不改变源码程序的合法性。

64 MiB 是测试生成／审核余量，不是源语言可查询的堆大小，也不是要求一次申请必须
成功到该大小。`.data`、`.rodata`、`.bss`、text、stack 和 heap 共同受 REIMU 的
256 MiB 执行内存约束；仍不另设静态数据分区限额。发布时记录并验证参考 runtime、
REIMU commit 和这组完整参数。

## 5. 汇编产物与执行行为

编译器生成的汇编与所选 runtime 一起构成完整程序，必须能够按本文的工具链与
REIMU 配置构建并执行。源码入口正常结束时返回状态 0，输出遵守 I/O 接口规定的
字节序列；额外空白和换行也是可观察输出。

课程规范不限制单个源程序大小、编译时间、编译器峰值内存或生成汇编大小。
REIMU 使用 `--memory=256M --stack=1M`；不为 `.data`、
`.rodata` 和 `.bss` 另设静态数据限额。合法程序可以包含超出 12-bit immediate 的
常量和栈 offset，以及超出单条条件分支范围的控制流。

提交命令、阶段交付物和成绩计算由课程安排另行规定，不属于本语言与执行契约。

## 6. 代码生成与优化语义

合法执行的源码不会除零、计算 signed MIN / -1 或 MIN % -1、数组越界，或发生
已排除的引用／所有权违规。编译器不需要生成相应动态检查，也不要求实现 panic
或栈展开。普通整数回绕仍按已定义语义执行；整数 literal 超出最终类型范围、
let / 参数与可见的非限定 const 名称冲突均为课程 UB，所有测试均排除，不要求诊断。
最小负整数的合法 literal 形式仍受支持；普通静态类型错误仍按前端规则诊断。

上述前提针对源码语义。合法源码因错误翻译而非法访存、异常终止或输出错误，
仍不符合本契约。存储规模遵循允许的保守栈分配方案，不以精确栈槽复用或临时值
提前回收作为语言要求。

本规范不要求指定的优化算法。把所有局部值放在 stack slot 中是允许的实现；
实现仍须正确处理函数调用、有限的物理寄存器和合法控制流。寄存器分配、内联、
死代码消除、常量传播、尾递归消除和除法优化均不是强制的独立实现项目。

优化必须保持可观察行为与实际外部接口。不要求生成与参考编译器相同的 IR、
寄存器或汇编结构。

## 7. 执行成本口径

执行成本采用课程指定 REIMU 版本在默认权重下报告的 `Total cycles`，包括程序、
runtime 以及 REIMU 已计价的 libc 操作。不开启会改变默认成本模型的额外配置。
Clang 基线、跨测试聚合、成绩映射和排名规则属于课程评测安排，在修改测试点后
另行讨论，不纳入本规范。
