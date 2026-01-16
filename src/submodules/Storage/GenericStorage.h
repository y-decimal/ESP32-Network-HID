#ifndef GENERICSTORAGE_H
#define GENERICSTORAGE_H

#include <cstring>
#include <interfaces/IStorage.h>
#include <shared/GlobalHelpers.h>

/**
 * @brief Generic storage class for managing data persistence.
 *
 * The GenericStorage class provides a templated interface for storing
 * and retrieving data of any type that supports serialization.
 * It handles checksum calculation for data integrity verification
 * and interacts with a provided IStorage implementation for actual
 * read/write operations.
 * @tparam DATA The type of data to be stored. Must support serialization.
 */
template <typename DATA> class GenericStorage {
private:
  // Internal structure to hold data and its checksum
  struct DataBlock {
    DATA data;
    uint8_t checksum;
  };

  DataBlock dataBlock;
  bool dirty = false;

  // Reference to the storage interface for hardware independent operations
  IStorage &storage;
  const std::string key;

public:
  /**
   * @brief Constructor for GenericStorage.
   * @param storage Reference to an IStorage implementation for data operations.
   * @param key The key under which the data will be stored.
   */
  GenericStorage(IStorage &storage, const char *key)
      : storage(storage), key(key) {}

  /**
   * @brief Retrieve the stored data.
   * @return The stored data of type DATA.
   */
  DATA get() const { return dataBlock.data; }

  /**
   * @brief Set new data to be stored.
   * @param in The data to be stored.
   */
  void set(const DATA &in) {
    dataBlock.data = in;
    dirty = true;
  }

  /**
   * @brief Check if the data has been modified since the last save.
   * @return True if the data is dirty, false otherwise.
   */
  bool isDirty() const { return dirty; }

  /**
   * @brief Clear the dirty flag after saving.
   */
  void clearDirty() { dirty = false; }

  /**
   * @brief Load data from storage.
   * @return True if loading was successful and data is valid, false otherwise.
   */
  bool load() {

    DataBlock dataBuffer;

    // Read data from storage
    bool read = storage.load(key, reinterpret_cast<uint8_t *>(&dataBuffer),
                             sizeof(DataBlock));

    if (!read)
      return false;

    // Verify checksum
    uint8_t chkSumBuffer = calcCheckSum_8Bit(dataBuffer.data);
    if (chkSumBuffer != dataBuffer.checksum)
      return false;

    dataBlock = dataBuffer;
    dirty = false;

    return true;
  }

  /**
   * @brief Save data to storage.
   * @return True if saving was successful, false otherwise.
   */
  bool save() {
    // Calculate and set checksum
    dataBlock.checksum = calcCheckSum_8Bit(dataBlock.data);

    // Write data to storage
    bool written = storage.save(key, reinterpret_cast<uint8_t *>(&dataBlock),
                                sizeof(DataBlock));
    if (!written)
      return false;

    dirty = false;
    return written;
  }
};

#endif