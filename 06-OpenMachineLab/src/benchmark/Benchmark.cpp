#include "benchmark/Benchmark.h"
#include "utils/Timer.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace oml {
namespace benchmark {

namespace {

// Compute percentile using linear interpolation between nearest ranks.
// Values must be sorted in ascending order.
double percentile(const std::vector<double> &sorted_values, double p) {
  if (sorted_values.empty()) {
    return 0.0;
  }
  if (sorted_values.size() == 1) {
    return sorted_values[0];
  }

  const double rank = p / 100.0 * (static_cast<double>(sorted_values.size()) - 1.0);
  const size_t lower = static_cast<size_t>(std::floor(rank));
  const size_t upper = static_cast<size_t>(std::ceil(rank));
  const double frac = rank - static_cast<double>(lower);

  if (lower >= sorted_values.size()) {
    return sorted_values.back();
  }
  if (upper >= sorted_values.size()) {
    return sorted_values[lower];
  }

  return sorted_values[lower] * (1.0 - frac) + sorted_values[upper] * frac;
}

} // namespace

BenchmarkResult BenchmarkRunner::run(const PipelineFn &pipeline,
                                     const FrameSource &frame_source,
                                     size_t warmup_runs,
                                     size_t measured_runs) {
  if (!pipeline || !frame_source) {
    throw std::runtime_error("BenchmarkRunner requires a pipeline and frame source");
  }

  BenchmarkResult result;
  result.warmup_runs = warmup_runs;
  result.total_runs = measured_runs;

  // Warmup iterations are not measured.
  for (size_t i = 0; i < warmup_runs; ++i) {
    auto frame = frame_source();
    pipeline(std::move(frame));
  }

  std::vector<double> latencies;
  latencies.reserve(measured_runs);

  utils::Stopwatch total_watch;
  for (size_t i = 0; i < measured_runs; ++i) {
    auto frame = frame_source();
    utils::Stopwatch iter_watch;
    auto output = pipeline(std::move(frame));
    (void)output;
    latencies.push_back(iter_watch.elapsedMs());
  }
  result.total_time_ms = total_watch.elapsedMs();

  if (!latencies.empty()) {
    std::sort(latencies.begin(), latencies.end());
    result.min_ms = latencies.front();
    result.max_ms = latencies.back();
    result.mean_ms = result.total_time_ms / static_cast<double>(latencies.size());
    result.p50_ms = percentile(latencies, 50.0);
    result.p90_ms = percentile(latencies, 90.0);
    result.p99_ms = percentile(latencies, 99.0);
    if (result.mean_ms > 0.0) {
      result.throughput_fps = 1000.0 / result.mean_ms;
    }
  }

  return result;
}

std::string BenchmarkRunner::toString(const BenchmarkResult &result) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(3);
  oss << "Benchmark Result\n";
  oss << "  warmup runs: " << result.warmup_runs << "\n";
  oss << "  measured runs: " << result.total_runs << "\n";
  oss << "  total time: " << result.total_time_ms << " ms\n";
  oss << "  mean: " << result.mean_ms << " ms\n";
  oss << "  min: " << result.min_ms << " ms\n";
  oss << "  max: " << result.max_ms << " ms\n";
  oss << "  p50: " << result.p50_ms << " ms\n";
  oss << "  p90: " << result.p90_ms << " ms\n";
  oss << "  p99: " << result.p99_ms << " ms\n";
  oss << "  throughput: " << result.throughput_fps << " FPS\n";
  return oss.str();
}

} // namespace benchmark
} // namespace oml
