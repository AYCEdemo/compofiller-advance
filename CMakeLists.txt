cmake_minimum_required(VERSION 3.13)

if (DEFINED ENV{DEVKITPRO} AND (NOT DEVKITPRO))
    set(DEVKITPRO $ENV{DEVKITPRO})
endif ()

set(DEVKITPRO "${DEVKITPRO}" CACHE PATH "Path to devkitPro")
set(DKP_GBA_PLATFORM_LIBRARY libtonc)

set(CMAKE_TOOLCHAIN_FILE ${DEVKITPRO}/cmake/GBA.cmake)
set(GRIT_PATH ${DEVKITPRO}/tools/bin/grit)
# DKP installs MaxMod into libgba for some reason
set(MAXMOD_INCLUDE_DIR ${DEVKITPRO}/libgba/include)
set(MAXMOD_LIBRARY_DIR ${DEVKITPRO}/libgba/lib)
set(MAXMOD_LIBRARY mm)

project(compofiller-advance C ASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_ASM_FLAGS "-D__ASSEMBLY__")

set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 0)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/res)
set(SOURCES
src/main.c
src/spritetext.c
src/asm.s

${CMAKE_CURRENT_BINARY_DIR}/res/sound.bin
${CMAKE_CURRENT_BINARY_DIR}/res/sound.h
${CMAKE_CURRENT_BINARY_DIR}/res/wheel.bin
${CMAKE_CURRENT_BINARY_DIR}/res/wheel_map.bin
${CMAKE_CURRENT_BINARY_DIR}/res/wheel_pal.bin
${CMAKE_CURRENT_BINARY_DIR}/res/floor.bin
${CMAKE_CURRENT_BINARY_DIR}/res/floor_map.bin
${CMAKE_CURRENT_BINARY_DIR}/res/floor_pal.bin
${CMAKE_CURRENT_BINARY_DIR}/res/city.bin
${CMAKE_CURRENT_BINARY_DIR}/res/city_map.bin
${CMAKE_CURRENT_BINARY_DIR}/res/city_pal.bin
${CMAKE_CURRENT_BINARY_DIR}/res/font.bin
${CMAKE_CURRENT_BINARY_DIR}/res/font_pal.bin
${CMAKE_CURRENT_BINARY_DIR}/res/holder.bin
${CMAKE_CURRENT_BINARY_DIR}/res/holder_pal.bin
${CMAKE_CURRENT_BINARY_DIR}/res/sky.bin
)

mm_add_soundbank_target(soundbank
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/res/sound.bin
    HEADER ${CMAKE_CURRENT_BINARY_DIR}/res/sound.h
    INPUTS res/willows.it
)
grit_add_binary_target(img_wheel res/wheel.png
    OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/wheel.bin
    OUTPUT_MAP ${CMAKE_CURRENT_BINARY_DIR}/res/wheel_map.bin
    OUTPUT_PAL ${CMAKE_CURRENT_BINARY_DIR}/res/wheel_pal.bin
    DEPTH 8
    OPTIONS -gzl -mLa -mRa -pS
)
grit_add_binary_target(img_floor res/floor.png
    OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/floor.bin
    OUTPUT_MAP ${CMAKE_CURRENT_BINARY_DIR}/res/floor_map.bin
    OUTPUT_PAL ${CMAKE_CURRENT_BINARY_DIR}/res/floor_pal.bin
    DEPTH 8
    OPTIONS -gzl -mLa -mRa -mzl -pS
)
grit_add_binary_target(img_city res/city.png
    OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/city.bin
    OUTPUT_MAP ${CMAKE_CURRENT_BINARY_DIR}/res/city_map.bin
    OUTPUT_PAL ${CMAKE_CURRENT_BINARY_DIR}/res/city_pal.bin
    DEPTH 8
    OPTIONS -gzl -mLs -mR8 -mzl -pS
)
grit_add_binary_target(img_font res/font.png
    OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/font.bin
    OUTPUT_PAL ${CMAKE_CURRENT_BINARY_DIR}/res/font_pal.bin
    NO_MAP
    DEPTH 4
    OPTIONS -gzl -pS
)
grit_add_binary_target(img_holder res/holder.png
    OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/holder.bin
    OUTPUT_PAL ${CMAKE_CURRENT_BINARY_DIR}/res/holder_pal.bin
    NO_MAP
    DEPTH 4
    OPTIONS -gzl -pS
)
# at the time of making this, there's a bug in grit function that always make
# it assume that the palette file exists even if NO_PALETTE option is specified
# here comes the hacky solution
file(TOUCH ~$img_sky.pal.bin)
grit_add_binary_target(img_sky res/sky.png
    OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/sky.bin
    BITMAP
    DEPTH 16
)

add_executable(compofiller-advance ${SOURCES})

add_dependencies(compofiller-advance soundbank img_wheel img_floor img_city img_font img_holder img_sky)

target_include_directories(compofiller-advance PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${MAXMOD_INCLUDE_DIR})
target_link_directories(compofiller-advance PRIVATE ${MAXMOD_LIBRARY_DIR})
target_link_libraries(compofiller-advance PRIVATE ${MAXMOD_LIBRARY})

gba_create_rom(compofiller-advance TITLE "CMPOFLLR ADV")