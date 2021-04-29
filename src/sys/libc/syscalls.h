#pragma once

#include "types.h"

__attribute((implicit_return)) u16 syscall(u16 id, __attribute((optional)) u16 ab, __attribute((optional)) u16 ac, __attribute((optional)) u16 ad)
{
    asm( \
        "int #U1\n" \
    : "ax"(id), "bx"(ab), "cx"(ac), "dx"(ad));
}

#define sys_read  0
#define sys_write 1
#define sys_open  2
#define sys_close 3
#define sys_exit  4
#define sys_chdir 5
#define sys_mkdir 6
#define sys_ioctl 7
