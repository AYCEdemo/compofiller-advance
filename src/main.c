#include <stdint.h>
#include <stdlib.h>
#include "maxmod.h"
#include "tonc_bios.h"
#include "tonc_irq.h"
#include "tonc_memmap.h"

#include "res/sound.h"

#define MM_CHS 8

typedef struct {
    u8 mod[MM_SIZEOF_MODCH*MM_CHS];
    u8 act[MM_SIZEOF_ACTCH*MM_CHS];
    u8 mix[MM_SIZEOF_MIXCH*MM_CHS];
    u8 wave[MM_MIXLEN_31KHZ];
} mm_buf_t;

u8 mm_mixbuf[MM_MIXLEN_31KHZ];

extern u8 soundbank[];

void vbi() {
    mmVBlank();
}

void hbi() {

}

int main() {
    // WS0=3/1, WS1=4/4, WS2=8/8, SRAM=8, prefetch on
    REG_WAITCNT = 0x4317;
    
    mm_buf_t* mm_buf = (mm_buf_t*)malloc(sizeof(mm_buf_t));
    mm_gba_system mm_setup = {
        MM_MIX_31KHZ,
        MM_CHS, MM_CHS,
        mm_buf->mod, mm_buf->act, mm_buf->mix,
        mm_mixbuf,
        mm_buf->wave,
        soundbank
    };
    mmInit(&mm_setup);
    mmStart(MOD_WILLOWS, MM_PLAY_LOOP);

    REG_DISPCNT = 0;

    irq_init(NULL);
    irq_add(II_VBLANK, &vbi);
    irq_add(II_HBLANK, &hbi);
    irq_enable(II_VBLANK);
    irq_enable(II_HBLANK);
    
    while (1)
    {
        VBlankIntrWait();
        pal_bg_mem[0]=0xffff;
        mmFrame();
        pal_bg_mem[0]=0;
    }
    return 0;
}
