#include "utils.h"
#include <algorithm>
#include <fstream>
#include <numeric>

namespace utils {

std::vector<float> mat_to_tensor_nchw(const cv::Mat &img, int &c, int &h,
                                      int &w) {

  cv::Mat resized;
  cv::resize(img, resized, cv::Size(224, 224));
  resized.convertTo(resized, CV_32F, 1.0 / 255.0);

  h = resized.rows;
  w = resized.cols;
  c = resized.channels();

  std::vector<cv::Mat> channels;
  cv::split(resized, channels);

  std::vector<float> tensor;
  for (auto &ch : channels) {
    tensor.insert(tensor.end(), (float *)ch.datastart, (float *)ch.dataend);
  }

  return tensor;
}

std::vector<int> topk(const std::vector<float> &data, int k) {

  std::vector<int> idx(data.size());
  std::iota(idx.begin(), idx.end(), 0);

  std::partial_sort(idx.begin(), idx.begin() + k, idx.end(),
                    [&](int a, int b) { return data[a] > data[b]; });

  idx.resize(k);
  return idx;
}

std::vector<std::string> loadLabels(const std::string &path) {
  std::ifstream ifs(path);

  std::vector<std::string> labels;

  std::string line;

  while (std::getline(ifs, line)) {
    labels.push_back(line);
  }

  return labels;
}

} // namespace utils