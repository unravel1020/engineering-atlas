#include "test_framework.h"

namespace test {

std::vector<TestCase> &registry() {
  static std::vector<TestCase> instance;
  return instance;
}

} // namespace test
