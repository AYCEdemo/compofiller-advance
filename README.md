# Building

Following programs are required:
- CMake
- [devkitPro](https://devkitpro.org/wiki/Getting_Started), with these packages:
    - devkitARM
    - libtonc
    - maxmod-gba
    - gba-cmake

Also the environment variable `DEVKITPRO` should be set to where you installed DKP (usually /opt/devkitpro)

Then run following commands:
```
mkdir build
cd build
cmake ..
cmake --build .
```
