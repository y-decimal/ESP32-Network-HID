#include <KeyScanner.cpp>
#include <KeyScanner.h>
#include <unity.h>

// Test wrapper class to access private members
class TestKeyScanner {
public:
  static void test_updateKeyState(KeyScanner &scanner) {
    scanner.updateKeyState();
  }

  static void test_setKey(KeyScanner &scanner, uint8_t row, uint8_t col) {
    scanner.setKey(&row, &col);
  }

  static void test_clearKey(KeyScanner &scanner, uint8_t row, uint8_t col) {
    scanner.clearKey(&row, &col);
  }

  static uint8_t test_getBitIndex(KeyScanner &scanner, uint8_t row,
                                  uint8_t col) {
    return scanner.getBitIndex(&row, &col);
  }

  static uint8_t *test_getKeyStateArray(KeyScanner &scanner) {
    return scanner.keyState;
  }
};

// Test cases
void test_getBitIndex() {
  uint8_t bitMap[5] = {0};
  KeyScanner scanner = KeyScanner(bitMap);
  TEST_ASSERT_EQUAL(0, TestKeyScanner::test_getBitIndex(scanner, 0, 0));
  TEST_ASSERT_EQUAL(1, TestKeyScanner::test_getBitIndex(scanner, 0, 1));
  TEST_ASSERT_EQUAL(2, TestKeyScanner::test_getBitIndex(scanner, 1, 0));
  TEST_ASSERT_EQUAL(3, TestKeyScanner::test_getBitIndex(scanner, 1, 1));
}

void test_setKey() {
  uint8_t bitMap[5] = {0};
  KeyScanner scanner = KeyScanner(bitMap);
  TestKeyScanner::test_setKey(scanner, 0, 0);
  uint8_t *state = TestKeyScanner::test_getKeyStateArray(scanner);
  TEST_ASSERT_EQUAL(1, state[0]);
}

void test_clearKey() {
  uint8_t bitMap[5] = {0};
  KeyScanner scanner = KeyScanner(bitMap);
  TestKeyScanner::test_setKey(scanner, 0, 0);
  TestKeyScanner::test_clearKey(scanner, 0, 0);
  uint8_t *state = TestKeyScanner::test_getKeyStateArray(scanner);
  TEST_ASSERT_EQUAL(0, state[0]);
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_getBitIndex);
  RUN_TEST(test_setKey);
  RUN_TEST(test_clearKey);
  UNITY_END();
}

void loop() {}
