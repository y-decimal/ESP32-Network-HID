#ifndef ISTORAGE_H
#define ISTORAGE_H

#include <cstdint>
#include <string>

/**
 * @brief Interface for storage operations.
 *
 * The IStorage interface provides methods for saving, loading,
 * removing, and checking the existence of data associated with keys.
 */
class IStorage {
public:
  /**
   * @brief Save data to storage with a specific key.
   * @param key The key to associate with the data.
   * @param in Pointer to the input data to save.
   * @param size Size of the input data in bytes.
   * @return True if save was successful, false otherwise.
   */
  virtual bool save(const std::string &key, const uint8_t *in, size_t size) = 0;

  /**
   * @brief Load data from storage associated with a specific key.
   * @param key The key associated with the data to load.
   * @param out Pointer to the output buffer to store loaded data.
   * @param size Size of the output buffer in bytes.
   * @return True if load was successful, false otherwise.
   */
  virtual bool load(const std::string &key, uint8_t *out, size_t size) = 0;

  /**
   * @brief Remove data from storage associated with a specific key.
   * @param key The key associated with the data to remove.
   * @return True if removal was successful, false otherwise.
   */
  virtual bool remove(const std::string &key) = 0;

  /**
   * @brief Check if data exists in storage for a specific key.
   * @param key The key to check for existence.
   * @return True if data exists for the key, false otherwise.
   */
  virtual bool exists(const std::string &key) = 0;

  // Virtual destructor
  virtual ~IStorage() = default;
};

#endif