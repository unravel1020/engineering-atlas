#include "pipeline.h"
#include "test_framework.h"
#include <string>

TEST(pipeline_factory_creates_classification) {
  // We only verify factory dispatches by task; no real inference needed.
  // Passing nullptr is safe because run() is not called.
  auto pipe = pipeline::PipelineFactory::create("classification", nullptr);
  ASSERT_TRUE(pipe != nullptr);
}

TEST(pipeline_factory_creates_detection) {
  auto pipe = pipeline::PipelineFactory::create("detection", nullptr);
  ASSERT_TRUE(pipe != nullptr);
}

TEST(pipeline_factory_rejects_unknown_task) {
  ASSERT_THROW(pipeline::PipelineFactory::create("segmentation", nullptr));
}
