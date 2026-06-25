#include "preProcessor.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>

TEST(pre_processor_rgb_color_format) {
  // Create a BGR image: blue channel = 255, green = 0, red = 0
  cv::Mat img(10, 10, CV_8UC3, cv::Scalar(255, 0, 0));

  PreProcessor pp;
  TensorInfo info;
  PreprocessConfig cfg;
  cfg.color_format = "RGB";
  cfg.resize_mode = "stretch";
  cfg.target_h = 10;
  cfg.target_w = 10;
  cfg.scale = 1.0f / 255.0f;

  int c = 0, h = 0, w = 0;
  auto tensor = pp.mat_to_tensor(img, info, cfg, c, h, w, nullptr);

  // In RGB order, the first channel should be red = 0.
  ASSERT_NEAR(tensor[0], 0.0f, 1e-5f);
  // The third channel should be blue = 1.
  ASSERT_NEAR(tensor[2 * 10 * 10], 1.0f, 1e-5f);
}

TEST(pre_processor_invalid_color_format_throws) {
  cv::Mat img = cv::Mat::ones(10, 10, CV_8UC3);
  PreProcessor pp;
  TensorInfo info;
  PreprocessConfig cfg;
  cfg.color_format = "GRAY";

  int c = 0, h = 0, w = 0;
  ASSERT_THROW(pp.mat_to_tensor(img, info, cfg, c, h, w, nullptr));
}

TEST(pre_processor_letterbox_square_image_has_no_padding) {
  // Square image resized to square target should have zero padding.
  cv::Mat img = cv::Mat::ones(100, 100, CV_8UC3) * 128;

  PreProcessor pp;
  TensorInfo info;
  PreprocessConfig cfg;
  cfg.resize_mode = "letterbox";
  cfg.target_h = 64;
  cfg.target_w = 64;
  cfg.layout = "NCHW";

  PreprocessInfo preprocess_info;
  int c = 0, h = 0, w = 0;
  auto tensor = pp.mat_to_tensor(img, info, cfg, c, h, w, &preprocess_info);

  ASSERT_EQ(preprocess_info.pad_top, 0);
  ASSERT_EQ(preprocess_info.pad_bottom, 0);
  ASSERT_EQ(preprocess_info.pad_left, 0);
  ASSERT_EQ(preprocess_info.pad_right, 0);
  ASSERT_EQ(tensor.size(), 3u * 64 * 64);
}
