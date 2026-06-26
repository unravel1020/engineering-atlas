#pragma once

#include "FrameData.h"
#include "IPipelineStage.h"
#include "SequentialPipeline.h"
#include "ThreadPool.h"
#include <future>
#include <memory>
#include <vector>

namespace oml {
namespace pipeline {

// ThreadPoolPipeline runs the same SequentialPipeline on multiple frames in
// parallel using a thread pool. This is the first step towards concurrency:
// frame-level parallelism, not yet stage-level pipeline overlapping.
class ThreadPoolPipeline {
public:
  explicit ThreadPoolPipeline(std::size_t num_threads);

  // Build the pipeline by adding stages. The stages are shared across worker
  // threads, so they must be thread-safe.
  void addStage(PipelineStagePtr stage);

  // Submit a frame for asynchronous processing. Returns a future to the result.
  std::future<FrameDataPtr> submit(FrameDataPtr input);

  // Process a batch of frames and return results in input order.
  std::vector<FrameDataPtr> runBatch(const std::vector<FrameDataPtr> &inputs);

  // Wait for all currently submitted tasks to complete.
  void waitForAll();

  const std::vector<PipelineStagePtr> &stages() const { return pipeline_.stages(); }

private:
  SequentialPipeline pipeline_;
  ThreadPool pool_;
};

} // namespace pipeline
} // namespace oml
