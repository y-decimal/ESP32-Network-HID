#ifndef ISTORAGE_H
#define ISTORAGE_H

#include <cstdint>
#include <string>

class IStorage {
public:
  virtual bool save(const std::string &key, const uint8_t *in, size_t size) = 0;
  virtual bool load(const std::string &key, uint8_t *out, size_t size) = 0;
  virtual bool remove(const std::string &key) = 0;
  virtual bool exists(const std::string &key) = 0;

  virtual ~IStorage() = default;
};

#endif