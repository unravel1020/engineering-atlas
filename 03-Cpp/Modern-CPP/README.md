# Modern-CPP

Engineering Atlas 的 Modern C++ 最小可运行示例集合。

本模块与 `Cpp-lab/opencv_onnx_demo` 形成互补：前者拆解现代 C++ 核心特性，后者展示这些特性在真实 AI 推理工程中的综合运用。

---

## 学习目标

- 理解并实践 AGENTS.md 中列出的 C++ 规则：RAII、智能指针、STL、const 正确性
- 掌握 C++17 常用特性，为阅读和维护 `opencv_onnx_demo` 代码做准备
- 每个示例独立可运行，便于调试和修改

---

## 目录结构

```
.
├── CMakeLists.txt
├── README.md
├── src/
│   ├── 01_raii.cpp              # RAII：对象生命周期管理资源
│   ├── 02_smart_pointers.cpp    # unique_ptr / shared_ptr / weak_ptr
│   ├── 03_stl_algorithms.cpp    # STL 容器与算法
│   ├── 04_const_correctness.cpp # const 成员函数、const 引用
│   ├── 05_move_semantics.cpp    # 移动语义与右值引用
│   ├── 06_lambda.cpp            # Lambda 表达式与 std::function
│   └── 07_cpp17_features.cpp    # 结构化绑定、optional、string_view
└── build/                       # 构建产物（被 .gitignore 忽略）
```

---

## 依赖

- CMake >= 3.16
- C++17 编译器

---

## 构建

```bash
cd 03-Cpp/Modern-CPP
cmake -S . -B build
cmake --build build -j
```

所有可执行文件会输出到 `build/` 目录。

---

## 运行

逐个运行示例：

```bash
./build/01_raii
./build/02_smart_pointers
./build/03_stl_algorithms
./build/04_const_correctness
./build/05_move_semantics
./build/06_lambda
./build/07_cpp17_features
```

也可以一键运行全部：

```bash
for exe in build/0*; do echo "=== $(basename $exe) ==="; $exe; done
```

---

## 示例概览

| 示例 | 主题 | 与 opencv_onnx_demo 的关联 |
|------|------|---------------------------|
| `01_raii` | 用对象生命周期管理文件句柄 | `Inference` 管理 ONNX Runtime 会话、模型资源 |
| `02_smart_pointers` | `unique_ptr/shared_ptr/weak_ptr` | `Pipeline` 使用 `shared_ptr<Inference>` 共享会话 |
| `03_stl_algorithms` | `sort/transform/accumulate/find_if` | `postProcessor` 中 softmax、topk、NMS 实现 |
| `04_const_correctness` | `const` 成员函数与引用 | `ModelInfo::name() const` 等只读接口 |
| `05_move_semantics` | 移动构造、移动赋值、`std::move` | 大对象（张量缓冲）的高效传递 |
| `06_lambda` | Lambda 捕获、泛型 lambda | 算法比较器、回调与异步任务 |
| `07_cpp17_features` | 结构化绑定、`std::optional`、`string_view` | 配置解析、可选返回值、只读字符串 |

---

## 学习建议

1. 按编号顺序阅读，每个示例聚焦一个知识点。
2. 每个 `.cpp` 文件头部都有注释说明核心要点。
3. 尝试修改示例中的参数，观察编译器和运行时的行为变化。
4. 学完本模块后，回到 `opencv_onnx_demo` 源码，识别这些特性是如何组合使用的。

---

## 设计原则

- **最小可运行**：每个示例只展示一个核心概念，无外部依赖。
- **独立构建**：每个 `.cpp` 单独编译为可执行文件，避免互相干扰。
- **Out-of-Source Build**：生成物统一放在 `build/`，不污染源码目录。
