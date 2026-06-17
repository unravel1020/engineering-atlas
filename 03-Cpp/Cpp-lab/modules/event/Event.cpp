#include "Event.h"
#include <iostream>

void Event::trigger() { std::cout << "[Event] triggered" << std::endl; }