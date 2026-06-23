#pragma once

#include "inference.h"
#include "modelRegistry.h"
#include <memory>
#include <string>
#include <unordered_map>

// ModelManager is a factory and cache for Inference sessions. It loads models
// lazily from the registry so that only the models actually used are bound
// into ONNX Runtime memory.
class ModelManager {
public:
  explicit ModelManager(const std::string &registry_path);

  std::shared_ptr<Inference> load(const std::string &name);

  std::shared_ptr<Inference> get(const std::string &name) const;

  void select(const std::string &name);

  std::shared_ptr<Inference> current() const;

  std::vector<std::string> modelNames() const;

  bool hasModel(const std::string &name) const;

  std::string task(const std::string &name) const;

  std::vector<float> run(const cv::Mat &img);

  std::unordered_map<std::string, std::vector<float>>
  run(const std::unordered_map<std::string, cv::Mat> &input_images);

private:
  ModelRegistry registry_;

  std::unordered_map<std::string, std::shared_ptr<Inference>> models_;

  std::shared_ptr<Inference> current_;
};
