#include "modelManager.h"
#include "postProcessor.h"
#include "utils.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  ModelManager manager("models/registry.json");

  const std::string model_name = "squeezenet1.1-7";
  manager.select(model_name);

  auto model = manager.current();
  const auto &model_info = model->modelInfo();

  std::cout << "Current model: " << model_name << "\n";
  std::cout << "Current task: " << model_info.task() << "\n";

  cv::Mat img = cv::imread(model_info.modelDir() + "/test_data/dog.jpg");

  if (img.empty()) {
    std::cout << "image not found\n";
    return -1;
  }

  auto output = manager.run(img);

  float total = 0.0f;

  if (model_info.task() == "classification") {
    auto result =
        postProcessor::classify(output, model_info.postprocess(),
                                model_info.modelDir());

    std::cout << "Top-" << model_info.postprocess().topk << " indices:\n";
    for (auto i : result.indices) {
      std::cout << result.labels[i] << " probability=" << result.probs[i] * 100.0f
                << "%\n";
    }

    for (auto p : result.probs) {
      total += p;
    }
  } else {
    for (auto p : output) {
      total += p;
    }
  }

  model->printModelInfo();

  std::cout << "sum = " << total << std::endl;

  return 0;
}
