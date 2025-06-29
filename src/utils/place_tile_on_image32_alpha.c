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
 * Stage 1: Unpack 4 RGBA pixels into 4 __m128 float vectors
 * gdb -batch -ex 'file ./cub3d' -ex 'disassemble /s unpack_rgba_bytes_to_floats'
 * @param pixels
 * @return
 */
static inline __attribute__((always_inline, used))
t_vec4	unpack_rgba_bytes_to_floats(__m128i pixels)
{
	t_vec4			out;
	const __m128i	zero = _mm_setzero_si128();
	const __m128i	lo = _mm_unpacklo_epi8(pixels, zero);
	const __m128i	hi = _mm_unpackhi_epi8(pixels, zero);

	out.r0 = _mm_cvtepi32_ps(_mm_unpacklo_epi16(lo, zero));
	out.r1 = _mm_cvtepi32_ps(_mm_unpackhi_epi16(lo, zero));
	out.r2 = _mm_cvtepi32_ps(_mm_unpacklo_epi16(hi, zero));
	out.r3 = _mm_cvtepi32_ps(_mm_unpackhi_epi16(hi, zero));
	return (out);
}

static inline __attribute__((always_inline, used))
t_vec8f unpack_rgba_bytes_to_floats_avx(__m256i pixels)
{
	t_vec8f         out;
	const __m256i   zero = _mm256_setzero_si256();

	const __m256i lo8 = _mm256_unpacklo_epi8(pixels, zero);
	const __m256i hi8 = _mm256_unpackhi_epi8(pixels, zero);

	out.r0 = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(lo8, zero));
	out.r1 = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(lo8, zero));
	out.r2 = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(hi8, zero));
	out.r3 = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(hi8, zero));

	return out;
}

static inline __attribute__((always_inline, used))
t_vec8f	unpack_rgba_bytes_to_floats_avx2(__m256i pixels)
{
	t_vec8f         out;
	const __m256i   zero = _mm256_setzero_si256();

	const __m256i lo8 = _mm256_unpacklo_epi8(pixels, zero);
	const __m256i hi8 = _mm256_unpackhi_epi8(pixels, zero);

	out.r0 = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(lo8, zero));
	out.r1 = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(lo8, zero));
	out.r2 = _mm256_cvtepi32_ps(_mm256_unpacklo_epi16(hi8, zero));
	out.r3 = _mm256_cvtepi32_ps(_mm256_unpackhi_epi16(hi8, zero));

	return out;
}

/**
 * MLX System (with inverted alpha):
 * 	transparency = alpha / 255
 * 	opacity = 1 - transparency
 * Stage 2: Extract transparency (alpha / 255)
 * @param s
 * @return
 */
