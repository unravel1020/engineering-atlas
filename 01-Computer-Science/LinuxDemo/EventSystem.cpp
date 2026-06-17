#include "EventSystem.h"
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <thread>

using namespace std;

int main() {
  int flag = 0;
  while (1) {
    cout << "running" << endl;
    this_thread::sleep_for(chrono::seconds(3));
    flag++;
    if (flag == 10)
      break;
  }
  return 0;
}
