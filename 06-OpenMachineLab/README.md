# OpenMachineLab

工业级机器视觉推理框架的架构原型。

`06-OpenMachineLab` 是 `opencv_onnx_demo` 的工业级演进目标。它继承 demo 中验证过的模块划分（预处理、推理、后处理、结果），并在此基础上引入：

- 插件化后端（CPU / GPU / TensorRT / ...）
- 统一的配置管理与生命周期管理
- 多线程 / 流水线调度能力
- 非侵入式性能统计与基准测试

---

## 当前阶段

已完成：

- ✅ 插件化后端接口 + ONNX Runtime CPU 实现
- ✅ TensorRT / GPU 后端占位（注册到后端注册表，待接入 SDK）
- ✅ 配置管理 + 生命周期管理（Application）
- ✅ 同步顺序流水线（SequentialPipeline）
- ✅ 线程池帧级并发流水线（ThreadPoolPipeline）
- ✅ 阶段级重叠流水线（StagedPipeline）
- ✅ 端到端分类 / 检测推理测试
- ✅ 可运行的图像/视频推理 Demo（oml_demo）
- ✅ 性能统计：Timer、TimingStage、BenchmarkRunner

当前测试覆盖：backend registry、ONNX backend、TensorRT 占位、ConfigManager、Application 生命周期、三种流水线端到端、Timer / TimingStage / Benchmark 统计。

## 目录结构

```
.
├── docs/adr/              # 架构决策记录
├── include/
│   ├── backend/           # 后端插件接口
│   ├── benchmark/         # 基准测试统计
│   ├── core/              # 配置、生命周期
│   ├── pipeline/          # 流水线调度抽象
│   └── utils/             # 工具（Timer 等）
├── src/                   # 实现
│   ├── backend/
│   ├── benchmark/
│   ├── core/
│   ├── demo/              # oml_demo 入口
│   ├── pipeline/
│   └── utils/
├── tests/                 # 测试
└── README.md
```

## 构建与测试

```bash
cd 06-OpenMachineLab
export ONNXRUNTIME_DIR=/path/to/onnxruntime-linux-x64-1.16.3
cmake -S . -B build
cmake --build build -j
./build/oml_tests
```

## 运行 Demo

处理单张图片：

```bash
./build/oml_demo \
  ../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json \
  yolov8n \
  ../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/yolov8n/test_data/dog.jpg
```

处理视频（可选输出到文件）：

```bash
./build/oml_demo \
  ../03-Cpp/Cpp-lab/modules/opencv_onnx_demo/models/registry.json \
  yolov8n \
  input.mp4 \
  output.mp4
```

## ADR 列表

- `0001-backend-plugin-interface.md`：插件化后端接口
- `0002-config-and-lifecycle-management.md`：配置与生命周期管理
- `0003-pipeline-scheduling.md`：流水线调度抽象
- `0004-benchmark-statistics.md`：性能统计与基准测试

## 与 opencv_onnx_demo 的关系

| 能力 | opencv_onnx_demo | OpenMachineLab |
|------|------------------|----------------|
| 任务支持 | classification / detection | 可扩展 |
| 后端 | ONNX Runtime CPU | 插件化多后端 |
| 配置 | registry.json + model.json | 全局 + 模型 + 后端三层 |
| 执行 | 同步串行 | 同步 + 线程池 + 阶段重叠 |
| 性能统计 | 端到端 benchmark | 端到端 + 每阶段 latency |
| 目标 | 学习 + 原型验证 | 工业级部署 |
