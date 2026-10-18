#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "legacy_ipl_boot.h"

static uint8_t image[0x40000], original[0x40000];
static void word(size_t offset, uint16_t value)
{
    image[offset ^ 1] = (uint8_t)(value >> 8);
    image[(offset + 1) ^ 1] = (uint8_t)value;
}

int main(void)
{
    assert(!LegacyIPL_UpdateBootProbe(NULL, 0x40000, 0xbffffc));
    assert(!LegacyIPL_UpdateBootProbe(image, 0x3ffff, 0xbffffc));
    assert(!LegacyIPL_UpdateBootProbe(image, sizeof(image), 0xbffffc));
    for (size_t bank = 0; bank <= 0x20000; bank += 0x20000) {
        size_t base = bank + 0x10236;
        const uint16_t signature[] = {0x2079, 0x00ed, 0x000c, 0x2c4f,
                                      0x43f9, 0x00ff, 0x0264};
        for (size_t n = 0; n < sizeof(signature)/sizeof(signature[0]); n++)
            word(base + n * 2, signature[n]);
        word(bank + 0x10260, 0x4e93);
        word(bank + 0x10262, 0x4e75);
        word(bank + 0x10264, 0x2e4e);
    }
    memcpy(original, image, sizeof(image));
    assert(!LegacyIPL_UpdateBootProbe(image, sizeof(image), 0xea0020));
    assert(!memcmp(original, image, sizeof(image)));
    assert(LegacyIPL_UpdateBootProbe(image, sizeof(image), 0xbffffc) == 2);
    assert(LegacyIPL_ReadWord(image, 0x30260) == 0x4e71);
    assert(LegacyIPL_ReadWord(image, 0x10260) == 0x4e71);
    assert(!LegacyIPL_UpdateBootProbe(image, sizeof(image), 0xbffffc));
    assert(LegacyIPL_UpdateBootProbe(image, sizeof(image), 0xea0020) == 2);
    assert(!memcmp(original, image, sizeof(image)));
    assert(LegacyIPL_UpdateBootProbe(image, sizeof(image), 0) == 2);
    assert(!LegacyIPL_UpdateBootProbe(image, sizeof(image), UINT32_MAX));
    assert(LegacyIPL_UpdateBootProbe(image, sizeof(image), 0xea0020) == 2);
    word(0x30236, 0x4e71);  // A different ROM must not be patched.
    assert(LegacyIPL_UpdateBootProbe(image, sizeof(image), 0xbffffc) == 1);
    assert(LegacyIPL_ReadWord(image, 0x30260) == 0x4e93);
    puts("PASS: dummy ROM probe, mirrored IPL, real ROM boot, unknown IPL, bounds and reset restoration");
    return 0;
}
