#include "pipeline/TimingStage.h"
#include "utils/Timer.h"

namespace oml {
namespace pipeline {

TimingStage::TimingStage(std::string name, PipelineStagePtr inner)
    : name_(std::move(name)), inner_(std::move(inner)) {}

FrameDataPtr TimingStage::process(FrameDataPtr input) {
  if (!inner_) {
    return nullptr;
  }

  utils::ScopedTimer timer([this, &input](double duration_ms) {
    if (input) {
      input->stage_timings.push_back({name_, duration_ms});
    }
  });

  return inner_->process(input);
}

const char *TimingStage::name() const { return name_.c_str(); }

} // namespace pipeline
} // namespace oml
