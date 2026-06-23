#include "test_framework.h"

// Entry point for the unit-test executable. All tests are discovered through
// the global registry populated by TEST() macros in individual test files.
int main() { return ::test::runAll(); }
