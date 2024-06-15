#include <stdint.h>
#include <stdlib.h>
#include "maxmod.h"
#include "tonc_bios.h"
#include "tonc_irq.h"
#include "tonc_memmap.h"
#include "tonc_text.h"
#include "tonc_video.h"

#include "../res/sound.h"

#include "spritetext.h"

#define MM_CHS   8
#define MAX_GRAD 31
#define TILT     (1 << 10)

typedef struct {
    u8 mod[MM_SIZEOF_MODCH*MM_CHS];
    u8 act[MM_SIZEOF_ACTCH*MM_CHS];
    u8 mix[MM_SIZEOF_MIXCH*MM_CHS];
    u8 wave[MM_MIXLEN_31KHZ];
} mm_buf_t;

u8 mm_mixbuf[MM_MIXLEN_31KHZ];

struct {
    u32 cnt;
    u32 stat;
    u16 bgcnt[4];
    u16 bgofs[4][2];
    BgAffineDest affines[79];
} dma_data;
u16 cur_grad = 0;

extern u8 soundbank[];
extern s16 sine_table[1024];
extern u8 wheel_data[];
extern u32 wheel_data_size;
extern u8 wheel_map[];
extern u8 wheel_palette[];
extern u32 wheel_palette_size;
extern u8 sprites_data[];
extern u32 sprites_data_size;
extern u8 sprites_palette[];
extern u32 sprites_palette_size;

Letter letters[32];
s32 inv_scalar(const s32 x);
s32 mul_vec2(s32* a, const s32 b);
// void render_floor(BgAffineDest* affines, s32 pitch, s32 yaw, u32 lines);

void vbi() {
    mmVBlank();
    REG_DISPCNT = DCNT_MODE1 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ;
    REG_DISPSTAT = DSTAT_VBL_IRQ | DSTAT_VCT_IRQ | DSTAT_VCT(80);
    REG_BG0CNT = BG_CBB(2) | BG_SBB(30) | BG_REG_64x32;
    REG_BG1CNT = BG_CBB(2) | BG_SBB(28) | BG_REG_64x32;
    REG_BG2CNT = BG_CBB(0) | BG_SBB( 0) | BG_AFF_16x16;
    REG_DMA0CNT = 0;
    cur_grad = MAX_GRAD - 1;
}

void vci() {
    REG_BLDY = MAX_GRAD;
    REG_DISPSTAT = (u16)dma_data.stat; // needed to be written first so the next line can fire right away
    REG_DMA0SAD = (u32)&dma_data;
    REG_DMA0DAD = (u32)&REG_DISPCNT;
    REG_DMA0CNT = DMA_COUNT(12) | DMA_SRC_INC | DMA_DST_INC | DMA_AT_HBLANK | DMA_32NOW;
}

void hbi() {
    REG_BLDY = cur_grad >> 1;
    if (cur_grad == MAX_GRAD - 1) {
        REG_DMA0SAD = (u32)(&dma_data.affines[1]);
        REG_DMA0DAD = (u32)&REG_BG2PA;
        REG_DMA0CNT = DMA_COUNT(4) | DMA_SRC_INC | DMA_DST_RELOAD | DMA_AT_HBLANK | DMA_REPEAT | DMA_32NOW;
    }
    else if (cur_grad == 0) {
        REG_DISPSTAT &= ~DSTAT_HBL_IRQ;
    }
    cur_grad--;
}

