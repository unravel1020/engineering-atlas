# ADR 0004: 性能统计与基准测试

## 状态

已接受

## 背景

`opencv_onnx_demo` 已经提供了端到端 benchmark（warmup + measured runs + 分位数 + FPS）。在 `OpenMachineLab` 中，流水线被拆分为 `PreprocessStage`、`InferenceStage`、`PostprocessStage`，并且存在三种调度模式（Sequential、ThreadPool、StagedPipeline）。为了定位瓶颈，我们需要：

1. 测量每个阶段的耗时。
2. 在不影响正常推理的前提下，提供可开关的性能采集。
3. 复用 `opencv_onnx_demo` 的统计方法（mean/min/max/p50/p90/p99/FPS）。

## 决策

### 1. 非侵入式阶段计时：TimingStage 装饰器

不修改 `PreprocessStage` / `InferenceStage` / `PostprocessStage` 的实现，而是提供一个 `TimingStage` 装饰器：

- 实现 `IPipelineStage`。
- 在 `process()` 前后用 `utils::ScopedTimer` 计时。
- 将 `StageTiming{stage_name, duration_ms}` 写入 `FrameData::stage_timings`。

当 pipeline 启用 profiling 时，`addStage()` 自动把普通 stage 包装成 `TimingStage`。关闭 profiling 时无任何开销。

### 2. 端到端时间戳

`FrameData` 增加：

```cpp
uint64_t submit_time_us;   // 帧进入 pipeline 的时刻
uint64_t complete_time_us; // 帧离开 pipeline 的时刻
```

各 pipeline 在入口/出口处写入这两个字段。对于 `ThreadPoolPipeline`，在 worker lambda 中记录，反映实际执行耗时。

### 3. 独立的 Timer 工具

`utils::Timer.h` 提供：

- `Stopwatch`：显式 start + elapsedUs/elapsedMs。
- `ScopedTimer`：RAII，析构时回调耗时（毫秒）。
- `nowUs()`：当前微秒时间戳。

基于 `std::chrono::high_resolution_clock`，无外部依赖。

### 4. BenchmarkRunner 聚合统计

`benchmark::BenchmarkRunner` 接受一个 callable pipeline 和一个 frame source：

- 先执行 `warmup_runs` 次，不计入统计。
- 再执行 `measured_runs` 次，记录每次迭代耗时。
- 输出 `BenchmarkResult`：warmup_runs、total_runs、total_time_ms、mean/min/max/p50/p90/p99 ms、throughput_fps。

分位数采用与 `opencv_onnx_demo` 一致的最近秩线性插值。

## 后果

- **优点**：
  - 阶段耗时可见，便于定位瓶颈。
  - profiling 可开关，不影响生产路径。
  - 统计方法向后兼容 demo 项目。

- **缺点**：
  - `FrameData` 多了 profiling 字段，内存占用略有增加。
  - `TimingStage` 在 stage 内部串行执行，对极短 stage 有微小回调开销。

## 相关文件

- `include/utils/Timer.h`
- `include/pipeline/FrameData.h`
- `include/pipeline/TimingStage.h` / `src/pipeline/TimingStage.cpp`
- `include/benchmark/Benchmark.h` / `src/benchmark/Benchmark.cpp`
- `tests/test_benchmark.cpp`
