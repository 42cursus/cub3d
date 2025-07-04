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
t_rgba_ps256	unpack_rgba_bytes_to_floats_avx2_soa(__m256i pixels)
{
	const __m256i mask_8 = _mm256_set1_epi32(0xFF);

	t_rgba_ps256 in;

	in.b = _mm256_and_si256(pixels, mask_8);
	in.g = _mm256_and_si256(_mm256_srli_epi32(pixels, 8), mask_8);
	in.r = _mm256_and_si256(_mm256_srli_epi32(pixels, 16), mask_8);
	in.a = _mm256_and_si256(_mm256_srli_epi32(pixels, 24), mask_8);

	t_rgba_ps256 out;
	out.b = _mm256_cvtepi32_ps(in.b);
	out.g = _mm256_cvtepi32_ps(in.g);
	out.r = _mm256_cvtepi32_ps(in.r);
	out.a = _mm256_cvtepi32_ps(in.a);
	return out;
}

static inline __attribute__((always_inline, used))
__m256 extract_transparency_avx2_soa(t_rgba_ps256 fs)
{
	__m256 out;
	const __m256 byte = _mm256_set1_ps(255.0f);

	out = _mm256_div_ps(fs.a, byte);

	return (out);
}

static inline __attribute__((always_inline, used))
t_rgba_ps256 blend_pixels_avx2_soa(t_rgba_ps256 src, t_rgba_ps256 dst, __m256 transparency)
{
	t_rgba_ps256 diff;
	t_rgba_ps256 out;

	diff.b = _mm256_sub_ps(dst.b, src.b);
	diff.g = _mm256_sub_ps(dst.g, src.g);
	diff.r = _mm256_sub_ps(dst.r, src.r);

	out.b = _mm256_add_ps(src.b, _mm256_mul_ps(diff.b, transparency));
	out.g = _mm256_add_ps(src.g, _mm256_mul_ps(diff.g, transparency));
	out.r = _mm256_add_ps(src.r, _mm256_mul_ps(diff.r, transparency));

	return out;
}

static inline __attribute__((always_inline, used))
__m256i repack_floats_to_bytes_avx2_soa(t_rgba_ps256 blended)
{
	t_rgba_ps256 rgba;

	const __m256i zero = _mm256_setzero_si256();
	const __m256i max255 = _mm256_set1_epi32(255);
	const __m256i shuffle_mask = _mm256_setr_epi8(
		0,  4,  8, 12,
		1,  5,  9, 13,
		2,  6, 10, 14,
		3,  7, 11, 15,

		16, 20, 24, 28,
		17, 21, 25, 29,
		18, 22, 26, 30,
		19, 23, 27, 31
	);

	rgba.b = _mm256_cvtps_epi32(blended.b);
	rgba.g = _mm256_cvtps_epi32(blended.g);
	rgba.r = _mm256_cvtps_epi32(blended.r);
	rgba.a = _mm256_cvtps_epi32(blended.a);

	rgba.b = _mm256_min_epi32(_mm256_max_epi32(rgba.b, zero), max255);
	rgba.g = _mm256_min_epi32(_mm256_max_epi32(rgba.g, zero), max255);
	rgba.r = _mm256_min_epi32(_mm256_max_epi32(rgba.r, zero), max255);
	rgba.a = _mm256_min_epi32(_mm256_max_epi32(rgba.a, zero), max255);

	__m256i bg16 = _mm256_packs_epi32(rgba.b, rgba.g);
	__m256i ra16 = _mm256_packs_epi32(rgba.r, rgba.a);
	__m256i rgba8 = _mm256_packus_epi16(bg16, ra16);

	__m256i shuffled = _mm256_shuffle_epi8(rgba8, shuffle_mask);

	return shuffled;
}

static inline __attribute__((always_inline, used))
void blend_8pixels_soa(int *src, int *dst)
{
	__m256i _src = _mm256_loadu_si256((__m256i *)src);
	__m256i _dst = _mm256_loadu_si256((__m256i *)dst);

	t_rgba_ps256 fs = unpack_rgba_bytes_to_floats_avx2_soa(_src);
	t_rgba_ps256 fd = unpack_rgba_bytes_to_floats_avx2_soa(_dst);

	__m256 transparency = extract_transparency_avx2_soa(fs);
	t_rgba_ps256 blended = blend_pixels_avx2_soa(fs, fd, transparency);

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
 * 	alpha = 0 => fully transparent
 *  alpha = 255 => fully opaque
 *
 * 	alpha = 255 * opacity
 * 	alpha = 255 * (1 - transparency)
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
 *  alpha = 0 => fully opaque
 *  alpha = 255 => fully transparent
 *
 * 	alpha = 255 * transparency
 * 	alpha = 255 * (1 - opacity)
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

static inline __attribute__((always_inline, used))
t_point	blend_pixels_scalar_fast_path(int limit_x, t_point it, t_cdata cd)
{
	uint32_t	mask;
	uint32_t	src_pixel;
	uint32_t	dst_pixel;

	while (it.x < limit_x)
	{
		src_pixel = (uint32_t)cd.src[it.x];
		dst_pixel = (uint32_t)cd.dst[it.x];
		mask = -(((src_pixel >> 24) & 0xFF) == 0);
		cd.dst[it.x] = (int)((src_pixel & mask) | (dst_pixel & ~mask));
		it.x++;
	}
	return it;
}

static inline __attribute__((always_inline, unused))
void	blend_8pixels_fast_path_avx2(int *src, int *dst)
{
	const __m256i v_src = _mm256_loadu_si256((__m256i *)src);
	const __m256i v_dst = _mm256_loadu_si256((__m256i *)dst);

	const __m256i alpha_mask = _mm256_set1_epi32(ALPHA_CHANNEL);
	const __m256i alpha = _mm256_and_si256(v_src, alpha_mask);

	const __m256i zero_alpha = _mm256_setzero_si256();
//	const __m256i full_alpha = _mm256_set1_epi32(XPM_TRANSPARENT);
//	__m256i mask_alpha = _mm256_cmpeq_epi32(alpha, full_alpha);
	__m256i mask_alpha = _mm256_cmpeq_epi32(alpha, zero_alpha);
	__m256i result = _mm256_blendv_epi8(v_dst, v_src, mask_alpha);

	_mm256_storeu_si256((__m256i *)dst, result);
}

inline __attribute__((always_inline, used))
void	place_img_alpha_avx2_fast_path_soa(t_img *image, t_img *tile, t_point p)
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
			blend_8pixels_fast_path_avx2((cd.src + it.x), (cd.dst + it.x));
			it.x += 8;
		}
		it = blend_pixels_scalar_fast_path(limit.x, it, cd);
	}
}
