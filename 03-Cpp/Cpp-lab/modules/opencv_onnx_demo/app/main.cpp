#include "modelManager.h"
#include "pipeline.h"
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

    auto pipe = pipeline::PipelineFactory::create(model_info.task(), model);
    auto task_result = pipe->run(img);

    task_result->print(std::cout);
    std::string out_path = "output/" + model_name + "_result.jpg";
    task_result->save(out_path, img);
    std::cout << "Saved visualization to " << out_path << "\n";

    model->printModelInfo();
  }

  return 0;
}
