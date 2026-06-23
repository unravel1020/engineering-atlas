#include "benchmark.h"
#include "test_framework.h"
#include <thread>

TEST(benchmark_measures_runs_correctly) {
  benchmark::Benchmark bench;
  bench.setWarmup(2);
  bench.setRuns(5);

  int counter = 0;
  auto result = bench.run([&]() { ++counter; });

  ASSERT_EQ(result.warmup_runs, 2);
  ASSERT_EQ(result.total_runs, 5);
  ASSERT_EQ(counter, 7); // 2 warmup + 5 measured
  ASSERT_TRUE(result.mean_ms >= 0.0);
  ASSERT_TRUE(result.min_ms <= result.max_ms);
  ASSERT_TRUE(result.p50_ms >= result.min_ms && result.p50_ms <= result.max_ms);
}

TEST(benchmark_percentile_interpolation) {
  benchmark::Benchmark bench;
  bench.setWarmup(0);
  bench.setRuns(4);

  // Fixed durations controlled by counter to avoid timing flakiness.
  int counter = 0;
  auto result = bench.run([&]() {
    // Each call takes (counter + 1) ms deterministically.
    std::this_thread::sleep_for(std::chrono::milliseconds(counter + 1));
    ++counter;
  });

  ASSERT_EQ(result.total_runs, 4);
  ASSERT_TRUE(result.p50_ms >= result.min_ms);
  ASSERT_TRUE(result.p90_ms >= result.p50_ms);
  ASSERT_TRUE(result.p99_ms >= result.p90_ms);
}
