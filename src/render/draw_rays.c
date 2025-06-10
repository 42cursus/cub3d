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

#include "cub3d.h"

static inline __attribute__((always_inline))
t_var3	adjust_values(t_ivect draw_pos, t_lvars line, t_lvect *var)
{
	const int	dy = line.top + draw_pos.y;
	const int	adjust = -dy & dy >> 31;
	const int	row = draw_pos.y - 1 + adjust;
	int			max_rows;
	int			diff;

	var->y = var->x * adjust;
	max_rows = WIN_HEIGHT - (dy + adjust);
	diff = line.height - max_rows;
	max_rows += diff & diff >> 31;
	max_rows += row;
	return ((t_var3){row, dy + adjust, max_rows});
}

static inline __attribute__((always_inline, unused))
void	slice_draw_fixed(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_lvars line)
{
	t_var3		v3;
	t_mcol		mc;
	t_lvect		var;
	u_int32_t	*dst_px;
	t_tex		*texture = ray->tex;
	u_int32_t	*const tex_data = texture->data + ((int)ray->pos * texture->w);

	var.x = ((long)texture->h << FIXED_SHIFT) / line.height;
	v3 = adjust_values(draw_pos, line, &var);
	dst_px = (u_int32_t *) canvas->data + v3.screen_y * canvas->width + draw_pos.x;
	while (++v3.row < v3.max_rows)
	{
		mc.colour = tex_data[(int)(var.y >> FIXED_SHIFT)];
		mc.mask = -(mc.colour != XPM_TRANSPARENT);
		mc.colour |= ((u_int32_t[]) {0, MLX_RED})[ray->damaged];
		*dst_px = (mc.colour & mc.mask) | (*dst_px & ~mc.mask);
		dst_px += canvas->width;
		var.y += var.x;
	}
}

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
void	slice_drawing_sse41(t_ivect pos, t_ray *ray, t_img *cnvs, t_lvars line)
{
	const t_dmask	dm = {line.top + pos.y, (line.top + pos.y) >> 31};
	t_iter			it;
	t_tstep			ts;
	t_m128i			mc;
	t_cdata			cd;

	mc.overlay = ((u_int[]){0, MLX_RED})[ray->damaged];
	cd.src = (int *)ray->tex->data + (ray->tex->w * (int)ray->pos);
	it.i = ((~dm.mask) & (pos.y - 1)) | (dm.mask & (-dm.diff));
	it.j = dm.diff & ~dm.mask;
	ts.step = (double)ray->tex->h / line.height;
	ts.tex_y = ts.step * it.i * (dm.diff < 0);
	cd.dst = (int *)cnvs->data + it.j * cnvs->width + pos.x;
	while (++it.i < line.height && it.j < WIN_HEIGHT)
	{
		mc.colour = cd.src[(int)ts.tex_y];
		mc.tex_vec = _mm_set1_epi32(mc.colour | mc.overlay);
		mc.dst_vec = _mm_set1_epi32(*cd.dst);
		mc.mask = _mm_set1_epi32(-(mc.colour != (int)XPM_TRANSPARENT));
		mc.blend = _mm_blendv_epi8(mc.dst_vec, mc.tex_vec, mc.mask);
		*cd.dst = _mm_cvtsi128_si32(mc.blend);
		cd.dst += cnvs->width;
		ts.tex_y += ts.step;
		it.j = line.top + it.i;
	}
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_img *canvas)
{
	t_anim	*anim;
	t_ivect	pos;
	t_lvars	line;

	pos.x = x;
	pos.y = 0;
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
	slice_drawing_sse41(pos, ray, canvas, line);
	if (ray->in_front != NULL)
		draw_slice(x, ray->in_front, app, canvas);
}

void	draw_rays(t_info *app, t_img *canvas)
{
	t_player	*player;
	t_ray		*rays;
	int			i;

	player = app->player;
	rays = player->rays;
	i = -1;
	while (++i < WIN_WIDTH)
		draw_slice(i, &rays[i], app, canvas);
}
