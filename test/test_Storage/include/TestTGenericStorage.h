#ifndef TESTTGENERICSTORAGE_H
#define TESTTGENERICSTORAGE_H

#include <submodules/Storage/TGenericStorage.h>
#include <unity.h>
#include "StorageTestCommon.h"

void test_ThreadSafeGenericStorage_initialization()
{
  struct TestData
  {
    int value;
    char name[10];
  };

  ThreadSafeGenericStorage<TestData> storage(TEST_KEY, &testStorage);
  TEST_ASSERT_FALSE(storage.isDirty());
}

void test_ThreadSafeGenericStorage_set_marks_dirty()
{
  struct TestData
  {
    int value;
  };

  ThreadSafeGenericStorage<TestData> storage(TEST_KEY, &testStorage);
  TestData data = {42};

  storage.set(data);
  TEST_ASSERT_TRUE(storage.isDirty());
  TEST_ASSERT_EQUAL(42, storage.get().value);
}

void test_ThreadSafeGenericStorage_save_clears_dirty()
{
  struct TestData
  {
    int value;
  };

  ThreadSafeGenericStorage<TestData> storage(TEST_KEY, &testStorage);
  TestData data = {42};

  storage.set(data);
  TEST_ASSERT_TRUE(storage.isDirty());

  bool saved = storage.save();
  TEST_ASSERT_TRUE(saved);
  TEST_ASSERT_FALSE(storage.isDirty());
}

void test_ThreadSafeGenericStorage_load_restores_data()
{
  struct TestData
  {
    int value;
    char text[5];
  };

  ThreadSafeGenericStorage<TestData> storage1(LOAD_KEY, &testStorage);
  TestData data = {123, "test"};

  storage1.set(data);
  storage1.save();

  ThreadSafeGenericStorage<TestData> storage2(LOAD_KEY, &testStorage);
  bool loaded = storage2.load();

  TEST_ASSERT_TRUE(loaded);
  TEST_ASSERT_EQUAL(123, storage2.get().value);
  TEST_ASSERT_EQUAL_STRING("test", storage2.get().text);
  TEST_ASSERT_FALSE(storage2.isDirty());
}

void test_ThreadSafeGenericStorage_load_fails_on_nonexistent_key()
{
  struct TestData
  {
    int value;
  };

  ThreadSafeGenericStorage<TestData> storage(NONEXISTENT_KEY, &testStorage);
  bool loaded = storage.load();

  TEST_ASSERT_FALSE(loaded);
}

void test_ThreadSafeGenericStorage_multiple_saves()
{
  struct TestData
  {
    int value;
  };

  ThreadSafeGenericStorage<TestData> storage(MULTI_KEY, &testStorage);

  TestData data1 = {100};
  storage.set(data1);
  storage.save();

  TestData data2 = {200};
  storage.set(data2);
  storage.save();

  ThreadSafeGenericStorage<TestData> storage2(MULTI_KEY, &testStorage);
  storage2.load();

  TEST_ASSERT_EQUAL(200, storage2.get().value);
}

void run_ThreadSafeGenericStorage_tests()
{
  RUN_TEST(test_ThreadSafeGenericStorage_initialization);
  RUN_TEST(test_ThreadSafeGenericStorage_set_marks_dirty);
  RUN_TEST(test_ThreadSafeGenericStorage_save_clears_dirty);
  RUN_TEST(test_ThreadSafeGenericStorage_load_restores_data);
  RUN_TEST(test_ThreadSafeGenericStorage_load_fails_on_nonexistent_key);
  RUN_TEST(test_ThreadSafeGenericStorage_multiple_saves);
}

#endif