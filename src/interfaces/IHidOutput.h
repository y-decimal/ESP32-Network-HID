#ifndef IHIDOUTPUT_H
#define IHIDOUTPUT_H

#include <string>

class IHidOutput
{
public:
    virtual ~IHidOutput() = default;

    enum class KroMode : uint8_t
    {
        MODE_6KRO,
        MODE_NKRO
    };

    virtual void sendHidReport(uint8_t *hidBitmap, size_t size) = 0;
    virtual KroMode getKroMode() = 0;
};

#endif