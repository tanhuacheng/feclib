#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "fec.h"

static fecEncoder* e;
static fecDecoder* d;

static size_t userSend (void* buf, size_t size, size_t count, void* userData)
{
    static int i = 0;

    printf("userSend: %d\n", ++i);
    printf("buf: %p, size: %d, count: %d, userData: %p\n", buf, (int)size, (int)count, userData);
    for (int i = 0; i < (int)count; i++) {
        printf("count = %d\n", i + 1);
        for (int j = 0; j < (int)size; j++) {
            if (j > 0 && j % 64 == 0) {
                printf("\n");
            }
            printf("%02x ", ((uint8_t*)buf)[j]);
        }
        printf("\n");
    }
    printf("\n");

    if (rand() % 100 > 5) {
        FecDecode(buf, size, count, d);
    }

    return size * count;
}

static void userReceive (void* userData, int64_t position, fecPayload* buf, int len)
{
    static int i = 0;

    printf("userReceive: %d, %d\n", (int)position, ++i);
    printf("userData: %p, position: %d, buf: %p, len: %d\n", userData, (int)position, buf, len);
    for (int i = 0; i < len; i++) {
        if (i > 0 && i % 64 == 0) {
            printf("\n");
        }
        printf("%02x ", ((uint8_t*)buf)[i]);
    }
    printf("\n\n");
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("time: %lu, %lu\n", tv.tv_sec, (tv.tv_usec / 1000));

    char* err = NULL;
    e = NewFecEncoder(NULL, userSend, &err, 160, 10, 3, 40, 4, 2 * (160 + 12) * 50 * 8 * 13 / 10);
    if (err) {
        printf("%s\n", err);
        exit(EXIT_SUCCESS);
    }
    d = NewFecDecoder(NULL, userReceive);

    srand(time(NULL));
    uint8_t data[160];
    for (int i = 0; i < 50 * 30; i++) {
        printf("FecEncode: %d\n", i + 1);
        for (int j = 0; j < 160; j++) {
            if (j > 0 && j % 64 == 0) {
                printf("\n");
            }
            data[j] = rand();
            printf("%02x ", data[j]);
        }
        printf("\n\n");
        FecEncode((void*)data, e);
    }

    DeleteFecEncoder(e);
    FlushFecDecoder(d);
    DeleteFecDecoder(d);

    gettimeofday(&tv, NULL);
    printf("time: %lu, %lu\n", tv.tv_sec, (tv.tv_usec / 1000));

    return 0;
}
