#include <string.h>
#include "spritetext.h"
#include "tonc_oam.h"
#include "tonc_types.h"
#include "tonc_memdef.h"
#include "tonc_memmap.h"
#include "tonc_math.h"
#include "lap.h"

#define START_X     200
#define END_X       -200
#define SHADOW_TILE 38

Letter letters[NUM_LETTERS];
OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

vec2_t origin = VEC2(16 - (LETTER_SIZE * 2), 80 - (LETTER_SIZE / 2));
vec2_t side_origin = VEC2(0, 100 - (LETTER_SIZE / 2));
vec2_t offscreen_origin = VEC2(256, 100 - (LETTER_SIZE / 2));

extern s16 sine_table[1024];

const char text_scroller[] = "YO!! WHAT IS UP!  AYCE HERE WITH SOME COMPOFILLER PRODUCED UNDER EXTREME DURESS! NOTHING LIKE MAKING A PROD MOMENTS BEFORE DEADLINE, EH? CODE: NATT AND TFX   GRAPHICS: GRACIOUSLY CREATED BY HIIJ   MUSIC: SHAMELESSLY REPOSTED FROM MY ENTRY TO CHIPCHOP17. THANKS FOR ORGANIZING SUCH A COOL DISK RAMON!            ";
int text_scroller_ind = 0;

int map_char_to_int(const char c);

void init_letters() {
    oam_init(obj_buffer, 128);
    // draw wheel holder to the last slot
    obj_buffer[127].attr0 = ATTR0_Y(HOLDER_Y);
    obj_buffer[127].attr2 = ATTR2_ID(HOLDER_ADDR>>5) | ATTR2_PRIO(2) | ATTR2_PALBANK(1);
    dma3_fill(letters, 0, sizeof(letters));
    text_scroller_ind = 0;
}

int map_char_to_int(const char c) {
    if (c >= 65 && c <= 90) return c - 65;
    if (c >= 48 && c <= 56) return c - 20;
    switch (c) {
        case 32: return -2;
        case 33: return 36;
        case 63: return 37;
        case 46: return 40;
        case 44: return 41;
        case 58: return 42;
        case 59: return 43;
        default: return -1;
    }
}

void add_letter(Letter* letter, int c, uint tick) {
    letter->pos = START_X;
    letter->attr[0].attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(1) | tileOffset(c);
    letter->attr[1].attr2 = ATTR2_PALBANK(0) | ATTR2_PRIO(1) | tileOffset(SHADOW_TILE);
    letter->attr[0].fill = TRUE;
}

void update_letters(uint tick) {
    // add a new letter every 32 ticks
    if ((tick & 31) == 0) {
        int i = 0;
        // find the first dead slot
        while (i < (NUM_LETTERS - 1)) {
            if (!letters[i].attr[0].fill) break;
            i++;
        }
        int nextLetter = map_char_to_int(text_scroller[text_scroller_ind++]);
        if (text_scroller_ind >= (sizeof(text_scroller) - 1)) {
            text_scroller_ind = 0;
        }
        // skip if it's a space
        if (nextLetter != -2) add_letter(&letters[i], nextLetter, tick);
    }
    // update positions and pick visible letters
    int visible[NUM_LETTERS];
    int num_visible = 0;
    for (int i = 0; i < NUM_LETTERS; i++) {
        Letter* letter = &letters[i];
        // skip if dead
        if (!letter->attr[0].fill) continue;

        s32 scale = sine_table[(tick*8+letter->pos)&1023] + (1 << 16);
        s32 iscale = inv_scalar(scale);
        s32 xpos = (letter->pos << 6) * iscale;
        if ((xpos >= -(120+32) << 22) && (xpos < (120+32) << 22)) {
            s32 ypos = ((sine_table[(tick*11-letter->pos)&1023] >> 4) - (4 << 6)) * iscale;
            s32 spos = (iscale - (1 << 16)) * ((1 << 22) / FLOOR_TILT);
            xpos = ((xpos >> 22) + 120 - 32) & ATTR1_X_MASK;
            ypos = ((ypos >> 22) + 80 - 32) & ATTR0_Y_MASK;
            spos = ((spos >> 22) + 120 - 32) & ATTR0_Y_MASK;
            letter->attr[1].fill = TRUE;
            letter->attr[0].attr0 = ATTR0_Y(ypos) | ATTR0_SQUARE | ATTR0_AFF_DBL;
            letter->attr[0].attr1 = ATTR1_X(xpos) | ATTR1_SIZE_32;
            letter->attr[1].attr0 = ATTR0_Y(spos) | ATTR0_SQUARE | ATTR0_AFF_DBL;
            letter->attr[1].attr1 = ATTR1_X(xpos) | ATTR1_SIZE_32;
            letter->scale = scale >> 8;
        } else {
            letter->attr[1].fill = FALSE;
        }

        if (letter->pos-- == END_X) {
            // mark dead
            letter->attr[0].fill = FALSE;
            letter->attr[1].fill = FALSE;
            continue;
        }
        if (letter->attr[1].fill) {
            visible[num_visible++] = i;
        }
    }
    // sort by pos from center
    for (int i = 0; i < (num_visible - 1); i++) {
        int minj = i;
        s16 ipos = ABS(letters[visible[i]].pos);
        for (int j = i + 1; j < num_visible; j++) {
            if (ABS(letters[visible[j]].pos) < ipos) minj = j;
        }
        if (minj != i) {
            int tmp = visible[i];
            visible[i] = visible[minj];
            visible[minj] = tmp;
        }
    }
    // put into obj_buffer
    dma3_fill(obj_buffer, ATTR0_Y(240) | ATTR0_HIDE, sizeof(OBJ_ATTR)*NUM_LETTERS*2);
    for (int i = 0; i < num_visible; i++) {
        Letter* letter = &letters[visible[i]];
        obj_buffer[i].attr0 = letter->attr[0].attr0;
        obj_buffer[i].attr1 = letter->attr[0].attr1 | ATTR1_AFF_ID(i);
        obj_buffer[i].attr2 = letter->attr[0].attr2;
        obj_buffer[i+num_visible].attr0 = letter->attr[1].attr0;
        obj_buffer[i+num_visible].attr1 = letter->attr[1].attr1 | ATTR1_AFF_ID(i);
        obj_buffer[i+num_visible].attr2 = letter->attr[1].attr2;
        obj_aff_buffer[i].pa = letter->scale;
        obj_aff_buffer[i].pd = letter->scale;
    }
}

void render_letters(int wheel_x) {
    obj_buffer[127].attr1 = ATTR1_X(-(wheel_x + HOLDER_X) & ATTR1_X_MASK) | ATTR1_SIZE_64x64;
    dma3_cpy(oam_mem, obj_buffer, OAM_SIZE);
}


// read from ROM location
// text packed in format from old scroller, but simplified
// extra character for HOLD

// 0 - empty
// 1-26 - A-Z
// 27-37 - 0-9
// 38 - !
// 39 - ?
// 40 - .
// 41 - HOLD

// if HOLD, next 2-bytes is hold time (frames) 65535 frames (1k seconds)

// letters can be ~10 (8 max on screen at once, with some clipping)