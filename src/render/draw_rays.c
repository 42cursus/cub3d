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

#include <sys/param.h>
#include "cub3d.h"

/**
 * epi32 - packed elements of 32-bit signed integers
 * `e` is not "integer" per se, it implies integer in the context of Intel
 *
 * epi8	Packed 8-bit signed integers
 * epu8	Packed 8-bit unsigned integers
 * epi16	Packed 16-bit signed integers
 * ps	Packed single-precision floats
 * pd	Packed double-precision floats
 *
 * storeu - store unaligned
 *
 * https://www.cs.virginia.edu/~cr4bd/3330/S2023/simdref.html
 * @param x
 * @param ray
 * @param cnvs
 * @param line
 */
static inline __attribute__((always_inline, unused))
void	slice_drawing_avx2x8_strided(int x, t_ray *ray, t_tex *cnvs, t_lvars line)
{
	t_iter		it;
	t_tstep		ts;
	t_cdata		cd;
	t_m256i		mc;
	t_ivect8	dst;
	t_ivect8	ycoord; // for calculating texel Y coordinates
	int			offset = (int)ray->pos * ray->tex->h;
	int			stride = cnvs->w;

	it.i = (-(line.top < 0) & -line.top);
	it.j = line.end - line.top;

	ts.step = (double)ray->tex->h / line.height;
	ts.tex_y = ts.step * it.i;

	cd.src = (int *)ray->tex->data;
	cd.dst = (int *)cnvs->data + x + (line.top + it.i) * stride;

	mc.overlay256 = _mm256_set1_epi32(-(ray->damaged) & MLX_RED);
	mc.transparent = _mm256_set1_epi32(XPM_TRANSPARENT);

	while (it.i + 7 < it.j)
	{
		ycoord.t0 = (int)(ts.tex_y + ts.step * 0);
		ycoord.t1 = (int)(ts.tex_y + ts.step * 1);
		ycoord.t2 = (int)(ts.tex_y + ts.step * 2);
		ycoord.t3 = (int)(ts.tex_y + ts.step * 3);
		ycoord.t4 = (int)(ts.tex_y + ts.step * 4);
		ycoord.t5 = (int)(ts.tex_y + ts.step * 5);
		ycoord.t6 = (int)(ts.tex_y + ts.step * 6);
		ycoord.t7 = (int)(ts.tex_y + ts.step * 7);

		mc.src = _mm256_setr_epi32(
			cd.src[ycoord.t0 + offset],
			cd.src[ycoord.t1 + offset],
			cd.src[ycoord.t2 + offset],
			cd.src[ycoord.t3 + offset],
			cd.src[ycoord.t4 + offset],
			cd.src[ycoord.t5 + offset],
			cd.src[ycoord.t6 + offset],
			cd.src[ycoord.t7 + offset]
		);

		dst.t0 = cd.dst[stride * 0];
		dst.t1 = cd.dst[stride * 1];
		dst.t2 = cd.dst[stride * 2];
		dst.t3 = cd.dst[stride * 3];
		dst.t4 = cd.dst[stride * 4];
		dst.t5 = cd.dst[stride * 5];
		dst.t6 = cd.dst[stride * 6];
		dst.t7 = cd.dst[stride * 7];

		mc.dst = _mm256_setr_epi32(
			dst.t0,
			dst.t1,
			dst.t2,
			dst.t3,
			dst.t4,
			dst.t5,
			dst.t6,
			dst.t7
		);

		__m256i mask = _mm256_cmpeq_epi32(mc.src, mc.transparent);
		mask = _mm256_andnot_si256(mask, _mm256_set1_epi32(-1));

		mc.src = _mm256_or_si256(mc.src, mc.overlay256);

		mc.blend = _mm256_or_si256(
			_mm256_and_si256(mask, mc.src),
			_mm256_andnot_si256(mask, mc.dst)
		);

		_mm256_storeu_si256((__m256i *)&dst, mc.blend);


		cd.dst[stride * 0] = dst.t0;
		cd.dst[stride * 1] = dst.t1;
		cd.dst[stride * 2] = dst.t2;
		cd.dst[stride * 3] = dst.t3;
		cd.dst[stride * 4] = dst.t4;
		cd.dst[stride * 5] = dst.t5;
		cd.dst[stride * 6] = dst.t6;
		cd.dst[stride * 7] = dst.t7;

		cd.dst += 8 * stride;
		ts.tex_y += ts.step * 8;
		it.i += 8;
	}

	// Scalar remainder
	while (it.i < it.j)
	{
		int tex_y = (int)ts.tex_y;
		int color = cd.src[tex_y + offset];
		if (color != (int)XPM_TRANSPARENT)
			*cd.dst = color | (-(ray->damaged) & MLX_RED);
		cd.dst += stride;
		ts.tex_y += ts.step;
		it.i++;
	}
}

