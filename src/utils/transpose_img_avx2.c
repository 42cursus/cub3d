/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   transpose_img_stack_avx2.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abelov <abelov@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 14:27:29 by abelov            #+#    #+#             */
/*   Updated: 2025/06/24 14:27:29 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

#define TILE 32

inline __attribute__((always_inline, used))
void transpose8x8_u32_avx2(__m256i *out, const __m256i *in)
{
	const t_vec8i v1 = {
		_mm256_unpacklo_epi32(in[0], in[1]),
		_mm256_unpackhi_epi32(in[0], in[1]),
		_mm256_unpacklo_epi32(in[2], in[3]),
		_mm256_unpackhi_epi32(in[2], in[3]),
		_mm256_unpacklo_epi32(in[4], in[5]),
		_mm256_unpackhi_epi32(in[4], in[5]),
		_mm256_unpacklo_epi32(in[6], in[7]),
		_mm256_unpackhi_epi32(in[6], in[7])
	};
	const t_vec8i v2 = {
		_mm256_unpacklo_epi64(v1.t0, v1.t2),
		_mm256_unpackhi_epi64(v1.t0, v1.t2),
		_mm256_unpacklo_epi64(v1.t1, v1.t3),
		_mm256_unpackhi_epi64(v1.t1, v1.t3),
		_mm256_unpacklo_epi64(v1.t4, v1.t6),
		_mm256_unpackhi_epi64(v1.t4, v1.t6),
		_mm256_unpacklo_epi64(v1.t5, v1.t7),
		_mm256_unpackhi_epi64(v1.t5, v1.t7),
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

inline __attribute__((always_inline, used))
void transpose_img_avx2_tiled_read(int *dst, int *src, int width, int height)
{
	int		i;
	t_ivect	it;
	t_ivect	tile;
	t_ivect	max;
	__m256i	in[8];
	__m256i	out[8];

	tile.y = 0;
	while (tile.y < width)
	{
		tile.x = 0;
		while (tile.x < height)
		{
			max.y = MIN(tile.y + TILE, width);
			max.x = MIN(tile.x + TILE, height);

			it.y = tile.y;
			while (it.y + 7 < max.y)
			{
				it.x = tile.x;
				while (it.x + 7 < max.x)
				{
					i = -1;
					while (++i < 8)
						in[i] = _mm256_loadu_si256((__m256i_u *)(src + (it.y + i) * height + it.x));

					transpose8x8_u32_avx2(out, in);

					i = -1;
					while (++i < 8)
						_mm256_storeu_si256((__m256i_u *)(dst + (it.x + i) * width + it.y), out[i]);
					it.x += 8;
				}
				it.x = tile.x + ((max.x - tile.x) & ~7) - 1;
				while (++it.x < max.x)
				{
					i = -1;
					while (++i < 8 && (it.y + i) < max.y)
						dst[(it.x) * width + (it.y + i)] = src[(it.y + i) * height + it.x];
				}
				it.y += 8;
			}
			it.y = tile.y + ((max.y - tile.y) & ~7) - 1;
			while (++it.y < max.y)
			{
				it.x = tile.x - 1;
				while (++it.x < max.x)
					dst[it.x * width + it.y] = src[it.y * height + it.x];
			}
			tile.x += TILE;
		}
		tile.y += TILE;
	}
}
