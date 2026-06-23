#include "benchmark.h"
#include <algorithm>
#include <iomanip>
#include <numeric>

namespace benchmark {

namespace {

// Linear interpolation between the two closest ranks. This matches the common
// definition of percentile used by load-testing tools and avoids bias from
// always rounding down to the nearest sample.
double percentile(const std::vector<double> &sorted_ms, double p) {
  if (sorted_ms.empty()) {
    return 0.0;
  }
  if (sorted_ms.size() == 1) {
    return sorted_ms[0];
  }

  double pos = p * (sorted_ms.size() - 1);
  size_t lower = static_cast<size_t>(std::floor(pos));
  size_t upper = static_cast<size_t>(std::ceil(pos));
  double frac = pos - lower;

  return sorted_ms[lower] * (1.0 - frac) + sorted_ms[upper] * frac;
}

} // namespace

BenchmarkResult Benchmark::run(const std::function<void()> &fn) const {
  BenchmarkResult result;
  result.warmup_runs = warmup_;
  result.total_runs = runs_;

  for (int i = 0; i < warmup_; ++i) {
    fn();
  }

  std::vector<double> times_ms;
  times_ms.reserve(runs_);

  for (int i = 0; i < runs_; ++i) {
    auto start = std::chrono::high_resolution_clock::now();
    fn();
    auto end = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    times_ms.push_back(ms);
  }

  std::sort(times_ms.begin(), times_ms.end());

  result.min_ms = times_ms.front();
  result.max_ms = times_ms.back();
  result.p50_ms = percentile(times_ms, 0.50);
  result.p90_ms = percentile(times_ms, 0.90);
  result.p99_ms = percentile(times_ms, 0.99);
  result.mean_ms =
      std::accumulate(times_ms.begin(), times_ms.end(), 0.0) / times_ms.size();
  result.throughput_fps = (result.mean_ms > 0.0) ? 1000.0 / result.mean_ms : 0.0;

  return result;
}

BenchmarkResult benchmarkPipeline(pipeline::Pipeline &pipe, const cv::Mat &img,
                                  int warmup, int runs) {
  Benchmark bench;
  bench.setWarmup(warmup);
  bench.setRuns(runs);
  return bench.run([&]() {
    // Materialize the result to ensure post-processing is included in the
    // measured latency, not just raw inference.
    auto result = pipe.run(img);
    (void)result;
  });
}

void printBenchmarkResult(std::ostream &os, const BenchmarkResult &result) {
  os << "Benchmark (warmup=" << result.warmup_runs
     << ", runs=" << result.total_runs << ")\n";
  os << std::fixed << std::setprecision(3);
  os << "  mean: " << result.mean_ms << " ms\n";
  os << "  min:  " << result.min_ms << " ms\n";
  os << "  max:  " << result.max_ms << " ms\n";
  os << "  p50:  " << result.p50_ms << " ms\n";
  os << "  p90:  " << result.p90_ms << " ms\n";
  os << "  p99:  " << result.p99_ms << " ms\n";
  os << "  throughput: " << result.throughput_fps << " fps\n";
}

} // namespace benchmark
