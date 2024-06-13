#include <stdint.h>
#include <stdlib.h>
#include "maxmod.h"
#include "tonc_bios.h"
#include "tonc_irq.h"
#include "tonc_memmap.h"
#include "tonc_text.h"

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
extern s16 sine_table[1024];

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
    REG_DISPCNT = DCNT_BLANK;
    REG_BG0CNT = BG_CBB(2) | BG_SBB(30) | BG_REG_64x32;
    REG_BG1CNT = BG_CBB(2) | BG_SBB(28) | BG_REG_64x32;
    REG_BG2CNT = BG_CBB(0) | BG_SBB( 0) | BG_AFF_16x16;

    txt_bup_1toX((void*)(MEM_VRAM), toncfontTiles, toncfontTilesLen, 8, 0);
    txt_bup_1toX((void*)(MEM_VRAM+32768), toncfontTiles, toncfontTilesLen, 4, 0);
    txt_bup_1toX((void*)(MEM_VRAM_OBJ), toncfontTiles, toncfontTilesLen, 4, 0);

    for (int i = 0; i < 64*32; i++) {
        if (i < 128) se_mem[0][i] = ((i * 2 + 1) << 8) | (i * 2);
        se_mem[28][i] = (i & 1023);
        se_mem[30][i] = (i & 1023) | SE_HFLIP;
    }
    pal_bg_mem[1]=0xffff;
    
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

    REG_DISPCNT = DCNT_MODE1 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;

    init_letters(letters);

    irq_init(NULL);
    irq_add(II_VBLANK, &vbi);
    irq_add(II_HBLANK, &hbi);
    irq_enable(II_VBLANK);
    irq_enable(II_HBLANK);

    int frame = 0;
    
    while (1)
    {
        VBlankIntrWait();

        s16 sr = sine_table[frame&1023];
        s16 cr = sine_table[(frame+256)&1023];
        REG_BG2PA = cr >> 7;
        REG_BG2PB = sr >> 7;
        REG_BG2PC = -sr >> 7;
        REG_BG2PD = cr >> 7;
        REG_BG2X = (((s32)sr) >> 1) - (160 << 8);
        REG_BG2Y = (((s32)cr) >> 1) - (40 << 8);

        pal_bg_mem[0]=0xffff;
        mmFrame();
        pal_bg_mem[0]=0;
        uint j = 0;
        for (int i = 0; i < NUM_LETTERS; i++)
        {
            update_letter(&letters[i], tick, j * 3 << 6);
            j++;
        }
        render_letters();

        frame++;
    }
    return 0;
}
