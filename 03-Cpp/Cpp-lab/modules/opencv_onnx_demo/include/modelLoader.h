#pragma once

#include "modelInfo.h"
#include <string>

class ModelLoader {
public:
  static ModelInfo load(const std::string &model_dir,
                        const std::string &model_name);
};
