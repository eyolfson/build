/*******************************************************************************
Copyright 2015 Jonathan Eyolfson

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* inline size_t linux_write(fd_t, char *, size_t) __attribute__((always_inline)); */


__attribute__((always_inline)) inline
size_t linux_write(unsigned int fd, char *buf, size_t count)
{
    size_t ret;
    __asm__ volatile
    (
        "syscall"
        : "=rax" (ret)
        : "rax"(1), "D"(fd), "S"(buf), "d"(count)
        : "cc", "rcx", "r11", "memory"
    );
    return ret;
}

int main(int argc, char **argv)
{
    for (int i; i < argc; ++i) {
        if (strcmp(argv[i], "--version") == 0) {
            printf("Build 0.0.1-unknown\n");
            return EXIT_SUCCESS;
        }
    }

    linux_write(1, "test 2\n", 7);

    /* int ret; */
    /* char *buf = "test\ning"; */
    /* asm volatile */
    /* ( */
    /*     "syscall" */
    /*     : "=a" (ret) */
    /*     : "0"(1), "D"(1), "S"(buf), "d"(5) */
    /*     : "cc", "rcx", "r11", "memory" */
    /* ); */
    
    return EXIT_FAILURE;
}
