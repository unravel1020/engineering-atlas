#pragma once

#include "BlockingQueue.h"
#include "FrameData.h"
#include "IPipelineStage.h"
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace oml {
namespace pipeline {

// StagedPipeline runs each stage in its own worker thread, connected by bounded
// queues. This allows stage-level overlap: while frame N is in inference,
// frame N+1 can be preprocessed and frame N-1 can be postprocessed.
class StagedPipeline {
public:
  StagedPipeline();
  ~StagedPipeline();

  void addStage(PipelineStagePtr stage);

  // Submit a frame to the input queue. Non-blocking unless the queue is full.
  void submit(FrameDataPtr input);

  // Signal that no more frames will be submitted. After this, workers drain
  // remaining frames and stop.
  void stop();

  // Collect all available results. Call after stop() to retrieve outputs in
  // approximate completion order.
  std::vector<FrameDataPtr> collectResults();

  // Run a batch synchronously and return results in input order.
  std::vector<FrameDataPtr> runBatch(const std::vector<FrameDataPtr> &inputs);

  bool running() const { return running_; }

private:
  void start();

  std::vector<PipelineStagePtr> stages_;
  std::vector<std::unique_ptr<BlockingQueue<FrameDataPtr>>> queues_;
  std::vector<std::thread> workers_;
  std::vector<FrameDataPtr> results_;
  std::mutex results_mutex_;
  std::atomic<bool> running_{false};
  std::atomic<bool> stopped_{false};
};

} // namespace pipeline
} // namespace oml
