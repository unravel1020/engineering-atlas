#include "modelManager.h"
#include "postProcessor.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

static std::vector<std::string> loadLabels(const std::string &path) {
  std::ifstream ifs(path);
  std::vector<std::string> labels;
  std::string line;
  while (std::getline(ifs, line)) {
    labels.push_back(line);
  }
  return labels;
}

static void visualizeDetections(
    cv::Mat &img, const std::vector<postProcessor::Detection> &detections,
    const std::vector<std::string> &labels) {
  for (const auto &det : detections) {
    cv::Point pt1(static_cast<int>(det.x1), static_cast<int>(det.y1));
    cv::Point pt2(static_cast<int>(det.x2), static_cast<int>(det.y2));
    cv::rectangle(img, pt1, pt2, cv::Scalar(0, 255, 0), 2);

    std::string label =
        (det.class_id >= 0 && det.class_id < static_cast<int>(labels.size()))
            ? labels[det.class_id]
            : "class_" + std::to_string(det.class_id);
    std::string text = label + " " + std::to_string(static_cast<int>(det.confidence * 100)) + "%";

    int baseline = 0;
    cv::Size text_size =
        cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
    cv::rectangle(img, pt1,
                  cv::Point(pt1.x + text_size.width, pt1.y - text_size.height - 4),
                  cv::Scalar(0, 255, 0), -1);
    cv::putText(img, text, cv::Point(pt1.x, pt1.y - 2),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
  }
}

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

    auto result = model->runWithInfo(img);

    if (model_info.task() == "classification") {
      auto output = result.outputs.at(model_info.outputs()[0].name);
      auto cls = postProcessor::classify(output, model_info.postprocess(),
                                         model_info.modelDir());

      std::cout << "Top-" << model_info.postprocess().topk << " indices:\n";
      for (auto i : cls.indices) {
        std::cout << cls.labels[i] << " probability=" << cls.probs[i] * 100.0f
                  << "%\n";
      }

      float total = 0.0f;
      for (auto p : cls.probs) {
        total += p;
      }
      std::cout << "sum = " << total << std::endl;
    } else if (model_info.task() == "detection") {
      auto output = result.outputs.at(model_info.outputs()[0].name);
      auto dets = postProcessor::detect(output, model_info.postprocess(),
                                        model_info.modelDir(),
                                        result.preprocess);

      std::cout << "Detections: " << dets.size() << "\n";
      for (const auto &det : dets) {
        std::cout << "class=" << det.class_id
                  << " conf=" << det.confidence << " box=[" << det.x1 << ","
                  << det.y1 << "," << det.x2 << "," << det.y2 << "]\n";
      }

      auto labels = loadLabels(model_info.modelDir() + "/" +
                               model_info.postprocess().labels_file);
      cv::Mat vis = img.clone();
      visualizeDetections(vis, dets, labels);

      std::string out_path = "output/" + model_name + "_result.jpg";
      cv::imwrite(out_path, vis);
      std::cout << "Saved visualization to " << out_path << "\n";
    } else {
      std::cout << "Unsupported task: " << model_info.task() << "\n";
    }

    model->printModelInfo();
  }

  return 0;
}
