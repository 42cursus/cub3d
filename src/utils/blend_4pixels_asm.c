/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blend_4pixels_asm.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:01:58 by abelov            #+#    #+#             */
/*   Updated: 2025/06/03 21:01:58 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"

static inline __attribute__((always_inline, used))
__m128i blend_4pixels_asm(__m128i fg, __m128i bg)
{
	__m128  alpha_vec;
	__m128i shuffle;
	__m128i result;

	__asm__ __volatile__ (
		// alpha_vec = _mm_set1_ps(1.0f / 255.0f)
		"vbroadcastss %[scale], %[a_vec]\n\t"
		"movdqu %[shuf_data], %[shuf]\n\t"

		// Load pixels from fg and bg
		"movdqu (%[fg]), %%xmm0\n\t"
		"movdqu (%[bg]), %%xmm1\n\t"

		// Zero xmm2
		"vpxor %%xmm2, %%xmm2, %%xmm2\n\t"

		// Unpack high bytes for fg and bg (to words)
		"vpunpckhbw %%xmm2, %%xmm0, %%xmm3\n\t"
		"vpunpckhbw %%xmm2, %%xmm1, %%xmm4\n\t"

		// Extract alpha bytes using shuffle
		"vpshufb %[shuf], %%xmm0, %%xmm0\n\t"
		"vpshufb %[shuf], %%xmm1, %%xmm1\n\t"

		// Zero-extend unpacked 8-bit RGB channels
		"vpmovzxbd %%xmm3, %%xmm5\n\t" // fg.rgb
		"vpmovzxbd %%xmm4, %%xmm6\n\t" // bg.rgb

		// Convert to float
		"vcvtdq2ps %%xmm0, %%xmm7\n\t" // fg alpha
		"vcvtdq2ps %%xmm1, %%xmm8\n\t" // bg alpha
		"vcvtdq2ps %%xmm5, %%xmm9\n\t" // fg.rgb
		"vcvtdq2ps %%xmm6, %%xmm10\n\t" // bg.rgb

		// Normalize alpha (alpha_vec * alpha)
		"vmulps %[a_vec], %%xmm7, %%xmm7\n\t"
		"vmulps %[a_vec], %%xmm8, %%xmm8\n\t"

		// out.rgb = fg.rgb + (1 - fg_alpha) * bg.rgb
		"vsubps %%xmm7, %%xmm9, %%xmm11\n\t"
		"vfmadd213ps %%xmm9, %%xmm7, %%xmm10\n\t"

		// Convert back to int
		"vcvtps2dq %%xmm10, %%xmm10\n\t"
		"packssdw %%xmm10, %%xmm10\n\t"
		"packuswb %%xmm10, %%xmm10\n\t"

		// Return result
		"movdqa %%xmm10, %[res]\n\t"

		: [res] "=x" (result), [a_vec] "=x" (alpha_vec), [shuf] "=x" (shuffle)
	: [scale] "m" ( (float){1.0f / 255.0f} ), [shuf_data] "m" ((const char[16]){
		0x0C, -1, -1, -1,
		0x08, -1, -1, -1,
		0x04, -1, -1, -1,
		0x00, -1, -1, -1 }), [fg] "r" (fg), [bg] "r" (bg)
	: "memory", "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5",
		"xmm6", "xmm7", "xmm8", "xmm9", "xmm10", "xmm11"
	);
	return result;
}

