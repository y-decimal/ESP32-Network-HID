#ifndef NULLSTORAGE_H
#define NULLSTORAGE_H

#include <interfaces/IStorage.h>

class NullStorage : public IStorage
{
public:
    bool save(const std::string &key, const uint8_t *in, size_t size) override { return true; }
    bool load(const std::string &key, uint8_t *out, size_t size) override { return true; }
    bool remove(const std::string &key) override { return true; }
    bool exists(const std::string &key) override { return true; }
    size_t getSize(const std::string &key) override { return 0; }
    bool clearAll() override { return true; }
};

#endif