set(CMAKE_HOST_SYSTEM "Linux-5.10.16.3-microsoft-standard-WSL2")
set(CMAKE_HOST_SYSTEM_NAME "Linux")
set(CMAKE_HOST_SYSTEM_VERSION "5.10.16.3-microsoft-standard-WSL2")
set(CMAKE_HOST_SYSTEM_PROCESSOR "x86_64")

include("/opt/devkitpro/cmake/GBA.cmake")

set(CMAKE_SYSTEM "NintendoGBA-1")
set(CMAKE_SYSTEM_NAME "NintendoGBA")
set(CMAKE_SYSTEM_VERSION "1")
set(CMAKE_SYSTEM_PROCESSOR "armv4t")

set(CMAKE_CROSSCOMPILING "TRUE")

set(CMAKE_SYSTEM_LOADED 1)