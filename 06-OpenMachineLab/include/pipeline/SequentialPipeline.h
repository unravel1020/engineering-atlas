#pragma once

#include "IPipelineStage.h"
#include <vector>

namespace oml {
namespace pipeline {

// SequentialPipeline runs stages one after another on the calling thread.
// It matches the behavior of opencv_onnx_demo::Pipeline and is used for
// testing, debugging, and simple deployments.
class SequentialPipeline {
public:
  void addStage(PipelineStagePtr stage);

  // Process a single frame synchronously. Returns nullptr if any stage fails.
  FrameDataPtr run(FrameDataPtr input);

  // When enabled, addStage wraps each stage with TimingStage and run() records
  // submit/complete timestamps. Enable before adding stages.
  void setProfilingEnabled(bool enabled) { profiling_enabled_ = enabled; }

  const std::vector<PipelineStagePtr> &stages() const { return stages_; }

private:
  std::vector<PipelineStagePtr> stages_;
  bool profiling_enabled_ = false;
};

} // namespace pipeline
} // namespace oml
