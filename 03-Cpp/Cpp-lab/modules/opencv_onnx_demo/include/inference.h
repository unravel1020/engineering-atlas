#pragma once

#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct TensorInfo {
  std::string name;

  std::vector<int64_t> shape;

  ONNXTensorElementDataType type;
};

class Inference {
public:
  Inference(const std::string &model_path);

  std::vector<float> run(const cv::Mat &img);
  void printModelInfo() const;

private:
  void parseModelInfo();

private:
  Ort::Env env;
  Ort::Session session;
  Ort::SessionOptions options;

  std::string input_name;
  std::string output_name;

  std::vector<TensorInfo> outputs;
  std::vector<TensorInfo> inputs;

  void init_io_names();
}; // namespace inference