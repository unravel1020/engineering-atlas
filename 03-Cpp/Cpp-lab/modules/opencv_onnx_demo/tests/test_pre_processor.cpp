#include "preProcessor.h"
#include "test_framework.h"
#include <opencv2/opencv.hpp>
#include <vector>

TEST(pre_processor_stretch_shape) {
  cv::Mat img = cv::Mat::ones(100, 200, CV_8UC3) * 128;
  PreProcessor pp;
  TensorInfo info;
  PreprocessConfig cfg;
  cfg.resize_mode = "stretch";
  cfg.target_h = 64;
  cfg.target_w = 64;
  cfg.layout = "NCHW";

  int c = 0, h = 0, w = 0;
  auto tensor = pp.mat_to_tensor(img, info, cfg, c, h, w, nullptr);

  ASSERT_EQ(c, 3);
  ASSERT_EQ(h, 64);
  ASSERT_EQ(w, 64);
  ASSERT_EQ(tensor.size(), 3u * 64 * 64);
}

TEST(pre_processor_letterbox_preserves_aspect_ratio) {
  cv::Mat img = cv::Mat::ones(100, 200, CV_8UC3) * 128;
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

  ASSERT_EQ(h, 64);
  ASSERT_EQ(w, 64);
  ASSERT_EQ(preprocess_info.orig_h, 100);
  ASSERT_EQ(preprocess_info.orig_w, 200);
  ASSERT_LT(preprocess_info.scale, 1.0f);
  ASSERT_TRUE(preprocess_info.pad_top > 0 || preprocess_info.pad_bottom > 0);
  ASSERT_EQ(tensor.size(), 3u * 64 * 64);
}

TEST(pre_processor_scale_and_mean_std) {
  cv::Mat img(10, 10, CV_8UC3, cv::Scalar(255, 128, 0));
  PreProcessor pp;
  TensorInfo info;
  PreprocessConfig cfg;
  cfg.resize_mode = "stretch";
  cfg.target_h = 10;
  cfg.target_w = 10;
  cfg.scale = 1.0f / 255.0f;
  cfg.mean = {0.5f, 0.0f, 0.0f};
  cfg.std = {0.5f, 1.0f, 1.0f};

  int c = 0, h = 0, w = 0;
  auto tensor = pp.mat_to_tensor(img, info, cfg, c, h, w, nullptr);

  // OpenCV Scalar is BGR order: channel 0 = blue = 255 -> (255/255 - 0.5)/0.5 = 1
  ASSERT_NEAR(tensor[0], 1.0f, 1e-5f);
  // channel 1 = green = 128 -> (128/255 - 0)/1 = 0.50196
  ASSERT_NEAR(tensor[10 * 10], 128.0f / 255.0f, 1e-5f);
  // channel 2 = red = 0 -> (0/255 - 0)/1 = 0
  ASSERT_NEAR(tensor[2 * 10 * 10], 0.0f, 1e-5f);
}

TEST(pre_processor_invalid_layout_throws) {
  cv::Mat img = cv::Mat::ones(10, 10, CV_8UC3);
  PreProcessor pp;
  TensorInfo info;
  PreprocessConfig cfg;
  cfg.layout = "NHWC";

  int c = 0, h = 0, w = 0;
  ASSERT_THROW(pp.mat_to_tensor(img, info, cfg, c, h, w, nullptr));
}

TEST(pre_processor_invalid_resize_mode_throws) {
  cv::Mat img = cv::Mat::ones(10, 10, CV_8UC3);
  PreProcessor pp;
  TensorInfo info;
  PreprocessConfig cfg;
  cfg.resize_mode = "crop";

  int c = 0, h = 0, w = 0;
  ASSERT_THROW(pp.mat_to_tensor(img, info, cfg, c, h, w, nullptr));
}
