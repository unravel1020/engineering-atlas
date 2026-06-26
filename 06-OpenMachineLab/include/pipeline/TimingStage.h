#pragma once

#include "FrameData.h"
#include "IPipelineStage.h"
#include <memory>
#include <string>

namespace oml {
namespace pipeline {

// TimingStage wraps an inner stage and records its execution time in the
// FrameData under stage_timings. It is used by pipelines when profiling is
// enabled, allowing latency measurements to be added without modifying concrete
// stage implementations.
class TimingStage : public IPipelineStage {
public:
  TimingStage(std::string name, PipelineStagePtr inner);

  FrameDataPtr process(FrameDataPtr input) override;
  const char *name() const override;

private:
  std::string name_;
  PipelineStagePtr inner_;
};

} // namespace pipeline
} // namespace oml
