#ifndef PREFERENCESSTORAGE_H
#define PREFERENCESSTORAGE_H

#include <Preferences.h>
#include <interfaces/IStorage.h>

class PreferencesStorage : public IStorage {
private:
  Preferences prefs;
  const char *namespaceName;

public:
  PreferencesStorage(const char *namespaceName)
      : namespaceName(namespaceName) {}

  bool save(const std::string &key, const uint8_t *in, size_t size) override {
    prefs.begin(namespaceName, false);
    size_t written = prefs.putBytes(key.c_str(), in, size);
    prefs.end();
    return written == size;
  }

  bool load(const std::string &key, uint8_t *out, size_t size) override {

    if (!exists(key)) {
      return false;
    }

    prefs.begin(namespaceName, true);
    size_t read = prefs.getBytes(key.c_str(), out, size);
    prefs.end();
    return read == size;
  }

  bool remove(const std::string &key) override {

    if (!exists(key)) {
      return false;
    }

    prefs.begin(namespaceName, false);
    prefs.remove(key.c_str());
    prefs.end();
    return true;
  }

  bool exists(const std::string &key) override {
    prefs.begin(namespaceName, true);
    bool exists = prefs.isKey(key.c_str());
    prefs.end();
    return exists;
  }
};

#endif