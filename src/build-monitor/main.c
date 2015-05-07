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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/user.h>
#include <sys/reg.h>
#include <sys/types.h>

#include <sys/syscall.h>

static int monitor(pid_t pid)
{
    int wait_status;
    unsigned icounter = 0;
    printf("monitor started\n");

    /* Wait for child to stop on its first instruction */
    wait(&wait_status);

    while (WIFSTOPPED(wait_status)) {
        icounter++;

        struct user_regs_struct regs;
        long rc = ptrace(PTRACE_GETREGS, pid, NULL, &regs);

        if (regs.orig_rax == __NR_open) {
            printf("syscall [open]\n");
            rc = ptrace(PTRACE_PEEKDATA, pid, regs.rdi, 0);
            char * s = (char *) &rc;
            printf("%c%c%c%c\n", s[0], s[1], s[2], s[3]);
        }

        /* Make the child execute another instruction */
        if (ptrace(PTRACE_SYSCALL, pid, 0, 0) < 0) {
            perror("ptrace");
            return EXIT_FAILURE;
        }

        /* Wait for child to stop on its next instruction */
        wait(&wait_status);
    }

    printf("the child executed %u syscalls\n", icounter);

    return EXIT_SUCCESS;
}

static int target(char *const path)
{
    printf("target started. will run '%s'\n", path);

    /* Allow tracing of this process */
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        perror("ptrace");
        return EXIT_FAILURE;
    }

    /* Replace this process's image with the given program */
    execl(path, path, 0);

    /* Reached if there's an error */
    perror("target execl");
    return EXIT_FAILURE;
}

int main(int argc, char *const *const argv)
{
    for (int i; i < argc; ++i) {
        if (strcmp(argv[i], "--version") == 0) {
            printf("Build Monitor 0.0.1-unknown\n");
            return EXIT_SUCCESS;
        }
    }

    if (argc < 2) {
        fprintf(stderr, "Expected a program name as argument\n");
        return EXIT_FAILURE;
    }
    
    pid_t pid;
    pid = fork();
    if (pid == 0) {
        return target(argv[1]);
    }
    else if (pid > 0) {
        return monitor(pid);
    }
    else {
        perror("fork");
        return EXIT_FAILURE;
    }
}
