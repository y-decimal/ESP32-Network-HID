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
        MODE_NKRO,
        COUNT
    };

    static constexpr const uint8_t getKroModeValue(KroMode mode) { return static_cast<uint8_t>(mode); }

    virtual bool initialize() = 0;
    virtual void sendHidReport(const uint8_t *hidBitmap, size_t size) = 0;
    virtual KroMode getKroMode() = 0;
};

#endif