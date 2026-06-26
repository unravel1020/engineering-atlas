#include "pipeline/SequentialPipeline.h"
#include <stdexcept>

namespace oml {
namespace pipeline {

void SequentialPipeline::addStage(PipelineStagePtr stage) {
  if (!stage) {
    throw std::runtime_error("Cannot add null pipeline stage");
  }
  stages_.push_back(std::move(stage));
}

FrameDataPtr SequentialPipeline::run(FrameDataPtr input) {
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
  return current;
}

} // namespace pipeline
} // namespace oml
