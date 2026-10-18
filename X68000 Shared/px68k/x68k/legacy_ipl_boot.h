#ifndef X68K_LEGACY_IPL_BOOT_H
#define X68K_LEGACY_IPL_BOOT_H

#include <stddef.h>
#include <stdint.h>

static inline uint16_t LegacyIPL_ReadWord(const uint8_t *ipl, size_t offset)
{
    return (uint16_t)((ipl[offset ^ 1] << 8) | ipl[(offset + 1) ^ 1]);
}

/* IPL 1.0's dummy ROM handle ($BFFFFC) falls inside 12 MB RAM. When no
 * disk boots, its optional ROM probe calls a zero RAM pointer and never
 * reaches the insert-disk screen. Skip only that dummy probe, in the loaded
 * image; a real ROM handle retains the original JSR. The file is unchanged.
 * See https://www.stdkmd.net/bugsx68k/ (ROM 1.0, 12 MB boot failure).
 */
static inline int LegacyIPL_UpdateBootProbe(uint8_t *ipl, size_t size,
                                          uint32_t romBootHandle)
{
    if (!ipl || size < 0x40000) return 0;
    int changed = 0;
    for (size_t bank = 0; bank <= 0x20000; bank += 0x20000) {
        size_t base = bank + 0x10236;
        size_t call = bank + 0x10260;
        uint16_t opcode = LegacyIPL_ReadWord(ipl, call);
        /* Match the known probe: MOVEA.L $ED000C,A0; MOVEA.L A7,A6;
         * LEA $FF0264,A1, ending in JSR (A3); RTS; MOVEA.L A6,A7.
         * Unknown ROM layouts are left untouched. */
        if (LegacyIPL_ReadWord(ipl, base) != 0x2079 ||
            LegacyIPL_ReadWord(ipl, base + 2) != 0x00ed ||
            LegacyIPL_ReadWord(ipl, base + 4) != 0x000c ||
            LegacyIPL_ReadWord(ipl, base + 6) != 0x2c4f ||
            LegacyIPL_ReadWord(ipl, base + 8) != 0x43f9 ||
            LegacyIPL_ReadWord(ipl, base + 10) != 0x00ff ||
            LegacyIPL_ReadWord(ipl, base + 12) != 0x0264 ||
            (opcode != 0x4e93 && opcode != 0x4e71) ||
            LegacyIPL_ReadWord(ipl, call + 2) != 0x4e75 ||
            LegacyIPL_ReadWord(ipl, call + 4) != 0x2e4e) continue;
        /* Blank SRAM is initialized to the same dummy handle by IPL 1.0. */
        int dummy = romBootHandle == 0x00bffffc || romBootHandle == 0 ||
                    romBootHandle == UINT32_MAX;
        uint16_t desired = dummy ? 0x4e71 : 0x4e93;
        if (opcode != desired) {
            ipl[call ^ 1] = (uint8_t)(desired >> 8);
            ipl[(call + 1) ^ 1] = (uint8_t)desired;
            changed++;
        }
    }
    return changed;
}

#endif
