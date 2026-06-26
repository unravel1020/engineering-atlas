#pragma once

#include "IPipelineStage.h"
#include "core/ModelConfig.h"
#include <string>
#include <vector>

namespace oml {
namespace pipeline {

// PostprocessStage parses raw outputs into a human-readable result stored in
// FrameData::result_json. Current implementation supports classification and
// detection, mirroring opencv_onnx_demo capabilities.
class PostprocessStage : public IPipelineStage {
public:
  PostprocessStage(const core::ModelConfig &model_config,
                   const std::string &model_dir);

  FrameDataPtr process(FrameDataPtr input) override;
  const char *name() const override { return "PostprocessStage"; }

private:
  FrameDataPtr processClassification(FrameDataPtr input);
  FrameDataPtr processDetection(FrameDataPtr input);

  core::ModelConfig model_config_;
  std::string model_dir_;
  std::vector<std::string> labels_;
};

} // namespace pipeline
} // namespace oml