static inline __attribute__((always_inline, used))
t_vec4	extract_transparency(t_vec4 fs)
{
	t_vec4 			alpha;
	t_vec4 			transparency;
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

static inline __attribute__((always_inline, used))
t_vec8f	extract_transparency_avx2(t_vec8f fs)
{
	t_vec8f alpha;
	t_vec8f transparency;
	const __m256 byte = _mm256_set1_ps(255.0f);

	alpha.r0 = _mm256_permute_ps(fs.r0, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r1 = _mm256_permute_ps(fs.r1, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r2 = _mm256_permute_ps(fs.r2, _MM_SHUFFLE(3, 3, 3, 3));
	alpha.r3 = _mm256_permute_ps(fs.r3, _MM_SHUFFLE(3, 3, 3, 3));
	transparency.r0 = _mm256_div_ps(alpha.r0, byte);
	transparency.r1 = _mm256_div_ps(alpha.r1, byte);
	transparency.r2 = _mm256_div_ps(alpha.r2, byte);
	transparency.r3 = _mm256_div_ps(alpha.r3, byte);

	return transparency;
}

/**
 * Stage 3: Blend pixel = ((dst - src) * transparency) + src
 * @param src
 * @param dst
 * @param alpha
 * @return
 */
static inline __attribute__((always_inline, used))
t_vec4	blend_pixels(t_vec4 src, t_vec4 dst, t_vec4 transparency)
{
	t_vec4	out;
	t_vec4	diff;

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


static inline __attribute__((always_inline, used))
t_vec8f blend_pixels_avx2(t_vec8f src, t_vec8f dst, t_vec8f transparency)
{
	t_vec8f diff;
	t_vec8f out;

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

/**
 * Stage 4: Convert 4 float vectors to a packed 4-pixel __m128i
 * @param blended
 * @return
 */
static inline __attribute__((always_inline))
__m128i	repack_floats_to_bytes(t_vec4 blended)
{
	t_vec4 i;

	i.r0 = _mm_cvtps_epi32(blended.r0);
	i.r1 = _mm_cvtps_epi32(blended.r1);
	i.r2 = _mm_cvtps_epi32(blended.r2);
	i.r3 = _mm_cvtps_epi32(blended.r3);

	__m128i p01 = _mm_packs_epi32(i.r0, i.r1);
	__m128i p23 = _mm_packs_epi32(i.r2, i.r3);
	return _mm_packus_epi16(p01, p23);
}

static inline __attribute__((always_inline, used))
__m256i repack_floats_to_bytes_avx2(t_vec8f blended)
{
	__m256i i0 = _mm256_cvtps_epi32(blended.r0);
	__m256i i1 = _mm256_cvtps_epi32(blended.r1);
	__m256i i2 = _mm256_cvtps_epi32(blended.r2);
	__m256i i3 = _mm256_cvtps_epi32(blended.r3);

	__m256i p01 = _mm256_packs_epi32(i0, i1); // 16-bit
	__m256i p23 = _mm256_packs_epi32(i2, i3);

	__m256i packed = _mm256_packus_epi16(p01, p23); // 8-bit
	return packed;
}

static inline __attribute__((always_inline, used))
void	blend_4pixels(int *src, int *dst)
{
	const __m128i	_src = _mm_loadu_si128((__m128i *) src);
	const __m128i	_dst = _mm_loadu_si128((__m128i *) dst);

	const t_vec4	fs = unpack_rgba_bytes_to_floats(_src);
	const t_vec4	fd = unpack_rgba_bytes_to_floats(_dst);

	const t_vec4	transparency = extract_transparency(fs);
	const t_vec4	blended = blend_pixels(fs, fd, transparency);

	_mm_storeu_si128((__m128i *)dst, repack_floats_to_bytes(blended));
}

static inline __attribute__((always_inline, used))
void blend_8pixels(int *src, int *dst)
{
	const __m256i _src = _mm256_loadu_si256((__m256i *) src);
	const __m256i _dst = _mm256_loadu_si256((__m256i *) dst);

	t_vec8f fs = unpack_rgba_bytes_to_floats_avx2(_src);
	t_vec8f fd = unpack_rgba_bytes_to_floats_avx2(_dst);

	t_vec8f transparency = extract_transparency_avx2(fs);
	t_vec8f blended = blend_pixels_avx2(fs, fd, transparency);

	_mm256_storeu_si256((__m256i *)dst, repack_floats_to_bytes_avx2(blended));
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
void	place_img_on_image32_alpha_sse41(t_img *image, t_img *tile, t_point p)
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
				src.r = lround((dst.r - src.r) * transparency + src.r);
				src.g = ((dst.g - src.g) * transparency) + src.g + 0.5;
				src.b = ((dst.b - src.b) * transparency) + src.b + 0.5;
			}
			cd.dst[it.x] = src.raw;
			it.x++;
		}
	}
}

inline __attribute__((always_inline, used))
void	place_img_on_image32_alpha_avx2(t_img *image, t_img *tile, t_point p)
{
	t_point	it;
	t_point	offset;
	t_point	limit;
	t_cdata cd;
	t_m256i mc;

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
			const double transparency = col1.a / 255.0;

			out = col1;
			if (col1.raw != col2.raw)
			{
				out.r = ((col2.r - col1.r) * transparency) + col1.r + 0.5;
				out.g = ((col2.g - col1.g) * transparency) + col1.g + 0.5;
				out.b = ((col2.b - col1.b) * transparency) + col1.b + 0.5;
			}
			u_int colour = (out.raw);
			dst_row[j] = colour;
		}
	}
}
