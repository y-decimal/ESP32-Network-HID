#ifndef PREFERENCESSTORAGE_H
#define PREFERENCESSTORAGE_H

#include <Preferences.h>
#include <interfaces/IStorage.h>

/**
 * @brief Preferences-based storage implementation.
 *
 * The PreferencesStorage class provides an implementation of the IStorage
 * interface using the Preferences library for persistent storage on ESP32.
 */
class PreferencesStorage : public IStorage
{
private:
  // Preferences instance for storage operations
  Preferences prefs;

  // Namespace for preferences
  const char *namespaceName;

public:
  /**
   * @brief Constructor for PreferencesStorage.
   * @param namespaceName The namespace to use for storing preferences.
   */
  PreferencesStorage(const char *namespaceName)
      : namespaceName(namespaceName) {}

  /**
   * @brief Save data to preferences.
   * @param key The key under which the data will be stored.
   * @param in Pointer to the data to be stored.
   * @param size Size of the data to be stored.
   * @return True if saving was successful, false otherwise.
   */
  bool save(const std::string &key, const uint8_t *in, size_t size) override
  {
    prefs.begin(namespaceName, false);
    size_t written = prefs.putBytes(key.c_str(), in, size);
    prefs.end();
    return written == size;
  }

  /**
   * @brief Load data from preferences.
   * @param key The key under which the data is stored.
   * @param out Pointer to the buffer where the data will be loaded.
   * @param size Size of the data to be loaded.
   * @return True if loading was successful, false otherwise.
   */
  bool load(const std::string &key, uint8_t *out, size_t size) override
  {

    if (!exists(key))
    {
      return false;
    }

    prefs.begin(namespaceName, true);
    size_t read = prefs.getBytes(key.c_str(), out, size);
    prefs.end();
    return read == size;
  }

  /**
   * @brief Remove data from preferences.
   * @param key The key under which the data is stored.
   * @return True if removal was successful, false otherwise.
   */
  bool remove(const std::string &key) override
  {

    if (!exists(key))
    {
      return false;
    }

    prefs.begin(namespaceName, false);
    prefs.remove(key.c_str());
    prefs.end();
    return true;
  }

  /**
   * @brief Check if a key exists in preferences.
   * @param key The key to check for existence.
   * @return True if the key exists, false otherwise.
   */
  bool exists(const std::string &key) override
  {
    prefs.begin(namespaceName, true);
    bool exists = prefs.isKey(key.c_str());
    prefs.end();
    return exists;
  }

  size_t getSize(const std::string &key) override
  {
    if (!exists(key))
      return 0;

    prefs.begin(namespaceName, true);
    size_t length = prefs.getBytesLength(key.c_str());
    prefs.end();
    return length;
  }

  bool clearAll() override
  {
    prefs.begin(namespaceName, false);
    bool result = prefs.clear();
    prefs.end();
    return result;
  }
};

#endif