#pragma once

#include "pipeline.h"
#include <chrono>
#include <functional>
#include <ostream>
#include <vector>

namespace benchmark {

struct BenchmarkResult {
  int warmup_runs = 0;
  int total_runs = 0;
  double mean_ms = 0.0;
  double min_ms = 0.0;
  double max_ms = 0.0;
  double p50_ms = 0.0;
  double p90_ms = 0.0;
  double p99_ms = 0.0;
  double throughput_fps = 0.0;
};

// Benchmark measures end-to-end latency by running a callable repeatedly.
// Warmup runs are excluded from statistics to reduce cold-start effects such
// as lazy memory allocation or first-run kernel initialization.
class Benchmark {
public:
  Benchmark() = default;

  void setWarmup(int warmup) { warmup_ = warmup; }

  void setRuns(int runs) { runs_ = runs; }

  BenchmarkResult run(const std::function<void()> &fn) const;

private:
  int warmup_ = 3;
  int runs_ = 10;
};

BenchmarkResult benchmarkPipeline(pipeline::Pipeline &pipe, const cv::Mat &img,
                                  int warmup, int runs);

void printBenchmarkResult(std::ostream &os, const BenchmarkResult &result);

} // namespace benchmark
