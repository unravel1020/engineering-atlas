#include "preProcessor.h"

namespace preProcessor {
std::vector<float> mat_to_tensor_nchw(const cv::Mat &img, int &c, int &h,
                                      int &w, int target_h, int target_w) {

  cv::Mat resized;
  cv::resize(img, resized, cv::Size(target_w, target_h));
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
} // namespace preProcessor