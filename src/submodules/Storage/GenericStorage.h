#ifndef GENERICSTORAGE_H
#define GENERICSTORAGE_H

#include <cstring>
#include <interfaces/IStorage.h>
#include <shared/GlobalHelpers.h>

#define NAMESPACE "GenericStorage"

template <typename DATA> class GenericStorage {
private:
  struct DataBlock {
    DATA data;
    uint8_t checksum;
  };

  DataBlock dataBlock;
  bool dirty = false;

  IStorage &storage;
  const std::string key;

public:
  GenericStorage(IStorage &storage, const char *key)
      : storage(storage), key(key) {}

  DATA get() const { return dataBlock.data; }

  void set(const DATA &in) {
    dataBlock.data = in;
    dirty = true;
  }

  bool isDirty() const { return dirty; }

  void clearDirty() { dirty = false; }

  bool load() {

    DataBlock dataBuffer;
    bool read = storage.load(key, reinterpret_cast<uint8_t *>(&dataBuffer),
                             sizeof(DataBlock));

    if (!read)
      return false;

    uint8_t chkSumBuffer = calcCheckSum_8Bit(dataBuffer.data);
    if (chkSumBuffer != dataBuffer.checksum)
      return false;

    dataBlock = dataBuffer;
    dirty = false;

    return true;
  }

  bool save() {
    dataBlock.checksum = calcCheckSum_8Bit(dataBlock.data);
    size_t written = storage.save(key, &dataBlock, sizeof(DataBlock));
    dirty = false;
    return written == sizeof(DataBlock);
  }
};

#endif