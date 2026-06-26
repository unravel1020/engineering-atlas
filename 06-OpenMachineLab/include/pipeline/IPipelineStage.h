#pragma once

#include "FrameData.h"
#include <memory>

namespace oml {
namespace pipeline {

// A pipeline stage transforms a FrameData into the next state.
// Stages must be thread-safe because they may be executed concurrently in a
// thread pool.
class IPipelineStage {
public:
  virtual ~IPipelineStage() = default;

  // Process one frame. Returning nullptr signals a fatal error for this frame.
  virtual FrameDataPtr process(FrameDataPtr input) = 0;

  // Human-readable stage name for logging and profiling.
  virtual const char *name() const = 0;
};

using PipelineStagePtr = std::shared_ptr<IPipelineStage>;

} // namespace pipeline
} // namespace oml
