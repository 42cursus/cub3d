/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   place_img_alpha_avx2.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 22:07:57 by abelov            #+#    #+#             */
/*   Updated: 2025/06/29 22:07:59 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"

static inline __attribute__((always_inline, used))
t_vec4f_avx	unpack_rgba_bytes_to_floats_avx2(__m256i pixels)
{
	t_vec4f_avx         out;
	const __m256i   zero = _mm256_setzero_si256();

	const __m256i lo8 = _mm256_unpacklo_epi8(pixels, zero);
	const __m256i hi8 = _mm256_unpackhi_epi8(pixels, zero);

	out.r0 = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(lo8, zero));
	out.r1 = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(lo8, zero));
	out.r2 = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(hi8, zero));
	out.r3 = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(hi8, zero));
	return (out);
}

static inline __attribute__((always_inline, used))
t_vec4f_avx	extract_transparency_avx2(t_vec4f_avx fs)
{
	t_vec4f_avx			alpha;
	t_vec4f_avx			transparency;
	const __m256	byte = _mm256_set1_ps(255.0f);

	alpha.r0 = _mm256_permute_ps(fs.r0, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r1 = _mm256_permute_ps(fs.r1, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r2 = _mm256_permute_ps(fs.r2, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r3 = _mm256_permute_ps(fs.r3, _MM_SHUFFLE(3, 3, 3, 3));

	transparency.r0 = _mm256_div_ps(alpha.r0, byte);
	transparency.r1 = _mm256_div_ps(alpha.r1, byte);
	transparency.r2 = _mm256_div_ps(alpha.r2, byte);
	transparency.r3 = _mm256_div_ps(alpha.r3, byte);

	return (transparency);
}

static inline __attribute__((always_inline, used))
t_vec4f_avx blend_pixels_avx2(t_vec4f_avx src, t_vec4f_avx dst, t_vec4f_avx transparency)
{
	t_vec4f_avx diff;
	t_vec4f_avx out;

	diff.r0 = _mm256_sub_ps(dst.r0, src.r0);
	diff.r1 = _mm256_sub_ps(dst.r1, src.r1);
	diff.r2 = _mm256_sub_ps(dst.r2, src.r2);
	diff.r3 = _mm256_sub_ps(dst.r3, src.r3);

	out.r0 = _mm256_add_ps(src.r0, _mm256_mul_ps(diff.r0, transparency.r0));
	out.r1 = _mm256_add_ps(src.r1, _mm256_mul_ps(diff.r1, transparency.r1));
	out.r2 = _mm256_add_ps(src.r2, _mm256_mul_ps(diff.r2, transparency.r2));
	out.r3 = _mm256_add_ps(src.r3, _mm256_mul_ps(diff.r3, transparency.r3));

	return out;
}


static inline __attribute__((always_inline, used))
__m256i repack_floats_to_bytes_avx2(t_vec4f_avx blended)
{
	t_vec4i_avx blended_i;
	t_vec2i_avx packed;

	blended_i.r0 = _mm256_cvtps_epi32(blended.r0);
	blended_i.r1 = _mm256_cvtps_epi32(blended.r1);
	blended_i.r2 = _mm256_cvtps_epi32(blended.r2);
	blended_i.r3 = _mm256_cvtps_epi32(blended.r3);

	packed.r0 = _mm256_packs_epi32(blended_i.r0, blended_i.r1);
	packed.r1 = _mm256_packs_epi32(blended_i.r2, blended_i.r3);

	__m256i out = _mm256_packus_epi16(packed.r0, packed.r1);
	return (out);
}

static inline __attribute__((always_inline, used))
void blend_8pixels(int *src, int *dst)
{
	const __m256i _src = _mm256_loadu_si256((__m256i *) src);
	const __m256i _dst = _mm256_loadu_si256((__m256i *) dst);

	t_vec4f_avx fs = unpack_rgba_bytes_to_floats_avx2(_src);
	t_vec4f_avx fd = unpack_rgba_bytes_to_floats_avx2(_dst);

	t_vec4f_avx transparency = extract_transparency_avx2(fs);
	t_vec4f_avx blended = blend_pixels_avx2(fs, fd, transparency);

	_mm256_storeu_si256((__m256i *)dst, repack_floats_to_bytes_avx2(blended));
}

static inline __attribute__((always_inline, used))
t_point	blend_pixels_scalar(int limit_x, t_point it, t_cdata cd)
{
	t_mcol	mc;

	while (it.x < limit_x)
	{
		mc.colour = cd.src[it.x];

		t_colour src = *(t_colour *) &mc.colour;
		t_colour dst = *(t_colour *) &cd.dst[it.x];

		double transparency = src.a / 255.0;
		if (src.raw != dst.raw)
		{
			src.r = ((dst.r - src.r) * transparency) + src.r + 0.5;
			src.g = ((dst.g - src.g) * transparency) + src.g + 0.5;
			src.b = ((dst.b - src.b) * transparency) + src.b + 0.5;
		}
		cd.dst[it.x] = src.raw;
		it.x++;
	}
	return (it);
}

inline __attribute__((always_inline, used))
void	place_img_alpha_avx2(t_img *image, t_img *tile, t_point p)
{
	t_point	it;
	t_point	offset;
	t_point	limit;
	t_cdata cd;

	offset.x = -p.x * (p.x < 0);
	offset.y = -p.y * (p.y < 0);

	limit.x = MIN(tile->width, image->width - p.x);
	limit.y = MIN(tile->height, image->height - p.y);
	it.y = offset.y - 1;
	while (++it.y < limit.y)
	{
		cd.src = (int *) tile->data + it.y * tile->width;
		cd.dst = (int *) image->data + (it.y + p.y) * image->width + p.x;
		it.x = offset.x;
		while (it.x + 7 < limit.x)
		{
			blend_8pixels((cd.src + it.x), (cd.dst + it.x));
			it.x += 8;
		}
		it = blend_pixels_scalar(limit.x, it, cd);
	}
}
