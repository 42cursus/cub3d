/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   place_tile_on_image32_alpha.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 20:14:08 by abelov            #+#    #+#             */
/*   Updated: 2025/06/03 20:14:08 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"

/**
 * gdb -batch -ex 'file ./cub3d' -ex 'disassemble /s blend_4pixels'
 * @param src
 * @param dst
 * @return
 */
// Blend src and dst using per-channel alpha
//static inline __attribute__((always_inline, used))
__attribute__((optnone, unused))
static __m128i blend_4pixels_old(__m128i src, __m128i dst)
{
	__m128i zero = _mm_setzero_si128();

	// Unpack bytes to 16-bit integers: [B G R A] × 4
	__m128i src_lo = _mm_unpacklo_epi8(src, zero); // pixels 0 and 1
	__m128i src_hi = _mm_unpackhi_epi8(src, zero); // pixels 2 and 3

	__m128i dst_lo = _mm_unpacklo_epi8(dst, zero);
	__m128i dst_hi = _mm_unpackhi_epi8(dst, zero);

	// Unpack 16-bit to 32-bit per channel
	__m128i s0 = _mm_unpacklo_epi16(src_lo, zero); // pixel 0: B0 G0 R0 A0
	__m128i s1 = _mm_unpackhi_epi16(src_lo, zero); // pixel 1: B1 G1 R1 A1
	__m128i s2 = _mm_unpacklo_epi16(src_hi, zero); // pixel 2: B2 G2 R2 A2
	__m128i s3 = _mm_unpackhi_epi16(src_hi, zero); // pixel 3: B3 G3 R3 A3

	__m128i d0 = _mm_unpacklo_epi16(dst_lo, zero);
	__m128i d1 = _mm_unpackhi_epi16(dst_lo, zero);
	__m128i d2 = _mm_unpacklo_epi16(dst_hi, zero);
	__m128i d3 = _mm_unpackhi_epi16(dst_hi, zero);

	// Convert to float
	__m128 fs0 = _mm_cvtepi32_ps(s0);
	__m128 fs1 = _mm_cvtepi32_ps(s1);
	__m128 fs2 = _mm_cvtepi32_ps(s2);
	__m128 fs3 = _mm_cvtepi32_ps(s3);

	__m128 fd0 = _mm_cvtepi32_ps(d0);
	__m128 fd1 = _mm_cvtepi32_ps(d1);
	__m128 fd2 = _mm_cvtepi32_ps(d2);
	__m128 fd3 = _mm_cvtepi32_ps(d3);

	// Extract alpha from source
	__m128 a0 = _mm_shuffle_ps(fs0, fs0, _MM_SHUFFLE(3, 3, 3, 3));
	__m128 a1 = _mm_shuffle_ps(fs1, fs1, _MM_SHUFFLE(3, 3, 3, 3));
	__m128 a2 = _mm_shuffle_ps(fs2, fs2, _MM_SHUFFLE(3, 3, 3, 3));
	__m128 a3 = _mm_shuffle_ps(fs3, fs3, _MM_SHUFFLE(3, 3, 3, 3));

	__m128 alpha0 = _mm_div_ps(a0, _mm_set1_ps(255.0f));
	__m128 opacity0 = _mm_sub_ps(_mm_set1_ps(1.0f), alpha0);

	__m128 alpha1 = _mm_div_ps(a1, _mm_set1_ps(255.0f));
	__m128 opacity1 = _mm_sub_ps(_mm_set1_ps(1.0f), alpha1);

	__m128 alpha2 = _mm_div_ps(a2, _mm_set1_ps(255.0f));
	__m128 opacity2 = _mm_sub_ps(_mm_set1_ps(1.0f), alpha2);

	__m128 alpha3 = _mm_div_ps(a3, _mm_set1_ps(255.0f));
	__m128 opacity3 = _mm_sub_ps(_mm_set1_ps(1.0f), alpha3);

	// out = src + (dst - src) * (1 - alpha)
	fs0 = _mm_add_ps(fs0, _mm_mul_ps(_mm_sub_ps(fd0, fs0), _mm_sub_ps(_mm_set1_ps(1.0f), opacity0)));
	fs1 = _mm_add_ps(fs1, _mm_mul_ps(_mm_sub_ps(fd1, fs1), _mm_sub_ps(_mm_set1_ps(1.0f), opacity1)));
	fs2 = _mm_add_ps(fs2, _mm_mul_ps(_mm_sub_ps(fd2, fs2), _mm_sub_ps(_mm_set1_ps(1.0f), opacity2)));
	fs3 = _mm_add_ps(fs3, _mm_mul_ps(_mm_sub_ps(fd3, fs3), _mm_sub_ps(_mm_set1_ps(1.0f), opacity3)));

	// Convert back to int
	__m128i i0 = _mm_cvtps_epi32(fs0);
	__m128i i1 = _mm_cvtps_epi32(fs1);
	__m128i i2 = _mm_cvtps_epi32(fs2);
	__m128i i3 = _mm_cvtps_epi32(fs3);

	// Pack 32-bit -> 16-bit
	__m128i p01 = _mm_packs_epi32(i0, i1); // 2 pixels
	__m128i p23 = _mm_packs_epi32(i2, i3); // 2 pixels

	// Pack 16-bit -> 8-bit
	__m128i result = _mm_packus_epi16(p01, p23); // 4 pixels packed in 16 bytes

	return (result);
}

