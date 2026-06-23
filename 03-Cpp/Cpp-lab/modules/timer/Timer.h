#pragma once

// Timer is kept minimal on purpose: it demonstrates module separation and
// public headers without introducing cross-platform timing complexity.
class Timer {
public:
  void Start();
};