int main() {
    // WS0=3/1, WS1=4/4, WS2=8/8, SRAM=8, prefetch on
    REG_WAITCNT = 0x4317;
    REG_DISPCNT = DCNT_BLANK;
    REG_DISPSTAT = 0;
    REG_BLDCNT = BLD_BG0 | BLD_BG1 | BLD_BG2 | BLD_BLACK;
    memset32(&dma_data, 0, sizeof(dma_data)/4);
    dma_data.cnt = DCNT_MODE1 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ;
    dma_data.stat = DSTAT_VBL_IRQ | DSTAT_HBL_IRQ;
    dma_data.bgcnt[0] = BG_CBB(2) | BG_SBB(30) | BG_REG_64x32;
    dma_data.bgcnt[1] = BG_CBB(2) | BG_SBB(28) | BG_REG_64x32;
    dma_data.bgcnt[2] = BG_CBB(1) | BG_SBB( 8) | BG_AFF_16x16 | BG_WRAP;

    // prepare graphics
    // grit couldn't bake offsets for some reason so we have to manually patch it
    u8* vram_buf = malloc(VRAM_BG_SIZE);
    for (int i = 0; i < 256; i++) {
        vram_buf[i] = wheel_map[i] + 4;
    }
    for (int i = 0; i < wheel_data_size; i++) {
        vram_buf[i+256] = (wheel_data[i] == 0) ? 0 : wheel_data[i] + 128;
    }
    memcpy32((void*)MEM_VRAM, vram_buf, VRAM_BG_SIZE/4);
    memcpy32((void*)MEM_VRAM_OBJ, sprites_data, sprites_data_size/4);
    memcpy16((void*)(MEM_PAL+128*2), wheel_palette, wheel_palette_size/2);
    memcpy16((void*)MEM_PAL_OBJ, sprites_palette, sprites_palette_size/2);
    free(vram_buf);

    txt_bup_1toX((void*)(MEM_VRAM+16384), toncfontTiles, toncfontTilesLen, 8, 0);
    txt_bup_1toX((void*)(MEM_VRAM+32768), toncfontTiles, toncfontTilesLen, 4, 0);

    for (int i = 0; i < 64*32; i++) {
        if (i < 128) se_mem[8][i] = ((i * 2 + 1) << 8) | (i * 2);
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

    init_letters(letters);

    vid_vsync(); // just to be safe, i'm sure irq_enable() also sets STAT
    REG_IF = -1;
    irq_init(NULL);
    irq_add(II_VBLANK, &vbi);
    irq_add(II_HBLANK, &hbi);
    irq_add(II_VCOUNT, &vci);
    irq_enable(II_VBLANK);
    irq_enable(II_HBLANK);
    irq_enable(II_VCOUNT);
    REG_DISPSTAT = DSTAT_VBL_IRQ;

    int frame = 0;
    s32 nsr = 0;
    s32 ncr = 0;
    s32 sr = 0;
    s32 cr = 0;
    
    while (1)
    {
        VBlankIntrWait();

        if ((frame & 1) == 1) {
            sr = (sr + nsr) / 2;
            cr = (cr + ncr) / 2;
        } else {
            sr = nsr;
            cr = ncr;
            nsr = sine_table[(frame/2)&1023];
            ncr = sine_table[(frame/2+256)&1023];
        }

        s32 px = 120;
        s32 py = 40;
        REG_BG2PA = cr >> 7;
        REG_BG2PB = sr >> 7;
        REG_BG2PC = -sr >> 7;
        REG_BG2PD = cr >> 7;
        REG_BG2X = ((-px * cr - py * sr) >> 7) + (64 << 8);
        REG_BG2Y = ((px * sr - py * cr) >> 7) + (64 << 8);

        s32 sf = sine_table[frame&1023];
        s32 cf = sine_table[(frame+256)&1023];
        s32 ps = (1 << 16) - TILT * 39;
        for (int i = 0; i < 79; i++) {
            s32 ips = inv_scalar(ps);
            s32 sc[2] = {sf, cf};
            mul_vec2(sc, ips);
            s32 pa = -sc[1];
            s32 pb = sc[0];
            s32 pc = sc[0];
            s32 pd = sc[1];
            dma_data.affines[i].pa = pa >> 7;
            dma_data.affines[i].pc = pc >> 7;
            dma_data.affines[i].dx = ((64 << 15) - (120 * pa + (i - 39) * pb)) >> 7;
            dma_data.affines[i].dy = ((64 << 15) - (120 * pc + (i - 39) * pd)) >> 7;
            ps += TILT;
        }

        pal_bg_mem[0]=0xffff;
        mmFrame();
        pal_bg_mem[0]=0;
        uint j = 0;
        for (int i = 0; i < NUM_LETTERS; i++)
        {
            update_letter(letters, &letters[i], frame, j * 64);
            j++;
        }
        render_letters(frame * 4);

        frame++;
    }
    return 0;
}
