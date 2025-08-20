/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transpose_img_avx2.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 14:27:29 by abelov            #+#    #+#             */
/*   Updated: 2025/08/08 16:19:08 by fsmyth           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

#define TILE 32

inline __attribute__((always_inline, used, externally_visible))
void	transpose8x8_u32_avx2(__m256i *out, const __m256i *i)
{
	const __m256i	v[8] = {
		_mm256_unpacklo_epi32(i[0], i[1]), _mm256_unpackhi_epi32(i[0], i[1]),
		_mm256_unpacklo_epi32(i[2], i[3]), _mm256_unpackhi_epi32(i[2], i[3]),
		_mm256_unpacklo_epi32(i[4], i[5]), _mm256_unpackhi_epi32(i[4], i[5]),
		_mm256_unpacklo_epi32(i[6], i[7]), _mm256_unpackhi_epi32(i[6], i[7])
	};
	const t_vec8i	v2 = {
		_mm256_unpacklo_epi64(v[0], v[2]), _mm256_unpackhi_epi64(v[0], v[2]),
		_mm256_unpacklo_epi64(v[1], v[3]), _mm256_unpackhi_epi64(v[1], v[3]),
		_mm256_unpacklo_epi64(v[4], v[6]), _mm256_unpackhi_epi64(v[4], v[6]),
		_mm256_unpacklo_epi64(v[5], v[7]), _mm256_unpackhi_epi64(v[5], v[7]),
	};

	out[0] = _mm256_permute2x128_si256(v2.t0, v2.t4, 0x20);
	out[1] = _mm256_permute2x128_si256(v2.t1, v2.t5, 0x20);
	out[2] = _mm256_permute2x128_si256(v2.t2, v2.t6, 0x20);
	out[3] = _mm256_permute2x128_si256(v2.t3, v2.t7, 0x20);
	out[4] = _mm256_permute2x128_si256(v2.t0, v2.t4, 0x31);
	out[5] = _mm256_permute2x128_si256(v2.t1, v2.t5, 0x31);
	out[6] = _mm256_permute2x128_si256(v2.t2, v2.t6, 0x31);
	out[7] = _mm256_permute2x128_si256(v2.t3, v2.t7, 0x31);
}

inline __attribute__((always_inline))
int	scalar_loop(const t_cdata cd, const t_tex t, t_ivect max, t_ivect3 it)
{
	while (++it.x < max.x)
	{
		it.z = -1;
		while (++it.z < 8 && (it.y + it.z) < max.y)
			cd.dst[(it.x) * t.w + (it.y + it.z)]
				= cd.src[(it.y + it.z) * t.h + it.x];
	}
	return (it.y + 8);
}

inline __attribute__((always_inline))
t_ivect	main_loop(t_ivect tile, const t_cdata cd, const t_tex t, t_ivect max)
{
	t_ivect3	it;
	__m256i		in[8];
	__m256i		out[8];

	it.y = tile.y;
	while (it.y + 7 < max.y)
	{
		it.x = tile.x;
		while (it.x + 7 < max.x)
		{
			it.z = -1;
			while (++it.z < 8)
				in[it.z] = _mm256_loadu_si256(
						(__m256i_u *)(cd.src + (it.y + it.z) * t.h + it.x));
			transpose8x8_u32_avx2(out, in);
			it.z = -1;
			while (++it.z < 8)
				_mm256_storeu_si256((__m256i_u *)
					(cd.dst + (it.x + it.z) * t.w + it.y), out[it.z]);
			it.x += 8;
		}
		it.x = tile.x + ((max.x - tile.x) & ~7) - 1;
		it.y = scalar_loop(cd, t, max, it);
	}
	return (it.xy);
}

inline __attribute__((always_inline, used, externally_visible))
void	transpose_img_avx2_tiled_read(int *dst, int *src, int width, int height)
{
	t_ivect			t;
	t_ivect			max;
	t_ivect			it;
	const t_cdata	cd = {.src = src, .dst = dst};
	const t_tex		tex = {.w = width, .h = height};

	t.y = 0;
	while (t.y < width)
	{
		t.x = 0;
		while (t.x < height)
		{
			max = (t_ivect){MIN(t.x + TILE, tex.h), MIN(t.y + TILE, tex.w)};
			it = main_loop(t, cd, tex, max);
			it.y = t.y + ((max.y - t.y) & ~7) - 1;
			while (++it.y < max.y)
			{
				it.x = t.x - 1;
				while (++it.x < max.x)
					dst[it.x * width + it.y] = src[it.y * height + it.x];
			}
			t.x += TILE;
		}
		t.y += TILE;
	}
}
