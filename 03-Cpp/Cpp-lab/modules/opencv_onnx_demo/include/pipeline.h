#pragma once

#include "inference.h"
#include "modelInfo.h"
#include "result.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>

namespace pipeline {

class Pipeline {
public:
  Pipeline(std::shared_ptr<Inference> inference)
      : inference_(std::move(inference)) {}

  virtual ~Pipeline() = default;

  virtual std::unique_ptr<result::Result> run(const cv::Mat &img) = 0;

  const ModelInfo &modelInfo() const { return inference_->modelInfo(); }

  std::shared_ptr<Inference> inference() const { return inference_; }

protected:
  std::shared_ptr<Inference> inference_;
};

class ClassificationPipeline : public Pipeline {
public:
  using Pipeline::Pipeline;

  std::unique_ptr<result::Result> run(const cv::Mat &img) override;
};

class DetectionPipeline : public Pipeline {
public:
  using Pipeline::Pipeline;

  std::unique_ptr<result::Result> run(const cv::Mat &img) override;
};

class PipelineFactory {
public:
  static std::unique_ptr<Pipeline> create(
      const std::string &task, std::shared_ptr<Inference> inference);
};

} // namespace pipeline
