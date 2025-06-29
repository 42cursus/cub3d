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


#define TILE 64
#define BASE_BLOCK 8
#define MAX_STACK 1024  // enough for large images (log_2N depth)

typedef struct
{
	int	*dst;
	int	*src;
	int	dst_stride;
	int	src_stride;
	int	width;
	int	height;
}	t_transpose_job;

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

static inline __attribute__((always_inline))
void	transpose_scalar_block(int *dst, int *src, int dst_stride, int src_stride, int w, int h)
{
	t_ivect	it;

	it.y = -1;
	while (++it.y < h)
	{
		it.x = -1;
		while (++it.x < w)
			dst[it.y * dst_stride + it.x] = src[it.x * src_stride + it.y];
	}
}

static inline __attribute__((always_inline))
void	transpose_base_case_avx2(int *dst, int *src, int dst_stride, int src_stride)
{
	int i;
	__m256i	in[8];
	__m256i	out[8];

	i = -1;
	while (++i < 8)
		in[i] = _mm256_loadu_si256((__m256i *)(src + i * src_stride));

	transpose8x8_u32_avx2(out, in);

	i = -1;
	while (++i < 8)
		_mm256_storeu_si256((__m256i *)(dst + i * dst_stride), out[i]);
}

/**
 * Cache-oblivious blocking is a key technique in cache-oblivious algorithms
 * that aims to minimize cache misses by structuring data into smaller blocks
 * (tiles) that fit within the cache, without needing explicit knowledge of
 * the cache size.
 *
 * Cache-oblivious algorithms are contrasted with explicit loop tiling
 *
 * @see https://en.wikipedia.org/wiki/Cache-oblivious_algorithm
 * @see https://stackoverflow.com/questions/47554044/
 * @param dst
 * @param src
 * @param width
 * @param height
 */
void transpose_img_stack(int *dst, int *src, int width, int height)
{
	int				sp;
	int				mid;
	t_cdata			cd;
	t_transpose_job	job;
	t_transpose_job	stack[MAX_STACK];

	sp = 0;
	stack[sp++] = (t_transpose_job){dst, src, width, height, width, height};
	while (sp > 0)
	{
		job = stack[--sp];
		if (job.width <= BASE_BLOCK && job.height <= BASE_BLOCK)
		{
			if (job.width == BASE_BLOCK && job.height == BASE_BLOCK)
				transpose_base_case_avx2(job.dst, job.src, job.dst_stride, job.src_stride);
			else
				transpose_scalar_block(job.dst, job.src, job.dst_stride, job.src_stride, job.width, job.height);
		}
		else if (job.width >= job.height)
		{
			mid = job.width / 2;
			cd.src = job.src + mid * job.src_stride;
			cd.dst = job.dst + mid;
			stack[sp++] = (t_transpose_job){
				cd.dst, cd.src, job.dst_stride, job.src_stride,
				job.width - mid, job.height};
			stack[sp++] = (t_transpose_job){
				job.dst, job.src,
				job.dst_stride, job.src_stride,
				mid, job.height};
		}
		else
		{
			mid = job.height / 2;
			cd.src = job.src + mid;
			cd.dst = job.dst + mid * job.dst_stride;
			stack[sp++] = (t_transpose_job){
				cd.dst, cd.src, job.dst_stride, job.src_stride,
				job.width, job.height - mid};
			stack[sp++] = (t_transpose_job){
				job.dst, job.src, job.dst_stride, job.src_stride,
				job.width, mid};
		}
	}
}

/**
 *
 * Loop nest optimization (LNO)
 *
 * @see: https://en.wikipedia.org/wiki/Loop_nest_optimization
 * @param dst
 * @param src
 * @param width
 * @param height
 */
inline __attribute__((always_inline, used))
void transpose_img_avx2_tiled_write(int *dst, int *src, int width, int height)
{
	int		i;
	t_ivect	it;
	t_ivect	tile;
	t_ivect	max;
	__m256i	in[8];
	__m256i	out[8];

	tile.y = 0;
	while (tile.y < height)
	{
		tile.x = 0;
		while (tile.x < width)
		{
			max.y = MIN(tile.y + TILE, height);
			max.x = MIN(tile.x + TILE, width);

			it.y = tile.y;
			while (it.y + 7 < max.y)
			{
				it.x = tile.x;
				while (it.x + 7 < max.x)
				{
					i = -1;
					while (++i < 8)
						in[i] = _mm256_loadu_si256((__m256i *)(src + (it.x + i) * height + it.y));

					transpose8x8_u32_avx2(out, in);

					i = -1;
					while (++i < 8)
						_mm256_storeu_si256((__m256i *)(dst + (it.y + i) * width + it.x), out[i]);
					it.x += 8;
				}
				it.x = tile.x + ((max.x - tile.x) & ~7) - 1;
				while (++it.x < max.x)
				{
					i = -1;
					while (++i < 8 && (it.y + i) < max.y)
						dst[(it.y + i) * width + it.x] = src[it.x * height + it.y + i];
				}
				it.y += 8;
			}
			it.y = tile.y + ((max.y - tile.y) & ~7) - 1;
			while (++it.y < max.y)
			{
				it.x = tile.x - 1;
				while (++it.x < max.x)
					dst[it.y * width + it.x] = src[it.x * height + it.y];
			}
			tile.x += TILE;
		}
		tile.y += TILE;
	}
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
						in[i] = _mm256_loadu_si256((__m256i *)(src + (it.y + i) * height + it.x));

					transpose8x8_u32_avx2(out, in);

					i = -1;
					while (++i < 8)
						_mm256_storeu_si256((__m256i *)(dst + (it.x + i) * width + it.y), out[i]);
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

/**
 * Alternative to https://en.m.wikipedia.org/wiki/In-place_matrix_transposition
 * especially there: #Non-square_matrices%3a_Following_the_cycles
 * @param dst
 * @param src
 * @param width
 * @param height
 */
inline __attribute__((always_inline, used))
void transpose_img_avx2_old(int *dst, int *src, int width, int height)
{
	int		y;
	int		x;
	int		i;
	__m256i	in[8];
	__m256i	out[8];

	int full_width = width & ~7;   // width divisible by 8
	int full_height = height & ~7; // height divisible by 8

	y = 0;
	while (y < full_height)
	{
		x = 0;
		while (x < full_width)
		{
			i = -1;
			while (++i < 8) // Load 8 columns of 8 pixels (column-major input)
				in[i] = _mm256_loadu_si256((__m256i *)(src + (x + i) * height + y));

			transpose8x8_u32_avx2(out, in);

			i = -1;
			while (++i < 8) // Store 8 rows of 8 pixels (row-major output)
				_mm256_storeu_si256((__m256i *)(dst + (y + i) * width + x), out[i]);
			x += 8;
		}
		y += 8;
	}
	y = 0;
	while (y < full_height) 	// Handle leftover columns (right edge)
	{
		x = full_width - 1;
		while (++x < width)
		{
			i = -1;
			while (++i < 8 && (y + i) < height)
				dst[(y + i) * width + x] = src[x * height + y + i];
		}
		y += 8;
	}
	y = full_height - 1;
	while (++y < height) 	// Handle leftover rows (bottom edge)
	{
		x = -1;
		while (++x < width)
			dst[y * width + x] = src[x * height + y];
	}
}
