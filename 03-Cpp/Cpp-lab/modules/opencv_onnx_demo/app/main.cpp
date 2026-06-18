#include "inference.h"
#include "postProcessor.h"
#include "preProcessor.h"
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

  auto topk = postProcessor::topk(output, 5);

  auto labels = postProcessor::loadLabels("data/imagenet_classes.txt");

  auto probs = postProcessor::softmax(output);

  std::cout << "Top-5 indices:\n";
  for (auto i : topk) {
    std::cout << labels[i] << " probilitiy=" << probs[i] * 100.0f << "%\n";
  }

  float total = 0.0f;

  for (auto p : probs) {
    total += p;
  }

  std::cout << "sum = " << total << std::endl;

  return 0;
}