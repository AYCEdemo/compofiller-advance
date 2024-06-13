#include <stdint.h>
#include <stdlib.h>
#include "maxmod.h"
#include "tonc_bios.h"
#include "tonc_irq.h"
#include "tonc_memmap.h"

#include "../res/sound.h"

#include "spritetext.h"

#define MM_CHS 8

typedef struct {
    u8 mod[MM_SIZEOF_MODCH*MM_CHS];
    u8 act[MM_SIZEOF_ACTCH*MM_CHS];
    u8 mix[MM_SIZEOF_MIXCH*MM_CHS];
    u8 wave[MM_MIXLEN_31KHZ];
} mm_buf_t;

u8 mm_mixbuf[MM_MIXLEN_31KHZ];

extern u8 soundbank[];

Letter letters[32];
uint tick = 0;

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

    REG_DISPCNT = DCNT_OBJ | DCNT_OBJ_2D;

    init_letters(letters);

    irq_init(NULL);
    irq_add(II_VBLANK, &vbi);
    irq_add(II_HBLANK, &hbi);
    irq_enable(II_VBLANK);
    irq_enable(II_HBLANK);


    while (1)
    {
        VBlankIntrWait();
        //pal_bg_mem[0]=0xffff;
        mmFrame();
        uint j = 0;
        for (int i = 0; i < NUM_LETTERS; i++)
        {
            update_letter(&letters[i], tick, j * 3 << 6);
            j++;
        }
        render_letters();
        tick++;
        //pal_bg_mem[0]=0;
    }
    return 0;
}
