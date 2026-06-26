# ADR 0004: 引入分级日志抽象

- 状态：Accepted
- 日期：2026-06-25
- 范围：`03-Cpp/Cpp-lab/modules/opencv_onnx_demo`

---

## 上下文

在 `opencv_onnx_demo` 中，诊断信息此前直接使用 `std::cout` 输出，例如：

- `main.cpp` 打印当前处理的模型名称
- `Inference` 加载模型时无显式提示
- 各种警告/错误通过 `std::cerr` 或异常处理，缺少统一分级

这种写法在 demo 阶段可行，但存在明显问题：

- 无法区分调试信息、运行信息、警告和错误
- 单元测试难以捕获和断言输出内容
- 向 `06-OpenMachineLab` 工业级版本迁移时，需要替换所有 `std::cout` 调用

---

## 决策

在 `opencv_onnx_demo` 内部引入一个轻量级、零外部依赖的分级日志模块 `logger`。

### 设计

- **四级日志**：`Debug` / `Info` / `Warning` / `Error`
- **单例 + 可切换输出流**：默认输出到 `std::cout`，测试可通过 `setOutput()` 重定向到 `std::ostringstream`
- **级别过滤**：通过 `setLevel()` 控制最小输出级别
- **线程安全**：使用 `std::mutex` 保护输出操作
- **宏封装**：提供 `LOG_DEBUG` / `LOG_INFO` / `LOG_WARNING` / `LOG_ERROR` 简化调用

### 使用边界

日志模块只用于诊断信息，**不替代**以下合法输出：

- `Result::print()`：任务结果输出，属于程序正常输出
- `benchmark::printBenchmarkResult()`：结构化 benchmark 报告
- `Inference::printModelInfo()`：模型元数据打印

这些函数继续保留 `std::ostream &` 参数接口。

### 集成点

- `main.cpp`：模型处理开始、图像缺失警告、可视化保存信息
- `inference.cpp`：模型加载成功提示
- `modelLoader.cpp`：未知张量类型警告
- `modelRegistry.cpp`：模型任务缺失警告

---

## 后果

### 正面

- 模块职责清晰：`logger` 专门负责诊断输出。
- 测试可观测：测试可以重定向输出流并断言日志内容。
- 向 `06-OpenMachineLab` 迁移时，日志接口已经抽象化，可替换为更复杂的日志后端。
- 新增 4 个 logger 单元测试，总测试数达到 48。

### 负面

- 单例模式在测试中需要手动重置输出流和级别，否则可能污染后续测试。
- 当前实现是同步阻塞输出，高并发场景下会成为瓶颈；工业级版本可能需要异步 sink。

---

## 备选方案

### 方案 A：复用 `Cpp-lab/modules/logger`

`Cpp-lab` 已有一个极简的 `Logger::log()`，但没有级别概念。

- 放弃原因：该模块能力太弱，无法满足分级需求；增强它会影响 Cpp-lab 其他模块。

### 方案 B：直接引入 spdlog / glog

使用成熟日志库。

- 放弃原因：增加外部依赖，与“保持构建环境零网络依赖”的目标冲突；当前阶段不需要如此重的日志框架。

### 方案 C：继续用 `std::cout` / `std::cerr`

不引入日志抽象。

- 放弃原因：无法分级过滤，测试难以断言，后续工业级迁移成本高。

---

## 相关文件

- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/include/logger.h`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/logger.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/tests/test_logger.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/app/main.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/inference.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/modelLoader.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/modelRegistry.cpp`
