# ADR 0001: 插件化后端接口设计

- 状态：Proposed
- 日期：2026-06-25
- 范围：`06-OpenMachineLab`

---

## 上下文

`opencv_onnx_demo` 当前直接使用 ONNX Runtime CPU 后端，`Inference` 类与 ONNX Runtime 强耦合。工业级视觉系统通常需要：

- 在不同硬件上运行同一模型（CPU、GPU、NVIDIA TensorRT、Intel OpenVINO 等）
- 根据部署环境动态选择后端
- 未来接入自研推理引擎或国产化芯片

因此需要一个插件化的后端抽象层，把“模型加载 / 推理执行”与“具体硬件/框架实现”解耦。

---

## 决策

### 1. 抽象接口 `IBackend`

定义后端必须实现的最小接口：

- `initialize(config)`：后端初始化（选择设备、加载 runtime 等）
- `loadModel(model_path, model_config)`：加载模型并返回 `IModelSession` 句柄
- `getName()` / `getSupportedDevices()`：元信息查询
- `shutdown()`：资源释放

### 2. 模型会话接口 `IModelSession`

每次 `loadModel` 返回一个会话，负责一次推理的完整生命周期：

- `getInputInfos()` / `getOutputInfos()`：查询张量元数据
- `run(inputs)`：执行一次推理，返回输出张量
- `release()`：释放会话资源

### 3. 后端注册表 `BackendRegistry`

通过注册表在运行时 discovery 可用的后端：

- 内置 CPU / TensorRT 后端注册
- 未来可通过动态库扩展，当前阶段先静态注册

### 4. 与 `opencv_onnx_demo` 的关系

`opencv_onnx_demo` 中的 `Inference` 类可视为 `ONNXCPUBackend + ONNXModelSession` 的一个具体实现。`06-OpenMachineLab` 将其泛化为接口，当前先保留 ONNX Runtime 作为第一个后端实现，后续逐步添加 TensorRT 等。

---

## 后果

### 正面

- 新增后端不需要修改核心流水线代码。
- 测试可以在不同后端间复用同一套输入输出。
- 部署时根据硬件能力选择最优后端。

### 负面

- 抽象层会增加一定的接口适配代码。
- 不同后端的张量布局、精度、内存管理差异需要统一封装。

---

## 备选方案

### 方案 A：直接复用 `opencv_onnx_demo::Inference`

不抽象，直接在每个硬件分支写专门的推理类。

- 放弃原因：与工业级可扩展目标冲突，新增后端会复制大量流水线代码。

### 方案 B：动态库插件

每个后端编译为 `.so`，运行时加载。

- 放弃原因：当前阶段过于复杂；先使用静态注册表，未来再演进为动态插件。

---

## 相关文件

- `06-OpenMachineLab/include/backend/IBackend.h`
- `06-OpenMachineLab/include/backend/IModelSession.h`
- `06-OpenMachineLab/include/backend/BackendRegistry.h`
