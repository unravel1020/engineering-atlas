#pragma once

#include "modelInfo.h"
#include <string>

// ModelLoader builds a ModelInfo from a per-model directory containing
// model.json and the ONNX model file. It is intentionally stateless so that
// loading can happen lazily on demand.
class ModelLoader {
public:
  static ModelInfo load(const std::string &model_dir,
                        const std::string &model_name);
};