/**
 * Stage 1: Unpack 4 RGBA pixels into 4 __m128 float vectors
 * @param pixels
 * @return
 */
static inline __attribute__((always_inline))
t_vec4	unpack_rgba_bytes_to_floats(__m128i pixels)
{
	__m128i zero = _mm_setzero_si128();
	__m128i lo = _mm_unpacklo_epi8(pixels, zero);
	__m128i hi = _mm_unpackhi_epi8(pixels, zero);

	t_vec4 out;
	out.r0 = _mm_cvtepi32_ps(_mm_unpacklo_epi16(lo, zero));
	out.r1 = _mm_cvtepi32_ps(_mm_unpackhi_epi16(lo, zero));
	out.r2 = _mm_cvtepi32_ps(_mm_unpacklo_epi16(hi, zero));
	out.r3 = _mm_cvtepi32_ps(_mm_unpackhi_epi16(hi, zero));
	return out;
}

/**
 * Stage 2: Extract normalized alpha (alpha / 255)
 * @param s
 * @return
 */
static inline __attribute__((always_inline, unused))
t_vec4 extract_normalized_alpha(t_vec4 s)
{
	t_vec4 alpha;
	t_vec4 opacity;

	alpha.r0 = _mm_div_ps(_mm_shuffle_ps(s.r0, s.r0, _MM_SHUFFLE(3, 3, 3, 3)), _mm_set1_ps(255.0f));
	alpha.r1 = _mm_div_ps(_mm_shuffle_ps(s.r1, s.r1, _MM_SHUFFLE(3, 3, 3, 3)), _mm_set1_ps(255.0f));
	alpha.r2 = _mm_div_ps(_mm_shuffle_ps(s.r2, s.r2, _MM_SHUFFLE(3, 3, 3, 3)), _mm_set1_ps(255.0f));
	alpha.r3 = _mm_div_ps(_mm_shuffle_ps(s.r3, s.r3, _MM_SHUFFLE(3, 3, 3, 3)), _mm_set1_ps(255.0f));

	opacity.r0 = _mm_sub_ps(_mm_set1_ps(1.0f), alpha.r0);
	opacity.r1 = _mm_sub_ps(_mm_set1_ps(1.0f), alpha.r1);
	opacity.r2 = _mm_sub_ps(_mm_set1_ps(1.0f), alpha.r2);
	opacity.r3 = _mm_sub_ps(_mm_set1_ps(1.0f), alpha.r3);

	return opacity;
}

/**
 * Stage 2.1: Extract opacity
 * the alpha already represents opacity
 * opacity = (255.0 - alpha_byte) / 255.0
 * @param s
 * @return
 */
static inline __attribute__((always_inline))
t_vec4 extract_opacity_from_inverted_alpha(t_vec4 s)
{
	t_vec4	opacity;
	__m128	full_byte = _mm_set1_ps(255.0f);

	opacity.r0 = _mm_div_ps(_mm_sub_ps(full_byte, _mm_shuffle_ps(s.r0, s.r0, _MM_SHUFFLE(3, 3, 3, 3))), full_byte );
	opacity.r1 = _mm_div_ps( _mm_sub_ps(full_byte, _mm_shuffle_ps(s.r1, s.r1, _MM_SHUFFLE(3, 3, 3, 3))), full_byte );
	opacity.r2 = _mm_div_ps( _mm_sub_ps(full_byte, _mm_shuffle_ps(s.r2, s.r2, _MM_SHUFFLE(3, 3, 3, 3))), full_byte );
	opacity.r3 = _mm_div_ps( _mm_sub_ps(full_byte, _mm_shuffle_ps(s.r3, s.r3, _MM_SHUFFLE(3, 3, 3, 3))), full_byte );

	return opacity;
}

/**
 * Stage 3: Blend pixel = src + (dst - src) * (1 - alpha)
 * @param src
 * @param dst
 * @param alpha
 * @return
 */
static inline __attribute__((always_inline))
t_vec4 blend_pixels(t_vec4 src, t_vec4 dst, t_vec4 alpha) {
	t_vec4	out;
	__m128	one = _mm_set1_ps(1.0f);

	out.r0 = _mm_add_ps(src.r0, _mm_mul_ps(_mm_sub_ps(dst.r0, src.r0), _mm_sub_ps(one, alpha.r0)));
	out.r1 = _mm_add_ps(src.r1, _mm_mul_ps(_mm_sub_ps(dst.r1, src.r1), _mm_sub_ps(one, alpha.r1)));
	out.r2 = _mm_add_ps(src.r2, _mm_mul_ps(_mm_sub_ps(dst.r2, src.r2), _mm_sub_ps(one, alpha.r2)));
	out.r3 = _mm_add_ps(src.r3, _mm_mul_ps(_mm_sub_ps(dst.r3, src.r3), _mm_sub_ps(one, alpha.r3)));
	return out;
}

