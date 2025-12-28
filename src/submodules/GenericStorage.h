#ifndef GENERICSTORAGE_H
#define GENERICSTORAGE_H

#include <Preferences.h>
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

  Preferences prefs;
  const char *key;

public:
  GenericStorage(const char *key) : key(key) {}

  Data get() const { return dataBlock.data; }

  void set(const DATA &in) {
    dataBlock.data = in;
    dirty = true;
  }

  bool isDirty() const { return dirty; }

  void clearDirty() { dirty = false; }

  bool load() {
    prefs.begin(NAMESPACE, true);

    if (!prefs.isKey(key)) {
      prefs.end();
      return false;
    }

    DataBlock dataBuffer;
    size_t read = prefs.getBytes(key, &dataBuffer, sizeof(DataBlock));
    prefs.end();

    if (read != sizeof(DataBlock))
      return false;

    uint8_t chkSumBuffer = calcCheckSum_8Bit(dataBuffer.data);
    if (chkSumBuffer != dataBuffer.checksum)
      return false;

    dataBlock = buffer;
    dirty = false;

    return true;
  }

  bool save() {
    dataBlock.checksum = calcCheckSum_8Bit(dataBlock.data);
    prefs.begin(NAMESPACE, false);
    size_t written = prefs.putBytes(key, &dataBlock, sizeof(DataBlock));
    prefs.end();
    dirty = false;
    return written == sizeof(DataBlock);
  }
};

#endif