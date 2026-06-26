#pragma once

#include "pipeline/FrameData.h"
#include <functional>
#include <string>
#include <vector>

namespace oml {
namespace benchmark {

// Aggregated benchmark result for a pipeline run.
struct BenchmarkResult {
  size_t warmup_runs = 0;
  size_t total_runs = 0;

  double total_time_ms = 0.0;
  double mean_ms = 0.0;
  double min_ms = 0.0;
  double max_ms = 0.0;
  double p50_ms = 0.0;
  double p90_ms = 0.0;
  double p99_ms = 0.0;
  double throughput_fps = 0.0;
};

// BenchmarkRunner executes a pipeline repeatedly and produces latency
// percentiles and throughput. The pipeline is supplied as a callable that takes
// a FrameDataPtr and returns a FrameDataPtr, which matches the run() methods of
// SequentialPipeline, StagedPipeline, and ThreadPoolPipeline via lambdas.
class BenchmarkRunner {
public:
  using PipelineFn = std::function<oml::pipeline::FrameDataPtr(
      oml::pipeline::FrameDataPtr)>;
  using FrameSource = std::function<oml::pipeline::FrameDataPtr()>;

  // Run warmup_runs iterations followed by measured_runs iterations.
  // The frame_source callable is invoked before each iteration to supply input.
  static BenchmarkResult run(const PipelineFn &pipeline, const FrameSource &frame_source,
                             size_t warmup_runs, size_t measured_runs);

  // Convenience helper for SequentialPipeline-like objects with a run() method.
  template <typename Pipeline>
  static BenchmarkResult runPipeline(Pipeline &pipeline,
                                     const FrameSource &frame_source,
                                     size_t warmup_runs, size_t measured_runs) {
    return run(
        [&pipeline](oml::pipeline::FrameDataPtr input) {
          return pipeline.run(std::move(input));
        },
        frame_source, warmup_runs, measured_runs);
  }

  // Serialize a result to human-readable text.
  static std::string toString(const BenchmarkResult &result);
};

} // namespace benchmark
} // namespace oml
