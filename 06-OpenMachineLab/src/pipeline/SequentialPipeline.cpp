#include "pipeline/SequentialPipeline.h"
#include "pipeline/TimingStage.h"
#include "utils/Timer.h"
#include <stdexcept>

namespace oml {
namespace pipeline {

void SequentialPipeline::addStage(PipelineStagePtr stage) {
  if (!stage) {
    throw std::runtime_error("Cannot add null pipeline stage");
  }
  if (profiling_enabled_) {
    stage = std::make_shared<TimingStage>(stage->name(), std::move(stage));
  }
  stages_.push_back(std::move(stage));
}

FrameDataPtr SequentialPipeline::run(FrameDataPtr input) {
  if (input && profiling_enabled_) {
    input->submit_time_us = utils::nowUs();
  }

  FrameDataPtr current = input;
  for (auto &stage : stages_) {
    if (!current) {
      return nullptr;
    }
    current = stage->process(current);
    if (!current) {
      return nullptr;
    }
  }

  if (current && profiling_enabled_) {
    current->complete_time_us = utils::nowUs();
  }
  return current;
}

} // namespace pipeline
} // namespace oml
