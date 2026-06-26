#pragma once

#include "IPipelineStage.h"
#include "core/ModelConfig.h"

namespace oml {
namespace pipeline {

// PreprocessStage converts the original image in FrameData into the input
// tensors expected by the backend. It uses the per-model PreprocessConfig.
class PreprocessStage : public IPipelineStage {
public:
  explicit PreprocessStage(const core::ModelConfig &model_config);

  FrameDataPtr process(FrameDataPtr input) override;
  const char *name() const override { return "PreprocessStage"; }

private:
  core::ModelConfig model_config_;
};

} // namespace pipeline
} // namespace oml
