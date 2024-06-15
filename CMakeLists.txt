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

project(atparty-2024-demo C ASM)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_ASM_FLAGS "-D__ASSEMBLY__")

set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 1)
set(CMAKE_PROJECT_VERSION_PATCH 0)

set(SOURCES
src/main.c
src/asm.s

${CMAKE_CURRENT_BINARY_DIR}/res/sound.bin
${CMAKE_CURRENT_BINARY_DIR}/res/sound.h

${CMAKE_CURRENT_BINARY_DIR}/res/pixcat.bin
)

set(GENERATED_RES
)
list(APPEND SOURCES ${GENERATED_RES})

mm_add_soundbank_target(soundbank
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/res/sound.bin
    HEADER ${CMAKE_CURRENT_BINARY_DIR}/res/sound.h
    INPUTS res/willows.it
)
grit_add_binary_target(pixcat res/pixcat.png BITMAP
    OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/pixcat.bin
    DEPTH 4
)

add_executable(atparty-2024-demo ${SOURCES})

add_dependencies(atparty-2024-demo soundbank pixcat)

target_include_directories(atparty-2024-demo PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${MAXMOD_INCLUDE_DIR})
target_link_directories(atparty-2024-demo PRIVATE ${MAXMOD_LIBRARY_DIR})
target_link_libraries(atparty-2024-demo PRIVATE ${MAXMOD_LIBRARY})

gba_create_rom(atparty-2024-demo TITLE "AYCE")
