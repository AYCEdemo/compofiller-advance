#include <string.h>
#include "spritetext.h"
#include "../res/ayceFontSheet.h"
#include "tonc_oam.h"
#include "tonc_types.h"
#include "tonc_memdef.h"
#include "tonc_memmap.h"
#include "tonc_math.h"
#include "lap.h"

#define MAX_SCALE 256
#define MID_SCALE 384
#define MIN_SCALE 512

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
u32 pb = 0;
vec2_t origin = VEC2(120 - (LETTER_SIZE * 2), 80 - (LETTER_SIZE / 2));
//Word debug_onscreen_word;
WordSent debug_sent;

extern s16 sine_table[1024];

typedef struct Debug_Letter_Sent {
    int sent;
    vec2_t vec;
    int l;
} Debug_Letter_Sent;

Debug_Letter_Sent debug_sent2;

// void set_word_pos(Letter*, int*);
void debug_set_word_pos(Letter*);
int debug_map_char_to_int(const char c);

Letter* init_letters(Letter* letters)
{
    memcpy32(&tile_mem[4], ayceFontSheetTiles, ayceFontSheetTilesLen/4);
    memcpy32(pal_obj_mem, ayceFontSheetPal, ayceFontSheetPalLen/4);
    oam_init(obj_buffer, 128);

    for (int i = 0; i < NUM_LETTERS; i++)
    {
        // 44 << 4 : good midsize
        letters[i] = (Letter){i, origin, 24 << 4, &obj_buffer[i], FALSE};
    }
    debug_set_word_pos(letters);
}


void queue_word(char* word) 
{
    int num = strlen(word);

}

int debug_map_char_to_int(const char c) {
    if (c >= 65 && c <= 90) return c - 65;
    if (c >= 48 && c <= 56) return c - 20;
    if (c == 33) return 36;
    if (c == 63) return 37;
    return -1;
}

// void set_word_pos(Letter* letters, int* word) {
//     int num = strlen(word);
//     int inds[num];
//     for (int i; i < num; i++)
//         inds[i] = add_letter(letters, debug_map_char_to_int(word[i]));
//     debug_onscreen_word = (Word) { word, inds };

//     debug_sent = (WordSent) { 999, debug_onscreen_word};
// }

void debug_set_word_pos(Letter* letters) {
    int inds2[4] = { 0, 1, 2, 3 };
    int inds[4];
    inds[0] = add_letter(letters, debug_map_char_to_int('A'), origin);
    inds[1] = add_letter(letters, debug_map_char_to_int('Y'), add_vec2(origin, VEC2(LETTER_SIZE + 1, 0)));
    inds[2] = add_letter(letters, debug_map_char_to_int('C'), add_vec2(origin, VEC2(LETTER_SIZE * 2 + 1, 0)));
    inds[3] = add_letter(letters, debug_map_char_to_int('E'), add_vec2(origin, VEC2(LETTER_SIZE * 3 + 1, 0)));
}

int add_letter(Letter* letters, int letter, vec2_t startingPos) {
    for (int i = 0; i < NUM_LETTERS; i++)
    {
        if (letters[i].on_screen) continue;
        letters[i].curr_pos = startingPos;
        letters[i].on_screen = TRUE;
        obj_set_attr(letters[i].oam,
            ATTR0_SQUARE | ATTR0_AFF_DBL,
            ATTR1_SIZE_32 | ATTR1_AFF_ID(i),
            ATTR2_PALBANK(6) | tileOffset(letter));
        obj_aff_identity(&obj_aff_buffer[i]);
        obj_set_pos(letters[i].oam, comp_to_int(letters[i].curr_pos.x), comp_to_int(letters[i].curr_pos.y));            
        return i;
    }
    return -1;
}

void update_letter(Letter *letter, uint tick, uint pos) {
    if (letter == NULL) return;
    if (!letter->on_screen) return;

    int32_t x = comp_to_int(letter->curr_pos.x);
    int32_t y = comp_to_int(letter->curr_pos.y);

    if (x < -32)
    {
        kill_letter(letter);
        return;
    }

    int32_t sine = sine_table[(tick + pos)&1023] / 2;
    letter->curr_pos.y += sine;

    letter->scale = ((sine_table[((tick) - pos)&1023]) >> 8) + MID_SCALE;
    obj_aff_rotscale(&obj_aff_buffer[letter->id], letter->scale, letter->scale, 0);
    obj_set_pos(letter->oam, comp_to_int(letter->curr_pos.x), comp_to_int(letter->curr_pos.y));            
}

void kill_letter(Letter* letter) {
    letter->on_screen = FALSE;
    memset32(letter->oam, 0, 2);
    obj_set_pos(letter->oam, 0, 160); //off screen
}

void render_letters() {
    oam_copy(oam_mem, obj_buffer, NUM_LETTERS);
}


// cool effect that will make you extremely nauseated
    // letter->scale += (sine_table[((tick / 4) - pos)&1023] / 4) >> 12; 
    // if (letter->scale > MAX_SCALE) letter->scale = MAX_SCALE;
    // else if (letter->scale < MAX_SCALE) letter->scale = MIN_SCALE;