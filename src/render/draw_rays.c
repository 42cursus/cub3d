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

// static inline __attribute__((always_inline, unused))
// void	my_put_pixel_mask(t_img *img, int x, int y, unsigned int colour)
// {
// 	u_int32_t	*dst_pixel;
// 	u_int32_t	mask;
//
// 	dst_pixel = &(*(u_int32_t (*)[img->height][img->width])img->data)[y][x];
// 	mask = -(colour != MLX_TRANSPARENT);
// 	*dst_pixel = (colour & mask) | (*dst_pixel & ~mask);
// }

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
	t_texture	*texture = ray->tex;
	u_int32_t	*const tex_data = texture->data + ((int)ray->pos * texture->x);

	var.x = ((long)texture->y << FIXED_SHIFT) / line.height;
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

static inline __attribute__((always_inline))
void	slice_draw_fixed_old(t_ivect draw_pos, t_ray *ray, t_img *canvas,
							 t_lvars line)
{
	t_mcol			mc;
	t_lvect			src_it;
	t_ivect			it;
	u_int			*dst_px;
	u_int *const	tex_data = ray->tex->data + ((int)ray->pos * ray->tex->x);

	src_it.x = ((long)ray->tex->y << FIXED_SHIFT) / line.height;
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
	u_int	*tex_data = texture->data + (texture->x * (int) ray->pos);
	u_int	*dst_data = (u_int *)canvas->data;

	if (line.top < 0)
		draw_pos.y = 0 - line.top;
	while (draw_pos.y < line.height && draw_pos.y + line.top < WIN_HEIGHT)
	{
		tex_y = ((double) draw_pos.y / line.height) * texture->y;
		mc.colour = tex_data[(int) tex_y];
		mc.mask = -(mc.colour != XPM_TRANSPARENT);
		mc.colour |= ((u_int[]) {0, MLX_RED})[ray->damaged];
		dst_px = &dst_data[(line.top + draw_pos.y) * canvas->width + draw_pos.x];
		*dst_px = (mc.colour & mc.mask) | (*dst_px & ~mc.mask);
		draw_pos.y++;
	}
}

static inline __attribute__((always_inline, unused))
void	slice_drawing_float_v2(t_ivect draw_pos, t_ray *ray, t_img *canvas, t_lvars line)
{
	t_ctex	*texture = ray->tex;
	t_mcol	mc;
	t_point	i;
	u_int	*dst_px;
	u_int	*tex_data = texture->data + (texture->x * (int) ray->pos);

	i.x = draw_pos.y - 1;
	double tex_y = 0;
	int screen_y = line.top + draw_pos.y;
	double step = (double)texture->y / line.height;

	if (screen_y < 0)
	{
		i.x = 0 - screen_y;
		tex_y = step * i.x;
		screen_y = 0;
	}
	dst_px = (u_int *)canvas->data + screen_y * canvas->width + draw_pos.x;
	while (++i.x < line.height && screen_y < WIN_HEIGHT)
	{
		mc.colour = tex_data[(int)tex_y];
		mc.mask = -(mc.colour != XPM_TRANSPARENT);
		mc.colour |= ((u_int[]) {0, MLX_RED})[ray->damaged];
		*dst_px = (mc.colour & mc.mask) | (*dst_px & ~mc.mask);
		dst_px += canvas->width;
		tex_y += step;
		screen_y = line.top + i.x;
	}
}

void	draw_slice(int x, t_ray *ray, t_info *app, t_img *canvas)
{
	t_anim				*anim;
	t_ivect				pos;
	t_lvars				line;
	static t_sldraw_f __attribute__((used)) fns[2] = {&slice_draw_fixed_old,
													  &slice_drawing_float };

	pos.x = x;
	pos.y = 0;
	if (ray->face >= DOOR_N && ray->face < DOOR_N_OPEN)
	{
		anim = &app->map->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			ray->tex = get_close_door_tex(anim, app);
	}
	else if (ray->face >= DOOR_N_OPEN)
	{
		anim = &app->map->anims[ray->maptile.y][ray->maptile.x];
		if (anim->active == 1)
			ray->tex = get_open_door_tex(anim, app);
	}
	line.height = (int)(WIN_WIDTH / (ray->distance * 2.0 * app->fov_opp_len));
	line.top = WIN_HEIGHT / 2 - line.height / 2;
//	if (lvars.lheight > WIN_HEIGHT)
//		slice_drawing(pos, ray, canvas, line);
//	else
//		slice_draw_fixed(pos, ray, canvas, line);
//	fns[line.height > WIN_HEIGHT](pos, ray, canvas, line);
//	slice_draw_fixed_old(pos, ray, canvas, line);
	slice_drawing_float_v2(pos, ray, canvas, line);
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
