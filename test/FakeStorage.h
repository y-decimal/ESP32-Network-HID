#ifndef TEST_FAKESTORAGE_H_
#define TEST_FAKESTORAGE_H_

#ifndef UNITY_NATIVE
#include <submodules/Storage/PreferencesStorage.h>
#endif

#include <cstring>
#include <interfaces/IStorage.h>
#include <unordered_map>
#include <vector>

class FakeStorage : public IStorage {

private:
  std::unordered_map<std::string, std::vector<uint8_t>> storage;

public:
  bool save(const std::string &key, const uint8_t *in, size_t size) override {
    storage[key] = std::vector<uint8_t>(in, in + size);
    return true;
  }

  bool load(const std::string &key, uint8_t *out, size_t size) override {
    auto it = storage.find(key);
    if (it == storage.end() || it->second.size() != size) {
      return false;
    }
    std::memcpy(out, it->second.data(), size);
    return true;
  }

  bool remove(const std::string &key) override {
    return storage.erase(key) > 0;
  }

  bool exists(const std::string &key) override {
    return storage.find(key) != storage.end();
  }
};

#endif