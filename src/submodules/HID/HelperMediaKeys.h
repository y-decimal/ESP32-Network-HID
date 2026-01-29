#ifndef HELPERMEDIAKEYS_H
#define HELPERMEDIAKEYS_H

#include <HIDTypes.h>

#include <string>
#include <vector>

class MediaKeysHelper
{
public:
    static constexpr const char *NAMESPACE = "MediaKeysHelper";

    static uint8_t reportDescriptorMediaControls[31];

private:
    uint8_t mediaKeyBitmap[2] = {0};
};
#endif