#ifndef _SPRITETEXT_H
#define _SPRITETEXT_H

#define LETTER_SIZE 32
#define NUM_LETTERS 9

#include "tonc_types.h"
#include "tonc_math.h"
#include "lap.h"

typedef struct Letter {
    uint32_t id;
    vec2_t curr_pos;
    FIXED scale;
    OBJ_ATTR *oam;
    OBJ_ATTR *shadow;
    int on_screen;
} Letter;

typedef struct Word {
    char* word;
    int* letterInds;
} Word;

typedef struct WordSent {
    u32 sent;
    Word word;
} WordSent;

inline u32 tileOffset(int i) { return ((i % 8) * 4) + ((i / 8) * 128); }

void init_letters(Letter* letters);
int add_letter(Letter* letters, int letter, vec2_t startingPos);
void kill_letter(Letter *letter);
void update_letter(Letter *letters, Letter *letter, uint tick, uint pos);
void render_letters(int tick);

#endif