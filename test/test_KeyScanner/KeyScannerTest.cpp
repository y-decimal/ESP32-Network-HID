#include <unity.h>
#include "include/KeyScannerTest.h"

#ifndef UNITY_NATIVE
void setup() {
#else
int main(int argc, char **argv) {
#endif
  TestKeyScanner keyScannerTest;

  UNITY_BEGIN();
  keyScannerTest.runAllTests();
  UNITY_END();
}

void loop() {}


