#include "postProcessor.h"
#include "test_framework.h"
#include <cmath>
#include <vector>

TEST(post_processor_softmax_sums_to_one) {
  std::vector<float> logits = {1.0f, 2.0f, 3.0f};
  auto probs = postProcessor::softmax(logits);
  ASSERT_EQ(probs.size(), 3u);

  float sum = 0.0f;
  for (auto p : probs) {
    sum += p;
  }
  ASSERT_NEAR(sum, 1.0f, 1e-5f);
  ASSERT_TRUE(probs[2] > probs[1] && probs[1] > probs[0]);
}

TEST(post_processor_topk) {
  std::vector<float> data = {0.1f, 0.5f, 0.2f, 0.9f, 0.3f};
  auto idx = postProcessor::topk(data, 3);
  ASSERT_EQ(idx.size(), 3u);
  ASSERT_EQ(idx[0], 3);
  ASSERT_EQ(idx[1], 1);
  ASSERT_EQ(idx[2], 4);
}

TEST(post_processor_detect_yolo_format) {
  // Simulate 1 candidate: box at center (320,320), size (160,160), class 5 score 0.9
  const int num_candidates = 1;
  const int num_classes = 80;
  std::vector<float> output((4 + num_classes) * num_candidates, 0.0f);
  output[0] = 320.0f; // x
  output[1] = 320.0f; // y
  output[2] = 160.0f; // w
  output[3] = 160.0f; // h
  output[4 + 5] = 0.9f; // class 5

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
  ASSERT_EQ(result.detections()[0].class_id, 5);
  ASSERT_NEAR(result.detections()[0].confidence, 0.9f, 1e-5f);
  ASSERT_NEAR(result.detections()[0].x1, 240.0f, 1e-5f);
  ASSERT_NEAR(result.detections()[0].y1, 240.0f, 1e-5f);
  ASSERT_NEAR(result.detections()[0].x2, 400.0f, 1e-5f);
  ASSERT_NEAR(result.detections()[0].y2, 400.0f, 1e-5f);
}

TEST(post_processor_detect_nms_removes_duplicates) {
  const int num_candidates = 2;
  const int num_classes = 80;
  std::vector<float> output((4 + num_classes) * num_candidates, 0.0f);
  // Two nearly identical boxes, both class 0, high score
  // Layout N_C_K: channel c at offset c * num_candidates
  output[0 * num_candidates + 0] = 320.0f; // cand0 x
  output[0 * num_candidates + 1] = 322.0f; // cand1 x
  output[1 * num_candidates + 0] = 320.0f; // cand0 y
  output[1 * num_candidates + 1] = 322.0f; // cand1 y
  output[2 * num_candidates + 0] = 160.0f; // cand0 w
  output[2 * num_candidates + 1] = 160.0f; // cand1 w
  output[3 * num_candidates + 0] = 160.0f; // cand0 h
  output[3 * num_candidates + 1] = 160.0f; // cand1 h
  output[(4 + 0) * num_candidates + 0] = 0.9f; // cand0 class 0
  output[(4 + 0) * num_candidates + 1] = 0.8f; // cand1 class 0

  PostprocessConfig cfg;
  cfg.conf_threshold = 0.25f;
  cfg.nms_threshold = 0.1f; // strict NMS
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
}
