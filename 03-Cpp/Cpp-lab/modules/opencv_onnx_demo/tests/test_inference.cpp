#include "inference.h"
#include "modelLoader.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>

TEST(inference_throws_when_model_file_missing) {
  ModelInfo info("/nonexistent/dir", "missing.onnx", "classification", {}, {},
                 {}, {});
  ASSERT_THROW(Inference inference(info));
}

TEST(inference_throws_on_empty_input_image) {
  // Use a real model so the Inference object can be constructed.
  ModelInfo info = ModelLoader::load("models/squeezenet1.1-7", "squeezenet");
  Inference inference(info);

  cv::Mat empty_img;
  ASSERT_THROW(inference.run(empty_img));
}

TEST(inference_throws_on_empty_input_map) {
  ModelInfo info = ModelLoader::load("models/squeezenet1.1-7", "squeezenet");
  Inference inference(info);

  std::unordered_map<std::string, cv::Mat> inputs;
  inputs[info.inputs()[0].name] = cv::Mat();
  ASSERT_THROW(inference.run(inputs));
}

TEST(inference_run_with_info_returns_outputs) {
  ModelInfo info = ModelLoader::load("models/squeezenet1.1-7", "squeezenet");
  Inference inference(info);

  cv::Mat img = cv::imread("models/squeezenet1.1-7/test_data/dog.jpg");
  ASSERT_FALSE(img.empty());

  auto result = inference.runWithInfo(img);
  ASSERT_TRUE(result.outputs.find(info.outputs()[0].name) != result.outputs.end());
  ASSERT_GT(result.outputs[info.outputs()[0].name].size(), 0u);
}
