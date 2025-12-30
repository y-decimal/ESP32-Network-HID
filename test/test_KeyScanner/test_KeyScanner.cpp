#include <submodules/KeyScanner.cpp>
#include <submodules/KeyScanner.h>
#include <unity.h>

// Test wrapper class to access private members
class TestKeyScanner {
public:
  friend class KeyScanner;
  static void setUp() {}
  static void tearDown() {}
  static void test_getBitMask();
  static void test_setKey();
  static void test_updateKeyState_noKeysPressed();
  static void test_updateKeyState_singleKeyPressed();
  static void test_updateKeyState_multipleKeysPressed();
};

void setup() {
  TestKeyScanner keyScannerTest;

  UNITY_BEGIN();
  RUN_TEST(keyScannerTest.test_getBitMask);
  RUN_TEST(keyScannerTest.test_setKey);
  RUN_TEST(keyScannerTest.test_updateKeyState_noKeysPressed);
  RUN_TEST(keyScannerTest.test_updateKeyState_singleKeyPressed);
  RUN_TEST(keyScannerTest.test_updateKeyState_multipleKeysPressed);
  UNITY_END();
}

void loop() {}

// Test cases
void TestKeyScanner::test_getBitMask() {
  uint8_t rowPins[2] = {9, 10};
  uint8_t colPins[2] = {17, 17};
  KeyScanner scanner = KeyScanner(rowPins, colPins, 2, 2);
  TEST_ASSERT_EQUAL(0b00000001, scanner.getBitMask(0, 0));
  TEST_ASSERT_EQUAL(0b00000010, scanner.getBitMask(0, 1));
  TEST_ASSERT_EQUAL(0b00000100, scanner.getBitMask(1, 0));
  TEST_ASSERT_EQUAL(0b00001000, scanner.getBitMask(1, 1));
}

void TestKeyScanner::test_setKey() {
  uint8_t rowPins[7] = {0};
  uint8_t colPins[7] = {0};
  KeyScanner scanner = KeyScanner(rowPins, colPins, 7, 7);

  uint8_t byteIndex0_0 = scanner.getByteIndex(0, 0);
  TEST_ASSERT_EQUAL_UINT8(0, byteIndex0_0);

  uint8_t byteIndex0_6 = scanner.getByteIndex(0, 6);
  TEST_ASSERT_EQUAL_UINT8(0, byteIndex0_6);

  uint8_t bitMask0_0 = scanner.getBitMask(0, 0);
  TEST_ASSERT_EQUAL_UINT8(0b00000001, bitMask0_0);

  uint8_t bitMask0_6 = scanner.getBitMask(0, 6);
  TEST_ASSERT_EQUAL_UINT8(0b01000000, bitMask0_6);

  uint16_t mapSize = scanner.getBitMapSize();
  uint16_t expectedSize = ((7 * 7 + 7) / 8);
  TEST_ASSERT_EQUAL_UINT16(expectedSize, mapSize);

  memset(scanner.workingBuffer, 0, mapSize);

  scanner.setKey(0, 0);

  uint8_t *state = scanner.workingBuffer;
  uint8_t keyStateKey0_0 = state[byteIndex0_0] & bitMask0_0;
  uint8_t keyStateKey0_6 = state[byteIndex0_6] & bitMask0_6;
  TEST_ASSERT_EQUAL_UINT8(bitMask0_0, keyStateKey0_0);
  TEST_ASSERT_EQUAL_UINT8(0, keyStateKey0_6);

  memset(scanner.workingBuffer, 0, mapSize);
  uint8_t expectedArray[mapSize] = {0};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedArray, scanner.workingBuffer, mapSize);

  scanner.setKey(0, 6);
  state = scanner.workingBuffer;
  keyStateKey0_0 = state[byteIndex0_0] & bitMask0_0;
  TEST_ASSERT_EQUAL_UINT8(0, keyStateKey0_0);

  keyStateKey0_6 = state[byteIndex0_6] & bitMask0_6;
  TEST_ASSERT_EQUAL_UINT8(bitMask0_6, keyStateKey0_6);

  bool keyStateFirstByte = state[0] == bitMask0_6;
  TEST_ASSERT_TRUE(keyStateFirstByte);
  scanner.setKey(0, 0);
  scanner.setKey(0, 1);
  uint8_t bitMask0_1 = scanner.getBitMask(0, 1);
  keyStateFirstByte = state[0] == (bitMask0_0 | bitMask0_1 | bitMask0_6);

  TEST_ASSERT_TRUE(keyStateFirstByte);
}

void TestKeyScanner::test_updateKeyState_noKeysPressed() {
  uint8_t rowPins[2] = {9, 10};
  uint8_t colPins[2] = {17, 18};
  KeyScanner scanner = KeyScanner(rowPins, colPins, 2, 2);
  size_t bitMapSize = scanner.getBitMapSize();

  scanner.updateKeyState();
  uint8_t state[bitMapSize];
  scanner.copyPublishedBitmap(state, bitMapSize);
  TEST_ASSERT_EQUAL(0, state[0]);
}

void TestKeyScanner::test_updateKeyState_singleKeyPressed() {
  uint8_t rowPins[2] = {9, 10};
  uint8_t colPins[2] = {17, 18};
  KeyScanner scanner = KeyScanner(rowPins, colPins, 2, 2);
  size_t bitMapSize = scanner.getBitMapSize();

  scanner.setKey(0, 0);
  std::swap(scanner.workingBuffer, scanner.publishedBuffer);

  uint8_t state[bitMapSize];
  scanner.copyPublishedBitmap(state, bitMapSize);
  TEST_ASSERT_EQUAL(1, state[0]);

  scanner.updateKeyState();
  scanner.copyPublishedBitmap(state, bitMapSize);
  TEST_ASSERT_EQUAL(0, state[0]);
}

void TestKeyScanner::test_updateKeyState_multipleKeysPressed() {
  uint8_t rowPins[2] = {9, 10};
  uint8_t colPins[2] = {17, 18};
  KeyScanner scanner = KeyScanner(rowPins, colPins, 2, 2);
  size_t bitMapSize = scanner.getBitMapSize();

  scanner.setKey(0, 0);
  scanner.setKey(1, 1);

  scanner.swapBuffers();
  uint8_t state[bitMapSize];
  scanner.copyPublishedBitmap(state, bitMapSize);

  uint8_t expected0_0 = scanner.getBitMask(0, 0);
  uint8_t expected1_1 = scanner.getBitMask(1, 1);

  uint8_t stateBit0_0 =
      state[scanner.getByteIndex(0, 0)] & scanner.getBitMask(0, 0);
  uint8_t stateBit1_1 =
      state[scanner.getByteIndex(1, 1)] & scanner.getBitMask(1, 1);

  TEST_ASSERT_EQUAL(expected0_0, stateBit0_0);
  TEST_ASSERT_EQUAL(expected1_1, stateBit1_1);
}
