/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   place_img_alpha_sse_soa.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 00:17:27 by abelov            #+#    #+#             */
/*   Updated: 2025/06/30 00:17:28 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"

/**
 * Stage 1: Unpack 4 RGBA pixels into 4 __m128 float vectors
 * gdb -batch -ex 'file ./cub3d' -ex 'disassemble /s unpack_rgba_bytes_to_floats'
 * @param pixels
 * @return
 */
static inline __attribute__((always_inline, used))
t_rgba_ps128 unpack_rgba_si128_to_ps128_soa(__m128i pixels)
{
	t_rgba_ps128	out;
	t_rgba_si128	rgba;
	const __m128i	mask_8 = _mm_set1_epi32(0xFF);

	rgba.b = _mm_and_si128(pixels, mask_8);
	rgba.g = _mm_and_si128(_mm_srli_epi32(pixels, 8), mask_8);
	rgba.r = _mm_and_si128(_mm_srli_epi32(pixels, 16), mask_8);
	rgba.a = _mm_and_si128(_mm_srli_epi32(pixels, 24), mask_8);

	out.b = _mm_cvtepi32_ps(rgba.b);
	out.g = _mm_cvtepi32_ps(rgba.g);
	out.r = _mm_cvtepi32_ps(rgba.r);
	out.a = _mm_cvtepi32_ps(rgba.a);

	return out;
}

/**
 * Stage 2: Extract transparency (alpha / 255)
 * MLX System (with inverted alpha):
 * 	transparency = alpha / 255
 * 	opacity = 1 - transparency
 * @param s
 * @return
 */
static inline __attribute__((always_inline, used))
__m128 extract_transparency_sse_soa(t_rgba_ps128 fs)
{
	__m128 out;
	const __m128 byte = _mm_set1_ps(255.0f);

	out = _mm_div_ps(fs.a, byte);
	return (out);
}

/**
 * Stage 3: Blend pixel = ((dst - src) * transparency) + src
 * @param src
 * @param dst
 * @param alpha
 * @return
 */
static inline __attribute__((always_inline, used))
t_rgba_ps128 blend_pixels_avx2_soa(t_rgba_ps128 src, t_rgba_ps128 dst, __m128 transparency)
{
	t_rgba_ps128 diff;
	t_rgba_ps128 out;

	diff.b = _mm_sub_ps(dst.b, src.b);
	diff.g = _mm_sub_ps(dst.g, src.g);
	diff.r = _mm_sub_ps(dst.r, src.r);
	diff.a = _mm_sub_ps(dst.a, src.a);

	out.b = _mm_add_ps(src.b, _mm_mul_ps(diff.b, transparency));
	out.g = _mm_add_ps(src.g, _mm_mul_ps(diff.g, transparency));
	out.r = _mm_add_ps(src.r, _mm_mul_ps(diff.r, transparency));
	out.a = _mm_add_ps(src.a, _mm_mul_ps(diff.a, transparency));

	return out;
}

/**
 * Stage 4: Convert 4 float vectors to a packed 4-pixel __m128i
 * @param blended
 * @return
 */
static inline __attribute__((always_inline))
__m128i repack_floats_to_bytes_soa(t_rgba_ps128 blended)
{
	t_rgba_si128	rgba;
	__m128i			out;
	const __m128i	zero = _mm_setzero_si128();
	const __m128i	max255 = _mm_set1_epi32(255);

	rgba.b = _mm_cvtps_epi32(blended.b);
	rgba.g = _mm_cvtps_epi32(blended.g);
	rgba.r = _mm_cvtps_epi32(blended.r);
	rgba.a = _mm_cvtps_epi32(blended.a);

	// Clamp channels to [0, 255]
	rgba.b = _mm_min_epi32(_mm_max_epi32(rgba.b, zero), max255);
	rgba.g = _mm_min_epi32(_mm_max_epi32(rgba.g, zero), max255);
	rgba.r = _mm_min_epi32(_mm_max_epi32(rgba.r, zero), max255);
	rgba.a = _mm_min_epi32(_mm_max_epi32(rgba.a, zero), max255);

	// Pack to 8-bit: RGBA per pixel via Shift and OR to combine into 0xAABBGGRR
	rgba.b = _mm_slli_epi32(rgba.b, 0);
	rgba.g = _mm_slli_epi32(rgba.g, 8);
	rgba.r = _mm_slli_epi32(rgba.r, 16);
	rgba.a = _mm_slli_epi32(rgba.a, 24);

	out = _mm_or_si128(rgba.b, _mm_or_si128(rgba.g, _mm_or_si128(rgba.r, rgba.a)));
	return (out);
}

static inline __attribute__((always_inline, used))
void blend_4pixels_soa(int *src, int *dst)
{
	const __m128i	_src = _mm_loadu_si128((__m128i *)src);
	const __m128i	_dst = _mm_loadu_si128((__m128i *)dst);
	t_rgba_ps128	fs = unpack_rgba_si128_to_ps128_soa(_src);
	t_rgba_ps128	fd = unpack_rgba_si128_to_ps128_soa(_dst);
	__m128			transparency = extract_transparency_sse_soa(fs);
	t_rgba_ps128	blended = blend_pixels_avx2_soa(fs, fd, transparency);

	_mm_storeu_si128((__m128i *)dst, repack_floats_to_bytes_soa(blended));
}

/**
 * Blend formula:
 * 		`final_color = src * opacity + dst * transparency`
 *
 * Canonical alpha convention:
 * 	opacity = alpha / 255
 * 	transparency = 1 - opacity
 *
 * 	blend formula becomes:
 * 		`final_color = src * (alpha / 255.0) + dst * (1 - alpha / 255.0)`
 * 		or
 * 		`final_color = ((src - dst) * opacity) + dst`
 *
 * MLX System (with inverted alpha):
 * 	transparency = alpha / 255
 * 	opacity = 1 - transparency
 *
 * 	blend formula becomes:
 * 		`final_color = src * (1 - alpha / 255.0) + dst * (alpha / 255.0)`
 * 		or
 * 		`final_color = ((dst - src) * transparency) + src`
 *
 * @param image
 * @param tile
 * @param p
 */
inline __attribute__((always_inline, used))
void	place_img_alpha_sse_soa(t_img *image, t_img *tile, t_point p)
{
	t_point	it;
	t_point	offset;
	t_point	limit;
	t_cdata cd;
	t_m128i mc;

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
		while (it.x + 3 < limit.x)
		{
			blend_4pixels_soa((cd.src + it.x), (cd.dst + it.x));
			it.x += 4;
		}
		while (it.x < limit.x)
		{
			mc.colour = cd.src[it.x];

			t_colour src = *(t_colour *) &mc.colour;
			t_colour dst = *(t_colour *) &cd.dst[it.x];

			double transparency = src.a / 255.0;
			if (src.raw != dst.raw)
			{
				src.r = lround((dst.r - src.r) * transparency + src.r);
				src.g = ((dst.g - src.g) * transparency) + src.g + 0.5;
				src.b = ((dst.b - src.b) * transparency) + src.b + 0.5;
			}
			cd.dst[it.x] = src.raw;
			it.x++;
		}
	}
}
