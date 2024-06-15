#include <string.h>
#include "spritetext.h"
#include "tonc_oam.h"
#include "tonc_types.h"
#include "tonc_memdef.h"
#include "tonc_memmap.h"
#include "tonc_math.h"
#include "lap.h"

#define MIN_SCALE 448
#define MID_SCALE 256
#define MAX_SCALE 196

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

vec2_t origin = VEC2(16 - (LETTER_SIZE * 2), 80 - (LETTER_SIZE / 2));
vec2_t side_origin = VEC2(0, 100 - (LETTER_SIZE / 2));
vec2_t offscreen_origin = VEC2(256, 100 - (LETTER_SIZE / 2));
// vec2_t origin = VEC2(120 - (LETTER_SIZE * 2), 80 - (LETTER_SIZE / 2));

typedef struct DebugSent {
    int32_t senti;
    int val;
} DebugSent;
DebugSent senti = { 999, 0 };

extern s16 sine_table[1024];
extern u32 font[];
extern u8 sprites_data[];
extern u32 sprites_data_size;
extern u8 sprites_palette[];
extern u32 sprites_palette_size;

char* debug_text_scroller = "          YO!! WHAT IS UP!  AYCE HERE WITH SOME COMPOFILLER PRODUCED UNDER EXTREME DURESS! NOTHING LIKE MAKING A PROD MOMENTS BEFORE DEADLINE, EH? CODE: NATT AND TFX   GRAPHICS: GRACIOUSLY CREATED BY HIIJ   MUSIC: SHAMELESSLY REPOSTED FROM MY ENTRY TO CHIPCHOP17. THANKS FOR ORGANIZING SUCH A COOL DISK RAMON!  ";
int debug_text_scroller_len = 0;
int debug_text_scroller_ind = 0;
int newWord = FALSE;
int wordId = 0;

void debug_set_word_pos(Letter*);
int debug_map_char_to_int(const char c);
void set_letter_pos(Letter*);

Letter* init_letters(Letter* letters)
{
    memcpy32(&tile_mem[4], sprites_data, sprites_data_size);
    memcpy32(pal_obj_mem, sprites_palette, sprites_palette_size);
    oam_init(obj_buffer, 128);

    for (int i = 0; i < NUM_LETTERS; i++)
    {
        letters[i] = (Letter){i, origin, MID_SCALE, &obj_buffer[i], &obj_buffer[i+NUM_LETTERS], FALSE};
        obj_aff_identity(&obj_aff_buffer[i]);
    }

    debug_text_scroller_len = strlen(debug_text_scroller);
    debug_set_word_pos(letters);
}

int debug_map_char_to_int(const char c) {
    if (c >= 65 && c <= 90) return c - 65;
    if (c >= 48 && c <= 56) return c - 20;
    if (c == 32) return -2;
    if (c == 33) return 36;
    if (c == 63) return 37;
    if (c == 46) return 40;
    if (c == 44) return 41;
    if (c == 58) return 42;
    if (c == 59) return 43;
    return -1;
}

void debug_set_word_pos(Letter* letters) {
    char* word = "         ";
    // char* word = "HELLOWORL";
    int strl = strlen(word);
    for (int i = 0; i < strl; i++)
    {
        add_letter(letters, debug_map_char_to_int(word[i]), add_vec2(side_origin, VEC2(LETTER_SIZE * i + 1, 0)));
    }
    debug_text_scroller_ind = 8;
}

void set_letter_pos(Letter* letter) {
    obj_set_pos(letter->oam, comp_to_int(letter->curr_pos.x), comp_to_int(letter->curr_pos.y));
    obj_set_pos(letter->shadow, comp_to_int(letter->curr_pos.x), 110);        
}

