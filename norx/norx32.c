/* norx32.c */
/*
    This file is part of the AVR-Crypto-Lib.
    Copyright (C) 2006-2014 Daniel Otte (daniel.otte@rub.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <inttypes.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include <memxor.h>
#include <norx32.h>

#include <stdio.h>
#include "cli.h"

#define R0  8
#define R1 11
#define R2 16
#define R3 31

#define U0 0x243f6a88l
#define U1 0x85a308d3l
#define U2 0x13198a2el
#define U3 0x03707344l
#define U4 0x254f537al
#define U5 0x38531d48l
#define U6 0x839c6E83l
#define U7 0xf97a3ae5l
#define U8 0x8c91d88cl
#define U9 0x11eafb59l

#if 0


#define U0 0x886a3f24l
#define U1 0xd308a385l
#define U2 0x2e8a1913l
#define U3 0x44737003l
#define U4 0x7a534f25l
#define U5 0x481d5338l
#define U6 0x836e9c83l
#define U7 0xe53a7af9l
#define U8 0xxxxxxxxxx
#define U9 0x59fbea11l

#endif

#define WORD_SIZE 32

#define RATE_WORDS 10
#define CAPACITY_WORDS 6

#define RATE_BITS (RATE_WORDS * WORD_SIZE)
#define CAPACITY_BITS (CAPACITY_WORDS * WORD_SIZE)

#define RATE_BYTES (RATE_BITS / 8)
#define CAPACITY_BYTES (CAPACITY_BITS / 8)

#define TAG_HEADER    0x01
#define TAG_PAYLOAD   0x02
#define TAG_TRAILER   0x04
#define TAG_TAG       0x08
#define TAG_BRANCHING 0x10
#define TAG_MERGING   0x20


#define SET_TAG(ctx,t) do { ((uint8_t*)&(ctx)->s[15])[0] ^= (t); } while (0)


void norx32_dump(const norx32_ctx_t *ctx)
{
    printf("\n--- DUMP STATE ---");
    printf("\n\t%08lX %08lX %08lX %08lX", ctx->s[ 0], ctx->s[ 1], ctx->s[ 2], ctx->s[ 3]);
    printf("\n\t%08lX %08lX %08lX %08lX", ctx->s[ 4], ctx->s[ 5], ctx->s[ 6], ctx->s[ 7]);
    printf("\n\t%08lX %08lX %08lX %08lX", ctx->s[ 8], ctx->s[ 9], ctx->s[10], ctx->s[11]);
    printf("\n\t%08lX %08lX %08lX %08lX", ctx->s[12], ctx->s[13], ctx->s[14], ctx->s[15]);
    printf("\n--- END ---\n");
}

static void phi(uint32_t *(a[2]))
{
    uint32_t x;
    x = (*(a[0]) & *(a[1])) << 1;
    *(a[0]) ^= *(a[1]) ^ x;
}

static void xrot(uint32_t *a, const uint32_t *b, uint8_t r)
{
    uint32_t x;
    x = *a ^ *b;
    *a = (x << (32 - r)) | (x >> r);
}

#define A (a[0])
#define B (a[1])
#define C (a[2])
#define D (a[3])

void g32(uint32_t *(a[4]))
{
    phi(&A);
    xrot(D, A, R0);
    phi(&C);
    xrot(B, C, R1);
    phi(&A);
    xrot(D, A, R2);
    phi(&C);
    xrot(B, C, R3);
}

static uint8_t g2_table[4][4] = {
        {0, 5, 10, 15},
        {1, 6, 11, 12},
        {2, 7,  8, 13},
        {3, 4,  9, 14},
};

void f32(norx32_ctx_t *ctx)
{
    uint32_t *(a[4]);
    uint8_t i, rounds;
    rounds = ctx->r;
    do {
        a[0] = &ctx->s[0];
        a[1] = &ctx->s[4];
        a[2] = &ctx->s[8];
        a[3] = &ctx->s[12];
        g32(a);
        i = 3;
        do {
            a[0]++;
            a[1]++;
            a[2]++;
            a[3]++;
            g32(a);
        } while (--i);
        i = 4;
        do {
            --i;
            a[0] = &ctx->s[g2_table[i][0]];
            a[1] = &ctx->s[g2_table[i][1]];
            a[2] = &ctx->s[g2_table[i][2]];
            a[3] = &ctx->s[g2_table[i][3]];
            g32(a);
        } while (i);
    } while (--rounds);
}

static const uint32_t init_state[] PROGMEM = {
        U0,  0,  0, U1,
         0,  0,  0,  0,
        U2, U3, U4, U5,
        U6, U7, U8 ^ (1l << 15), U9
};

static void norx32_process_block(
    norx32_ctx_t *ctx,
    const void *block,
    uint8_t tag )
{
    SET_TAG(ctx, tag);
    f32(ctx);
    memxor(ctx->s, block, RATE_BYTES);
}

static void norx32_process_last_block(
    norx32_ctx_t *ctx,
    void *out_block,
    const void *block,
    uint16_t length_b,
    uint8_t tag )
{
    while (length_b >= RATE_BITS) {
        norx32_process_block(ctx, block, tag);
        block = (uint8_t*)block + RATE_BYTES;
        length_b -= RATE_BITS;
        if (out_block) {
            memcpy(out_block, ctx->s, RATE_BYTES);
            out_block = (uint8_t*)out_block + RATE_BYTES;
        }
    }
    SET_TAG(ctx, tag);
    f32(ctx);
    memxor(ctx->s, block, (length_b + 7) / 8);
    if (out_block) {
        memcpy(out_block, ctx->s, (length_b + 7) / 8);
        out_block = (uint8_t*)out_block + (length_b + 7) / 8;
        if ((length_b & 7) != 0) {
            ((uint8_t*)out_block)[length_b / 8 + 1] &= 0xff << (7 - (length_b & 7));
        }
    }
    ((uint8_t*)ctx->s)[length_b / 8] ^= 1 << (length_b & 7);
    if (length_b == RATE_BITS - 1) {
        SET_TAG(ctx, tag);
        f32(ctx);
    }
    ((uint8_t*)ctx->s)[RATE_BYTES - 1] ^= 0x80;

}

/******************************************************************************/

