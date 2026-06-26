#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace oml {
namespace backend {

// Unified tensor metadata across all backends.
// Backends must map their native tensor descriptions to this struct so the
// rest of the pipeline remains backend-agnostic.
struct TensorInfo {
  std::string name;
  std::vector<int64_t> shape;
  // Element type is kept as a string to avoid leaking backend-specific enums
  // into the core pipeline. Common values: "float32", "float16", "int32".
  std::string dtype;
};

} // namespace backend
} // namespace oml
