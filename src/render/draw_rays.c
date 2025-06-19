/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_rays.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fsmyth <fsmyth@student.42london.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 17:18:41 by fsmyth            #+#    #+#             */
/*   Updated: 2025/06/02 17:30:19 by abelov           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/param.h>
#include "cub3d.h"

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
}

static inline void transpose8x8_u32_avx2(__m256i *out, const __m256i *in)
{
	// Step 1: unpack 32-bit values into 64-bit lanes
	const t_vec8 v1 = {
		_mm256_unpacklo_epi32(in[0], in[1]),
		_mm256_unpackhi_epi32(in[0], in[1]),
		_mm256_unpacklo_epi32(in[2], in[3]),
		_mm256_unpackhi_epi32(in[2], in[3]),
		_mm256_unpacklo_epi32(in[4], in[5]),
		_mm256_unpackhi_epi32(in[4], in[5]),
		_mm256_unpacklo_epi32(in[6], in[7]),
		_mm256_unpackhi_epi32(in[6], in[7])
	};

	// Step 2: combine 64-bit chunks
	const t_vec8 v2 = {
		_mm256_unpacklo_epi64(v1.t0, v1.t2),
		_mm256_unpackhi_epi64(v1.t0, v1.t2),
		_mm256_unpacklo_epi64(v1.t1, v1.t3),
		_mm256_unpackhi_epi64(v1.t1, v1.t3),
		_mm256_unpacklo_epi64(v1.t4, v1.t6),
		_mm256_unpackhi_epi64(v1.t4, v1.t6),
		_mm256_unpacklo_epi64(v1.t5, v1.t7),
		_mm256_unpackhi_epi64(v1.t5, v1.t7),
	};

	// Step 3: final blend across 128-bit lanes
	out[0] = _mm256_permute2x128_si256(v2.t0, v2.t4, 0x20);
	out[1] = _mm256_permute2x128_si256(v2.t1, v2.t5, 0x20);
	out[2] = _mm256_permute2x128_si256(v2.t2, v2.t6, 0x20);
	out[3] = _mm256_permute2x128_si256(v2.t3, v2.t7, 0x20);
	out[4] = _mm256_permute2x128_si256(v2.t0, v2.t4, 0x31);
	out[5] = _mm256_permute2x128_si256(v2.t1, v2.t5, 0x31);
	out[6] = _mm256_permute2x128_si256(v2.t2, v2.t6, 0x31);
	out[7] = _mm256_permute2x128_si256(v2.t3, v2.t7, 0x31);
}

void transpose_canvas_avx2(u_int *dst, u_int *src, int width, int height)
{
	int		y;
	int		x;
	int		i;
	__m256i	in[8];
	__m256i	out[8];

	y = 0;
	while (y < height)
	{
		x = 0;
		while (x < width)
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
}

void	draw_rays(t_info *app)
{
	int				i;
	t_ray			*rays;
	t_ray			*current_ray;
	t_img *const	canvas = app->canvas;

	u_int	trans_data[WIN_WIDTH * WIN_HEIGHT];
	const t_tex		trans = {.data = trans_data, .w = WIN_HEIGHT, .h = WIN_WIDTH};

	transpose_canvas_avx2(trans.data, (u_int *) canvas->data, WIN_HEIGHT, WIN_WIDTH);

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

	transpose_canvas_avx2((u_int *) canvas->data, trans.data, WIN_WIDTH, WIN_HEIGHT);
}