int8_t norx32_init (
    norx32_ctx_t *ctx,
    const void* nonce,
    const void* key,
    uint16_t tag_size_b,
    uint8_t rounds,
    uint8_t parallel )
{
    uint32_t v;
    if (ctx == NULL || nonce == NULL || key == NULL) {
        return -1;
    }
    if (tag_size_b > 320) {
        return -1;
    }
    if (rounds < 1 || rounds > 63) {
        return -1;
    }
    if (parallel != 1) {
        return -2;
    }
    memcpy_P(ctx->s, init_state, sizeof(ctx->s));
    memcpy(&ctx->s[1], nonce, 2 * sizeof(ctx->s[1]));
    memcpy(&ctx->s[4], key, 4 * sizeof(ctx->s[4]));
    v  = ((uint32_t)rounds)   << 26;
    v ^= ((uint32_t)parallel) << 18;
    v ^= tag_size_b;
    ctx->s[14] ^= v;
    ctx->d = parallel;
    ctx->a = tag_size_b;
    ctx->r = rounds;
    f32(ctx);
    return 0;
}

void norx32_finalize(norx32_ctx_t *ctx, void *tag)
{
    SET_TAG(ctx, TAG_TAG);
    f32(ctx);
    f32(ctx);
    if (tag) {
        memcpy(tag, ctx->s, (ctx->a + 7) / 8);
    }
}

void norx32_add_header_block(norx32_ctx_t *ctx, const void *block)
{
    norx32_process_block(ctx, block, TAG_HEADER);
}

void norx32_add_header_last_block(
    norx32_ctx_t *ctx,
    const void *block,
    uint16_t length_b )
{
    norx32_process_last_block(ctx, NULL, block, length_b, TAG_HEADER);
}

void norx32_encrypt_block(norx32_ctx_t *ctx, void *dest, const void *src)
{
    norx32_process_block(ctx, src, TAG_PAYLOAD);
    if (dest) {
        memcpy(dest, ctx->s, RATE_BYTES);
    }
}

void norx32_encrypt_last_block(
    norx32_ctx_t *ctx,
    void *dest,
    const void *src,
    uint16_t length_b )
{
    norx32_process_last_block(ctx, dest, src, length_b, TAG_PAYLOAD);
}

void norx32_add_trailer_block(norx32_ctx_t *ctx, const void *block)
{
    norx32_process_block(ctx, block, TAG_TRAILER);
}

void norx32_add_trailer_last_block(
    norx32_ctx_t *ctx,
    const void *block,
    uint16_t length_b )
{
    norx32_process_last_block(ctx, NULL, block, length_b, TAG_TRAILER);
}

/******************************************************************************/

void norx32_default_simple (
    void *data_dest,
    void *tag_dest,
    const void *key,
    const void *nonce,
    const void *header,
    size_t header_length_B,
    const void *data_src,
    size_t data_length_B,
    const void *trailer,
    size_t trailer_length_B )
{
    norx32_ctx_t ctx;
    norx32_init(&ctx, nonce, key, 4 * WORD_SIZE, 4, 1);
    if (header && header_length_B) {
        norx32_add_header_last_block(&ctx, header, header_length_B * 8);
    }
    if (data_src && data_length_B) {
        norx32_encrypt_last_block(&ctx, data_dest, data_src, data_length_B * 8);
    }
    if (trailer && trailer_length_B) {
        norx32_add_trailer_last_block(&ctx, trailer, trailer_length_B * 8);
    }
    norx32_finalize(&ctx, tag_dest);
}
