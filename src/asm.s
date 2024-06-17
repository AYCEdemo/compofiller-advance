
    .syntax unified
    
    .section .iwram, "ax", %progbits
    .arm

    .global inv_scalar
    .type inv_scalar STT_FUNC
inv_scalar:
    @ r0 = 1 / r0
    push    {r1-r4}
    movs    r4, r0
    negpl   r0, r0
    @ here we have to divide 0x100000000 by r0
    mov     r1, 1
    mov     r3, 8
1:
    .rept 4
        adds    r1, r0, r1, lsl 1
        subcc   r1, r1, r0
        adcs    r2, r2, r2  @ = rol 1
    .endr
    subs    r3, r3, 1
    bne     1b
    mov     r0, r2
    movs    r4, r4
    negmi   r0, r0
    pop     {r1-r4}
    bx      lr

    .global mul_vec2
    .type mul_vec2 STT_FUNC
mul_vec2:
    @ [r0] = [r0] * r1
    push    {r2-r5}
    ldm     r0, {r2, r4}
    smull   r2, r3, r1, r2
    lsl     r3, r3, 16
    orr     r3, r3, r2, lsr 16
    smull   r4, r5, r1, r4
    lsl     r5, r5, 16
    orr     r5, r5, r4, lsr 16
    stm     r0, {r3, r5}
    pop     {r2-r5}
    bx      lr

    .section .text

    .global sine_table
    @ sin((x + 0.5) * pi / 512) * 32768
