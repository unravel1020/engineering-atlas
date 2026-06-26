# OpenMachineLab

工业级机器视觉推理框架的架构原型。

`06-OpenMachineLab` 是 `opencv_onnx_demo` 的工业级演进目标。它继承 demo 中验证过的模块划分（预处理、推理、后处理、结果），并在此基础上引入：

- 插件化后端（CPU / GPU / TensorRT / ...）
- 统一的配置管理与生命周期管理
- 多线程 / 流水线调度能力

---

## 当前阶段

当前处于**架构设计阶段**，目录中以 ADR 和头文件骨架为主，尚未实现完整可运行版本。

## 目录结构

```
.
├── docs/adr/              # 架构决策记录
├── include/
│   ├── backend/           # 后端插件接口
│   ├── core/              # 配置、生命周期
│   └── pipeline/          # 流水线调度抽象
├── src/                   # 实现（待填充）
├── tests/                 # 测试（待填充）
└── README.md
```

## ADR 列表

- `0001-backend-plugin-interface.md`：插件化后端接口
- `0002-config-and-lifecycle-management.md`：配置与生命周期管理
- `0003-pipeline-scheduling.md`：流水线调度抽象

## 与 opencv_onnx_demo 的关系

| 能力 | opencv_onnx_demo | OpenMachineLab |
|------|------------------|----------------|
| 任务支持 | classification / detection | 可扩展 |
| 后端 | ONNX Runtime CPU | 插件化多后端 |
| 配置 | registry.json + model.json | 全局 + 模型 + 后端三层 |
| 执行 | 同步串行 | 同步 + 线程池 |
| 目标 | 学习 + 原型验证 | 工业级部署 |
