/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   place_img_alpha_sse.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 22:32:29 by abelov            #+#    #+#             */
/*   Updated: 2025/06/29 22:32:30 by abelov           ###   ########.fr       */
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
t_vec4f_sse	unpack_rgba_bytes_to_floats(__m128i pixels)
{
	t_vec4f_sse		out;
	const __m128i	zero = _mm_setzero_si128();
	const __m128i	lo = _mm_unpacklo_epi8(pixels, zero);
	const __m128i	hi = _mm_unpackhi_epi8(pixels, zero);

	out.r0 = _mm_cvtepi32_ps(_mm_unpacklo_epi16(lo, zero));
	out.r1 = _mm_cvtepi32_ps(_mm_unpackhi_epi16(lo, zero));
	out.r2 = _mm_cvtepi32_ps(_mm_unpacklo_epi16(hi, zero));
	out.r3 = _mm_cvtepi32_ps(_mm_unpackhi_epi16(hi, zero));
	return (out);
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
t_vec4f_sse	extract_transparency(t_vec4f_sse fs)
{
	t_vec4f_sse 	alpha;
	t_vec4f_sse 	transparency;
	const __m128	byte = _mm_set1_ps(255.0f);

	alpha.r0 = _mm_shuffle_ps(fs.r0, fs.r0, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r1 = _mm_shuffle_ps(fs.r1, fs.r1, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r2 = _mm_shuffle_ps(fs.r2, fs.r2, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r3 = _mm_shuffle_ps(fs.r3, fs.r3, _MM_SHUFFLE(3, 3, 3, 3));

	transparency.r0 = _mm_div_ps(alpha.r0, byte);
	transparency.r1 = _mm_div_ps(alpha.r1, byte);
	transparency.r2 = _mm_div_ps(alpha.r2, byte);
	transparency.r3 = _mm_div_ps(alpha.r3, byte);

	return (transparency);
}

/**
 * Stage 3: Blend pixel = ((dst - src) * transparency) + src
 * @param src
 * @param dst
 * @param alpha
 * @return
 */
static inline __attribute__((always_inline, used))
t_vec4f_sse	blend_pixels(t_vec4f_sse src, t_vec4f_sse dst, t_vec4f_sse transparency)
{
	t_vec4f_sse	out;
	t_vec4f_sse	diff;

	diff.r0 = _mm_sub_ps(dst.r0, src.r0);
	diff.r1 = _mm_sub_ps(dst.r1, src.r1);
	diff.r2 = _mm_sub_ps(dst.r2, src.r2);
	diff.r3 = _mm_sub_ps(dst.r3, src.r3);

	out.r0 = _mm_add_ps(src.r0, _mm_mul_ps(diff.r0, transparency.r0));
	out.r1 = _mm_add_ps(src.r1, _mm_mul_ps(diff.r1, transparency.r1));
	out.r2 = _mm_add_ps(src.r2, _mm_mul_ps(diff.r2, transparency.r2));
	out.r3 = _mm_add_ps(src.r3, _mm_mul_ps(diff.r3, transparency.r3));
	return out;
}

/**
 * Stage 4: Convert 4 float vectors to a packed 4-pixel __m128i
 * @param blended
 * @return
 */
static inline __attribute__((always_inline))
__m128i	repack_floats_to_bytes(t_vec4f_sse blended)
{
	t_vec4f_sse i;

	i.r0 = _mm_cvtps_epi32(blended.r0);
	i.r1 = _mm_cvtps_epi32(blended.r1);
	i.r2 = _mm_cvtps_epi32(blended.r2);
	i.r3 = _mm_cvtps_epi32(blended.r3);

	__m128i p01 = _mm_packs_epi32(i.r0, i.r1);
	__m128i p23 = _mm_packs_epi32(i.r2, i.r3);
	return _mm_packus_epi16(p01, p23);
}

static inline __attribute__((always_inline, used))
void	blend_4pixels(int *src, int *dst)
{
	const __m128i	_src = _mm_loadu_si128((__m128i *) src);
	const __m128i	_dst = _mm_loadu_si128((__m128i *) dst);

	const t_vec4f_sse	fs = unpack_rgba_bytes_to_floats(_src);
	const t_vec4f_sse	fd = unpack_rgba_bytes_to_floats(_dst);

	const t_vec4f_sse	transparency = extract_transparency(fs);
	const t_vec4f_sse	blended = blend_pixels(fs, fd, transparency);

	_mm_storeu_si128((__m128i *)dst, repack_floats_to_bytes(blended));
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
void	place_img_alpha_sse(t_img *image, t_img *tile, t_point p)
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
			blend_4pixels((cd.src + it.x), (cd.dst + it.x));
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
				src.r = ((dst.r - src.r) * transparency) + src.r + 0.5;
				src.g = ((dst.g - src.g) * transparency) + src.g + 0.5;
				src.b = ((dst.b - src.b) * transparency) + src.b + 0.5;
			}
			cd.dst[it.x] = src.raw;
			it.x++;
		}
	}
}
