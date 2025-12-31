#ifndef TGENERICSTORAGE_H
#define TGENERICSTORAGE_H

#include <mutex>
#include <submodules/Storage/GenericStorage.h>

template <typename DATA> class ThreadSafeGenericStorage {
private:
  GenericStorage<DATA> storage;
  mutable std::mutex mtx;

public:
  ThreadSafeGenericStorage(IStorage &storage, const char *key) : storage(storage, key) {}

  // Prevent copying
  ThreadSafeGenericStorage(const ThreadSafeGenericStorage &) = delete;
  ThreadSafeGenericStorage &
  operator=(const ThreadSafeGenericStorage &) = delete;

  // Prevent moving (mutex is not movable)
  ThreadSafeGenericStorage(ThreadSafeGenericStorage &&) = delete;
  ThreadSafeGenericStorage &operator=(ThreadSafeGenericStorage &&) = delete;

  DATA get() const {
    std::lock_guard<std::mutex> lock(mtx);
    return storage.get();
  }

  void set(const DATA &in) {
    std::lock_guard<std::mutex> lock(mtx);
    storage.set(in);
  }

  bool isDirty() const {
    std::lock_guard<std::mutex> lock(mtx);
    return storage.isDirty();
  }

  void clearDirty() {
    std::lock_guard<std::mutex> lock(mtx);
    storage.clearDirty();
  }

  bool load() {
    std::lock_guard<std::mutex> lock(mtx);
    return storage.load();
  }

  bool save() {
    std::lock_guard<std::mutex> lock(mtx);
    return storage.save();
  }
};

#endif