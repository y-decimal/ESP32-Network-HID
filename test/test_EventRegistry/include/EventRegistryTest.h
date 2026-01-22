#ifndef EVENTREGISTRYTEST_H
#define EVENTREGISTRYTEST_H

#include <unity.h>
#include <submodules/EventRegistry.h>
#include <shared/EventTypes.h>

// Test callbacks
static int callback1_count = 0;
static int callback2_count = 0;
static Event last_event;

void test_callback_1(const Event &event)
{
    callback1_count++;
    last_event = event;
}

void test_callback_2(const Event &event)
{
    callback2_count++;
    last_event = event;
}

void clearEventRegistry()
{
    for (size_t i = 0; i < (size_t)EventType::COUNT; ++i)
    {
        EventRegistry::clearHandlers(static_cast<EventType>(i));
    }
}

void resetCallbackCounters()
{
    callback1_count = 0;
    callback2_count = 0;
    last_event = Event{};
}

void test_register_single_handler(void) {
  EventRegistry::registerHandler(EventType::RawKey, test_callback_1);

  auto handlers = EventRegistry::getHandler(EventType::RawKey);
  TEST_ASSERT_EQUAL(1, handlers.size());
}

void test_register_multiple_handlers_same_type(void) {
  EventRegistry::registerHandler(EventType::RawKey, test_callback_1);
  EventRegistry::registerHandler(EventType::RawKey, test_callback_2);

  auto handlers = EventRegistry::getHandler(EventType::RawKey);
  TEST_ASSERT_EQUAL(2, handlers.size());
}

void test_register_handlers_different_types(void) {
  EventRegistry::registerHandler(EventType::RawKey, test_callback_1);
  EventRegistry::registerHandler(EventType::RawBitmap, test_callback_2);

  auto handlers_press = EventRegistry::getHandler(EventType::RawKey);
  auto handlers_release = EventRegistry::getHandler(EventType::RawBitmap);
  TEST_ASSERT_EQUAL(1, handlers_press.size());
  TEST_ASSERT_EQUAL(1, handlers_release.size());
}

void test_get_handler_empty(void) {
  auto handlers = EventRegistry::getHandler(EventType::RawKey);
  TEST_ASSERT_EQUAL(0, handlers.size());
}

void test_handlers_are_callable(void) {
  EventRegistry::registerHandler(EventType::RawKey, test_callback_1);

  auto handlers = EventRegistry::getHandler(EventType::RawKey);
  TEST_ASSERT_EQUAL(1, handlers.size());

  Event test_event{};
  test_event.type = EventType::RawKey;
  handlers[0](test_event);

  TEST_ASSERT_EQUAL(1, callback1_count);
  TEST_ASSERT_EQUAL(EventType::RawKey, last_event.type);
}

void test_multiple_handlers_execution(void) {
  EventRegistry::registerHandler(EventType::RawKey, test_callback_1);
  EventRegistry::registerHandler(EventType::RawKey, test_callback_2);

  auto handlers = EventRegistry::getHandler(EventType::RawKey);

  Event test_event{};
  test_event.type = EventType::RawKey;

  for (auto &handler : handlers) {
    handler(test_event);
  }

  TEST_ASSERT_EQUAL(1, callback1_count);
  TEST_ASSERT_EQUAL(1, callback2_count);
}

void run_EventRegistry_tests()
{
    RUN_TEST(test_register_single_handler);
    RUN_TEST(test_register_multiple_handlers_same_type);
    RUN_TEST(test_register_handlers_different_types);
    RUN_TEST(test_get_handler_empty);
    RUN_TEST(test_handlers_are_callable);
    RUN_TEST(test_multiple_handlers_execution);
}



#endif