#include "inference.h"
#include "utils.h"
#include <iostream>

Inference::Inference(const std::string &model_path)
    : env(ORT_LOGGING_LEVEL_WARNING, "demo"), session(nullptr) {
  options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

  session = Ort::Session(env, model_path.c_str(), options);

  init_io_names();
}

void Inference::init_io_names() {

  Ort::AllocatorWithDefaultOptions allocator;

  input_name = session.GetInputNameAllocated(0, allocator).get();
  output_name = session.GetOutputNameAllocated(0, allocator).get();
}

std::vector<float> Inference::run(const cv::Mat &img) {

  int c, h, w;
  auto input_tensor_values = utils::mat_to_tensor_nchw(img, c, h, w);

  std::vector<int64_t> input_shape = {1, c, h, w};

  Ort::MemoryInfo memory_info =
      Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

  auto input_tensor = Ort::Value::CreateTensor<float>(
      memory_info, input_tensor_values.data(), input_tensor_values.size(),
      input_shape.data(), input_shape.size());

  const char *input_names[] = {input_name.c_str()};
  const char *output_names[] = {output_name.c_str()};

  auto output = session.Run(Ort::RunOptions{nullptr}, input_names,
                            &input_tensor, 1, output_names, 1);

  float *out = output[0].GetTensorMutableData<float>();

  size_t out_size = output[0].GetTensorTypeAndShapeInfo().GetElementCount();

  return std::vector<float>(out, out + out_size);
}