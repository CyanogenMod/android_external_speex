/* Copyright (C) 2007-2008 Jean-Marc Valin
 * Copyright (C) 2008 Thorvald Natvig
 * Copyright (C) 2011 Jyri Sarha, Texas Instruments
 */
/**
   @file resample_neon.h
   @brief Resampler functions (NEON version)
*/
/*
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <arm_neon.h>

/* NOTE: This code only works with following filter lengths:
         8 or n*16 (where n = 1,2,3...)
         The current resampler quality presets follow the above rule.
*/

#ifdef FIXED_POINT
#define OVERRIDE_INNER_PRODUCT_SINGLE
static inline int32_t inner_product_single(const int16_t *a, const int16_t *b, unsigned int len)
{
    int32_t ret;
    if (len > 8) {
        asm volatile ("      vld1.16 {d16, d17, d18, d19}, [%[a]]!\n"
                      "      vld1.16 {d20, d21, d22, d23}, [%[b]]!\n"
                      "      subs %[len], %[len], #16\n"
                      "      vmull.s16 q0, d16, d20\n"
                      "      vmlal.s16 q0, d17, d21\n"
                      "      vmlal.s16 q0, d18, d22\n"
                      "      vmlal.s16 q0, d19, d23\n"
                      "      beq 2f\n"
                      "1:"
                      "      vld1.16 {d16, d17, d18, d19}, [%[a]]!\n"
                      "      vld1.16 {d20, d21, d22, d23}, [%[b]]!\n"
                      "      subs %[len], %[len], #16\n"
                      "      vmlal.s16 q0, d16, d20\n"
                      "      vmlal.s16 q0, d17, d21\n"
                      "      vmlal.s16 q0, d18, d22\n"
                      "      vmlal.s16 q0, d19, d23\n"
                      "      bne 1b\n"
                      "2:"
                      "      vaddl.s32 q0, d0, d1\n"
                      "      vadd.s64 d0, d0, d1\n"
                      "      vqmovn.s64 d0, q0\n"
                      "      vqrshrn.s32 d0, q0, #15\n"
                      "      vmov.s16 %[ret],d0[0]\n"
                      : [ret] "=&r" (ret), [a] "+r" (a), [b] "+r" (b),
                        [len] "+r" (len)
                      :
                      : "cc", "q0",
                        "d16", "d17", "d18", "d19",
                        "d20", "d21", "d22", "d23");
    }
    else {
        asm volatile ("vld1.16 {d4, d5}, [%[a]]\n"
                      "vld1.16 {d6, d7}, [%[b]]\n"
                      "vmull.s16 q0, d4, d6\n"
                      "vmlal.s16 q0, d5, d7\n"
                      "vaddl.s32 q0, d0, d1\n"
                      "vadd.s64 d0, d0, d1\n"
                      "vqmovn.s64 d0, q0\n"
                      "vqrshrn.s32 d0, q0, #15\n"
                      "vmov.s16 %[ret],d0[0]\n"
                      : [ret] "=&r" (ret)
                      : [a] "r" (a), [b] "r" (b)
                      : "q0", "d4", "d5", "d6", "d7");
    }
    return ret;
}
#endif
