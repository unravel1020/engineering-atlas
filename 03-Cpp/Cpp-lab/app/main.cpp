#include "Event.h"
#include "Logger.h"
#include "Timer.h"

int main() {
  Logger::log("system start");

  Timer t;
  t.Start();

  Event e;
  e.trigger();

  return 0;
}