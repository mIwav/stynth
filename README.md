# stynth - A simple STM32F767 synth

## Build Environment

WLS 2 - Debian
gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
Ruby
Ceedling (yum install ceedling)

# Build Instructions

The cross-compiler tools are assumed to be in /usr/local/x-tools/gcc-arm-none-eabi/bin.
Adjust your path accordingly in project.yml to fit your toolchain location.

Ceedling provides us with a project.yml file, containing build instructions and
flags for the cross-compiler.

To build only run:
ceedling release

The build output can be found in:
./build/release/project.elf and project.elf.hex


