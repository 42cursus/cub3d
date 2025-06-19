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
void	slice_draw_fixed_old(t_ivect draw_pos, t_ray *ray, t_img *canvas,
							 t_lvars line)
{
	t_mcol			mc;
	t_lvect			src_it;
	t_ivect			it;
	u_int			*dst_px;
	u_int *const	tex_data = ray->tex->data + ((int)ray->pos * ray->tex->w);

	src_it.x = ((long)ray->tex->h << FIXED_SHIFT) / line.height;
	src_it.y = 0;
	it = (t_ivect){.x = line.top + draw_pos.y, .y = draw_pos.y - 1};
	if (it.x < 0)
	{
		it.y = -it.x;
		it.x = 0;
		src_it.y = it.y * src_it.x;
	}
	dst_px = (u_int *)canvas->data + it.x * canvas->width + draw_pos.x;
	while (++it.y < line.height && it.x < WIN_HEIGHT)
	{
		mc.colour = tex_data[(int)(src_it.y >> FIXED_SHIFT)];
		mc.mask = -(mc.colour != XPM_TRANSPARENT);
		mc.colour |= ((u_int[]){0, MLX_RED})[ray->damaged];
		*dst_px = (mc.colour & mc.mask) | (*dst_px & ~mc.mask);
		dst_px += canvas->width;
		src_it.y += src_it.x;
		it.x = line.top + it.y;
	}
}

static inline __attribute__((always_inline, unused))
void	slice_drawing_float(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_lvars line)
{
	t_ctex	*texture = ray->tex;
	double	tex_y;
	t_mcol	mc;
	u_int	*dst_px;
	u_int	*tex_data = texture->data + (texture->w * (int) ray->pos);
	u_int	*dst_data = (u_int *)canvas->data;

	if (line.top < 0)
		draw_pos.y = 0 - line.top;
	while (draw_pos.y < line.height && draw_pos.y + line.top < WIN_HEIGHT)
	{
		tex_y = ((double) draw_pos.y / line.height) * texture->h;
		mc.colour = tex_data[(int) tex_y];
		mc.mask = -(mc.colour != XPM_TRANSPARENT);
		mc.colour |= ((u_int[]) {0, MLX_RED})[ray->damaged];
		dst_px = &dst_data[(line.top + draw_pos.y) * canvas->width + draw_pos.x];
		*dst_px = (mc.colour & mc.mask) | (*dst_px & ~mc.mask);
		draw_pos.y++;
	}
}

/**
 * 	typedef struct s_cdata
 * 	{
 * 		int	*src;
 * 		int	*dst;
 * 	}	t_cdata;
 *
 * 	typedef struct s_m128i
 * 	{
 * 		__m128i	tex_vec;
 * 		__m128i	dst_vec;
 * 		__m128i	mask;
 * 		__m128i	blend;
 * 		struct {
 * 			int colour;
 * 			u_int overlay;
 * 		};
 * 	}	t_m128i;
 *
 * @param pos
 * @param ray
 * @param cnvs
 * @param line
 */
static inline __attribute__((always_inline))
void	slice_drawing_sse41(int x, t_ray *ray, t_tex *cnvs, t_lvars line)
{
	t_iter			it;
	t_tstep			ts;
	t_m128i			mc;
	t_cdata			cd;

	it.i = (-(line.top < 0) & -line.top) - 1;
	it.j = line.end - line.top;

	mc.overlay = -(ray->damaged) & MLX_RED;
	ts.step = (double)ray->tex->h / line.height;

	ts.tex_y = ts.step * it.i;
	cd.src = (int *)ray->tex->data + (ray->tex->w * (int)ray->pos);
//	cd.dst = (int *)cnvs->data + (line.top + it.i) * cnvs->w + x;
	cd.dst = (int *)cnvs->data + (line.top + it.i) + cnvs->w * x;

	while (++it.i < it.j)
	{
		mc.colour = cd.src[(int)ts.tex_y];
		mc.src = _mm_set1_epi32(mc.colour | mc.overlay);

		mc.dst = _mm_set1_epi32(*cd.dst);

		mc.mask = _mm_set1_epi32(-(mc.colour != (int)XPM_TRANSPARENT));
		mc.blend = _mm_blendv_epi8(mc.dst, mc.src, mc.mask);
		*cd.dst = _mm_cvtsi128_si32(mc.blend);

		cd.dst++;
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
	slice_drawing_sse41(x, ray, canvas, line);
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


	static u_int	data[WIN_WIDTH * WIN_HEIGHT];
	static u_int	trans_data[WIN_WIDTH * WIN_HEIGHT];
	const t_tex		img = {.data = data, .h = WIN_HEIGHT, .w = WIN_WIDTH};
	const t_tex		trans = {.data = trans_data, .w = WIN_HEIGHT, .h = WIN_WIDTH};

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
			draw_slice(i, current_ray, app, (t_tex *)&trans);
			current_ray = current_ray->in_front;
		}
	}

//  // Naive approach
//	t_cdata			cd;
//	t_ivect			it;
//
//	it.y = -1;
//	while (++it.y < img.h)
//	{
//		cd.dst = (int *)img.data + it.y * img.w;
//		cd.src = (int *)trans.data + it.y;
//
//		it.x = -1;
//		while (++it.x < img.w)
//			cd.dst[it.x] = cd.src[it.x * img.h];
//	}

	transpose_canvas_avx2(img.data, trans.data, WIN_WIDTH, WIN_HEIGHT);

	place_tile_on_image32_alpha(canvas, (t_tex *)&img, (t_point){0, 0});
}