static inline __attribute__((always_inline, unused))
void	slice_drawing_avx2x8(int x, t_ray *ray, t_tex *cnvs, t_lvars line)
{
	t_iter		it;
	t_tstep		ts;
	t_cdata		cd;
	t_m256i		mc;
	t_ivect8	ycoord; // for calculating texel Y coordinates
	int			offset = (int)ray->pos * ray->tex->h;

	it.i = (-(line.top < 0) & -line.top);
	it.j = line.end - line.top;

	ts.step = (double)ray->tex->h / line.height;
	ts.tex_y = ts.step * it.i;

	cd.src = (int *)ray->tex->data;
	cd.dst = (int *)cnvs->data + (line.top + it.i) + cnvs->w * x;

	mc.overlay256 = _mm256_set1_epi32(-(ray->damaged) & MLX_RED);
	mc.transparent = _mm256_set1_epi32(XPM_TRANSPARENT);

	while (it.i + 7 < it.j)
	{
		ycoord.t0 = (int)(ts.tex_y + ts.step * 0);
		ycoord.t1 = (int)(ts.tex_y + ts.step * 1);
		ycoord.t2 = (int)(ts.tex_y + ts.step * 2);
		ycoord.t3 = (int)(ts.tex_y + ts.step * 3);
		ycoord.t4 = (int)(ts.tex_y + ts.step * 4);
		ycoord.t5 = (int)(ts.tex_y + ts.step * 5);
		ycoord.t6 = (int)(ts.tex_y + ts.step * 6);
		ycoord.t7 = (int)(ts.tex_y + ts.step * 7);

		mc.src = _mm256_setr_epi32(
			cd.src[ycoord.t0 + offset],
			cd.src[ycoord.t1 + offset],
			cd.src[ycoord.t2 + offset],
			cd.src[ycoord.t3 + offset],
			cd.src[ycoord.t4 + offset],
			cd.src[ycoord.t5 + offset],
			cd.src[ycoord.t6 + offset],
			cd.src[ycoord.t7 + offset]
		);

		mc.mask = _mm256_cmpeq_epi32(mc.src, mc.transparent);
		mc.mask = _mm256_andnot_si256(mc.mask, _mm256_set1_epi32(-1));

		mc.src = _mm256_or_si256(mc.src, mc.overlay256);
		mc.dst = _mm256_loadu_si256((__m256i *)cd.dst);

		mc.blend = _mm256_or_si256(
			_mm256_and_si256(mc.mask, mc.src),
			_mm256_andnot_si256(mc.mask, mc.dst)
		);

		_mm256_storeu_si256((__m256i *)cd.dst, mc.blend);

		cd.dst += 8;
		ts.tex_y += ts.step * 8;
		it.i += 8;
	}

	while (it.i < it.j)
	{
		int tex_y = (int)ts.tex_y;
		int color = cd.src[tex_y + offset];
		if (color != (int)XPM_TRANSPARENT)
			*cd.dst = color | (-(ray->damaged) & MLX_RED);
		cd.dst++;
		ts.tex_y += ts.step;
		it.i++;
	}
}

