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
static inline __attribute__((always_inline, unused))
void	slice_drawing_avx2x8_strided(int x, t_ray *ray, t_tex *cnvs, t_lvars line)
{
	t_iter		it;
	t_tstep		ts;
	t_cdata		cd;
	t_m256i		mc;
	t_ivect8	dst;
	int			stride = cnvs->w;

	it.i = (-(line.top < 0) & -line.top);
	it.j = line.end - line.top;

	ts.step = (double)ray->tex->h / line.height;
	ts.tex_y = ts.step * it.i;

	cd.src = (int *)ray->tex->data + (int)ray->pos * ray->tex->h;
	cd.dst = (int *)cnvs->data + x + (line.top + it.i) * stride;

	mc.overlay256 = _mm256_set1_epi32(-(ray->damaged) & MLX_RED);
	mc.transparent = _mm256_set1_epi32(XPM_TRANSPARENT);

	while (it.i + 7 < it.j)
	{
		mc.src = _mm256_setr_epi32(
			cd.src[(int)(ts.tex_y + ts.step * 0)],
			cd.src[(int)(ts.tex_y + ts.step * 1)],
			cd.src[(int)(ts.tex_y + ts.step * 2)],
			cd.src[(int)(ts.tex_y + ts.step * 3)],
			cd.src[(int)(ts.tex_y + ts.step * 4)],
			cd.src[(int)(ts.tex_y + ts.step * 5)],
			cd.src[(int)(ts.tex_y + ts.step * 6)],
			cd.src[(int)(ts.tex_y + ts.step * 7)]
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
	while (it.i < it.j)
	{
		int color = cd.src[((int)ts.tex_y)];
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
	t_m256i		mmc;

	it.i = (-(line.top < 0) & -line.top);
	it.j = line.end - line.top;

	ts.step = (double)ray->tex->h / line.height;
	ts.tex_y = ts.step * it.i;

	cd.src = (int *)ray->tex->data + (int) ray->pos * ray->tex->h;
	cd.dst = (int *)cnvs->data + (line.top + it.i) + cnvs->w * x;

	mmc.overlay256 = _mm256_set1_epi32(-(ray->damaged) & MLX_RED);
	mmc.transparent = _mm256_set1_epi32(XPM_TRANSPARENT);

	while (it.i + 7 < it.j)
	{
		mmc.src = _mm256_setr_epi32(
			cd.src[(int)(ts.tex_y + ts.step * 0)],
			cd.src[(int)(ts.tex_y + ts.step * 1)],
			cd.src[(int)(ts.tex_y + ts.step * 2)],
			cd.src[(int)(ts.tex_y + ts.step * 3)],
			cd.src[(int)(ts.tex_y + ts.step * 4)],
			cd.src[(int)(ts.tex_y + ts.step * 5)],
			cd.src[(int)(ts.tex_y + ts.step * 6)],
			cd.src[(int)(ts.tex_y + ts.step * 7)]
		);

		mmc.mask = _mm256_cmpeq_epi32(mmc.src, mmc.transparent);
		mmc.mask = _mm256_andnot_si256(mmc.mask, _mm256_set1_epi32(-1));

		mmc.src = _mm256_or_si256(mmc.src, mmc.overlay256);
		mmc.dst = _mm256_loadu_si256((__m256i *)cd.dst);

		mmc.blend = _mm256_or_si256(
			_mm256_and_si256(mmc.mask, mmc.src),
			_mm256_andnot_si256(mmc.mask, mmc.dst)
		);

		_mm256_storeu_si256((__m256i *)cd.dst, mmc.blend);

		cd.dst += 8;
		ts.tex_y += ts.step * 8;
		it.i += 8;
	}
	t_m128i			mc;
	mc.overlay = -(ray->damaged) & MLX_RED;
	while (it.i < it.j)
	{
		mc.colour = cd.src[(int)ts.tex_y];
		mc.src = _mm_set1_epi32(mc.colour | mc.overlay);
		mc.dst = _mm_set1_epi32(*cd.dst);
		mc.mask = _mm_set1_epi32(-(mc.colour != (int)XPM_TRANSPARENT));
		mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);
		*cd.dst = _mm_cvtsi128_si32(mc.blend);
		cd.dst++;
		ts.tex_y += ts.step;
		it.i++;
	}
}

static inline __attribute__((always_inline, unused))
void	slice_drawing_sse41x4(int x, t_ray *ray, t_tex *cnvs, t_lvars line)
{
	t_iter			it;
	t_tfstep		ts;
	t_m128i			mc;
	t_cdata			cd;
	int				offset = (int)ray->pos * ray->tex->h;

	it.i = (-(line.top < 0) & -line.top);
	it.j = line.end - line.top;

	ts.step = (float)ray->tex->h / line.height;
	ts.tex_y = ts.step * it.i;

	cd.src = (int *)ray->tex->data + offset;
	cd.dst = (int *)cnvs->data + (line.top + it.i) + cnvs->w * x;

	mc.overlay = -(ray->damaged) & MLX_RED;
	mc.overlay128 = _mm_set1_epi32(-(ray->damaged) & MLX_RED);
	mc.transparent = _mm_set1_epi32(XPM_TRANSPARENT);

//	__m128 idx_f = _mm_set_ps(3.0f, 2.0f, 1.0f, 0.0f);
//	__m128 addon = _mm_mul_ps(_mm_set1_ps(ts.step), idx_f);
	while (it.i + 3 < it.j)
	{
//		__m128i i_x = _mm_cvtps_epi32(_mm_add_ps(_mm_set1_ps(ts.tex_y), addon));
//		mc.src = _mm_i32gather_epi32((const int *)cd.src, i_x, sizeof(int));

		mc.src = _mm_setr_epi32(
			cd.src[(int)(ts.tex_y + ts.step * 0)],
			cd.src[(int)(ts.tex_y + ts.step * 1)],
			cd.src[(int)(ts.tex_y + ts.step * 2)],
			cd.src[(int)(ts.tex_y + ts.step * 3)]
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
		mc.colour = cd.src[(int) ts.tex_y];
		mc.src = _mm_set1_epi32(mc.colour | mc.overlay);
		mc.dst = _mm_set1_epi32(*cd.dst);
		mc.mask = _mm_set1_epi32(-(mc.colour != (int)XPM_TRANSPARENT));
		mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);
		*cd.dst = _mm_cvtsi128_si32(mc.blend);
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
	cd.dst = (int *)cnvs->data + (line.top + it.i) * cnvs->w + x;
//	cd.dst = (int *)cnvs->data + (line.top + it.i) + cnvs->w * x;

	while (it.i < it.j)
	{
		mc.colour = cd.src[(int)ts.tex_y];

		mc.src = _mm_set1_epi32(mc.colour | mc.overlay);

		mc.dst = _mm_set1_epi32(*cd.dst);

		mc.mask = _mm_set1_epi32(-(mc.colour != (int)XPM_TRANSPARENT));
		mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);
		*cd.dst = _mm_cvtsi128_si32(mc.blend);

		it.i++;
//		cd.dst++;
		cd.dst += cnvs->w;
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
	slice_drawing_sse41(x, ray, canvas, line);
//	slice_drawing_sse41x4(x, ray, canvas, line);
//	slice_drawing_avx2x8(x, ray, canvas, line);
//	slice_drawing_avx2x8_strided(x, ray, canvas, line);
}

void	draw_slice_transposed(int x, t_ray *ray, t_info *app, t_tex *canvas)
{
	t_anim	*anim;
	t_lvars	line;

	bool closed = ray->face >= DOOR_N && ray->face < DOOR_N_OPEN;
	bool open = ray->face >= DOOR_N_OPEN;

	if ((open || closed))
	{
		anim = &app->lvl->anims[ray->maptile.y][ray->maptile.x];
		if(anim->active)
		{
			if (closed)
				ray->tex = get_close_door_tex(anim, app);
			else
				ray->tex = get_open_door_tex(anim, app);
		}
	}
	line.height = (int)(WIN_WIDTH / (ray->distance * 2.0 * app->fov_opp_len));
	line.top = WIN_HEIGHT / 2 - line.height / 2;
	line.end = MIN(WIN_HEIGHT / 2 - line.height / 2 + line.height, WIN_HEIGHT);
	slice_drawing_avx2x8(x, ray, canvas, line);
//	slice_drawing_sse41x4(x, ray, canvas, line);
}

void	draw_rays(t_info *app)
{
	int				i;
	t_ray			*rays;
	t_ray			*current_ray;
	t_img *const	canvas = app->canvas;

	const t_tex		tex = {.data = (u_int *) canvas->data, .w = WIN_WIDTH, .h = WIN_HEIGHT};

	rays = app->player->rays;
	i = -1;
	while (++i < WIN_WIDTH)
	{
		current_ray = &rays[i];
		while (current_ray)
		{
			draw_slice(i, current_ray, app, (t_tex *)&tex);
			current_ray = current_ray->in_front;
		}
	}
}

void	draw_rays_transposed_alt(t_info *app)
{
	int				i;
	t_ray			*rays;
	t_ray			*current_ray;
	t_img *const	canvas = app->canvas_r;

	u_int	trans_data[WIN_WIDTH * WIN_HEIGHT];
	const t_tex		trans = {.data = trans_data, .w = WIN_HEIGHT, .h = WIN_WIDTH};
	const t_img source = {.data = canvas->data, .width = WIN_WIDTH, .height = WIN_HEIGHT};

	i = -1;
	while (++i < WIN_WIDTH * WIN_HEIGHT)
		trans_data[i] = XPM_TRANSPARENT;

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
	transpose_img_avx2_tiled_write((int *) canvas->data, (int *)trans.data, WIN_WIDTH, WIN_HEIGHT);

	t_point p = {0, 0};
	place_img_alpha_avx2_soa(app->canvas, (t_img *)&source, p);
}

void	draw_rays_transposed(t_info *app)
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
			draw_slice_transposed(i, current_ray, app, (t_tex *)&trans);
			current_ray = current_ray->in_front;
		}
	}
}
