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
src/spritetext.c
src/asm.s

${CMAKE_CURRENT_SOURCE_DIR}/res/sound.bin
${CMAKE_CURRENT_SOURCE_DIR}/res/sound.h

# ${CMAKE_CURRENT_BINARY_DIR}/res/pixcat.bin
# ${CMAKE_CURRENT_BINARY_DIR}/res/pixcat.h

${CMAKE_CURRENT_SOURCE_DIR}/res/ayceFontSheet.s
${CMAKE_CURRENT_SOURCE_DIR}/res/ayceFontSheet.h
)

set(GENERATED_RES
)
list(APPEND SOURCES ${GENERATED_RES})

# mm_add_soundbank_target(soundbank
#     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/res/sound.s
#     HEADER ${CMAKE_CURRENT_BINARY_DIR}/res/sound.h
#     INPUTS res/willows.mod
# )
# grit_add_binary_target(pixcat res/pixcat.png BITMAP
#     OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/pixcat.bin
#     DEPTH 4
# )

# grit_add_binary_target(ayceFontSheet res/ayceFontSheet.png BITMAP
#     OUTPUT_GFX ${CMAKE_CURRENT_BINARY_DIR}/res/ayceFontSheet.bin
#     DEPTH 4
# )

# set(IMG_OUTFILES)
# file(GLOB IMG_SRCS "res/*.png")
# file(GLOB IMG_GRITS "res/*.grit")
# foreach(src grt IN ZIP_LISTS IMG_SRCS IMG_GRITS)
#     get_filename_component(IMG_NAME "${src}" NAME_WE)
#     set(IMG_OUTPUT_S_FILE "${IMG_NAME}.bin")
#     set(IMG_OUTPUT_H_FILE "${IMG_NAME}.h")
#     set(IMG_OUTPUTS)
#     list(APPEND IMG_OUTPUTS ${IMG_OUTPUT_S_FILE} ${IMG_OUTPUT_H_FILE})
#     set(GRIT_FLAGS)
#     file(STRINGS "${grt}" GRIT_FLAGS)
#     message(STATUS "image-translate rule for [${IMG_NAME}] using flags: ${GRIT_FLAGS}")

#     add_custom_command(
#         OUTPUT "${IMG_OUTPUT_S_FILE}" "${IMG_OUTPUT_H_FILE}"
#         COMMAND grit ${src} -fts -o"${CMAKE_CURRENT_BINARY_DIR}/res/${IMG_NAME}.bin" "${GRIT_FLAGS}"
#         DEPENDS ${src} ${grit}
#     )
#     add_custom_target(${IMG_NAME} DEPENDS ${IMG_OUTPUTS})
# endforeach()

add_executable(atparty-2024-demo ${SOURCES})

#add_dependencies(atparty-2024-demo) # soundbank) #ayceFontSheet) # image-translate)

target_include_directories(atparty-2024-demo PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${MAXMOD_INCLUDE_DIR})
target_link_directories(atparty-2024-demo PRIVATE ${MAXMOD_LIBRARY_DIR})
target_link_libraries(atparty-2024-demo PRIVATE ${MAXMOD_LIBRARY})

gba_create_rom(atparty-2024-demo TITLE "AYCE")