static inline __attribute__((always_inline, unused))
void	slice_drawing_sse41x4(int x, t_ray *ray, t_tex *cnvs, t_lvars line)
{
	t_iter			it;
	t_tstep			ts;
	t_m128i			mc;
	t_cdata			cd;
	t_ivect4		ycoord; // for calculating texel Y coordinates
	int				offset = (int)ray->pos * ray->tex->h;

	it.i = (-(line.top < 0) & -line.top);
	it.j = line.end - line.top;

	ts.step = (double)ray->tex->h / line.height;
	ts.tex_y = ts.step * it.i;

	cd.src = (int *)ray->tex->data;
	cd.dst = (int *)cnvs->data + (line.top + it.i) + cnvs->w * x;

	mc.overlay128 = _mm_set1_epi32(-(ray->damaged) & MLX_RED);
	mc.transparent = _mm_set1_epi32(XPM_TRANSPARENT);

	while (it.i + 3 < it.j)
	{
		ycoord.t0 = (int)(ts.tex_y + ts.step * 0);
		ycoord.t1 = (int)(ts.tex_y + ts.step * 1);
		ycoord.t2 = (int)(ts.tex_y + ts.step * 2);
		ycoord.t3 = (int)(ts.tex_y + ts.step * 3);

		mc.src = _mm_setr_epi32(
			cd.src[ycoord.t0 + offset],
			cd.src[ycoord.t1 + offset],
			cd.src[ycoord.t2 + offset],
			cd.src[ycoord.t3 + offset]
		);
		mc.mask = _mm_cmpeq_epi32(mc.src, mc.transparent);
		mc.mask = _mm_andnot_si128(mc.mask, _mm_set1_epi32(-1));
		mc.src = _mm_or_si128(mc.src, mc.overlay128);
		mc.dst = _mm_loadu_si128((__m128i *)cd.dst);

		mc.blend = _mm_or_si128(
			_mm_and_si128(mc.mask, mc.src),
			_mm_andnot_si128(mc.mask, mc.dst)
		);
		_mm_storeu_si128((__m128i *)cd.dst, mc.blend);

		cd.dst += 4;
		ts.tex_y += ts.step * 4;
		it.i += 4;
	}
	while (it.i < it.j)
	{
		int tex_y = (int)ts.tex_y;
		int color = cd.src[tex_y + offset];
		if (color != (int)XPM_TRANSPARENT)
			*cd.dst = color | (-(ray->damaged) & MLX_RED);
		cd.dst++;
		ts.tex_y += ts.step;
		it.i++;
	}
}

/**
 * slice_drawing_sse41()
 * @param pos
 * @param ray
 * @param cnvs
 * @param line
 */
static inline __attribute__((always_inline, unused))
void	slice_drawing_sse41(int x, t_ray *ray, t_tex *cnvs, t_lvars line)
{
	t_iter			it;
	t_tstep			ts;
	t_m128i			mc;
	t_cdata			cd;

	it.i = (-(line.top < 0) & -line.top);
	it.j = line.end - line.top;

	mc.overlay = -(ray->damaged) & MLX_RED;
	ts.step = (double)ray->tex->h / line.height;

	ts.tex_y = ts.step * it.i;
	cd.src = (int *)ray->tex->data + (ray->tex->w * (int)ray->pos);
//	cd.dst = (int *)cnvs->data + (line.top + it.i) * cnvs->w + x;
	cd.dst = (int *)cnvs->data + (line.top + it.i) + cnvs->w * x;

	while (it.i < it.j)
	{
		mc.colour = cd.src[(int)ts.tex_y];
		mc.src = _mm_set1_epi32(mc.colour | mc.overlay);
		mc.dst = _mm_set1_epi32(*cd.dst);

		mc.mask = _mm_set1_epi32(-(mc.colour != (int)XPM_TRANSPARENT));
		mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);
		*cd.dst = _mm_cvtsi128_si32(mc.blend);

		cd.dst++;
		it.i++;
//		cd.dst += cnvs->w;
		ts.tex_y += ts.step;
	}
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_tex *canvas)
{
	t_anim	*anim;
	t_lvars	line;

	if (ray->face >= DOOR_N && ray->face < DOOR_N_OPEN)
	{
		anim = &app->lvl->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			ray->tex = get_close_door_tex(anim, app);
	}
	else if (ray->face >= DOOR_N_OPEN)
	{
		anim = &app->lvl->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			ray->tex = get_open_door_tex(anim, app);
	}
	line.height = (int)(WIN_WIDTH / (ray->distance * 2.0 * app->fov_opp_len));
	line.top = WIN_HEIGHT / 2 - line.height / 2;
	line.end = MIN(WIN_HEIGHT / 2 - line.height / 2 + line.height, WIN_HEIGHT);
//	slice_drawing_sse41(x, ray, canvas, line);
//	slice_drawing_sse41x4(x, ray, canvas, line);
	slice_drawing_avx2x8(x, ray, canvas, line);
//	slice_drawing_avx2x8_strided(x, ray, canvas, line);
}

void	draw_rays(t_info *app)
{
	int				i;
	t_ray			*rays;
	t_ray			*current_ray;
	t_img *const	canvas = app->canvas_r;

	const t_tex		trans = {.data = (u_int *) canvas->data, .w = WIN_HEIGHT, .h = WIN_WIDTH};

	rays = app->player->rays;
	i = -1;
	while (++i < WIN_WIDTH)
	{
		current_ray = &rays[i];
		while (current_ray)
		{
			draw_slice(i, current_ray, app, (t_tex *)&trans);
			current_ray = current_ray->in_front;
		}
	}
}
