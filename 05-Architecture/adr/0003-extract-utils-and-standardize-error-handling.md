# ADR 0003: 提取通用工具函数并统一错误处理

- 状态：Accepted
- 日期：2026-06-25
- 范围：`03-Cpp/Cpp-lab/modules/opencv_onnx_demo`

---

## 上下文

随着测试覆盖增加，`opencv_onnx_demo` 中暴露出一些架构层面的问题：

1. `utils.h` / `utils.cpp` 是空占位文件，没有实际职责，违背了“模块职责清晰”的原则。
2. `postProcessor.cpp` 和 `result.cpp` 各自有一个匿名命名空间内的 `loadLabels()` 函数，实现完全相同，违反了 AGENTS.md 中“避免重复代码”的规则。
3. `ModelLoader` 和 `ModelRegistry` 都直接操作 `std::ifstream` 和 `nlohmann::json`，错误处理风格不一致：
   - `ModelLoader` 已添加文件存在性、JSON 解析、字段存在性校验
   - `ModelRegistry` 仍缺少这些校验
4. 自研测试框架只有 `ASSERT_TRUE` / `ASSERT_EQ` / `ASSERT_NEAR` / `ASSERT_THROW`，写边界测试时不够直观（例如 `scale < 1.0f` 只能用 `ASSERT_TRUE`）。

---

## 决策

### 1. 赋予 `utils` 真实职责

将 `utils` 改造为头文件-only 的通用工具模块，提供：

- `readTextFile(path)`：读取整个文本文件
- `loadLabels(path)`：按行读取标签文件
- `joinPath(a, b)`：拼接路径

这些工具被 `ModelLoader`、`ModelRegistry`、`postProcessor` 共享。删除空占位 `utils.cpp`，模块变为纯头文件库，减少构建目标数量。

### 2. 消除重复的标签加载代码

移除 `postProcessor.cpp` 和 `result.cpp` 中各自的本地 `loadLabels()` 实现，统一使用 `utils::loadLabels()`。

同时从 `postProcessor.h` 中删除 `loadLabels()` 的公开声明，避免暴露内部工具函数。

### 3. 统一 `ModelRegistry` 的错误处理

参考 `ModelLoader` 的做法，`ModelRegistry::load()` 增加：

- 文件是否存在校验（通过 `utils::readTextFile` 自动抛出）
- JSON 解析异常捕获并包装为带上下文的 `std::runtime_error`
- `models` 字段存在性校验

模型目录拼接也改用 `utils::joinPath`，避免手动处理 `/` 分隔符。

### 4. 扩展测试框架断言

新增：

- `ASSERT_NE`
- `ASSERT_GT`
- `ASSERT_LT`
- `ASSERT_GE`
- `ASSERT_LE`

让测试代码能更直接地表达语义，例如 `ASSERT_LT(scale, 1.0f)` 比 `ASSERT_TRUE(scale < 1.0f)` 更易读，失败信息也更明确。

---

## 后果

### 正面

- `utils` 从空占位变成有明确职责的模块。
- 消除了一处重复代码（两个 `loadLabels` 实现）。
- `ModelLoader` 与 `ModelRegistry` 的错误处理风格一致，异常信息更友好。
- 测试框架更完善，后续添加边界测试更方便。
- 测试数量从 39 增加到 44，新增 `test_utils.cpp`。

### 负面

- `utils` 作为头文件-only 模块，所有使用方都会重新编译其内容；但工具函数很小，对编译时间影响可忽略。
- `utils` 目前只包含简单文件/路径工具，未来若变复杂可能需要拆分为更细粒度模块。

---

## 备选方案

### 方案 A：保持 `utils` 为空占位，重复代码继续保留

- 放弃原因：与 AGENTS.md 中“避免重复代码”“模块职责清晰”原则冲突。

### 方案 B：把 `loadLabels` 放到 `modelInfo.h` 或 `result.h`

- 放弃原因：`loadLabels` 是文件 IO 工具，不属于值对象或结果抽象，放到 `utils` 最自然。

### 方案 C：引入 `<filesystem>` 做完整路径规范化

- 放弃原因：当前只需要简单拼接，`<filesystem>` 会增加不必要的复杂度；`utils::joinPath` 已满足需求。

---

## 相关文件

- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/include/utils.h`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/postProcessor.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/result.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/modelLoader.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/modelRegistry.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/tests/test_framework.h`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/tests/test_utils.cpp`
