#include "modelManager.h"
#include "postProcessor.h"
#include "result.h"
#include "utils.h"
#include <iostream>
#include <memory>
#include <opencv2/opencv.hpp>
#include <string>

int main() {
  ModelManager manager("models/registry.json");

  for (const auto &model_name : manager.modelNames()) {
    manager.select(model_name);
    auto model = manager.current();
    const auto &model_info = model->modelInfo();

    std::cout << "\n========== Model: " << model_name
              << " | Task: " << model_info.task() << " ==========\n";

    cv::Mat img = cv::imread(model_info.modelDir() + "/test_data/dog.jpg");
    if (img.empty()) {
      std::cout << "image not found for " << model_name << "\n";
      continue;
    }

    auto inference_result = model->runWithInfo(img);

    std::unique_ptr<result::Result> task_result;
    if (model_info.task() == "classification") {
      auto output = inference_result.outputs.at(model_info.outputs()[0].name);
      task_result = std::make_unique<result::ClassificationResult>(
          postProcessor::classify(output, model_info.postprocess(),
                                  model_info.modelDir()));
    } else if (model_info.task() == "detection") {
      auto output = inference_result.outputs.at(model_info.outputs()[0].name);
      task_result = std::make_unique<result::DetectionResult>(
          postProcessor::detect(output, model_info.postprocess(),
                                model_info.modelDir(),
                                inference_result.preprocess));
    } else {
      std::cout << "Unsupported task: " << model_info.task() << "\n";
      continue;
    }

    task_result->print(std::cout);
    std::string out_path = "output/" + model_name + "_result.jpg";
    task_result->save(out_path, img);
    std::cout << "Saved visualization to " << out_path << "\n";

    model->printModelInfo();
  }

  return 0;
}
