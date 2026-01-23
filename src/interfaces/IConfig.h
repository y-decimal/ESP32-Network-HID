#ifndef ICONFIG_H
#define ICONFIG_H

#include <interfaces/ISerializable.h>
#include <interfaces/IStorage.h>

class IConfig : public ISerializable
{
public:
    virtual ~IConfig() = default;

    virtual void setStorage(IStorage storage) = 0;

    virtual bool save() = 0;
    virtual bool load() = 0;
};
#endif