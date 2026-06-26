#include "postProcessor.h"
#include "test_framework.h"
#include <cmath>
#include <vector>

TEST(post_processor_softmax_empty_returns_empty) {
  auto probs = postProcessor::softmax({});
  ASSERT_EQ(probs.size(), 0u);
}

TEST(post_processor_topk_larger_than_data) {
  std::vector<float> data = {0.5f, 0.3f};
  auto idx = postProcessor::topk(data, 10);
  ASSERT_EQ(idx.size(), 2u);
  ASSERT_EQ(idx[0], 0);
  ASSERT_EQ(idx[1], 1);
}

TEST(post_processor_topk_zero_returns_empty) {
  std::vector<float> data = {0.5f, 0.3f};
  auto idx = postProcessor::topk(data, 0);
  ASSERT_EQ(idx.size(), 0u);
}

TEST(post_processor_detect_invalid_output_layout_throws) {
  const int num_candidates = 1;
  const int num_classes = 80;
  std::vector<float> output((4 + num_classes) * num_candidates, 0.0f);

  PostprocessConfig cfg;
  cfg.output_layout = "N_K_C";

  PreprocessInfo preprocess;
  ASSERT_THROW(postProcessor::detect(output, cfg, ".", preprocess));
}

TEST(post_processor_detect_invalid_box_format_throws) {
  const int num_candidates = 1;
  const int num_classes = 80;
  std::vector<float> output((4 + num_classes) * num_candidates, 0.0f);

  PostprocessConfig cfg;
  cfg.box_format = "cxcywh";
  cfg.output_layout = "N_C_K";

  PreprocessInfo preprocess;
  ASSERT_THROW(postProcessor::detect(output, cfg, ".", preprocess));
}

TEST(post_processor_detect_conf_threshold_filters_boxes) {
  const int num_candidates = 2;
  const int num_classes = 80;
  std::vector<float> output((4 + num_classes) * num_candidates, 0.0f);

  // Candidate 0 has low confidence.
  output[(4 + 0) * num_candidates + 0] = 0.1f;
  // Candidate 1 has high confidence.
  output[(4 + 0) * num_candidates + 1] = 0.9f;

  PostprocessConfig cfg;
  cfg.conf_threshold = 0.25f;
  cfg.nms_threshold = 0.45f;
  cfg.num_classes = num_classes;
  cfg.num_candidates = num_candidates;
  cfg.box_format = "xywh";
  cfg.output_layout = "N_C_K";

  PreprocessInfo preprocess;
  preprocess.orig_h = 640;
  preprocess.orig_w = 640;
  preprocess.scale = 1.0f;
  preprocess.pad_top = 0;
  preprocess.pad_left = 0;

  auto result = postProcessor::detect(output, cfg, ".", preprocess);
  ASSERT_EQ(result.detections().size(), 1u);
  ASSERT_GT(result.detections()[0].confidence, 0.0f);
}
