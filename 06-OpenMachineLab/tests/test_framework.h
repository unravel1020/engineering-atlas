#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace test {

struct TestCase {
  std::string name;
  std::function<void()> fn;
};

std::vector<TestCase> &registry();

struct Registrar {
  Registrar(const std::string &name, std::function<void()> fn) {
    registry().push_back({name, fn});
  }
};

inline int runAll() {
  const auto &tests = registry();
  int passed = 0;
  int failed = 0;

  for (const auto &t : tests) {
    try {
      t.fn();
      std::cout << "[PASS] " << t.name << "\n";
      ++passed;
    } catch (const std::exception &e) {
      std::cout << "[FAIL] " << t.name << ": " << e.what() << "\n";
      ++failed;
    } catch (...) {
      std::cout << "[FAIL] " << t.name << ": unknown exception\n";
      ++failed;
    }
  }

  std::cout << "\n" << passed << " passed, " << failed << " failed, "
            << tests.size() << " total\n";
  return failed == 0 ? 0 : 1;
}

} // namespace test

#define TEST(name)                                                             \
  static void test_##name();                                                   \
  static ::test::Registrar g_test_registrar_##name(#name, test_##name);        \
  static void test_##name()

#define ASSERT_TRUE(expr)                                                      \
  do {                                                                         \
    if (!(expr)) {                                                             \
      std::ostringstream oss;                                                  \
      oss << "ASSERT_TRUE failed: " << #expr << " at line " << __LINE__;       \
      throw std::runtime_error(oss.str());                                     \
    }                                                                          \
  } while (0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    if (!((a) == (b))) {                                                       \
      std::ostringstream oss;                                                  \
      oss << "ASSERT_EQ failed: " << #a << " != " << #b << " at line "        \
          << __LINE__;                                                         \
      throw std::runtime_error(oss.str());                                     \
    }                                                                          \
  } while (0)

#define ASSERT_NE(a, b)                                                        \
  do {                                                                         \
    if ((a) == (b)) {                                                          \
      std::ostringstream oss;                                                  \
      oss << "ASSERT_NE failed: " << #a << " == " << #b << " at line "        \
          << __LINE__;                                                         \
      throw std::runtime_error(oss.str());                                     \
    }                                                                          \
  } while (0)

#define ASSERT_GT(a, b)                                                        \
  do {                                                                         \
    if (!((a) > (b))) {                                                        \
      std::ostringstream oss;                                                  \
      oss << "ASSERT_GT failed: " << #a << " <= " << #b << " at line "        \
          << __LINE__;                                                         \
      throw std::runtime_error(oss.str());                                     \
    }                                                                          \
  } while (0)

#define ASSERT_NEAR(a, b, eps)                                                 \
  do {                                                                         \
    if (std::abs((a) - (b)) > (eps)) {                                         \
      std::ostringstream oss;                                                  \
      oss << "ASSERT_NEAR failed: |" << (a) << " - " << (b) << "| > " << (eps)\
          << " at line " << __LINE__;                                          \
      throw std::runtime_error(oss.str());                                     \
    }                                                                          \
  } while (0)

#define ASSERT_THROW(expr)                                                     \
  do {                                                                         \
    bool caught = false;                                                       \
    try {                                                                      \
      expr;                                                                    \
    } catch (...) {                                                            \
      caught = true;                                                           \
    }                                                                          \
    if (!caught) {                                                             \
      std::ostringstream oss;                                                  \
      oss << "ASSERT_THROW failed: no exception at line " << __LINE__;         \
      throw std::runtime_error(oss.str());                                     \
    }                                                                          \
  } while (0)
