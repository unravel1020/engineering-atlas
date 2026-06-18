#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

namespace utils {

std::vector<float> mat_to_tensor_nchw(const cv::Mat &img, int &c, int &h,
                                      int &w);

std::vector<int> topk(const std::vector<float> &data, int k);

std::vector<std::string> loadLabels(const std::string &path);
} // namespace utils