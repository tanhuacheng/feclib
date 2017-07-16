#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "fec.h"

static fecEncoder* e;
static fecDecoder* d;

static int send = 0;
static size_t userSend (void* buf, size_t size, size_t count, void* userData)
{
    (void)userData;

    send++;
    // if (rand() % 10 >= 1) {
    static int i = 0;
    if ((i++) % 10 != 4) {
        FecDecode(buf, size, count, d);
    }

    return size * count;
}

static int recv = 0;
static void userReceive (void* userData, int64_t position, fecPayload* buf, int len)
{
    (void)userData;

    recv++;
    if (*(uint8_t*)buf % 8 != position / len) {
        printf("failed\n");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // srand(time(NULL));

    uint8_t data[160];
    for (int i = 0; i < 500; i++) {
        e = NewFecEncoder(NULL, userSend, NULL, 160, 8, 2, 40, 4, 0);
        d = NewFecDecoder(NULL, userReceive);

        for (int j = 0; j < 8; j++) {
            memset(data, i * 8 + j, sizeof(data));
            FecEncode((void*)data, e);
        }

        DeleteFecEncoder(e);
        FlushFecDecoder(d);
        DeleteFecDecoder(d);
    }

    printf("send: %d, recv: %d\n", send, recv);

    return 0;
}
