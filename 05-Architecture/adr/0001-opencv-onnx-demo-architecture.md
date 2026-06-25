# ADR 0001: opencv_onnx_demo 模块架构

- 状态：Accepted
- 日期：2026-06-25
- 范围：`03-Cpp/Cpp-lab/modules/opencv_onnx_demo`

---

## 上下文

`opencv_onnx_demo` 是 Engineering Atlas 中第一个端到端的 AI 推理学习项目，目标是在 C++ 环境里跑通 ONNX 模型的分类与检测推理。随着支持的模型从单一分类（SqueezeNet）扩展到检测（YOLOv8n），代码里开始出现任务相关的分支和硬编码常量，继续平铺扩展会导致：

- 每加一个新模型都要改 C++ 预处理/后处理代码
- 主程序里充斥着 `if (task == "classification")` 之类的分支
- 测试难以覆盖新增任务
- 模型路径、输入输出名、缩放参数散落各处

因此需要一次有意识的架构决策，把模块拆成可扩展、可测试、元数据驱动的形态。

---

## 决策

采用 **“元数据驱动 + 任务抽象 + 核心静态库”** 的三层架构。

### 1. 元数据驱动配置

每个模型的所有可变参数都放在 JSON 里：

- `models/registry.json`：模型名称 → 任务类型 + 目录
- `models/<name>/model.json`：输入输出张量、预处理参数、后处理参数

C++ 代码只负责解析和执行配置，不硬编码模型特定常量。这样新增模型时，只要准备目录和 JSON，无需重新编译主程序。

### 2. 任务抽象

- `Pipeline` 抽象类定义 `preprocess → inference → postprocess → result` 的统一流程。
- `ClassificationPipeline` 和 `DetectionPipeline` 分别实现具体任务。
- `PipelineFactory` 按 `task` 字符串创建对应实现。
- `Result` 同样按任务派生，统一提供 `print()` 和 `save()` 接口。

主程序只和 `PipelineFactory`、`ModelManager` 打交道，不直接感知任务差异。

### 3. 模型生命周期分层

- `ModelRegistry`：只读 `registry.json`，负责“有哪些模型”。
- `ModelLoader`：只读 `model.json`，负责“单个模型的元数据长什么样”。
- `ModelManager`：持有 `ModelRegistry`，按需懒加载 `Inference` 会话并缓存。
- `Inference`：只负责 ONNX Runtime 的前向推理，与任务无关。

这种分层让每一层的职责单一，也便于单元测试逐层覆盖。

### 4. 核心静态库

所有非 `main` 的源文件编译为静态库 `opencv_onnx_demo_core`，主程序和单元测试链接同一份库。避免测试与主程序出现实现分叉，也避免 `main()` 重复定义。

### 5. Out-of-Source 构建

所有生成物放在 `build/`，源码目录保持干净，与 AGENTS.md 中的 CMake 规则一致。

---

## 后果

### 正面

- **添加新模型成本低**：只需新增模型目录、JSON 和注册表条目。
- **主程序稳定**：新增任务类型时，只需扩展 `PipelineFactory` 和 `Result` 派生类。
- **可测试性强**：核心逻辑在静态库里，可以脱离 `main()` 单独测试。
- **职责清晰**：注册、加载、管理、推理四层分离，便于后续替换后端（如 GPU、TensorRT）。

### 负面

- **JSON 成为隐式接口**：JSON 字段名和结构与 C++ 解析代码强耦合，字段变更需要两端同步。
- **一定程度的过度设计**：如果只是跑通一个模型，抽象层会显得多余；但当模型数量 ≥ 2、任务类型 ≥ 2 时，收益明显。
- **运行时错误后移**：配置错误要到运行期才会暴露，需要在 `ModelLoader` 里做强校验和清晰报错。

---

## 备选方案

### 方案 A：所有参数硬编码在 C++ 里

每个模型写一组专用预处理/后处理函数，主程序直接调用。

- 放弃原因：加一个模型改一次代码，无法沉淀通用模式，与“可持续演进的知识体系”目标冲突。

### 方案 B：单一巨型管理类

用一个 `Demo` 类把注册、加载、推理、可视化全部包起来。

- 放弃原因：违反单一职责，测试和后续扩展困难，容易形成 God Object。

### 方案 C：为每个模型单独生成一个可执行文件

每个模型一个 `main.cpp` 和 CMake target。

- 放弃原因：大量重复代码，无法共享 Pipeline 和测试框架，与“避免重复代码”原则冲突。

---

## 相关文件

- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/README.md`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/CMakeLists.txt`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/include/pipeline.h`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/include/modelInfo.h`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/include/modelManager.h`
- `03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json`
