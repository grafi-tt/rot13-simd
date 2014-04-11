#include <stddef.h>
#include <stdint.h>
#include <emmintrin.h>
#include <smmintrin.h>

/* FIXME */
#ifdef __GNUC__
#define bsf __builtin_ctz
#endif
#ifdef __INTEL_COMPILER
#define bsf _bit_scan_forward
#endif

static const unsigned char TABLE[256] = {
/*
256.times do |i|
  j = i
  [0x41, 0x61].each do |a|
    if a <= i && i < a + 26
      j = a + (i - a + 13) % 26
    end
  end
  print "0x%02X," % j
  print i % 16 == 15 ? "\n" : " "
end
*/
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x41, 0x42,
    0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x61, 0x62,
    0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

void rot13_naivetable(char *s) {
    for (; *s; s++) *s = TABLE[(unsigned char) *s];
}

void rot13_simd(char *s) {
    ptrdiff_t offset = (uintptr_t) s & 15;
    s = s - offset;

    /* naive
    __m128i xmm_const_0 = _mm_set1_epi8(0);
    __m128i xmm_const_1 = _mm_set1_epi8(1);
    __m128i xmm_const_13 = _mm_set1_epi8(13);
    __m128i xmm_const_13_skip = _mm_set1_epi16(13);
    __m128i xmm_const_mask = _mm_set1_epi8(0xDF);
    __m128i xmm_const_A_ = _mm_set1_epi8('A' - 1);
    __m128i xmm_const_M = _mm_set1_epi8('M');
    __m128i xmm_const_Z = _mm_set1_epi8('Z');
    */

    __m128i xmm_const_0 = _mm_setzero_si128();
    __m128i xmm_const_m1;
    xmm_const_m1 = _mm_cmpeq_epi8(xmm_const_m1, xmm_const_m1); /* ignore warning */
    __m128i xmm_const_1 = _mm_sub_epi8(xmm_const_0, xmm_const_m1);

    __m128i xmm_const_16 = _mm_slli_epi16(xmm_const_1, 4);
    __m128i xmm_const_15 = _mm_add_epi8(xmm_const_16, xmm_const_m1);
    __m128i xmm_const_2 = _mm_add_epi8(xmm_const_1, xmm_const_1);
    __m128i xmm_const_13 = _mm_andnot_si128(xmm_const_2, xmm_const_15);
    __m128i xmm_const_13_skip = _mm_srli_epi16(xmm_const_13, 8);

    __m128i xmm_const_32 = _mm_add_epi8(xmm_const_16, xmm_const_16);
    __m128i xmm_const_mask = _mm_xor_si128(xmm_const_m1, xmm_const_32);

    __m128i xmm_const_A_ = _mm_add_epi8(xmm_const_32, xmm_const_32); /* 'A' - 1 = 64 */
    __m128i xmm_const_M = _mm_add_epi16(xmm_const_A_, xmm_const_13);
    __m128i xmm_const_Z = _mm_add_epi16(xmm_const_M, xmm_const_13);

    __m128i xmm_orig = _mm_load_si128((__m128i*) s);
    /* map 'a'-'z' into 'A'-'Z' */
    __m128i xmm_masked = _mm_and_si128(xmm_orig, xmm_const_mask);

    __m128i xmm_border, xmm_zero_vec, xmm_new;

    /* `xmm_masked` is only used for determining the value added to char, so setting unused position into '\0' (no addition) is ok */
    if (offset <= 8) {
        uint64_t border = (uint64_t) (int64_t) -1 >> ((8 - offset) << 3);
        xmm_border = _mm_cvtsi64_si128(border);
        xmm_border = _mm_xor_si128(xmm_border, xmm_const_m1);
    } else {
        uint64_t border = (uint64_t) (int64_t) -1 << ((offset - 8) << 3);
        xmm_border = _mm_cvtsi64_si128(border);
        xmm_border = _mm_slli_si128(xmm_border, 8);
    }
    do { // copied loop
        xmm_orig = _mm_load_si128((__m128i*) s);
        xmm_masked = _mm_and_si128(xmm_orig, xmm_const_mask);

        xmm_zero_vec = _mm_cmpeq_epi8(xmm_orig, xmm_const_0);
        xmm_zero_vec = _mm_and_si128(xmm_zero_vec, xmm_border);
        int all_zero = _mm_test_all_zeros(xmm_zero_vec, xmm_zero_vec);
        __m128i xmm_origm13 = _mm_sub_epi8(xmm_orig, xmm_const_13);

        __m128i xmm_diff_A_ = _mm_cmpgt_epi8(xmm_masked, xmm_const_A_);
        __m128i xmm_diff_M = _mm_cmpgt_epi8(xmm_masked, xmm_const_M);
        __m128i xmm_diff_Z = _mm_cmpgt_epi8(xmm_masked, xmm_const_Z);

        __m128i xmm_diff_2Mp1 = _mm_or_si128(xmm_diff_M, xmm_const_1);
        __m128i xmm_diff_A_Z = _mm_add_epi8(xmm_diff_A_, xmm_diff_Z);
        __m128i xmm_diff_unified = _mm_sub_epi8(xmm_diff_2Mp1, xmm_diff_A_Z);
        __m128i xmm_diff_scaled_p13 = _mm_mullo_epi16(xmm_diff_unified, xmm_const_13_skip);
        xmm_new = _mm_add_epi8(xmm_origm13, xmm_diff_scaled_p13);

        if (!all_zero) goto loop_end;
        _mm_maskmoveu_si128(xmm_new, xmm_border, s);
    } while (0);
    s += 16;

    xmm_border = xmm_const_m1;
    for (; ; s += 16) {
        xmm_orig = _mm_load_si128((__m128i*) s);
        /* map 'a'-'z' into 'A'-'Z' */
        xmm_masked = _mm_and_si128(xmm_orig, xmm_const_mask);

        xmm_zero_vec = _mm_cmpeq_epi8(xmm_orig, xmm_const_0);
        int all_zero = _mm_test_all_zeros(xmm_zero_vec, xmm_zero_vec);
        __m128i xmm_origm13 = _mm_sub_epi8(xmm_orig, xmm_const_13);

        /*
        diff_A_ = c > 'A' - 1 ? -1 : 0
        diff_M = c > 'M' 1 ? -1 : 0
        diff_Z = c > 'Z' 1 ? -1 : 0
        (c - 13) + ((diff_M_ * 2 + 1) - (diff_A + diff_Z)) * 13
        comparing is done for singed char, but that doesn't change result
        */
        __m128i xmm_diff_A_ = _mm_cmpgt_epi8(xmm_masked, xmm_const_A_);
        __m128i xmm_diff_M = _mm_cmpgt_epi8(xmm_masked, xmm_const_M);
        __m128i xmm_diff_Z = _mm_cmpgt_epi8(xmm_masked, xmm_const_Z);

        __m128i xmm_diff_2Mp1 = _mm_or_si128(xmm_diff_M, xmm_const_1);
        __m128i xmm_diff_A_Z = _mm_add_epi8(xmm_diff_A_, xmm_diff_Z);
        __m128i xmm_diff_unified = _mm_sub_epi8(xmm_diff_2Mp1, xmm_diff_A_Z);
        __m128i xmm_diff_scaled_p13 = _mm_mullo_epi16(xmm_diff_unified, xmm_const_13_skip);
        xmm_new = _mm_add_epi8(xmm_origm13, xmm_diff_scaled_p13);

        if (!all_zero) break;
        _mm_store_si128((__m128i*) s, xmm_new);
    }

loop_end: ;
    __m128i xmm_zero_vec_neg_64 = _mm_sub_epi64(xmm_const_0, xmm_zero_vec);
    __m128i xmm_zero_vec_lowest_64 = _mm_and_si128(xmm_zero_vec_neg_64, xmm_zero_vec);
    __m128i xmm_zero_vec_extended_64 = _mm_sub_epi64(xmm_const_0, xmm_zero_vec_lowest_64);
    /* high latency
    __m128i xmm_zero_vec_allextended_64 = _mm_cmpgt_epi64(xmm_const_0, xmm_zero_vec_extended_64);
    */
    __m128i xmm_zero_vec_extended_64_sign = _mm_srli_epi64(xmm_zero_vec_extended_64, 63);
    __m128i xmm_zero_vec_allextended_64 = _mm_sub_epi64(xmm_const_0, xmm_zero_vec_extended_64_sign);
    __m128i xmm_zero_vec_allextended_64_moved = _mm_slli_si128(xmm_zero_vec_allextended_64, 8);
    __m128i xmm_zero_vec_extended = _mm_or_si128(xmm_zero_vec_extended_64, xmm_zero_vec_allextended_64_moved);

    xmm_border = _mm_andnot_si128(xmm_zero_vec_extended, xmm_border);
    _mm_maskmoveu_si128(xmm_new, xmm_border, s); // maskmove
}

void rot13_unrolltable(char *s) {
#define UNROLL1(x) case x: if (!*(s+x)) return; *(s+x) = TABLE[(unsigned char) *(s+x)];
    unsigned int i = (uintptr_t) s & 15;
    s = s - i;
    switch (i) {
    default: break;
    UNROLL1(1)
    UNROLL1(2)
    UNROLL1(3)
    UNROLL1(4)
    UNROLL1(5)
    UNROLL1(6)
    UNROLL1(7)
    UNROLL1(8)
    UNROLL1(9)
    UNROLL1(10)
    UNROLL1(11)
    UNROLL1(12)
    UNROLL1(13)
    UNROLL1(14)
    UNROLL1(15)
    s = s + 16;
    }
    int zero_vec;

    __m128i xmm_const_0 = _mm_setzero_si128();
    for (; ; s += 16) {
        __m128i xmm = _mm_load_si128((__m128i*) s);
        __m128i xmm_zero_vec = _mm_cmpeq_epi8(xmm, xmm_const_0);
        zero_vec = _mm_movemask_epi8(xmm_zero_vec);
        if (zero_vec) break;

        uint64_t u = _mm_cvtsi128_si64(xmm);
        uint64_t v = TABLE[u & 255];
        v |= (uint64_t) TABLE[u >> 8 & 255] << 8;
        v |= (uint64_t) TABLE[u >> 16 & 255] << 16;
        v |= (uint64_t) TABLE[u >> 24 & 255] << 24;
        v |= (uint64_t) TABLE[u >> 32 & 255] << 32;
        v |= (uint64_t) TABLE[u >> 40 & 255] << 40;
        v |= (uint64_t) TABLE[u >> 48 & 255] << 48;
        v |= (uint64_t) TABLE[u >> 56] << 56;
        *(uint64_t*) s = v;

        xmm = _mm_srli_si128(xmm, 8);
        u = _mm_cvtsi128_si64(xmm);
        v = TABLE[u & 255];
        v |= (uint64_t) TABLE[u >> 8 & 255] << 8;
        v |= (uint64_t) TABLE[u >> 16 & 255] << 16;
        v |= (uint64_t) TABLE[u >> 24 & 255] << 24;
        v |= (uint64_t) TABLE[u >> 32 & 255] << 32;
        v |= (uint64_t) TABLE[u >> 40 & 255] << 40;
        v |= (uint64_t) TABLE[u >> 48 & 255] << 48;
        v |= (uint64_t) TABLE[u >> 56] << 56;
        *((uint64_t*) s + 1) = v;
    }

    unsigned int zero_pos = bsf(zero_vec);
#define UNROLL2(x) case x: *(s+x-1) = TABLE[(unsigned char) *(s+x-1)];
    switch (zero_pos) {
    UNROLL2(15)
    UNROLL2(14)
    UNROLL2(13)
    UNROLL2(12)
    UNROLL2(11)
    UNROLL2(10)
    UNROLL2(9)
    UNROLL2(8)
    UNROLL2(7)
    UNROLL2(6)
    UNROLL2(5)
    UNROLL2(4)
    UNROLL2(3)
    UNROLL2(2)
    UNROLL2(1)
    }
}
