#include "inference.h"
#include "utils.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {

  cv::Mat img = cv::imread("data/dog.jpg");

  if (img.empty()) {
    std::cout << "image not found\n";
    return -1;
  }

  Inference model("model/squeezenet1.1-7.onnx");

  auto output = model.run(img);

  auto topk = utils::topk(output, 5);

  auto labels = utils::loadLabels("data/imagenet_classes.txt");

  std::cout << "Top-5 indices:\n";
  for (auto i : topk) {
    std::cout << labels[i] << " score=" << output[i] << "\n";
  }

  return 0;
}