int add_letter(Letter* letters, int letter, vec2_t startingPos) {
    for (int i = 0; i < NUM_LETTERS; i++)
    {
        if (letters[i].on_screen) continue;
        letters[i].on_screen = TRUE;

        if (letter == -2) newWord = TRUE;
        else if (newWord) {
            newWord = FALSE;
            wordId = !wordId;
        }

        //letters[i].id = wordId;
        letters[i].curr_pos = startingPos;

        if (letter == -2) return -1;
        senti.val = letters[i].id;
        obj_set_attr(letters[i].oam,
            ATTR0_SQUARE | ATTR0_AFF_DBL,
            ATTR1_SIZE_32 | ATTR1_AFF_ID(letters[i].id),
            ATTR2_PALBANK(6) | tileOffset(letter));
        // shadow
        obj_set_attr(letters[i].shadow,
            ATTR0_SQUARE | ATTR0_AFF_DBL,
            ATTR1_SIZE_32 | ATTR1_AFF_ID(letters[i].id),
            ATTR2_PALBANK(6) | tileOffset(38));

        set_letter_pos(&letters[i]);    
        return i;
    }
    return -1;
}

// TODO: lazy pass whole array so that we can create next letters
void update_letter(Letter *letters, Letter *letter, uint tick, uint pos) {
    if (letter == NULL) return;
    if (!letter->on_screen) return;

    int32_t x = comp_to_int(letter->curr_pos.x);

    letter->curr_pos.x -= SCALAR(1);
    if (x < -32)
    {
        kill_letter(letter);
        return;
    }

    letter->curr_pos.y += sine_table[(tick + x)&1023] / 6;

    // how in the fuck do I clamp this shit
    letter->scale = (sine_table[(tick + x * 4)&1023] >> 8) + MID_SCALE;
    if (letter->scale > MIN_SCALE) letter->scale = MIN_SCALE;
    else if (letter->scale < MAX_SCALE) letter->scale = MAX_SCALE;
    obj_aff_rotscale(&obj_aff_buffer[letter->id], letter->scale, letter->scale, 0);
    if (letter->curr_pos.y > SCALAR(100)) letter->curr_pos.y = SCALAR(100);
    set_letter_pos(letter);
}

void update_scale(int tick) {
}

// TODO: lazy pass whole array so that we can create next letters
void kill_letter(Letter* letter) {
    int nextLetter = debug_map_char_to_int(debug_text_scroller[debug_text_scroller_ind]);
    debug_text_scroller_ind++;
    if (debug_text_scroller_ind > debug_text_scroller_len)
        debug_text_scroller_ind = 0;

    if (nextLetter == -2) newWord = TRUE;
    else if (newWord) {
        newWord = FALSE;
        wordId = !wordId;
    }

    //letter->id = wordId;
    letter->curr_pos = offscreen_origin;

    if (nextLetter == -2) 
    {
        memset32(letter->oam, 0, 2);
        memset32(letter->shadow, 0, 2);
        obj_set_pos(letter->oam, 0, 160); //off screen=
        obj_set_pos(letter->shadow, 0, 160);
        return;   
    }
    obj_set_attr(letter->oam,
        ATTR0_SQUARE | ATTR0_AFF_DBL,
        ATTR1_SIZE_32 | ATTR1_AFF_ID(letter->id),
        ATTR2_PALBANK(6) | tileOffset(nextLetter));
    // shadow
    obj_set_attr(letter->shadow,
        ATTR0_SQUARE | ATTR0_AFF_DBL,
        ATTR1_SIZE_32 | ATTR1_AFF_ID(letter->id),
        ATTR2_PALBANK(6) | tileOffset(38));

    set_letter_pos(letter);
}

void render_letters(int tick) {
    update_scale(tick);
    oam_copy(oam_mem, obj_buffer, 128);
}


// cool effect that will make you extremely nauseated
    // letter->scale += (sine_table[((tick / 4) - pos)&1023] / 4) >> 12; 
    // if (letter->scale > MAX_SCALE) letter->scale = MAX_SCALE;
    // else if (letter->scale < MAX_SCALE) letter->scale = MIN_SCALE;


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