#include "test_framework.h"

namespace test {

// Single global registry shared across all translation units. Defined in this
// source file to avoid multiple-definition errors from the header-only macros.
std::vector<TestCase> &registry() {
  static std::vector<TestCase> instance;
  return instance;
}

} // namespace test
