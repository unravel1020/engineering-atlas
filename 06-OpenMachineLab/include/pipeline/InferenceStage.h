#pragma once

#include "IPipelineStage.h"
#include "backend/IModelSession.h"

namespace oml {
namespace pipeline {

// InferenceStage runs the backend model session and stores raw outputs in
// FrameData. It is backend-agnostic and depends only on IModelSession.
class InferenceStage : public IPipelineStage {
public:
  explicit InferenceStage(backend::ModelSessionPtr session);

  FrameDataPtr process(FrameDataPtr input) override;
  const char *name() const override { return "InferenceStage"; }

private:
  backend::ModelSessionPtr session_;
};

} // namespace pipeline
} // namespace oml
