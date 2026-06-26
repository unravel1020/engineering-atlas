#include "pipeline/ThreadPoolPipeline.h"
#include <stdexcept>

namespace oml {
namespace pipeline {

ThreadPoolPipeline::ThreadPoolPipeline(std::size_t num_threads)
    : pool_(num_threads) {
  if (num_threads == 0) {
    throw std::runtime_error("ThreadPoolPipeline requires at least one thread");
  }
}

void ThreadPoolPipeline::addStage(PipelineStagePtr stage) {
  pipeline_.addStage(std::move(stage));
}

std::future<FrameDataPtr> ThreadPoolPipeline::submit(FrameDataPtr input) {
  return pool_.submit(
      [this, input]() mutable { return pipeline_.run(std::move(input)); });
}

std::vector<FrameDataPtr>
ThreadPoolPipeline::runBatch(const std::vector<FrameDataPtr> &inputs) {
  std::vector<std::future<FrameDataPtr>> futures;
  futures.reserve(inputs.size());
  for (const auto &input : inputs) {
    futures.push_back(submit(input));
  }

  std::vector<FrameDataPtr> results;
  results.reserve(inputs.size());
  for (auto &f : futures) {
    results.push_back(f.get());
  }
  return results;
}

void ThreadPoolPipeline::waitForAll() { pool_.waitForAll(); }

} // namespace pipeline
} // namespace oml
