# ADR 0005: 补强 Inference 与核心模块的错误处理

- 状态：Accepted
- 日期：2026-06-25
- 范围：`03-Cpp/Cpp-lab/modules/opencv_onnx_demo`

---

## 上下文

在 ADR 0003 中，我们已经统一了 `ModelLoader` 和 `ModelRegistry` 的配置解析错误处理。但 `Inference` 模块作为 ONNX Runtime 的封装层，仍然存在一些“静默失败”或“后知后觉”的风险：

1. **模型文件缺失**：`Ort::Session` 在模型文件不存在时会抛出异常，但异常信息来自 ONNX Runtime，对调用方不够友好。
2. **空输入图像**：`runWithInfo` 没有检查输入 `cv::Mat` 是否为空，空图像进入预处理可能产生难以调试的结果。
3. **输出张量访问**：`run(const cv::Mat &)` 直接通过 `operator[]` 访问输出 map，如果输出名不匹配会导致未定义行为。
4. **输出数据类型**：`Inference` 只支持 FP32 和 FP16 输出，其他类型会执行错误的指针转换。

---

## 决策

在 `Inference` 的关键边界增加显式校验，并为每种错误抛出带上下文的 `std::runtime_error`。

### 1. 构造期快速失败

在创建 `Ort::Session` 之前，先用 `std::filesystem::exists()` 检查模型文件是否存在。若不存在，抛出明确提示路径的异常，避免把 ONNX Runtime 的内部错误暴露给调用方。

### 2. 输入校验

`runWithInfo(const std::unordered_map<...> &)` 中，对每个输入张量：

- 检查是否缺失
- 检查 `cv::Mat` 是否为空

### 3. 输出访问安全化

`run(const cv::Mat &)` 中，将 `operator[]` 改为 `find()` 检查，输出名不存在时抛出异常。

### 4. 输出类型白名单

在解析 ONNX 输出时，只接受 `FLOAT` 和 `FLOAT16`，其他类型抛出 `Unsupported output element type` 异常。

### 5. 测试覆盖

新增 `test_inference.cpp`，覆盖：

- 模型文件缺失时的构造异常
- 空图像输入异常
- 空图像 map 输入异常
- `runWithInfo` 正常返回输出

---

## 后果

### 正面

- 错误信息更明确，调试成本降低。
- 潜在未定义行为（输出 map 越界、空图像预处理）被提前拦截。
- 测试数量从 48 增加到 52，`Inference` 模块有了独立单元测试。

### 负面

- 增加了一次文件系统检查，对构造期性能有微小影响；但相比 ONNX 模型加载本身可忽略。
- 更严格的校验意味着某些“碰巧能跑”的异常输入现在会失败；这是预期行为。

---

## 备选方案

### 方案 A：依赖 ONNX Runtime 自身的异常

不增加前置检查，让 `Ort::Session` 自己报错。

- 放弃原因：ONNX Runtime 的错误信息通常只包含路径，不够完整；提前检查可以给出更友好的上下文。

### 方案 B：返回错误码而非抛出异常

将错误处理改为返回 `std::optional` 或错误码。

- 放弃原因：当前项目已统一使用异常处理错误路径；引入混合风格会增加心智负担。

### 方案 C：在预处理阶段检查空图像

把空图像检查放到 `PreProcessor` 中。

- 放弃原因：空输入是调用契约问题，越早检查越好；在 `Inference::runWithInfo` 入口检查最清晰。

---

## 相关文件

- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/inference.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/tests/test_inference.cpp`
