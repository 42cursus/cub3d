/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   credits_avx2_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/11 15:56:02 by abelov            #+#    #+#             */
/*   Updated: 2025/08/11 15:56:03 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

inline __attribute__((always_inline, used, externally_visible))
t_rgba_ps256	unpack_rgba_bytes_to_floats_avx(__m256i pixels)
{
	t_rgba_ps256	out;
	t_vec4i_avx		vec;
	const __m256i	shuffle
		= _mm256_setr_epi8(0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15,
			16, 20, 24, 28, 17, 21, 25, 29, 18, 22, 26, 30, 19, 23, 27, 31);

	vec.r0 = _mm256_setzero_si256();
	vec.r1 = _mm256_shuffle_epi8(pixels, shuffle);
	vec.r2 = _mm256_unpackhi_epi8(vec.r1, vec.r0);
	vec.r3 = _mm256_unpacklo_epi8(vec.r1, vec.r0);
	out.b = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(vec.r3, vec.r0));
	out.g = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(vec.r3, vec.r0));
	out.r = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(vec.r2, vec.r0));
	out.a = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(vec.r2, vec.r0));
	return (out);
}

inline __attribute__((always_inline, used, externally_visible))
t_rgba_ps256	lerp_biased_unpvec(t_rgba_ps256 argb_a, t_rgba_ps256 argb_b,
					__m256 tt)
{
	t_rgba_ps256	out;
	const __m256	diff = _mm256_sub_ps(argb_b.a, argb_a.a);
	const __m256	mask = _mm256_cmp_ps(argb_a.a, argb_b.a, _CMP_LT_OQ);

	out.a = _mm256_add_ps(_mm256_mul_ps(diff, tt), argb_a.a);
	out.r = _mm256_blendv_ps(argb_b.r, argb_a.r, mask);
	out.g = _mm256_blendv_ps(argb_b.g, argb_a.g, mask);
	out.b = _mm256_blendv_ps(argb_b.b, argb_a.b, mask);
	return (out);
}

inline __attribute__((always_inline, used, externally_visible))
t_rgba_ps256	dim_colour2_unpvec_avx2(t_rgba_ps256 in, float dim)
{
	t_rgba_ps256	out;
	__m256			dim_vec;
	__m256			alpha_vec;
	const __m256	alphas = _mm256_set1_ps(255.0f);
	const __m256	mask = _mm256_cmp_ps(in.a, alphas, _CMP_EQ_OQ);

	dim = fmaxf(0.0f, fminf(1.0f, dim));
	dim_vec = _mm256_set1_ps(dim);
	alpha_vec = _mm256_set1_ps((1.0f - dim) * 255.0f);
	out.a = alpha_vec;
	out.r = _mm256_mul_ps(in.r, dim_vec);
	out.g = _mm256_mul_ps(in.g, dim_vec);
	out.b = _mm256_mul_ps(in.b, dim_vec);
	out.a = _mm256_blendv_ps(out.a, in.a, mask);
	out.r = _mm256_blendv_ps(out.r, in.r, mask);
	out.g = _mm256_blendv_ps(out.g, in.g, mask);
	out.b = _mm256_blendv_ps(out.b, in.b, mask);
	return (out);
}

inline __attribute__((always_inline, used, externally_visible))
t_rgba4_ps256	get_and_unpack(t_vec2i_avx xx, const int **row)
{
	t_vec4i_avx		source;
	t_rgba4_ps256	out;

	source.r0 = _mm256_i32gather_epi32(row[0], xx.r0, sizeof(int));
	source.r1 = _mm256_i32gather_epi32(row[0], xx.r1, sizeof(int));
	source.r2 = _mm256_i32gather_epi32(row[1], xx.r0, sizeof(int));
	source.r3 = _mm256_i32gather_epi32(row[1], xx.r1, sizeof(int));
	out.r0 = unpack_rgba_bytes_to_floats_avx(source.r0);
	out.r1 = unpack_rgba_bytes_to_floats_avx(source.r1);
	out.r2 = unpack_rgba_bytes_to_floats_avx(source.r2);
	out.r3 = unpack_rgba_bytes_to_floats_avx(source.r3);
	return (out);
}

inline __attribute__((always_inline, used, externally_visible))
t_rgba4_ps256	dim_avx2_unp4(float dim, t_rgba4_ps256 s)
{
	const t_rgba4_ps256	d = {
		dim_colour2_unpvec_avx2(s.r0, dim),
		dim_colour2_unpvec_avx2(s.r1, dim),
		dim_colour2_unpvec_avx2(s.r2, dim),
		dim_colour2_unpvec_avx2(s.r3, dim),
	};

	return (d);
}