/**
 * Stage 4: Convert 4 float vectors to a packed 4-pixel __m128i
 * @param blended
 * @return
 */
static inline __attribute__((always_inline))
__m128i repack_floats_to_bytes(t_vec4 blended)
{
	__m128i i0 = _mm_cvtps_epi32(blended.r0);
	__m128i i1 = _mm_cvtps_epi32(blended.r1);
	__m128i i2 = _mm_cvtps_epi32(blended.r2);
	__m128i i3 = _mm_cvtps_epi32(blended.r3);

	__m128i p01 = _mm_packs_epi32(i0, i1);
	__m128i p23 = _mm_packs_epi32(i2, i3);
	return _mm_packus_epi16(p01, p23);
}

static inline __attribute__((always_inline, used))
__m128i blend_4pixels(__m128i src, __m128i dst)
{
	t_vec4 fs = unpack_rgba_bytes_to_floats(src);
	t_vec4 fd = unpack_rgba_bytes_to_floats(dst);
	t_vec4 alpha = extract_opacity_from_inverted_alpha(fs);
	t_vec4 blended = blend_pixels(fs, fd, alpha);
	return repack_floats_to_bytes(blended);
}

void place_tile_on_image32_alpha(t_img *image, t_img *tile, t_point p)
{
	t_point	it;
	t_point offset;
	t_point limit;

	offset.x = (int[]){0, -p.x}[p.x < 0];
	offset.y = (int[]){0, -p.y}[p.y < 0];

	limit.x = MIN(tile->width, image->width - p.x);
	limit.y = MIN(tile->height, image->height - p.y);

	it.y = offset.y - 1;
	while (++it.y < limit.y)
	{
		u_int32_t *src_row = (u_int32_t *)tile->data + it.y * tile->width;
		u_int32_t *dst_row = (u_int32_t *)image->data + (it.y + p.y) * image->width + p.x;
		it.x = offset.x;
		while (it.x + 3 < limit.x)
		{
			__m128i src = _mm_loadu_si128((__m128i *) &src_row[it.x]);
			__m128i dst = _mm_loadu_si128((__m128i *) &dst_row[it.x]);
			_mm_storeu_si128((__m128i *) &dst_row[it.x], blend_4pixels(src, dst));
			it.x += 4;
		}
	}
}

void	place_tile_on_image32_alpha_old(t_img *image, t_img *tile, t_point p)
{
	t_ivect		t;
	t_ivect		offset;
	t_point		boundaries;
	u_int32_t	*src_row;
	t_colour	*dst_row;

	t_mcol			mc;

	offset.x = (int []){0, -p.x}[p.x < 0];
	offset.y = (int []){0, -p.y}[p.y < 0];

	boundaries.x = MIN(tile->width, image->width - p.x);
	boundaries.y = MIN(tile->height, image->height - p.y);
	t.y = offset.y - 1;
	while (++t.y < boundaries.y)
	{
		src_row = (u_int32_t *) tile->data + (t.y * tile->width);
		dst_row = (t_colour *) image->data + ((t.y + p.y) * image->width) + p.x;
		t.x = offset.x - 1;
		while (++t.x < boundaries.x)
		{
			mc.colour = src_row[t.x];
			t_colour src = *(t_colour *) &mc.colour;
			t_colour dst = dst_row[t.x];
			mc.frac = src.a / 255.0;
			if (src.raw != dst.raw)
			{
				src.r = ((dst.r - src.r) * mc.frac) + src.r + 0.5;
				src.g = ((dst.g - src.g) * mc.frac) + src.g + 0.5;
				src.b = ((dst.b - src.b) * mc.frac) + src.b + 0.5;
			}
			dst_row[t.x] = src;
		}
	}
}

void	pix_copy_alpha(t_img *image, t_img *tile, t_point p)
{
	int			i;
	int			j;
	u_int32_t	*src_row;
	u_int32_t	*dst_row;

	i = -1;
	while (++i < tile->height)
	{
		src_row = (u_int32_t *) tile->data + (i * tile->width);
		dst_row = (u_int32_t *) image->data + ((i + p.y) * image->width) + p.x;
		j = -1;
		while (++j < tile->width)
		{
			t_colour col1 = *(t_colour *) &src_row[j];
			t_colour col2 = *(t_colour *) &dst_row[j];
			t_colour out;
			const double frac = col1.a / 255.0;

			out = col1;
			if (col1.raw != col2.raw)
			{
				out.r = ((col2.r - col1.r) * frac) + col1.r + 0.5;
				out.g = ((col2.g - col1.g) * frac) + col1.g + 0.5;
				out.b = ((col2.b - col1.b) * frac) + col1.b + 0.5;
			}
			u_int colour = (out.raw);
			dst_row[j] = colour;
		}
	}
}
