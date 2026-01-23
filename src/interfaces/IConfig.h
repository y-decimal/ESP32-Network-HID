#ifndef ICONFIG_H
#define ICONFIG_H

#include <interfaces/ISerializable.h>
#include <interfaces/IStorage.h>

// IMPORTANT: All implementations must provide a static const char* NAMESPACE member
class IConfig : public ISerializable
{
public:
    virtual ~IConfig() = default;

    virtual void setStorage(IStorage *storage) = 0;

    virtual bool save() = 0;
    virtual bool load() = 0;
    virtual bool erase() = 0;
};
#endif