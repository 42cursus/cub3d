/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   place_img_alpha_avx2_soa.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 22:26:19 by abelov            #+#    #+#             */
/*   Updated: 2025/06/29 22:26:20 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include <stdalign.h>
#include "cub3d.h"

/**
 * https://github.com/qingtian805/My-SM4/blob/main/src/sm4avx/sm4avx.c#L301
 * @param pixels
 * @return
 */
static inline __attribute__((always_inline, used))
t_vec4f_avx	unpack_rgba_bytes_to_floats_avx2_soa(__m256i pixels)
{
	const __m256i mask_8 = _mm256_set1_epi32(0xFF);

	__m256i r = _mm256_and_si256(pixels, mask_8);
	__m256i g = _mm256_and_si256(_mm256_srli_epi32(pixels, 8), mask_8);
	__m256i b = _mm256_and_si256(_mm256_srli_epi32(pixels, 16), mask_8);
	__m256i a = _mm256_and_si256(_mm256_srli_epi32(pixels, 24), mask_8);

	t_vec4f_avx out;
	out.r0 = _mm256_cvtepi32_ps(r);
	out.r1 = _mm256_cvtepi32_ps(g);
	out.r2 = _mm256_cvtepi32_ps(b);
	out.r3 = _mm256_cvtepi32_ps(a);
	return out;
}

static inline __attribute__((always_inline, used))
__m256 extract_transparency_avx2_soa(t_vec4f_avx fs)
{
	__m256 out;
	const __m256 byte = _mm256_set1_ps(255.0f);

	out = _mm256_div_ps(fs.r3, byte);

	return (out);
}

static inline __attribute__((always_inline, used))
t_vec4f_avx blend_pixels_avx2_soa(t_vec4f_avx src, t_vec4f_avx dst, __m256 transparency)
{
	t_vec4f_avx diff;
	t_vec4f_avx out;

	diff.r0 = _mm256_sub_ps(dst.r0, src.r0);
	diff.r1 = _mm256_sub_ps(dst.r1, src.r1);
	diff.r2 = _mm256_sub_ps(dst.r2, src.r2);
	diff.r3 = _mm256_sub_ps(dst.r3, src.r3);

	out.r0 = _mm256_add_ps(src.r0, _mm256_mul_ps(diff.r0, transparency));
	out.r1 = _mm256_add_ps(src.r1, _mm256_mul_ps(diff.r1, transparency));
	out.r2 = _mm256_add_ps(src.r2, _mm256_mul_ps(diff.r2, transparency));
	out.r3 = _mm256_add_ps(src.r3, _mm256_mul_ps(diff.r3, transparency));

	return out;
}

static inline __attribute__((always_inline, used))
__m256i repack_floats_to_bytes_avx2_soa(t_vec4f_avx blended)
{
	t_vec4i_avx rgba;
	t_vec2i_sse hi_lo;
	t_vec2i_sse shuf;

	const __m256i zero = _mm256_setzero_si256();
	const __m256i max255 = _mm256_set1_epi32(255);
	const __m128i shuffle = _mm_setr_epi8(
		0,  4,  8, 12,
		1,  5,  9, 13,
		2,  6, 10, 14,
		3,  7, 11, 15
	);

	rgba.r0 = _mm256_cvtps_epi32(blended.r0);
	rgba.r1 = _mm256_cvtps_epi32(blended.r1);
	rgba.r2 = _mm256_cvtps_epi32(blended.r2);
	rgba.r3 = _mm256_cvtps_epi32(blended.r3);

	rgba.r0 = _mm256_min_epi32(_mm256_max_epi32(rgba.r0, zero), max255);
	rgba.r1 = _mm256_min_epi32(_mm256_max_epi32(rgba.r1, zero), max255);
	rgba.r2 = _mm256_min_epi32(_mm256_max_epi32(rgba.r2, zero), max255);
	rgba.r3 = _mm256_min_epi32(_mm256_max_epi32(rgba.r3, zero), max255);

	__m256i rg16 = _mm256_packs_epi32(rgba.r0, rgba.r1);
	__m256i ba16 = _mm256_packs_epi32(rgba.r2, rgba.r3);
	__m256i rgba8 = _mm256_packus_epi16(rg16, ba16);

	// Split into 2 128-bit lanes
	hi_lo.r0 = _mm256_extracti128_si256(rgba8, 0);
	hi_lo.r1 = _mm256_extracti128_si256(rgba8, 1);

	// Interleave RGBA (4 channels) for 4 pixels per lane (4×4 = 16 bytes)
	shuf.r0 = _mm_shuffle_epi8(hi_lo.r0, shuffle);
	shuf.r1 = _mm_shuffle_epi8(hi_lo.r1, shuffle);

	return _mm256_inserti128_si256(_mm256_castsi128_si256(shuf.r0), shuf.r1, 1);
}

static inline __attribute__((always_inline, used))
void blend_8pixels_soa(int *src, int *dst)
{
	__m256i _src = _mm256_loadu_si256((__m256i *)src);
	__m256i _dst = _mm256_loadu_si256((__m256i *)dst);

	t_vec4f_avx fs = unpack_rgba_bytes_to_floats_avx2_soa(_src);
	t_vec4f_avx fd = unpack_rgba_bytes_to_floats_avx2_soa(_dst);

	__m256 transparency = extract_transparency_avx2_soa(fs);
	t_vec4f_avx blended = blend_pixels_avx2_soa(fs, fd, transparency);

	__m256i out = repack_floats_to_bytes_avx2_soa(blended);
	_mm256_storeu_si256((__m256i *)dst, out);
}

static inline __attribute__((always_inline, used))
t_point	blend_pixels_scalar(int limit_x, t_point it, t_cdata cd)
{
	t_mcol		mc;
	t_colour	src;
	t_colour	dst;
	double		transparency;

	while (it.x < limit_x)
	{
		mc.colour = cd.src[it.x];
		src = *(t_colour *) &mc.colour;
		dst = *(t_colour *) &cd.dst[it.x];
		transparency = src.a / 255.0;
		if (src.raw != dst.raw)
		{
			src.r = lround((dst.r - src.r) * transparency + src.r);
			src.g = lround((dst.g - src.g) * transparency + src.g);
			src.b = lround((dst.b - src.b) * transparency + src.b);
		}
		cd.dst[it.x] = src.raw;
		it.x++;
	}
	return (it);
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
void	place_img_alpha_avx2_soa(t_img *image, t_img *tile, t_point p)
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
			blend_8pixels_soa((cd.src + it.x), (cd.dst + it.x));
			it.x += 8;
		}
		it = blend_pixels_scalar(limit.x, it, cd);
	}
}
