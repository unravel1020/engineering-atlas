#include "pipeline.h"
#include "postProcessor.h"
#include <stdexcept>

namespace pipeline {

std::unique_ptr<result::Result> ClassificationPipeline::run(const cv::Mat &img) {
  auto inference_result = inference_->runWithInfo(img);
  const auto &model_info = inference_->modelInfo();
  auto output = inference_result.outputs.at(model_info.outputs()[0].name);

  return std::make_unique<result::ClassificationResult>(
      postProcessor::classify(output, model_info.postprocess(),
                              model_info.modelDir()));
}

std::unique_ptr<result::Result> DetectionPipeline::run(const cv::Mat &img) {
  auto inference_result = inference_->runWithInfo(img);
  const auto &model_info = inference_->modelInfo();
  auto output = inference_result.outputs.at(model_info.outputs()[0].name);

  return std::make_unique<result::DetectionResult>(postProcessor::detect(
      output, model_info.postprocess(), model_info.modelDir(),
      inference_result.preprocess));
}

std::unique_ptr<Pipeline>
PipelineFactory::create(const std::string &task,
                        std::shared_ptr<Inference> inference) {
  if (task == "classification") {
    return std::make_unique<ClassificationPipeline>(std::move(inference));
  }
  if (task == "detection") {
    return std::make_unique<DetectionPipeline>(std::move(inference));
  }
  throw std::runtime_error("Unsupported task for pipeline: " + task);
}

} // namespace pipeline
