#ifndef TGENERICSTORAGE_H
#define TGENERICSTORAGE_H

#include <mutex>
#include <submodules/Storage/GenericStorage.h>

/**
 * @brief Thread-safe wrapper for GenericStorage.
 *
 * The ThreadSafeGenericStorage class provides a thread-safe interface
 * for the GenericStorage class by using mutexes to synchronize access
 * to the underlying storage operations.
 * @tparam DATA The type of data to be stored. Must support serialization.
 */
template <typename DATA>
class ThreadSafeGenericStorage
{
private:
  // Underlying GenericStorage instance
  GenericStorage<DATA> genericStorage;

  // Mutex for synchronizing access
  mutable std::mutex mtx;

public:
  /**
   * @brief Constructor for ThreadSafeGenericStorage.
   * @param storage Reference to an IStorage implementation for data operations.
   * @param key The key under which the data will be stored.
   */
  ThreadSafeGenericStorage(const char *key, IStorage *storageBackend = nullptr)
      : genericStorage(key, storageBackend) {}

  // Prevent copying
  ThreadSafeGenericStorage(const ThreadSafeGenericStorage &) = delete;
  ThreadSafeGenericStorage &
  operator=(const ThreadSafeGenericStorage &) = delete;

  // Prevent moving (mutex is not movable)
  ThreadSafeGenericStorage(ThreadSafeGenericStorage &&) = delete;
  ThreadSafeGenericStorage &operator=(ThreadSafeGenericStorage &&) = delete;

  /**
   * @brief Retrieve the stored data.
   * @return The stored data of type DATA.
   */
  DATA get() const
  {
    std::lock_guard<std::mutex> lock(mtx);
    return genericStorage.get();
  }

  /**
   * @brief Set new data to be stored.
   * @param in The data to be stored.
   */
  void set(const DATA &in)
  {
    std::lock_guard<std::mutex> lock(mtx);
    genericStorage.set(in);
  }

  /**
   * @brief Check if the data has been modified since the last save.
   * @return True if the data is dirty, false otherwise.
   */
  bool isDirty() const
  {
    std::lock_guard<std::mutex> lock(mtx);
    return genericStorage.isDirty();
  }

  /**
   * @brief Clear the dirty flag after saving.
   */
  void clearDirty()
  {
    std::lock_guard<std::mutex> lock(mtx);
    genericStorage.clearDirty();
  }

  /**
   * @brief Load data from storage.
   * @return True if loading was successful and data is valid, false otherwise.
   */
  bool load()
  {
    std::lock_guard<std::mutex> lock(mtx);
    return genericStorage.load();
  }

  /**
   * @brief Save data to storage.
   * @return True if saving was successful, false otherwise.
   */
  bool save()
  {
    std::lock_guard<std::mutex> lock(mtx);
    return genericStorage.save();
  }

  /**
   * @brief Clear all data from storage.
   * @return True if clear was successful, false otherwise.
   */
  bool clearAll()
  {
    std::lock_guard<std::mutex> lock(mtx);
    return genericStorage.clearAll();
  }
};

#endif