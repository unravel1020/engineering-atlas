#pragma once

#include <functional>
#include <string>
#include <vector>

// Event is a minimal placeholder used in C++ module-linking exercises.
// It intentionally avoids complex subscription logic to keep the focus on
// build-system integration and static-library boundaries.
class Event {
public:
  void trigger();
};