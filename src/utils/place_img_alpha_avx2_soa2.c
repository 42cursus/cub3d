/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   place_img_alpha_avx2_soa2.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 23:56:20 by abelov            #+#    #+#             */
/*   Updated: 2025/07/13 23:56:20 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"

/**
 * https://github.com/qingtian805/My-SM4/blob/main/src/sm4avx/sm4avx.c#L301
 * @param pixels
 * @return
 */
inline __attribute__((always_inline, used))
t_rgba_ps256	unpack_rgba_bytes_to_floats_avx2_soa(__m256i pixels)
{
	const __m256i	mask_8 = _mm256_set1_epi32(0xFF);
	t_rgba_si256	in;
	t_rgba_ps256	out;

	in.b = _mm256_and_si256(pixels, mask_8);
	in.g = _mm256_and_si256(_mm256_srli_epi32(pixels, 8), mask_8);
	in.r = _mm256_and_si256(_mm256_srli_epi32(pixels, 16), mask_8);
	in.a = _mm256_and_si256(_mm256_srli_epi32(pixels, 24), mask_8);
	out.b = _mm256_cvtepi32_ps(in.b);
	out.g = _mm256_cvtepi32_ps(in.g);
	out.r = _mm256_cvtepi32_ps(in.r);
	out.a = _mm256_cvtepi32_ps(in.a);
	return (out);
}

inline __attribute__((always_inline, used))
__m256i	repack_floats_to_bytes_avx2_soa(t_rgba_ps256 blended)
{
	t_rgba_si256	rgba;
	t_repack_256	rp;

	rp.zero = _mm256_setzero_si256();
	rp.max255 = _mm256_set1_epi32(255);
	rp.shuffle_mask = _mm256_setr_epi8(
			0, 4, 8, 12, 1, 5, 9, 13,
			2, 6, 10, 14, 3, 7, 11, 15,
			16, 20, 24, 28, 17, 21, 25, 29,
			18, 22, 26, 30, 19, 23, 27, 31);
	rgba.b = _mm256_cvtps_epi32(blended.b);
	rgba.g = _mm256_cvtps_epi32(blended.g);
	rgba.r = _mm256_cvtps_epi32(blended.r);
	rgba.a = _mm256_cvtps_epi32(blended.a);
	rgba.b = _mm256_min_epi32(_mm256_max_epi32(rgba.b, rp.zero), rp.max255);
	rgba.g = _mm256_min_epi32(_mm256_max_epi32(rgba.g, rp.zero), rp.max255);
	rgba.r = _mm256_min_epi32(_mm256_max_epi32(rgba.r, rp.zero), rp.max255);
	rgba.a = _mm256_min_epi32(_mm256_max_epi32(rgba.a, rp.zero), rp.max255);
	rp.bg16 = _mm256_packs_epi32(rgba.b, rgba.g);
	rp.ra16 = _mm256_packs_epi32(rgba.r, rgba.a);
	rp.rgba8 = _mm256_packus_epi16(rp.bg16, rp.ra16);
	rp.shuffled = _mm256_shuffle_epi8(rp.rgba8, rp.shuffle_mask);
	return (rp.shuffled);
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
	return (it);
}

static inline __attribute__((always_inline))
void	blend_8pixels_fast_path_avx2(int *src, int *dst)
{
	t_m256i	mc;
	__m256i	alpha;

	mc.src = _mm256_loadu_si256((__m256i *)src);
	mc.dst = _mm256_loadu_si256((__m256i *)dst);
	mc.transparent = _mm256_set1_epi32(ALPHA_CHANNEL);
	alpha = _mm256_and_si256(mc.src, mc.transparent);
	mc.mask = _mm256_cmpeq_epi32(alpha, _mm256_setzero_si256());
	mc.blend = _mm256_blendv_epi8(mc.dst, mc.src, mc.mask);
	_mm256_storeu_si256((__m256i *)dst, mc.blend);
}

inline __attribute__((always_inline, used))
void	place_img_alpha_avx2_fast_path_soa(t_img *image, t_img *tile, t_point p)
{
	t_point	it;
	t_point	offset;
	t_point	limit;
	t_cdata	cd;

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
