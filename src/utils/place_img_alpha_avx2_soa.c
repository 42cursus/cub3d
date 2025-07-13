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
#include "cub3d.h"

t_rgba_ps256	unpack_rgba_bytes_to_floats_avx2_soa(__m256i pixels);
__m256i			repack_floats_to_bytes_avx2_soa(t_rgba_ps256 blended);

static inline __attribute__((always_inline, used))
__m256	extract_transparency_avx2_soa(t_rgba_ps256 fs)
{
	__m256			out;
	const __m256	byte = _mm256_set1_ps(255.0f);

	out = _mm256_div_ps(fs.a, byte);
	return (out);
}

static inline __attribute__((always_inline, used))
t_rgba_ps256	blend_pixels_avx2_soa(t_rgba_ps256 src,
					t_rgba_ps256 dst, __m256 transparency)
{
	t_rgba_ps256	diff;
	t_rgba_ps256	out;

	diff.b = _mm256_sub_ps(dst.b, src.b);
	diff.g = _mm256_sub_ps(dst.g, src.g);
	diff.r = _mm256_sub_ps(dst.r, src.r);
	out.b = _mm256_add_ps(src.b, _mm256_mul_ps(diff.b, transparency));
	out.g = _mm256_add_ps(src.g, _mm256_mul_ps(diff.g, transparency));
	out.r = _mm256_add_ps(src.r, _mm256_mul_ps(diff.r, transparency));
	return (out);
}

static inline __attribute__((always_inline, used))
void	blend_8pixels_soa(int *src, int *dst)
{
	t_m256i			mc;
	t_rgba_ps256	fs;
	t_rgba_ps256	fd;
	t_rgba_ps256	blended;
	__m256			transparency;

	mc.src = _mm256_loadu_si256((__m256i *) src);
	mc.dst = _mm256_loadu_si256((__m256i *) dst);
	fs = unpack_rgba_bytes_to_floats_avx2_soa(mc.src);
	fd = unpack_rgba_bytes_to_floats_avx2_soa(mc.dst);
	transparency = extract_transparency_avx2_soa(fs);
	blended = blend_pixels_avx2_soa(fs, fd, transparency);
	mc.blend = repack_floats_to_bytes_avx2_soa(blended);
	_mm256_storeu_si256((__m256i *)dst, mc.blend);
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
			blend_8pixels_soa((cd.src + it.x), (cd.dst + it.x));
			it.x += 8;
		}
		it = blend_pixels_scalar(limit.x, it, cd);
	}
}
