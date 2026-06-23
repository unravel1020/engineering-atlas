#pragma once

#include "modelInfo.h"
#include <opencv2/opencv.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace result {

struct Detection {
  int class_id = 0;

  float confidence = 0.0f;

  float x1 = 0.0f;

  float y1 = 0.0f;

  float x2 = 0.0f;

  float y2 = 0.0f;
};

// Result provides a uniform interface for consuming task-specific outputs.
// The caller does not need to know whether the underlying task is
// classification or detection in order to print, visualize or save it.
class Result {
public:
  Result(const std::string &model_dir, const PostprocessConfig &postprocess)
      : model_dir_(model_dir), postprocess_(postprocess) {}

  virtual ~Result() = default;

  virtual void print(std::ostream &os) const = 0;

  virtual cv::Mat visualize(const cv::Mat &img) const = 0;

  virtual void save(const std::string &base_path,
                    const cv::Mat &img) const = 0;

protected:
  std::string model_dir_;

  PostprocessConfig postprocess_;
};

class ClassificationResult : public Result {
public:
  ClassificationResult(const std::string &model_dir,
                       const PostprocessConfig &postprocess,
                       std::vector<std::string> labels,
                       std::vector<float> probs, std::vector<int> indices)
      : Result(model_dir, postprocess), labels_(std::move(labels)),
        probs_(std::move(probs)), indices_(std::move(indices)) {}

  void print(std::ostream &os) const override;

  cv::Mat visualize(const cv::Mat &img) const override;

  void save(const std::string &base_path, const cv::Mat &img) const override;

  const std::vector<std::string> &labels() const { return labels_; }

  const std::vector<float> &probs() const { return probs_; }

  const std::vector<int> &indices() const { return indices_; }

private:
  std::vector<std::string> labels_;

  std::vector<float> probs_;

  std::vector<int> indices_;
};

class DetectionResult : public Result {
public:
  DetectionResult(const std::string &model_dir,
                  const PostprocessConfig &postprocess,
                  std::vector<std::string> labels,
                  std::vector<Detection> detections)
      : Result(model_dir, postprocess), labels_(std::move(labels)),
        detections_(std::move(detections)) {}

  void print(std::ostream &os) const override;

  cv::Mat visualize(const cv::Mat &img) const override;

  void save(const std::string &base_path, const cv::Mat &img) const override;

  const std::vector<std::string> &labels() const { return labels_; }

  const std::vector<Detection> &detections() const { return detections_; }

private:
  std::vector<std::string> labels_;

  std::vector<Detection> detections_;
};

} // namespace result
