#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "rot13.h"

int main() {
    static char buf1[0x1000000];
    static char buf2[0x1000000];
    static char buf3[0x1000000];
    int i;
    for (i = 0; i < 0xFFFFFF; ) {
        char a = (char) rand();
        if (!a) continue;
        buf1[i] = buf2[i] = buf3[i] = a;
        i++;
    }
    buf1[0xFFFFFF] = buf2[0xFFFFFF] = buf3[0xFFFFFF] = '\0';

    /*
    for (i = 0; i < 0x1000000; i++) {
        if (!buf1[i]) break;
        printf("%02X ", (unsigned char)buf1[i]);
    };
    putchar('\n');
    rot13_naive(buf1);
    rot13_simd(buf2);
    for (i = 0; i < 0x1000000; i++) {
        if (!buf1[i]) break;
        printf("%02X ", (unsigned char)buf1[i]);
    };
    putchar('\n');
    for (i = 0; i < 0x1000000; i++) {
        if (!buf2[i]) break;
        printf("%02X ", (unsigned char)buf2[i]);
    };
    putchar('\n');
    */

    uint32_t hash1, hash2, hash3;
    hash1 = hash2 = hash3 = 0;
    struct timeval tv1, tv2;
    double t1, t2, t3;

    gettimeofday(&tv1, NULL);
    for (i = 0; i < 100; i++) rot13_naive(buf1);
    gettimeofday(&tv2, NULL);
    t1 = tv2.tv_sec - tv1.tv_sec + (tv2.tv_usec - tv1.tv_usec) / 1000000.0;

    gettimeofday(&tv1, NULL);
    for (i = 0; i < 100; i++) rot13_simd(buf2);
    gettimeofday(&tv2, NULL);
    t2 = tv2.tv_sec - tv1.tv_sec + (tv2.tv_usec - tv1.tv_usec) / 1000000.0;

    gettimeofday(&tv1, NULL);
    for (i = 0; i < 100; i++) rot13_hybrid(buf3);
    gettimeofday(&tv2, NULL);
    t3 = tv2.tv_sec - tv1.tv_sec + (tv2.tv_usec - tv1.tv_usec) / 1000000.0;

    printf("1M 100times naive %fsec simd %fsec hybrid %fsec\n", t1, t2, t3);

    for (i = 0; i < 0x100000; i++) hash1 ^= ((uint32_t*) buf1)[i];
    for (i = 0; i < 0x100000; i++) hash2 ^= ((uint32_t*) buf2)[i];
    for (i = 0; i < 0x100000; i++) hash3 ^= ((uint32_t*) buf3)[i];
    printf("naive %08X simd %08X hybrid %08X\n", hash1, hash2, hash3);

    static char hw1[64] = "Hello, world! Hello, world! Hello, world!";
    static char hw2[64] = "Hello, world! Hello, world! Hello, world!";
    static char hw3[64] = "Hello, world! Hello, world! Hello, world!";

    gettimeofday(&tv1, NULL);
    for (i = 0; i < 10000001; i++) rot13_naive(hw1+2);
    gettimeofday(&tv2, NULL);
    t1 = tv2.tv_sec - tv1.tv_sec + (tv2.tv_usec - tv1.tv_usec) / 1000000.0;

    gettimeofday(&tv1, NULL);
    for (i = 0; i < 10000001; i++) rot13_simd(hw2+2);
    gettimeofday(&tv2, NULL);
    t2 = tv2.tv_sec - tv1.tv_sec + (tv2.tv_usec - tv1.tv_usec) / 1000000.0;

    gettimeofday(&tv1, NULL);
    for (i = 0; i < 10000001; i++) rot13_hybrid(hw3+2);
    gettimeofday(&tv2, NULL);
    t3 = tv2.tv_sec - tv1.tv_sec + (tv2.tv_usec - tv1.tv_usec) / 1000000.0;

    printf("Hello, world!x3[1] 10Mtimes naive %fsec simd %fsec hybrid %fsec\n", t1, t2, t3);
    puts(hw1);
    puts(hw2);
    puts(hw3);

    return 0;
}
