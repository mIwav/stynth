# Building with gcc-arm-none-eabi

There are certain issues that are recurring on the build and especially
the link stage when using the cross-compiler.

  - hardware vs software floating point
  - startup code (`init_array`)
  - trace and printf functionality (where to trace out)

It is dependent on the build environment and what code is used
if the link is failing.
 
This relates to symbols like:
  - `bss_start` and `bss_end`
  - `init_array` - c++ static constructors
  - `__exidx_start` and `__exidx_end` - c++ exception handlers


The linker ld file also provides us with sections. Using objdump -h we can
see a section listing with the VMA (virtual memory address) and LMA (load 
memory address).

Each section has a set of attributes like CONTENTS, ALLOC, LOAD, DATA, CODE...
  - CONTENTS: this section in the bin file contains data to load to LMA
  - ALLOC: allocate space of Size bytes in LMA
  - LOAD: load the data to LMA
  - CODE: the sections contains code

In the .ld file we have the location counter, keeping track of the size
of a segment. It is the . before a section name.
We can avoid creating individual segments for each function by using the 
star in front of a segment (i.e. `(*(.text))`)

## What happens on uC start actually?

When the microcontroller is starting, it has to do the following steps:

  1. Set up the stack pointer to the end of the stack (RAM + sizeof(RAM))
  2. Initialize the Interrupt Vector table
  3. Zero the .bss section (uninitialized variables) and copy .data sections from LMA to VMA
  4. Call constructors of global and static objects (if C++ is used)
  5. Jump to the main function

This task is handled by the start-up code, which is called as soon
as the reset interrupt is triggered after a few clock cycles.

Some words on .bss and .data initialization:
  - .data contains initialized data
  - .bss contains unititialized data (its sections needs to be zeroed) and therefore 
    does not need space in the bin file
  - .bss also contains COMMON

COMMON contains the maximum memory space needed by an uninitialized global variable declared
with the same name at two different locations in the c files. So i.e. in one file int blub[6] 
and in the other int blub[7] would allocate 28 bytes in the common section for BOTH of them.

This is compiler specific behavior, if you would actually init the variables with a value or make 
them static, the linker would complain about the multiple definitions.
This can be overridden by the -fno-common gcc flag and this will allocate global uninit vars 
to the data section. This will blow up your binary file size and waste flash space however.

Constants are contained in the .rodata section. It can simply be placed into the flash, as
we do not need it to waste SRAM space.

The available heap grows from the end of the bss section up, the stack goes from top of RAM
down to the end of the heap.