sine_table:
    .short    100,   301,   502,   703,   904,  1105,  1306,  1507,  1708,  1909,  2109,  2310,  2510,  2711,  2911,  3111
    .short   3311,  3511,  3711,  3911,  4110,  4310,  4509,  4708,  4907,  5106,  5304,  5503,  5701,  5898,  6096,  6294
    .short   6491,  6688,  6884,  7081,  7277,  7473,  7669,  7864,  8059,  8254,  8448,  8642,  8836,  9029,  9223,  9415
    .short   9608,  9800,  9991, 10183, 10374, 10564, 10754, 10944, 11133, 11322, 11511, 11699, 11886, 12073, 12260, 12446
    .short  12632, 12817, 13002, 13186, 13370, 13554, 13736, 13919, 14100, 14282, 14462, 14642, 14822, 15001, 15180, 15357
    .short  15535, 15712, 15888, 16063, 16238, 16413, 16586, 16759, 16932, 17104, 17275, 17445, 17615, 17784, 17953, 18121
    .short  18288, 18454, 18620, 18785, 18950, 19113, 19276, 19439, 19600, 19761, 19921, 20080, 20239, 20396, 20553, 20709
    .short  20865, 21020, 21173, 21326, 21479, 21630, 21781, 21931, 22080, 22228, 22375, 22521, 22667, 22812, 22956, 23099
    .short  23241, 23382, 23523, 23662, 23801, 23939, 24075, 24211, 24346, 24480, 24614, 24746, 24877, 25008, 25137, 25266
    .short  25393, 25520, 25645, 25770, 25894, 26016, 26138, 26259, 26379, 26498, 26615, 26732, 26848, 26963, 27076, 27189
    .short  27301, 27411, 27521, 27630, 27737, 27844, 27949, 28054, 28157, 28259, 28361, 28461, 28560, 28658, 28755, 28851
    .short  28946, 29039, 29132, 29223, 29314, 29403, 29491, 29578, 29664, 29749, 29833, 29915, 29997, 30077, 30156, 30235
    .short  30312, 30387, 30462, 30535, 30608, 30679, 30749, 30818, 30886, 30952, 31018, 31082, 31145, 31207, 31268, 31327
    .short  31386, 31443, 31499, 31554, 31607, 31660, 31711, 31761, 31810, 31857, 31904, 31949, 31993, 32036, 32078, 32118
    .short  32157, 32195, 32232, 32268, 32302, 32335, 32367, 32398, 32427, 32456, 32483, 32509, 32533, 32557, 32579, 32600
    .short  32619, 32638, 32655, 32671, 32686, 32700, 32712, 32723, 32733, 32741, 32749, 32755, 32760, 32764, 32766, 32767
    .short  32767, 32766, 32764, 32760, 32755, 32749, 32741, 32733, 32723, 32712, 32700, 32686, 32671, 32655, 32638, 32619
    .short  32600, 32579, 32557, 32533, 32509, 32483, 32456, 32427, 32398, 32367, 32335, 32302, 32268, 32232, 32195, 32157
    .short  32118, 32078, 32036, 31993, 31949, 31904, 31857, 31810, 31761, 31711, 31660, 31607, 31554, 31499, 31443, 31386
    .short  31327, 31268, 31207, 31145, 31082, 31018, 30952, 30886, 30818, 30749, 30679, 30608, 30535, 30462, 30387, 30312
    .short  30235, 30156, 30077, 29997, 29915, 29833, 29749, 29664, 29578, 29491, 29403, 29314, 29223, 29132, 29039, 28946
    .short  28851, 28755, 28658, 28560, 28461, 28361, 28259, 28157, 28054, 27949, 27844, 27737, 27630, 27521, 27411, 27301
    .short  27189, 27076, 26963, 26848, 26732, 26615, 26498, 26379, 26259, 26138, 26016, 25894, 25770, 25645, 25520, 25393
    .short  25266, 25137, 25008, 24877, 24746, 24614, 24480, 24346, 24211, 24075, 23939, 23801, 23662, 23523, 23382, 23241
    .short  23099, 22956, 22812, 22667, 22521, 22375, 22228, 22080, 21931, 21781, 21630, 21479, 21326, 21173, 21020, 20865
    .short  20709, 20553, 20396, 20239, 20080, 19921, 19761, 19600, 19439, 19276, 19113, 18950, 18785, 18620, 18454, 18288
    .short  18121, 17953, 17784, 17615, 17445, 17275, 17104, 16932, 16759, 16586, 16413, 16238, 16063, 15888, 15712, 15535
    .short  15357, 15180, 15001, 14822, 14642, 14462, 14282, 14100, 13919, 13736, 13554, 13370, 13186, 13002, 12817, 12632
    .short  12446, 12260, 12073, 11886, 11699, 11511, 11322, 11133, 10944, 10754, 10564, 10374, 10183,  9991,  9800,  9608
    .short   9415,  9223,  9029,  8836,  8642,  8448,  8254,  8059,  7864,  7669,  7473,  7277,  7081,  6884,  6688,  6491
    .short   6294,  6096,  5898,  5701,  5503,  5304,  5106,  4907,  4708,  4509,  4310,  4110,  3911,  3711,  3511,  3311
    .short   3111,  2911,  2711,  2510,  2310,  2109,  1909,  1708,  1507,  1306,  1105,   904,   703,   502,   301,   100
    .short   -101,  -302,  -503,  -704,  -905, -1106, -1307, -1508, -1709, -1910, -2110, -2311, -2511, -2712, -2912, -3112
    .short  -3312, -3512, -3712, -3912, -4111, -4311, -4510, -4709, -4908, -5107, -5305, -5504, -5702, -5899, -6097, -6295
    .short  -6492, -6689, -6885, -7082, -7278, -7474, -7670, -7865, -8060, -8255, -8449, -8643, -8837, -9030, -9224, -9416
    .short  -9609, -9801, -9992,-10184,-10375,-10565,-10755,-10945,-11134,-11323,-11512,-11700,-11887,-12074,-12261,-12447
    .short -12633,-12818,-13003,-13187,-13371,-13555,-13737,-13920,-14101,-14283,-14463,-14643,-14823,-15002,-15181,-15358
    .short -15536,-15713,-15889,-16064,-16239,-16414,-16587,-16760,-16933,-17105,-17276,-17446,-17616,-17785,-17954,-18122
    .short -18289,-18455,-18621,-18786,-18951,-19114,-19277,-19440,-19601,-19762,-19922,-20081,-20240,-20397,-20554,-20710
    .short -20866,-21021,-21174,-21327,-21480,-21631,-21782,-21932,-22081,-22229,-22376,-22522,-22668,-22813,-22957,-23100
    .short -23242,-23383,-23524,-23663,-23802,-23940,-24076,-24212,-24347,-24481,-24615,-24747,-24878,-25009,-25138,-25267
    .short -25394,-25521,-25646,-25771,-25895,-26017,-26139,-26260,-26380,-26499,-26616,-26733,-26849,-26964,-27077,-27190
    .short -27302,-27412,-27522,-27631,-27738,-27845,-27950,-28055,-28158,-28260,-28362,-28462,-28561,-28659,-28756,-28852
    .short -28947,-29040,-29133,-29224,-29315,-29404,-29492,-29579,-29665,-29750,-29834,-29916,-29998,-30078,-30157,-30236
    .short -30313,-30388,-30463,-30536,-30609,-30680,-30750,-30819,-30887,-30953,-31019,-31083,-31146,-31208,-31269,-31328
    .short -31387,-31444,-31500,-31555,-31608,-31661,-31712,-31762,-31811,-31858,-31905,-31950,-31994,-32037,-32079,-32119
    .short -32158,-32196,-32233,-32269,-32303,-32336,-32368,-32399,-32428,-32457,-32484,-32510,-32534,-32558,-32580,-32601
    .short -32620,-32639,-32656,-32672,-32687,-32701,-32713,-32724,-32734,-32742,-32750,-32756,-32761,-32765,-32767,-32768
    .short -32768,-32767,-32765,-32761,-32756,-32750,-32742,-32734,-32724,-32713,-32701,-32687,-32672,-32656,-32639,-32620
    .short -32601,-32580,-32558,-32534,-32510,-32484,-32457,-32428,-32399,-32368,-32336,-32303,-32269,-32233,-32196,-32158
    .short -32119,-32079,-32037,-31994,-31950,-31905,-31858,-31811,-31762,-31712,-31661,-31608,-31555,-31500,-31444,-31387
    .short -31328,-31269,-31208,-31146,-31083,-31019,-30953,-30887,-30819,-30750,-30680,-30609,-30536,-30463,-30388,-30313
    .short -30236,-30157,-30078,-29998,-29916,-29834,-29750,-29665,-29579,-29492,-29404,-29315,-29224,-29133,-29040,-28947
    .short -28852,-28756,-28659,-28561,-28462,-28362,-28260,-28158,-28055,-27950,-27845,-27738,-27631,-27522,-27412,-27302
    .short -27190,-27077,-26964,-26849,-26733,-26616,-26499,-26380,-26260,-26139,-26017,-25895,-25771,-25646,-25521,-25394
    .short -25267,-25138,-25009,-24878,-24747,-24615,-24481,-24347,-24212,-24076,-23940,-23802,-23663,-23524,-23383,-23242
    .short -23100,-22957,-22813,-22668,-22522,-22376,-22229,-22081,-21932,-21782,-21631,-21480,-21327,-21174,-21021,-20866
    .short -20710,-20554,-20397,-20240,-20081,-19922,-19762,-19601,-19440,-19277,-19114,-18951,-18786,-18621,-18455,-18289
    .short -18122,-17954,-17785,-17616,-17446,-17276,-17105,-16933,-16760,-16587,-16414,-16239,-16064,-15889,-15713,-15536
    .short -15358,-15181,-15002,-14823,-14643,-14463,-14283,-14101,-13920,-13737,-13555,-13371,-13187,-13003,-12818,-12633
    .short -12447,-12261,-12074,-11887,-11700,-11512,-11323,-11134,-10945,-10755,-10565,-10375,-10184, -9992, -9801, -9609
    .short  -9416, -9224, -9030, -8837, -8643, -8449, -8255, -8060, -7865, -7670, -7474, -7278, -7082, -6885, -6689, -6492
    .short  -6295, -6097, -5899, -5702, -5504, -5305, -5107, -4908, -4709, -4510, -4311, -4111, -3912, -3712, -3512, -3312
    .short  -3112, -2912, -2712, -2511, -2311, -2110, -1910, -1709, -1508, -1307, -1106,  -905,  -704,  -503,  -302,  -101

    .global wheel_data, wheel_data_size
wheel_data:             .incbin "res/wheel.bin"
1:
    .align 2
wheel_data_size:        .word 1b-wheel_data

    .global wheel_map
wheel_map:              .incbin "res/wheel_map.bin"
    .align 2

    .global wheel_palette, wheel_palette_size
wheel_palette:          .incbin "res/wheel_pal.bin"
1:
    .align 2
wheel_palette_size:     .word 1b-wheel_palette

    .global font_data, font_data_size
font_data:              .incbin "res/font.bin"
1:
    .align 2
font_data_size:         .word 1b-font_data

    .global font_palette, font_palette_size
font_palette:           .incbin "res/font_pal.bin"
1:
    .align 2
font_palette_size:      .word 1b-font_palette

    .global sky_data
sky_data:               .incbin "res/sky.bin"
    .align 2

    .global soundbank
soundbank: .incbin "res/sound.bin"
    .align 2
