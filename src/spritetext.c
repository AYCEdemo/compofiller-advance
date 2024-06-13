#include <string.h>
#include "spritetext.h"
#include "../res/ayceFontSheet.h"
#include "tonc_oam.h"
#include "tonc_types.h"
#include "tonc_memdef.h"
#include "tonc_memmap.h"
#include "tonc_math.h"
#include "lap.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
u32 pb = 0;
vec2_t origin = VEC2(120 - (LETTER_SIZE * 2), 80 - (LETTER_SIZE / 2));
//Word debug_onscreen_word;
WordSent debug_sent;

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
        letters[i] = (Letter){i, origin, 1, &obj_buffer[i], FALSE};
        // obj_set_attr(letters[i].oam,
        //     ATTR0_SQUARE | ATTR0_AFF,
        //     ATTR1_SIZE_32 | ATTR1_AFF_ID(i),
        //     ATTR2_PALBANK(6) | tileOffset(i));
        // obj_aff_identity(&obj_aff_buffer[i]);
    }
    // oam_copy(oam_mem, obj_buffer, NUM_LETTERS);
    // set_word_pos(letters, "AYCE");
    //debug_set_word_pos(letters);
    debug_set_word_pos(letters);
    //obj_set_pos(letters[0].oam, 96, 32);            
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
            ATTR0_SQUARE | ATTR0_AFF,
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

    // int32_t x = comp_to_int(letter->curr_pos.x);
    // int32_t y = comp_to_int(letter->curr_pos.y);

    // // if (x < -16 || x > 240 || y < -16 || y > 160)
    // // {
    // //     //kill_letter(letter);
    // //     return;
    // // }
    
    if (is_comp_neg(SCALAR(1), 16)) letter->curr_pos.y = SCALAR(10);
    else letter->curr_pos.y = SCALAR(90);

    // if (is_comp_neg(sin, 0)) letter->curr_pos.y -= sin;
    // else letter->curr_pos.y += sin;
    // letter->curr_pos.y += (lu_sin((tick + pos) * 64) * 32) << 4 * 32;
    // letter->curr_pos = add_vec2(letter->curr_pos, VEC2(0, lu_sin(tick)));
    obj_set_pos(letter->oam, comp_to_int(letter->curr_pos.x), comp_to_int(letter->curr_pos.y));            
}

void render_letters() {
    oam_copy(oam_mem, obj_buffer, NUM_LETTERS);
}