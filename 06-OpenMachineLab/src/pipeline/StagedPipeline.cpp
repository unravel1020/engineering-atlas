#include "pipeline/StagedPipeline.h"
#include <stdexcept>

namespace oml {
namespace pipeline {

StagedPipeline::StagedPipeline() = default;

StagedPipeline::~StagedPipeline() { stop(); }

void StagedPipeline::addStage(PipelineStagePtr stage) {
  if (!stage) {
    throw std::runtime_error("Cannot add null pipeline stage");
  }
  if (running_) {
    throw std::runtime_error("Cannot add stages after pipeline has started");
  }
  stages_.push_back(std::move(stage));
}

void StagedPipeline::start() {
  if (running_ || stages_.empty()) {
    return;
  }

  running_ = true;

  // N stages need N+1 queues: input queue, inter-stage queues, output queue.
  for (std::size_t i = 0; i <= stages_.size(); ++i) {
    queues_.push_back(std::make_unique<BlockingQueue<FrameDataPtr>>());
  }

  for (std::size_t i = 0; i < stages_.size(); ++i) {
    workers_.emplace_back([this, i]() {
      auto &input_queue = *queues_[i];
      auto &output_queue = *queues_[i + 1];
      auto &stage = *stages_[i];

      while (true) {
        auto opt = input_queue.pop();
        if (!opt.has_value()) {
          // Sentinel received: propagate sentinel and exit.
          output_queue.push(nullptr);
          break;
        }

        auto frame = opt.value();
        if (!frame) {
          // Sentinel frame: propagate and exit.
          output_queue.push(nullptr);
          break;
        }

        auto result = stage.process(frame);
        output_queue.push(result);
      }
    });
  }
}

void StagedPipeline::submit(FrameDataPtr input) {
  if (stopped_) {
    throw std::runtime_error("StagedPipeline has been stopped");
  }
  if (!running_) {
    start();
  }
  queues_.front()->push(std::move(input));
}

void StagedPipeline::stop() {
  if (!running_ || stopped_) {
    return;
  }

  stopped_ = true;

  // Push sentinel through the pipeline to drain workers.
  queues_.front()->push(nullptr);

  for (auto &worker : workers_) {
    if (worker.joinable()) {
      worker.join();
    }
  }

  // Drain any remaining results from the output queue.
  if (!queues_.empty()) {
    auto &output_queue = queues_.back();
    while (true) {
      auto opt = output_queue->pop();
      if (!opt.has_value()) {
        break;
      }
      auto frame = opt.value();
      if (!frame) {
        break;
      }
      std::lock_guard<std::mutex> lock(results_mutex_);
      results_.push_back(std::move(frame));
    }
  }

  // Stop all queues to wake any waiters.
  for (auto &queue : queues_) {
    queue->stop();
  }

  running_ = false;
}

std::vector<FrameDataPtr> StagedPipeline::collectResults() {
  std::lock_guard<std::mutex> lock(results_mutex_);
  std::vector<FrameDataPtr> out = std::move(results_);
  results_.clear();
  return out;
}

std::vector<FrameDataPtr>
StagedPipeline::runBatch(const std::vector<FrameDataPtr> &inputs) {
  for (const auto &input : inputs) {
    submit(input);
  }
  stop();

  auto results = collectResults();

  // Sort results by frame_id to match input order.
  std::sort(results.begin(), results.end(),
            [](const FrameDataPtr &a, const FrameDataPtr &b) {
              return a->frame_id < b->frame_id;
            });
  return results;
}

} // namespace pipeline
} // namespace oml
