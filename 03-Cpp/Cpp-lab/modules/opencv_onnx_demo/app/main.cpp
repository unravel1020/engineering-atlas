#include "inference.h"
#include "modelLoader.h"
#include "modelRegistry.h"
#include "postProcessor.h"
#include "utils.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  auto registry = ModelRegistry::load("models/registry.json");

  const std::string model_name = "squeezenet1.1-7";
  const std::string model_dir = registry.modelDir(model_name);

  auto model_info = ModelLoader::load(model_dir, model_name);
  Inference model(model_info);

  cv::Mat img = cv::imread(model_dir + "/test_data/dog.jpg");

  if (img.empty()) {
    std::cout << "image not found\n";
    return -1;
  }

  auto output = model.run(img);

  auto result =
      postProcessor::classify(output, model_info.postprocess(), model_dir);

  std::cout << "Top-" << model_info.postprocess().topk << " indices:\n";
  for (auto i : result.indices) {
    std::cout << result.labels[i] << " probability=" << result.probs[i] * 100.0f
              << "%\n";
  }

  float total = 0.0f;

  for (auto p : result.probs) {
    total += p;
  }

  model.printModelInfo();

  std::cout << "sum = " << total << std::endl;

  return 0;
}
