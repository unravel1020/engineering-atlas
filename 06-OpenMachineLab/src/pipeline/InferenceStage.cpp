#include "pipeline/InferenceStage.h"
#include <stdexcept>

namespace oml {
namespace pipeline {

InferenceStage::InferenceStage(backend::ModelSessionPtr session)
    : session_(std::move(session)) {
  if (!session_) {
    throw std::runtime_error("InferenceStage received null session");
  }
}

FrameDataPtr InferenceStage::process(FrameDataPtr input) {
  if (!input) {
    return nullptr;
  }

  input->raw_outputs = session_->run(input->inputs);
  return input;
}

} // namespace pipeline
} // namespace oml
