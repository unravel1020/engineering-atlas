# ADR 0002: 在迁移到 OpenMachineLab 之前先固化测试与架构

- 状态：Accepted
- 日期：2026-06-25
- 范围：`03-Cpp/Cpp-lab/modules/opencv_onnx_demo` → `06-OpenMachineLab`

---

## 上下文

`opencv_onnx_demo` 已经实现了元数据驱动的分类 / 检测推理流程，但 `06-OpenMachineLab` 定位为正式工业级别版本。在把 demo 代码迁移到工业级框架之前，需要先回答一个问题：**当前架构是否经得起真实场景的考验？**

工业级版本与 demo 的核心差异不在于功能多寡，而在于：

- 可维护性：模块职责是否清晰
- 可测试性：是否有足够的单元测试和集成测试覆盖
- 可观测性：错误是否能被快速定位
- 可扩展性：新增任务、模型、后端是否不需要破坏现有代码

当前 demo 存在以下明显缺口：

- 部分模块（如 `Result`）没有单元测试
- 异常路径覆盖不足（缺失 `model.json`、格式错误等）
- `ModelLoader` 对非法输入的报错信息不够明确
- `utils` 是空占位文件，没有实际职责
- 缺少端到端集成测试验证真实模型推理链路

---

## 决策

**在创建 `06-OpenMachineLab` 之前，先把 `opencv_onnx_demo` 的测试覆盖率和错误处理补到当前架构的“稳定边界”。**

### 1. 测试策略

采用两层测试结构：

- **单元测试**：每个模块独立测试，覆盖正常路径、边界路径和异常路径。
- **集成测试**：使用仓库内置的真实 ONNX 模型（SqueezeNet、YOLOv8n）跑通完整 Pipeline，验证模块组合后的行为。

测试框架继续沿用项目自研的轻量框架，不引入 GTest/Catch2 等外部依赖，保持构建环境零网络依赖。

### 2. 错误处理补强

- `ModelLoader` 增加显式校验：文件是否存在、JSON 是否可解析、`inputs`/`outputs` 字段是否存在。
- 将 JSON 解析异常包装为带上下文的 `std::runtime_error`，便于定位问题。
- 修复 `softmax` 空输入时的未定义行为（`std::max_element` 对空区间解引用）。

### 3. 架构边界确认

- 保留现有 `ModelRegistry → ModelManager → Inference → Pipeline → Result` 分层。
- 不提前设计插件系统、服务化、多线程调度等工业级特性。
- 把当前架构作为 `06-OpenMachineLab` 的“可验证原型”，而不是直接照搬。

---

## 后果

### 正面

- 测试从 20 个增加到 39 个，覆盖了 `Result`、预处理/后处理边界、模型加载异常和端到端集成。
- 潜在崩溃问题（空输入 softmax）在迁移前被发现并修复。
- 为 `06-OpenMachineLab` 提供了经过验证的模块划分参考。

### 负面

- 自研测试框架能力有限，无法提供参数化测试、测试夹具等高级特性；未来工业级版本需要评估是否引入 GTest。
- 集成测试依赖真实模型文件，CI 环境需要保证模型文件存在。

---

## 备选方案

### 方案 A：直接开始构建 `06-OpenMachineLab`

把 demo 代码复制一份到 `06-OpenMachineLab`，在其上做工业级改造。

- 放弃原因：demo 中的隐藏问题（如空输入 softmax）会一起迁移；没有先验证当前架构的稳定性。

### 方案 B：引入 GTest 等外部测试框架

用成熟的测试框架替换自研框架，获得更丰富的断言和测试组织能力。

- 放弃原因：当前阶段以学习核心 C++ 工程能力为主，外部依赖会增加构建复杂度；自研框架已满足最小测试需求。

---

## 相关文件

- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/tests/`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/modelLoader.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/src/postProcessor.cpp`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/README.md`
