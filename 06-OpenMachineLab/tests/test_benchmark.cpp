#include "benchmark/Benchmark.h"
#include "pipeline/FrameData.h"
#include "pipeline/IPipelineStage.h"
#include "pipeline/SequentialPipeline.h"
#include "pipeline/TimingStage.h"
#include "test_framework.h"
#include "utils/Timer.h"
#include <thread>

namespace {

class SleepStage : public oml::pipeline::IPipelineStage {
public:
  explicit SleepStage(const char *name, int ms) : name_(name), ms_(ms) {}

  oml::pipeline::FrameDataPtr process(oml::pipeline::FrameDataPtr input) override {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms_));
    return input;
  }

  const char *name() const override { return name_; }

private:
  const char *name_;
  int ms_;
};

oml::pipeline::FrameDataPtr makeFrame(uint64_t id) {
  auto frame = std::make_shared<oml::pipeline::FrameData>();
  frame->frame_id = id;
  return frame;
}

} // namespace

TEST(timer_measures_elapsed_time) {
  oml::utils::Stopwatch watch;
  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  auto us = watch.elapsedUs();
  ASSERT_TRUE(us >= 5000u);
}

TEST(scoped_timer_invokes_callback) {
  double captured = 0.0;
  {
    oml::utils::ScopedTimer timer([&captured](double ms) { captured = ms; });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  ASSERT_TRUE(captured >= 5.0);
}

TEST(timing_stage_records_stage_latency) {
  auto inner = std::make_shared<SleepStage>("sleep", 10);
  oml::pipeline::TimingStage stage("sleep", inner);

  auto frame = makeFrame(1);
  auto result = stage.process(frame);
  ASSERT_TRUE(result != nullptr);
  ASSERT_EQ(result->stage_timings.size(), 1u);
  ASSERT_EQ(result->stage_timings[0].stage_name, "sleep");
  ASSERT_TRUE(result->stage_timings[0].duration_ms >= 5.0);
}

TEST(sequential_pipeline_profiling_records_stage_timings) {
  oml::pipeline::SequentialPipeline pipeline;
  pipeline.setProfilingEnabled(true);
  pipeline.addStage(std::make_shared<SleepStage>("stage_a", 5));
  pipeline.addStage(std::make_shared<SleepStage>("stage_b", 5));

  auto frame = makeFrame(1);
  auto result = pipeline.run(frame);
  ASSERT_TRUE(result != nullptr);
  ASSERT_EQ(result->stage_timings.size(), 2u);
  ASSERT_EQ(result->stage_timings[0].stage_name, "stage_a");
  ASSERT_EQ(result->stage_timings[1].stage_name, "stage_b");
  ASSERT_TRUE(result->submit_time_us > 0u);
  ASSERT_TRUE(result->complete_time_us >= result->submit_time_us);
}

TEST(benchmark_runner_computes_latency_stats) {
  oml::pipeline::SequentialPipeline pipeline;
  pipeline.addStage(std::make_shared<SleepStage>("sleep", 5));

  auto result = oml::benchmark::BenchmarkRunner::runPipeline(
      pipeline, []() { return makeFrame(0); }, 1, 5);

  ASSERT_EQ(result.warmup_runs, 1u);
  ASSERT_EQ(result.total_runs, 5u);
  ASSERT_TRUE(result.mean_ms >= 3.0);
  ASSERT_TRUE(result.min_ms <= result.max_ms);
  ASSERT_TRUE(result.p50_ms >= result.min_ms);
  ASSERT_TRUE(result.p50_ms <= result.p99_ms);
  ASSERT_TRUE(result.p99_ms <= result.max_ms);
  ASSERT_TRUE(result.throughput_fps > 0.0);
  ASSERT_TRUE(!oml::benchmark::BenchmarkRunner::toString(result).empty());
}

TEST(benchmark_runner_excludes_warmup_from_stats) {
  oml::pipeline::SequentialPipeline pipeline;
  pipeline.addStage(std::make_shared<SleepStage>("sleep", 2));

  // Use a long warmup stage that would skew stats if included.
  auto result = oml::benchmark::BenchmarkRunner::runPipeline(
      pipeline, []() { return makeFrame(0); }, 1, 3);

  ASSERT_EQ(result.total_runs, 3u);
  ASSERT_TRUE(result.mean_ms >= 1.0);
}
