/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_rays.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:18:41 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/19 17:42:55 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <immintrin.h>
#include <sys/param.h>
#include "cub3d.h"

/**
 * epi32 - packed elements of 32-bit signed integers
 * `e` is not "integer" per se, it implies integer in the context of Intel
 *
 * epi8		Packed 8-bit signed integers
 * epu8		Packed 8-bit unsigned integers
 * epi16	Packed 16-bit signed integers
 * ps		Packed single-precision floats
 * pd		Packed double-precision floats
 *
 * storeu - store unaligned
 * loadu - load unaligned
 *
 * https://www.cs.virginia.edu/~cr4bd/3330/S2023/simdref.html
 * @param x
 * @param ray
 * @param cnvs
 * @param line
 */
static inline __attribute__((always_inline))
void	slice_drawing_scalar(const t_ray *ray, t_iter it, t_ftstep ts, t_cdata cd)
{
	t_m128i		mc;

	mc.transparent = _mm_set1_epi32(XPM_TRANSPARENT);
	mc.overlay = -(ray->damaged) & MLX_RED;
	while (it.i < it.j)
	{
		mc.colour = cd.src[(int) ts.tex_y];
		mc.src = _mm_set1_epi32(mc.colour | mc.overlay);
		mc.dst = _mm_set1_epi32(*cd.dst);
//		mc.mask = _mm_cmpeq_epi32(mc.src, mc.transparent);
		mc.mask = _mm_set1_epi32(-(mc.colour != (int)XPM_TRANSPARENT));
		mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);
		*cd.dst = _mm_cvtsi128_si32(mc.blend);
		cd.dst++;
		ts.tex_y += ts.step;
		it.i++;
	}
}

/**
 * instead of `4_mm256_i32gather_epi32((const int *)cd.src, fma.indices, 4);`
 * we can use manual gather, which is more performant:
 * 		_mm256_store_si256((__m256i *)fma.idx, fma.indices);
 * 		mmc.src = _mm256_setr_epi32(
 * 				cd.src[fma.idx[0]],
 * 				cd.src[fma.idx[1]],
 * 				cd.src[fma.idx[2]],
 * 				cd.src[fma.idx[3]],
 * 				cd.src[fma.idx[4]],
 * 				cd.src[fma.idx[5]],
 * 				cd.src[fma.idx[6]],
 * 				cd.src[fma.idx[7]]);
 * @param x
 * @param ray
 * @param cnvs
 * @param line
 */
static inline __attribute__((always_inline))
void	slice_drawing_avx2x8(int x, t_ray *ray, t_tex *cnvs, t_lvars line)
{
	t_iter		it;
	t_ftstep	ts;
	t_m256i		mmc;
	t_cdata		cd;
	t_fma_avx2	fma;

	it.i = (-(line.top < 0) & -line.top);
	it.j = line.end - line.top;
	ts.step = (float)ray->tex->h / line.height;
	ts.tex_y = ts.step * it.i;

	fma.offsets = _mm256_setr_ps(0, 1, 2, 3, 4, 5, 6, 7);
	fma.step = _mm256_set1_ps(ts.step);
	cd.src = (int *)ray->tex->data + (int) ray->pos * ray->tex->h;
	cd.dst = (int *)cnvs->data + (line.top + it.i) + cnvs->w * x;
	mmc.overlay256 = _mm256_set1_epi32(-(ray->damaged) & MLX_RED);
	mmc.transparent = _mm256_set1_epi32(XPM_TRANSPARENT);
//	while (it.i + 7 < it.j)
//	{
//		fma.indices = _mm256_cvttps_epi32(_mm256_fmadd_ps(fma.offsets, fma.step,
//			_mm256_set1_ps(ts.tex_y)));
//		mmc.src = _mm256_i32gather_epi32((const int *)cd.src, fma.indices, 4);
//		mmc.mask = _mm256_cmpeq_epi32(mmc.src, mmc.transparent);
//		mmc.src = _mm256_or_si256(mmc.src, mmc.overlay256);
//		mmc.dst = _mm256_loadu_si256((__m256i *)cd.dst);
//		mmc.blend = _mm256_blendv_epi8(mmc.src, mmc.dst, mmc.mask);
//		_mm256_storeu_si256((__m256i *)cd.dst, mmc.blend);
//		cd.dst += 8;
//		ts.tex_y += ts.step * 8;
//		it.i += 8;
//	}
	slice_drawing_scalar(ray, it, ts, cd);
}

void	draw_slice_transposed(int x, t_ray *ray, t_info *app, t_tex *canvas)
{
	t_lvars	line;

	line.height = (int)(WIN_WIDTH / (ray->distance * 2.0 * app->fov_opp_len));
	line.top = WIN_HEIGHT / 2 - line.height / 2;
	line.end = MIN(WIN_HEIGHT / 2 - line.height / 2 + line.height, WIN_HEIGHT);
	slice_drawing_avx2x8(x, ray, canvas, line);
}

void	draw_rays_transposed(t_info *app)
{
	int				i;
	t_ray			*rays;
	t_ray			*current_ray;
	t_img *const	canvas = app->canvas_r;
	t_tex			trans;

	trans.w = WIN_HEIGHT;
	trans.h = WIN_WIDTH;
	trans.data = (u_int *)canvas->data;
	rays = app->player->rays;
	i = -1;
	while (++i < WIN_WIDTH)
	{
		current_ray = &rays[i];
		while (current_ray)
		{
			draw_slice_transposed(i, current_ray, app, (t_tex *)&trans);
			current_ray = current_ray->in_front;
		}
	}
}
