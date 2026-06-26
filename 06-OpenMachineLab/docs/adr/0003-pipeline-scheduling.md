# ADR 0003: 多线程 / 流水线调度抽象

- 状态：Proposed
- 日期：2026-06-25
- 范围：`06-OpenMachineLab`

---

## 上下文

`opencv_onnx_demo` 的 Pipeline 是同步串行的：

```
preprocess -> inference -> postprocess
```

在工业视觉场景中，通常需要：

- 并行处理多帧图像，提高吞吐
- 将预处理、推理、后处理拆分为独立 stage，允许流水线重叠
- 控制并发度，避免 GPU/CPU 资源冲突
- 支持同步（单帧）和异步（批量/流式）两种模式

---

## 决策

### 1. Stage 抽象 `IPipelineStage`

把 Pipeline 拆分为三个可独立执行的 Stage：

- `PreprocessStage`
- `InferenceStage`
- `PostprocessStage`

每个 Stage 实现统一接口：

```cpp
class IPipelineStage {
public:
  virtual ~IPipelineStage() = default;
  virtual std::shared_ptr<FrameData> process(std::shared_ptr<FrameData> input) = 0;
  virtual const char *name() const = 0;
};
```

### 2. 数据载体 `FrameData`

一帧数据在 Stage 间传递：

- `frame_id`
- 原始图像
- 预处理后的张量
- 推理输出
- 最终结果
- 元数据（时间戳、来源等）

### 3. 两种执行模式

#### 模式 A：同步顺序执行（SequentialPipeline）

与 `opencv_onnx_demo` 行为一致，适合简单场景和测试。

#### 模式 B：线程池流水线（ThreadPoolPipeline）

每个 Stage 有独立线程池，通过队列传递 `FrameData`。当前阶段只实现接口和单线程池顺序调度，未来再扩展为真流水线。

### 4. 线程池 `ThreadPool`

- 固定大小线程池
- 提交任务并返回 `std::future`
- 支持优雅关闭

### 5. 调度策略

当前先实现最简单的策略：

- 所有帧按顺序经过完整 Pipeline
- 多帧之间可以并发（帧级并行）
- Stage 之间暂时不重叠（避免过早复杂化）

后续可演进为：

- Stage 级流水线重叠
- 批量推理（batching）
- 优先级调度

---

## 后果

### 正面

- Pipeline 可扩展为并发执行，提升吞吐。
- Stage 抽象便于替换实现（例如用 GPU 做预处理、用 NPU 做推理）。
- 同步模式保留，便于测试和调试。

### 负面

- 引入线程安全要求，Stage 实现必须是无状态或线程安全的。
- 帧顺序、资源竞争、背压等问题需要在后续版本中处理。

---

## 备选方案

### 方案 A：每个任务一个独立线程

为每个推理请求创建 `std::thread`。

- 放弃原因：创建/销毁线程开销大，无法控制并发度。

### 方案 B：直接依赖 ONNX Runtime 的并行能力

不自己实现多线程，完全依赖后端内部并行。

- 放弃原因：无法利用帧级并行，预处理/后处理仍串行，吞吐受限。

### 方案 C：使用第三方调度库（如 Intel TBB）

引入成熟并行框架。

- 放弃原因：增加外部依赖，与学习阶段“保持简单”的目标冲突。

---

## 相关文件

- `06-OpenMachineLab/include/pipeline/IPipelineStage.h`
- `06-OpenMachineLab/include/pipeline/FrameData.h`
- `06-OpenMachineLab/include/pipeline/SequentialPipeline.h`
- `06-OpenMachineLab/include/pipeline/ThreadPool.h`
