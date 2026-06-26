#include "test_framework.h"

namespace test {

std::vector<TestCase> &registry() {
  static std::vector<TestCase> g_registry;
  return g_registry;
}

} // namespace test
