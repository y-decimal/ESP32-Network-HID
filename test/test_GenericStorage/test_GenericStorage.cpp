#ifndef TEST_GENERICSTORAGE_H
#define TEST_GENERICSTORAGE_H

#ifndef UNITY_NATIVE
#include <Arduino.h>
#include <submodules/Storage/PreferencesStorage.h>
#else
#include "../FakeStorage.h"
#endif

#include <submodules/Storage/GenericStorage.h>
#include <unity.h>

#define NAMESPACE "GenStorTest"
#define TEST_KEY "test_key"
#define LOAD_KEY "load_key"
#define NONEXISTENT_KEY "nonexistent_key"
#define MULTI_KEY "multi_key"

namespace {
#ifndef UNITY_NATIVE
PreferencesStorage testStorage(NAMESPACE);
#else
FakeStorage testStorage;
#endif
} // namespace

void setUp() {
  // No setup needed with FakeStorage
}

void tearDown() {
  testStorage.remove(TEST_KEY);
  testStorage.remove(LOAD_KEY);
  testStorage.remove(NONEXISTENT_KEY);
  testStorage.remove(MULTI_KEY);
}

void test_GenericStorage_initialization() {
  struct TestData {
    int value;
    char name[10];
  };

  GenericStorage<TestData> storage(&testStorage, TEST_KEY);
  TEST_ASSERT_FALSE(storage.isDirty());
}

void test_GenericStorage_set_marks_dirty() {
  struct TestData {
    int value;
  };

  GenericStorage<TestData> storage(&testStorage, TEST_KEY);
  TestData data = {42};

  storage.set(data);
  TEST_ASSERT_TRUE(storage.isDirty());
  TEST_ASSERT_EQUAL(42, storage.get().value);
}

void test_GenericStorage_save_clears_dirty() {
  struct TestData {
    int value;
  };

  GenericStorage<TestData> storage(&testStorage, TEST_KEY);
  TestData data = {42};

  storage.set(data);
  TEST_ASSERT_TRUE(storage.isDirty());

  bool saved = storage.save();
  TEST_ASSERT_TRUE(saved);
  TEST_ASSERT_FALSE(storage.isDirty());
}

void test_GenericStorage_load_restores_data() {
  struct TestData {
    int value;
    char text[5];
  };

  GenericStorage<TestData> storage1(&testStorage, LOAD_KEY);
  TestData data = {123, "test"};

  storage1.set(data);
  storage1.save();

  GenericStorage<TestData> storage2(&testStorage, LOAD_KEY);
  bool loaded = storage2.load();

  TEST_ASSERT_TRUE(loaded);
  TEST_ASSERT_EQUAL(123, storage2.get().value);
  TEST_ASSERT_EQUAL_STRING("test", storage2.get().text);
  TEST_ASSERT_FALSE(storage2.isDirty());
}

void test_GenericStorage_load_fails_on_nonexistent_key() {
  struct TestData {
    int value;
  };

  GenericStorage<TestData> storage(&testStorage, NONEXISTENT_KEY);
  bool loaded = storage.load();

  TEST_ASSERT_FALSE(loaded);
}

void test_GenericStorage_clearDirty() {
  struct TestData {
    int value;
  };

  GenericStorage<TestData> storage(&testStorage, TEST_KEY);
  TestData data = {42};

  storage.set(data);
  TEST_ASSERT_TRUE(storage.isDirty());

  storage.clearDirty();
  TEST_ASSERT_FALSE(storage.isDirty());
}

void test_GenericStorage_multiple_saves() {
  struct TestData {
    int value;
  };

  GenericStorage<TestData> storage(&testStorage, MULTI_KEY);

  TestData data1 = {100};
  storage.set(data1);
  storage.save();

  TestData data2 = {200};
  storage.set(data2);
  storage.save();

  GenericStorage<TestData> storage2(&testStorage, MULTI_KEY);
  storage2.load();

  TEST_ASSERT_EQUAL(200, storage2.get().value);
}

void run_GenericStorage_tests() {
  RUN_TEST(test_GenericStorage_initialization);
  RUN_TEST(test_GenericStorage_set_marks_dirty);
  RUN_TEST(test_GenericStorage_save_clears_dirty);
  RUN_TEST(test_GenericStorage_load_restores_data);
  RUN_TEST(test_GenericStorage_load_fails_on_nonexistent_key);
  RUN_TEST(test_GenericStorage_clearDirty);
  RUN_TEST(test_GenericStorage_multiple_saves);
}

#ifndef UNITY_NATIVE
void setup() {
  delay(1000); // Wait for Preferences to be ready
#else
int main(int argc, char **argv) {
#endif
  UNITY_BEGIN();
  run_GenericStorage_tests();
  UNITY_END();
}

void loop() {
  // No loop needed
}

#endif