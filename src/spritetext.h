#ifndef _SPRITETEXT_H
#define _SPRITETEXT_H

#define LETTER_SIZE 32
#define NUM_LETTERS 16

#define HOLDER_ADDR 0x5200
#define HOLDER_X    31
#define HOLDER_Y    17
#define FLOOR_TILT  (0x10000 / 40)

#include "tonc_types.h"
#include "lap.h"

typedef struct Letter {
    s32 pos;
    // [0].fill = alive
    // [1].fill = visible
    OBJ_ATTR attr[2];
    s16 scale;
} Letter;

inline u32 tileOffset(int i) { return ((i % 8) * 4) + ((i / 8) * 128); }

void init_letters();
void add_letter(Letter* letters, int letter, uint tick);
void update_letters(uint tick);
void render_letters(int wheel_x);

#endif